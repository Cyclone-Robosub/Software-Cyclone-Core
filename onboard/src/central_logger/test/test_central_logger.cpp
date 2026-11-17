#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <regex>
#include <string>
#include <rclcpp/rclcpp.hpp>

#include "central_logger.hpp"
#include "log_publisher.hpp"

using custom_interfaces::msg::Loginfo;
using namespace std::chrono_literals;

class TestCentralLogger : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // central_logger writes under $HOME, so point HOME at a temp directory
        const char* home = std::getenv("HOME");
        old_home = home ? std::optional<std::string>(home) : std::nullopt;

        std::string dir = (std::filesystem::temp_directory_path() / "central_logger_test_XXXXXX").string();
        ASSERT_NE(mkdtemp(dir.data()), nullptr);
        temp_home = dir;
        setenv("HOME", temp_home.c_str(), 1);

        node = std::make_shared<rclcpp::Node>("test_node");
        executor.add_node(node);
    }

    void TearDown() override
    {
        if (old_home)
        {
            setenv("HOME", old_home->c_str(), 1);
        }
        else
        {
            unsetenv("HOME");
        }
        std::filesystem::remove_all(temp_home);
    }

    // Spin until done() returns true, or give up after the timeout
    bool spin_until(const std::function<bool()>& done, std::chrono::milliseconds timeout = 2s)
    {
        const auto deadline = std::chrono::steady_clock::now() + timeout;
        while (!done())
        {
            if (std::chrono::steady_clock::now() > deadline)
            {
                return false;
            }
            executor.spin_once(10ms);
        }
        return true;
    }

    std::optional<std::string> old_home;
    std::filesystem::path temp_home;
    rclcpp::Node::SharedPtr node;
    rclcpp::executors::SingleThreadedExecutor executor;
};

TEST_F(TestCentralLogger, PublisherRejectsInvalidNames)
{
    EXPECT_THROW(log_publisher(*node, ""), std::invalid_argument);
    EXPECT_THROW(log_publisher(*node, std::string(33, 'a')), std::invalid_argument);
    EXPECT_THROW(log_publisher(*node, "a/b"), std::invalid_argument);
    EXPECT_THROW(log_publisher(*node, "../a"), std::invalid_argument);
    EXPECT_NO_THROW(log_publisher(*node, std::string(32, 'a')));
}

TEST_F(TestCentralLogger, PublisherTruncatesContents)
{
    std::optional<Loginfo> received;
    const auto sub = node->create_subscription<Loginfo>(
        "loginfo_receive", 10, [&received](const Loginfo& msg) { received = msg; });

    const std::string name(32, 'n');
    log_publisher publisher(*node, name);
    ASSERT_TRUE(spin_until([&] { return sub->get_publisher_count() > 0; }));

    publisher.log(std::string(300, 'c'));
    ASSERT_TRUE(spin_until([&] { return received.has_value(); }));

    // Both fields are completely filled, with no null terminator
    EXPECT_EQ(std::string(received->name.begin(), received->name.end()), name);
    EXPECT_EQ(std::string(received->contents.begin(), received->contents.end()), std::string(256, 'c'));
}

TEST_F(TestCentralLogger, LogIsWrittenToFile)
{
    const auto logger = std::make_shared<central_logger>();
    executor.add_node(logger);

    // The constructor creates exactly one run directory under the log root
    const auto run_dir = std::filesystem::directory_iterator(temp_home / "Robosub" / "Logs")->path();
    const auto log_file = run_dir / "motors.log";

    log_publisher publisher(*node, "motors");
    ASSERT_TRUE(spin_until([&] { return node->count_subscribers("loginfo_receive") > 0; }));

    publisher.log("hello");
    ASSERT_TRUE(spin_until([&] { return std::filesystem::exists(log_file); }));

    std::ifstream file(log_file);
    std::string line;
    ASSERT_TRUE(std::getline(file, line));
    // e.g. "2026-11-04-13-05-09-123: hello"
    EXPECT_TRUE(std::regex_match(line, std::regex(R"(\d{4}(-\d{2}){5}-\d{3}: hello)"))) << line;
}

#ifdef ENABLE_TESTING
/**
 * @brief Main test runner
 */
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    rclcpp::init(argc, argv);
    const int result = RUN_ALL_TESTS();
    rclcpp::shutdown();
    return result;
}
#endif
