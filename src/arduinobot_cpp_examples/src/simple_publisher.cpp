#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <chrono>


using namespace std::chrono_literals;

class SimplePublisher : public rclcpp::Node
{
 public:
 SimplePublisher() : Node("simple_publisher"), counter_(0)
 {
   put = create_pulisher<std_msgs::msg::String>("chatter", 10); 
   timer_ = create_wall_timer(1s, std::bind(&SimplePublisher::timerCallback, this));
   RCLCPP_INFO(get_logger(), "Publishing at 1Hz");
 }

 void timerCallback(){
    auto message = std_msgs::msg::String();
    message.data = "Hello ROs2 - counter: " + std::to_string(counter_++);
    pub_->publish(message);
 }

 private:
    unsigned int counter_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;

};