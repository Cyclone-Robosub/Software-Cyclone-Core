'''
Takes user input from the command line and publishes a pwm_cli 
'''

import time
import threading
import math
import copy

from .cli_publisher import *

# Global variables
cli = None
default_power = 70
timer_running = False
current_command = None
previous_commands = []
timed_command_thread = None

# pwm constants
PWM_ZERO = 1500
EMERGENCY_BRAKES = [PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO]


def main():
	global current_command
	global default_power
	global cli
	global timed_command_thread
	global timer_running
	global previous_commands

	default_power = 70
	current_command = RobotCommand("Stop", default_power, -1, EMERGENCY_BRAKES)
	previous_commands = []

	rclpy.init(args=None)

	cli = CLIPublisher()

	# Create a seperate thread to spin the heartbeat
	heartbeat_thread = threading.Thread(name="heartbeat_thread", target=spin_heartbeat)
	heartbeat_thread.start()

	# Prints usage info to screen
	info()

	reading_input = True

	'''
	Parses user input and takes action or translates into RobotCommand
		If action is taken, the result is printed to the screen
		If a RobotCommand is created, after confirmation the relevant pwms are published
		Exception is the 'Stop' command, which is a RobotCommand that gets processed immediately
	Ends excecution when the user types 'end session'
	'''
	while (reading_input):
		user_input = input("Input a command: ").lower()

		if (user_input == "end session"):
			break

		# Check whether user input is a valid command
		command = translate_command(user_input)

		# if an invalid command is inputted, warn the user
		if command is None:
			print(f"{user_input} is not a valid command. Type 'help' for valid commands.")
			continue

		# Valid non-robot commands should output their result
		elif isinstance(command, str):
			print(command)
		
		# A stop command should be processed immediately
		elif "Stop" in command.name:
			cli.publish_pwm(command.pwm)
			current_command = command
			previous_commands.append(current_command)
		
		# A robot command should be processed after the user confirms it was intended
		elif command.confirm_command():
			if timed_command_thread is not None and timed_command_thread.is_alive():
				timer_running = False
				timed_command_thread.join()
			cli.publish_pwm(command.pwm)
			if (command.time != -1):
				timed_command_thread = threading.Thread(name="timed_command_thread", target=run_command_timer, args=[command.time])
				timer_running = True
				timed_command_thread.start()
			current_command = command
			previous_commands.append(current_command)
	# End of while loop

	reading_input = False

	# Shut down timer thread if it exists
	if timed_command_thread is not None and timed_command_thread.is_alive():
		timer_running = False
		timed_command_thread.join()

	# Stop robot before shutting down cli
	cli.publish_pwm(EMERGENCY_BRAKES)

	time.sleep(0.1)
	rclpy.shutdown()

	print("Goodbye!")


'''
Reads a string and takes the action requested or outputs a robot command
command is the string to process
Returns a robot command or the result of the non-robot action taken
'''
def translate_command(command):

	if "info" in command or "help" in command:
		info()
		return ""
	
	if "history" in command:
		print("Command History:\n")
		print("Note: this only shows *correctly input and accepted* robot commands, *not* user commands.\n")
		for cmd in previous_commands:
			print(f"{cmd.user_input}\n")
		return ""

	global default_power
	
	# Non-Robot Commands
	if "set" in command and "power" in command:
		new_power = find_num_in_string(command[command.index("power"):])
		if new_power == None or int(new_power) > 100 or int(new_power) == 0:
			return "Invalid default power inputted\n"
		default_power = new_power
		return f"Set default power to {new_power}%\n"

	if "current" in command and "command" in command:
		return get_current_command()


	cmd = RobotCommand(power = default_power)
	# Setting user input to list in the `history` command
	cmd.user_input = command

	# Changing settings for a given robot command
	if "power:" in command:
		power = find_num_in_string(command[command.index("power:"):])
		if power is not None and int(power) <= 100:
			cmd.power = power
		else:
			return "Invalid power inputted\n"
	elif "p:" in command:
		power = find_num_in_string(command[command.index("p:"):])
		if power is not None and int(power) <= 100:
			cmd.power = power
		else:
			return "Invalid power inputted\n"

	if "time:" in command:
		cmd.time = find_num_in_string(command[command.index("time:"):])
	elif "t:" in command:
		cmd.time = find_num_in_string(command[command.index("t:"):])
	if cmd.time is None:
		return "Invalid time inputted\n"

	# Robot Commands
	if "stop" in command and "thruster" in command:
		# If there is not a run thruster command running, just send a stop set.
		if (current_command is None) or ("Thruster" not in current_command.name):
			cmd.name = "Stop"
			cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
			return cmd
		
		cmd.name = "Stop Thruster"
		# Copy the pwms of the previous [run thruster] command
		cmd.pwm = current_command.pwm

		# Get and validate thruster number
		thruster_num = find_num_in_string(command[command.index("thruster"):])
		if thruster_num is None:
			return "No thruster number inputted\n"
		thruster_num = int(thruster_num)
		if thruster_num > 7:
			return f"Invalid thruster number inputted: {thruster_num}, max 7\n"
		cmd.pwm[thruster_num] = PWM_ZERO

		# After updating the pwm, if the pwm set is a stop set, change the command to a stop command
		if cmd.pwm == cmd.command_dictionary()["Stop"]:
			cmd.name = "Stop"

		# Ignore custom time if provided
		if cmd.time != -1:
			print("Note: Custom times are ignored for thruster commands.\n")
			cmd.time = -1
		
		# Indicate which thruster was changed (and to what) for the prev command
		cmd.thruster = thruster_num
		cmd.thruster_pwm = cmd.pwm[thruster_num]
		
		return cmd
	if "stop" in command:
		return RobotCommand("Stop", default_power, -1, EMERGENCY_BRAKES, command)
	if "forwards" in command:
		cmd.name = "Move Forwards"
		cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
		return cmd
	if "backwards" in command:
		cmd.name = "Move Backwards"
		cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
		return cmd
	if "strafe" in command and "left" in command:
		cmd.name = "Strafe Left"
		cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
		return cmd
	if "strafe" in command and "right" in command:
		cmd.name = "Strafe Right"
		cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
		return cmd
	if "rise" in command:
		cmd.name = "Rise"
		cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
		return cmd
	if "sink" in command:
		cmd.name = "Sink"
		cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
		return cmd
	if "yaw" in command and (("counter" in command and "clockwise" in command) or "ccw" in command):
		cmd.name = "Yaw Counterclockwise"
		cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
		return cmd
	if "yaw" in command and ("clockwise" in command or "cw" in command):
		cmd.name = "Yaw Clockwise"
		cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
		return cmd
	if "pitch" in command and "up" in command:
		cmd.name = "Pitch Up"
		cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
		return cmd
	if "pitch" in command and "down" in command:
		cmd.name = "Pitch Down"
		cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
		return cmd
	if "roll" in command and "left" in command:
		cmd.name = "Roll Left"
		cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
		return cmd
	if "roll" in command and "right" in command:
		cmd.name = "Roll Right"
		cmd.pwm = cmd.command_dictionary()[f"{cmd.name}"]
		return cmd

	if "run" in command and "thruster" in command:
		# If the previous command was not a run/stop thruster command, initialize the new command pwm set to a stop set
		if current_command is None or "Thruster" not in current_command.name:
			cmd.pwm = cmd.command_dictionary()["Stop"]
		# ... Otherwise, copy it from the previous commands
		else:
			cmd.pwm = current_command.pwm

		# Ignore custom time if provided
		if cmd.time != -1:
			print("Note: Custom times are ignored for thruster commands.\n")
			cmd.time = -1
		
		cmd.name = "Run Thruster"
		
		# Get and validate thruster number
		thruster_num = find_num_in_string(command[command.index("thruster"):])
		if thruster_num is None:
			return "No thruster number inputted\n"
		thruster_num = int(thruster_num)
		if thruster_num > 7:
			return f"Invalid thruster number inputted: {thruster_num}, max 7\n"
		
		# Handle explicitly pwm value
		if "pwm:" in command:
			pwm_val = find_num_in_string(command[command.index("pwm:"):])
			if pwm_val is None:
				return "No thruster pwm inputted\n"
			pwm_val = int(pwm_val)
			if pwm_val > 1900 or pwm_val < 1100:
				return f"Invalid thruster pwm inputted: {pwm_val}\n"

			cmd.pwm[thruster_num] = pwm_val

		# If there is no pwm value given, use default power
		else:
			cmd.pwm[thruster_num] = math.floor(PWM_ZERO + 400 * (int(cmd.power) / 100))

		# Indicate which thruster was changed (and to what) for the confirmation and for the prev command
		cmd.thruster = thruster_num
		cmd.thruster_pwm = cmd.pwm[thruster_num]

		return cmd

	# custom pwm syntax: "Custom pwm [flt, frt, rlt, rrt, flb, frb, rlb, rrb]"
	if "custom" in command and '[' in command and ']' in command:
		cmd.name = "Custom pwm"

		# Let the substring be the command starting at the opening square bracket
		ss = command[command.index("["):]
		
		# Ensures the pwms will not be read past the closing square bracket ']'
		ending_len = len(command[command.index("]"):])

		pwms = []
		# While there are still numbers in the pwm array, search for the next pwm in the substring starting after the previous pwm
		while ss and len(ss) > ending_len:
			pwms.append(find_num_in_string(ss))
			# If another number is not read in the string
			if (pwms[-1] is None):
				pwms.pop()
				break
			# Update the substring to remove the just-added number
			ss = ss[ss.index(f"{pwms[-1]}") + len(pwms[-1]):]

		cmd.pwm = pwms
		# If 8 pwms were entered and every pwm is a valid pwm,
		if (len(cmd.pwm) == 8) and (all(i is not None and int(i) >= 1100 and int(i) <= 1900 for i in cmd.pwm)):
			return cmd
		else:
			# Prints: "Invalid custom pwms inputted: [...] (Note: X pwms recieved, expected 8)\n"
			return f"Invalid custom pwms inputted: {cmd.pwm}" +\
				   (f" (Note: {len(cmd.pwm)} pwm" + ( "" if len(cmd.pwm) == 1 else "s") + " recieved, expected 8)\n"
				   if (not len(cmd.pwm) == 8)
				   else "\n")

	if "prev" in command:
		if len(previous_commands) == 0:
			return "No previous command to run.\n"
		
		# Calculate index of the command to run
		num_back = find_num_in_string(command)
		if num_back is None or int(num_back) < 1:
			num_back = 1
		num_back = int(num_back)
		i = len(previous_commands)-num_back
		if i < 0:
			return f"Invalid index inputted, max {len(previous_commands)}\n"
		
		original_user_input = cmd.user_input
		# Need to deep copy the command, so the command is not modified within the history array
		cmd = copy.deepcopy(previous_commands[i])
		# Dealing with run thruster
		if "Thruster" in cmd.name:
			# Simplified/modified version of the run thruster command
			if current_command is None or "Thruster" not in current_command.name:
				cmd.pwm = cmd.command_dictionary()["Stop"]
			else:
				cmd.pwm = current_command.pwm
			cmd.pwm[cmd.thruster] = cmd.thruster_pwm
			print(f"Command to Run: {cmd.name} {cmd.thruster}{" at " + str(cmd.thruster_pwm) if "Stop" not in cmd.name else ""} with result: {get_running_thruster_list(cmd)[0]}\n")
		
		else:
			print(get_robot_command(cmd, "Command to Run: "))
		
		cmd.user_input = f"{original_user_input} AKA {cmd.user_input}"
		
		return cmd

	return None

'''
Prints a list of all valid commands and examples of how to use them
Does not return a value
'''
def info():
	print("Notes:")
	print("\tWhitespace is ignored in command names")
	print("\tDefault power is 70%")
	print("\n")
	print("Valid User Commands:")
	print("\t'set power {num}'\t\t changes default power for robot commands (as a percentage)")
	print("\t'current command'\t\t prints out the currently active command")
	print("\t'info' or 'help'\t\t resends this message")
	print("\t'history'\t\t displays all previously executed robot commands")
	print("\t'end session'\t\t\t ends program excecution")
	print("\n")
	print("Valid Robot Commands:")
	print("\tstop | forwards | backwards | strafe left | strafe right | rise | sink |")
	print("\tyaw counter clockwise (yaw ccw) | yaw clockwise (yaw cw) | pitch up | pitch down | roll left | roll right")
	print("\tcustom [{pwm}, {pwm}, {pwm}, {pwm}, {pwm}, {pwm}, {pwm}, {pwm}]")
	print("\trun thruster {number} | stop thruster {number} | previous (prev)")
	print("\n")
	print("All robot commands have optional power and time fields:")
	print("\t'power: {num}' or 'p: {num}' for a custom power (as a percentage)")
	print("\t'time: {num}' or 't: {num}' for a timed command (in seconds)")
	print("")
	print("Run thruster has an optional pwm field:")
	print("\t'run thruster pwm: {pwm}")
	print("")
	print("Previous has an optional field for the amount to go back in the history:")
	print("\t'previous 5' or 'prev 5'")
	print("\n")
	print("Examples:")
	print("\tForwards p:50 t: 10\t\tForwards at 50% power for 10 seconds")
	print("\tstrafe-left time: 3\t\tStrafe Left at default power for 3 seconds")
	print("\tROLL_RIGHT power: 80\t\tRoll Right at 80% power until stopped")

	print("\n\n")

'''
Describes the currently running RobotCommand, or says no commands are running
Returns a string describing current command to user
'''
def get_current_command():
	global current_command
	if current_command.name == "Stop":
		return "There is no currently active command\n"
	
	return get_robot_command(current_command, "Current Command: ")

'''
Gets the string for a given command. Abstracted from get_current_command to allow printing when running the "prev" command.
'''
def get_robot_command(cmd, prefix):
	# Note that although this is different from the stop string to print for get current command,
	# that specific case is handled in get_current_command before this function is called.
	# This is to ensure that a power value is not printed alongside a stop command, as that would
	# not make sense.
	if cmd.name == "Stop":
		return f"{prefix}Stop\n"

	# Current command string start is dependent on whether the command is a custom pwm
	str_start = f"{prefix}Custom pwm {cmd.pwm}" \
				if cmd.name == "Custom pwm" \
				else (get_thruster_command_text(cmd, prefix) \
				if "Thruster" in cmd.name \
				else f"{prefix}{cmd.name} at {cmd.power}% power")
	
	# Current command string end is dependent on whether the command is timed
	str_end = "\n" if cmd.time == -1 else f" for {cmd.time} seconds\n"

	return str_start + str_end


'''
Due to the complexity of the current command text for the run thruster command,
I have broken it out into a seperate function to not clutter get_current_command.

Returns the thruster command text.
'''
def get_thruster_command_text(cmd, prefix):
	(thrusters_string, num_thrusters) = get_running_thruster_list(cmd)
	return f"{prefix}Run Thruster" + ("s " if num_thrusters > 1 else " ") + thrusters_string

def get_running_thruster_list(cmd):
	num_thrusters = 0
	thrusters_string = ""
	# For each nonzero thruster, add its number and value to the string
	for i in range(len(cmd.pwm)):
		cur_pwm = cmd.pwm[i]
		if cur_pwm != 1500:
			num_thrusters += 1
			if num_thrusters != 1:
				thrusters_string += (", ")
			thrusters_string += f"{i} at {cur_pwm}"
	return (thrusters_string, num_thrusters)

'''
Looks through a string for the first number in it
string is the string to look through
Returns the number found, or None
'''
def find_num_in_string(string):
	started = False
	has_decimal_point = False
	num = ""

	for char in string:
		if char.isdigit() or (char == '.' and not has_decimal_point):
			num += str(char)
			if char == '.':
				has_decimal_point = True
			if not started:
				started = True
		elif started:
			break
	if num == "" or num == ".":
		return None
	return num

'''
Waits for a timer with a given duration to finish, then sends a stop command
	While waiting, checks whether it needs to terminate early without publishing
duration is the duration the timer will wait in seconds
Does not return a value
'''
def run_command_timer(duration):
	start_time = time.time()
	global timer_running
	while time.time() - start_time < float(duration):
		if not timer_running:
			return
	global current_command
	global cli
	current_command = RobotCommand("Stop")
	cli.publish_pwm(current_command.command_dictionary()["Stop"])

'''
Spins the HeartbeatPublisher
Used as a threading target, as thread(rclpy.spin) crashes program
'''
def spin_heartbeat():
	while (not rclpy.ok()):
		pass
	rclpy.spin(HeartbeatPublisher())
	

		
'''
Ask user for confirmation until valid response is given
prompt is the statement asking user to confirm command
Returns True if response is yes, or False if no
'''
def confirm(prompt):
	while True:
		response = input(prompt).lower()
		if "yes" in response or response == "y":
			return True
		elif "no" in response or response == "n":
			return False


'''
Used to store the name, power, time, and pwm set for a thruster command
name is the name of the command
power is the percent of full power the command should run at (0-100%)
time is the amount of the time the command should run for, or -1 if it is untimed
pwm is the set of 8 pwms to be sent to the thrusters
'''
class RobotCommand():
	def __init__(self, name = "Stop", power = default_power, time = -1, pwm = EMERGENCY_BRAKES, user_input = "USER INPUT MISSING"):
		self.name = name
		self.power = power
		self.time = time
		self.pwm = pwm
		self.user_input = user_input
	
	'''
	Calculates the pwm sets for all commands using the RobotCommand's power
	Returns a dictionary with all calculated pwm sets obtained via their name
	'''
	# NOTE: Could be made more efficient by remaking to check self.name and return only that set
	def command_dictionary(self):
		pwm_fwd = math.floor(PWM_ZERO + 400 * (int(self.power) / 100))
		pwm_rev = math.ceil(PWM_ZERO - 400 * (int(self.power) / 100))
		# pwm order: [flt, frt, rlt, rrt, flb, frb, rlb, rrb]
		return {
			"Stop" : [PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO],
			"Move Forwards" : [PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, pwm_fwd, pwm_rev, pwm_fwd, pwm_rev],
			"Move Backwards" : [PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, pwm_rev, pwm_fwd, pwm_rev, pwm_fwd],
			"Strafe Left" : [PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, pwm_rev, pwm_rev, pwm_fwd, pwm_fwd],
			"Strafe Right" : [PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, pwm_fwd, pwm_fwd, pwm_rev, pwm_rev],
			"Rise" : [pwm_fwd, pwm_rev, pwm_fwd, pwm_rev, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO],
			"Sink" : [pwm_rev, pwm_fwd, pwm_rev, pwm_fwd, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO],
			"Yaw Counterclockwise" : [PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, pwm_rev, pwm_rev, pwm_rev, pwm_rev],
			"Yaw Clockwise" : [PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO, pwm_fwd, pwm_fwd, pwm_fwd, pwm_fwd],
			"Pitch Up" : [pwm_fwd, pwm_rev, pwm_rev, pwm_fwd, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO],
			"Pitch Down" : [pwm_rev, pwm_fwd, pwm_fwd, pwm_rev, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO],
			"Roll Left" : [pwm_rev, pwm_rev, pwm_rev, pwm_rev, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO],
			"Roll Right" : [pwm_fwd, pwm_fwd, pwm_fwd, pwm_fwd, PWM_ZERO, PWM_ZERO, PWM_ZERO, PWM_ZERO]
		}

	'''
	Prints a confirmation message to screen and waits for the user to respond 'yes' or 'no'
	self is the RobotCommand that needs to be confirmed
	Returns whether the command should be run
	'''
	def confirm_command(self):
		# Confirmation string start is dependent on whether the command is a custom pwm
		str_start = f"Are you sure you want to run the Custom pwm {self.pwm}" \
					if self.name == "Custom pwm" \
					else (\
					f"Are you sure to want to run thruster {self.thruster} at {self.pwm[self.thruster]}"
					if "Thruster" in self.name \
					else f"Are you sure you want to {self.name} at {self.power}% power")
		# Confirmation string end is dependent on whether the command is timed
		str_end = " until stopped? [(y)es/(n)o]\n" if self.time == -1 else f" for {self.time} seconds? [(y)es/(n)o]\n"
		confirm_str = str_start + str_end

		return confirm(confirm_str)
