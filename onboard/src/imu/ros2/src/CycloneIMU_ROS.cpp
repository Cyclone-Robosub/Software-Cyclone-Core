#include "CycloneIMU_ROS.hpp"

#include <iostream>
std::chrono::time_point startTime = std::chrono::steady_clock::now();
int main(int argc, char* argv[])
{
    std::cout << "Start of Cyclone Robosub IMU API" << std::endl;
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CycloneIMU_ROS>();
    startTime = std::chrono::steady_clock::now();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
int imucount = 0;
std::atomic<bool> isBenchmarkCompletedIMU = false;
void CycloneIMU_ROS::imu_callback(std::shared_ptr<sensor_msgs::msg::Imu> msg)
{
    imucount++;
    std::unique_lock<std::mutex> lock(imu_mutex);
    imu_ptr = msg;
    lock.unlock();
    if (imucount > 100 && isBenchmarkCompletedIMU == false)
    {
        /* something which might take time */

        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        std::cout << "It took " << duration.count() << " seconds.\n";
        isBenchmarkCompletedIMU = true;
    }
}
void CycloneIMU_ROS::DIDINS1_callback(const inertial_sense_ros2::msg::DIDINS1::SharedPtr msg)
{
    std::unique_lock<std::mutex> lock(DIDINS1_mutex);
    DIDINS1_ptr = msg;
    lock.unlock();

    // Optional: Print for debugging (convert radians to degrees for readability)
    // float roll_deg = msg->theta.x * (180.0 / M_PI);
    // float pitch_deg = msg->theta.y * (180.0 / M_PI);
    // std::cout << "Current Roll: " << roll_deg << " Pitch: " << pitch_deg << std::endl;
}
void CycloneIMU_ROS::mag_callback(std::shared_ptr<sensor_msgs::msg::MagneticField> msg)
{
    std::unique_lock<std::mutex> lock(mag_mutex);
    mag_ptr = msg;
    lock.unlock();
}
std::atomic<bool> isOdomBenchmarkCompletedODOM = false;
void CycloneIMU_ROS::odom_callback(std::shared_ptr<nav_msgs::msg::Odometry> msg)
{

    std::unique_lock<std::mutex> lock(odom_mutex);
    odom_ptr = msg;
    somerandomValue = msg->twist.twist.linear.x;
    lock.unlock();
   std:: cout << somerandomValue << std::endl;
    odomCount++;
    if(isBenchmarkCompletedIMU == true && isOdomBenchmarkCompletedODOM == false)
    {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        std::cout << "It took " << duration.count() << " seconds to receive 100 odom messages.\n" << odomCount;
        isOdomBenchmarkCompletedODOM = true;
    }
}

void CycloneIMU_ROS::pressure_callback(std::shared_ptr<sensor_msgs::msg::FluidPressure> msg)
{
    std::unique_lock<std::mutex> lock(pressure_mutex);
    pressure_ptr = msg;
    lock.unlock();
}

void CycloneIMU_ROS::Controls_Publisher()
{
    while (true)
    {
        auto start_time_publishing = std::chrono::steady_clock::now();
        auto predicted_time = start_time_publishing + std::chrono::milliseconds(10);
        std::unique_lock<std::mutex> lock_imu(imu_mutex);
        if (imu_ptr)
        {
            custom_msg.imu_fusion = *imu_ptr;
          std::cout <<"linear Acceleration x" << custom_msg.imu_fusion.linear_acceleration.x << std::endl;
        }
        lock_imu.unlock();
        std::unique_lock<std::mutex> lock_DIDINS1(DIDINS1_mutex);
        if (DIDINS1_ptr)
        {
            custom_msg.roll = DIDINS1_ptr->theta[0];
            custom_msg.pitch = DIDINS1_ptr->theta[1];
        }
        lock_DIDINS1.unlock();
        std::unique_lock<std::mutex> lockmag(mag_mutex);
        if (mag_ptr)
        {
                custom_msg.mag_array = *mag_ptr;
    //         std::cout << "mag: " << custom_msg.mag_array.magnetic_field.x << std::endl;
        }
        else
        {
      //      std::cout << "No Mag data received yet." << std::endl;
        }
        lockmag.unlock();
        std::unique_lock<std::mutex> lockpressure(pressure_mutex);
        if (pressure_ptr)
        {
                custom_msg.pressure = *pressure_ptr;
        }
        
        lockpressure.unlock();
        std::unique_lock<std::mutex> lock_odom(odom_mutex);
        if (odom_ptr)
        {
                custom_msg.ahrs_database = *odom_ptr;
          //      std::cout <<" Twist linear x" <<custom_msg.ahrs_database.twist.twist.linear.x << std::endl;
            
        }
        lock_odom.unlock();

        imu_publisher->publish(custom_msg);
      //  RCLCPP_INFO(this->get_logger(), "Published combined sensor message");
        std::this_thread::sleep_until(predicted_time);
    }
}
