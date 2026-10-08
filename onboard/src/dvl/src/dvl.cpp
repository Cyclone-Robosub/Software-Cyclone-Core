#include <dvl.hpp>
#include <iostream>

using namespace std::chrono_literals;

namespace dvl {

    // Constructor
    DVL::DVL(std::unique_ptr<FD_Interface> fd) : 
        rclcpp::Node("dvl"),
        fd(std::move(fd)),
        error_config(config_report{0.0, 0.0, "x", "x", "x", "x"}),
        config(error_config)
        {

        if (this->fd->get_read_fd() < 0 || this->fd->get_write_fd() < 0) {
            RCLCPP_WARN(this->get_logger(), "Unable to connect to DVL.");
            exit(42);
        }

        //Services
        config_service = this->create_service<custom_interfaces::srv::SetConfig>("set_config", std::bind(&dvl::DVL::setConfig, this, std::placeholders::_1, std::placeholders::_2));
        drr_service = this->create_service<std_srvs::srv::Trigger>("reset_drr", std::bind(&dvl::DVL::resetDRR, this, std::placeholders::_1, std::placeholders::_2));
        gyro_service = this->create_service<std_srvs::srv::Trigger>("reset_gyro", std::bind(&dvl::DVL::resetGyro, this, std::placeholders::_1, std::placeholders::_2));
        trigger_ping = this->create_service<std_srvs::srv::Trigger>("triggerPing", std::bind(&dvl::DVL::triggerPing, this, std::placeholders::_1, std::placeholders::_2));

        //Publishers
        velocity_report_publisher = this->create_publisher<custom_interfaces::msg::VR>("velocity_report", 10);
        drr_report_publisher = this->create_publisher<custom_interfaces::msg::DRR>("dead_reck_report", 10);
        config_publisher = this->create_publisher<custom_interfaces::msg::Config>("config", 10);
        heartbeat_publisher = this->create_publisher<std_msgs::msg::Empty>("dvl_heartbeat", 10);

        heartbeat_timer = this->create_wall_timer(500ms,
            std::bind(&DVL::publish_heartbeat_callback, this)); // 60 hz
    }

    // Ros2 publish functions
    void DVL::publishVR() {
        custom_interfaces::msg::VR vrMessage;
        vrMessage.velocity_data.x = vr.vx;
        vrMessage.velocity_data.y = vr.vy;
        vrMessage.velocity_data.z = vr.vz;
        for (int i = 0; i < 9; i++) {
            (vrMessage.covariance.data).push_back((vr.covariance)[i]);
        }
        vrMessage.altitude = vr.altitude;
        vrMessage.fom = vr.fom;
        vrMessage.time = vr.time;
        vrMessage.time_of_validity = vr.time_of_validity;
        vrMessage.time_of_transmission = vr.time_of_transmission;
        vrMessage.status = vr.status;
        vrMessage.valid = vr.valid;
        this->velocity_report_publisher->publish(vrMessage);
    }

    void DVL::publishDRR() {
        custom_interfaces::msg::DRR drrMessage;
        
        drrMessage.time_stamp = drr.time_stamp;
        drrMessage.position.x = drr.x;
        drrMessage.position.y = drr.y;
        drrMessage.position.z = drr.z;

        drrMessage.angle.x = drr.roll;
        drrMessage.angle.y = drr.pitch;
        drrMessage.angle.z = drr.yaw;
        drrMessage.pos_std = drr.pos_std;
        drrMessage.status = drr.status;
       
        this->drr_report_publisher->publish(drrMessage);
    }

    void DVL::publishConfig() {
        custom_interfaces::msg::Config configMessage;
        configMessage.speed_of_sound = config.speed_of_sound;
        configMessage.mounting_rotation_offset = config.mounting_rotation_offset;
        configMessage.acoustic_enabled= config.acoustic_enabled;
        configMessage.dark_mode_enabled = config.dark_mode_enabled;
        configMessage.range_mode = config.range_mode;
        configMessage.periodic_cycling_enabled = config.periodic_cycling_enabled;
        this->config_publisher->publish(configMessage);
    }
    
    // Public Reads
    velocity_report DVL::readVelocityReport(){
        if(getResponse(REC_VR)){
            return vr;
        } else {
            RCLCPP_WARN(this->get_logger(), "Returning error_vr");
            return error_vr;
        }
        
    }

    dead_reck_report DVL::readDRReport(){
        if(getResponse(REC_DRR)){
            return drr;
        } else {
            return error_drr;
        }
    }

    config_report DVL::readConfig(){
        bool success = sendCommand(CMD_GET_SETTINGS); 
        if (success){
            return config;
        } else {
            return error_config;
        }
    }

    // Public Writes
    void DVL::setConfig(const std::shared_ptr<custom_interfaces::srv::SetConfig::Request> request, const std::shared_ptr<custom_interfaces::srv::SetConfig::Response> response) { /*float speed_of_sound, float mounting_rotation_offset, std::string acoustic_enabled, std::string dark_mode_enabled, std::string range_mode, std::string periodic_cycling_enabled*/
        response->success = sendCommand(CMD_SET_SETTINGS, 
            {
                std::to_string(request->config_data.speed_of_sound),
                std::to_string(request->config_data.mounting_rotation_offset), 
                request->config_data.acoustic_enabled, 
                request->config_data.range_mode, 
                request->config_data.periodic_cycling_enabled
            });
    }

    void DVL::resetDRR(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, const std::shared_ptr<std_srvs::srv::Trigger::Response> response){
        bool success = sendCommand(CMD_RESET_DR);
        response->success = success; 
        (void) request;     
    }

    void DVL::resetGyro(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, const std::shared_ptr<std_srvs::srv::Trigger::Response> response){
        bool success = sendCommand(CMD_CALIBRATE_GYRO);
        response->success = success;     
        (void) request; 
    }

    void DVL::triggerPing(const std::shared_ptr<std_srvs::srv::Trigger::Request> request, const std::shared_ptr<std_srvs::srv::Trigger::Response> response){
        bool success = sendCommand(CMD_TRIGGER_PING);
        response->success = success;
        (void) request;
    }

    void DVL::publish_heartbeat_callback() {
        std_msgs::msg::Empty msg;
        heartbeat_publisher->publish(msg);
    }

    //Private Methods
    char DVL::getCommandFromSerial(){
        std::string curr_line = "";
        ssize_t n = -1;
        char c = '\0';
        
        while(curr_line.size() < 3){ // keep reading until curr_line is length 3
            n = read(fd->get_read_fd(), &c, 1); // read 1 byte from the serial port
            if (n == 1) {
                if(curr_line.size() == 0 && c != 'w') continue; // keep reading w/o appending, until the start of a data sequence is reached
                curr_line += c; // append to the end of the existing string
            } else if (n <= 0) {
                RCLCPP_WARN(this->get_logger(), "Failed to read from serial port. Attempting to reconnect to DVL.");
                fd->attempt_reconnect();
                return '0';
            }
        }

        return (curr_line[0] == 'w') ? curr_line[2] : '0'; //checks if curr_line is a command 
    }

    char DVL::getCommandFromSerial(std::chrono::time_point<std::chrono::steady_clock> start, std::chrono::milliseconds TIMEOUT){
        using clock = std::chrono::steady_clock;
        std::string curr_line = "";
        ssize_t n = -1;
        char c = '\0';
        
        while(curr_line.size() < 3 && clock::now() - start < TIMEOUT){ //keep reading until curr_line is length 3 or times out
            n = read(fd->get_read_fd(), &c, 1); 
            if (n == 1) {
                if(curr_line.size() == 0 && c != 'w') continue; 
                curr_line += c; 
            } else if (n < 0) {
                RCLCPP_WARN(this->get_logger(), "Failed to read from serial port. Attempting to reconnect to DVL.");
                fd->attempt_reconnect();
                return '0';
            }
        }

        if(curr_line.empty()) return '0';
        return (curr_line[0] == 'w') ? curr_line[2] : '0'; 
    }

    void DVL::publishCommandFromSerial(char cmd){
        if(getResponse(cmd)){
            switch(cmd){
                case 'c': // config
                    publishConfig();
                    break;
                case 'z': // velocity report
                    publishVR();
                    break;
                case 'p': // dead reckoning report
                    publishDRR();
                    break;
                default:
                    break;
            }   
        }
    }
    
    bool DVL::getResponse(const char expected_response){
        std::string complete_line = "";
        char c;
        ssize_t n = read(fd->get_read_fd(), &c, 1); // read 1 byte from the serial port
        if(n <= 0){
            RCLCPP_WARN(this->get_logger(), "Failed to read from serial port. Attempting to reconnect to DVL.");
            fd->attempt_reconnect();
            return false;
        }

        if(c != 'w'){
            complete_line += 'w';
            complete_line += 'r';
            complete_line += expected_response;
        }
        
        while(n > 0){
            std::string partial_line = "";
            
            if (n == 1) {
                partial_line += c; // append to the end of the existing string
            }

            if (partial_line.empty()) {
                continue; // loop again if the the partial line is empty
            }

            complete_line += partial_line; // add the partial line to the complete line

            if (partial_line == "\n" || partial_line == "\r") {
                break; // break out of the reading loop if an end-of-line char is detected
            }

            n = read(fd->get_read_fd(), &c, 1); // read 1 byte from the serial port
            if (n <= 0) {
                RCLCPP_WARN(this->get_logger(), "Failed to read from serial port. Attempting to reconnect to DVL.");
                fd->attempt_reconnect();
                return false;
            }
        }

        bool parse_is_success;

        try {
            parse_is_success = parseResponse(complete_line);
        } catch (const std::exception& e) {
            parse_is_success = false;
        }
        

        return parse_is_success && complete_line[2] == expected_response;
    }

    bool DVL::parseResponse(std::string& complete_line){

        // Make sure we have at least 3 characters to safely access complete_line[2]
        if (complete_line.size() < 3) return false;

        char cmd = complete_line[2];

        // Strip leading/trailing whitespace and newlines
        complete_line.erase(0, complete_line.find_first_not_of(" \t\r\n"));
        complete_line.erase(complete_line.find_last_not_of(" \t\r\n") + 1);

        // Strip checksum "*xx" if present
        if (complete_line.size() >= 3 && complete_line[complete_line.size() - 3] == '*') {
            complete_line.erase(complete_line.size() - 3);
        }

        switch (cmd) {
            case 'a': // acknowledge
                return true;

            case 'c': { // configuration
                std::stringstream ss(complete_line);
                std::string field;
                std::vector<std::string> fields;
                while (std::getline(ss, field, ',') && !field.empty()) fields.push_back(field);

                if (fields.size() < 7) {
                    return false;
                }

                config.speed_of_sound = std::stof(fields[1]);
                config.mounting_rotation_offset = std::stof(fields[2]);
                config.acoustic_enabled = fields[3];
                config.dark_mode_enabled = fields[4];
                config.range_mode = fields[5];
                config.periodic_cycling_enabled = fields[6];

                return true;
            }

            case 'z': { // velocity report
                std::stringstream ss(complete_line);
                std::string field;
                std::vector<std::string> fields;
                while (std::getline(ss, field, ',') && !field.empty()) {
                    fields.push_back(field);
                }

                std::stringstream ss_covariance(fields[7]);
                std::vector<std::string> covariance_fields;
                while(std::getline(ss_covariance, field, ';') && !field.empty()) {
                    covariance_fields.push_back(field);
                }

                if (fields.size() < 10) {
                    return false;
                }
                if (covariance_fields.size() < 9){
                    return false;
                }

                vr.vx = std::stof(fields[1]);
                vr.vy = std::stof(fields[2]);
                vr.vz = std::stof(fields[3]);
                vr.valid = fields[4][0];
                vr.altitude = std::stof(fields[5]);
                vr.fom = std::stof(fields[6]);

                for (int i = 0; i < 9; ++i) {
                    vr.covariance[i] = std::stof(covariance_fields[i]);
                }

                vr.time_of_validity = std::stoll(fields[8]);
                vr.time_of_transmission = std::stoll(fields[9]);
                vr.time = std::stof(fields[10]);
                vr.status = static_cast<uint8_t>(std::stoul(fields[11], nullptr, 10));
                
                return true;
            }

            case 'p': { // dead reckoning report
                std::stringstream ss(complete_line);
                std::string field;
                std::vector<std::string> fields;
                while (std::getline(ss, field, ',')) fields.push_back(field);

                if (fields.size() < 9) return false;

                drr.time_stamp = std::stof(fields[1]);
                drr.x = std::stof(fields[2]);
                drr.y = std::stof(fields[3]);
                drr.z = std::stof(fields[4]);
                drr.pos_std = std::stof(fields[5]);
                drr.roll = std::stof(fields[6]);
                drr.pitch = std::stof(fields[7]);
                drr.yaw = std::stof(fields[8]);
                drr.status = static_cast<uint8_t>(std::stoul(fields[9], nullptr, 10));

                return true;
            }

            case '?': // malformed request
            case '!': // bad checksum
            case 'n': // not acknowledged
                return false;

            default:
                return false;
        }
        return false;
    }

    bool DVL::sendCommand(uint8_t cmd, const std::vector<std::string>& options) { // cmd with optional input args
        dvl_mutex.lock();
        std::stringstream msg;

        // Build message

        msg << SOP << DIR_CMD << static_cast<char>(cmd);  // add start character, direction, and command

        for (const auto& opt : options) {               // add options as comma-separated
            msg << "," << opt;
        }

        // Compute checksum (CRC-8)
        std::string body = msg.str();
        uint8_t crc = crc8(reinterpret_cast<uint8_t*>(body.data()), body.size());

        msg << CS << std::hex << std::setw(2) << std::setfill('0') << (int)crc << "\n";

        // Write to serial port using POSIX write
        std::string data = msg.str();
        ssize_t n = write(fd->get_write_fd(), data.c_str(), data.size());
        if (n <= 0) {
            fd->attempt_reconnect();
            RCLCPP_WARN(this->get_logger(), "Failed to write to serial port. Attempting to reconnect to DVL.");
            dvl_mutex.unlock();
            return false;
        }
        bool success = false;
        if (cmd == CMD_GET_SETTINGS) success = getResponse(cmd); // if setting config, check serial has published config
        else { // else wait 15 seconds for an acknowledgement
            using clock = std::chrono::steady_clock;
            constexpr auto TIMEOUT = std::chrono::milliseconds(15000);
            auto start = clock::now();
            while(clock::now() - start < TIMEOUT){
                cmd = getCommandFromSerial(start, TIMEOUT);
                if(cmd == ACK){
                    success = true;
                    break;
                }
            }
        }
        dvl_mutex.unlock();
        return success;
    }

    void DVL::workLoop() {
        if(!sendCommand(CMD_RESET_DR)) RCLCPP_WARN(this->get_logger(), "Error with DR reset");
       
        while (rclcpp::ok()) {
            dvl_mutex.lock();
            char cmd = getCommandFromSerial();
            dvl_mutex.unlock();
            if(cmd != '0') publishCommandFromSerial(cmd);
        }    
    }

    static const uint8_t lookup_table[256] = {
        0x00U,0x07U,0x0EU,0x09U,0x1CU,0x1BU,0x12U,0x15U,
        0x38U,0x3FU,0x36U,0x31U,0x24U,0x23U,0x2AU,0x2DU,
        0x70U,0x77U,0x7EU,0x79U,0x6CU,0x6BU,0x62U,0x65U,
        0x48U,0x4FU,0x46U,0x41U,0x54U,0x53U,0x5AU,0x5DU,
        0xE0U,0xE7U,0xEEU,0xE9U,0xFCU,0xFBU,0xF2U,0xF5U,
        0xD8U,0xDFU,0xD6U,0xD1U,0xC4U,0xC3U,0xCAU,0xCDU,
        0x90U,0x97U,0x9EU,0x99U,0x8CU,0x8BU,0x82U,0x85U,
        0xA8U,0xAFU,0xA6U,0xA1U,0xB4U,0xB3U,0xBAU,0xBDU,
        0xC7U,0xC0U,0xC9U,0xCEU,0xDBU,0xDCU,0xD5U,0xD2U,
        0xFFU,0xF8U,0xF1U,0xF6U,0xE3U,0xE4U,0xEDU,0xEAU,
        0xB7U,0xB0U,0xB9U,0xBEU,0xABU,0xACU,0xA5U,0xA2U,
        0x8FU,0x88U,0x81U,0x86U,0x93U,0x94U,0x9DU,0x9AU,
        0x27U,0x20U,0x29U,0x2EU,0x3BU,0x3CU,0x35U,0x32U,
        0x1FU,0x18U,0x11U,0x16U,0x03U,0x04U,0x0DU,0x0AU,
        0x57U,0x50U,0x59U,0x5EU,0x4BU,0x4CU,0x45U,0x42U,
        0x6FU,0x68U,0x61U,0x66U,0x73U,0x74U,0x7DU,0x7AU,
        0x89U,0x8EU,0x87U,0x80U,0x95U,0x92U,0x9BU,0x9CU,
        0xB1U,0xB6U,0xBFU,0xB8U,0xADU,0xAAU,0xA3U,0xA4U,
        0xF9U,0xFEU,0xF7U,0xF0U,0xE5U,0xE2U,0xEBU,0xECU,
        0xC1U,0xC6U,0xCFU,0xC8U,0xDDU,0xDAU,0xD3U,0xD4U,
        0x69U,0x6EU,0x67U,0x60U,0x75U,0x72U,0x7BU,0x7CU,
        0x51U,0x56U,0x5FU,0x58U,0x4DU,0x4AU,0x43U,0x44U,
        0x19U,0x1EU,0x17U,0x10U,0x05U,0x02U,0x0BU,0x0CU,
        0x21U,0x26U,0x2FU,0x28U,0x3DU,0x3AU,0x33U,0x34U,
        0x4EU,0x49U,0x40U,0x47U,0x52U,0x55U,0x5CU,0x5BU,
        0x76U,0x71U,0x78U,0x7FU,0x6AU,0x6DU,0x64U,0x63U,
        0x3EU,0x39U,0x30U,0x37U,0x22U,0x25U,0x2CU,0x2BU,
        0x06U,0x01U,0x08U,0x0FU,0x1AU,0x1DU,0x14U,0x13U,
        0xAEU,0xA9U,0xA0U,0xA7U,0xB2U,0xB5U,0xBCU,0xBBU,
        0x96U,0x91U,0x98U,0x9FU,0x8AU,0x8DU,0x84U,0x83U,
        0xDEU,0xD9U,0xD0U,0xD7U,0xC2U,0xC5U,0xCCU,0xCBU,
        0xE6U,0xE1U,0xE8U,0xEFU,0xFAU,0xFDU,0xF4U,0xF3U,
    };

    uint8_t crc8(uint8_t *message, int message_length) {
        uint8_t checksum = 0;
        while (message_length > 0) {
            checksum = lookup_table[*message ^ checksum];
            message++;
            message_length--;
        }
        return checksum;
    }
} //namespace

DVL_FD::DVL_FD(std::string path) : Path_FD(path) {
    fd = open_file();
}

int DVL_FD::open_file() {
    struct termios tty;
    speed_t baud = 0010002; // This means baud rate of 115200
    int fd;
    
    if ((fd = open(path.c_str(), O_RDWR | O_NOCTTY | O_SYNC)) == -1) {
        return -1;
    }
    
    if (tcgetattr(fd, &tty) != 0) {
        close(fd);
        fprintf(stderr, "Failed to get terminal attributes\n");
        return -1;
    }

    
    // Get and modify current options:
    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);
    
    // Configure 8N1, no flow control
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 bits
    tty.c_cflag &= ~PARENB; // no parity
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    tty.c_cflag &= ~CRTSCTS; // no hardware flow control
    tty.c_cflag |= CLOCAL | CREAD; // enable receiver

    tty.c_lflag = 0; // non-canonical mode
    tty.c_oflag = 0; // no remapping, no delays
    tty.c_iflag = 0; // no special handling

    tty.c_cc[VMIN] = 0;  // non-blocking read
    tty.c_cc[VTIME] = 10; // 1 second timeout (VTIME is in deciseconds)
    
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        close(fd);
        fprintf(stderr, "Failed to set terminal attributes\n");
        return -1;
    }
    
    return fd;
}

#ifndef ENABLE_TESTING
    int main(int argc, char* argv[]) {
        rclcpp::init(argc, argv);
        std::unique_ptr<FD_Interface> path_fd = std::make_unique<DVL_FD>("/dev/serial/by-id/usb-FTDI_FT230X_Basic_UART_D30I35JH-if00-port0");
        auto dvl = std::make_shared<dvl::DVL>(std::move(path_fd));
        
        std::thread ros_thread([&]() { 
            rclcpp::spin(dvl);
        });
        
        dvl->workLoop();

        rclcpp::shutdown();
        ros_thread.join();
        return 0;
    }
#endif
