bin_PROGRAMS = leet
leet_SOURCES = main.cc gattclient.cc

leet_LDFLAGS = -ldbus-c++-1

BUILT_SOURCES = glue/gatt_proxy-glue.h

glue/gatt_proxy-glue.h: ${XMLDIR}/gatt.xml
	mkdir -p glue
	cpp -P \
		-DINC_RegisterReq \
        -DINC_RegisterCfm \
        -DINC_UnregisterReq \
        -DINC_UnregisterCfm \
        -DINC_ScanReqStart \
        -DINC_ScanReqStop \
        -DINC_ScanCfm \
        -DINC_ReportInd \
        -DINC_DiscoverServicesInd \
        -DINC_DiscoverServicesCfm \
        -DINC_DiscoverAllPrimaryServicesReq \
        -DINC_CentralReq \
        -DINC_CentralCfm \
        -DINC_ConnectInd \
        -DINC_DisconnectInd \
        -DINC_DisconnectReq \
        -DINC_DiscoverAllCharacOfAServiceReq \
        -DINC_DiscoverCharacInd \
        -DINC_DiscoverCharacCfm \
        -DINC_DiscoverAllCharacDescriptorsReq \
        -DINC_DiscoverCharacDescriptorsInd \
        -DINC_DiscoverCharacDescriptorsCfm \
        -DINC_WriteClientConfigurationReq \
        -DINC_WriteCfm \
        -DINC_NotificationInd \
        -DINC_CancelReq \
	$< > /tmp/trimmedgatt.xml
	dbusxx-xml2cpp /tmp/trimmedgatt.xml --proxy=$@
	rm -f /tmp/trimmedgatt.xml
