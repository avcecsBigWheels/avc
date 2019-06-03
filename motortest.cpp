# include <stdio.h>
# include <time.h>
# include "E101.h"

int motorOne = 1;
int motorTwo = 5;
int fixedSpeed = 63;

class Robot {  
private:
public:
    void goForward();
};

void Robot::goForward () {
    set_motors (motorOne, 65);
    set_motors (motorTwo, 30);
    hardware_exchange();
}

int main() {
	Robot robot;	
	init(1); // set to 1 for debug messages, 0 for final release.	
	robot.goForward();
}	
