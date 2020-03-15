#include "ros/ros.h"
#include <std_msgs/Float64.h>
#include "geometry_msgs/Twist.h"
#include "ball_chaser/DriveToTarget.h"


// ROS::Publisher motor commands;
ros::Publisher motor_command_publisher;


// Publish Requested Velocities To Wheel Joints
// Return Message Feedback
bool handle_drive_request(ball_chaser::DriveToTarget::Request& req,
                          ball_chaser::DriveToTarget::Response& res)
{
	// Display Received Request
	ROS_INFO("DriveToTargetRequest Received -- Linear_X:%1.2f, Angular_Z:%1.2f",
              (float)req.linear_x, (float)req.angular_z);
	
	// Create motor_command Object of Type geometry_msgs::Twist
    geometry_msgs::Twist motor_command;
	
    // Pub Req To Robot Wheel Joints
	motor_command.linear.x = req.linear_x;
	motor_command.angular.z = req.angular_z;
	motor_command_publisher.publish(motor_command);
    
	// Return Message Feedback
    res.msg_feedback = "Linear X Set: " + std::to_string(req.linear_x) + ", Angular Z Set: " + std::to_string(req.angular_z);
	ROS_INFO_STREAM(res.msg_feedback);
	
	return true;
}


int main(int argc, char** argv)
{
    // Initialize ROS Node
    ros::init(argc, argv, "drive_bot");

    // Create a ROS NodeHandle object
    ros::NodeHandle n;

    // Inform ROS Master That We Will Be Publishing A Message Of Type geometry_msgs::Twist 
	// On Robot Actuation Topic With Publishing Queue Size of 10
    motor_command_publisher = n.advertise<geometry_msgs::Twist>("cmd_vel", 10);
	
    // Define Drive /ball_chaser/command_robot Service With handle_drive_request Callback
	ros::ServiceServer service = n.advertiseService("/ball_chaser/command_robot", handle_drive_request);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
