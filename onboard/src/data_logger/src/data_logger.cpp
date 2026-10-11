#include "data_logger.hpp"

using namespace std::chrono_literals;
using namespace rclcpp;
#include <iostream>
#include <sys/time.h>

Data_Logger::Data_Logger(std::unique_ptr<FD_Interface> drr_fd, 
                         std::unique_ptr<FD_Interface> vr_fd,
                         std::unique_ptr<FD_Interface> imu_fd) : 
    Node("data_logger"),
    drr_fd(std::move(drr_fd)),
    vr_fd(std::move(vr_fd)),
    imu_fd(std::move(imu_fd)) 
     {
    drr_subscription = this->create_subscription<custom_interfaces::msg::DRR>(
        "dead_reck_report", 10, 
        std::bind(&Data_Logger::dead_reck_report_received_callback, this, std::placeholders::_1));
    vr_subscription = this->create_subscription<custom_interfaces::msg::VR>(
        "velocity_report", 10, 
        std::bind(&Data_Logger::velocity_report_received_callback, this, std::placeholders::_1));
    imu_subscription = this->create_subscription<custom_interfaces::msg::Imu>(
        "imu_custom", 10,
        std::bind(&Data_Logger::imu_data_received_callback, this, std::placeholders::_1));
    
    std::string drr_header = "time,position_x,position_y,position_z,angle_x,angle_y,angle_z,pos_std\n";
    std::string vr_header = "time,velocity_x,velocity_y,velocity_z,altitude,fom,covariance_1_1,covariance_1_2,covariance_1_3,covariance_2_1,covariance_2_2,covariance_2_3,covariance_3_1,covariance_3_2,covariance_3_3\n";
    std::string imu_header = "orientation_x,orientation_y,orientation_z,orientation_w,orientation_covariance_c11,orientation_covariance_c12,orientation_covariance_c13,orientation_covariance_c21,orientation_covariance_c22,orientation_covariance_c23,orientation_covariance_c31,orientation_covariance_c32,orientation_covariance_c33,angular_velocity_x,angular_velocity_y,angular_velocity_z,angular_velocity_covariance_c11,angular_velocity_covariance_c12,angular_velocity_covariance_c13,angular_velocity_covariance_c21,angular_velocity_covariance_c22,angular_velocity_covariance_c23,angular_velocity_covariance_c31,angular_velocity_covariance_c32,angular_velocity_covariance_c33,linear_acceleration_x,linear_acceleration_y,linear_acceleration_z,linear_acceleration_covariance_c11,linear_acceleration_covariance_c12,linear_acceleration_covariance_c13,linear_acceleration_covariance_c21,linear_acceleration_covariance_c22,linear_acceleration_covariance_c23,linear_acceleration_covariance_c31,linear_acceleration_covariance_c32,linear_acceleration_covariance_c33,magnetic_field_x,magnetic_field_y,magnetic_field_z,magnetic_field_covariance_c11,magnetic_field_covariance_c12,magnetic_field_covariance_c13,magnetic_field_covariance_c21,magnetic_field_covariance_c22,magnetic_field_covariance_c23,magnetic_field_covariance_c31,magnetic_field_covariance_c32,magnetic_field_covariance_c33,fluid_pressure,fluid_pressure_variance,pose_position_x,pose_position_y,pose_position_z,pose_orientation_x,pose_orientation_y,pose_orientation_z,pose_orientation_w,pose_covariance_c11,pose_covariance_c12,pose_covariance_c13,pose_covariance_c14,pose_covariance_c15,pose_covariance_c16,pose_covariance_c21,pose_covariance_c22,pose_covariance_c23,pose_covariance_c24,pose_covariance_c25,pose_covariance_c26,pose_covariance_c31,pose_covariance_c32,pose_covariance_c33,pose_covariance_c34,pose_covariance_c35,pose_covariance_c36,pose_covariance_c41,pose_covariance_c42,pose_covariance_c43,pose_covariance_c44,pose_covariance_c45,pose_covariance_c46,pose_covariance_c51,pose_covariance_c52,pose_covariance_c53,pose_covariance_c54,pose_covariance_c55,pose_covariance_c56,pose_covariance_c61,pose_covariance_c62,pose_covariance_c63,pose_covariance_c64,pose_covariance_c65,pose_covariance_c66,twist_linear_x,twist_linear_y,twist_linear_z,twist_angular_x,twist_angular_y,twist_angular_z,twist_covariance_c11,twist_covariance_c12,twist_covariance_c13,twist_covariance_c14,twist_covariance_c15,twist_covariance_c16,twist_covariance_c21,twist_covariance_c22,twist_covariance_c23,twist_covariance_c24,twist_covariance_c25,twist_covariance_c26,twist_covariance_c31,twist_covariance_c32,twist_covariance_c33,twist_covariance_c34,twist_covariance_c35,twist_covariance_c36,twist_covariance_c41,twist_covariance_c42,twist_covariance_c43,twist_covariance_c44,twist_covariance_c45,twist_covariance_c46,twist_covariance_c51,twist_covariance_c52,twist_covariance_c53,twist_covariance_c54,twist_covariance_c55,twist_covariance_c56,twist_covariance_c61,twist_covariance_c62,twist_covariance_c63,twist_covariance_c64,twist_covariance_c65,twist_covariance_c66,roll,pitch\n";
    write(this->drr_fd->get_write_fd(), drr_header.c_str(), drr_header.length());
    write(this->vr_fd->get_write_fd(), vr_header.c_str(), vr_header.length());
    write(this->imu_fd->get_write_fd(), imu_header.c_str(), imu_header.length());
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

void Data_Logger::imu_data_received_callback(custom_interfaces::msg::Imu::UniquePtr imu_msg) {
    std::string log_data = Data_Logger::get_current_time();
    log_data += "," + std::to_string(imu_msg->imu_fusion.orientation.x);
    log_data += "," + std::to_string(imu_msg->imu_fusion.orientation.y);
    log_data += "," + std::to_string(imu_msg->imu_fusion.orientation.z);
    log_data += "," + std::to_string(imu_msg->imu_fusion.orientation.w);
    if (imu_msg->imu_fusion.orientation_covariance.size() < 9) {
        RCLCPP_WARN(this->get_logger(), "Incomplete data for covariance. Look for \"-1\" in the covariance field.");
        for (int i = 0; i < 9; i++) {
            log_data += "," + std::to_string(-1);
        }
    }
    else {
        for (auto cov_data : imu_msg->imu_fusion.orientation_covariance) {
            log_data += "," + std::to_string(cov_data);
        }
    }
    log_data += "," + std::to_string(imu_msg->imu_fusion.angular_velocity.x);
    log_data += "," + std::to_string(imu_msg->imu_fusion.angular_velocity.y);
    log_data += "," + std::to_string(imu_msg->imu_fusion.angular_velocity.z);
    if (imu_msg->imu_fusion.angular_velocity_covariance.size() < 9) {
        RCLCPP_WARN(this->get_logger(), "Incomplete data for covariance. Look for \"-1\" in the covariance field.");
        for (int i = 0; i < 9; i++) {
            log_data += "," + std::to_string(-1);
        }
    }
    else {
        for (auto cov_data : imu_msg->imu_fusion.angular_velocity_covariance) {
            log_data += "," + std::to_string(cov_data);
        }
    }
    log_data += "," + std::to_string(imu_msg->imu_fusion.linear_acceleration.x);
    log_data += "," + std::to_string(imu_msg->imu_fusion.linear_acceleration.y);
    log_data += "," + std::to_string(imu_msg->imu_fusion.linear_acceleration.z);
    if (imu_msg->imu_fusion.linear_acceleration_covariance.size() < 9) {
        RCLCPP_WARN(this->get_logger(), "Incomplete data for covariance. Look for \"-1\" in the covariance field.");
        for (int i = 0; i < 9; i++) {
            log_data += "," + std::to_string(-1);
        }
    }
    else {
        for (auto cov_data : imu_msg->imu_fusion.linear_acceleration_covariance) {
            log_data += "," + std::to_string(cov_data);
        }
    }
    log_data += "," + std::to_string(imu_msg->mag_array.magnetic_field.x);
    log_data += "," + std::to_string(imu_msg->mag_array.magnetic_field.y);
    log_data += "," + std::to_string(imu_msg->mag_array.magnetic_field.z);
    if (imu_msg->mag_array.magnetic_field_covariance.size() < 9) {
        RCLCPP_WARN(this->get_logger(), "Incomplete data for covariance. Look for \"-1\" in the covariance field.");
        for (int i = 0; i < 9; i++) {
            log_data += "," + std::to_string(-1);
        }
    }
    else {
        for (auto cov_data : imu_msg->mag_array.magnetic_field_covariance) {
            log_data += "," + std::to_string(cov_data);
        }
    }
    log_data += "," + std::to_string(imu_msg->pressure.fluid_pressure);
    log_data += "," + std::to_string(imu_msg->pressure.variance);
    log_data += "," + std::to_string(imu_msg->ahrs_database.pose.pose.position.x);
    log_data += "," + std::to_string(imu_msg->ahrs_database.pose.pose.position.y);
    log_data += "," + std::to_string(imu_msg->ahrs_database.pose.pose.position.z);
    log_data += "," + std::to_string(imu_msg->ahrs_database.pose.pose.orientation.x);
    log_data += "," + std::to_string(imu_msg->ahrs_database.pose.pose.orientation.y);
    log_data += "," + std::to_string(imu_msg->ahrs_database.pose.pose.orientation.z);
    if (imu_msg->ahrs_database.pose.covariance.size() < 36) {
        RCLCPP_WARN(this->get_logger(), "Incomplete data for covariance. Look for \"-1\" in the covariance field.");
        for (int i = 0; i < 36; i++) {
            log_data += "," + std::to_string(-1);
        }
    }
    else {
        for (auto cov_data : imu_msg->ahrs_database.pose.covariance) {
            log_data += "," + std::to_string(cov_data);
        }
    }
    log_data += "," + std::to_string(imu_msg->ahrs_database.twist.twist.linear.x);
    log_data += "," + std::to_string(imu_msg->ahrs_database.twist.twist.linear.y);
    log_data += "," + std::to_string(imu_msg->ahrs_database.twist.twist.linear.z);
    log_data += "," + std::to_string(imu_msg->ahrs_database.twist.twist.angular.x);
    log_data += "," + std::to_string(imu_msg->ahrs_database.twist.twist.angular.y);
    log_data += "," + std::to_string(imu_msg->ahrs_database.twist.twist.angular.z);
    if (imu_msg->ahrs_database.twist.covariance.size() < 36) {
        RCLCPP_WARN(this->get_logger(), "Incomplete data for covariance. Look for \"-1\" in the covariance field.");
        for (int i = 0; i < 36; i++) {
            log_data += "," + std::to_string(-1);
        }
    }
    else {
        for (auto cov_data : imu_msg->ahrs_database.twist.covariance) {
            log_data += "," + std::to_string(cov_data);
        }
    }
    log_data += "," + std::to_string(imu_msg->roll);
    log_data += "," + std::to_string(imu_msg->pitch);
    log_data += "\n";
    write(this->imu_fd->get_write_fd(), log_data.c_str(), log_data.length());
}

std::string Data_Logger::get_current_time() {
    char buffer [80];

    std::time_t rawtime;
    std::tm* timeinfo;
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    std::strftime(buffer,80,"%Y-%m-%d-%H-%M-%S",timeinfo);

    struct timespec current_time_for_milliseconds;
    clock_gettime(CLOCK_MONOTONIC, &current_time_for_milliseconds);
    long milliseconds = current_time_for_milliseconds.tv_nsec / 1000000;

    std::string cpp_time = buffer;
    cpp_time += "-" + std::to_string(milliseconds);
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
    std::unique_ptr<FD_Interface> imu_fd = std::make_unique<Data_FD>(filepath + "-imu.csv");

    rclcpp::init(argc, argv);
    auto data_logger = std::make_shared<Data_Logger>(std::move(drr_fd), std::move(vr_fd), std::move(imu_fd));
    rclcpp::spin(data_logger);
    
    rclcpp::shutdown();
    
    return 0;
}
#endif // ENABLE_TESTING

