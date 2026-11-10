#include "log_publisher.hpp"

#include <algorithm>
#include <filesystem>
#include <stdexcept>

log_publisher::log_publisher(rclcpp::Node& node, const std::string& name)
: name(name)
{
    const custom_interfaces::msg::Loginfo msg;
    if (name.empty() || name.size() > msg.name.size() || std::filesystem::path(name).filename() != name)
    {
        throw std::invalid_argument("Invalid log name: '" + name + "'");
    }

    publisher = node.create_publisher<custom_interfaces::msg::Loginfo>("loginfo_receive", 10);
}

void log_publisher::log(const std::string& contents)
{
    // A fresh Loginfo is zero filled by its constructor, so shorter strings are null terminated
    custom_interfaces::msg::Loginfo msg;
    std::copy(name.begin(), name.end(), msg.name.begin());
    std::copy_n(contents.begin(), std::min(contents.size(), msg.contents.size()), msg.contents.begin());
    publisher->publish(msg);
}
