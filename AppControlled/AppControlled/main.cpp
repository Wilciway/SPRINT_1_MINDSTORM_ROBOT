#include "BrickPi3.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <iomanip>
#include "BluetoothSocket.h"
#include <unistd.h>

using namespace std;

BrickPi3 				BP;

void exit_signal_handler(int signo);

void forward(){
	BP.set_motor_power(PORT_C,25);
	BP.set_motor_power(PORT_B,25);
}

void reverse(){
    BP.set_motor_power(PORT_C,-25);
    BP.set_motor_power(PORT_B,-25);
}

void right(){
        BP.set_motor_power(PORT_C,40);
	    BP.set_motor_power(PORT_B,-128);
}

void left(){
        BP.set_motor_power(PORT_B,40);
        BP.set_motor_power(PORT_C,-128);
}

void stop(){
        BP.set_motor_power(PORT_B,0);
        BP.set_motor_power(PORT_C,0);
}


int main(){
	BP.detect();
	
	BluetoothServerSocket serversock(2, 1);  //2 is het channel-number
	cout << "listening" << endl;
	while(true) {
		BluetoothSocket* clientsock = serversock.accept();
		cout << "accepted from " << clientsock->getForeignAddress().getAddress() << endl;
		MessageBox& mb = clientsock->getMessageBox();
			
		string input;
		string clean_input;
		while(mb.isRunning()) {
			input = mb.readMessage();  //blokkeert niet
			clean_input = input.substr(0, input.size()-1);
			
			if(clean_input.compare("LEFT") == 0){
				cout<< endl << "LEFT" << endl;
				left();
				}
			if(clean_input.compare("RIGHT") == 0){
				cout<< endl << "RIGHT" << endl;
				right();
				}
			if(clean_input.compare("UP") == 0){
				cout<< endl << "FORWARD" << endl;
				forward();
				}
			if(clean_input.compare("DOWN") == 0){
				cout<< endl << "REVERSE" << endl;
				reverse();
				}
			if(clean_input.compare("FIRE") == 0){
				cout<< endl << "BREAK" << endl;
				stop();
				}
			if(clean_input.compare("A") == 0){
				cout<< endl << "A" << endl;
				}
			if(clean_input.compare("B") == 0){
				cout<< endl << "B" << endl;
				}
			if(clean_input.compare("C") == 0){
				cout<< endl << "C" << endl;
				}

			cout.flush();
			usleep(500000); // wacht 500 ms
		}	
		
		clientsock->close();
			
	}
}

void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();
    exit(-2);
  }
}
