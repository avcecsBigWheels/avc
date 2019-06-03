# include <stdio.h>
# include <time.h>
# include "E101.h"
/*
 *  Kinda two options for going about making this
 *  we can have the start hardcoded aka always move x distance, towards 
 *  gate etc, and then send signal, or we can use 
 *  	while (read_analog(0)>250) from https://github.com/danysat1/ENGR101-AVC/blob/1a7723d3d4967d29eceb6be3a8da3ba62d04cbbf/AVC/mainQ3.cpp
 * 
 *  Either way, they should be able to call the gate file, idk how to make it a 
 *  class in c++ though, so pls help
 * 
 *  After take picture we need to scan the array of pixels in the image, and find
 *  the black ones to be able to make the robot move to the line
 *  once we have this working it should move on quite a bit faster tbh.
 * 
 */
int motorOne = 1;
int motorTwo = 5;
int fixedSpeed = 63;

class Robot {  
private:
public:
    void goForward();
};

void Robot::goForward () {
    set_motors (motorOne, 47);
    set_motors (motorTwo, 48);
    hardware_exchange();
}

int main() {
	Robot robot;	
	init(1); // set to 1 for debug messages, 0 for final release.	
	robot.goForward();
}	
