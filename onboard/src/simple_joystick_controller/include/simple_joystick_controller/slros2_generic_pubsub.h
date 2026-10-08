// Copyright 2022-2025 The MathWorks, Inc.

#ifndef _SLROS2_GENERIC_PUBSUB_H_
#define _SLROS2_GENERIC_PUBSUB_H_

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/qos.hpp"
#include "slros_busmsg_conversion.cpp"
#include <vector>

#ifndef _SL_ROS2_CONTROL_PLUGIN_
// Use shared pointer for standard/component node generation
extern rclcpp::Node::SharedPtr SLROSNodePtr;
#endif

#ifdef _SL_ROS2_CONTROL_PLUGIN_
#include "realtime_tools/realtime_box.h"
#include "realtime_tools/realtime_buffer.h"
#include "realtime_tools/realtime_publisher.h"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
// Use lifecycle node for ROS 2 control plugin generation
extern rclcpp_lifecycle::LifecycleNode::SharedPtr SLROSNodePtr;
#endif

extern std::vector<rclcpp::SubscriptionBase*> SLROSSubscribers;

inline rclcpp::QoS getQOSSettingsFromRMW(const rmw_qos_profile_t& qosProfile);

static std::string getPolicyNameFromKind(int policyKind) {
    switch (policyKind) {
        case 1 << 0: return "INVALID";
        case 1 << 1: return "DURABILITY";
        case 1 << 2: return "DEADLINE";
        case 1 << 3: return "LIVELINESS";
        case 1 << 4: return "RELIABILITY";
        case 1 << 5: return "HISTORY";
        case 1 << 6: return "LIFESPAN";
        case 1 << 7: return "DEPTH";
        case 1 << 8: return "LIVELINESS_LEASE_DURATION";
        case 1 << 9: return "AVOID_ROS_NAMESPACE_CONVENTIONS";
        default: return "UNKNOWN_POLICY";
    }
}

/**
 * Utility class for executing the subscriber callback.
 */
class ExecutorUtils : public rclcpp::Executor{
	public:
	static void executeSubscription(rclcpp::SubscriptionBase::SharedPtr subscriber){
		rclcpp::Executor::execute_subscription(subscriber);
	}
};

/**
 * Class for subscribing to ROS 2 messages in C++.
 *
 * This class is used by code generated from the Simulink ROS 2
 * subscriber blocks and is templatized by the ROS 2 message type and
 * Simulink bus type.
 */
template <class MsgType, class BusType>
class SimulinkSubscriber {
  public:
    void createSubscriber(std::string const& topic, const rmw_qos_profile_t& qosProfile);
    bool getLatestMessage(BusType* busPtr); // returns true iff message is new
    void resetSubscriberPtr(){
        _subscriber.reset();
    }
  private:
    std::shared_ptr<rclcpp::Subscription<MsgType>> _subscriber;
    bool _newMessageReceived;
    std::unique_ptr<MsgType> _lastMsg;
    std::mutex _subMutex;
    std::shared_ptr<rclcpp::WaitSet> wait_set;
	std::string mTopic;
    size_t msgCtr = 0;
};

/**
 * Class for publishing ROS 2 messages in C++.
 *
 * This class is used by code generated from the Simulink ROS 2
 * publisher blocks and is templatized by the ROS 2 message type and
 * Simulink bus type.
 */
template <class MsgType, class BusType>
class SimulinkPublisher {
  public:
    void createPublisher(std::string const& topic, const rmw_qos_profile_t& qosProfile);
    void publish(const BusType* busPtr); // returns true iff message is new
    void resetPublisherPtr(){
        _publisher.reset();
    }
  private:
    std::shared_ptr<rclcpp::Publisher<MsgType>> _publisher;
    std::string mTopic;
    size_t msgCtr = 0;
};


/**
 * Create a C++ subscriber object
 *
 * @param topic The topic name to subscribe to
 * @param qosProfile QoS profile of the subscription
 */
template <class MsgType, class BusType>
void SimulinkSubscriber<MsgType, BusType>::createSubscriber(std::string const& topic,
                                                            const rmw_qos_profile_t& qosProfile) {
    auto callback = [this](std::unique_ptr<MsgType> msg) {
        std::lock_guard<std::mutex> lockMsg(_subMutex);
        _lastMsg = std::move(msg);
    };

    rclcpp::SubscriptionOptions subscription_options;

    subscription_options.event_callbacks.deadline_callback = [this, topic](const rclcpp::QOSDeadlineRequestedInfo& event) {
        RCLCPP_WARN(SLROSNodePtr->get_logger(), "Requested deadline missed on topic '%s'. The subscription did not receive any messages within the expected duration set by the deadline QoS policy. Total missed deadlines: %d\n",topic.c_str(),event.total_count);
    };

    subscription_options.event_callbacks.liveliness_callback = [this, topic](const rclcpp::QOSLivelinessChangedInfo& event) {
        RCLCPP_WARN(SLROSNodePtr->get_logger(), "Liveliness changed on topic '%s'. A change in the liveliness of one or more publishers on the topic has been detected. Alive count: %d, Not alive count: %d\n",topic.c_str(),event.alive_count,event.not_alive_count);
    };

    subscription_options.event_callbacks.message_lost_callback = [this, topic](const rclcpp::QOSMessageLostInfo& event) {
        RCLCPP_WARN(SLROSNodePtr->get_logger(), "Messages lost on topic '%s'. Total lost messages: %lu\n",topic.c_str(),event.total_count);
    };
    
    subscription_options.event_callbacks.incompatible_qos_callback = [this, topic](const rclcpp::QOSRequestedIncompatibleQoSInfo& event) {
        RCLCPP_WARN(SLROSNodePtr->get_logger(), "New publisher discovered on topic '%s', offering incompatible QoS. No messages will be received from it. Last incompatible policy: %s",topic.c_str(),getPolicyNameFromKind(event.last_policy_kind).c_str());
    };

    _subscriber = SLROSNodePtr->create_subscription<MsgType>(
        topic, getQOSSettingsFromRMW(qosProfile), callback, subscription_options);
    
    // Add the subscriber to the list of subscriber
    SLROSSubscribers.push_back(_subscriber.get());
	
	mTopic = topic;
}

/**
 * Get the latest received message
 *
 * @param busPtr Simulink bus structure that should be populated with message contents
 * @return =TRUE, then a new message has been received and *busPtr holds the newly-received message.
 * =FALSE when a new message has not been received and *busPtr is unchanged.
 */
template <class MsgType, class BusType>
bool SimulinkSubscriber<MsgType, BusType>::getLatestMessage(BusType* busPtr) {

    //Execute the subscriber
    ExecutorUtils::executeSubscription(_subscriber);
	
	if (_lastMsg.get()) {
        std::lock_guard<std::mutex> lockMsg(_subMutex);
        convertToBus(busPtr, *_lastMsg);
		msgCtr++;
#ifdef MW_DEBUG_LOG
	    RCLCPP_INFO(SLROSNodePtr->get_logger(),"SUB-%s : %ld, msg_ptr : %p",mTopic.c_str(),msgCtr,(void*)(_lastMsg.get()));
#endif
        _lastMsg.reset();
        return true;
    }
    return false;
}

/**
 * Create a C++ publisher object
 *
 * @param topic The topic name to publish to
 * @param qosProfile QoS profile of the publisher
 */
template <class MsgType, class BusType>
void SimulinkPublisher<MsgType, BusType>::createPublisher(std::string const& topic,
                                                          const rmw_qos_profile_t& qosProfile) {

    rclcpp::PublisherOptions publisher_options;

    publisher_options.event_callbacks.deadline_callback = [this,topic](rclcpp::QOSDeadlineOfferedInfo& event) {
        RCLCPP_WARN(SLROSNodePtr->get_logger(), "Offered deadline missed on topic '%s'. The publisher failed to meet the expected deadline for publishing messages as per the deadline QoS policy. Total missed deadlines: %d\n",topic.c_str(),event.total_count);
    };

    publisher_options.event_callbacks.liveliness_callback = [this,topic](rclcpp::QOSLivelinessLostInfo& event) {
        RCLCPP_WARN(SLROSNodePtr->get_logger(), "Liveliness lost on topic '%s'. The publisher has not indicated liveliness within the lease duration. Liveliness lost count: %d\n",topic.c_str(),event.total_count);
    };

    publisher_options.event_callbacks.incompatible_qos_callback = [this,topic](rclcpp::QOSOfferedIncompatibleQoSInfo& event) {
        RCLCPP_WARN(SLROSNodePtr->get_logger(), "New subscription discovered on topic '%s', requesting incompatible QoS. No messages will be sent to it. Last incompatible policy: %s\n",topic.c_str(),getPolicyNameFromKind(event.last_policy_kind).c_str());
    };

    _publisher = SLROSNodePtr->create_publisher<MsgType>(topic, getQOSSettingsFromRMW(qosProfile), publisher_options);
    mTopic = topic;
}

/**
 * Publish a message
 *
 * @param busPtr Pointer to the bus structure for the outgoing message
 */
template <class MsgType, class BusType>
void SimulinkPublisher<MsgType, BusType>::publish(const BusType* inBus) {
    auto msg = std::make_unique<MsgType>();
    convertFromBus(*msg, inBus);
    
    msgCtr++;
   
#ifdef MW_DEBUG_LOG
	RCLCPP_INFO(SLROSNodePtr->get_logger(),"PUB-%s : %ld, msg-ptr : %p",mTopic.c_str(),msgCtr,(void*)(msg.get()));
#endif
    _publisher->publish(std::move(msg));
}

#ifdef _SL_ROS2_CONTROL_PLUGIN_
/**
 * Class for publishing realtime ROS 2 messages in C++.
 *
 * This class is used by code ros2_control controller package
 * generated from the Simulink ROS 2 publisher blocks and is templatized by
 * the ROS 2 message type and Simulink bus type.
 */
template <class MsgType, class BusType>
class SimulinkRTPublisher {
  public:
    void createPublisher(std::string const& topic, const rmw_qos_profile_t& qosProfile);
    void publish(const BusType* busPtr);// returns true if message is new
    void resetPublisherPtr(){
        _realtime_publisher.reset();
        _regular_publisher.reset();
    }

  private:
    std::shared_ptr<rclcpp::Publisher<MsgType>> _regular_publisher = nullptr; // Regular publisher
    std::shared_ptr<realtime_tools::RealtimePublisher<MsgType>> _realtime_publisher = nullptr; /// Realtime publisher
};

/**
 * Initialize the realtime publisher
 *
 * @param topic The topic name to publish to
 * @param qosProfile QoS profile of the publisher
 */
template <class MsgType, class BusType>
void SimulinkRTPublisher<MsgType, BusType>::createPublisher(std::string const& topic,
                                                            const rmw_qos_profile_t& qosProfile) {
    rclcpp::PublisherOptions publisher_options;

    publisher_options.event_callbacks.deadline_callback = [this,topic](rclcpp::QOSDeadlineOfferedInfo& event) {
        RCLCPP_WARN(SLROSNodePtr->get_logger(), "Offered deadline missed on topic '%s'. The publisher failed to meet the expected deadline for publishing messages as per the deadline QoS policy. Total missed deadlines: %d\n",topic.c_str(),event.total_count);
    };

    publisher_options.event_callbacks.liveliness_callback = [this,topic](rclcpp::QOSLivelinessLostInfo& event) {
        RCLCPP_WARN(SLROSNodePtr->get_logger(), "Liveliness lost on topic '%s'. The publisher has not indicated liveliness within the lease duration. Liveliness lost count: %d\n",topic.c_str(),event.total_count);
    };

    publisher_options.event_callbacks.incompatible_qos_callback = [this,topic](rclcpp::QOSOfferedIncompatibleQoSInfo& event) {
        RCLCPP_WARN(SLROSNodePtr->get_logger(), "New subscription discovered on topic '%s', requesting incompatible QoS. No messages will be sent to it. Last incompatible policy: %s\n",topic.c_str(),getPolicyNameFromKind(event.last_policy_kind).c_str());
    };

    _regular_publisher = SLROSNodePtr->create_publisher<MsgType>(topic, getQOSSettingsFromRMW(qosProfile), publisher_options);
    _realtime_publisher = std::make_shared<realtime_tools::RealtimePublisher<MsgType>>(_regular_publisher);
}
/**
 * Convert from bus and publish the message
 *
 * @param busPtr Pointer to the bus structure for the outgoing message
 */
template <class MsgType, class BusType>
void SimulinkRTPublisher<MsgType, BusType>::publish(const BusType* inBus) {
    if (_realtime_publisher->trylock())
    {
        convertFromBus(_realtime_publisher->msg_, inBus);
        _realtime_publisher->unlockAndPublish();
    }
}
#endif
#endif
