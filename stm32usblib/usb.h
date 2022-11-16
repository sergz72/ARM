#ifndef _USB_H
#define _USB_H

#include <board.h>

#ifndef USB_EP_MAX
#define USB_EP_MAX 15
#endif

#ifndef USB_SETUP_SIZE
#define USB_SETUP_SIZE 12
#endif

typedef enum
{
  PCD_LPM_L0_ACTIVE = 0x00U, /* on */
  PCD_LPM_L1_ACTIVE = 0x01U, /* LPM L1 sleep */
} USB_LPM_MsgTypeDef;

/* Following USB Device status */
typedef enum {
  USB_OK   = 0,
  USB_BUSY,
  USB_FAIL
}USB_StatusTypeDef;

typedef struct
{
  unsigned int dev_endpoints;        /*!< Device Endpoints number.
                                      This parameter depends on the used USB core.
                                      This parameter must be a number between Min_Data = 1 and Max_Data = 15 */
  unsigned int dma_enable;           /*!< Enable or disable of the USB embedded DMA.                             */
  unsigned int Sof_enable;           /*!< Enable or disable the output of the SOF signal.                        */
  unsigned int use_dedicated_ep1;    /*!< Enable or disable the use of the dedicated EP1 interrupt.              */
  unsigned int vbus_sensing_enable;  /*!< Enable or disable the VBUS Sensing feature.                            */
  unsigned int use_external_vbus;    /*!< Enable or disable the use of the external VBUS.                        */
  unsigned int dev_remote_wakeup;
  void *pEpClassData[USB_EP_MAX];
  void *pItfClassData[USB_EP_MAX];
  void *pCDCClassData[USB_EP_MAX];
} USB_OTG_CfgTypeDef;

typedef struct
{
  unsigned int   is_stall;       /*!< Endpoint stall condition
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */
  unsigned int   type;           /*!< Endpoint type
                                 This parameter can be any value of @ref USB_EP_Type_                     */
  unsigned char *xfer_buff;     /*!< Pointer to transfer buffer                                               */
  unsigned int  dma_addr;       /*!< 32 bits aligned transfer buffer address                                  */
  unsigned int  xfer_len;       /*!< Current transfer length                                                  */
  unsigned int  xfer_count;     /*!< Partial transfer length in case of multi packet transfer                 */
} USB_OTG_EPTypeDef;

typedef  struct
{
  unsigned char   bmRequest;
  unsigned char   bRequest;
  unsigned short  wValue;
  unsigned short  wIndex;
  unsigned short  wLength;
} USB_SetupReqTypedef;

typedef void USB_Callback(void);
typedef void USB_UINTCallback(unsigned int);
typedef void USB_LPMCallback(USB_LPM_MsgTypeDef);
typedef unsigned int USB_UINTRUINTCallback(unsigned int);

typedef struct {
  USB_Callback *Reset_Callback;
  USB_Callback *Connect_Callback;
  USB_Callback *Disconnect_Callback;
  USB_Callback *Suspend_Callback;
  USB_Callback *Resume_Callback;
  USB_Callback *SOF_Callback;
  USB_Callback *SetupStage_Callback;
  USB_Callback *ISOOUTIncomplete_Callback;
  USB_Callback *ISOINIncomplete_Callback;
  USB_UINTCallback *DataOutStage_Callback;
  USB_UINTCallback *DataInStage_Callback;
  USB_Callback *LPM_Callback;
} USB_Callbacks;

typedef struct
{
  USB_UINTRUINTCallback *Init;
  USB_UINTCallback  *DeInit;
  /* Control Endpoints*/
  USB_UINTCallback  *Setup;
  USB_Callback  *EP0_TxSent;
  USB_Callback  *EP0_RxReady;
  /* Class Specific Endpoints*/
  USB_UINTCallback  *DataIn;
  USB_UINTCallback  *DataOut;
  USB_Callback  *SOF;
  USB_UINTCallback  *IsoINIncomplete;
  USB_UINTCallback  *IsoOUTIncomplete;
#ifdef USBD_SUPPORT_USER_STRING
  unsigned char  *(*GetUsrStrDescriptor)(struct _USBD_HandleTypeDef *pdev , unsigned int index,  unsigned int *length);
#endif
} USB_ClassTypeDef;

/* USB Device descriptors structure */
typedef struct
{
  unsigned char  *(*GetDeviceDescriptor)(unsigned int *length);
  unsigned char  *(*GetLangIDStrDescriptor)(unsigned int *length);
  unsigned char  *(*GetManufacturerStrDescriptor)(unsigned int *length);
  unsigned char  *(*GetProductStrDescriptor)(unsigned int *length);
  unsigned char  *(*GetSerialStrDescriptor)(unsigned int *length);
  unsigned char  *(*GetConfigurationStrDescriptor)(unsigned int *length);
  unsigned char  *(*GetInterfaceStrDescriptor)(unsigned int *length);
#ifdef USBD_LPM_ENABLED
  unsigned char  *(*GetBOSDescriptor)(unsigned int *length);
#endif
} USB_DescriptorsTypeDef;

typedef struct
{
  USB_OTG_GlobalTypeDef   *Instance;
  USB_OTG_CfgTypeDef      *Cfg;
  USB_Callbacks           *Callbacks;
  USB_ClassTypeDef       *pClass;
  USB_DescriptorsTypeDef *pDesc;
  USB_OTG_EPTypeDef       IN_ep[USB_EP_MAX];  /*!< IN endpoint parameters             */
  USB_OTG_EPTypeDef       OUT_ep[USB_EP_MAX]; /*!< OUT endpoint parameters            */
  union
  {
    unsigned char data[USB_SETUP_SIZE];  /*!< Setup packet buffer                */
    USB_SetupReqTypedef req;
  } Setup;
  unsigned int ep0_state;
  unsigned int ep0_data_len;
  unsigned int ep0_total_length;
  unsigned int ep0_rem_length;
  unsigned int dev_state;
  unsigned int dev_old_state;
  unsigned int dev_address;
  unsigned int dev_config;
  unsigned int dev_remote_wakeup;
  unsigned int dev_test_mode;
} USB_HandleTypeDef;

#define USBx_DEVICE     ((USB_OTG_DeviceTypeDef *)((unsigned int)USBHandle.Instance + USB_OTG_DEVICE_BASE))
#define USBx_INEP(i)    ((USB_OTG_INEndpointTypeDef *)((unsigned int)USBHandle.Instance + USB_OTG_IN_ENDPOINT_BASE + (i)*USB_OTG_EP_REG_SIZE))
#define USBx_OUTEP(i)   ((USB_OTG_OUTEndpointTypeDef *)((unsigned int)USBHandle.Instance + USB_OTG_OUT_ENDPOINT_BASE + (i)*USB_OTG_EP_REG_SIZE))

#define USB_OTG_FS_MAX_PACKET_SIZE 64

/*  EP0 State */
#define USB_EP0_IDLE                                     0
#define USB_EP0_SETUP                                    1
#define USB_EP0_DATA_IN                                  2
#define USB_EP0_DATA_OUT                                 3
#define USB_EP0_STATUS_IN                                4
#define USB_EP0_STATUS_OUT                               5
#define USB_EP0_STALL                                    6

#define USB_EP_TYPE_CTRL                                 0
#define USB_EP_TYPE_ISOC                                 1
#define USB_EP_TYPE_BULK                                 2
#define USB_EP_TYPE_INTR                                 3

#define USB_HS_MAX_PACKET_SIZE                            512
#define USB_FS_MAX_PACKET_SIZE                            64
#define USB_MAX_EP0_SIZE                                  64

#define  USB_REQ_RECIPIENT_DEVICE                       0x00
#define  USB_REQ_RECIPIENT_INTERFACE                    0x01
#define  USB_REQ_RECIPIENT_ENDPOINT                     0x02

#define  USB_REQ_GET_STATUS                             0x00
#define  USB_REQ_CLEAR_FEATURE                          0x01
#define  USB_REQ_SET_FEATURE                            0x03
#define  USB_REQ_SET_ADDRESS                            0x05
#define  USB_REQ_GET_DESCRIPTOR                         0x06
#define  USB_REQ_SET_DESCRIPTOR                         0x07
#define  USB_REQ_GET_CONFIGURATION                      0x08
#define  USB_REQ_SET_CONFIGURATION                      0x09
#define  USB_REQ_GET_INTERFACE                          0x0A
#define  USB_REQ_SET_INTERFACE                          0x0B
#define  USB_REQ_SYNCH_FRAME                            0x0C

#define USB_FEATURE_EP_HALT                                0
#define USB_FEATURE_REMOTE_WAKEUP                          1
#define USB_FEATURE_TEST_MODE                              2

#define USB_STATE_DEFAULT                                1
#define USB_STATE_ADDRESSED                              2
#define USB_STATE_CONFIGURED                             3
#define USB_STATE_SUSPENDED                              4

#define  USB_DESC_TYPE_DEVICE                              1
#define  USB_DESC_TYPE_CONFIGURATION                       2
#define  USB_DESC_TYPE_STRING                              3
#define  USB_DESC_TYPE_INTERFACE                           4
#define  USB_DESC_TYPE_ENDPOINT                            5
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                    6
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION           7
#define  USB_DESC_TYPE_BOS                                 0x0F

#define  USB_IDX_LANGID_STR                            0x00
#define  USB_IDX_MFC_STR                               0x01
#define  USB_IDX_PRODUCT_STR                           0x02
#define  USB_IDX_SERIAL_STR                            0x03
#define  USB_IDX_CONFIG_STR                            0x04
#define  USB_IDX_INTERFACE_STR                         0x05

#define USB_CONFIG_REMOTE_WAKEUP                           2
#define USB_CONFIG_SELF_POWERED                            1

#define  USB_LEN_DEV_QUALIFIER_DESC                     0x0A
#define  USB_LEN_DEV_DESC                               0x12
#define  USB_LEN_LANGID_STR_DESC                        0x04

#define  USB_REQ_TYPE_STANDARD                          0x00
#define  USB_REQ_TYPE_CLASS                             0x20
#define  USB_REQ_TYPE_VENDOR                            0x40
#define  USB_REQ_TYPE_MASK                              0x60

#define USB_IAD_SIZE 8
#define USB_EP_IN 0x80

#define LOBYTE(x)  (x & 0xFF)
#define HIBYTE(x)  ((x & 0xFF00)>>8)
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

/* In HS mode and when the DMA is used, all variables and data structures dealing
   with the DMA during the transaction process should be 4-bytes aligned */

#if defined   (__GNUC__)        /* GNU Compiler */
#define __ALIGN_END    __attribute__ ((aligned (4)))
#define __ALIGN_BEGIN
#else
#define __ALIGN_END
  #if defined   (__CC_ARM)      /* ARM Compiler */
    #define __ALIGN_BEGIN    __align(4)
  #elif defined (__ICCARM__)    /* IAR Compiler */
    #define __ALIGN_BEGIN
  #elif defined  (__TASKING__)  /* TASKING Compiler */
    #define __ALIGN_BEGIN    __align(4)
  #endif /* __CC_ARM */
#endif /* __GNUC__ */

void USBInit(USB_OTG_GlobalTypeDef *USBx,
             const USB_OTG_CfgTypeDef *cfg,
             const USB_Callbacks *callbacks,
             const USB_ClassTypeDef *pClass,
             const USB_DescriptorsTypeDef *pDesc);
void USB_FlushTxFifo (unsigned int num);
unsigned int USB_ReadInterrupts(void);
unsigned int USB_ReadDevAllOutEpInterrupt(void);
unsigned int USB_ReadDevAllInEpInterrupt(void);
void USB_EP0_OutStart(void);
void USB_ActivateSetup(void);
void USBInterruptHandler(void);
void USB_EP_Open(unsigned int is_in, unsigned int num, unsigned int type);
void *USB_ReadPacket(unsigned char *dest, unsigned int len);
void USB_WritePacket(unsigned char *src, unsigned int ch_ep_num, unsigned int len);
unsigned int USB_ReadDevOutEPInterrupt(unsigned int epnum);
unsigned int USB_ReadDevInEPInterrupt(unsigned int epnum);
void USB_WriteEmptyTxFifo(unsigned int epnum);
void USB_StdDevReq(void);
void USB_StdItfReq(void);
void USB_StdEPReq(void);
void USB_CtlError(void);
void USB_StallEP(unsigned int is_in, unsigned int num);
unsigned int USB_IsStallEP(unsigned int is_in, unsigned int num);
void USB_ClearStallEP(unsigned int is_in, unsigned int num);
void USB_CtlSendStatus(void);
void USB_EP0StartXfer(unsigned int is_in);
void USB_EPStartXfer(unsigned int is_in, unsigned int num);
void USB_CtlSendData(unsigned char *pbuf, unsigned int len);
void USB_SetDevAddress(unsigned int address);
void USB_ClrClassConfig(unsigned int cfgidx);
USB_StatusTypeDef USB_SetClassConfig(unsigned int cfgidx);
void USB_EP_Receive(unsigned int num, unsigned char *pBuf, unsigned int len);
void USB_Transmit(unsigned int num, unsigned char *pBuf, unsigned int len);
void USB_RunTestMode(void);
void USB_CtlReceiveStatus(void);
void USB_GetString(unsigned char *desc, unsigned char *unicode, unsigned int *len);
void USB_DeactivateEndpoint(unsigned int is_in, unsigned int num);
void USB_CtlPrepareRx (unsigned char *pbuf, unsigned int len);
unsigned int USB_EP_GetRxCount(unsigned int num);
void USB_FIFO_Init(void);
void USB_SetRxFiFo(unsigned int size);
void USB_SetTxFiFo(unsigned int fifo, unsigned int size);

void USB_Default_ResetCallback(void);
void USB_Default_ConnectCallback(void);
void USB_Default_DisconnectCallback(void);
void USB_Default_SuspendCallback(void);
void USB_Default_ResumeCallback(void);
void USB_Default_SOFCallback(void);
void USB_Default_SetupStageCallback(void);
void USB_Default_ISOOUTIncompleteCallback(void);
void USB_Default_ISOINIncompleteCallback(void);
void USB_Default_DataOutStageCallback(unsigned int epnum);
void USB_Default_DataInStageCallback(unsigned int epnum);
void USB_Default_LPMCallback(void);

extern USB_HandleTypeDef USBHandle;
extern const USB_Callbacks Default_Callbacks;
extern const unsigned char USB_CfgFSDesc[];
extern const USB_DescriptorsTypeDef USB_Desc;

#endif
