#ifndef USB_DEVICE_DRD_H
#define USB_DEVICE_DRD_H

#include <usb_device.h>

class USB_Device_CH32X035: public USB_Device {
  private:
    USB_DeviceManager *manager;
    unsigned char *endpoint_buffers_rx[8];
    unsigned char *endpoint_buffers_tx[8];

    void AssignEndpointsBuffers();
    void AssignEndpointsModes();

    unsigned int CreateEndpointBuffer(unsigned int endpoint_no);
public:
    USB_Device_CH32X035();

    void Init(USB_DeviceManager *m) override;
    void ConfigureEndpoint(unsigned int endpoint_no, USBEndpointConfiguration rx_config,
                                  USBEndpointConfiguration tx_config) override;
    void ConfigureEndpointRX(unsigned int endpoint_no, USBEndpointConfiguration config) override;
    void ConfigureEndpointTX(unsigned int endpoint_no, USBEndpointConfiguration config) override;
    void SetEndpointData(unsigned endpoint_no, const void *data, unsigned int length) override;
    void ZeroTransfer(unsigned int endpoint_no) override;
    void InterruptHandler() override;
    void SetEndpointTransferType(unsigned int endpoint, USBEndpointTransferType transfer_type) override;
    void Connect() override;
    void Reset() override;
    void SetAddress(unsigned short address) override;
    const unsigned int GetEnabledEndpoints() override;
};

#endif
