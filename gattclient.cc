#include "gattclient.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <com_bluegiga_v2_bt.h>
#include <math.h>


const uint8_t GattClient::client_characteristic_configuration_uuid[16] = {0x00, 0x00, 0x29, 0x02, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb};
const uint8_t GattClient::health_thermometer_uuid[16] = {0x00, 0x00, 0x18, 0x09, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb};
const uint8_t GattClient::temperature_measurement_uuid[16] = {0x00, 0x00, 0x2a, 0x1c, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb};

// the UUID I want to talk to and the one I'm currently seeing...
std::string targetUUID = "4c23efb61bc83590064e0100cd67f5cb";

// this handle is used for reading a characteristic
uint16_t myHandle;
int8_t threshold = -120;

// create a map to hold all particles found
particles particleList;

// Print a time stamp
void PrintTime() {
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


void GattClient::ScanCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier) {
	if (resultCode != 0) {
		std::cerr << "Scan start/stop failed. Error code " << resultCode << ", supplier " << resultSupplier << std::endl;
		this->UnregisterReq(this->getGattId());
	}
	else {
		std::cout << "ScanCfm passed.\n";
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
	if (!data.empty()) {

		// Putting in a hard-coded RSSI level for debug purposes....
		if( rssi > threshold ){
			PrintTime();

			// jump through hoops to print out the rssi threshold
			char thresh[ 4 ];
			sprintf( thresh, "%d", threshold );
			std::string rssiMin( thresh );

			std::cout << "\033[035m  " << gattId << "\033[037m" << " found " << address  <<  rssi << " < " << rssiMin << ". Connecting to device..." << std::endl;
			this->CentralReq(gattId, address, 0, 0);
		} /* if rssi */
	} /* if !data.empty */
}

void GattClient::DiscoverServicesInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& startHandle, const uint16_t& endHandle, const std::vector< uint8_t >& uuid){
	//printf("\033[032m%u\033[37m: Discovered service ", btConnId); 
	PrintTime();

	std::cout << "\033[032m" << btConnId << "\033[37m: Discovered service, start: " << startHandle << ", end: " << endHandle << ", ID: ";  
	std::cout << "\033[036m"; 

	for (std::vector< uint8_t >::const_iterator i=uuid.begin(); i != uuid.end(); i++) printf("%.2x", *i);

	// handle this spectial case...must have been a demo
	if (std::equal(health_thermometer_uuid, health_thermometer_uuid+16, uuid.begin())) {
		printf(" The service is Health thermometer service.");
		this->CancelReq(gattId, btConnId);
		this->health_thermometer_service[btConnId].uuid = uuid;
		this->health_thermometer_service[btConnId].start_handle = startHandle;
		this->health_thermometer_service[btConnId].end_handle = endHandle;
	}
	std::cout << "\033[037m" << std::endl; 
	fflush(stdout);

	// Now, let's find the characteristics...
	PrintTime();
	std::cout << "\033[032m" << btConnId << "\033[37m:   Discovering characteristics for service with start: " << startHandle << std::endl;  
	this->DiscoverAllCharacOfAServiceReq( gattId, btConnId, startHandle, endHandle );
}
	
void GattClient::DiscoverServicesCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId) {
	PrintTime();
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ":   Services discovered." << std::endl;
	bool disconnect = false;

	if (resultCode == BT_GATT_RESULT_SUCCESS || resultCode == BT_GATT_RESULT_CANCELLED) {
		std::map<uint32_t, DiscoveredService>::const_iterator service = this->health_thermometer_service.find(btConnId);	
		if (service != this->health_thermometer_service.end()) { // health thermometer service was found
			disconnect = false;
			std::cout << "Discovering all characteristics of the health thermometer service...\n";
			this->DiscoverAllCharacOfAServiceReq(gattId, btConnId, service->second.start_handle, service->second.end_handle);
		}
	}

	// Now that we know the services, let's disconnect...
	if (disconnect) {
		std::cout << "Disconnecting...\n";
		this->DisconnectReq(gattId, btConnId);
	}
}
	
void GattClient::CentralCfm(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& resultCode, const uint16_t& resultSupplier) {
} 

void GattClient::ConnectInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& connInfo, const std::string& address, const uint16_t& mtu) {
	PrintTime();

	// create a new entry in the list of all seen Particles
	// start with an initialized particle struct
	particle tempPart = { 0, 0, 0, 0 };
	particleList.insert( particles::value_type( btConnId, tempPart ) );

	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	if (resultCode == 0) {
		std::cout << ": Connected to " << address  << ". Discovering all primary services...\n";
		this->DiscoverAllPrimaryServicesReq(gattId, btConnId);
	}
	else
		std::cout << ": Connection to " << address << "failed: " << resultCode << ", " << resultSupplier << std::endl;
}

void GattClient::DisconnectInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& reasonCode, const uint16_t& reasonSupplier, const std::string& address, const uint32_t& connInfo) {
	PrintTime();
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ": Disconnected\n";
	this->health_thermometer_service.erase(btConnId);
}

void GattClient::DiscoverCharacInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& declarationHandle, const uint8_t& property, const std::vector< uint8_t >& uuid, const uint16_t& valueHandle) {
	PrintTime();
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ": Characteristic with dec " << declarationHandle << ", val " << valueHandle << ", and UUID: ";

	// NOTE: the property field has the flags...see pg. 166 in SDK doc for decode
	
	// Load up and array with the UUID.  Next, we'll convert it to a string and
	// compare it with our targetUUID to see if this is the one we want to talk to.
	char tempId[2] = { };
	std::string currentUUID = "";
	int j = 0;
	// spit out the UUID byte by byte...
	for (std::vector< uint8_t >::const_iterator i=uuid.begin(); i != uuid.end(); i++) {
		//printf("%.2x", *i);
		sprintf( tempId, "%02x", *i );
		std::string temp(tempId);
		currentUUID += temp;
	} /* for */

	// look for Particle-specific characteristics, load up the particle
	// struct, and update the particleList map with those values...
	std::string passcodeChar = "0000c69b"; 
	std::string opcodeChar = "0000c69c"; 
	std::string operandChar = "0000c69d"; 
	std::string resultChar = "0000c69e"; 

	// grab the structure with the handles...
	particle tempstruct = particleList.find( btConnId )->second;
	if( currentUUID.find( passcodeChar ) != -1 ) {
		// Load the passcode handle into the appropriate struct
		tempstruct.passcode = valueHandle;
		std::cout << "***** PASSCODE ***** ";

		std::vector< uint8_t > message;
		// loading ASCII decimal values for '0x0000000000000000'
		for( int i = 0; i < 16; i++ ){
			message.push_back( 0x00 );
		} // for
		this->WriteReq( gattId, btConnId, valueHandle, 0,  message ) ;

	} else if( currentUUID.find( opcodeChar ) != -1 ) {
		tempstruct.opcode = valueHandle;
		std::cout << "***** OPCODE ***** ";
	} else if( currentUUID.find( operandChar ) != -1 ) {
		tempstruct.operand = valueHandle;
		std::cout << "***** OPERAND ***** ";
	} else if( currentUUID.find( resultChar ) != -1 ) {
		tempstruct.result = valueHandle;
		std::cout << "***** RESULT ***** ";
	}
	// save the modified handle structure...
	particleList.insert( particles::value_type( btConnId, tempstruct ));
	//this->WriteReq( gattId, btConnId, valueHandle, 0,  message ) ;

	/*
	 *  This is for the gumstick
	 *
	// check for a match, if same, print the UUID blue
	// and attempt to write and read from it...
	if( currentUUID == targetUUID )
	{
		std::cout << "\033[36m  " << currentUUID << "\033[37m ";

		std::vector< uint8_t > message;
		// loading ASCII decimal values for 'HAR'
		message.push_back( 72 );
		message.push_back( 65 );
		message.push_back( 82 );
		//message.push_back( 0 );

		// store the valueHandle for reading later...
		myHandle = valueHandle;

		this->WriteReq( gattId, btConnId, valueHandle, 0,  message ) ;
	} else {
		std::cout << "\033[32m  " << currentUUID << "\033[37m  ";
	} // if currentUUID 
	*/
	std::cout << "(0x" << std::hex << (uint16_t) property << std::dec << ")";

	// Again with the special cases...
	if (std::equal(temperature_measurement_uuid, temperature_measurement_uuid+16, uuid.begin())) {
		printf(" Temperature Measurement characteristic found.\n");
		this->health_thermometer_service[btConnId].temperature_measurement_characteristic.value_handle = valueHandle;
		this->health_thermometer_service[btConnId].temperature_measurement_characteristic.property = property;
		this->CancelReq(gattId, btConnId);
	}
	else
		printf("\n");
}
	
void GattClient::DiscoverCharacCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId) {
	PrintTime();
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ": Service characteristics discovered. ";

	if ( resultCode == BT_GATT_RESULT_SUCCESS || resultCode == BT_GATT_RESULT_CANCELLED) {
		std::cout << "Discovering characteristic descriptors...\n";
		this->DiscoverAllCharacDescriptorsReq(
			gattId,
			btConnId,
			this->health_thermometer_service[btConnId].temperature_measurement_characteristic.value_handle+1,
			this->health_thermometer_service[btConnId].end_handle);
	}
	else {
		std::cout << "Disconnecting...\n";
		this->DisconnectReq(gattId, btConnId);
	}
}

void GattClient::DiscoverCharacDescriptorsInd(const uint32_t& gattId, const uint32_t& btConnId, const std::vector< uint8_t >& uuid, const uint16_t& descriptorHandle) {
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ":   Discovered characteristic descriptor UUID ";
	std::cout << "\033[36m"; 
	std::cout.flush();
	for (std::vector< uint8_t >::const_iterator i=uuid.begin(); i != uuid.end(); i++) printf("%.2x", *i);
               fflush(stdout);
	std::cout << "\033[37m" << std::endl;
	
	if (std::equal(client_characteristic_configuration_uuid, client_characteristic_configuration_uuid+16, uuid.begin())) {
		std::cout << btConnId << ": Client characteristic configuration handle found for temperature measurement characteristic\n";
		this->health_thermometer_service[btConnId].temperature_measurement_characteristic.client_characteristic_configuration_handle = descriptorHandle;
		this->CancelReq(gattId, btConnId);
	}
}
	
void GattClient::DiscoverCharacDescriptorsCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId) {
	PrintTime();
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ":   Characteristic descriptors discovered." << std::endl;

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
		std::cout << btConnId << ": Writing client characteristic configuration failed: " << resultCode << ", " << resultSupplier << std::endl;
		this->DisconnectReq(gattId, btConnId);
	}
	else {
		std::cout << btConnId << ":   Write successful: " << std::hex << resultCode << std::dec << std::endl;
		// HAR NOTE: added this here to read after a successful write...myHandle is a global
		particle tempstruct;
		tempstruct = particleList.find( btConnId )->second;
		myHandle = tempstruct.result;
		this->ReadReq( gattId, btConnId, myHandle, 0 );
	}
}

void  GattClient::ReadCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId, const std::vector< uint8_t >& value ) {
	PrintTime();
	if (resultCode != BT_GATT_RESULT_SUCCESS) {
		std::cout << btConnId << ":   Reading client characteristic failed: " << resultCode << ", " << resultSupplier << std::endl;
		this->DisconnectReq(gattId, btConnId);
	}
	else {
		std::cout << btConnId << ": " << "   Read successful; Data: ";
		for (std::vector< uint8_t >::const_iterator i=value.begin(); i != value.end(); i++) printf("%.2x", *i);
		std::cout << std::endl;
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

