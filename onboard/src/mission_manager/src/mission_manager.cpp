#include "mission_manager.hpp"

MissionManager::MissionManager() :
    Node("mission_manager") {

}

#ifndef ENABLE_TESTING

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MissionManager>());
    rclcpp::shutdown();
    return 0;
}

#endif // ENABLE_TESTING
