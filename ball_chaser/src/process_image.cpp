#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <std_msgs/String.h>


// Define Global Client That Can Request Services
ros::ServiceClient client;

// Calls command_robot Service To Drive Robot In Specified Direction
void drive_robot(float lin_x, float ang_z)
{
	ROS_INFO_STREAM("Ball Acquired. Moving Toward Ball...");
	
    // Request Srv And Pass Velocities
	ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    // If Unable To Call Serive, Display Error
    if(!client.call(srv))
        ROS_ERROR("Failed To Call Service 'DriveToTarget'");
	
}


// Callback Function Continuously Executes And Reads Camera Data
void process_image_callback(const sensor_msgs::Image img)
{
    // Define Per Channel Value And Init Pixel Position Var
    int white = 255;
    int px_pos = 0;
    
    // Iterate Throughout Image
    for(int i=0; i<img.height*img.step; i+=3)
    {
        // Check If R-Channel Value Is 255
        // If So, Continue Through Remaining Px Values
        if(img.data[i] == white)
        {
           // Check Remaining Color Channels
            if(img.data[i+1] == white && img.data[i+2]==white)
            {
                // Record Pixel Position
                // Divide By Image Step To Reduce To Image Size
                // Break Loop, Px Found
                px_pos = i;
                px_pos = px_pos%img.step;
                break;
            }
        }
     

    
    }
    
    // Defualt Stop If Not Any Px Found
    // Command Bot Left If Found In Left Third
    // Command Bot Right If Found In Right Third
    // Command Bot Straight If Found In Center Third
    if(!px_pos)
    {
        
        drive_robot(0.0,0.0);   
    }
    else if(px_pos<(int)img.step/3)
    {
        drive_robot(0.1, 0.5);
    }
    else if(px_pos>(int)img.step*2/3)
    {
        drive_robot(0.1,-0.5);   
    }
    else
    {
        drive_robot(0.5,0.0);   
    }

}


int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
