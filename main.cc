
#include "gattclient.h"
#include <signal.h>
#include <iostream>
#include <string.h>


#define UNIX_PATH_MAX 108

DBus::BusDispatcher dispatcher;

bool scanning = true;

// ==================================================================================================================
// HELPERS
// ==================================================================================================================
void sighandler(int sig) {
	dispatcher.leave(false);
}

// TODO: signal blocking when appropriate

// ==================================================================================================================
// MAIN
// ==================================================================================================================
int main() {

	// catch ctrl-c to exit the dispatcher
	signal(SIGINT, sighandler);

	
	std::cout << "\033[36m" << "--------------------------<  Initiate BLE Scan  >--------------------------" << "\033[37m" << std::endl;
	
	// ==================================================================================================================
	// SCAN BLE, ENUMERATE GATT, READ, & WRITE
	// ==================================================================================================================
	DBus::default_dispatcher = &dispatcher;
	DBus::Connection conn = DBus::Connection::SystemBus();
	GattClient gc(conn, "/bt/gatt", "com.bluegiga.v2.bt0");
	
	gc.RegisterReq(0);
	dispatcher.enter();
	if (!gc.isRegistered())
		return 1;

	std::cout << "Starting scan... ";
	if (!gc.startStopScan(true)) // start scanning
		return 2;

	std::cout << "Receiving results...\n";
	dispatcher.enter(); // leaving this when SIGINT arrives
	if (!gc.isRegistered()) 
		return 3;


	// ==================================================================================================================
	// CLEAN UP BLE STUFF
	// ==================================================================================================================
	std::cout << "Stopping scan... ";
	if (!gc.startStopScan(false)) // stop scanning
		return 4;

	std::cout << "Unregistering and leaving app...\n";
	std::cout << "\033[36m" << "--------------------------<         BYE          >--------------------------" << "\033[37m" << std::endl;
	gc.UnregisterReq(gc.getGattId());
	dispatcher.enter();	 
	return (gc.isRegistered() ? 5 : 0);
	return 0;

}

