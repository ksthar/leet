#include "gattclient.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

// for domain sockets
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>

#include <com_bluegiga_v2_bt.h>
#include <math.h>
#include <time.h>


const uint8_t GattClient::client_characteristic_configuration_uuid[16] = {0x00, 0x00, 0x29, 0x02, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb};
const uint8_t GattClient::health_thermometer_uuid[16] = {0x00, 0x00, 0x18, 0x09, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb};
const uint8_t GattClient::temperature_measurement_uuid[16] = {0x00, 0x00, 0x2a, 0x1c, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb};

// the UUID I want to talk to...
std::string targetUUID = "4c23efb61bc83590064e0100cd67f5cb";

// the address of the device I want to talk to 
std::string targetAddr[8] = { 
	"00:07:80:68:05:d3|public", 
	"00:07:80:68:05:ef|public", 
	"00:07:80:68:05:f1|public", 
	"00:07:80:67:75:41|public", 
	"00:07:80:68:05:ea|public",
	"00:07:80:68:05:e8|public", 
	"00:07:80:68:05:b5|public",
	"00:07:80:68:05:eb|public"
}; 
targets_t targetConnected;
uint16_t numTargets = 8;
 
// this handle is used for reading a characteristic
uint16_t myHandle;
int8_t threshold = -120;
uint16_t connections = 0;
uint16_t maxConnections = 8;

// setup my characteristic maps
passcodes_t passcodes;
opcodes_t opcodes;
operands_t operands;
results_t results;
gumstick_t gumsticks;

// Global that says whether we want to connect or not
bool connToBLE = true;
void SetConnection( bool setConn ){ connToBLE = setConn; }
bool GetConnection(){ return connToBLE; }


void PrintTime() {
	// Print a time stamp
	time_t rawtime;
	struct tm *timeinfo;
	char reportTime [13]; 
	int n;

	time (&rawtime);
	timeinfo = localtime( &rawtime );
	n = sprintf( reportTime, "[ %02d:%02d:%02d ]  ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec );
	printf( "%s", reportTime );
	//std::cout << "[ " << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec << " ]  "; // << std::endl;
}

void GattClient::InitTargets() {
	PrintTime();
	std::cout << "Initializing target map." << std::endl;
	// initialize our target map
	for( int i = 0; i < numTargets; i++ ){
		targetConnected.insert( targets_t::value_type( targetAddr[i], 0 ));
		//std::cout << targetAddr[i] << " -> " << 0 << std::endl;
	} // for i
} // InitTargets

void GattClient::ScanCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier) {
	if (resultCode != 0) {
		std::cerr << "Scan start/stop failed. Error code " << resultCode << ", supplier " << resultSupplier << std::endl;
		this->UnregisterReq(this->getGattId());
	}
	else {
		std::cout << "ScanCfm passed." << std::endl;
		dispatcher.leave(false);
	}
}

void GattClient::RegisterCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint16_t& context) {
	if (resultCode != 0) {
		std::cerr << "Profile registration failed. Error code " << resultCode << ", supplier " << resultSupplier << std::endl;
	}
	else { 
		this->gid=gattId;
		this->registered=true;
	}
	dispatcher.leave(false);
}

void GattClient::UnregisterCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier) {
	if (resultCode != 0)
		std::cerr << "Profile unregistration failed. Error code " << resultCode << ", supplier " << resultSupplier << std::endl;
	else
		this->registered = false;
	dispatcher.leave(false);
}

void GattClient::ReportInd(const uint32_t& gattId, const uint8_t& eventType, const std::string& address, const std::string& permanentAddress, const std::vector< uint8_t >& data, const int16_t& rssi) {

	int i = 0;

	if (!data.empty()) {

		// Gate on RSSI threshold
		if( rssi > threshold ){

			// jump through hoops to print out the rssi threshold
			char thresh[ 4 ];
			sprintf( thresh, "%d", threshold );
			std::string rssiMin( thresh );

			//PrintTime();
			//std::cout << "\033[035m  " << gattId << "\033[037m" << " Found " << address << " " <<  rssi << " < " << rssiMin << std::endl;
			if( GetConnection() ) {
				// only connect if we have less than five existing connections
				if( connections < maxConnections ){
					// Verify its an address we want to connect to
					for( i = 0; i < numTargets; i++ ){
						if( targetAddr[i] == address ){ 
							if( targetConnected[address] == 0 ) {
								targetConnected[address] = 1;
								//std::cout << targetAddr[i] << " = " << address << std::endl;
								SetConnection( false );
								this->CentralReq(gattId, address, 0, 0);
							} // if targetConnected
						} // if targetAddr
					} // for i
				}// if connections < 5
			} // if GetConnection()

		} /* if rssi */
	} /* if !data.empty */
}

void GattClient::DiscoverServicesInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& startHandle, const uint16_t& endHandle, const std::vector< uint8_t >& uuid){

	/*
	PrintTime();
	std::cout << "\033[032m" << btConnId << "\033[37m: Discovered service, start: " << startHandle << ", end: " << endHandle << ", ID: ";  
	std::cout << "\033[036m"; 
	for (std::vector< uint8_t >::const_iterator i=uuid.begin(); i != uuid.end(); i++) printf("%.2x", *i);
	std::cout << "\033[037m" << std::endl; 
	*/

	// Now, let's find the characteristics...
	//PrintTime();
	//std::cout << "\033[032m" << btConnId << "\033[37m:   Discovering characteristics for service with start: " << startHandle << std::endl;  
	this->DiscoverAllCharacOfAServiceReq( gattId, btConnId, startHandle, endHandle );
}
	
void GattClient::DiscoverServicesCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId) {
	/*
	PrintTime();
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ":   Services discovered." << std::endl;
	*/
	bool disconnect = false;

	/*
	if (resultCode == BT_GATT_RESULT_SUCCESS || resultCode == BT_GATT_RESULT_CANCELLED) {
		std::map<uint32_t, DiscoveredService>::const_iterator service = this->health_thermometer_service.find(btConnId);	
		if (service != this->health_thermometer_service.end()) { // health thermometer service was found
			disconnect = false;
			std::cout << "Discovering all characteristics of the health thermometer service...\n";
			this->DiscoverAllCharacOfAServiceReq(gattId, btConnId, service->second.start_handle, service->second.end_handle);
		}
	}
	*/

	// Now that we know the services, let's disconnect...
	if (disconnect) {
		std::cout << "Disconnecting...\n";
		//this->DisconnectReq(gattId, btConnId);
	}
}
	
void GattClient::CentralCfm(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& resultCode, const uint16_t& resultSupplier) {
} 

void GattClient::ConnectInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& connInfo, const std::string& address, const uint16_t& mtu) {

	// allow more connections now that this one is made
	SetConnection( true );

	PrintTime();
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	if (resultCode == 0) {
		connections++;
		char myConnections[ 4 ];
		sprintf( myConnections, "%d", connections );

		std::cout << " [" << myConnections << "] Connected to " << address << std::endl; 

		/*
		char numConn[ 4 ];
		sprintf( numConn, "%d", connections );
		std::string numConnections( numConn );
		*/
		this->DiscoverAllPrimaryServicesReq(gattId, btConnId);
	} else {
		// release this address for another connection
		targetConnected[address] = 0;
		std::cout << " " << address << " is reconnectable." << std::endl;
		//std::cout << " Connection to " << address << " failed: (" << resultCode << ", " << resultSupplier << ")" << std::endl;
	} // if resultCode == 0
}

void GattClient::DisconnectInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& reasonCode, const uint16_t& reasonSupplier, const std::string& address, const uint32_t& connInfo) {
	PrintTime();
	--connections;
	char myConnections[ 4 ];
	sprintf( myConnections, "%d", connections );
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << " [" << myConnections << "] Disconnected from " << address << " (" << btConnId << ")" << std::endl;
	//this->ScanReqStop( gattId );

	// Now, allow leet to make another connection
	SetConnection( true );
}

void GattClient::DiscoverCharacInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& declarationHandle, const uint8_t& property, const std::vector< uint8_t >& uuid, const uint16_t& valueHandle) {

	
	/*
	std::cout << ": Characteristic with dec " << declarationHandle << ", val " << valueHandle << ", and UUID: ";
	*/

	// NOTE: the property field has the flags...see pg. 166 in SDK doc for decode
	
	// Load up and array with the UUID.  Next, we'll convert it to a string and
	// compare it with our targetUUID to see if this is the one we want to talk to.
	char tempId[2] = { };
	std::string currentUUID = "";
	int j = 0;
	// spit out the UUID byte by byte...
	for (std::vector< uint8_t >::const_iterator i=uuid.begin(); i != uuid.end(); i++) {
		sprintf( tempId, "%02x", *i );
		std::string temp(tempId);
		currentUUID += temp;
	} // for 

	// these are the first 8 bytes that identify each characteristic
	// we're looking for
	std::string passcodeChar = "0000c69b"; 
	std::string opcodeChar = "0000c69c"; 
	std::string operandChar = "0000c69d"; 
	std::string resultChar = "0000c69e"; 

	// or, if we're looking at a gumstick
	std::string gumstickChar = "4c23efb61bc8"; 

	if( currentUUID.find( passcodeChar ) != -1 ) {
		passcodes.insert( passcodes_t::value_type( btConnId, valueHandle ));
		/*
		std::cout << "PASSCODE ";
		std::cout << "p: " << passcodes.find( btConnId )->second  << " oc: " << opcodes.find( btConnId )->second; 
		std::cout << " od: " << operands.find( btConnId )->second << " r: " << results.find( btConnId )->second << " "; 
		*/

		/*
		std::vector< uint8_t > message;
		// loading ASCII decimal values for '0x0000000000000000'
		for( int i = 0; i < 16; i++ ){
			message.push_back( 0x00 );
		} // for
		this->WriteReq( gattId, btConnId, passcodes.find( btConnId )->second, 0,  message ) ;
		*/

	} else if( currentUUID.find( opcodeChar ) != -1 ) {
		opcodes.insert( opcodes_t::value_type( btConnId, valueHandle ));
		/*
		std::cout << "OPCODE ";
		std::cout << "p: " << passcodes.find( btConnId )->second  << " oc: " << opcodes.find( btConnId )->second; 
		std::cout << " od: " << operands.find( btConnId )->second << " r: " << results.find( btConnId )->second << " "; 
		*/
	} else if( currentUUID.find( operandChar ) != -1 ) {
		operands.insert( operands_t::value_type( btConnId, valueHandle ));
		/*
		std::cout << "OPERAND ";
		std::cout << "p: " << passcodes.find( btConnId )->second  << " oc: " << opcodes.find( btConnId )->second; 
		std::cout << " od: " << operands.find( btConnId )->second << " r: " << results.find( btConnId )->second << " "; 
		*/
	} else if( currentUUID.find( resultChar ) != -1 ) {
		results.insert( results_t::value_type( btConnId, valueHandle ));
		/*
		std::cout << "RESULT ";
		std::cout << "p: " << passcodes.find( btConnId )->second  << " oc: " << opcodes.find( btConnId )->second; 
		std::cout << " od: " << operands.find( btConnId )->second << " r: " << results.find( btConnId )->second << " "; 
		*/
	} else if( currentUUID.find( gumstickChar ) != -1 ) {
		gumsticks.insert( gumstick_t::value_type( btConnId, valueHandle ));
		//std::cout << "GUMSTICK ";
		/*
		std::cout << "p: " << passcodes.find( btConnId )->second  << " oc: " << opcodes.find( btConnId )->second; 
		std::cout << " od: " << operands.find( btConnId )->second << " r: " << results.find( btConnId )->second << " "; 
		*/
		
		std::vector< uint8_t > message;
		// loading ASCII decimal values for '0x0000000000000000'
		for( int i = 0; i < 16; i++ ){
			message.push_back( 0x00 );
		} // for
		PrintTime();
		std::cout << "\033[32m" << btConnId << "\033[37m"; 
		std::cout << " Characteristic Flags (0x" << std::hex << (uint16_t) property << std::dec << ")" << std::endl;
		// for the disconnect experiment, let's not try to connect
		this->WriteReq( gattId, btConnId, gumsticks.find( btConnId )->second, 0,  message ) ;
	}

}
	
void GattClient::DiscoverCharacCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId) {
	/*
	PrintTime();
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ": Service characteristics discovered. ";
	*/

	if ( resultCode == BT_GATT_RESULT_SUCCESS || resultCode == BT_GATT_RESULT_CANCELLED) {
		//std::cout << "Discovering characteristic descriptors...\n";
		this->DiscoverAllCharacDescriptorsReq(
			gattId,
			btConnId,
			this->health_thermometer_service[btConnId].temperature_measurement_characteristic.value_handle+1,
			this->health_thermometer_service[btConnId].end_handle);
	}
	else {
		std::cout << "Disconnecting...\n";
		//this->DisconnectReq(gattId, btConnId);
	}
}

void GattClient::DiscoverCharacDescriptorsInd(const uint32_t& gattId, const uint32_t& btConnId, const std::vector< uint8_t >& uuid, const uint16_t& descriptorHandle) {
	/*
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ":   Discovered characteristic descriptor UUID ";
	std::cout << "\033[36m"; 
	std::cout.flush();
	for (std::vector< uint8_t >::const_iterator i=uuid.begin(); i != uuid.end(); i++) printf("%.2x", *i);
               fflush(stdout);
	std::cout << "\033[37m" << std::endl;
	*/
	if (std::equal(client_characteristic_configuration_uuid, client_characteristic_configuration_uuid+16, uuid.begin())) {
		std::cout << btConnId << ": Client characteristic configuration handle found for temperature measurement characteristic\n";
		this->health_thermometer_service[btConnId].temperature_measurement_characteristic.client_characteristic_configuration_handle = descriptorHandle;
		this->CancelReq(gattId, btConnId);
	}
}
	
void GattClient::DiscoverCharacDescriptorsCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId) {
	/*
	PrintTime();
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ":   Characteristic descriptors discovered." << std::endl;
	*/

	// may need to do something here...
	
	/*
	if (this->health_thermometer_service[btConnId].temperature_measurement_characteristic.client_characteristic_configuration_handle == 0) { // not found.
		std::cout << " Client characteristic configuration handle not found. Disconnecting...\n";
		this->DisconnectReq(gattId, btConnId);
	}
	else if (!(this->health_thermometer_service[btConnId].temperature_measurement_characteristic.property & BT_GATT_CHARAC_PROPERTIES_INDICATE)) {
		 std::cout << " Not allowed to write indicate bit. Disconnecting...\n";
		this->DisconnectReq(gattId, btConnId);
	}
	else {
		std::cout << " Enabling indications by writing client characteristic configuration...\n";
		this->WriteClientConfigurationReq(	gattId,
							btConnId,
							this->health_thermometer_service[btConnId].temperature_measurement_characteristic.client_characteristic_configuration_handle,
							this->health_thermometer_service[btConnId].temperature_measurement_characteristic.value_handle, 
							BT_GATT_CLIENT_CHARAC_CONFIG_INDICATION); 
	}
	*/
}


void  GattClient::WriteCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId) {
	PrintTime();
	if (resultCode != BT_GATT_RESULT_SUCCESS) {
		std::cout << btConnId << "\033[031m" << " -> Write failed! ";
		std::cout << "(" << resultCode << ", " << resultSupplier << ")" << "\033[037m" << std::endl;
		this->DisconnectReq(gattId, btConnId);
	}
	else {
		std::cout << btConnId << " -> Write successful " << std::endl;
		// HAR NOTE: added this here to read after a successful write...myHandle is a global
		// don't try to read if we don't have a handle for the result characteristic yet
		if( results.find( btConnId )->second == 0 ) {
			this->ReadReq( gattId, btConnId, results.find( btConnId )->second, 0 );
		} else {
		}
	}
}

void  GattClient::ReadCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId, const std::vector< uint8_t >& value ) {
	PrintTime();
	if (resultCode != BT_GATT_RESULT_SUCCESS) {
		std::cout << btConnId << "\033[031m" << " <- Read failed! ";
		std::cout << "(" << resultCode << ", " << resultSupplier << ")" << "\033[037m" << std::endl;
		this->DisconnectReq(gattId, btConnId);
	}
	else {
		std::cout << btConnId << " <- Read successful; Data: ";
		for (std::vector< uint8_t >::const_iterator i=value.begin(); i != value.end(); i++) {
			printf("%.2x", *i);
		}
		std::cout << std::endl;

		// We successfully read, now let's disconnect
		this->DisconnectReq(gattId, btConnId);
	}
}

void GattClient::NotificationInd(const uint32_t& gattId, const uint32_t& btConnId, const std::string& address, const uint16_t& valueHandle, const std::vector< uint8_t >& value, const uint32_t& connInfo) {
	std::cout << btConnId << ": " << valueHandle << ": ";
	std::cout.flush();
	for (std::vector< uint8_t >::const_iterator i=value.begin(); i != value.end(); i++) printf("%.2x", *i);
	int8_t exponent = value[4];
	uint32_t mantissa = 0;
	mantissa |= value[3] << 16;
	mantissa |= value[2] << 8;
	mantissa |= value[1];
	printf(". Parsed value: %ue%d %c\n", mantissa, exponent, value[0] & 0x01 ? 'F' : 'C');
}

