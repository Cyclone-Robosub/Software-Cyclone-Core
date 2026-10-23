#include "central_logger.hpp"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <rclcpp/executors.hpp>

namespace
{
std::string get_cur_timestamp()
{
    const auto now = std::chrono::system_clock::now();
    const auto seconds = std::chrono::floor<std::chrono::seconds>(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now - seconds)
                        .count();
    const auto time = std::chrono::system_clock::to_time_t(seconds);

    std::tm local{};
    if (localtime_r(&time, &local) == nullptr)
    {
        throw std::runtime_error("Could not convert local time");
    }

    std::ostringstream result;
    result << std::put_time(&local, "%Y-%m-%d-%H-%M-%S")
           << '-' << std::setfill('0') << std::setw(3) << ms;
    return result.str();
}
} // namespace

central_logger::central_logger()
: rclcpp::Node("Central_logger")
{
    const char* home = std::getenv("HOME");
    if (home == nullptr || *home == '\0')
    {
        throw std::runtime_error("HOME not found");
    }

    current_run_dir = std::filesystem::path(home) / "Robosub" / "Logs" / get_cur_timestamp();
    std::filesystem::create_directories(current_run_dir);

    loginfo_subscriber = this->create_subscription<custom_interfaces::msg::Loginfo>("loginfo_receive", 10, std::bind(&central_logger::central_logger_callback, this, std::placeholders::_1));
}

void central_logger::central_logger_callback(custom_interfaces::msg::Loginfo::UniquePtr msg)
{
    log_to_file(msg->name, msg->contents);
}

void central_logger::log_to_file(const std::array<std::uint8_t, 32>& name, const std::array<std::uint8_t, 256>& contents)
{
    const std::string file_name(name.begin(), std::find(name.begin(), name.end(), '\0'));
    const std::string msg_contents(contents.begin(), std::find(contents.begin(), contents.end(), '\0'));

    if (file_name.empty() || std::filesystem::path(file_name).filename() != file_name)
    {
        RCLCPP_ERROR(this->get_logger(), "Invalid log file name: '%s'", file_name.c_str());
        return;
    }

    const auto log_file = current_run_dir / (file_name + ".log");
    std::ofstream file(log_file, std::ios::app);
    if (!file)
    {
        RCLCPP_ERROR(this->get_logger(), "Could not open log file: '%s'", log_file.c_str());
        return;
    }

    file << get_cur_timestamp()
         << ": " << msg_contents << '\n';
    file.flush();
    if (!file)
    {
        RCLCPP_ERROR(this->get_logger(), "Could not write log file: '%s'", log_file.c_str());
    }
}

#ifndef ENABLE_TESTING
int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<central_logger>());
    rclcpp::shutdown();
    return 0;
}
#endif
