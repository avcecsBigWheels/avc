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
bool quad2 = true;
bool quad3 = false;

class Robot {  
private:
	int v_left, v_right, cam_tilt;
	float dv;
	float de;
	float whiteness;
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
	bool junction;
	int vertLine [120] = {};
	int horizLine [320] = {};
	bool vLine;
	bool hLine;
	bool deadEnd;
	bool goStraight;
	bool endRun;
public:
	//Rob () {};    //default constructor
	int InitHardware ();
	void SetMotors ();
	int MeasureLine ();
	int FollowLine ();
	void goForward();
	void MeasureColor();
	void maze ();
	void MeasureMaze();
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
	int line;
	line_error = 0;		
	line_present = false;
	if (quad2) {
		whiteness = 0;
		for (int i = 0; i < 320; i++) {
			whiteness += get_pixel (cam_height/2, i, 3);
		}
		whiteness /= cam_width;
	}
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
	}	
	clock_gettime (CLOCK_MONOTONIC, &ts_end);	
	return 0;
}
void Robot::MeasureMaze () {
	//variable setting
	junction = false;
	deadEnd = false;
	goStraight = false;
	line_present = false;	
	
	vertLine [120] = {};
	horizLine [320] = {};
	
	vLine = false;
	hLine = false;
	bool rLine;
	bool lLine;
	
	
	int leftLine = 0;
	int rightLine = 0;
	int lineH = 0;
	int lineV = 0;
	
	//whiteness check
	whiteness = 0;	
	float vertWhiteness = 0;
	for (int i = 0; i < 320; i++) {
		whiteness += get_pixel (cam_height/2, i, 3);
	}
	for (int i = 120; i < 240; i++) {
		vertWhiteness += get_pixel (i, 160, 3);
	}
	vertWhiteness /= 120;
	whiteness /= cam_width;
	
	//vertical line check
	for (int i = 120; i < 240; i ++) {
		if (get_pixel (120, i, 3) > vertWhiteness - 15) {
			vertLine[i - 120] = 0;
		}
		else {
			vertLine [i - 120] = 1;
		}
		lineV += vertLine [i - 120];
	}
	
	//horizontal line check
	for (int i = 0; i < 320; i++) {
		if (get_pixel (120, i, 3) > whiteness - 15) {
			horizLine[i] = 0;
		}
		else {
			horizLine[i] = 1; 
			line_present = true;
		}
		if (i > 160) {
			leftLine += horizLine [i];
		}
		else {
			rightLine += horizLine[i];
		}
		lineH += horizLine[i];
	}
	hLine = lineH > 60;
	vLine = lineV > 10;
	lLine = leftLine > 50;
	rLine = rightLine > 50;
	goStraight = ((lLine && !rLine) || (!lLine && rLine)) && lineV > 15;
	junction = lineV > 30 && lineH > 100;
	deadEnd = lineH < 20 && lineV < 20;
	printf ("Vertical Amnt: %d\nHorizontal Amnt: %d\n", lineV, lineH);
}
int Robot::FollowLine () {
	 
	MeasureColor ();
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
void Robot::MeasureColor () {
	float red = 0;
	float green = 0;
	float blue = 0;
	for (int i = 120; i < 200; i ++) {
		for (int k = 80; k < 160; k ++) {
			red += (float) (get_pixel (k, i, 0));	
			green += (float) (get_pixel (k, i, 1));	
			blue += (float) (get_pixel (k, i, 2));	
		}
	}
	red /= 6400;
	green /= 6400;
	blue /= 6400;
	if ((red - blue) > 80) {
		quad2 = false;
		quad3 = true;
	}
	if (quad3 && ((red - blue) > 80)) {
		endRun = true;
	}
}
void Robot::maze() {
	MeasureColor();
	MeasureMaze();
	if (junction) {
		sleep1 (150);
		v_right = 58;
		v_left = 50;
		sleep1 (300);
		printf ("Junction\n");
	}
	else if (deadEnd) {
		v_right = 58;
		v_left = 54;
		SetMotors();
		sleep1 (300);
		printf ("DeadEnd\n");
	}
	else if (goStraight) {
		goForward();
		sleep1 (325);
		printf ("Go forward\n");
	}
	else {
		FollowLine();	
		printf ("Follow Line\n");
	}
}

int main() {
	Robot robot;
	
	init(0); // set to 1 for debug messages, 0 for final release.
	open_screen_stream();
	
	//char ip[24] = {'1','3','0','.','1','9','5','.','6','.','1','9','6'};
    	//connect_to_server(ip,1024);
	//char message[24] = {'P','l','e','a','s','e'};
	//send_to_server(message);
	//receive_from_server(message);
	//send_to_server(message);// literally a ping pong
	
	//robot.goForward();
	//sleep1(3000);	
	
	while (quad3) { // sets up a loop for the rest of our stuff to be in
		// this should call camera to take a ss.
		take_picture();
		robot.FollowLine();		
		// for(x pixel) decide which direction to move		
	}
	while (quad2) {
		take_picture();
		robot.maze();	
	}	
}	
