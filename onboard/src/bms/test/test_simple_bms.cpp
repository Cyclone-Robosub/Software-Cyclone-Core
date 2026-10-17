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
    int write_pipe_fds[2];  // For when BMS needs to write to the . [0] = read end, [1] = write end
    int read_pipe_fds[2];  // For when BMS needs to read from the . [0] = read end, [1] = write end

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

    
};


/**
 * @brief Test bms construction
 */
TEST_F(TestSimpleBMS, MBMSConstruction) {
    ASSERT_NO_THROW({
        createMux();
    });
    ASSERT_NE(mux, nullptr);
    EXPECT_EQ(mux->get_name(), std::string("SoftMux"));
}
