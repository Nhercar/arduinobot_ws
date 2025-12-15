#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from arduinobot_msgs.srv import QuaternionToEuler, EulerToQuaternion
from tf_transformations import quaternion_from_euler, euler_from_quaterion

class AnglesConverter(Node):
    def __init__(self):
        super().__init__("angle_conversion_service_server")


        self.quaternion_to_euler_ = self.create_service(QuaternionToEuler, "quaternion_to_euler", self.quaternionToEulerCallback)
        self.euler_to_quaternion_ = self.create_service(EulerToQuaternion, "euler_to_quaternion", self.eulerToQuaternionCallback)
        self.get_logger().info("Angle conversion services ready")

    
    def eulerToQuaternionCallback(self, req, res):
        self.get_logger().info("Requeste to convert euler angles roll: %f, pitch: %f, yaw: %f to quaternion." % (req.roll, req.pitch, req.yaw))
        (res.x, res.y, res.z, res.w) = quaternion_from_euler(req.roll, req.pitch, req.yaw)
        self.get_logger().info("Response -> x: %f, y: %f, z: %f, w: %f to quaternion." % (res.x, res.y, res.z, res.w))




    def quaternionToEulerCallback(self, req, res):
        self.get_logger().info("Convert -> x: %f, y: %f, z: %f, w: %f to euler." % (req.x, req.y, req.z, req.w))
        (res.roll, res.pitch, res.yaw) = euler_from_quaterion(req.x, req.y, req.z, req.w)
        self.get_logger().info("Response Roll: %f, pitch: %f, yaw: %f to quaternion." % (res.roll, res.pitch, res.yaw))
        return res
    
       

def main():
    rclpy.init()
    angle_conversion_servirce_server = AnglesConverter()
    rclpy.spin(angle_conversion_servirce_server)
    angle_conversion_servirce_server.destroy_node()
    rclpy.shutdown()



if __name__ == '__main__':
    main()