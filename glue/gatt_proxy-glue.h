
/*
 *	This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */

#ifndef __dbusxx__glue_gatt_proxy_glue_h__PROXY_MARSHAL_H
#define __dbusxx__glue_gatt_proxy_glue_h__PROXY_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace com {
namespace bluegiga {
namespace v2 {
namespace bt {

class gatt_proxy
: public ::DBus::InterfaceProxy
{
public:

    gatt_proxy()
    : ::DBus::InterfaceProxy("com.bluegiga.v2.bt.gatt")
    {
        connect_signal(gatt_proxy, CentralCfm, _CentralCfm_stub);
        connect_signal(gatt_proxy, ConnectInd, _ConnectInd_stub);
        connect_signal(gatt_proxy, DisconnectInd, _DisconnectInd_stub);
        connect_signal(gatt_proxy, ScanCfm, _ScanCfm_stub);
        connect_signal(gatt_proxy, RegisterCfm, _RegisterCfm_stub);
        connect_signal(gatt_proxy, UnregisterCfm, _UnregisterCfm_stub);
        connect_signal(gatt_proxy, DiscoverServicesInd, _DiscoverServicesInd_stub);
        connect_signal(gatt_proxy, DiscoverServicesCfm, _DiscoverServicesCfm_stub);
        connect_signal(gatt_proxy, DiscoverCharacInd, _DiscoverCharacInd_stub);
        connect_signal(gatt_proxy, DiscoverCharacCfm, _DiscoverCharacCfm_stub);
        connect_signal(gatt_proxy, DiscoverCharacDescriptorsInd, _DiscoverCharacDescriptorsInd_stub);
        connect_signal(gatt_proxy, DiscoverCharacDescriptorsCfm, _DiscoverCharacDescriptorsCfm_stub);
        connect_signal(gatt_proxy, ReadCfm, _ReadCfm_stub);
        connect_signal(gatt_proxy, WriteCfm, _WriteCfm_stub);
        connect_signal(gatt_proxy, NotificationInd, _NotificationInd_stub);
        connect_signal(gatt_proxy, ReportInd, _ReportInd_stub);
    }

public:

    /* properties exported by this interface */
public:

    /* methods exported by this interface,
     * this functions will invoke the corresponding methods on the remote objects
     */
    void CentralReq(const uint32_t& gattId, const std::string& address, const uint32_t& flags, const uint16_t& preferredMtu)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << gattId;
        wi << address;
        wi << flags;
        wi << preferredMtu;
        call.member("CentralReq");
        ::DBus::Message ret = invoke_method (call);
    }

    void DisconnectReq(const uint32_t& gattId, const uint32_t& btConnId)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << gattId;
        wi << btConnId;
        call.member("DisconnectReq");
        ::DBus::Message ret = invoke_method (call);
    }

    void RegisterReq(const uint16_t& context)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << context;
        call.member("RegisterReq");
        ::DBus::Message ret = invoke_method (call);
    }

    void UnregisterReq(const uint32_t& gattId)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << gattId;
        call.member("UnregisterReq");
        ::DBus::Message ret = invoke_method (call);
    }

    void ReadReq(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& handle, const uint16_t& offset)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << gattId;
        wi << btConnId;
        wi << handle;
        wi << offset;
        call.member("ReadReq");
        ::DBus::Message ret = invoke_method (call);
    }

    void WriteReq(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& handle, const uint16_t& offset, const std::vector< uint8_t >& value)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << gattId;
        wi << btConnId;
        wi << handle;
        wi << offset;
        wi << value;
        call.member("WriteReq");
        ::DBus::Message ret = invoke_method (call);
    }

    void WriteClientConfigurationReq(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& clientConfigHandle, const uint16_t& valueHandle, const uint16_t& configuration)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << gattId;
        wi << btConnId;
        wi << clientConfigHandle;
        wi << valueHandle;
        wi << configuration;
        call.member("WriteClientConfigurationReq");
        ::DBus::Message ret = invoke_method (call);
    }

    void CancelReq(const uint32_t& gattId, const uint32_t& btConnId)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << gattId;
        wi << btConnId;
        call.member("CancelReq");
        ::DBus::Message ret = invoke_method (call);
    }

    void DiscoverAllPrimaryServicesReq(const uint32_t& gattId, const uint32_t& btConnId)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << gattId;
        wi << btConnId;
        call.member("DiscoverAllPrimaryServicesReq");
        ::DBus::Message ret = invoke_method (call);
    }

    void DiscoverAllCharacOfAServiceReq(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& startHandle, const uint16_t& endHandle)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << gattId;
        wi << btConnId;
        wi << startHandle;
        wi << endHandle;
        call.member("DiscoverAllCharacOfAServiceReq");
        ::DBus::Message ret = invoke_method (call);
    }

    void ScanReqStop(const uint32_t& gattId)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << gattId;
        call.member("ScanReqStop");
        ::DBus::Message ret = invoke_method (call);
    }

    void ScanReqStart(const uint32_t& gattId, const uint8_t& flags)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << gattId;
        wi << flags;
        call.member("ScanReqStart");
        ::DBus::Message ret = invoke_method (call);
    }

    void DiscoverAllCharacDescriptorsReq(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& startHandle, const uint16_t& endHandle)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << gattId;
        wi << btConnId;
        wi << startHandle;
        wi << endHandle;
        call.member("DiscoverAllCharacDescriptorsReq");
        ::DBus::Message ret = invoke_method (call);
    }


public:

    /* signal handlers for this interface
     */
    virtual void CentralCfm(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& resultCode, const uint16_t& resultSupplier) = 0;
    virtual void ConnectInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& connInfo, const std::string& address, const uint16_t& mtu) = 0;
    virtual void DisconnectInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& reasonCode, const uint16_t& reasonSupplier, const std::string& address, const uint32_t& connInfo) = 0;
    virtual void ScanCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier) = 0;
    virtual void RegisterCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint16_t& context) = 0;
    virtual void UnregisterCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier) = 0;
    virtual void DiscoverServicesInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& startHandle, const uint16_t& endHandle, const std::vector< uint8_t >& uuid) = 0;
    virtual void DiscoverServicesCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId) = 0;
    virtual void DiscoverCharacInd(const uint32_t& gattId, const uint32_t& btConnId, const uint16_t& declarationHandle, const uint8_t& property, const std::vector< uint8_t >& uuid, const uint16_t& valueHandle) = 0;
    virtual void DiscoverCharacCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId) = 0;
    virtual void DiscoverCharacDescriptorsInd(const uint32_t& gattId, const uint32_t& btConnId, const std::vector< uint8_t >& uuid, const uint16_t& descriptorHandle) = 0;
    virtual void DiscoverCharacDescriptorsCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId) = 0;
    virtual void ReadCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId, const std::vector< uint8_t >& value) = 0;
    virtual void WriteCfm(const uint32_t& gattId, const uint16_t& resultCode, const uint16_t& resultSupplier, const uint32_t& btConnId) = 0;
    virtual void NotificationInd(const uint32_t& gattId, const uint32_t& btConnId, const std::string& address, const uint16_t& valueHandle, const std::vector< uint8_t >& value, const uint32_t& connInfo) = 0;
    virtual void ReportInd(const uint32_t& gattId, const uint8_t& eventType, const std::string& address, const std::string& permanentAddress, const std::vector< uint8_t >& data, const int16_t& rssi) = 0;

private:

    /* unmarshalers (to unpack the DBus message before calling the actual signal handler)
     */
    void _CentralCfm_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint32_t btConnId;
        ri >> btConnId;
        uint16_t resultCode;
        ri >> resultCode;
        uint16_t resultSupplier;
        ri >> resultSupplier;
        CentralCfm(gattId, btConnId, resultCode, resultSupplier);
    }
    void _ConnectInd_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint32_t btConnId;
        ri >> btConnId;
        uint16_t resultCode;
        ri >> resultCode;
        uint16_t resultSupplier;
        ri >> resultSupplier;
        uint32_t connInfo;
        ri >> connInfo;
        std::string address;
        ri >> address;
        uint16_t mtu;
        ri >> mtu;
        ConnectInd(gattId, btConnId, resultCode, resultSupplier, connInfo, address, mtu);
    }
    void _DisconnectInd_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint32_t btConnId;
        ri >> btConnId;
        uint16_t reasonCode;
        ri >> reasonCode;
        uint16_t reasonSupplier;
        ri >> reasonSupplier;
        std::string address;
        ri >> address;
        uint32_t connInfo;
        ri >> connInfo;
        DisconnectInd(gattId, btConnId, reasonCode, reasonSupplier, address, connInfo);
    }
    void _ScanCfm_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint16_t resultCode;
        ri >> resultCode;
        uint16_t resultSupplier;
        ri >> resultSupplier;
        ScanCfm(gattId, resultCode, resultSupplier);
    }
    void _RegisterCfm_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint16_t resultCode;
        ri >> resultCode;
        uint16_t resultSupplier;
        ri >> resultSupplier;
        uint16_t context;
        ri >> context;
        RegisterCfm(gattId, resultCode, resultSupplier, context);
    }
    void _UnregisterCfm_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint16_t resultCode;
        ri >> resultCode;
        uint16_t resultSupplier;
        ri >> resultSupplier;
        UnregisterCfm(gattId, resultCode, resultSupplier);
    }
    void _DiscoverServicesInd_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint32_t btConnId;
        ri >> btConnId;
        uint16_t startHandle;
        ri >> startHandle;
        uint16_t endHandle;
        ri >> endHandle;
        std::vector< uint8_t > uuid;
        ri >> uuid;
        DiscoverServicesInd(gattId, btConnId, startHandle, endHandle, uuid);
    }
    void _DiscoverServicesCfm_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint16_t resultCode;
        ri >> resultCode;
        uint16_t resultSupplier;
        ri >> resultSupplier;
        uint32_t btConnId;
        ri >> btConnId;
        DiscoverServicesCfm(gattId, resultCode, resultSupplier, btConnId);
    }
    void _DiscoverCharacInd_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint32_t btConnId;
        ri >> btConnId;
        uint16_t declarationHandle;
        ri >> declarationHandle;
        uint8_t property;
        ri >> property;
        std::vector< uint8_t > uuid;
        ri >> uuid;
        uint16_t valueHandle;
        ri >> valueHandle;
        DiscoverCharacInd(gattId, btConnId, declarationHandle, property, uuid, valueHandle);
    }
    void _DiscoverCharacCfm_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint16_t resultCode;
        ri >> resultCode;
        uint16_t resultSupplier;
        ri >> resultSupplier;
        uint32_t btConnId;
        ri >> btConnId;
        DiscoverCharacCfm(gattId, resultCode, resultSupplier, btConnId);
    }
    void _DiscoverCharacDescriptorsInd_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint32_t btConnId;
        ri >> btConnId;
        std::vector< uint8_t > uuid;
        ri >> uuid;
        uint16_t descriptorHandle;
        ri >> descriptorHandle;
        DiscoverCharacDescriptorsInd(gattId, btConnId, uuid, descriptorHandle);
    }
    void _DiscoverCharacDescriptorsCfm_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint16_t resultCode;
        ri >> resultCode;
        uint16_t resultSupplier;
        ri >> resultSupplier;
        uint32_t btConnId;
        ri >> btConnId;
        DiscoverCharacDescriptorsCfm(gattId, resultCode, resultSupplier, btConnId);
    }
    void _ReadCfm_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint16_t resultCode;
        ri >> resultCode;
        uint16_t resultSupplier;
        ri >> resultSupplier;
        uint32_t btConnId;
        ri >> btConnId;
        std::vector< uint8_t > value;
        ri >> value;
        ReadCfm(gattId, resultCode, resultSupplier, btConnId, value);
    }
    void _WriteCfm_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint16_t resultCode;
        ri >> resultCode;
        uint16_t resultSupplier;
        ri >> resultSupplier;
        uint32_t btConnId;
        ri >> btConnId;
        WriteCfm(gattId, resultCode, resultSupplier, btConnId);
    }
    void _NotificationInd_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint32_t btConnId;
        ri >> btConnId;
        std::string address;
        ri >> address;
        uint16_t valueHandle;
        ri >> valueHandle;
        std::vector< uint8_t > value;
        ri >> value;
        uint32_t connInfo;
        ri >> connInfo;
        NotificationInd(gattId, btConnId, address, valueHandle, value, connInfo);
    }
    void _ReportInd_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        uint32_t gattId;
        ri >> gattId;
        uint8_t eventType;
        ri >> eventType;
        std::string address;
        ri >> address;
        std::string permanentAddress;
        ri >> permanentAddress;
        std::vector< uint8_t > data;
        ri >> data;
        int16_t rssi;
        ri >> rssi;
        ReportInd(gattId, eventType, address, permanentAddress, data, rssi);
    }
};

} } } } 
#endif //__dbusxx__glue_gatt_proxy_glue_h__PROXY_MARSHAL_H
