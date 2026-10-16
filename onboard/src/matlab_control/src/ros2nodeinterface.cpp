
//
// File ros2nodeinterface.cpp
//
// Code generated for Simulink model 'matlab_control'.
//
// Model version                  : 1.13
// Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
// C/C++ source code generated on : Mon Jul 13 00:21:14 2026
//
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4265)
#pragma warning(disable : 4458)
#pragma warning(disable : 4100)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wredundant-decls"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wshadow"
#endif //_MSC_VER
#include "rclcpp/rclcpp.hpp"
#include "matlab_control.h"
#include "ros2nodeinterface.h"
#include <thread>
#include <chrono>
#include <utility>
#undef ROS_SET_RTM_ERROR_STATUS
#undef ROS_GET_RTM_ERROR_STATUS
#undef ROS_RTM_STEP_TASK
#define ROS_SET_RTM_ERROR_STATUS(status) mModel->getRTM()->setErrorStatus(status)
#define ROS_GET_RTM_ERROR_STATUS()       mModel->getRTM()->getErrorStatus()
#define ROS_RTM_STEP_TASK(id)            mModel->getRTM()->StepTask(id)
#include "slros2_multi_threaded_executor.h"
std::vector<rclcpp::SubscriptionBase*> SLROSSubscribers;
extern rclcpp::Node::SharedPtr SLROSNodePtr;
#ifndef RT_MEMORY_ALLOCATION_ERROR_DEF
#define RT_MEMORY_ALLOCATION_ERROR_DEF
const char *RT_MEMORY_ALLOCATION_ERROR = "memory allocation error";
#endif
namespace ros2 {
namespace matlab {
NodeInterface::NodeInterface()
    : mModel()
    , mExec()
    , mBaseRateSem()
    , mBaseRateThread()
    , mSchedulerTimer()
    , mSubRate1Sem()
    , mSubRate1Thread()
    , mStopSem()
    , mRunModel(true){
  }
NodeInterface::~NodeInterface() {
    terminate();
  }
void NodeInterface::initialize(int argc, char * const argv[]) {
    try {
        //initialize ros2
        std::vector<char *> args(argv, argv + argc);
        rclcpp::init(static_cast<int>(args.size()), args.data());
        //create the Node specified in Model
        std::string NodeName("matlab_control");
        SLROSNodePtr = std::make_shared<rclcpp::Node>(NodeName);
        RCLCPP_INFO(SLROSNodePtr->get_logger(),"** Starting the model \"matlab_control\" **\n");
        mExec = std::make_shared<rclcpp::executors::SLMultiThreadedExecutor>();
        mExec->add_node(SLROSNodePtr);
        //initialize the model which will initialize the publishers and subscribers
        mModel = std::make_shared<matlab_control>(
        );
        ROS_SET_RTM_ERROR_STATUS(NULL);
        mModel->initialize();
        //create the threads for the rates in the Model
        mBaseRateThread = std::make_shared<std::thread>(&NodeInterface::baseRateTask, this);
            mSubRate1Thread = std::make_shared<std::thread>(&NodeInterface::subRate1Task, this);
        // Create "MutuallyExclusive" callback group for callback associated with mSchedulerTimer to prevent
        // it from being executed in parallel.
		mSchedulerGroup = SLROSNodePtr->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
		mSchedulerTimer = SLROSNodePtr->create_wall_timer(std::chrono::nanoseconds(10000000),std::bind(&NodeInterface::schedulerThreadCallback,this),mSchedulerGroup);
		for(size_t ctr = 0; ctr<SLROSSubscribers.size();ctr++){
           mExec->stopSubscriberCallback(SLROSSubscribers[ctr]);
       }
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
        throw ex;
    }
    catch (...) {
        std::cout << "Unknown exception" << std::endl;
        throw;
    }
}
int NodeInterface::run() {
  if (mExec) {
    mExec->spin();
  }
  mRunModel = false;
  return 0;
}
boolean_T NodeInterface::getStopRequestedFlag(void) {
    #ifndef rtmGetStopRequested
    return (!(ROS_GET_RTM_ERROR_STATUS()
        == (NULL)));
    #else
    return (!(ROS_GET_RTM_ERROR_STATUS()
        == (NULL)) || rtmGetStopRequested(mModel->getRTM()));
    #endif
}
void NodeInterface::stop(void) {
  if (mExec.get()) {
    mExec->cancel();
    if (SLROSNodePtr) {
      mExec->remove_node(SLROSNodePtr);
    }
    while (mExec.use_count() > 1);
  }
}
void NodeInterface::terminate(void) {
    if (mBaseRateThread.get()) {
        mRunModel = false;
        mBaseRateSem.notify(); // break out wait
        mBaseRateThread->join();
        if (mSchedulerTimer.get()) {
	        mSchedulerTimer->cancel();
    	    mSchedulerTimer->reset();
        }
        mSubRate1Sem.notify();
        mSubRate1Thread->join();
        mSubRate1Thread.reset();
        mBaseRateThread.reset();
        if (mModel.get()) {
            mModel->terminate();
        }
        mModel.reset();
        mExec.reset();
        SLROSNodePtr.reset();
        rclcpp::shutdown();
    }
}
//
// Scheduler Task using wall clock timer to run base-rate
//
void NodeInterface::schedulerThreadCallback(void)
{
  if(mRunModel) {
        mBaseRateSem.notify();
    }
}
//
//Model specific
void NodeInterface::baseRateTask(void) {
  mRunModel = (ROS_GET_RTM_ERROR_STATUS() ==
              (NULL));
  while (mRunModel) {
    mBaseRateSem.wait();
#ifdef MW_DEBUG_LOG
    RCLCPP_INFO(SLROSNodePtr->get_logger(),"** Base rate task semaphore received\n");
#endif
    if (!mRunModel) break;
         // Execute the task which needs to run at base rate
        //Check if subrate 1 should be unblocked
        if (ROS_RTM_STEP_TASK(1)) {
          mSubRate1Sem.notify();
        }
    mModel->step0();
    mRunModel &= !NodeInterface::getStopRequestedFlag(); //If RunModel and not stop requested
  }
  // Shutdown the ROS 2 node
  NodeInterface::stop();
}
void NodeInterface::subRate1Task(void) {
  while (mRunModel) {
    mSubRate1Sem.wait();
#ifdef MW_DEBUG_LOG
    RCLCPP_INFO(SLROSNodePtr->get_logger(),"** Sub-rate 1 task semaphore received\n");
#endif
    if (!mRunModel) break;
    mModel->step1();
  }
}
}//namespace matlab
}//namespace ros2
#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif //_MSC_VER
