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
	int v_left, v_right, cam_tilt;
	float dv;
	float de;
	long dt;
	int error;
	struct timespec ts_start;
	struct timespec ts_end;
	double line_error;
	double previous_line_error;
	double change_in_time;
	int quadrant;
	const int cam_width = 320;
	const int cam_height = 240;
	const int v_left_go = 52;
	const int v_right_go = 43;
	double kp = 0.0008;
	double kd = 0.0007;
	bool line_present = true;
public:
	//Rob () {};    //default constructor
	int InitHardware ();
	void SetMotors ();
	int MeasureLine ();
	int FollowLine ();
	void goForward();
	//int forward(int speed);
};

/*int Robot :: forward(int speed){
		// Motors are bound to pins, 1 and 2 should work ok to define
		// which motor to run
		// speeds sould be <255
		if(speed < 255){
			set_motors(motorOne,speed);
			set_motors(motorTwo,speed);
			hardware_exchange(); // should apply the new speed to motors? maybe wrong name
			}
		else{
			printf("Robot Speed Should be under 255, stopping..");
			}
		}*/
void Robot::SetMotors () {
	set_motors (1, v_right);
	set_motors (5, v_left);
	hardware_exchange();
}

void Robot::goForward () {
	set_motors (1, 43);
	set_motors (5, 52); //63
	hardware_exchange();
}

int Robot :: MeasureLine(){
	//int line [cam_width] = {};
	take_picture();
	int line;
	line_error = 0;
	float whiteness = 0;
	line_present = false;
	for (int i = 0; i < 320; i++) {
		whiteness += get_pixel (cam_height/2, i, 3);
	}
	whiteness /= cam_width;
	clock_gettime (CLOCK_MONOTONIC, &ts_start);
	for (int i = 0; i < 320; i++) {
		if (get_pixel (120, i, 3) > whiteness - 15) {
			//line[i] = 1;
			line = 0;
	}
		else {
			//line[i] = 0; 
			line = 1;
			line_present = true;
		}
		//line_error += line[i] * (i - ((cam_width - 1) / 2));
		line_error += (double)(line * (i - 159.5));
		double centre = i - 159.5;
		printf ("Line: %d\n", line);
		printf ("Centre: %.2f\n", centre);
		printf ("Line Error!!!!: %d\n", line_error);
	}	
	clock_gettime (CLOCK_MONOTONIC, &ts_end);
	return 0;
}
int Robot::FollowLine () {
	 
	MeasureLine ();	
	if (line_present) {
		dv = (line_error * kp);
		de = (line_error - previous_line_error);
		dt = (ts_end.tv_sec - ts_start.tv_sec) * 1000000000 +
		ts_end.tv_nsec - ts_start.tv_nsec;
		error = (int)(dv + (kd * (de/dt)));
		v_left = v_left_go + error;
		v_right = v_right_go + error;
		if (v_left > 65) {
			v_left = 65;
		}
		else if (v_left < 30) {
			v_left = 30;
		}
		if (v_right > 65) {
			v_right = 65;
		}
		else if (v_right < 30) {
			v_right = 30;
		}
		previous_line_error = line_error;
		printf ("Error: %d\n", error);
		printf ("V_left: %d\nV_right: %d\n", v_left, v_right);
		SetMotors ();
	}
	else {
		// go back
		//cout << " Line missing " << endl;
		v_left = 44;
		v_right = 53;
		SetMotors ();
		sleep1 (100);
	}
}

int main() {
	Robot robot;
	printf("Program Started..");
	
	init(1); // set to 1 for debug messages, 0 for final release.
	/*
	open_screen_stream();
	
	char ip[24] = {'1','3','0','.','1','9','5','.','6','.','1','9','6'};
    	connect_to_server(ip,1024);
	char message[24] = {'P','l','e','a','s','e'};
	send_to_server(message);
	receive_from_server(message);
	send_to_server(message);// literally a ping pong
	*/
	
	robot.goForward();
	sleep1(750);
	
	
	while(true){ // sets up a loop for the rest of our stuff to be in
		// this should call camera to take a ss.
		robot.FollowLine();
		
		// for(x pixel) decide which direction to move
		
		}
	
	}
		
