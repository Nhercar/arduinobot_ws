#include<rclcpp/rclcpp.hpp>
#include<memory>
#include<arduinobot_msgs/srv/euler_to_quaternion.hpp>
#include<arduinobot_msgs/srv/quaternion_to_euler.hpp>
#include<tf2/LinearMath/Quaternion.h>
#include<tf2/LinearMath/Matrix3x3.h>

using namespace std::placeholders;

class AnglesConverter
 : public rclcpp::Node
{
    public:
        AnglesConverter() : Node("angles_conversion_service")
        {
            euler_to_quaternion = create_service<arduinobot_msgs::srv::EulerToQuaternion>("euler_to_quaternion", std::bind(&AnglesConverter::eulerToQuaternion, this, _1, _2));
            quaternion_to_euler = create_service<arduinobot_msgs::srv::QuaternionToEuler>("quaternion_to_euler", std::bind(&AnglesConverter::quaternionToEuler, this, _1, _2));           
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Service add_two_ints is Ready");
        }           

    private:
        rclcpp::Service<arduinobot_msgs::srv::EulerToQuaternion>::SharedPtr euler_to_quaternion;
        rclcpp::Service<arduinobot_msgs::srv::QuaternionToEuler>::SharedPtr quaternion_to_euler;

        void eulerToQuaternion(const std::shared_ptr<arduinobot_msgs::srv::EulerToQuaternion::Request> req,
                             const std::shared_ptr<arduinobot_msgs::srv::EulerToQuaternion::Response> res)
        {
            RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "New request received roll: " <<req->roll << " pitch: " << req->pitch << " yaw: " << req->yaw);
            tf2::Quaternion q;
            q.setRPY(req->roll, req->pitch,  req->yaw);
            res->x = q.x();;
            res->y = q.y();
            res->z = q.z();
            res->w = q.w();
            RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Returning: x:" << res->x << " y: " << res->y << " z: " << res->z << " w: " << res->w);

        }

        void quaternionToEuler(const std::shared_ptr<arduinobot_msgs::srv::QuaternionToEuler::Request> req,
                             const std::shared_ptr<arduinobot_msgs::srv::QuaternionToEuler::Response> res)
        {
            RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Received: x:" << req->x << " y: " << req->y << " z: " << req->z << " w: " << req->w);
            tf2::Quaternion q(req->x, req->y, req->z, req->w);
            tf2::Matrix3x3 m(q);
            double roll, pitch, yaw;
            m.getRPY(roll, pitch, yaw);
            res->roll = roll;
            res->pitch = pitch;
            res->yaw = yaw;
            RCLCPP_INFO_STREAM(rclcpp::get_logger("rclcpp"), "Returning: " <<res->roll << " pitch: " << res->pitch << " yaw: " << res->yaw); 
         }
};


int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<AnglesConverter>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}