#include "ros/ros.h"
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <iostream>
#include "/opt/ros/kinetic/include/mavros_msgs/VFR_HUD.h"

int INIT_HEADING;
bool INIT = false;
ros::Publisher pub_ENU_pose;

void getHeading()
{
	boost::shared_ptr<const mavros_msgs::VFR_HUD> vmsg = ros::topic::waitForMessage<mavros_msgs::VFR_HUD>("/mavros/vfr_hud");
	INIT_HEADING = vmsg->heading;
	if (INIT_HEADING)
	{
	std::cout << "Initial heading: " << INIT_HEADING << std::endl;
	INIT = true;
	}
	else
	std::cout << "Cannot determine initial heading!";	
}

void odomCallback(const nav_msgs::OdometryConstPtr &odom)
{
	if (INIT == false)
	getHeading();
	float x = odom->pose.pose.position.x;
	float y = odom->pose.pose.position.y;
	float z = odom->pose.pose.position.z;
	
	float dist = sqrt(pow(x,2) + pow(y,2));
        float relativebearing = atan2(-y,x); //radians
	
	double init_heading_rad = (double) INIT_HEADING/180*3.141593; //conversion to rad
	
	double bearing = init_heading_rad + relativebearing;
        float east = sin(bearing) * dist;
        float north = cos(bearing) * dist;
	
	geometry_msgs::PoseStamped pose_stamped;
	//pose_stamped.header = header;
	pose_stamped.header.frame_id = "world";
	pose_stamped.pose.position.x = east;
	pose_stamped.pose.position.y = north;
	pose_stamped.pose.position.z = z;
	
	pub_ENU_pose.publish(pose_stamped);
}

int main(int argc, char **argv)
{
	 ros::init(argc, argv, "listener");
	 
	 ros::NodeHandle r;
	 pub_ENU_pose = r.advertise<geometry_msgs::PoseStamped>("mavros/fake_gps/vision", 1000);
	 
	 ros::NodeHandle n;
	 ros::Subscriber sub = n.subscribe("/camera/odom/sample", 1000, odomCallback);
	 
	 ros::spin();

	 return 0;
}




