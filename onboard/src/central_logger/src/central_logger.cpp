#include "central_logger.hpp"
#include <fstream>

central_logger::central_logger() : rclcpp::Node("Central_logger"){
    loginfo_subscriber = this->reate_subcription<custom_interface::msg::Loginfo>("loginfo_receive", 10, std::bind(&central_logger_callback, this, std::placeholders:: 1));

}

void central_logger::central_logger_callback(custom_interface::msg::Loginfo msg){
    current_name = msg -> name;
    current_contents = msg -> contents;
}

void central_logger::check_name(){
    filename = current_name + ".log"
    filepath = "~/Robosub/Logs" + filename
    std::ifstream file(filepath);
    if file.is_open(){
        std::ofstream file1(filepath, std::ios::app);
        timestamp = std::chrono::steady_clock::now();
        file1 << timestamp + ":" + current_contents;
        file1.close();
    } else {
        std::ofstream file1(filepath, std::ios::out);
        timestamp = std::chrono::steady_clock::now();
        file1 << timestamp + ":" + current_contents;
        file1.close();
    }
    file.close();
}