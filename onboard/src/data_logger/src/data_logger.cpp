#include "data_logger.hpp"

using namespace std::chrono_literals;
using namespace rclcpp;
#include <iostream>

// TODO: Add IMU logging in the future

Data_Logger::Data_Logger(std::unique_ptr<FD_Interface> drr_fd, 
                         std::unique_ptr<FD_Interface> vr_fd) : 
    Node("data_logger"),
    drr_fd(std::move(drr_fd)),
    vr_fd(std::move(vr_fd))
    // imu_fd(std::move(imu_fd)) 
     {
    drr_subscription = this->create_subscription<custom_interfaces::msg::DRR>(
        "dead_reck_report", 10, 
        std::bind(&Data_Logger::dead_reck_report_received_callback, this, std::placeholders::_1));
    vr_subscription = this->create_subscription<custom_interfaces::msg::VR>(
        "velocity_report", 10, 
        std::bind(&Data_Logger::velocity_report_received_callback, this, std::placeholders::_1));
    
    std::string drr_header = "time,position_x,position_y,position_z,angle_x,angle_y,angle_z,pos_std\n";
    std::string vr_header = "time,velocity_x,velocity_y,velocity_z,altitude,fom,covariance_1_1,covariance_1_2,covariance_1_3,covariance_2_1,covariance_2_2,covariance_2_3,covariance_3_1,covariance_3_2,covariance_3_3\n";
    // std::string imu_header = "TODO\n";
    write(this->drr_fd->get_write_fd(), drr_header.c_str(), drr_header.length());
    write(this->vr_fd->get_write_fd(), vr_header.c_str(), vr_header.length());
    // write(this->imu_fd->get_write_fd(), imu_header.c_str(), imu_header.length());
}

void Data_Logger::dead_reck_report_received_callback(custom_interfaces::msg::DRR::UniquePtr drr_msg) {
    std::string log_data = Data_Logger::get_current_time();    
    log_data += "," + std::to_string(drr_msg->position.x);
    log_data += "," + std::to_string(drr_msg->position.y);
    log_data += "," + std::to_string(drr_msg->position.z);
    log_data += "," + std::to_string(drr_msg->angle.x);
    log_data += "," + std::to_string(drr_msg->angle.y);
    log_data += "," + std::to_string(drr_msg->angle.z);
    log_data += "," + std::to_string(drr_msg->pos_std);
    log_data += "\n";
    write(this->drr_fd->get_write_fd(), log_data.c_str(), log_data.length());
}

void Data_Logger::velocity_report_received_callback(custom_interfaces::msg::VR::UniquePtr vr_msg) {
    std::string log_data = Data_Logger::get_current_time();    
    log_data += "," + std::to_string(vr_msg->velocity_data.x);
    log_data += "," + std::to_string(vr_msg->velocity_data.y);
    log_data += "," + std::to_string(vr_msg->velocity_data.z);
    log_data += "," + std::to_string(vr_msg->altitude); 
    log_data += "," + std::to_string(vr_msg->fom);
    if (vr_msg->covariance.data.size() < 9) {
        RCLCPP_WARN(this->get_logger(), "Incomplete data for covariance. Look for \"-1\" in the covariance field.");
        for (int i = 0; i < 9; i++) {
            log_data += "," + std::to_string(-1);
        }
    }
    else {
        for (auto cov_data : vr_msg->covariance.data) {
            log_data += "," + std::to_string(cov_data);
        }
    }
    log_data += "\n";
    write(this->vr_fd->get_write_fd(), log_data.c_str(), log_data.length());
}

std::string Data_Logger::get_current_time() {
    char buffer [80];

    std::time_t rawtime;
    std::tm* timeinfo;
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    std::strftime(buffer,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
    std::string cpp_time = buffer;
    return cpp_time;
}

Data_FD::Data_FD(std::string path) : Path_FD(path) {
    fd = open_file();
}

int Data_FD::open_file() {
    if ((fd = open(path.c_str(), O_CREAT | O_RDWR | O_APPEND, 0644)) == -1) {
        return -1;
    }
    return fd;
}

#ifndef ENABLE_TESTING

int main(int argc, char* argv[]) {
    std::string filepath = getenv("HOME");
    filepath += "/Robosub/Data/";
    filepath += Data_Logger::get_current_time();
    std::unique_ptr<FD_Interface> drr_fd = std::make_unique<Data_FD>(filepath + "-dvl-drr.csv");
    std::unique_ptr<FD_Interface> vr_fd = std::make_unique<Data_FD>(filepath + "-dvl-vr.csv");
    // std::unique_ptr<FD_Interface> imu_fd = std::make_unique<Data_FD>(filepath + "-imu.csv");

    rclcpp::init(argc, argv);
    auto data_logger = std::make_shared<Data_Logger>(std::move(drr_fd), std::move(vr_fd));
    rclcpp::spin(data_logger);
    
    rclcpp::shutdown();
    
    return 0;
}
#endif // ENABLE_TESTING

