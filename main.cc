
#include "gattclient.h"
#include <signal.h>
#include <iostream>

DBus::BusDispatcher dispatcher;

bool scanning = true;

void sighandler(int sig) {
	dispatcher.leave(false);
}


// TODO: signal blocking when appropriate

int main() {

	signal(SIGINT, sighandler);
	DBus::default_dispatcher = &dispatcher;
	DBus::Connection conn = DBus::Connection::SystemBus();
	GattClient gc(conn, "/bt/gatt", "com.bluegiga.v2.bt0");
	
	std::cout << "\033[34m" << "-------------< Hars Hacked leet App >-------------" << "\033[37m" << std::endl;

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

	std::cout << "Stopping scan... ";
	if (!gc.startStopScan(false)) // stop scanning
		return 4;

	std::cout << "Unregistering and leaving app...\n";
	gc.UnregisterReq(gc.getGattId());
	dispatcher.enter();	 
	return (gc.isRegistered() ? 5 : 0);
}

