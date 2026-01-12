import rclpy
from rclpy.action import ActionClient
from rclpy.node import Node
from arduinobot_msgs.action import Fibonacci

class SimpleActionClient(Node):
    def __init__(self):
        super().__init__("simple_action_client")
        self.action_client = ActionClient(self, Fibonacci, "fibonacci")
        self.get_logger().info("Client created")

        self.action_client.wait_for_server()
        

        self.goal = Fibonacci.Goal()
        self.goal.order = 10
        self.future = self.action_client.send_goal_async(self.goal, feedback_callback=self.feedbackCallback)
        self.future.add_done_callback(self.responseCallback)

    def responseCallback(self, future):
        goal_handle = future.result()
        if not goal_handle.accepted:
            self.get_logger().info("Goal reajecte by server ")
            return
        
        self.get_logger().info("Goal Accepted")
        self.future = goal_handle.get_result_async()
        self.future.add_done_callback(self.resultCallback)

    def resultCallback(self, future):
        result = future.result().result
        self.get_logger().info("Result: {0}".format(result.sequence))
        rclpy.shutdown()

    def feedbackCallback(self, feedback_msg):
        self.get_logger().info("Received Feddback: {0}".format(feedback_msg.feedback.partial_sequence))


def main():
    rclpy.init()
    action_client = SimpleActionClient()
    rclpy.spin(action_client)


if __name__ == '__main__':
    main()
