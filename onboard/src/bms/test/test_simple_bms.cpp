#include <gtest/gtest.h>
#include <rclcpp/rclcpp.hpp>

#include "simple_bms.hpp"

#include <chrono>


/**
* @brief Test code for Simple_BMS node
* This code will create a mock serial device to test the
* simple bms without requiring actual hardware.
*/

class TestSimpleBMS : public::testing::Test {
protected:
    std::shared_ptr<SimpleBMS> node;
    rclcpp::Subscription<custom_interfaces::msg::Battery>::SharedPtr bms_subscriber;
    int write_pipe_fds[2];  // For when BMS needs to write to the ADC. [0] = read end, [1] = write end
    int read_pipe_fds[2];  // For when BMS needs to read from the ADC. [0] = read end, [1] = write end
    int voltage_reading;
    int current_reading;

    void SetUp() override {
        // Initialize ROS2
        if (!rclcpp::ok()) {
            rclcpp::init(0, nullptr);
        }
        
        // Create pipe for mock serial communication
        // write_pipe_fds[0] = read end, write_pipe_fds[1] = write end
        if (pipe(write_pipe_fds) == -1) {
            FAIL() << "Failed to create write pipe for mock serial";
        }
        if (pipe(read_pipe_fds) == -1) {
            FAIL() << "Failed to create read pipe for mock serial";
        }

        fcntl(read_pipe_fds[0], F_SETFL, (fcntl(read_pipe_fds[0], F_GETFD)|O_NONBLOCK));
    }

     /**
     * @brief Clean up after each test completes
     * 
     * Shuts down the node and closes pipes
     */
    void TearDown() override {
        node.reset();
        close(write_pipe_fds[0]);
        close(write_pipe_fds[1]);
        close(read_pipe_fds[0]);
        close(read_pipe_fds[1]);
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
        FD_SET(write_pipe_fds[0], &readfds);
        
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = (timeout_ms % 1000) * 1000;
        
        int result = select(write_pipe_fds[0] + 1, &readfds, nullptr, nullptr, &timeout);
        
        if (result > 0) {
            ssize_t bytes_read = read(write_pipe_fds[0], buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                return std::string(buffer);
            }
        }
        return "";
    }

    /**
    * @brief Helper to write a serial message to pipe_fds[1]
    * 
    * @param data_type The data type of the serial message
    */
    void write_serial_message(std::string message){
        size_t length = message.size();
        write(read_pipe_fds[1], message.c_str(), length);
    }

    void bms_callback(custom_interfaces::msg::Battery::UniquePtr bms_msg) {
        voltage_reading = bms_msg->voltage;
        current_reading = bms_msg->current;
    }

    /**
     * @brief Create a node with custom parameters using test mode
     * 
     */
    void create_node_with_params() {
        // Use write_pipe_fds[1] (write end) as the file descriptor
        // The node will write to it, and we'll read from write_pipe_fds[0]
        std::unique_ptr<FD_Interface> pipe_fd = std::make_unique<Direct_FD>(read_pipe_fds[0], write_pipe_fds[1]);

        node = std::make_shared<SimpleBMS>( 
            std::move(pipe_fd)  // Write end of pipe
        );
        bms_subscriber = node->create_subscription<custom_interfaces::msg::Battery>("bms", 10, std::bind(&TestSimpleBMS::bms_callback, this, std::placeholders::_1, std::placeholders::_2));
    }
};


/**
 * @brief Test bms construction
 */
TEST_F(TestSimpleBMS, BMSConstruction) {
    ASSERT_NO_THROW({
        create_node_with_params();
    });
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->get_name(), std::string("SimpleBMS"));
}
TEST_F(TestSimpleBMS, TestReadings) {
    create_node_with_params();
    write_serial_message("10");
    write_serial_message("20");
    ASSERT_EQ(voltage_reading, 10);
    ASSERT_EQ(current_reading, 20);
}