#include <gtest/gtest.h>
#include <rclcpp/rclcpp.hpp>
#include <custom_interfaces/msg/pwms.hpp>
#include "soft_mux.hpp"
#include "std_msgs/msg/bool.hpp"


using namespace std::chrono_literals;
class TestSoftMuxInterface : public::testing::Test {
    protected:
        std::shared_ptr<SoftMux> mux;
        rclcpp::Subscription<custom_interfaces::msg::Pwms>::SharedPtr pwm_cmd_subscriber;
        rclcpp::Subscription<std_msgs::msg::UInt8>::SharedPtr current_control_mode_subscriber;
        rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr mux_heartbeat_subscriber;
        custom_interfaces::msg::Pwms most_recent_msg;
        std_msgs::msg::UInt8 most_recent_mode;
        bool active_mux_heartbeat;
       
        void SetUp() override {
            // Initialize ROS2
            if (!rclcpp::ok()) {
                rclcpp::init(0, nullptr);
            }
            active_mux_heartbeat = false;
        }
        void TearDown() override {
            mux.reset();
            rclcpp::shutdown();
        }
        void createMux() {
            mux = std::make_shared<SoftMux>();
        }

        /*
        *   Helper function to subscribe to the pwm_cmd topic to get the PWMs sent through
        */
        void subscribe() {
            pwm_cmd_subscriber = mux->create_subscription<custom_interfaces::msg::Pwms>("pwm_cmd", 10, std::bind(&TestSoftMuxInterface::pwm_cmd_callback, this, std::placeholders::_1));
        }
        
        /*
        *   Helper function for the callback for pwm_cmd
        */
        void pwm_cmd_callback(custom_interfaces::msg::Pwms::UniquePtr pwm) {
            for (int i = 0; i < 8; i++) {
                most_recent_msg.pwms[i] = pwm->pwms[i];
            }
        }

        /*
        *   Helper function to subscribe to the current_mode topic and get the current mode
        */
        void subscribe_control_mode() {
            current_control_mode_subscriber = mux->create_subscription<std_msgs::msg::UInt8>("current_mode", 10, std::bind(&TestSoftMuxInterface::control_mode_callback, this, std::placeholders::_1));
        }

        /*
        *   Helper function for the callback for control mode
        */
        void control_mode_callback(std_msgs::msg::UInt8 msg) {
            most_recent_mode = msg;
        }

        /*
        *   Helper function to subscribe to mux_heartbeat topic and get the heartbeats from mux
        */
        void subscribe_mux_heartbeat() {
            mux_heartbeat_subscriber = mux->create_subscription<std_msgs::msg::Empty>("mux_heartbeat", 10, std::bind(&TestSoftMuxInterface::mux_heartbeat_callback, this, std::placeholders::_1));
        }
        
        /*
        *   Helper function for the callback for mux heartbeat
        */
        void mux_heartbeat_callback(std_msgs::msg::Empty msg) {
            active_mux_heartbeat = true;
            (void) msg;
        }
};

/**
 * @brief Test mux construction
 */
TEST_F(TestSoftMuxInterface, MuxConstruction) {
    ASSERT_NO_THROW({
        createMux();
    });
    ASSERT_NE(mux, nullptr);
    EXPECT_EQ(mux->get_name(), std::string("SoftMux"));
}

/**
 * @brief Test mux sending ctrl pwms
 */
TEST_F(TestSoftMuxInterface, MuxSendCtrlPwms) {
    createMux();
    /* generate sample data
       expected1: sent through pwm_ctrl
       expected2: sent through pwm_cli
    */
    auto msg1 = std::make_unique<custom_interfaces::msg::Pwms>();
    auto msg2 = std::make_unique<custom_interfaces::msg::Pwms>();
    std::array<int, 8> expected1;
    std::array<int, 8> expected2;
    for (int i = 0; i < 8; i++) {
         msg1->pwms[i] = 2 * (i + 1);
         expected1[i] = msg1->pwms[i];
    }
    for (int i = 0; i < 8; i++) {
         msg2->pwms[i] = 8 * (i + 7);
         expected2[i] = msg2->pwms[i];
    }
    // subscribe to pwm_cmd and get the pwms
    subscribe();
   
    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(mux);

    mux->control_mode = CTRL;
    mux->no_ctrl_heartbeat = false;
    ASSERT_NE(mux, nullptr);

    // send through the messages
    mux->pwm_ctrl_callback(std::move(msg1));
    mux->pwm_cli_callback(std::move(msg2));
    exec.spin_some();
   
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(most_recent_msg.pwms[i], expected1[i]);
    }
}

/**
 * @brief Test mux sending cli pwms
 */
TEST_F(TestSoftMuxInterface, MuxSendCliPwms) {
    createMux();
    /* generate sample data
       expected1: sent through pwm_ctrl
       expected2: sent through pwm_cli
    */
    auto msg1 = std::make_unique<custom_interfaces::msg::Pwms>();
    auto msg2 = std::make_unique<custom_interfaces::msg::Pwms>();
    std::array<int, 8> expected1;
    std::array<int, 8> expected2;
    for (int i = 0; i < 8; i++) {
         msg1->pwms[i] = 2 * (i + 1);
         expected1[i] = msg1->pwms[i];
    }
    for (int i = 0; i < 8; i++) {
         msg2->pwms[i] = 8 * (i + 7);
         expected2[i] = msg2->pwms[i];
    }
    // subscribe to pwm_cmd and get the pwms
    subscribe();
   
    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(mux);

    mux->control_mode = CLI;
    mux->no_cli_heartbeat = false;
    ASSERT_NE(mux, nullptr);

    // send through the messages
    mux->pwm_ctrl_callback(std::move(msg1));
    mux->pwm_cli_callback(std::move(msg2));
    exec.spin_some();
   
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(most_recent_msg.pwms[i], expected2[i]);
    }
}

/**
 * @brief Test control mode service
 */
TEST_F(TestSoftMuxInterface, MuxTestSetModeService) {
    createMux();
    auto client = mux->create_client<custom_interfaces::srv::ControlMode>("control_mode");
    subscribe_control_mode(); // since the mode changes
    ASSERT_TRUE(client->wait_for_service(std::chrono::seconds(1)));

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(mux);

    std::thread spin_thread([&executor]() {
        executor.spin();
    });

    std::shared_ptr<custom_interfaces::srv::ControlMode::Request> request = std::make_shared<custom_interfaces::srv::ControlMode::Request>();
    request->mode = CTRL;
    auto future = client->async_send_request(request);

    ASSERT_EQ (future.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    EXPECT_EQ(mux->control_mode, CTRL);
   
    request->mode = CLI;
    auto future2 = client->async_send_request(request);

    ASSERT_EQ (future2.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    EXPECT_EQ(mux->control_mode, CLI);

    EXPECT_EQ(most_recent_mode.data, CLI); // resulting mode

    executor.cancel();
    spin_thread.join();
}

/**
 * @brief Test that we can set mode to echo
*/

TEST_F(TestSoftMuxInterface, MuxTestSetModeEcho) {
    createMux();
    auto client = mux->create_client<custom_interfaces::srv::ControlMode>("control_mode");
    subscribe_control_mode(); // since the mode changes
    ASSERT_TRUE(client->wait_for_service(std::chrono::seconds(1)));

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(mux);

    std::thread spin_thread([&executor]() {
        executor.spin();
    });

    std::shared_ptr<custom_interfaces::srv::ControlMode::Request> request = std::make_shared<custom_interfaces::srv::ControlMode::Request>();
    request->mode = Echo;
    auto future = client->async_send_request(request);

    ASSERT_EQ (future.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    EXPECT_EQ(mux->control_mode, Echo);
   
    executor.cancel();
    spin_thread.join();
}

/**
 * @brief Test control mode service
 */
TEST_F(TestSoftMuxInterface, MuxTestGetModeService) {
    createMux();
    auto set_client = mux->create_client<custom_interfaces::srv::ControlMode>("control_mode");
    auto get_client = mux->create_client<std_srvs::srv::SetBool>("force_pub");
    ASSERT_TRUE(set_client->wait_for_service(std::chrono::seconds(1)));
    ASSERT_TRUE(get_client->wait_for_service(std::chrono::seconds(1)));

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(mux);

    std::thread spin_thread([&executor]() {
        executor.spin();
    });

    subscribe_control_mode();
    most_recent_mode.data = CTRL; // set to be incorrect at first to verify that the message is received    

    /* Make the mux publish the current mode. Should be Disabled by default. */
    std::shared_ptr<std_srvs::srv::SetBool::Request> request = std::make_shared<std_srvs::srv::SetBool::Request>();
    request->data = true;
    auto future = get_client->async_send_request(request);
    
    ASSERT_EQ(future.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    EXPECT_EQ(most_recent_mode.data, Disabled);

    /* Force publishing again, this time when the mux is in Matlab/CTRL mode instead. */
    auto request2 = std::make_shared<custom_interfaces::srv::ControlMode::Request>();
    request2->mode = CTRL;
    auto future2 = set_client->async_send_request(request2);

    ASSERT_EQ (future2.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    most_recent_mode.data = CLI; // set to be incorrect at first to verify that the message is received

    auto request3 = std::make_shared<std_srvs::srv::SetBool::Request>();
    request3->data = true;
    auto future3 = get_client->async_send_request(request3);
    
    ASSERT_EQ(future3.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    EXPECT_EQ(most_recent_mode.data, CTRL); // resulting mode

    /* Force publishing again, this time when the mux is in Echo mode instead. */
    auto request4 = std::make_shared<custom_interfaces::srv::ControlMode::Request>();
    request4->mode = Echo;
    auto future4 = set_client->async_send_request(request4);

    ASSERT_EQ (future4.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    most_recent_mode.data = CLI; // set to be incorrect at first to verify that the message is received

    auto request5 = std::make_shared<std_srvs::srv::SetBool::Request>();
    request5->data = true;
    auto future5 = get_client->async_send_request(request3);
    
    ASSERT_EQ(future5.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    EXPECT_EQ(most_recent_mode.data, Echo); // resulting mode


    executor.cancel();
    spin_thread.join();
}

/**
 * @brief Test ctrl heartbeat
 */
TEST_F(TestSoftMuxInterface, MuxTestInputHB) {
    createMux();
   
    mux->control_mode = CTRL;
    std_msgs::msg::Empty::UniquePtr test_heartbeat = std::make_unique<std_msgs::msg::Empty>();
    mux->ctrl_heartbeat_callback(std::move(test_heartbeat));
   
    subscribe();
   
    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(mux);

    auto msg1 = std::make_unique<custom_interfaces::msg::Pwms>();
    for (int i = 0; i < 8; i++) {
        msg1->pwms[i] = 2 * (i + 1);
    }

    mux->pwm_ctrl_callback(std::move(msg1));
    exec.spin_some();

    for (int i = 0; i < 8; i++) {
        EXPECT_NE(most_recent_msg.pwms[i], 1500);
    }
   
}

/**
 * @brief Test no ctrl heartbeat on ctrl
 */
TEST_F(TestSoftMuxInterface, NoCtrlHeartbeatTriggersStop) {
    createMux();
    mux->control_mode = CTRL;
   
    subscribe();
    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(mux);

    auto msg = std::make_unique<custom_interfaces::msg::Pwms>();
    for (int i = 0; i < 8; i++) {
         msg->pwms[i] = 1600;
    }

    mux->pwm_ctrl_callback(std::move(msg));
    // fake bad heartbeat
    mux->recent_ctrl_heartbeat = std::chrono::steady_clock::now() - std::chrono::seconds(2);
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (std::chrono::steady_clock::now() < deadline) {
        exec.spin_some();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (most_recent_msg.pwms[0] == 1500) {
            break;
        }
    }

    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(most_recent_msg.pwms[i], 1500);
    }
}

/**
 * @brief Test no cli heartbeat on cli
 */
TEST_F(TestSoftMuxInterface, NoCliHeartbeatTriggersStop) {
    createMux();
    mux->control_mode = CLI;
   
    subscribe();
    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(mux);

    auto msg = std::make_unique<custom_interfaces::msg::Pwms>();
    for (int i = 0; i < 8; i++) {
         msg->pwms[i] = 1600;
    }

    mux->pwm_cli_callback(std::move(msg));
    // fake bad heartbeat
    mux->recent_cli_heartbeat = std::chrono::steady_clock::now() - std::chrono::seconds(2);
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (std::chrono::steady_clock::now() < deadline) {
        exec.spin_some();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (most_recent_msg.pwms[0] == 1500) {
            break;
        }
    }

    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(most_recent_msg.pwms[i], 1500);
    }
}

/**
 * @brief Test no echo heartbeat on echo
 */
TEST_F(TestSoftMuxInterface, NoEchoHeartbeatTriggersStop) {
    createMux();
    mux->control_mode = Echo;
   
    subscribe();
    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(mux);

    auto msg = std::make_unique<custom_interfaces::msg::Pwms>();
    for (int i = 0; i < 8; i++) {
         msg->pwms[i] = 1600;
    }

    mux->pwm_echo_callback(std::move(msg));
    // fake bad heartbeat
    mux->recent_echo_heartbeat = std::chrono::steady_clock::now() - std::chrono::seconds(2);
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (std::chrono::steady_clock::now() < deadline) {
        exec.spin_some();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (most_recent_msg.pwms[0] == 1500) {
            break;
        }
    }

    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(most_recent_msg.pwms[i], 1500);
    }
}


/**
 * @brief Test no ctrl heartbeat on cli
 */
TEST_F(TestSoftMuxInterface, NoCtrlHeartbeatonCli) {
    createMux();
    mux->control_mode = CLI;
   
    std_msgs::msg::Empty::UniquePtr test_heartbeat = std::make_unique< std_msgs::msg::Empty>();
    mux->cli_heartbeat_callback(std::move(test_heartbeat));

    subscribe();
    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(mux);

    auto msg = std::make_unique<custom_interfaces::msg::Pwms>();
    for (int i = 0; i < 8; i++) {
         msg->pwms[i] = 1600;
    }

    mux->pwm_cli_callback(std::move(msg));
    // fake bad heartbeat for ctrl
    mux->recent_ctrl_heartbeat = std::chrono::steady_clock::now() - std::chrono::seconds(2);
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (std::chrono::steady_clock::now() < deadline) {
        exec.spin_some();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (int i = 0; i < 8; i++) {
        EXPECT_NE(most_recent_msg.pwms[i], 1500);
        EXPECT_EQ(most_recent_msg.pwms[i], 1600);
    }
}

/**
 * @brief Test no cli heartbeat on ctrl
 */
TEST_F(TestSoftMuxInterface, NoCliHeartbeatonCtrl) {
    createMux();
    mux->control_mode = CTRL;
   
    std_msgs::msg::Empty::UniquePtr test_heartbeat = std::make_unique< std_msgs::msg::Empty>();
    mux->ctrl_heartbeat_callback(std::move(test_heartbeat));

    subscribe();
    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(mux);

    auto msg = std::make_unique<custom_interfaces::msg::Pwms>();
    for (int i = 0; i < 8; i++) {
         msg->pwms[i] = 1600;
    }

    mux->pwm_ctrl_callback(std::move(msg));
    // fake bad heartbeat for cli
    mux->recent_cli_heartbeat = std::chrono::steady_clock::now() - std::chrono::seconds(2);
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (std::chrono::steady_clock::now() < deadline) {
        exec.spin_some();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (int i = 0; i < 8; i++) {
        EXPECT_NE(most_recent_msg.pwms[i], 1500);
        EXPECT_EQ(most_recent_msg.pwms[i], 1600);
    }
}

/**
 * @brief Test mux active
 */
TEST_F(TestSoftMuxInterface, MuxHeartbeat) {
    createMux();
    mux->control_mode = CTRL;
    subscribe_mux_heartbeat();

    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(mux);

    auto start = std::chrono::steady_clock::now();
    auto deadline = start + std::chrono::seconds(2);
    int i = 1;
    int k = 0.5;
    while (std::chrono::steady_clock::now() < deadline) {
        exec.spin_some();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (std::chrono::steady_clock::now() == start + k * std::chrono::seconds(1)) {
            mux->no_ctrl_heartbeat = false;
            k = k + 1;
        }
        if (std::chrono::steady_clock::now() == start + i * std::chrono::seconds(1)) {
            EXPECT_TRUE(active_mux_heartbeat);
            i++;
        }
    }
}

/**
 * @brief Test Disabled publishes 1500s
 */
TEST_F(TestSoftMuxInterface, DisabledPublishesStopSets) {
    createMux();
    mux->control_mode = Disabled;
    subscribe();
   
    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(mux);

    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while (std::chrono::steady_clock::now() < deadline) {
        exec.spin_some();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(most_recent_msg.pwms[i], 1500);
    }
}

#ifdef ENABLE_TESTING
    int main(int argc, char** argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
#endif