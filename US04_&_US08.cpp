#include "BrickPi3.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <iomanip>

using namespace std;
BrickPi3                        BP;
sensor_light_t          leftLightSensor;
sensor_color_t          rightColorSensor;
sensor_ultrasonic_t     middleUltrasonicSensor;
int standardSpeed = 75; //standaard snelheid van de robot
int turnSpeed =10;	//snelheid van het wiel aan de kant die hij op gaat draaien
int stopSpeed = 0; //snelheid wanneer is moet stoppen



void exit_signal_handler(int signo){
	// stopt alle motors en sensors wanneer het programma wordt afgesloten
  if(signo == SIGINT){
	BP.reset_all();
	exit(-2);
  }
}
	void init(){
		//zet alle sensors aan.
		signal(SIGINT, exit_signal_handler);
		BP.detect();
		BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
		BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
		BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
 }

void rechtdoor(){
        cout<<"rechtdoor"<<endl;
        BP.set_motor_power(PORT_C,standardSpeed);
        BP.set_motor_power(PORT_B,standardSpeed);
}



void links(){
        cout<<"links"<<endl;
        BP.set_motor_power(PORT_B,100);
        BP.set_motor_power(PORT_C,stopSpeed);
}

void rechts(){
        cout<<"rechts"<<endl;
        BP.set_motor_power(PORT_C,100);
        BP.set_motor_power(PORT_B,turnSpeed);
}

int main(){
		//zet alle sensors aan.
		init();
		string last_seen="links";
		
			while(true){
				//haal informatie op van alle sensors.
				BP.get_sensor(PORT_1, rightColorSensor);
				BP.get_sensor(PORT_2, middleUltrasonicSensor);
				BP.get_sensor(PORT_3, leftLightSensor);
				
				if(middleUltrasonicSensor.cm > 10){ //Als er geen obstakel is binnen de komende 10cm doe het volgende.
					if(rightColorSensor.reflected_red > 500){ //Als de rechter kleur sensor wit ziet doe het volgende.
						if(leftLightSensor.reflected>2000){ //Als de linker licht sensor zwart ziet ga rechtdoor.
							last_seen = "links"; //Geeft aan dat de robot voor het laatst aan de linkerkant van de lijn is geweest.
							last_seen = "links";
                                        rechtdoor();
							}
					else{	//Als de linker licht sensor GEEN zwart ziet oftewel wit doe het volgende.
							if(last_seen == "links"){ //Als de robot voor het laatst aan de linkerkant van de lijn is geweest ga linksaf.
								links();
							}
							else{	//Als de robot voor het laatst NIET aan de linkerkant van de lijn is geweest oftewel aan de rechterkant ga rechtsaf.
								rechts();
							}
						}
					}
					else{	//Als de rechter kleur sensor GEEN wit ziet oftewel zwart doe het volgende.
						if(leftLightSensor.reflected<2000){ //Als de linker licht sensor wit ziet ziet ga rechtdoor.
							last_seen = "rechts"; //Geeft aan dat de robot voor het laatst aan de rechterkant van de lijn is geweest.
									
							}
							
					}
			}
			else{	//Als er een obstakel is stop de robot
					cout<<"obstakel"<<endl;
					BP.set_motor_power(PORT_B,stopSpeed);
					BP.set_motor_power(PORT_C,stopSpeed);
			}
		}
}



