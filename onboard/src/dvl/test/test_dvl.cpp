#include <gtest/gtest.h>
#include <rclcpp/rclcpp.hpp>
#include "dvl.hpp"
#include "fd_interface.hpp"
#include <chrono>

static constexpr uint8_t VR_TYPE = 'v';
static constexpr uint8_t BAD_VR_TYPE = 'V';
static constexpr uint8_t DRR_TYPE = 'd';
static constexpr uint8_t CONFIG_TYPE = 'c';
static constexpr uint8_t ACK_TYPE = 'a';

static constexpr uint8_t DVL_READ = 'r';
static constexpr uint8_t DVL_WRITE = 'w';

using namespace dvl;

class TestDVLInterface : public::testing::Test {
protected:
    std::shared_ptr<DVL> node;
    int dev_fd;
    int node_read_pipe_fds[2];  // [0] = read end, [1] = write end. For the node to read from.
    int node_write_pipe_fds[2];  // [0] = read end, [1] = write end. For the node to write from.
    bool active_heartbeat;
    rclcpp::Subscription<std_msgs::msg::Empty>::SharedPtr dvl_heartbeat_subscriber;
    rclcpp::Subscription<custom_interfaces::msg::VR>::SharedPtr velocity_report_subscriber;
    custom_interfaces::msg::VR most_recent_velocity_report;
    int msg_count;
    rclcpp::Subscription<custom_interfaces::msg::DRR>::SharedPtr drr_report_subscriber;
    custom_interfaces::msg::DRR most_recent_drr_report;
    rclcpp::Subscription<custom_interfaces::msg::Config>::SharedPtr config_report_subscriber;
    custom_interfaces::msg::Config most_recent_config_report;


    void SetUp() override {
        if (!rclcpp::ok()) {
            rclcpp::init(0, nullptr);
        }

        // Create pipe for mock serial communication
        // pipe_fds[0] = read end, pipe_fds[1] = write end
        if (pipe(node_read_pipe_fds) == -1 || pipe(node_write_pipe_fds) == -1) {
            FAIL() << "Failed to create pipes for mock serial";
        }

        msg_count = 0;
        active_heartbeat = false;
    }

    void TearDown() override {
        node.reset();
        close(node_read_pipe_fds[0]);
        close(node_read_pipe_fds[1]);
        close(node_write_pipe_fds[0]);
        close(node_write_pipe_fds[1]);
    }

    void create_node(){
        // We supply the DVL with both a read and write pipe. We can write to node_read_pipe_fds[1] for the 
        // DVL to read from it, and read from node_write_pipe_fds[0] to verify that the DVL wrote correct data.
        std::unique_ptr<FD_Interface> pipe_fd = std::make_unique<Direct_FD>(node_read_pipe_fds[0], node_write_pipe_fds[1]);
        node = std::make_shared<DVL>(std::move(pipe_fd));
    }

    void create_reset_fail_node(){
        struct termios tty;
        int dev_fd = open("/dev/null", O_RDWR);
        tcgetattr(dev_fd, &tty);
        tty.c_lflag = 0; // non-canonical mode
        tcsetattr(dev_fd, TCSANOW, &tty);

        std::unique_ptr<FD_Interface> dev_null_fd = std::make_unique<Direct_FD>(dev_fd, node_write_pipe_fds[1]);
        node = std::make_shared<DVL>(std::move(dev_null_fd));
    }

    void close_dev_fd(){
        close(dev_fd);
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
        FD_SET(node_write_pipe_fds[0], &readfds);
        
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = (timeout_ms % 1000) * 1000;
        
        int result = select(node_write_pipe_fds[0] + 1, &readfds, nullptr, nullptr, &timeout);
        
        if (result > 0) {
            ssize_t bytes_read = read(node_write_pipe_fds[0], buffer, sizeof(buffer) - 1);
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
    void write_serial_message(uint8_t data_type){
        std::string msg;
        get_serial_message(data_type, msg);
        int length = msg.size();
        write(node_read_pipe_fds[1], msg.c_str(), length + 1);
    }

    /**
    * @brief Helper to generate serial messages per data type
    * 
    * @param data_type The data type of the serial message
    * @param msg The message
    */
    void get_serial_message(uint8_t data_type, std::string& msg){
        switch(data_type){
        case VR_TYPE:
            msg = 
                "wrz,1.000000,2.000000,3.000000," //vx, vy, vz
                "n,2.000000,1.000000," //valid, altitude, fom
                "1.000000;2.000000;3.000000;4.000000;5.000000;" //covariance
                "1.000000;2.000000;3.000000;4.000000," //covariance
                "1,2,3.000000,0\r\n\r"; //time of validity, time of transmission, time, status
            break;
        case BAD_VR_TYPE:
            msg = 
                "wrz,1.000000,2.000000,3.000000," //vx, vy, vz
                "n,2.000000,1.000000," //valid, altitude, fom
                "1.000000;2.000000;3.000000;4.000000;5.000000;" //covariance
                "1.000000;2.000000;3.000000;," //covariance missing 1 field
                "1,2,3.000000,0\r\n\r"; //time of validity, time of transmission, time, status
            break;
        case DRR_TYPE:
            msg =
                "wrp,1716814976.000000,0.110000,0.280000,0.040000," //timestamp, x, y, z
                "15.400000,-1.100000,-0.300000,18.800000, 0\r\n\r"; //pos_std, roll, pitch, yaw, status
            break;
        case CONFIG_TYPE:
            msg =
                "wrc,1475.000000,0.000000,y," //speed of sound, mounting rotation offset, acoustic enabled
                "n,auto,y\r\n\r"; //dark mode enabled, range mode, periodic cycling enabled
            break;
        case ACK_TYPE:
            msg =  
                "wra\r\n\r";
            break;
        default: 
            msg = "error";
        }
    }

    void subscribe_velocity_report() {
        velocity_report_subscriber = node->create_subscription<custom_interfaces::msg::VR> ("velocity_report", 10, std::bind(&TestDVLInterface::velocity_report_callback, this, std::placeholders::_1));
    }

    void velocity_report_callback(custom_interfaces::msg::VR velocity_report) {
        most_recent_velocity_report = velocity_report;    
        msg_count++;
    }

    void subscribe_drr_report() {
        drr_report_subscriber = node->create_subscription<custom_interfaces::msg::DRR>("dead_reck_report", 10, std::bind(&TestDVLInterface::drr_report_callback, this, std::placeholders::_1));
    }

    void drr_report_callback(custom_interfaces::msg::DRR drr_report){
        most_recent_drr_report = drr_report;
    }

    void subscribe_config_report() {
        config_report_subscriber = node->create_subscription<custom_interfaces::msg::Config>("config", 10, std::bind(&TestDVLInterface::config_report_callback, this, std::placeholders::_1));
    }

    void config_report_callback(custom_interfaces::msg::Config config_report){
        most_recent_config_report = config_report;
    }

    /*
    *   Helper function to subscribe to dvl_heartbeat topic and get the heartbeats from dvl
    */
    void subscribe_dvl_heartbeat() {
        dvl_heartbeat_subscriber = node->create_subscription<std_msgs::msg::Empty>("dvl_heartbeat", 10, std::bind(&TestDVLInterface::dvl_heartbeat_callback, this, std::placeholders::_1));
    }
    
    /*
    *   Helper function for the callback for dvl heartbeat
    */
    void dvl_heartbeat_callback(std_msgs::msg::Empty msg) {
        active_heartbeat = true;
        (void) msg;
    }
};

/**
 * @brief Test node construction
 */
TEST_F(TestDVLInterface, DVLConstruction) {
    ASSERT_NO_THROW({
        create_node();
    });

    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->get_name(), std::string("dvl"));
}

/**
 * @brief Test DVL reading Velocity
 */
 TEST_F(TestDVLInterface, ValidReadVelocityReport){
    create_node();
    velocity_report vr;

    //write valid VR data into pipe_fds[1] 
    write_serial_message(VR_TYPE);

    //check if vr was read correctly
    vr = node->readVelocityReport();
    EXPECT_FLOAT_EQ(vr.vx, 1.000000);
    EXPECT_FLOAT_EQ(vr.covariance[3], 4.000000);
    EXPECT_FLOAT_EQ(vr.altitude, 2.000000);
    EXPECT_FLOAT_EQ(vr.fom, 1.000000);
    EXPECT_FLOAT_EQ(vr.time, 3.000000);
    EXPECT_EQ(vr.time_of_validity, 1);
    EXPECT_EQ(vr.time_of_transmission, 2);
    EXPECT_EQ(vr.status, 0);
    EXPECT_EQ(vr.valid, 'n'); 
 }

 /**
 * @brief Test DVL reading a bad velocity report, ie missing covariance values
 */
 TEST_F(TestDVLInterface, BadVelocityReport){
    create_node();
    velocity_report vr;

    //write bad VR data into pipe_fds[1] 
    write_serial_message(BAD_VR_TYPE);
    ASSERT_NE(node, nullptr);
    // vr should equal error_vr, which is an empty initialization 
    // of velocity_report if bad data is passed in
    EXPECT_FLOAT_EQ(vr.vx, 0);
    EXPECT_FLOAT_EQ(vr.covariance[3], 0);
    EXPECT_FLOAT_EQ(vr.altitude, 0);
    EXPECT_FLOAT_EQ(vr.fom, 0);
    EXPECT_FLOAT_EQ(vr.time, 0);
    EXPECT_EQ(vr.time_of_validity, 0);
    EXPECT_EQ(vr.time_of_transmission, 0);
    EXPECT_EQ(vr.status, 0);
 }

 /**
 * @brief Test DVL reading DRReport
 */
 TEST_F(TestDVLInterface, ValidReadDRReport){
    create_node();
    dead_reck_report drr;

    //write valid DRR data into pipe_fds[1]
    write_serial_message(DRR_TYPE);

    //check if drr was read correctly
    drr = node->readDRReport();
    EXPECT_EQ(drr.time_stamp, 1716814976.000000); 
    EXPECT_FLOAT_EQ(drr.x, 0.110000);
    EXPECT_FLOAT_EQ(drr.roll, -1.100000);
    EXPECT_EQ(drr.status, 0);
 }

 /**
 * @brief Test DVL reading Config
 */
 TEST_F(TestDVLInterface, ValidReadConfiguration){
    create_node();
    config_report config;

    //write valid Config data into pipe_fds[1]
    write_serial_message(CONFIG_TYPE);

    //check if config was read correctly
    config = node->readConfig();
    EXPECT_FLOAT_EQ(config.speed_of_sound, 1475.000000);
    EXPECT_FLOAT_EQ(config.mounting_rotation_offset, 0.000000);
    EXPECT_EQ(config.acoustic_enabled, "y");
    EXPECT_EQ(config.dark_mode_enabled, "n");
    EXPECT_EQ(config.range_mode, "auto");
    EXPECT_EQ(config.periodic_cycling_enabled, "y");
 }

 /**
 * @brief Test DVL publishing VR
 */
 TEST_F(TestDVLInterface, DVLPublishVR) {
    create_node();

    //write valid VR data into pipe_fds[1] 
    write_serial_message(VR_TYPE);

    //subscribe to VR node
    subscribe_velocity_report();

    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(node);

    char cmd = node->getCommandFromSerial();
    node->publishCommandFromSerial(cmd);
    exec.spin_some();

    EXPECT_EQ(most_recent_velocity_report.velocity_data.x, 1.000000);
    EXPECT_EQ(most_recent_velocity_report.velocity_data.y, 2.000000);
    EXPECT_EQ(most_recent_velocity_report.velocity_data.z, 3.000000);
    
    EXPECT_EQ((most_recent_velocity_report.covariance.data)[0], 1.000000);
    EXPECT_EQ((most_recent_velocity_report.covariance.data)[1], 2.000000);
    EXPECT_EQ((most_recent_velocity_report.covariance.data)[2], 3.000000);
    EXPECT_EQ((most_recent_velocity_report.covariance.data)[3], 4.000000);
    EXPECT_EQ((most_recent_velocity_report.covariance.data)[4], 5.000000);
    EXPECT_EQ((most_recent_velocity_report.covariance.data)[5], 1.000000);
    EXPECT_EQ((most_recent_velocity_report.covariance.data)[6], 2.000000);
    EXPECT_EQ((most_recent_velocity_report.covariance.data)[7], 3.000000);
    EXPECT_EQ((most_recent_velocity_report.covariance.data)[8], 4.000000);
    EXPECT_EQ(most_recent_velocity_report.altitude, 2.000000);
    EXPECT_EQ(most_recent_velocity_report.fom, 1.000000);
    EXPECT_EQ(most_recent_velocity_report.time, 3);
    EXPECT_EQ(most_recent_velocity_report.time_of_validity, 1);
    EXPECT_EQ(most_recent_velocity_report.time_of_transmission, 2);
    EXPECT_EQ(most_recent_velocity_report.status, 0);
    EXPECT_EQ(most_recent_velocity_report.valid, 'n');
 }

 /**
 * @brief Test DVL publishing bad VR
 */
 TEST_F(TestDVLInterface, DVLPublishBadVR) {
    create_node();

    //write bad VR data into pipe_fds[1] 
    write_serial_message(BAD_VR_TYPE);

    //subscribe to VR node
    subscribe_velocity_report();

    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(node);

    // should not get anything from topic
    char cmd = node->getCommandFromSerial();
    node->publishCommandFromSerial(cmd);
    exec.spin_some();
    
    EXPECT_EQ(msg_count, 0);
 }

  /**
 * @brief Test DVL publishing DRR
 */
 TEST_F(TestDVLInterface, DVLPublishDRR){
    create_node();

    //write valid DRR data into pipe_fds[1]
    write_serial_message(DRR_TYPE);

    //subscribe to DRR node
    subscribe_drr_report();

    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(node);

    char cmd = node->getCommandFromSerial();
    node->publishCommandFromSerial(cmd);
    exec.spin_some();

    EXPECT_EQ(most_recent_drr_report.time_stamp, 1716814976.000000);
    EXPECT_FLOAT_EQ(most_recent_drr_report.position.x, 0.110000);
    EXPECT_FLOAT_EQ(most_recent_drr_report.position.y, 0.280000);
    EXPECT_FLOAT_EQ(most_recent_drr_report.position.z, 0.040000);
    EXPECT_FLOAT_EQ(most_recent_drr_report.pos_std, 15.400000);
    EXPECT_FLOAT_EQ(most_recent_drr_report.angle.x, -1.100000);
    EXPECT_FLOAT_EQ(most_recent_drr_report.angle.y, -0.300000);
    EXPECT_FLOAT_EQ(most_recent_drr_report.angle.z, 18.800000);
    EXPECT_EQ(most_recent_drr_report.status, 0);
 }

  /**
 * @brief Test DVL publishing Config
 */
 TEST_F(TestDVLInterface, DVLPublishConfig){
    create_node();

    //write valid Config data into pipe_fds[1] 
    write_serial_message(CONFIG_TYPE);

    //subscribe to Config node
    subscribe_config_report();

    rclcpp::executors::SingleThreadedExecutor exec;
    exec.add_node(node);
    
    char cmd = node->getCommandFromSerial();
    node->publishCommandFromSerial(cmd);
    exec.spin_some();

    EXPECT_FLOAT_EQ(most_recent_config_report.speed_of_sound, 1475.000000);
    EXPECT_FLOAT_EQ(most_recent_config_report.mounting_rotation_offset, 0.000000);
    EXPECT_EQ(most_recent_config_report.acoustic_enabled, "y");
    EXPECT_EQ(most_recent_config_report.dark_mode_enabled, "n");
    EXPECT_EQ(most_recent_config_report.range_mode, "auto");
    EXPECT_EQ(most_recent_config_report.periodic_cycling_enabled, "y");
 }

 /**
 * @brief Test DVL successfully resetting DRR
 */
 TEST_F(TestDVLInterface, DVLResetDRRSuccess){
    create_node();
    auto client = node->create_client<std_srvs::srv::Trigger>("reset_drr");
    ASSERT_TRUE(client->wait_for_service(std::chrono::seconds(1)));

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);

    std::thread spin_thread([&executor]() {
        executor.spin();
    });

    std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();    
    auto future = client->async_send_request(request);

    write_serial_message(ACK_TYPE);

    ASSERT_EQ (future.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    EXPECT_TRUE(future.get()->success); // request should be recieved by service   
    executor.cancel();
    spin_thread.join();
 }

 /**
 * @brief Test DVL successfully resetting GYRO
 */
 TEST_F(TestDVLInterface, DVLResetGyroSuccess){
    create_node();
    auto client = node->create_client<std_srvs::srv::Trigger>("reset_gyro");
    ASSERT_TRUE(client->wait_for_service(std::chrono::seconds(1)));

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);

    std::thread spin_thread([&executor]() {
        executor.spin();
    });

    std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();    
    auto future = client->async_send_request(request);

    write_serial_message(ACK_TYPE);

    ASSERT_EQ (future.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    EXPECT_TRUE(future.get()->success); // request should be recieved by service   
    executor.cancel();
    spin_thread.join();
 }

 /**
 * @brief Test DVL resetting DRR with no acknowledgement
 */
 TEST_F(TestDVLInterface, DVLResetDRRFail){
    create_reset_fail_node();
    auto client = node->create_client<std_srvs::srv::Trigger>("reset_drr");
    ASSERT_TRUE(client->wait_for_service(std::chrono::seconds(1)));

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);

    std::thread spin_thread([&executor]() {
        executor.spin();
    });

    std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();    
    auto future = client->async_send_request(request);
    ASSERT_EQ(future.wait_for(std::chrono::seconds(16)), std::future_status::ready);
    EXPECT_FALSE(future.get()->success); // request should be recieved by service   
    executor.cancel();
    spin_thread.join();
    close_dev_fd();
 }

 /**
 * @brief Test DVL resetting GYRO with no acknowledgement
 */
 TEST_F(TestDVLInterface, DVLResetGyroFail){
    create_reset_fail_node();
    auto client = node->create_client<std_srvs::srv::Trigger>("reset_gyro");
    ASSERT_TRUE(client->wait_for_service(std::chrono::seconds(1)));

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);

    std::thread spin_thread([&executor]() {
        executor.spin();
    });

    std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();    
    auto future = client->async_send_request(request);
    ASSERT_EQ(future.wait_for(std::chrono::seconds(16)), std::future_status::ready);
    EXPECT_FALSE(future.get()->success); // request should be recieved by service   
    executor.cancel();
    spin_thread.join();
    close_dev_fd();
 }

  /**
 * @brief Test that DVL publishes a heartbeat
 */
 TEST_F(TestDVLInterface, DVLHeartbeat) {
    create_node();
    subscribe_dvl_heartbeat();

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


 #ifdef ENABLE_TESTING
    int main(int argc, char** argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
#endif

 