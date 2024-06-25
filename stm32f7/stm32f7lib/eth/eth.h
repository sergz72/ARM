#ifndef _ETH_H
#define _ETH_H

#include <board.h>
#include <queue.h>

#define ETH_AUTONEGOTIATION_ENABLE     1
#define ETH_AUTONEGOTIATION_DISABLE    0

#define ETH_SPEED_10M        0x00000000U
#define ETH_SPEED_100M       0x00004000U

#define ETH_MODE_FULLDUPLEX       0x00000800U
#define ETH_MODE_HALFDUPLEX       0x00000000U

#define ETH_MEDIA_INTERFACE_MII       0x00000000U
#define ETH_MEDIA_INTERFACE_RMII      SYSCFG_PMC_MII_RMII_SEL

#define ETH_RXPOLLING_MODE      0x00000000U
#define ETH_RXINTERRUPT_MODE    0x00000001U

#define ETH_CHECKSUM_BY_HARDWARE      0x00000000U
#define ETH_CHECKSUM_BY_SOFTWARE      0x00000001U

#define LAN8742A_PHY_ADDRESS           0

#define ETH_MAX_PACKET_SIZE    1524U    /*!< ETH_HEADER + ETH_EXTRA + ETH_VLAN_TAG + ETH_MAX_ETH_PAYLOAD + ETH_CRC */
#define ETH_HEADER               14U    /*!< 6 byte Dest addr, 6 byte Src addr, 2 byte length/type */
#define ETH_CRC                   4U    /*!< Ethernet CRC */
#define ETH_EXTRA                 2U    /*!< Extra bytes in some cases */
#define ETH_VLAN_TAG              4U    /*!< optional 802.1q VLAN Tag */
#define ETH_MIN_ETH_PAYLOAD       46U    /*!< Minimum Ethernet payload size */
#define ETH_MAX_ETH_PAYLOAD       1500U    /*!< Maximum Ethernet payload size */
#define ETH_JUMBO_FRAME_PAYLOAD   9000U    /*!< Jumbo frame payload size */

#define ETH_DMARXDESC_OWN         0x80000000U  /*!< OWN bit: descriptor is owned by DMA engine  */
#define ETH_DMARXDESC_AFM         0x40000000U  /*!< DA Filter Fail for the rx frame  */
#define ETH_DMARXDESC_FL          0x3FFF0000U  /*!< Receive descriptor frame length  */
#define ETH_DMARXDESC_ES          0x00008000U  /*!< Error summary: OR of the following bits: DE || OE || IPC || LC || RWT || RE || CE */
#define ETH_DMARXDESC_DE          0x00004000U  /*!< Descriptor error: no more descriptors for receive frame  */
#define ETH_DMARXDESC_SAF         0x00002000U  /*!< SA Filter Fail for the received frame */
#define ETH_DMARXDESC_LE          0x00001000U  /*!< Frame size not matching with length field */
#define ETH_DMARXDESC_OE          0x00000800U  /*!< Overflow Error: Frame was damaged due to buffer overflow */
#define ETH_DMARXDESC_VLAN        0x00000400U  /*!< VLAN Tag: received frame is a VLAN frame */
#define ETH_DMARXDESC_FS          0x00000200U  /*!< First descriptor of the frame  */
#define ETH_DMARXDESC_LS          0x00000100U  /*!< Last descriptor of the frame  */
#define ETH_DMARXDESC_IPV4HCE     0x00000080U  /*!< IPC Checksum Error: Rx Ipv4 header checksum error   */
#define ETH_DMARXDESC_LC          0x00000040U  /*!< Late collision occurred during reception   */
#define ETH_DMARXDESC_FT          0x00000020U  /*!< Frame type - Ethernet, otherwise 802.3    */
#define ETH_DMARXDESC_RWT         0x00000010U  /*!< Receive Watchdog Timeout: watchdog timer expired during reception    */
#define ETH_DMARXDESC_RE          0x00000008U  /*!< Receive error: error reported by MII interface  */
#define ETH_DMARXDESC_DBE         0x00000004U  /*!< Dribble bit error: frame contains non int multiple of 8 bits  */
#define ETH_DMARXDESC_CE          0x00000002U  /*!< CRC error */
#define ETH_DMARXDESC_MAMPCE      0x00000001U  /*!< Rx MAC Address/Payload Checksum Error: Rx MAC address matched/ Rx Payload Checksum Error */

typedef struct
{
  unsigned int             AutoNegotiation;           /*!< Selects or not the AutoNegotiation mode for the external PHY
                                                           The AutoNegotiation allows an automatic setting of the Speed (10/100Mbps)
                                                           and the mode (half/full-duplex).
                                                           This parameter can be a value of @ref ETH_AutoNegotiation */

  unsigned int             Speed;                     /*!< Sets the Ethernet speed: 10/100 Mbps.
                                                           This parameter can be a value of @ref ETH_Speed */

  unsigned int             DuplexMode;                /*!< Selects the MAC duplex mode: Half-Duplex or Full-Duplex mode
                                                           This parameter can be a value of @ref ETH_Duplex_Mode */
  
  unsigned int             PhyAddress;                /*!< Ethernet PHY address.
                                                           This parameter must be a number between Min_Data = 0 and Max_Data = 32 */
  
  unsigned char            *MACAddr;                   /*!< MAC Address of used Hardware: must be pointer on an array of 6 bytes */
  
  unsigned int             RxMode;                    /*!< Selects the Ethernet Rx mode: Polling mode, Interrupt mode.
                                                           This parameter can be a value of @ref ETH_Rx_Mode */
  
  unsigned int             ChecksumMode;              /*!< Selects if the checksum is check by hardware or by software. 
                                                         This parameter can be a value of @ref ETH_Checksum_Mode */
  
  unsigned int             MediaInterface    ;               /*!< Selects the media-independent interface or the reduced media-independent interface. 
                                                         This parameter can be a value of @ref ETH_Media_Interface */
  unsigned int             ip_addr;
} ETH_InitTypeDef;

typedef enum
{
  ETH_STATE_RESET             = 0x00U,    /*!< Peripheral not yet Initialized or disabled         */
  ETH_STATE_READY             = 0x01U,    /*!< Peripheral Initialized and ready for use           */
  ETH_STATE_BUSY              = 0x02U,    /*!< an internal process is ongoing                     */
  ETH_STATE_BUSY_TX           = 0x12U,    /*!< Data Transmission process is ongoing               */
  ETH_STATE_BUSY_RX           = 0x22U,    /*!< Data Reception process is ongoing                  */
  ETH_STATE_BUSY_TX_RX        = 0x32U,    /*!< Data Transmission and Reception process is ongoing */
  ETH_STATE_BUSY_WR           = 0x42U,    /*!< Write process is ongoing                           */
  ETH_STATE_BUSY_RD           = 0x82U,    /*!< Read process is ongoing                            */
  ETH_STATE_TIMEOUT           = 0x03U,    /*!< Timeout state                                      */
  ETH_STATE_ERROR             = 0x04U     /*!< Reception process is ongoing                       */
} ETH_StateTypeDef;

typedef struct  
{
  __IO unsigned int   Status;           /*!< Status */
  
  unsigned int   ControlBufferSize;     /*!< Control and Buffer1, Buffer2 lengths */
  
  unsigned int   Buffer1Addr;           /*!< Buffer1 address pointer */
  
  unsigned int   Buffer2NextDescAddr;   /*!< Buffer2 or next descriptor address pointer */
  
  /*!< Enhanced Ethernet DMA PTP Descriptors */
  unsigned int   ExtendedStatus;        /*!< Extended status for PTP receive descriptor */
  
  unsigned int   Reserved1;             /*!< Reserved */
  
  unsigned int   TimeStampLow;          /*!< Time Stamp Low value for transmit and receive */
  
  unsigned int   TimeStampHigh;         /*!< Time Stamp High value for transmit and receive */

} ETH_DMADescTypeDef;

typedef struct  
{
  ETH_DMADescTypeDef *FSRxDesc;          /*!< First Segment Rx Desc */
  
  ETH_DMADescTypeDef *LSRxDesc;          /*!< Last Segment Rx Desc */
  
  unsigned int  SegCount;                    /*!< Segment count */
  
  unsigned int length;                       /*!< Frame length */
  
  unsigned int buffer;                       /*!< Frame buffer */

} ETH_DMARxFrameInfos;

typedef struct
{
  ETH_TypeDef                *Instance;     /*!< Register base address       */
  
  ETH_InitTypeDef            Init;          /*!< Ethernet Init Configuration */
  
  unsigned int               LinkStatus;    /*!< Ethernet link status        */
  
  ETH_DMADescTypeDef         *RxDesc;       /*!< Rx descriptor to Get        */
  
  ETH_DMADescTypeDef         *TxDesc;       /*!< Tx descriptor to Set        */
  
  ETH_DMARxFrameInfos        RxFrameInfos;  /*!< last Rx frame infos         */
  
  __IO ETH_StateTypeDef  State;         /*!< ETH communication state     */
} ETH_HandleTypeDef;

typedef struct
{
  unsigned int             Watchdog;                  /*!< Selects or not the Watchdog timer
                                                           When enabled, the MAC allows no more then 2048 bytes to be received.
                                                           When disabled, the MAC can receive up to 16384 bytes.
                                                           This parameter can be a value of @ref ETH_Watchdog */

  unsigned int             Jabber;                    /*!< Selects or not Jabber timer
                                                           When enabled, the MAC allows no more then 2048 bytes to be sent.
                                                           When disabled, the MAC can send up to 16384 bytes.
                                                           This parameter can be a value of @ref ETH_Jabber */

  unsigned int             InterFrameGap;             /*!< Selects the minimum IFG between frames during transmission.
                                                           This parameter can be a value of @ref ETH_Inter_Frame_Gap */

  unsigned int             CarrierSense;              /*!< Selects or not the Carrier Sense.
                                                           This parameter can be a value of @ref ETH_Carrier_Sense */

  unsigned int             ReceiveOwn;                /*!< Selects or not the ReceiveOwn,
                                                           ReceiveOwn allows the reception of frames when the TX_EN signal is asserted
                                                           in Half-Duplex mode.
                                                           This parameter can be a value of @ref ETH_Receive_Own */

  unsigned int             LoopbackMode;              /*!< Selects or not the internal MAC MII Loopback mode.
                                                           This parameter can be a value of @ref ETH_Loop_Back_Mode */

  unsigned int             ChecksumOffload;           /*!< Selects or not the IPv4 checksum checking for received frame payloads' TCP/UDP/ICMP headers.
                                                           This parameter can be a value of @ref ETH_Checksum_Offload */

  unsigned int             RetryTransmission;         /*!< Selects or not the MAC attempt retries transmission, based on the settings of BL,
                                                           when a collision occurs (Half-Duplex mode).
                                                           This parameter can be a value of @ref ETH_Retry_Transmission */

  unsigned int             AutomaticPadCRCStrip;      /*!< Selects or not the Automatic MAC Pad/CRC Stripping.
                                                           This parameter can be a value of @ref ETH_Automatic_Pad_CRC_Strip */

  unsigned int             BackOffLimit;              /*!< Selects the BackOff limit value.
                                                           This parameter can be a value of @ref ETH_Back_Off_Limit */

  unsigned int             DeferralCheck;             /*!< Selects or not the deferral check function (Half-Duplex mode).
                                                           This parameter can be a value of @ref ETH_Deferral_Check */

  unsigned int             ReceiveAll;                /*!< Selects or not all frames reception by the MAC (No filtering).
                                                           This parameter can be a value of @ref ETH_Receive_All */

  unsigned int             SourceAddrFilter;          /*!< Selects the Source Address Filter mode.                                                           
                                                           This parameter can be a value of @ref ETH_Source_Addr_Filter */

  unsigned int             PassControlFrames;         /*!< Sets the forwarding mode of the control frames (including unicast and multicast PAUSE frames)                                                          
                                                           This parameter can be a value of @ref ETH_Pass_Control_Frames */

  unsigned int             BroadcastFramesReception;  /*!< Selects or not the reception of Broadcast Frames.
                                                           This parameter can be a value of @ref ETH_Broadcast_Frames_Reception */

  unsigned int             DestinationAddrFilter;     /*!< Sets the destination filter mode for both unicast and multicast frames.
                                                           This parameter can be a value of @ref ETH_Destination_Addr_Filter */

  unsigned int             PromiscuousMode;           /*!< Selects or not the Promiscuous Mode
                                                           This parameter can be a value of @ref ETH_Promiscuous_Mode */

  unsigned int             MulticastFramesFilter;     /*!< Selects the Multicast Frames filter mode: None/HashTableFilter/PerfectFilter/PerfectHashTableFilter.
                                                           This parameter can be a value of @ref ETH_Multicast_Frames_Filter */

  unsigned int             UnicastFramesFilter;       /*!< Selects the Unicast Frames filter mode: HashTableFilter/PerfectFilter/PerfectHashTableFilter.
                                                           This parameter can be a value of @ref ETH_Unicast_Frames_Filter */

  unsigned int             HashTableHigh;             /*!< This field holds the higher 32 bits of Hash table.
                                                           This parameter must be a number between Min_Data = 0x0 and Max_Data = 0xFFFFFFFF */

  unsigned int             HashTableLow;              /*!< This field holds the lower 32 bits of Hash table.
                                                           This parameter must be a number between Min_Data = 0x0 and Max_Data = 0xFFFFFFFF  */

  unsigned int             PauseTime;                 /*!< This field holds the value to be used in the Pause Time field in the transmit control frame. 
                                                           This parameter must be a number between Min_Data = 0x0 and Max_Data = 0xFFFF */

  unsigned int             ZeroQuantaPause;           /*!< Selects or not the automatic generation of Zero-Quanta Pause Control frames.
                                                           This parameter can be a value of @ref ETH_Zero_Quanta_Pause */

  unsigned int             PauseLowThreshold;         /*!< This field configures the threshold of the PAUSE to be checked for
                                                           automatic retransmission of PAUSE Frame.
                                                           This parameter can be a value of @ref ETH_Pause_Low_Threshold */

  unsigned int             UnicastPauseFrameDetect;   /*!< Selects or not the MAC detection of the Pause frames (with MAC Address0
                                                           unicast address and unique multicast address).
                                                           This parameter can be a value of @ref ETH_Unicast_Pause_Frame_Detect */

  unsigned int             ReceiveFlowControl;        /*!< Enables or disables the MAC to decode the received Pause frame and
                                                           disable its transmitter for a specified time (Pause Time)
                                                           This parameter can be a value of @ref ETH_Receive_Flow_Control */

  unsigned int             TransmitFlowControl;       /*!< Enables or disables the MAC to transmit Pause frames (Full-Duplex mode)
                                                           or the MAC back-pressure operation (Half-Duplex mode)
                                                           This parameter can be a value of @ref ETH_Transmit_Flow_Control */

  unsigned int             VLANTagComparison;         /*!< Selects the 12-bit VLAN identifier or the complete 16-bit VLAN tag for
                                                           comparison and filtering.
                                                           This parameter can be a value of @ref ETH_VLAN_Tag_Comparison */

  unsigned int             VLANTagIdentifier;         /*!< Holds the VLAN tag identifier for receive frames */
} ETH_MACInitTypeDef;

typedef struct
{
  unsigned int              DropTCPIPChecksumErrorFrame; /*!< Selects or not the Dropping of TCP/IP Checksum Error Frames.
                                                             This parameter can be a value of @ref ETH_Drop_TCP_IP_Checksum_Error_Frame */

  unsigned int             ReceiveStoreForward;         /*!< Enables or disables the Receive store and forward mode.
                                                             This parameter can be a value of @ref ETH_Receive_Store_Forward */

  unsigned int             FlushReceivedFrame;          /*!< Enables or disables the flushing of received frames.
                                                             This parameter can be a value of @ref ETH_Flush_Received_Frame */

  unsigned int             TransmitStoreForward;        /*!< Enables or disables Transmit store and forward mode.
                                                             This parameter can be a value of @ref ETH_Transmit_Store_Forward */

  unsigned int             TransmitThresholdControl;    /*!< Selects or not the Transmit Threshold Control.
                                                             This parameter can be a value of @ref ETH_Transmit_Threshold_Control */

  unsigned int             ForwardErrorFrames;          /*!< Selects or not the forward to the DMA of erroneous frames.
                                                             This parameter can be a value of @ref ETH_Forward_Error_Frames */

  unsigned int             ForwardUndersizedGoodFrames; /*!< Enables or disables the Rx FIFO to forward Undersized frames (frames with no Error
                                                             and length less than 64 bytes) including pad-bytes and CRC)
                                                             This parameter can be a value of @ref ETH_Forward_Undersized_Good_Frames */

  unsigned int             ReceiveThresholdControl;     /*!< Selects the threshold level of the Receive FIFO.
                                                             This parameter can be a value of @ref ETH_Receive_Threshold_Control */

  unsigned int             SecondFrameOperate;          /*!< Selects or not the Operate on second frame mode, which allows the DMA to process a second
                                                             frame of Transmit data even before obtaining the status for the first frame.
                                                             This parameter can be a value of @ref ETH_Second_Frame_Operate */

  unsigned int             AddressAlignedBeats;         /*!< Enables or disables the Address Aligned Beats.
                                                             This parameter can be a value of @ref ETH_Address_Aligned_Beats */

  unsigned int             FixedBurst;                  /*!< Enables or disables the AHB Master interface fixed burst transfers.
                                                             This parameter can be a value of @ref ETH_Fixed_Burst */

  unsigned int             RxDMABurstLength;            /*!< Indicates the maximum number of beats to be transferred in one Rx DMA transaction.
                                                             This parameter can be a value of @ref ETH_Rx_DMA_Burst_Length */

  unsigned int             TxDMABurstLength;            /*!< Indicates the maximum number of beats to be transferred in one Tx DMA transaction.
                                                             This parameter can be a value of @ref ETH_Tx_DMA_Burst_Length */

  unsigned int             EnhancedDescriptorFormat;    /*!< Enables the enhanced descriptor format.
                                                             This parameter can be a value of @ref ETH_DMA_Enhanced_descriptor_format */

  unsigned int             DescriptorSkipLength;        /*!< Specifies the number of word to skip between two unchained descriptors (Ring mode)
                                                             This parameter must be a number between Min_Data = 0 and Max_Data = 32 */

  unsigned int             DMAArbitration;              /*!< Selects the DMA Tx/Rx arbitration.
                                                             This parameter can be a value of @ref ETH_DMA_Arbitration */
} ETH_DMAInitTypeDef;

#define ETH_PROTOCOL_ARP 0x0608
#define ETH_PROTOCOL_IPV4 8

#define ETH_HWADDR_LEN 6

typedef struct
{
  unsigned char addr[ETH_HWADDR_LEN];
} ETH_Address;

typedef struct __attribute__((__packed__))
{
  ETH_Address dest;
  ETH_Address src;
  unsigned short type;
} ETH_Header;

#define ETH_HEADER_LENGTH 14

extern ETH_HandleTypeDef ETHInstance;

extern Queue eth_irq_queue;
extern Queue eth_user_queue;

unsigned int ETH_Init(void);
void ETH_InterruptHandler(void);
void ETH_DMARxDescListInit(ETH_DMADescTypeDef *DMARxDescTab, unsigned char *RxBuff, unsigned int RxBuffCount);
void ETH_DMATxDescListInit(ETH_DMADescTypeDef *DMATxDescTab, unsigned char *TxBuff, unsigned int TxBuffCount);
void ETH_Start(void);
unsigned int ETH_GetReceivedFrame_IT(void);
unsigned int ETH_TransmitFrame(unsigned int FrameLength);
void ETH_QueueInit(void);
void ETH_ProcessQueue(void);
unsigned short ETH_Swap(unsigned short v);
void ETH_UserRxCpltCallback(void);

#endif
