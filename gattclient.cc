#include "gattclient.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <com_bluegiga_v2_bt.h>
#include <math.h>


const uint8_t GattClient::client_characteristic_configuration_uuid[16] = {0x00, 0x00, 0x29, 0x02, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb};
const uint8_t GattClient::health_thermometer_uuid[16] = {0x00, 0x00, 0x18, 0x09, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb};
const uint8_t GattClient::temperature_measurement_uuid[16] = {0x00, 0x00, 0x2a, 0x1c, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb};

// the UUID I want to talk to and the one I'm currently seeing...
std::string targetUUID = "4c23efb61bc83590064e0100cd67f5cb";

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
		if( rssi > -40 ){
			std::cout << "\033[035m  " << gattId << "\033[037m" << " found " << address  << ": Got report. RSSI: " << rssi << ". Connecting...\n";
			this->CentralReq(gattId, address, 0, 0);
		} /* if rssi */
	} /* if !data.empty */
}

void GattClient::DiscoverServicesInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& startHandle, const uint16_t& endHandle, const std::vector< uint8_t >& uuid){
	//printf("\033[032m%u\033[37m: Discovered service ", btConnId); 
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
	std::cout << "\033[032m" << btConnId << "\033[37m:   Discovering characteristics for service with start: " << startHandle << std::endl;  
	this->DiscoverAllCharacOfAServiceReq( gattId, btConnId, startHandle, endHandle );
}
	
void GattClient::DiscoverServicesCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId) {
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ":   Services discovered. ";
	bool disconnect = true;

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
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	if (resultCode == 0) {
		std::cout << ": Connected to " << address  << ". Discovering all primary services...\n";
		this->DiscoverAllPrimaryServicesReq(gattId, btConnId);
	}
	else
		std::cout << ": Connection to " << address << "failed: " << resultCode << ", " << resultSupplier << std::endl;
}

void GattClient::DisconnectInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& reasonCode, const uint16_t& reasonSupplier, const std::string& address, const uint32_t& connInfo) {
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ": Disconnected\n";
	this->health_thermometer_service.erase(btConnId);
}

void GattClient::DiscoverCharacInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& declarationHandle, const uint8_t& property, const std::vector< uint8_t >& uuid, const uint16_t& valueHandle) {
	std::cout << "\033[32m" << btConnId << "\033[37m"; 
	std::cout << ": Discovered characteristic with dec " << declarationHandle << ", val " << valueHandle << ", and UUID: ";

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

	// check for a match, if same, print the UUID blue
	// and attempt to write and read from it...
	if( currentUUID == targetUUID )
	{
		std::cout << "\033[34m  " << currentUUID << "\033[37m  ";
		/*
		this->WriteReq( gattId, btConnId, <what handle?>, <what offset?>, value);
		this->ReadReq();
		*/
	} else {
		std::cout << "\033[32m  " << currentUUID << "\033[37m  ";
	} /* if currentUUID */

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
	if (resultCode != BT_GATT_RESULT_SUCCESS) {
		std::cout << btConnId << ": Writing client characteristic configuration failed: " << resultCode << ", " << resultSupplier << std::endl;
		this->DisconnectReq(gattId, btConnId);
	}
	else {
		std::cout << btConnId << ": Waiting indications...\n";
	}
}

void  GattClient::ReadCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId, const std::vector< uint8_t >& value ) {
	if (resultCode != BT_GATT_RESULT_SUCCESS) {
		std::cout << btConnId << ": Reading client characteristic failed: " << resultCode << ", " << resultSupplier << std::endl;
		this->DisconnectReq(gattId, btConnId);
	}
	else {
		std::cout << btConnId << ": " << " Data: ";
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

