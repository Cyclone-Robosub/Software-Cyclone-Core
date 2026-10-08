# https://docs.ros.org/en/jazzy/Tutorials/Beginner-Client-Libraries/Writing-A-Simple-Py-Publisher-And-Subscriber.html

import rclpy
from rclpy.node import Node

from custom_interfaces.msg import Pwms
from std_msgs.msg import Empty


'''
When publish_pwm() is called, publishes a set of 8 pwms
pwm order: [flt, frt, rlt, rrt, flb, frb, rlb, rrb]
Used to send thruster commands via a Command Line Interface
'''
class CLIPublisher(Node):
	def __init__(self):
		super().__init__('pwm_cli')
		self.publisher_ = self.create_publisher(Pwms, 'pwm_cli', 10)

	def publish_pwm(self, pwms):
		msg = Pwms()
		msg.pwms = pwms
		self.publisher_.publish(msg)

'''
Publishes 'True' 4 times per second
Used to check whether connection between publisher and subscriber is lost
'''
class HeartbeatPublisher(Node):
	def __init__(self):
		super().__init__('heartbeat')
		self.publisher_ = self.create_publisher(Empty, 'cli_heartbeat', 10)
		timer_period = 0.25
		self.timer = self.create_timer(timer_period, self.timer_callback)

	def timer_callback(self):
		if (rclpy.ok()):
			msg = Empty()
			self.publisher_.publish(msg)
		