#include <gtest/gtest.h>
#include <rclcpp/rclcpp.hpp>
#include "std_msgs/msg/u_int8.hpp"
#include "manipulator.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <chrono>

/**
 * @brief Test fixture for Manipulator node
 * 
 * This fixture creates a mock serial device using a pipe to test
 * the manipulator node without requiring actual hardware.
 */
class TestManipulator : public ::testing::Test {
protected:
    std::shared_ptr<Manipulator> node;
    int pipe_fds[2];  // [0] = read end, [1] = write end
    
    /**
     * @brief Set up test environment before each test
     * 
     * Creates a pipe for mock serial communication and initializes ROS2
     */
    void SetUp() override {
        // Initialize ROS2
        if (!rclcpp::ok()) {
            rclcpp::init(0, nullptr);
        }
        
        // Create pipe for mock serial communication
        // pipe_fds[0] = read end, pipe_fds[1] = write end
        if (pipe(pipe_fds) == -1) {
            FAIL() << "Failed to create pipe for mock serial";
        }
    }
    
    /**
     * @brief Clean up after each test completes
     * 
     * Shuts down the node and closes pipes
     */
    void TearDown() override {
        node.reset();
        close(pipe_fds[0]);
        close(pipe_fds[1]);
    }
    
    /**
     * @brief Helper function to read serial output from the node
     * 
     * @param timeout_ms Maximum time to wait for data
     * @return std::string The data read from the serial interface
     */
    std::string read_serial_output(int timeout_ms = 100) {
        char buffer[1024];
        fd_set readfds;
        struct timeval timeout;
        
        FD_ZERO(&readfds);
        FD_SET(pipe_fds[0], &readfds);
        
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = (timeout_ms % 1000) * 1000;
        
        int result = select(pipe_fds[0] + 1, &readfds, nullptr, nullptr, &timeout);
        
        if (result > 0) {
            ssize_t bytes_read = read(pipe_fds[0], buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                return std::string(buffer);
            }
        }
        
        return "";
    }
    
    /**
     * @brief Create a node
     */
    void create_node() {
        // Use pipe_fds[1] (write end) as the file descriptor
        // The node will write to it, and we'll read from pipe_fds[0]
        std::unique_ptr<FD_Interface> pipe_fd = std::make_unique<Direct_FD>(-1, pipe_fds[1]);

        node = std::make_shared<Manipulator>(
            std::move(pipe_fd)  // Write end of pipe
        );
    }
    
    /**
     * @brief Helper to publish PWM message and process it
     * 
     * @param msg The PWM message to publish
     */
    void publish_and_process(std_msgs::msg::UInt8::SharedPtr msg) {
        auto publisher = node->create_publisher<std_msgs::msg::UInt8>("manipulator_cmd", 10);
        
        // Give publisher time to connect
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        publisher->publish(*msg);
        
        // Process the message
        rclcpp::spin_some(node);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
};

/**
 * @brief Test basic node construction
 */
TEST_F(TestManipulator, NodeConstruction) {
    ASSERT_NO_THROW({
        create_node();
    });
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->get_name(), std::string("manipulator"));
}

/**
 * @brief Test releasing element 1
 */
TEST_F(TestManipulator, ReleaseOne) {
    create_node();
    
    auto msg = std::make_shared<std_msgs::msg::UInt8>();
    msg->data = 1;
    
    publish_and_process(msg);
    
    std::string output = read_serial_output(200);
    EXPECT_EQ(output, "1\r\nr\r\n"); // Release 1, then reset
}

/**
 * @brief Test releasing element 2
 */
TEST_F(TestManipulator, ReleaseTwo) {
    create_node();
    
    auto msg = std::make_shared<std_msgs::msg::UInt8>();
    msg->data = 2;
    
    publish_and_process(msg);
    
    std::string output = read_serial_output(200);
    EXPECT_EQ(output, "2\r\nr\r\n"); // Release 1, then reset
}
/**
 * @brief Test invalid message doesn't get sent
 */
TEST_F(TestManipulator, InvalidMessage) {
    create_node();
    
    auto msg = std::make_shared<std_msgs::msg::UInt8>();
    msg->data = 3;
    
    publish_and_process(msg);
    
    std::string output = read_serial_output(200);
    EXPECT_EQ(output, ""); // Release 1, then reset
}

/**
 * @brief Test that the subscription exists on the node
 */
TEST_F(TestManipulator, SubscriptionExists) {
    create_node();
    
    // Give node time to fully initialize
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto subscriptions = node->get_subscriptions_info_by_topic("manipulator_cmd");
    
    EXPECT_GE(subscriptions.size(), 1);
}


#ifdef ENABLE_TESTING
/**
 * @brief Main test runner
 */
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif