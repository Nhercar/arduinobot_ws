#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include "arduinobot_msgs/action/arduinobot_task.hpp"
#include <rclcpp_components/register_node_macro.hpp>
#include <moveit/move_group_interface/move_group_interface.hpp>

#include <memory>
#include <thread>

using namespace std::placeholders;

namespace arduinobot_remote
{

class TaskServer : public rclcpp::Node
{
public:
    explicit TaskServer(const rclcpp::NodeOptions & options = rclcpp::NodeOptions()) : Node("task_server", options)
    {
        action_server_ = rclcpp_action::create_server<arduinobot_msgs::action::ArduinobotTask>(
            this, "task_server",
            std::bind(&TaskServer::goalCallback, this, _1, _2),
            std::bind(&TaskServer::cancelCallback, this, _1),
            std::bind(&TaskServer::acceptedCallback, this, _1)
        );
        
        RCLCPP_INFO(this->get_logger(), "Task Server Node has been started.");

    }
private:
    rclcpp_action::Server<arduinobot_msgs::action::ArduinobotTask>::SharedPtr action_server_;
    std::shared_ptr<moveit::planning_interface::MoveGroupInterface> arm_move_group_, gripper_move_group_;

    std::vector<double> arm_joint_goal;
    std::vector<double> gripper_joint_goal;


    rclcpp_action::GoalResponse goalCallback( const rclcpp_action::GoalUUID& uuid,
        std::shared_ptr<const arduinobot_msgs::action::ArduinobotTask::Goal> goal)
    {
        RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Received goal request number: " << goal);
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    void acceptedCallback(const std::shared_ptr<rclcpp_action::ServerGoalHandle<arduinobot_msgs::action::ArduinobotTask>> goal_handle)
    {
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Goal accepted.");
        // Start executing the goal in a separate thread or asynchronously
        std::thread{std::bind(&TaskServer::execute, this, _1), goal_handle}.detach();
    }

    void execute(const std::shared_ptr<rclcpp_action::ServerGoalHandle<arduinobot_msgs::action::ArduinobotTask>> goal_handle)
    {
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Executing goal...");

        if(!arm_move_group_){
            arm_move_group_= std::make_shared<moveit::planning_interface::MoveGroupInterface>(shared_from_this(), "arm");
        }

        if(!gripper_move_group_){
            gripper_move_group_= std::make_shared<moveit::planning_interface::MoveGroupInterface>(shared_from_this(), "gripper");
        }

        auto result = std::make_shared<arduinobot_msgs::action::ArduinobotTask::Result>(); 

        switch (goal_handle->get_goal()->task_number)
        {
        case 0:
            arm_joint_goal = {0.0, 0.0, 0.0};
            gripper_joint_goal = {-0.7, 0.7};
            break;

        case 1:
            arm_joint_goal = {-1.14, -0.6, -0.07};
            gripper_joint_goal = {0.0, 0.0};
            break;
        
        case 2:
            arm_joint_goal = {-1.57, 0.0, -0.9};
            gripper_joint_goal = {0.0, 0.0};
            break;            
            
        default:
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "The goal  requested is not available");
            break;
        }

        arm_move_group_->setStartState(*arm_move_group_->getCurrentState());
        gripper_move_group_->setStartState(*gripper_move_group_->getCurrentState());

        bool arm_within_bounds = arm_move_group_->setJointValueTarget(arm_joint_goal);
        bool gripper_within_bounds = gripper_move_group_->setJointValueTarget(gripper_joint_goal);

        if(!arm_within_bounds || !gripper_within_bounds){
            RCLCPP_WARN(rclcpp::get_logger("rclcpp"), "Target joints are outside the working space");
            return;
        }
        
        moveit::planning_interface::MoveGroupInterface::Plan arm_plan, gripper_plan;

        bool arm_plan_success = (arm_move_group_->plan(arm_plan) == moveit::core::MoveItErrorCode::SUCCESS);
        bool gripper_plan_success = (gripper_move_group_->plan(gripper_plan) == moveit::core::MoveItErrorCode::SUCCESS);   
        
        if(arm_plan_success && gripper_plan_success){
            arm_move_group_->move();
            gripper_move_group_->move();
        } else {
            RCLCPP_WARN(rclcpp::get_logger("rclcpp"), "Planning failed for one or more move groups");
            return;
        }

        result->success = true;
        goal_handle->succeed(result);

    }


    rclcpp_action::CancelResponse cancelCallback(
        const std::shared_ptr<rclcpp_action::ServerGoalHandle<arduinobot_msgs::action::ArduinobotTask>> goal_handle)
    {
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Received request to cancel goal.");
        if(arm_move_group_){
            arm_move_group_->stop();
        }

        if(gripper_move_group_){
            gripper_move_group_->stop();
        }
        return rclcpp_action::CancelResponse::ACCEPT;
    }
};

}

RCLCPP_COMPONENTS_REGISTER_NODE(arduinobot_remote::TaskServer)