#ifndef __GATTCLIENT_H___
#define __GATTCLIENT_H___
#include "glue/gatt_proxy-glue.h"

using namespace com::bluegiga::v2::bt;

extern DBus::BusDispatcher dispatcher;

struct Characteristic {
	Characteristic(): value_handle(0), client_characteristic_configuration_handle(0), property(0) {}
	uint16_t value_handle;
	uint16_t client_characteristic_configuration_handle;
	uint8_t property;
};

struct DiscoveredService {
	DiscoveredService(): start_handle(0), end_handle(0) {}
	uint16_t start_handle;
	uint16_t end_handle;
	std::vector<uint8_t> uuid;
	Characteristic temperature_measurement_characteristic;
};

// create maps of key particle characteristics
// This is necessary because each handle is found independently
typedef std::map< uint16_t, uint16_t, std::less< uint16_t > > passcodes_t;
typedef std::map< uint16_t, uint16_t, std::less< uint16_t > > opcodes_t;
typedef std::map< uint16_t, uint16_t, std::less< uint16_t > > operands_t;
typedef std::map< uint16_t, uint16_t, std::less< uint16_t > > results_t;
typedef std::map< uint16_t, uint16_t, std::less< uint16_t > > gumstick_t;

// map of target addresses and how many times they have been connected to
typedef std::map< std::string, uint16_t, std::less< std::string > > targets_t;

class GattClient: public gatt_proxy, public DBus::IntrospectableProxy, public DBus::ObjectProxy 
{
   public:
		GattClient(DBus::Connection &connection, const char *path, const char *name): DBus::ObjectProxy(connection, path, name), gid(0), registered(false)
		{}


		// Helper 
		bool startStopScan(bool start) {
			if (start)
				this->ScanReqStart(this->getGattId(), 0);
			else
				this->ScanReqStop(this->getGattId());
			dispatcher.enter(); // leaving the dispatcher after ScanCfm
			return this->isRegistered();
		}

		void InitTargets();
		virtual void ScanCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier);
		virtual void RegisterCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint16_t& context);
		virtual void UnregisterCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier);
		virtual void ReportInd(const uint32_t& gattId, const uint8_t& eventType, const std::string& address, const std::string& permanentAddress, const std::vector< uint8_t >& data, const int16_t& rssi);
		virtual void DiscoverServicesInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& startHandle, const uint16_t& endHandle, const std::vector< uint8_t >& uuid);
		virtual void DiscoverServicesCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId);
		virtual void CentralCfm(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& resultCode, const uint16_t& resultSupplier);
		virtual void ConnectInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& connInfo, const std::string& address, const uint16_t& mtu);
		virtual void DisconnectInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& reasonCode, const uint16_t& reasonSupplier, const std::string& address, const uint32_t& connInfo);
		virtual void DiscoverCharacInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& declarationHandle, const uint8_t& property, const std::vector< uint8_t >& uuid, const uint16_t& valueHandle);
		virtual void DiscoverCharacCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId);
		virtual void DiscoverCharacDescriptorsInd(const uint32_t& gattId, const uint32_t& btConnId, const std::vector< uint8_t >& uuid, const uint16_t& descriptorHandle);
		virtual void DiscoverCharacDescriptorsCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId);
		virtual void NotificationInd(const uint32_t& gattId, const uint32_t& btConnId, const std::string& address, const uint16_t& valueHandle, const std::vector< uint8_t >& value, const uint32_t& connInfo);
		virtual void WriteCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId);	
		virtual void ReadCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId, const std::vector< uint8_t >& value);	

		inline uint32_t getGattId() const { return this->gid; }
		inline bool isRegistered() const { return this->registered; }

  private:
		uint32_t gid;
		bool registered;
		std::map<uint32_t, DiscoveredService> health_thermometer_service;
		static const uint8_t client_characteristic_configuration_uuid[16];
		static const uint8_t health_thermometer_uuid[16];
		static const uint8_t temperature_measurement_uuid[16]; 
};

#endif

