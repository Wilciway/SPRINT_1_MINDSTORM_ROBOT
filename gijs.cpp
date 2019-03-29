#include "BrickPi3.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <iomanip>
#include <string>

using namespace std;
BrickPi3 BP;

int logic_count = -1;
string status = "idle";

void exit_signal_handler(int signo);

void check(const int direction){
        // Left
        if(direction == 0){
                BP.set_motor_position(PORT_A, -90);
        }
        // Right
        else if(direction == 1){ BP.set_motor_position(PORT_A, 90); }
}

void turn_hard(const int direction){
        int degree = 450;
        int degree_neg = -450;
        // Left
        if(direction == 0){
                BP.set_motor_position_relative(PORT_B, degree);
                BP.set_motor_position_relative(PORT_C, degree_neg);
        }
        // Right
        else if(direction == 1){
                BP.set_motor_position_relative(PORT_B, degree_neg);
                BP.set_motor_position_relative(PORT_C, degree);
        }
}

void turn_soft(const int direction){
        if(direction == 0){
                BP.set_motor_power(PORT_B, 50);
                BP.set_motor_power(PORT_C, 20);
                sleep(1);
        }
        else if(direction == 1){
                BP.set_motor_power(PORT_B, 20);
                BP.set_motor_power(PORT_C, 50);
                sleep(1);
        }
}

void move(const int direction){
        int speed = 20;
        // Forward
        if(direction == 0){
                BP.set_motor_power(PORT_B, speed);
                BP.set_motor_power(PORT_C, speed);
        }
        // Stop
        else if(direction == 1){
                BP.set_motor_power(PORT_B, 0);
                BP.set_motor_power(PORT_C, 0);
        }
}

void reset(const int direction){
        // Reset for left
        if(direction == 0){ BP.set_motor_position_relative(PORT_A, 90);
        }
        // Reset for right
        else { BP.set_motor_position_relative(PORT_A, -90); }
        sleep(1);
}


void logic(const int distance){
        int safe_dist = 10;
        if(distance < safe_dist && status == "idle"){
                cout << "BrickPi: OBJECT DETECTED" << endl;
                move(1);

                logic_count = 0;
                status = "detected";
        } else if(status == "idle"){cout << distance << "cm" << endl;  move(0); }

        if(logic_count == 0){
                cout << "BrickPi: TURNED" << endl;
                turn_hard(0);
                check(1);
                sleep(1);

                logic_count = 1;
        }
        else if(logic_count == 1){
                cout << "BrickPi: MOVING 1" << endl;
                safe_dist = 30;
                move(0);
                sleep(2);
                if(distance > safe_dist){
                        cout << distance << "cm = SAFE" << endl;
                        move(1);

                        cout << "BrickPi: CLEARED 1" << endl;
                        turn_hard(1);
                        sleep(1);

                        logic_count = 2;
                }
        }
        else if(logic_count == 2){
                cout << "BrickPi: MOVEMENT INBETWEEN" << endl;
                move(0);
                sleep(5);
                logic_count = 3;
        }

        else if(logic_count == 3){
                cout << "BrickPi: MOVING 2" << endl;
                safe_dist = 30;
                move(0);
                sleep(2);
                if(distance > safe_dist){
                        cout << distance << "cm = SAFE" << endl;
                        move(1);

                        cout << "BrickPi: CLEARED 2" << endl;
                        reset(1);
                        move(0);
                        sleep(2);
                        move(1);
                        turn_hard(1);
                        sleep(1);
                        move(1);

                        logic_count = 4;
                }
        }
        else if(logic_count == 4){
                cout << "BrickPi: MOVING 3" << endl;
                move(0);
                sleep(3);
                move(1);

                logic_count = 5;
        }
        else if(logic_count == 5){
                cout << "BrickPi: CHECKING LEFT" << endl;
                safe_dist = 30;
                check(0);
                sleep(1);
                if(distance > safe_dist){
                        cout << distance << "cm" << endl;
                        turn_hard(0);
                        move(1);
                        sleep(1);
                        turn_hard(0);
                        sleep(1);
                        reset(0);
                        move(0);
                        cout << "BrickPi: DONE" << endl;

                        logic_count = -1;
                        status = "idle";
                } else { logic_count = 4; }
        }
}

int main(){
        // Sensor and motor setup
        signal(SIGINT, exit_signal_handler);
        BP.detect();
        BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
        sensor_ultrasonic_t sensor;

        BP.set_motor_limits(PORT_A, 60, 0);
        BP.set_motor_limits(PORT_B, 60, 0);
        BP.set_motor_limits(PORT_C, 60, 0);
        BP.set_motor_position_relative(PORT_A, 0);

        // ZERO
        BP.set_motor_position(PORT_A, 0);
        BP.set_motor_position(PORT_B, 0);
        BP.set_motor_position(PORT_C, 0);

        int distance = 666; // Otherwise robot will think there is a danger on first loop
        // Sending the distance to the "brain"
//      while(true){
                if(BP.get_sensor(PORT_2, sensor) == 0){
                        distance = sensor.cm;
                }
                logic(distance);
                sleep(0.1);
//      }
}


void exit_signal_handler(int signo){
        if(signo == SIGINT){
                BP.reset_all();
                exit(-2);
        }
}
