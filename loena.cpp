#include "BrickPi3.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <iomanip>

using namespace std;
BrickPi3 BP;
sensor_light_t      	Light3;
sensor_color_t 		Color1;
sensor_ultrasonic_t	Ultrasonic2;
int direction = 0;
int rightdir = 0;
int righttime = 4;
int lefttime = 4;
void exit_signal_handler(int signo);

void straight(){
	BP.set_motor_power(PORT_C,25);
	BP.set_motor_power(PORT_B,25);
	sleep(1);
}

void right(){
        BP.set_motor_power(PORT_C,70);
	BP.set_motor_power(PORT_B,0);
        sleep(1);
	while(1){
		BP.get_sensor(PORT_3, Light3);
		if(Light3.reflected>2000){
			BP.set_motor_power(PORT_C,0);
			break;
		}
	}
}

void left(){
        BP.set_motor_power(PORT_B,70);
        BP.set_motor_power(PORT_C,0);
	sleep(1);
        while(1){
                BP.get_sensor(PORT_3, Light3);
                if(Light3.reflected>2000){
                        BP.set_motor_power(PORT_B,0);
                        break;
                }
        }
}

void cross(){
	BP.set_motor_power(PORT_C,25);
	BP.set_motor_power(PORT_B,25);
	while(1){
		if(BP.get_sensor(PORT_1, Color1)==0){
			BP.get_sensor(PORT_3, Light3);
			if((Light3.reflected > 2100) && (Color1.reflected_red<500)){
				BP.set_motor_power(PORT_B,0);
				BP.set_motor_power(PORT_C,0);
				break;
			}
			if(Light3.reflected<1900){
				BP.set_motor_power(PORT_B,0);
				BP.set_motor_power(PORT_C,25);
			}
			if(Light3.reflected>2200){
				BP.set_motor_power(PORT_C,0);
				BP.set_motor_power(PORT_B,25);
			}
			else{
				BP.set_motor_power(PORT_B,25);
				BP.set_motor_power(PORT_C,25);
			}
		}
	}
}

void check(){
	cout << "Ik ga checken" << endl;
	BP.set_motor_position(PORT_A,0);
	sleep(1);
	cout << BP.get_sensor(PORT_2, Ultrasonic2)<< endl;;
        if(BP.get_sensor(PORT_2, Ultrasonic2)==0){
		cout << "C";
		cout << Ultrasonic2.cm << endl;
	        if(Ultrasonic2.cm < 10){
                        cout << "Er is iets voor me"<<endl;
                	right();
			cross();
			direction+=1;
			check();
			return;
		}
        }
	BP.set_motor_position(PORT_A,90);
	sleep(1);
	cout<<BP.get_sensor(PORT_2, Ultrasonic2)<<endl;
	if(BP.get_sensor(PORT_2, Ultrasonic2)==0){
		cout << Ultrasonic2.cm << endl;
		if(Ultrasonic2.cm < 10){
			BP.set_motor_position(PORT_A,0);
			cout << "Er is iets rechts van me"<<endl;
			straight();
			cross();
			check();
			return;
		}
	}
	BP.set_motor_position(PORT_A,-90);
        sleep(1);
	cout<<BP.get_sensor(PORT_2, Ultrasonic2)<<endl;
	if(BP.get_sensor(PORT_2, Ultrasonic2)==0){
		cout << Ultrasonic2.cm << endl;
                if(Ultrasonic2.cm < 10){
  			BP.set_motor_position(PORT_A,0);
                        cout << "Er is iets links van me"<<endl;
                	straight();
			cross();
			check();
			return;
		}
        }
	BP.set_motor_position(PORT_A,0);
	while(direction>0){
		if(direction==1){
			left();
			righttime=1;
			cout <<righttime<<endl;
			if(rightdir==1){
				cross();
				righttime--;
				left();
				cross();
				right();
			}
			direction=0;
		}
		if(direction==2){
			left();
			cross();
			left();
			cross();
			if(rightdir==1){
				right();
			}
			direction=0;
		}
	}
}

int main(){
	signal(SIGINT, exit_signal_handler);
	BP.detect();
	BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_RED);
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
	BP.set_motor_power(PORT_B,25);
	BP.set_motor_power(PORT_C,25);
	while(1){
		while(lefttime>0){
			cross();
			check();
			straight();
			lefttime--;
		}
		cross();
		right();
		cross();
		check();
		rightdir = 1;
		while(righttime>0){
			cout<<righttime<<endl;
			straight();
			cross();
			check();
			righttime--;
		}
		straight();
		cross();
		break;
	}
    	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
}

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();
    exit(-2);
  }
}
