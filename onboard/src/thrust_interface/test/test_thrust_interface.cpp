#include <gtest/gtest.h>
#include <rclcpp/rclcpp.hpp>

#include <custom_interfaces/msg/pwms.hpp>
#include "thrust_interface.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <chrono>

/**
 * @brief Test fixture for Thrust_Interface node
 * 
 * This fixture creates a mock serial device using a pipe to test
 * the thrust interface without requiring actual hardware.
 */
class TestThrustInterface : public ::testing::Test {
protected:
    std::shared_ptr<Thrust_Interface> node;
    rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr thrust_interface_heartbeat_subscriber;
    int write_pipe_fds[2];  // For when ThrustInterface needs to write to the Pico. [0] = read end, [1] = write end
    int read_pipe_fds[2];  // For when ThrustInterface needs to read from the Pico. [0] = read end, [1] = write end
    std::vector<int> test_thrusters;
    bool active_heartbeat;
    
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
        // write_pipe_fds[0] = read end, write_pipe_fds[1] = write end
        if (pipe(write_pipe_fds) == -1) {
            FAIL() << "Failed to create write pipe for mock serial";
        }
        if (pipe(read_pipe_fds) == -1) {
            FAIL() << "Failed to create read pipe for mock serial";
        }

        fcntl(read_pipe_fds[0], F_SETFL, (fcntl(read_pipe_fds[0], F_GETFD)|O_NONBLOCK));
        
        // Set up test thruster configuration
        test_thrusters = {2, 0, 6, 4, 19, 26, 21, 27};
        active_heartbeat = false;
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
    
    /**
     * @brief Create a node with custom parameters using test mode
     * 
     * @param min_pwm Minimum PWM value
     * @param max_pwm Maximum PWM value
     */
    void create_node_with_params(int min_pwm, int max_pwm) {
        // Use write_pipe_fds[1] (write end) as the file descriptor
        // The node will write to it, and we'll read from write_pipe_fds[0]
        std::unique_ptr<FD_Interface> pipe_fd = std::make_unique<Direct_FD>(read_pipe_fds[0], write_pipe_fds[1]);

        node = std::make_shared<Thrust_Interface>(
            test_thrusters, 
            std::move(pipe_fd),  // Write end of pipe
            min_pwm, 
            max_pwm
        );
    }

    void create_node_with_bad_pico_connection() {
        std::unique_ptr<FD_Interface> pipe_fd = std::make_unique<Direct_FD>(-1, -1);
        node = std::make_shared<Thrust_Interface>(
            test_thrusters,
            std::move(pipe_fd),  // Write end of pipe
            1100, 
            1900
        );
    }
    
    /**
     * @brief Helper to publish PWM message and process it
     * 
     * @param msg The PWM message to publish
     */
    void publish_and_process(custom_interfaces::msg::Pwms::SharedPtr msg) {
        auto publisher = node->create_publisher<custom_interfaces::msg::Pwms>("pwm_cmd", 10);
        
        // Give publisher time to connect
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        publisher->publish(*msg);
        
        // Process the message
        rclcpp::spin_some(node);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    /**
     * @brief Helper to publish the heartbeat message
     */
    void publish_heartbeat() {
        auto publisher = node->create_publisher<std_msgs::msg::Empty>("mux_heartbeat", 10);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        auto message = std_msgs::msg::Empty();

        publisher->publish(message);

        rclcpp::spin_some(node);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    /*
    *   Helper function to subscribe to thrust_interface_heartbeat topic and get the heartbeats from thrust_interface
    */
    void subscribe_thrust_interface_heartbeat() {
        thrust_interface_heartbeat_subscriber = node->create_subscription<std_msgs::msg::Empty>("thrust_interface_heartbeat", 10, std::bind(&TestThrustInterface::thrust_interface_heartbeat_callback, this, std::placeholders::_1));
    }
    
    /*
    *   Helper function for the callback for thrust_interface heartbeat
    */
    void thrust_interface_heartbeat_callback(std_msgs::msg::Empty msg) {
        active_heartbeat = true;
        (void) msg;
    }
};

/**
 * @brief Test basic node construction
 */
TEST_F(TestThrustInterface, NodeConstruction) {
    ASSERT_NO_THROW({
        create_node_with_params(1200, 1800);
    });
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->get_name(), std::string("thrust_interface"));
}

/**
 * @brief Test valid PWM values within range
 */
TEST_F(TestThrustInterface, ValidPWMValues) {
    create_node_with_params(1200, 1800);
    
    auto msg = std::make_shared<custom_interfaces::msg::Pwms>();
    // Set valid PWM values
    for (int i = 0; i < 8; i++) {
        msg->pwms[i] = 1500;  // Valid value within range
    }
    
    publish_heartbeat();
    publish_and_process(msg);
    
    std::string output = read_serial_output(200);
    
    // Verify that all commands have PWM value of 1500 (unchanged)
    for (int i = 0; i < 8; i++) {
        std::string expected = "Set " + std::to_string(test_thrusters[i]) + " PWM 1500";
        EXPECT_NE(output.find(expected), std::string::npos)
            << "Expected unchanged PWM command for thruster " << test_thrusters[i];
    }
}

/**
 * @brief Test mixed PWM values (some need clamping, some don't)
 */
TEST_F(TestThrustInterface, MixedPWMValues) {
    create_node_with_params(1200, 1800);
    
    auto msg = std::make_shared<custom_interfaces::msg::Pwms>();
    msg->pwms[0] = 1000;  // Below min -> should clamp to 1200
    msg->pwms[1] = 1300;  // Valid
    msg->pwms[2] = 1500;  // Valid
    msg->pwms[3] = 1700;  // Valid
    msg->pwms[4] = 2000;  // Above max -> should clamp to 1800
    msg->pwms[5] = 1200;  // At min boundary
    msg->pwms[6] = 1800;  // At max boundary
    msg->pwms[7] = 1400;  // Valid
    
    publish_heartbeat();
    publish_and_process(msg);
    
    std::string output = read_serial_output(200);
    
    // Expected PWM values after clamping
    std::vector<int> expected_pwms = {1200, 1300, 1500, 1700, 1800, 1200, 1800, 1400};
    
    for (int i = 0; i < 8; i++) {
        std::string expected = "Set " + std::to_string(test_thrusters[i]) + 
                              " PWM " + std::to_string(expected_pwms[i]);
        EXPECT_NE(output.find(expected), std::string::npos)
            << "Expected PWM " << expected_pwms[i] << " for thruster " << test_thrusters[i];
    }
}

/**
 * @brief Test subscription exists and is properly configured
 */
TEST_F(TestThrustInterface, SubscriptionExists) {
    create_node_with_params(1200, 1800);
    
    // Give node time to fully initialize
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto subscriptions = node->get_subscriptions_info_by_topic("pwm_cmd");
    
    EXPECT_GE(subscriptions.size(), 1);
}

/**
 * @brief Test PWM clamping to minimum value
 */
TEST_F(TestThrustInterface, PWMClampingMinimum) {
    create_node_with_params(1200, 1800);
    
    auto msg = std::make_shared<custom_interfaces::msg::Pwms>();
    // Set all PWMs below minimum
    for (int i = 0; i < 8; i++) {
        msg->pwms[i] = 1000;  // Below min_pwm of 1200
    }
    publish_heartbeat();
    publish_and_process(msg);
    
    // Read all serial output
    std::string output = read_serial_output(200);
    
    // Verify that all commands have PWM value of 1200 (clamped)
    for (int i = 0; i < 8; i++) {
        std::string expected = "Set " + std::to_string(test_thrusters[i]) + " PWM 1200";
        EXPECT_NE(output.find(expected), std::string::npos) 
            << "Expected clamped PWM command for thruster " << test_thrusters[i];
    }
}

/**
 * @brief Test PWM clamping to maximum value
 */
TEST_F(TestThrustInterface, PWMClampingMaximum) {
    create_node_with_params(1200, 1800);
    
    auto msg = std::make_shared<custom_interfaces::msg::Pwms>();
    // Set all PWMs above maximum
    for (int i = 0; i < 8; i++) {
        msg->pwms[i] = 2000;  // Above max_pwm of 1800
    }
    
    publish_heartbeat();
    publish_and_process(msg);
    
    std::string output = read_serial_output(200);
    
    // Verify that all commands have PWM value of 1800 (clamped)
    for (int i = 0; i < 8; i++) {
        std::string expected = "Set " + std::to_string(test_thrusters[i]) + " PWM 1800";
        EXPECT_NE(output.find(expected), std::string::npos)
            << "Expected clamped PWM command for thruster " << test_thrusters[i];
    }
}

/**
 * @brief Test custom PWM limits
 */
TEST_F(TestThrustInterface, CustomPWMLimits) {
    create_node_with_params(1000, 2000);
    
    auto msg = std::make_shared<custom_interfaces::msg::Pwms>();
    msg->pwms[0] = 900;   // Below custom min -> clamp to 1000
    msg->pwms[1] = 2100;  // Above custom max -> clamp to 2000
    msg->pwms[2] = 1500;  // Valid
    
    for (int i = 3; i < 8; i++) {
        msg->pwms[i] = 1500;
    }
    
    publish_heartbeat();
    publish_and_process(msg);
    
    std::string output = read_serial_output(200);
    
    // Check first thruster clamped to 1000
    std::string expected0 = "Set " + std::to_string(test_thrusters[0]) + " PWM 1000";
    EXPECT_NE(output.find(expected0), std::string::npos);
    
    // Check second thruster clamped to 2000
    std::string expected1 = "Set " + std::to_string(test_thrusters[1]) + " PWM 2000";
    EXPECT_NE(output.find(expected1), std::string::npos);
    
    // Check third thruster unchanged at 1500
    std::string expected2 = "Set " + std::to_string(test_thrusters[2]) + " PWM 1500";
    EXPECT_NE(output.find(expected2), std::string::npos);
}

/**
 * @brief Test boundary values exactly at limits
 */
TEST_F(TestThrustInterface, BoundaryValues) {
    create_node_with_params(1200, 1800);
    
    auto msg = std::make_shared<custom_interfaces::msg::Pwms>();
    // Set all to exactly minimum
    for (int i = 0; i < 4; i++) {
        msg->pwms[i] = 1200;
    }
    // Set rest to exactly maximum
    for (int i = 4; i < 8; i++) {
        msg->pwms[i] = 1800;
    }
    
    publish_heartbeat();
    publish_and_process(msg);
    
    std::string output = read_serial_output(200);
    
    // Verify boundary values are not clamped
    for (int i = 0; i < 4; i++) {
        std::string expected = "Set " + std::to_string(test_thrusters[i]) + " PWM 1200";
        EXPECT_NE(output.find(expected), std::string::npos);
    }
    
    for (int i = 4; i < 8; i++) {
        std::string expected = "Set " + std::to_string(test_thrusters[i]) + " PWM 1800";
        EXPECT_NE(output.find(expected), std::string::npos);
    }
}

/**
 * @brief Test 0 PWM goes through properly
 */
TEST_F(TestThrustInterface, ZeroPWM) {
    create_node_with_params(1200, 1800);
    
    auto msg = std::make_shared<custom_interfaces::msg::Pwms>();
    // Set valid PWM values
    for (int i = 0; i < 8; i++) {
        msg->pwms[i] = 0;
    }
    
    publish_heartbeat();
    publish_and_process(msg);
    
    std::string output = read_serial_output(200);
    
    // Verify that all commands have PWM value of 1500 (unchanged)
    for (int i = 0; i < 8; i++) {
        std::string expected = "Set " + std::to_string(test_thrusters[i]) + " PWM 0";
        EXPECT_NE(output.find(expected), std::string::npos)
            << "Expected unchanged PWM command for thruster " << test_thrusters[i];
    }
}


/**
 * @brief Test don't accept commands when no heartbeat
 */
TEST_F(TestThrustInterface, NoHeartbeat) {
    create_node_with_params(1200, 1800);

    auto msg = std::make_shared<custom_interfaces::msg::Pwms>();
    // Set valid PWM values
    for (int i = 0; i < 8; i++) {
        msg->pwms[i] = 1800;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    publish_and_process(msg);

    std::string output = read_serial_output(200);

    // Verify that all commands have PWM value of 1500 instead of 1800
    for (int i = 0; i < 8; i++) {
        std::string expected = "Set " + std::to_string(test_thrusters[i]) + " PWM 1500";
        EXPECT_NE(output.find(expected), std::string::npos)
            << "Expected unchanged PWM command for thruster " << test_thrusters[i];

        std::string not_expected = "Set " + std::to_string(test_thrusters[i]) + " PWM 1800";
        EXPECT_EQ(output.find(not_expected), std::string::npos)
            << "Expected to not send our original command " << test_thrusters[i];
    }
}

/**
 * @brief Test heartbeat published when connection to Pico exists
 */
TEST_F(TestThrustInterface, ActiveThrustInterfaceHeartbeat) {
    create_node_with_params(1200, 1800);
    subscribe_thrust_interface_heartbeat();

    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(node);

    auto start = std::chrono::steady_clock::now();
    auto deadline = start + std::chrono::seconds(2);
    while (std::chrono::steady_clock::now() < deadline) {
        exec.spin_some();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (std::chrono::steady_clock::now() >= start + std::chrono::seconds(1)) {
            EXPECT_TRUE(active_heartbeat);
            return;
        }
    }
}

/**
 * @brief Test heartbeat not published when no connection to Pico exists
 */
TEST_F(TestThrustInterface, InactiveThrustInterfaceHeartbeat) {
    create_node_with_bad_pico_connection();
    subscribe_thrust_interface_heartbeat();

    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(node);

    auto start = std::chrono::steady_clock::now();
    auto deadline = start + std::chrono::seconds(2);
    while (std::chrono::steady_clock::now() < deadline) {
        exec.spin_some();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (std::chrono::steady_clock::now() >= start + std::chrono::seconds(1)) {
            EXPECT_FALSE(active_heartbeat);
            return;
        }
    }
}

 /**
 * @brief Test that we can send a reset signal when the Pico is alive
 */
 TEST_F(TestThrustInterface, RevivePicoSuccess){
    create_node_with_params(1200,1800);
    auto client = node->create_client<std_srvs::srv::Trigger>("revive_pico");
    ASSERT_TRUE(client->wait_for_service(std::chrono::seconds(1)));

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);

    std::thread spin_thread([&executor]() {
        executor.spin();
    });

    std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();
    auto future = client->async_send_request(request);

    write_serial_message("revived\r\n");

    ASSERT_EQ(future.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    EXPECT_TRUE(future.get()->success); // expect that we did received a successful revived response
    executor.cancel();
    spin_thread.join();
 }

 /**
 * @brief Test that we can send fail successfully when no response from Pico
 */
 TEST_F(TestThrustInterface, RevivePicoFailure){
    create_node_with_params(1200,1800);
    auto client = node->create_client<std_srvs::srv::Trigger>("revive_pico");
    ASSERT_TRUE(client->wait_for_service(std::chrono::seconds(1)));

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);

    std::thread spin_thread([&executor]() {
        executor.spin();
    });

    std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();
    auto future = client->async_send_request(request);

    ASSERT_EQ(future.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    EXPECT_FALSE(future.get()->success); // expect that we did not receive a successful revived response
    executor.cancel();
    spin_thread.join();
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
