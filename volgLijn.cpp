#include "BrickPi3.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <iomanip>
//rechts is kleursensor 1
//links is lichtsensor3
//rechts is motor b
//links is motor c

// <2100 is wit
// >500 is wit
using namespace std;
BrickPi3 			BP;
sensor_light_t		leftLightSensor;
sensor_color_t		rightColorSensor;
sensor_ultrasonic_t	middleUltrasonicSensor;

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    
    exit(-2);
  }
}

void init(){
	signal(SIGINT, exit_signal_handler);
	BP.detect();
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
 }
 
 
int main(){
	init();

	int standardSpeed=100;
	int turnSpeed=20;
	int stopSpeed=0;

	while(true){
		BP.get_sensor(PORT_1, rightColorSensor);
		BP.get_sensor(PORT_2, middleUltrasonicSensor);
		BP.get_sensor(PORT_3, leftLightSensor);
		cout<<"rgb sensor"<<
		if(middleUltrasonicSensor.cm > 15){
			if(rightColorSensor.reflected_red>500){
				if(leftLightSensor.reflected<2200){
					cout<<"links"<<endl;
					BP.set_motor_power(PORT_B,standardSpeed);
					BP.set_motor_power(PORT_C,turnSpeed);
				}
				else{
					cout<<"rechtdoor"<<endl;
					BP.set_motor_power(PORT_C,standardSpeed);
					BP.set_motor_power(PORT_B,standardSpeed);
				}
			}
			else if(rightColorSensor.reflected_red<500){
				if(leftLightSensor<2200){
					cout<<"rechts"<<endl;
					BP.set_motor_power(PORT_C,standardSpeed);
					BP.set_motor_power(PORT_B,turnSpeed);
				}
			}
		}
		else{
			cout<<"obstakel"<<endl;
			BP.set_motor_power(PORT_B,stopSpeed);
			BP.set_motor_power(PORT_C,stopSpeed);
		}
	}
}


