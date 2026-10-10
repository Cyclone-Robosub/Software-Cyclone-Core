#include "custom_interfaces/msg/loginfo.hpp"

class central_logger : public rclcpp::NODE{
public:



private:
    rclcpp:Subscription<custom_interfaces::msg::Loginfo>::SharedPtr loginfo_subscriber;
    char[32] current_name;
    char[256] current_contents;

}