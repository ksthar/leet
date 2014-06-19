
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

	uint16_t	scanWindow = 10;
	uint16_t	scanInterval = 20;

	// catch ctrl-c to exit the dispatcher
	signal(SIGINT, sighandler);

	
	std::cout << "\033[36m" << "--------------------------<   Connection Test   >--------------------------" << "\033[37m" << std::endl;
	
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

	// setup scan window and interval here; interval & window in 0.625us slots
	//gc.ParamScanReq( gc.getGattId(), <scanInterval>, <scanWindow> );
	//gc.ParamScanReq( gc.getGattId(), 240, 1360 ); // interval = 150ms out of 1 sec
	gc.PrintTime();
	std::cout << "Scan window   = " << ((scanWindow * 625) / 1000) << "ms " << std::endl;
	gc.PrintTime();
	std::cout << "Scan interval = " << ((scanInterval * 625)/1000) << "ms " << std::endl;
	gc.ParamScanReq( gc.getGattId(), scanInterval, scanWindow ); 

	gc.PrintTime();
	std::cout << "\033[34m" << "Starting scan... " << "\033[37m" << std::endl;
	if (!gc.startStopScan(true)) // start scanning
		return 2;

	gc.PrintTime();
	std::cout << "Receiving results..." << std::endl;
	gc.InitTargets();
	dispatcher.enter(); // leaving this when SIGINT arrives
	if (!gc.isRegistered()) 
		return 3;


	// ==================================================================================================================
	// CLEAN UP BLE STUFF
	// ==================================================================================================================
	gc.PrintTime();
	std::cout << "\033[34m" << "Stopping scan... " << "\033[37m" << std::endl;
	if (!gc.startStopScan(false)) // stop scanning
		return 4;

	gc.PrintTime();
	std::cout << "Unregistering and leaving app..." << std::endl;
	std::cout << "\033[36m" << "--------------------------<         BYE          >--------------------------" << "\033[37m" << std::endl;
	gc.UnregisterReq(gc.getGattId());
	dispatcher.enter();	 
	return (gc.isRegistered() ? 5 : 0);
	return 0;

}

