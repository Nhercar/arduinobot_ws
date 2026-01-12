#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <arduinobot_msgs/action/fibonacci.hpp>

#include <memory>

using namespace std::placeholders;

namespace arduinobot_cpp_examples
{

class SimpleActionClient : public rclcpp::Node
{
public:
    explicit SimpleActionClient(const rclcpp::NodeOptions & options = rclcpp::NodeOptions()) 
        : Node("simple_action_client", options)
    {
        this->action_client_ = rclcpp_action::create_client<arduinobot_msgs::action::Fibonacci>(
            this, "fibonacci");
        
        RCLCPP_INFO(this->get_logger(), "Simple Action Client Node has been started.");
        
        // Send goal after a small delay to ensure server is ready
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(500),
            std::bind(&SimpleActionClient::send_goal, this));
    }

private:
    rclcpp_action::Client<arduinobot_msgs::action::Fibonacci>::SharedPtr action_client_;
    rclcpp::TimerBase::SharedPtr timer_;
    bool goal_sent_ = false;

    void send_goal()
    {
        // Send goal only once
        if (goal_sent_) {
            return;
        }
        goal_sent_ = true;
        timer_->cancel();

        if (!this->action_client_->wait_for_action_server(std::chrono::seconds(5))) {
            RCLCPP_ERROR(this->get_logger(), "Action server not available after waiting");
            return;
        }

        auto goal_msg = arduinobot_msgs::action::Fibonacci::Goal();
        goal_msg.order = 10;

        RCLCPP_INFO(this->get_logger(), "Sending goal with order 10");

        auto send_goal_options = rclcpp_action::Client<arduinobot_msgs::action::Fibonacci>::SendGoalOptions();
        send_goal_options.goal_response_callback = std::bind(&SimpleActionClient::goal_response_callback, this, _1);
        send_goal_options.feedback_callback = std::bind(&SimpleActionClient::feedback_callback, this, _1, _2);
        send_goal_options.result_callback = std::bind(&SimpleActionClient::result_callback, this, _1);

        this->action_client_->async_send_goal(goal_msg, send_goal_options);
    }

    void goal_response_callback(
        const rclcpp_action::ClientGoalHandle<arduinobot_msgs::action::Fibonacci>::SharedPtr & goal_handle)
    {
        if (!goal_handle) {
            RCLCPP_ERROR(this->get_logger(), "Goal was rejected by server");
        } else {
            RCLCPP_INFO(this->get_logger(), "Goal accepted by server, waiting for result");
        }
    }

    void feedback_callback(
        const rclcpp_action::ClientGoalHandle<arduinobot_msgs::action::Fibonacci>::SharedPtr & goal_handle,
        const std::shared_ptr<const arduinobot_msgs::action::Fibonacci::Feedback> feedback)
    {
        (void)goal_handle;
        std::string sequence_str = "";
        for (auto num : feedback->partial_sequence) {
            sequence_str += std::to_string(num) + " ";
        }
        RCLCPP_INFO_STREAM(this->get_logger(), "Feedback: " << sequence_str);
    }

    void result_callback(
        const rclcpp_action::ClientGoalHandle<arduinobot_msgs::action::Fibonacci>::WrappedResult & result)
    {
        switch (result.code) {
            case rclcpp_action::ResultCode::SUCCEEDED:
                break;
            case rclcpp_action::ResultCode::ABORTED:
                RCLCPP_ERROR(this->get_logger(), "Goal was aborted");
                return;
            case rclcpp_action::ResultCode::CANCELED:
                RCLCPP_ERROR(this->get_logger(), "Goal was canceled");
                return;
            default:
                RCLCPP_ERROR(this->get_logger(), "Unknown result code");
                return;
        }

        std::string sequence_str = "";
        for (auto num : result.result->sequence) {
            sequence_str += std::to_string(num) + " ";
        }
        RCLCPP_INFO_STREAM(this->get_logger(), "Result: " << sequence_str);

        rclcpp::shutdown();
    }
};

}

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(arduinobot_cpp_examples::SimpleActionClient)

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<arduinobot_cpp_examples::SimpleActionClient>());
    rclcpp::shutdown();
    return 0;
}
