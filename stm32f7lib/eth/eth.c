#include <board.h>
#include <eth.h>

#define ETH_TIMEOUT_SWRESET               0xFFFFF
#define ETH_TIMEOUT_LINKED_STATE          0xFFFFFF
#define ETH_TIMEOUT_AUTONEGO_COMPLETED    0xFFFFFF

#define ETH_MACMIIAR_CR_MASK    0xFFFFFFE3U

#define PHY_BCR                         0x00U    /*!< Transceiver Basic Control Register   */
#define PHY_BSR                         0x01U    /*!< Transceiver Basic Status Register    */

#define PHY_RESET                       0x8000U  /*!< PHY Reset */
#define PHY_LOOPBACK                    0x4000U  /*!< Select loop-back mode */
#define PHY_FULLDUPLEX_100M             0x2100U  /*!< Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M             0x2000U  /*!< Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M              0x0100U  /*!< Set the full-duplex mode at 10 Mb/s  */
#define PHY_HALFDUPLEX_10M              0x0000U  /*!< Set the half-duplex mode at 10 Mb/s  */
#define PHY_AUTONEGOTIATION             0x1000U  /*!< Enable auto-negotiation function     */
#define PHY_RESTART_AUTONEGOTIATION     0x0200U  /*!< Restart auto-negotiation function    */
#define PHY_POWERDOWN                   0x0800U  /*!< Select the power down mode           */
#define PHY_ISOLATE                     0x0400U  /*!< Isolate PHY from MII                 */

#define PHY_RESET_DELAY                 0x000000FFU
#define PHY_CONFIG_DELAY                0x00000FFFU

#define PHY_READ_TO                     0x00FFFFFFU
#define PHY_WRITE_TO                    0x00FFFFFFU

#define PHY_AUTONEGO_COMPLETE           0x0020U  /*!< Auto-Negotiation process completed   */
#define PHY_LINKED_STATUS               0x0004U  /*!< Valid link established               */
#define PHY_JABBER_DETECTION            0x0002U  /*!< Jabber condition detected            */

#define PHY_SR                          0x1FU    /*!< PHY status register Offset                      */

#define PHY_SPEED_STATUS                0x0004U  /*!< PHY Speed mask                                  */
#define PHY_DUPLEX_STATUS               0x0010U  /*!< PHY Duplex mask                                 */

#define  ETH_SUCCESS            0
#define  ETH_ERROR              1

#define ETH_WATCHDOG_ENABLE       0x00000000U
#define ETH_WATCHDOG_DISABLE      0x00800000U

#define ETH_JABBER_ENABLE    0x00000000U
#define ETH_JABBER_DISABLE   0x00400000U

#define ETH_INTERFRAMEGAP_96BIT   0x00000000U  /*!< minimum IFG between frames during transmission is 96Bit */
#define ETH_INTERFRAMEGAP_88BIT   0x00020000U  /*!< minimum IFG between frames during transmission is 88Bit */
#define ETH_INTERFRAMEGAP_80BIT   0x00040000U  /*!< minimum IFG between frames during transmission is 80Bit */
#define ETH_INTERFRAMEGAP_72BIT   0x00060000U  /*!< minimum IFG between frames during transmission is 72Bit */
#define ETH_INTERFRAMEGAP_64BIT   0x00080000U  /*!< minimum IFG between frames during transmission is 64Bit */
#define ETH_INTERFRAMEGAP_56BIT   0x000A0000U  /*!< minimum IFG between frames during transmission is 56Bit */
#define ETH_INTERFRAMEGAP_48BIT   0x000C0000U  /*!< minimum IFG between frames during transmission is 48Bit */
#define ETH_INTERFRAMEGAP_40BIT   0x000E0000U  /*!< minimum IFG between frames during transmission is 40Bit */

#define ETH_CARRIERSENCE_ENABLE   0x00000000U
#define ETH_CARRIERSENCE_DISABLE  0x00010000U

#define ETH_RECEIVEOWN_ENABLE     0x00000000U
#define ETH_RECEIVEOWN_DISABLE    0x00002000U

#define ETH_LOOPBACKMODE_ENABLE        0x00001000U
#define ETH_LOOPBACKMODE_DISABLE       0x00000000U

#define ETH_CHECKSUMOFFLAOD_ENABLE     0x00000400U
#define ETH_CHECKSUMOFFLAOD_DISABLE    0x00000000U

#define ETH_RETRYTRANSMISSION_ENABLE   0x00000000U
#define ETH_RETRYTRANSMISSION_DISABLE  0x00000200U

#define ETH_AUTOMATICPADCRCSTRIP_ENABLE     0x00000080U
#define ETH_AUTOMATICPADCRCSTRIP_DISABLE    0x00000000U

#define ETH_BACKOFFLIMIT_10  0x00000000U
#define ETH_BACKOFFLIMIT_8   0x00000020U
#define ETH_BACKOFFLIMIT_4   0x00000040U
#define ETH_BACKOFFLIMIT_1   0x00000060U

#define ETH_DEFFERRALCHECK_ENABLE       0x00000010U
#define ETH_DEFFERRALCHECK_DISABLE      0x00000000U

#define ETH_RECEIVEALL_ENABLE     0x80000000U
#define ETH_RECEIVEAll_DISABLE    0x00000000U

#define ETH_SOURCEADDRFILTER_NORMAL_ENABLE       0x00000200U
#define ETH_SOURCEADDRFILTER_INVERSE_ENABLE      0x00000300U
#define ETH_SOURCEADDRFILTER_DISABLE             0x00000000U

#define ETH_PASSCONTROLFRAMES_BLOCKALL                0x00000040U  /*!< MAC filters all control frames from reaching the application */
#define ETH_PASSCONTROLFRAMES_FORWARDALL              0x00000080U  /*!< MAC forwards all control frames to application even if they fail the Address Filter */
#define ETH_PASSCONTROLFRAMES_FORWARDPASSEDADDRFILTER 0x000000C0U  /*!< MAC forwards control frames that pass the Address Filter. */

#define ETH_BROADCASTFRAMESRECEPTION_ENABLE     0x00000000U
#define ETH_BROADCASTFRAMESRECEPTION_DISABLE    0x00000020U

#define ETH_DESTINATIONADDRFILTER_NORMAL    0x00000000U
#define ETH_DESTINATIONADDRFILTER_INVERSE   0x00000008U

#define ETH_PROMISCUOUS_MODE_ENABLE     0x00000001U
#define ETH_PROMISCUOUS_MODE_DISABLE    0x00000000U

#define ETH_MULTICASTFRAMESFILTER_PERFECTHASHTABLE    0x00000404U
#define ETH_MULTICASTFRAMESFILTER_HASHTABLE           0x00000004U
#define ETH_MULTICASTFRAMESFILTER_PERFECT             0x00000000U
#define ETH_MULTICASTFRAMESFILTER_NONE                0x00000010U

#define ETH_UNICASTFRAMESFILTER_PERFECTHASHTABLE 0x00000402U
#define ETH_UNICASTFRAMESFILTER_HASHTABLE        0x00000002U
#define ETH_UNICASTFRAMESFILTER_PERFECT          0x00000000U

#define ETH_ZEROQUANTAPAUSE_ENABLE     0x00000000U
#define ETH_ZEROQUANTAPAUSE_DISABLE    0x00000080U

#define ETH_PAUSELOWTHRESHOLD_MINUS4        0x00000000U  /*!< Pause time minus 4 slot times */
#define ETH_PAUSELOWTHRESHOLD_MINUS28       0x00000010U  /*!< Pause time minus 28 slot times */
#define ETH_PAUSELOWTHRESHOLD_MINUS144      0x00000020U  /*!< Pause time minus 144 slot times */
#define ETH_PAUSELOWTHRESHOLD_MINUS256      0x00000030U  /*!< Pause time minus 256 slot times */

#define ETH_UNICASTPAUSEFRAMEDETECT_ENABLE  0x00000008U
#define ETH_UNICASTPAUSEFRAMEDETECT_DISABLE 0x00000000U

#define ETH_RECEIVEFLOWCONTROL_ENABLE       0x00000004U
#define ETH_RECEIVEFLOWCONTROL_DISABLE      0x00000000U

#define ETH_TRANSMITFLOWCONTROL_ENABLE      0x00000002U
#define ETH_TRANSMITFLOWCONTROL_DISABLE     0x00000000U

#define ETH_VLANTAGCOMPARISON_12BIT    0x00010000U
#define ETH_VLANTAGCOMPARISON_16BIT    0x00000000U

#define ETH_MAC_ADDRESS0     0x00000000U
#define ETH_MAC_ADDRESS1     0x00000008U
#define ETH_MAC_ADDRESS2     0x00000010U
#define ETH_MAC_ADDRESS3     0x00000018U

#define ETH_MACCR_CLEAR_MASK    0xFF20810FU
#define ETH_MACFCR_CLEAR_MASK   0x0000FF41U
#define ETH_DMAOMR_CLEAR_MASK   0xF8DE3F23U

#define ETH_REG_WRITE_DELAY 0x00000001U

#define ETH_DROPTCPIPCHECKSUMERRORFRAME_ENABLE   0x00000000U
#define ETH_DROPTCPIPCHECKSUMERRORFRAME_DISABLE  0x04000000U

#define ETH_DMA_IT_TST       ((uint32_t)0x20000000U)  /*!< Time-stamp trigger interrupt (on DMA) */
#define ETH_DMA_IT_PMT       ((uint32_t)0x10000000U)  /*!< PMT interrupt (on DMA) */
#define ETH_DMA_IT_MMC       ((uint32_t)0x08000000U)  /*!< MMC interrupt (on DMA) */
#define ETH_DMA_IT_NIS       ((uint32_t)0x00010000U)  /*!< Normal interrupt summary */
#define ETH_DMA_IT_AIS       ((uint32_t)0x00008000U)  /*!< Abnormal interrupt summary */
#define ETH_DMA_IT_ER        ((uint32_t)0x00004000U)  /*!< Early receive interrupt */
#define ETH_DMA_IT_FBE       ((uint32_t)0x00002000U)  /*!< Fatal bus error interrupt */
#define ETH_DMA_IT_ET        ((uint32_t)0x00000400U)  /*!< Early transmit interrupt */
#define ETH_DMA_IT_RWT       ((uint32_t)0x00000200U)  /*!< Receive watchdog timeout interrupt */
#define ETH_DMA_IT_RPS       ((uint32_t)0x00000100U)  /*!< Receive process stopped interrupt */
#define ETH_DMA_IT_RBU       ((uint32_t)0x00000080U)  /*!< Receive buffer unavailable interrupt */
#define ETH_DMA_IT_R         ((uint32_t)0x00000040U)  /*!< Receive interrupt */
#define ETH_DMA_IT_TU        ((uint32_t)0x00000020U)  /*!< Underflow interrupt */
#define ETH_DMA_IT_RO        ((uint32_t)0x00000010U)  /*!< Overflow interrupt */
#define ETH_DMA_IT_TJT       ((uint32_t)0x00000008U)  /*!< Transmit jabber timeout interrupt */
#define ETH_DMA_IT_TBU       ((uint32_t)0x00000004U)  /*!< Transmit buffer unavailable interrupt */
#define ETH_DMA_IT_TPS       ((uint32_t)0x00000002U)  /*!< Transmit process stopped interrupt */
#define ETH_DMA_IT_T         ((uint32_t)0x00000001U)  /*!< Transmit interrupt */

#define ETH_RECEIVESTOREFORWARD_ENABLE      0x02000000U
#define ETH_RECEIVESTOREFORWARD_DISABLE     0x00000000U

#define ETH_FLUSHRECEIVEDFRAME_ENABLE       0x00000000U
#define ETH_FLUSHRECEIVEDFRAME_DISABLE      0x01000000U

#define ETH_TRANSMITSTOREFORWARD_ENABLE     0x00200000U
#define ETH_TRANSMITSTOREFORWARD_DISABLE    0x00000000U

#define ETH_TRANSMITTHRESHOLDCONTROL_64BYTES     0x00000000U  /*!< threshold level of the MTL Transmit FIFO is 64 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_128BYTES    0x00004000U  /*!< threshold level of the MTL Transmit FIFO is 128 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_192BYTES    0x00008000U  /*!< threshold level of the MTL Transmit FIFO is 192 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_256BYTES    0x0000C000U  /*!< threshold level of the MTL Transmit FIFO is 256 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_40BYTES     0x00010000U  /*!< threshold level of the MTL Transmit FIFO is 40 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_32BYTES     0x00014000U  /*!< threshold level of the MTL Transmit FIFO is 32 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_24BYTES     0x00018000U  /*!< threshold level of the MTL Transmit FIFO is 24 Bytes */
#define ETH_TRANSMITTHRESHOLDCONTROL_16BYTES     0x0001C000U  /*!< threshold level of the MTL Transmit FIFO is 16 Bytes */

#define ETH_FORWARDERRORFRAMES_ENABLE       0x00000080U
#define ETH_FORWARDERRORFRAMES_DISABLE      0x00000000U

#define ETH_FORWARDUNDERSIZEDGOODFRAMES_ENABLE   0x00000040U
#define ETH_FORWARDUNDERSIZEDGOODFRAMES_DISABLE  0x00000000U

#define ETH_RECEIVEDTHRESHOLDCONTROL_64BYTES      0x00000000U /*!< threshold level of the MTL Receive FIFO is 64 Bytes */
#define ETH_RECEIVEDTHRESHOLDCONTROL_32BYTES      0x00000008U  /*!< threshold level of the MTL Receive FIFO is 32 Bytes */
#define ETH_RECEIVEDTHRESHOLDCONTROL_96BYTES      0x00000010U  /*!< threshold level of the MTL Receive FIFO is 96 Bytes */
#define ETH_RECEIVEDTHRESHOLDCONTROL_128BYTES     0x00000018U  /*!< threshold level of the MTL Receive FIFO is 128 Bytes */

#define ETH_SECONDFRAMEOPERARTE_ENABLE       0x00000004U
#define ETH_SECONDFRAMEOPERARTE_DISABLE      0x00000000U

#define ETH_ADDRESSALIGNEDBEATS_ENABLE      0x02000000U
#define ETH_ADDRESSALIGNEDBEATS_DISABLE     0x00000000U

#define ETH_FIXEDBURST_ENABLE     0x00010000U
#define ETH_FIXEDBURST_DISABLE    0x00000000U

#define ETH_RXDMABURSTLENGTH_1BEAT          0x00020000U  /*!< maximum number of beats to be transferred in one RxDMA transaction is 1 */
#define ETH_RXDMABURSTLENGTH_2BEAT          0x00040000U  /*!< maximum number of beats to be transferred in one RxDMA transaction is 2 */
#define ETH_RXDMABURSTLENGTH_4BEAT          0x00080000U  /*!< maximum number of beats to be transferred in one RxDMA transaction is 4 */
#define ETH_RXDMABURSTLENGTH_8BEAT          0x00100000U  /*!< maximum number of beats to be transferred in one RxDMA transaction is 8 */
#define ETH_RXDMABURSTLENGTH_16BEAT         0x00200000U  /*!< maximum number of beats to be transferred in one RxDMA transaction is 16 */
#define ETH_RXDMABURSTLENGTH_32BEAT         0x00400000U  /*!< maximum number of beats to be transferred in one RxDMA transaction is 32 */
#define ETH_RXDMABURSTLENGTH_4XPBL_4BEAT    0x01020000U  /*!< maximum number of beats to be transferred in one RxDMA transaction is 4 */
#define ETH_RXDMABURSTLENGTH_4XPBL_8BEAT    0x01040000U  /*!< maximum number of beats to be transferred in one RxDMA transaction is 8 */
#define ETH_RXDMABURSTLENGTH_4XPBL_16BEAT   0x01080000U  /*!< maximum number of beats to be transferred in one RxDMA transaction is 16 */
#define ETH_RXDMABURSTLENGTH_4XPBL_32BEAT   0x01100000U  /*!< maximum number of beats to be transferred in one RxDMA transaction is 32 */
#define ETH_RXDMABURSTLENGTH_4XPBL_64BEAT   0x01200000U  /*!< maximum number of beats to be transferred in one RxDMA transaction is 64 */
#define ETH_RXDMABURSTLENGTH_4XPBL_128BEAT  0x01400000U  /*!< maximum number of beats to be transferred in one RxDMA transaction is 128 */

#define ETH_TXDMABURSTLENGTH_1BEAT          0x00000100U  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 1 */
#define ETH_TXDMABURSTLENGTH_2BEAT          0x00000200U  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 2 */
#define ETH_TXDMABURSTLENGTH_4BEAT          0x00000400U  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
#define ETH_TXDMABURSTLENGTH_8BEAT          0x00000800U  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
#define ETH_TXDMABURSTLENGTH_16BEAT         0x00001000U  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
#define ETH_TXDMABURSTLENGTH_32BEAT         0x00002000U  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
#define ETH_TXDMABURSTLENGTH_4XPBL_4BEAT    0x01000100U  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
#define ETH_TXDMABURSTLENGTH_4XPBL_8BEAT    0x01000200U  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
#define ETH_TXDMABURSTLENGTH_4XPBL_16BEAT   0x01000400U  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
#define ETH_TXDMABURSTLENGTH_4XPBL_32BEAT   0x01000800U  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
#define ETH_TXDMABURSTLENGTH_4XPBL_64BEAT   0x01001000U  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 64 */
#define ETH_TXDMABURSTLENGTH_4XPBL_128BEAT  0x01002000U  /*!< maximum number of beats to be transferred in one TxDMA (or both) transaction is 128 */

#define ETH_DMAENHANCEDDESCRIPTOR_ENABLE              0x00000080U
#define ETH_DMAENHANCEDDESCRIPTOR_DISABLE             0x00000000U

#define ETH_DMAARBITRATION_ROUNDROBIN_RXTX_1_1   0x00000000U
#define ETH_DMAARBITRATION_ROUNDROBIN_RXTX_2_1   0x00004000U
#define ETH_DMAARBITRATION_ROUNDROBIN_RXTX_3_1   0x00008000U
#define ETH_DMAARBITRATION_ROUNDROBIN_RXTX_4_1   0x0000C000U
#define ETH_DMAARBITRATION_RXPRIORTX             0x00000002U

#define ETH_RXPOLLING_MODE      0x00000000U
#define ETH_RXINTERRUPT_MODE    0x00000001U

#define ETH_DMA_FLAG_TST               0x20000000U  /*!< Time-stamp trigger interrupt (on DMA) */
#define ETH_DMA_FLAG_PMT               0x10000000U  /*!< PMT interrupt (on DMA) */
#define ETH_DMA_FLAG_MMC               0x08000000U  /*!< MMC interrupt (on DMA) */
#define ETH_DMA_FLAG_DATATRANSFERERROR 0x00800000U  /*!< Error bits 0-Rx DMA, 1-Tx DMA */
#define ETH_DMA_FLAG_READWRITEERROR    0x01000000U  /*!< Error bits 0-write transfer, 1-read transfer */
#define ETH_DMA_FLAG_ACCESSERROR       0x02000000U  /*!< Error bits 0-data buffer, 1-desc. access */
#define ETH_DMA_FLAG_NIS               0x00010000U  /*!< Normal interrupt summary flag */
#define ETH_DMA_FLAG_AIS               0x00008000U  /*!< Abnormal interrupt summary flag */
#define ETH_DMA_FLAG_ER                0x00004000U  /*!< Early receive flag */
#define ETH_DMA_FLAG_FBE               0x00002000U  /*!< Fatal bus error flag */
#define ETH_DMA_FLAG_ET                0x00000400U  /*!< Early transmit flag */
#define ETH_DMA_FLAG_RWT               0x00000200U  /*!< Receive watchdog timeout flag */
#define ETH_DMA_FLAG_RPS               0x00000100U  /*!< Receive process stopped flag */
#define ETH_DMA_FLAG_RBU               0x00000080U  /*!< Receive buffer unavailable flag */
#define ETH_DMA_FLAG_R                 0x00000040U  /*!< Receive flag */
#define ETH_DMA_FLAG_TU                0x00000020U  /*!< Underflow flag */
#define ETH_DMA_FLAG_RO                0x00000010U  /*!< Overflow flag */
#define ETH_DMA_FLAG_TJT               0x00000008U  /*!< Transmit jabber timeout flag */
#define ETH_DMA_FLAG_TBU               0x00000004U  /*!< Transmit buffer unavailable flag */
#define ETH_DMA_FLAG_TPS               0x00000002U  /*!< Transmit process stopped flag */
#define ETH_DMA_FLAG_T                 0x00000001U  /*!< Transmit flag */

#define ETH_DMATXDESC_OWN                     0x80000000U  /*!< OWN bit: descriptor is owned by DMA engine */
#define ETH_DMATXDESC_IC                      0x40000000U  /*!< Interrupt on Completion */
#define ETH_DMATXDESC_LS                      0x20000000U  /*!< Last Segment */
#define ETH_DMATXDESC_FS                      0x10000000U  /*!< First Segment */
#define ETH_DMATXDESC_DC                      0x08000000U  /*!< Disable CRC */
#define ETH_DMATXDESC_DP                      0x04000000U  /*!< Disable Padding */
#define ETH_DMATXDESC_TTSE                    0x02000000U  /*!< Transmit Time Stamp Enable */
#define ETH_DMATXDESC_CIC                     0x00C00000U  /*!< Checksum Insertion Control: 4 cases */
#define ETH_DMATXDESC_CIC_BYPASS              0x00000000U  /*!< Do Nothing: Checksum Engine is bypassed */
#define ETH_DMATXDESC_CIC_IPV4HEADER          0x00400000U  /*!< IPV4 header Checksum Insertion */
#define ETH_DMATXDESC_CIC_TCPUDPICMP_SEGMENT  0x00800000U  /*!< TCP/UDP/ICMP Checksum Insertion calculated over segment only */
#define ETH_DMATXDESC_CIC_TCPUDPICMP_FULL     0x00C00000U  /*!< TCP/UDP/ICMP Checksum Insertion fully calculated */
#define ETH_DMATXDESC_TER                     0x00200000U  /*!< Transmit End of Ring */
#define ETH_DMATXDESC_TCH                     0x00100000U  /*!< Second Address Chained */
#define ETH_DMATXDESC_TTSS                    0x00020000U  /*!< Tx Time Stamp Status */
#define ETH_DMATXDESC_IHE                     0x00010000U  /*!< IP Header Error */
#define ETH_DMATXDESC_ES                      0x00008000U  /*!< Error summary: OR of the following bits: UE || ED || EC || LCO || NC || LCA || FF || JT */
#define ETH_DMATXDESC_JT                      0x00004000U  /*!< Jabber Timeout */
#define ETH_DMATXDESC_FF                      0x00002000U  /*!< Frame Flushed: DMA/MTL flushed the frame due to SW flush */
#define ETH_DMATXDESC_PCE                     0x00001000U  /*!< Payload Checksum Error */
#define ETH_DMATXDESC_LCA                     0x00000800U  /*!< Loss of Carrier: carrier lost during transmission */
#define ETH_DMATXDESC_NC                      0x00000400U  /*!< No Carrier: no carrier signal from the transceiver */
#define ETH_DMATXDESC_LCO                     0x00000200U  /*!< Late Collision: transmission aborted due to collision */
#define ETH_DMATXDESC_EC                      0x00000100U  /*!< Excessive Collision: transmission aborted after 16 collisions */
#define ETH_DMATXDESC_VF                      0x00000080U  /*!< VLAN Frame */
#define ETH_DMATXDESC_CC                      0x00000078U  /*!< Collision Count */
#define ETH_DMATXDESC_ED                      0x00000004U  /*!< Excessive Deferral */
#define ETH_DMATXDESC_UF                      0x00000002U  /*!< Underflow Error: late data arrival from the memory */
#define ETH_DMATXDESC_DB                      0x00000001U  /*!< Deferred Bit */

#define ETH_DMATXDESC_CHECKSUMBYPASS             0x00000000U   /*!< Checksum engine bypass */
#define ETH_DMATXDESC_CHECKSUMIPV4HEADER         0x00400000U   /*!< IPv4 header checksum insertion  */
#define ETH_DMATXDESC_CHECKSUMTCPUDPICMPSEGMENT  0x00800000U   /*!< TCP/UDP/ICMP checksum insertion. Pseudo header checksum is assumed to be present */
#define ETH_DMATXDESC_CHECKSUMTCPUDPICMPFULL     0x00C00000U   /*!< TCP/UDP/ICMP checksum fully in hardware including pseudo header */

#define ETH_DMARXDESC_DIC   0x80000000U  /*!< Disable Interrupt on Completion */
#define ETH_DMARXDESC_RBS2  0x1FFF0000U  /*!< Receive Buffer2 Size */
#define ETH_DMARXDESC_RER   0x00008000U  /*!< Receive End of Ring */
#define ETH_DMARXDESC_RCH   0x00004000U  /*!< Second Address Chained */
#define ETH_DMARXDESC_RBS1  0x00001FFFU  /*!< Receive Buffer1 Size */

/* Ethernet DMA Rx descriptors Frame length Shift */
#define  ETH_DMARXDESC_FRAMELENGTHSHIFT            16

#define ETH_DMATXDESC_TBS2  0x1FFF0000U  /*!< Transmit Buffer2 Size */
#define ETH_DMATXDESC_TBS1  0x00001FFFU  /*!< Transmit Buffer1 Size */

#define ETH_MAC_ADDR_HBASE    (unsigned int)(ETH_MAC_BASE + 0x40U)  /* Ethernet MAC address high offset */
#define ETH_MAC_ADDR_LBASE    (unsigned int)(ETH_MAC_BASE + 0x44U)  /* Ethernet MAC address low offset */

#define __HAL_ETH_DMA_ENABLE_IT(__INTERRUPT__)                 (ETHInstance.Instance->DMAIER |= (__INTERRUPT__))
#define __HAL_ETH_DMA_GET_FLAG(__FLAG__)                   ((ETHInstance.Instance->DMASR &( __FLAG__)) == ( __FLAG__))
#define __HAL_ETH_DMA_CLEAR_IT(__INTERRUPT__)      (ETHInstance.Instance->DMASR =(__INTERRUPT__))

static unsigned int ETH_WritePHYRegister(unsigned int PHYReg, unsigned int RegValue);
static void ETH_MACDMAConfig(unsigned int err);
static unsigned int ETH_ReadPHYRegister(unsigned int PHYReg, unsigned int *RegValue);
static void ETH_MACAddressConfig(unsigned int MacAddr, unsigned char *Addr);

ETH_HandleTypeDef ETHInstance;

unsigned int ETH_Init(void)
{
  unsigned int tempreg, phyreg;
  unsigned int counter;

  /* Select MII or RMII Mode*/
  SYSCFG->PMC &= ~(SYSCFG_PMC_MII_RMII_SEL);
  SYSCFG->PMC |= (uint32_t)ETHInstance.Init.MediaInterface;
  
  /* Ethernet Software reset */
  /* Set the SWR bit: resets all MAC subsystem internal registers and logic */
  /* After reset all the registers holds their respective reset values */
  (ETHInstance.Instance)->DMABMR |= ETH_DMABMR_SR;

  counter = 0;
  /* Wait for software reset */
  while ((ETHInstance.Instance)->DMABMR & ETH_DMABMR_SR)
  {
    if (counter++ > ETH_TIMEOUT_SWRESET)
      return 1;
  }
  
  /*-------------------------------- MAC Initialization ----------------------*/
  /* Get the ETHERNET MACMIIAR value */
  tempreg = (ETHInstance.Instance)->MACMIIAR;
  /* Clear CSR Clock Range CR[2:0] bits */
  tempreg &= ETH_MACMIIAR_CR_MASK;
  tempreg |= ETH_MACMIIAR_CR_Div;

  /* Write to ETHERNET MAC MIIAR: Configure the ETHERNET CSR Clock Range */
  (ETHInstance.Instance)->MACMIIAR = tempreg;
  
  /*-------------------- PHY initialization and configuration ----------------*/
  /* Put the PHY in reset mode */
  if(ETH_WritePHYRegister(PHY_BCR, PHY_RESET))
  {
    /* Config MAC and DMA */
    ETH_MACDMAConfig(ETH_ERROR);
    
    /* Set the ETH peripheral state to READY */
    ETHInstance.State = ETH_STATE_READY;
    
    /* Return HAL_ERROR */
    return 2;
  }
  
  /* Delay to assure PHY reset */
  delayms(PHY_RESET_DELAY);
  
  if((ETHInstance.Init).AutoNegotiation != ETH_AUTONEGOTIATION_DISABLE)
  {
    counter = 0;
    /* We wait for linked status */
    do
    {
      ETH_ReadPHYRegister(PHY_BSR, &phyreg);
      
      /* Check for the Timeout */
      if(counter++ > ETH_TIMEOUT_LINKED_STATE)
      {
        /* Config MAC and DMA */
        ETH_MACDMAConfig(ETH_ERROR);

        ETHInstance.State = ETH_STATE_READY;
  
        return 3;
      }
    } while (((phyreg & PHY_LINKED_STATUS) != PHY_LINKED_STATUS));

    
    /* Enable Auto-Negotiation */
    if(ETH_WritePHYRegister(PHY_BCR, PHY_AUTONEGOTIATION))
    {
      /* Config MAC and DMA */
      ETH_MACDMAConfig(ETH_ERROR);
      
      /* Set the ETH peripheral state to READY */
      ETHInstance.State = ETH_STATE_READY;
      
      /* Return HAL_ERROR */
      return 4;
    }
    
    counter = 0;

    /* Wait until the auto-negotiation will be completed */
    do
    {
      ETH_ReadPHYRegister(PHY_BSR, &phyreg);
      
      /* Check for the Timeout */
      if(counter++ > ETH_TIMEOUT_AUTONEGO_COMPLETED)
      {
        /* Config MAC and DMA */
        ETH_MACDMAConfig(ETH_ERROR);

        ETHInstance.State= ETH_STATE_READY;
  
        return 5;
      }
      
    } while (((phyreg & PHY_AUTONEGO_COMPLETE) != PHY_AUTONEGO_COMPLETE));
    
    /* Read the result of the auto-negotiation */
    if(ETH_ReadPHYRegister(PHY_SR, &phyreg))
    {
      /* Config MAC and DMA */
      ETH_MACDMAConfig(ETH_ERROR);
      
      /* Set the ETH peripheral state to READY */
      ETHInstance.State = ETH_STATE_READY;
    }
    
    /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
    if (phyreg & PHY_DUPLEX_STATUS)
    {
      /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
      (ETHInstance.Init).DuplexMode = ETH_MODE_FULLDUPLEX;
    }
    else
    {
      /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
      (ETHInstance.Init).DuplexMode = ETH_MODE_HALFDUPLEX;
    }
    /* Configure the MAC with the speed fixed by the auto-negotiation process */
    if((phyreg & PHY_SPEED_STATUS) == PHY_SPEED_STATUS)
    {  
      /* Set Ethernet speed to 10M following the auto-negotiation */
      (ETHInstance.Init).Speed = ETH_SPEED_10M;
    }
    else
    {   
      /* Set Ethernet speed to 100M following the auto-negotiation */ 
      (ETHInstance.Init).Speed = ETH_SPEED_100M;
    }
  }
  else /* AutoNegotiation Disable */
  {
    /* Set MAC Speed and Duplex Mode */
    if(ETH_WritePHYRegister(PHY_BCR, ((ETHInstance.Init).DuplexMode >> 3) | ((ETHInstance.Init).Speed >> 1)))
    {
      /* Config MAC and DMA */
      ETH_MACDMAConfig(ETH_ERROR);
      
      /* Set the ETH peripheral state to READY */
      ETHInstance.State = ETH_STATE_READY;
      
      /* Return HAL_ERROR */
      return 6;
    }  
    
    /* Delay to assure PHY configuration */
    delayms(PHY_CONFIG_DELAY);
  }
  
  /* Config MAC and DMA */
  ETH_MACDMAConfig(ETH_SUCCESS);
  
  /* Set ETH HAL State to Ready */
  ETHInstance.State = ETH_STATE_READY;
  
  /* Return function status */
  return 0;
}

static unsigned int ETH_ReadPHYRegister(unsigned int PHYReg, unsigned int *RegValue)
{
  unsigned int tmpreg;
  unsigned int counter;

  /* Check the ETH peripheral state */
  if(ETHInstance.State == ETH_STATE_BUSY_RD)
  {
    return 1;
  }
  /* Set ETH HAL State to BUSY_RD */
  ETHInstance.State = ETH_STATE_BUSY_RD;

  /* Get the ETHERNET MACMIIAR value */
  tmpreg = ETHInstance.Instance->MACMIIAR;

  /* Keep only the CSR Clock Range CR[2:0] bits value */
  tmpreg &= ~ETH_MACMIIAR_CR_MASK;

  /* Prepare the MII address register value */
  tmpreg |=((ETHInstance.Init.PhyAddress << 11) & ETH_MACMIIAR_PA); /* Set the PHY device address   */
  tmpreg |=((PHYReg<<6) & ETH_MACMIIAR_MR);                   /* Set the PHY register address */
  tmpreg &= ~ETH_MACMIIAR_MW;                                           /* Set the read mode            */
  tmpreg |= ETH_MACMIIAR_MB;                                            /* Set the MII Busy bit         */

  /* Write the result value into the MII Address register */
  ETHInstance.Instance->MACMIIAR = tmpreg;

  counter = 0;

  /* Check for the Busy flag */
  while((tmpreg & ETH_MACMIIAR_MB) == ETH_MACMIIAR_MB)
  {
    /* Check for the Timeout */
    if(counter++ > PHY_READ_TO)
    {
      ETHInstance.State= ETH_STATE_READY;

      return 1;
    }

    tmpreg = ETHInstance.Instance->MACMIIAR;
  }

  /* Get MACMIIDR value */
  *RegValue = ETHInstance.Instance->MACMIIDR & 0xFFFF;

  /* Set ETH HAL State to READY */
  ETHInstance.State = ETH_STATE_READY;

  /* Return function status */
  return 0;
}

static unsigned int ETH_WritePHYRegister(unsigned int PHYReg, unsigned int RegValue)
{
  unsigned int tmpreg;
  unsigned int counter;

  /* Check the ETH peripheral state */
  if(ETHInstance.State == ETH_STATE_BUSY_WR)
  {
    return 1;
  }
  /* Set ETH HAL State to BUSY_WR */
  ETHInstance.State = ETH_STATE_BUSY_WR;

  /* Get the ETHERNET MACMIIAR value */
  tmpreg = ETHInstance.Instance->MACMIIAR;

  /* Keep only the CSR Clock Range CR[2:0] bits value */
  tmpreg &= ~ETH_MACMIIAR_CR_MASK;

  /* Prepare the MII register address value */
  tmpreg |=(((uint32_t)ETHInstance.Init.PhyAddress<<11) & ETH_MACMIIAR_PA); /* Set the PHY device address */
  tmpreg |=(((uint32_t)PHYReg<<6) & ETH_MACMIIAR_MR);                 /* Set the PHY register address */
  tmpreg |= ETH_MACMIIAR_MW;                                          /* Set the write mode */
  tmpreg |= ETH_MACMIIAR_MB;                                          /* Set the MII Busy bit */

  /* Give the value to the MII data register */
  ETHInstance.Instance->MACMIIDR = (uint16_t)RegValue;

  /* Write the result value into the MII Address register */
  ETHInstance.Instance->MACMIIAR = tmpreg;

  counter = 0;

  /* Check for the Busy flag */
  while((tmpreg & ETH_MACMIIAR_MB) == ETH_MACMIIAR_MB)
  {
    /* Check for the Timeout */
    if(counter++ > PHY_WRITE_TO)
    {
      ETHInstance.State= ETH_STATE_READY;

      return 1;
    }

    tmpreg = ETHInstance.Instance->MACMIIAR;
  }

  /* Set ETH HAL State to READY */
  ETHInstance.State = ETH_STATE_READY;

  /* Return function status */
  return 0;
}

static void ETH_MACDMAConfig(unsigned int err)
{
  ETH_MACInitTypeDef macinit;
  ETH_DMAInitTypeDef dmainit;
  unsigned int tmpreg;

  if (err != ETH_SUCCESS) /* Auto-negotiation failed */
  {
    /* Set Ethernet duplex mode to Full-duplex */
    (ETHInstance.Init).DuplexMode = ETH_MODE_FULLDUPLEX;

    /* Set Ethernet speed to 100M */
    (ETHInstance.Init).Speed = ETH_SPEED_100M;
  }

  /* Ethernet MAC default initialization **************************************/
  macinit.Watchdog = ETH_WATCHDOG_ENABLE;
  macinit.Jabber = ETH_JABBER_ENABLE;
  macinit.InterFrameGap = ETH_INTERFRAMEGAP_96BIT;
  macinit.CarrierSense = ETH_CARRIERSENCE_ENABLE;
  macinit.ReceiveOwn = ETH_RECEIVEOWN_ENABLE;
  macinit.LoopbackMode = ETH_LOOPBACKMODE_DISABLE;
  if(ETHInstance.Init.ChecksumMode == ETH_CHECKSUM_BY_HARDWARE)
  {
    macinit.ChecksumOffload = ETH_CHECKSUMOFFLAOD_ENABLE;
  }
  else
  {
    macinit.ChecksumOffload = ETH_CHECKSUMOFFLAOD_DISABLE;
  }
  macinit.RetryTransmission = ETH_RETRYTRANSMISSION_DISABLE;
  macinit.AutomaticPadCRCStrip = ETH_AUTOMATICPADCRCSTRIP_DISABLE;
  macinit.BackOffLimit = ETH_BACKOFFLIMIT_10;
  macinit.DeferralCheck = ETH_DEFFERRALCHECK_DISABLE;
  macinit.ReceiveAll = ETH_RECEIVEAll_DISABLE;
  macinit.SourceAddrFilter = ETH_SOURCEADDRFILTER_DISABLE;
  macinit.PassControlFrames = ETH_PASSCONTROLFRAMES_BLOCKALL;
  macinit.BroadcastFramesReception = ETH_BROADCASTFRAMESRECEPTION_ENABLE;
  macinit.DestinationAddrFilter = ETH_DESTINATIONADDRFILTER_NORMAL;
  macinit.PromiscuousMode = ETH_PROMISCUOUS_MODE_DISABLE;
  macinit.MulticastFramesFilter = ETH_MULTICASTFRAMESFILTER_PERFECT;
  macinit.UnicastFramesFilter = ETH_UNICASTFRAMESFILTER_PERFECT;
  macinit.HashTableHigh = 0x0;
  macinit.HashTableLow = 0x0;
  macinit.PauseTime = 0x0;
  macinit.ZeroQuantaPause = ETH_ZEROQUANTAPAUSE_DISABLE;
  macinit.PauseLowThreshold = ETH_PAUSELOWTHRESHOLD_MINUS4;
  macinit.UnicastPauseFrameDetect = ETH_UNICASTPAUSEFRAMEDETECT_DISABLE;
  macinit.ReceiveFlowControl = ETH_RECEIVEFLOWCONTROL_DISABLE;
  macinit.TransmitFlowControl = ETH_TRANSMITFLOWCONTROL_DISABLE;
  macinit.VLANTagComparison = ETH_VLANTAGCOMPARISON_16BIT;
  macinit.VLANTagIdentifier = 0x0;

  /*------------------------ ETHERNET MACCR Configuration --------------------*/
  /* Get the ETHERNET MACCR value */
  tmpreg = (ETHInstance.Instance)->MACCR;
  /* Clear WD, PCE, PS, TE and RE bits */
  tmpreg &= ETH_MACCR_CLEAR_MASK;
  /* Set the WD bit according to ETH Watchdog value */
  /* Set the JD: bit according to ETH Jabber value */
  /* Set the IFG bit according to ETH InterFrameGap value */
  /* Set the DCRS bit according to ETH CarrierSense value */
  /* Set the FES bit according to ETH Speed value */
  /* Set the DO bit according to ETH ReceiveOwn value */
  /* Set the LM bit according to ETH LoopbackMode value */
  /* Set the DM bit according to ETH Mode value */
  /* Set the IPCO bit according to ETH ChecksumOffload value */
  /* Set the DR bit according to ETH RetryTransmission value */
  /* Set the ACS bit according to ETH AutomaticPadCRCStrip value */
  /* Set the BL bit according to ETH BackOffLimit value */
  /* Set the DC bit according to ETH DeferralCheck value */
  tmpreg |= (unsigned int)(macinit.Watchdog |
                       macinit.Jabber |
                       macinit.InterFrameGap |
                       macinit.CarrierSense |
                       (ETHInstance.Init).Speed |
                       macinit.ReceiveOwn |
                       macinit.LoopbackMode |
                       (ETHInstance.Init).DuplexMode |
                       macinit.ChecksumOffload |
                       macinit.RetryTransmission |
                       macinit.AutomaticPadCRCStrip |
                       macinit.BackOffLimit |
                       macinit.DeferralCheck);

  /* Write to ETHERNET MACCR */
  (ETHInstance.Instance)->MACCR = tmpreg;

  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (ETHInstance.Instance)->MACCR;
  delayms(ETH_REG_WRITE_DELAY);
  (ETHInstance.Instance)->MACCR = tmpreg;

  /*----------------------- ETHERNET MACFFR Configuration --------------------*/
  /* Set the RA bit according to ETH ReceiveAll value */
  /* Set the SAF and SAIF bits according to ETH SourceAddrFilter value */
  /* Set the PCF bit according to ETH PassControlFrames value */
  /* Set the DBF bit according to ETH BroadcastFramesReception value */
  /* Set the DAIF bit according to ETH DestinationAddrFilter value */
  /* Set the PR bit according to ETH PromiscuousMode value */
  /* Set the PM, HMC and HPF bits according to ETH MulticastFramesFilter value */
  /* Set the HUC and HPF bits according to ETH UnicastFramesFilter value */
  /* Write to ETHERNET MACFFR */
  (ETHInstance.Instance)->MACFFR = (unsigned int)(macinit.ReceiveAll |
                                        macinit.SourceAddrFilter |
                                        macinit.PassControlFrames |
                                        macinit.BroadcastFramesReception |
                                        macinit.DestinationAddrFilter |
                                        macinit.PromiscuousMode |
                                        macinit.MulticastFramesFilter |
                                        macinit.UnicastFramesFilter);

  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (ETHInstance.Instance)->MACFFR;
  delayms(ETH_REG_WRITE_DELAY);
  (ETHInstance.Instance)->MACFFR = tmpreg;

  /*--------------- ETHERNET MACHTHR and MACHTLR Configuration --------------*/
  /* Write to ETHERNET MACHTHR */
  (ETHInstance.Instance)->MACHTHR = macinit.HashTableHigh;

  /* Write to ETHERNET MACHTLR */
  (ETHInstance.Instance)->MACHTLR = macinit.HashTableLow;
  /*----------------------- ETHERNET MACFCR Configuration -------------------*/

  /* Get the ETHERNET MACFCR value */
  tmpreg = (ETHInstance.Instance)->MACFCR;
  /* Clear xx bits */
  tmpreg &= ETH_MACFCR_CLEAR_MASK;

  /* Set the PT bit according to ETH PauseTime value */
  /* Set the DZPQ bit according to ETH ZeroQuantaPause value */
  /* Set the PLT bit according to ETH PauseLowThreshold value */
  /* Set the UP bit according to ETH UnicastPauseFrameDetect value */
  /* Set the RFE bit according to ETH ReceiveFlowControl value */
  /* Set the TFE bit according to ETH TransmitFlowControl value */
  tmpreg |= (unsigned int)((macinit.PauseTime << 16) |
                       macinit.ZeroQuantaPause |
                       macinit.PauseLowThreshold |
                       macinit.UnicastPauseFrameDetect |
                       macinit.ReceiveFlowControl |
                       macinit.TransmitFlowControl);

  /* Write to ETHERNET MACFCR */
  (ETHInstance.Instance)->MACFCR = tmpreg;

  /* Wait until the write operation will be taken into account:
  at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (ETHInstance.Instance)->MACFCR;
  delayms(ETH_REG_WRITE_DELAY);
  (ETHInstance.Instance)->MACFCR = tmpreg;

  /*----------------------- ETHERNET MACVLANTR Configuration ----------------*/
  /* Set the ETV bit according to ETH VLANTagComparison value */
  /* Set the VL bit according to ETH VLANTagIdentifier value */
  (ETHInstance.Instance)->MACVLANTR = (unsigned int)(macinit.VLANTagComparison |
                                           macinit.VLANTagIdentifier);

  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (ETHInstance.Instance)->MACVLANTR;
  delayms(ETH_REG_WRITE_DELAY);
  (ETHInstance.Instance)->MACVLANTR = tmpreg;

  /* Ethernet DMA default initialization ************************************/
  dmainit.DropTCPIPChecksumErrorFrame = ETH_DROPTCPIPCHECKSUMERRORFRAME_ENABLE;
  dmainit.ReceiveStoreForward = ETH_RECEIVESTOREFORWARD_ENABLE;
  dmainit.FlushReceivedFrame = ETH_FLUSHRECEIVEDFRAME_ENABLE;
  dmainit.TransmitStoreForward = ETH_TRANSMITSTOREFORWARD_ENABLE;
  dmainit.TransmitThresholdControl = ETH_TRANSMITTHRESHOLDCONTROL_64BYTES;
  dmainit.ForwardErrorFrames = ETH_FORWARDERRORFRAMES_DISABLE;
  dmainit.ForwardUndersizedGoodFrames = ETH_FORWARDUNDERSIZEDGOODFRAMES_DISABLE;
  dmainit.ReceiveThresholdControl = ETH_RECEIVEDTHRESHOLDCONTROL_64BYTES;
  dmainit.SecondFrameOperate = ETH_SECONDFRAMEOPERARTE_ENABLE;
  dmainit.AddressAlignedBeats = ETH_ADDRESSALIGNEDBEATS_ENABLE;
  dmainit.FixedBurst = ETH_FIXEDBURST_ENABLE;
  dmainit.RxDMABurstLength = ETH_RXDMABURSTLENGTH_32BEAT;
  dmainit.TxDMABurstLength = ETH_TXDMABURSTLENGTH_32BEAT;
  dmainit.EnhancedDescriptorFormat = ETH_DMAENHANCEDDESCRIPTOR_ENABLE;
  dmainit.DescriptorSkipLength = 0x0;
  dmainit.DMAArbitration = ETH_DMAARBITRATION_ROUNDROBIN_RXTX_1_1;

  /* Get the ETHERNET DMAOMR value */
  tmpreg = (ETHInstance.Instance)->DMAOMR;
  /* Clear xx bits */
  tmpreg &= ETH_DMAOMR_CLEAR_MASK;

  /* Set the DT bit according to ETH DropTCPIPChecksumErrorFrame value */
  /* Set the RSF bit according to ETH ReceiveStoreForward value */
  /* Set the DFF bit according to ETH FlushReceivedFrame value */
  /* Set the TSF bit according to ETH TransmitStoreForward value */
  /* Set the TTC bit according to ETH TransmitThresholdControl value */
  /* Set the FEF bit according to ETH ForwardErrorFrames value */
  /* Set the FUF bit according to ETH ForwardUndersizedGoodFrames value */
  /* Set the RTC bit according to ETH ReceiveThresholdControl value */
  /* Set the OSF bit according to ETH SecondFrameOperate value */
  tmpreg |= (unsigned int)(dmainit.DropTCPIPChecksumErrorFrame |
                       dmainit.ReceiveStoreForward |
                       dmainit.FlushReceivedFrame |
                       dmainit.TransmitStoreForward |
                       dmainit.TransmitThresholdControl |
                       dmainit.ForwardErrorFrames |
                       dmainit.ForwardUndersizedGoodFrames |
                       dmainit.ReceiveThresholdControl |
                       dmainit.SecondFrameOperate);

  /* Write to ETHERNET DMAOMR */
  (ETHInstance.Instance)->DMAOMR = tmpreg;

  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (ETHInstance.Instance)->DMAOMR;
  delayms(ETH_REG_WRITE_DELAY);
  (ETHInstance.Instance)->DMAOMR = tmpreg;

  /*----------------------- ETHERNET DMABMR Configuration ------------------*/
  /* Set the AAL bit according to ETH AddressAlignedBeats value */
  /* Set the FB bit according to ETH FixedBurst value */
  /* Set the RPBL and 4*PBL bits according to ETH RxDMABurstLength value */
  /* Set the PBL and 4*PBL bits according to ETH TxDMABurstLength value */
  /* Set the Enhanced DMA descriptors bit according to ETH EnhancedDescriptorFormat value*/
  /* Set the DSL bit according to ETH DesciptorSkipLength value */
  /* Set the PR and DA bits according to ETH DMAArbitration value */
  (ETHInstance.Instance)->DMABMR = (uint32_t)(dmainit.AddressAlignedBeats |
                                        dmainit.FixedBurst |
                                        dmainit.RxDMABurstLength |    /* !! if 4xPBL is selected for Tx or Rx it is applied for the other */
                                        dmainit.TxDMABurstLength |
                                        dmainit.EnhancedDescriptorFormat |
                                        (dmainit.DescriptorSkipLength << 2) |
                                        dmainit.DMAArbitration |
                                        ETH_DMABMR_USP); /* Enable use of separate PBL for Rx and Tx */

  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (ETHInstance.Instance)->DMABMR;
  delayms(ETH_REG_WRITE_DELAY);
  (ETHInstance.Instance)->DMABMR = tmpreg;

  if((ETHInstance.Init).RxMode == ETH_RXINTERRUPT_MODE)
  {
    /* Enable the Ethernet Rx Interrupt */
    __HAL_ETH_DMA_ENABLE_IT(ETH_DMA_IT_NIS | ETH_DMA_IT_R);// | ETH_DMA_IT_AIS | 0x23BA
    //__HAL_ETH_DMA_ENABLE_IT(0x1E7FF);//ALL
  }

  /* Initialize MAC address in ethernet MAC */
  ETH_MACAddressConfig(ETH_MAC_ADDRESS0, ETHInstance.Init.MACAddr);
}

static void ETH_MACAddressConfig(unsigned int MacAddr, unsigned char *Addr)
{
  unsigned int tmpreg;

  /* Calculate the selected MAC address high register */
  tmpreg = ((unsigned int)Addr[5] << 8) | (unsigned int)Addr[4];
  /* Load the selected MAC address high register */
  (*(__IO unsigned int *)((unsigned int)(ETH_MAC_ADDR_HBASE + MacAddr))) = tmpreg;
  /* Calculate the selected MAC address low register */
  tmpreg = ((unsigned int)Addr[3] << 24) | ((unsigned int)Addr[2] << 16) | ((unsigned int)Addr[1] << 8) | Addr[0];

  /* Load the selected MAC address low register */
  (*(__IO unsigned int *)((unsigned int)(ETH_MAC_ADDR_LBASE + MacAddr))) = tmpreg;
}

/**
  * @brief  Initializes the DMA Tx descriptors in chain mode.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  DMATxDescTab Pointer to the first Tx desc list
  * @param  TxBuff Pointer to the first TxBuffer list
  * @param  TxBuffCount Number of the used Tx desc in the list
  * @retval HAL status
  */
void ETH_DMATxDescListInit(ETH_DMADescTypeDef *DMATxDescTab, unsigned char *TxBuff, unsigned int TxBuffCount)
{
  unsigned int i;
  ETH_DMADescTypeDef *dmatxdesc;

  /* Set the ETH peripheral state to BUSY */
  ETHInstance.State = ETH_STATE_BUSY;

  /* Set the DMATxDescToSet pointer with the first one of the DMATxDescTab list */
  ETHInstance.TxDesc = DMATxDescTab;

  /* Fill each DMATxDesc descriptor with the right values */
  for(i=0; i < TxBuffCount; i++)
  {
    /* Get the pointer on the ith member of the Tx Desc list */
    dmatxdesc = DMATxDescTab + i;

    /* Set Second Address Chained bit */
    dmatxdesc->Status = ETH_DMATXDESC_TCH;

    /* Set Buffer1 address pointer */
    dmatxdesc->Buffer1Addr = (uint32_t)(&TxBuff[i*ETH_TX_BUF_SIZE]);

    if ((ETHInstance.Init).ChecksumMode == ETH_CHECKSUM_BY_HARDWARE)
    {
      /* Set the DMA Tx descriptors checksum insertion */
      dmatxdesc->Status |= ETH_DMATXDESC_CHECKSUMTCPUDPICMPFULL;
    }

    /* Initialize the next descriptor with the Next Descriptor Polling Enable */
    if(i < (TxBuffCount-1))
    {
      /* Set next descriptor address register with next descriptor base address */
      dmatxdesc->Buffer2NextDescAddr = (uint32_t)(DMATxDescTab+i+1);
    }
    else
    {
      /* For last descriptor, set next descriptor address register equal to the first descriptor base address */
      dmatxdesc->Buffer2NextDescAddr = (uint32_t) DMATxDescTab;
    }
  }

  /* Set Transmit Descriptor List Address Register */
  (ETHInstance.Instance)->DMATDLAR = (uint32_t) DMATxDescTab;

  /* Set ETH HAL State to Ready */
  ETHInstance.State= ETH_STATE_READY;
}

/**
  * @brief  Initializes the DMA Rx descriptors in chain mode.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  DMARxDescTab Pointer to the first Rx desc list
  * @param  RxBuff Pointer to the first RxBuffer list
  * @param  RxBuffCount Number of the used Rx desc in the list
  * @retval HAL status
  */
void ETH_DMARxDescListInit(ETH_DMADescTypeDef *DMARxDescTab, unsigned char *RxBuff, unsigned int RxBuffCount)
{
  unsigned int i;
  ETH_DMADescTypeDef *DMARxDesc;

  /* Set the ETH peripheral state to BUSY */
  ETHInstance.State = ETH_STATE_BUSY;

  /* Set the Ethernet RxDesc pointer with the first one of the DMARxDescTab list */
  ETHInstance.RxDesc = DMARxDescTab;

  /* Fill each DMARxDesc descriptor with the right values */
  for(i=0; i < RxBuffCount; i++)
  {
    /* Get the pointer on the ith member of the Rx Desc list */
    DMARxDesc = DMARxDescTab+i;

    /* Set Own bit of the Rx descriptor Status */
    DMARxDesc->Status = ETH_DMARXDESC_OWN;

    /* Set Buffer1 size and Second Address Chained bit */
    DMARxDesc->ControlBufferSize = ETH_DMARXDESC_RCH | ETH_RX_BUF_SIZE;

    /* Set Buffer1 address pointer */
    DMARxDesc->Buffer1Addr = (uint32_t)(&RxBuff[i*ETH_RX_BUF_SIZE]);

    if((ETHInstance.Init).RxMode == ETH_RXINTERRUPT_MODE)
    {
      /* Enable Ethernet DMA Rx Descriptor interrupt */
      DMARxDesc->ControlBufferSize &= ~ETH_DMARXDESC_DIC;
    }

    /* Initialize the next descriptor with the Next Descriptor Polling Enable */
    if(i < (RxBuffCount-1))
    {
      /* Set next descriptor address register with next descriptor base address */
      DMARxDesc->Buffer2NextDescAddr = (uint32_t)(DMARxDescTab+i+1);
    }
    else
    {
      /* For last descriptor, set next descriptor address register equal to the first descriptor base address */
      DMARxDesc->Buffer2NextDescAddr = (uint32_t)(DMARxDescTab);
    }
  }

  /* Set Receive Descriptor List Address Register */
  (ETHInstance.Instance)->DMARDLAR = (uint32_t) DMARxDescTab;

  /* Set ETH HAL State to Ready */
  ETHInstance.State= ETH_STATE_READY;
}

static void ETH_MACTransmissionEnable(void)
{
  __IO unsigned int tmpreg;

  /* Enable the MAC transmission */
  (ETHInstance.Instance)->MACCR |= ETH_MACCR_TE;

  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (ETHInstance.Instance)->MACCR;
  delayms(ETH_REG_WRITE_DELAY);
  (ETHInstance.Instance)->MACCR = tmpreg;
}

/**
  * @brief  Enables the MAC reception.
  * @retval None
  */
static void ETH_MACReceptionEnable(void)
{
  __IO uint32_t tmpreg;

  /* Enable the MAC reception */
  (ETHInstance.Instance)->MACCR |= ETH_MACCR_RE;

  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (ETHInstance.Instance)->MACCR;
  delayms(ETH_REG_WRITE_DELAY);
  (ETHInstance.Instance)->MACCR = tmpreg;
}

/**
  * @brief  Enables the DMA transmission.
  * @retval None
  */
static void ETH_DMATransmissionEnable(void)
{
  /* Enable the DMA transmission */
  (ETHInstance.Instance)->DMAOMR |= ETH_DMAOMR_ST;
}

/**
  * @brief  Enables the DMA reception.
  * @retval None
  */
static void ETH_DMAReceptionEnable(void)
{
  /* Enable the DMA reception */
  (ETHInstance.Instance)->DMAOMR |= ETH_DMAOMR_SR;
}

/**
  * @brief  Clears the ETHERNET transmit FIFO.
  * @retval None
  */
static void ETH_FlushTransmitFIFO(void)
{
  __IO uint32_t tmpreg;

  /* Set the Flush Transmit FIFO bit */
  (ETHInstance.Instance)->DMAOMR |= ETH_DMAOMR_FTF;

  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg = (ETHInstance.Instance)->DMAOMR;
  delayms(ETH_REG_WRITE_DELAY);
  (ETHInstance.Instance)->DMAOMR = tmpreg;
}

/**
 * @brief  Enables Ethernet MAC and DMA reception/transmission
 * @retval none
 */
void ETH_Start(void)
{
  /* Set the ETH peripheral state to BUSY */
  ETHInstance.State = ETH_STATE_BUSY;

  /* Enable transmit state machine of the MAC for transmission on the MII */
  ETH_MACTransmissionEnable();

  /* Enable receive state machine of the MAC for reception from the MII */
  ETH_MACReceptionEnable();

  /* Flush Transmit FIFO */
  ETH_FlushTransmitFIFO();

  /* Start DMA transmission */
  ETH_DMATransmissionEnable();

  /* Start DMA reception */
  ETH_DMAReceptionEnable();

  /* Set the ETH state to READY*/
  ETHInstance.State= ETH_STATE_READY;
}

/**
  * @brief  Gets the Received frame in interrupt mode.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval HAL status
  */
unsigned int ETH_GetReceivedFrame_IT(void)
{
  uint32_t descriptorscancounter = 0;

  /* Set ETH HAL State to BUSY */
  ETHInstance.State = ETH_STATE_BUSY;

  /* Scan descriptors owned by CPU */
  while (((ETHInstance.RxDesc->Status & ETH_DMARXDESC_OWN) == (uint32_t)RESET) && (descriptorscancounter < ETH_RXBUFNB))
  {
    /* Just for security */
    descriptorscancounter++;

    /* Check if first segment in frame */
    /* ((heth->RxDesc->Status & ETH_DMARXDESC_FS) != (uint32_t)RESET) && ((heth->RxDesc->Status & ETH_DMARXDESC_LS) == (uint32_t)RESET)) */
    if((ETHInstance.RxDesc->Status & (ETH_DMARXDESC_FS | ETH_DMARXDESC_LS)) == (uint32_t)ETH_DMARXDESC_FS)
    {
      ETHInstance.RxFrameInfos.FSRxDesc = ETHInstance.RxDesc;
      ETHInstance.RxFrameInfos.SegCount = 1;
      /* Point to next descriptor */
      ETHInstance.RxDesc = (ETH_DMADescTypeDef*) (ETHInstance.RxDesc->Buffer2NextDescAddr);
    }
      /* Check if intermediate segment */
      /* ((heth->RxDesc->Status & ETH_DMARXDESC_LS) == (uint32_t)RESET)&& ((heth->RxDesc->Status & ETH_DMARXDESC_FS) == (uint32_t)RESET)) */
    else if ((ETHInstance.RxDesc->Status & (ETH_DMARXDESC_LS | ETH_DMARXDESC_FS)) == (uint32_t)RESET)
    {
      /* Increment segment count */
      (ETHInstance.RxFrameInfos.SegCount)++;
      /* Point to next descriptor */
      ETHInstance.RxDesc = (ETH_DMADescTypeDef*)(ETHInstance.RxDesc->Buffer2NextDescAddr);
    }
      /* Should be last segment */
    else
    {
      /* Last segment */
      ETHInstance.RxFrameInfos.LSRxDesc = ETHInstance.RxDesc;

      /* Increment segment count */
      (ETHInstance.RxFrameInfos.SegCount)++;

      /* Check if last segment is first segment: one segment contains the frame */
      if ((ETHInstance.RxFrameInfos.SegCount) == 1)
      {
        ETHInstance.RxFrameInfos.FSRxDesc = ETHInstance.RxDesc;
      }

      /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
      ETHInstance.RxFrameInfos.length = (((ETHInstance.RxDesc)->Status & ETH_DMARXDESC_FL) >> ETH_DMARXDESC_FRAMELENGTHSHIFT) - 4;

      /* Get the address of the buffer start address */
      ETHInstance.RxFrameInfos.buffer =((ETHInstance.RxFrameInfos).FSRxDesc)->Buffer1Addr;

      /* Point to next descriptor */
      ETHInstance.RxDesc = (ETH_DMADescTypeDef*) (ETHInstance.RxDesc->Buffer2NextDescAddr);

      /* Set HAL State to Ready */
      ETHInstance.State = ETH_STATE_READY;

      return 0;
    }
  }

  /* Set HAL State to Ready */
  ETHInstance.State = ETH_STATE_READY;

  return 1;
}

/**
  * @brief  Sends an Ethernet frame.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  FrameLength Amount of data to be sent
  * @retval HAL status
  */
unsigned int ETH_TransmitFrame(unsigned int FrameLength)
{
  uint32_t bufcount, size, i;

  /* Set the ETH peripheral state to BUSY */
  ETHInstance.State = ETH_STATE_BUSY;

  if (FrameLength == 0)
  {
    /* Set ETH HAL state to READY */
    ETHInstance.State = ETH_STATE_READY;

    return 1;
  }

  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if((ETHInstance.TxDesc)->Status & ETH_DMATXDESC_OWN)
  {
    /* OWN bit set */
    ETHInstance.State = ETH_STATE_BUSY_TX;

    return 1;
  }

  /* Get the number of needed Tx buffers for the current frame */
  if (FrameLength > ETH_TX_BUF_SIZE)
  {
    bufcount = FrameLength/ETH_TX_BUF_SIZE;
    if (FrameLength % ETH_TX_BUF_SIZE)
    {
      bufcount++;
    }
  }
  else
  {
    bufcount = 1;
  }

  if (bufcount == 1)
  {
    /* Set LAST and FIRST segment */
    ETHInstance.TxDesc->Status |=ETH_DMATXDESC_FS|ETH_DMATXDESC_LS;
    /* Set frame size */
    ETHInstance.TxDesc->ControlBufferSize = (FrameLength & ETH_DMATXDESC_TBS1);
    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    ETHInstance.TxDesc->Status |= ETH_DMATXDESC_OWN;
    /* Point to next descriptor */
    ETHInstance.TxDesc= (ETH_DMADescTypeDef *)(ETHInstance.TxDesc->Buffer2NextDescAddr);
  }
  else
  {
    for (i=0; i< bufcount; i++)
    {
      /* Clear FIRST and LAST segment bits */
      ETHInstance.TxDesc->Status &= ~(ETH_DMATXDESC_FS | ETH_DMATXDESC_LS);

      if (i == 0)
      {
        /* Setting the first segment bit */
        ETHInstance.TxDesc->Status |= ETH_DMATXDESC_FS;
      }

      /* Program size */
      ETHInstance.TxDesc->ControlBufferSize = (ETH_TX_BUF_SIZE & ETH_DMATXDESC_TBS1);

      if (i == (bufcount-1))
      {
        /* Setting the last segment bit */
        ETHInstance.TxDesc->Status |= ETH_DMATXDESC_LS;
        size = FrameLength - (bufcount-1)*ETH_TX_BUF_SIZE;
        ETHInstance.TxDesc->ControlBufferSize = (size & ETH_DMATXDESC_TBS1);
      }

      /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
      ETHInstance.TxDesc->Status |= ETH_DMATXDESC_OWN;
      /* point to next descriptor */
      ETHInstance.TxDesc = (ETH_DMADescTypeDef *)(ETHInstance.TxDesc->Buffer2NextDescAddr);
    }
  }

  /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
  if (((ETHInstance.Instance)->DMASR & ETH_DMASR_TBUS) != (uint32_t)RESET)
  {
    /* Clear TBUS ETHERNET DMA flag */
    (ETHInstance.Instance)->DMASR = ETH_DMASR_TBUS;
    /* Resume DMA transmission*/
    (ETHInstance.Instance)->DMATPDR = 0;
  }

  /* Set ETH HAL State to Ready */
  ETHInstance.State = ETH_STATE_READY;

  return 0;
}

void ETH_RxCpltCallback(void);
void ETH_TxCpltCallback(void);
void ETH_ErrorCallback(void);

void ETH_InterruptHandler(void)
{
  /* Frame received */
  if (__HAL_ETH_DMA_GET_FLAG(ETH_DMA_FLAG_R))
  {
    /* Receive complete callback */
    ETH_RxCpltCallback();

    /* Clear the Eth DMA Rx IT pending bits */
    __HAL_ETH_DMA_CLEAR_IT(ETH_DMA_IT_R);

    /* Set HAL State to Ready */
    ETHInstance.State = ETH_STATE_READY;
  }
    /* Frame transmitted */
  if (__HAL_ETH_DMA_GET_FLAG(ETH_DMA_FLAG_T))
  {
    /* Transfer complete callback */
    ETH_TxCpltCallback();

    /* Clear the Eth DMA Tx IT pending bits */
    __HAL_ETH_DMA_CLEAR_IT(ETH_DMA_IT_T);

    /* Set HAL State to Ready */
    ETHInstance.State = ETH_STATE_READY;
  }

  /* Clear the interrupt flags */
  __HAL_ETH_DMA_CLEAR_IT(ETH_DMA_IT_NIS);

  /* ETH DMA Error */
  if(__HAL_ETH_DMA_GET_FLAG(ETH_DMA_FLAG_AIS))
  {
    /* Ethernet Error callback */
    ETH_ErrorCallback();

    /* Clear the interrupt flags */
    __HAL_ETH_DMA_CLEAR_IT(ETH_DMA_FLAG_AIS);

    /* Set HAL State to Ready */
    ETHInstance.State = ETH_STATE_READY;
  }
}

unsigned short ETH_Swap(unsigned short v)
{
  return ((v & 0xFF) << 8) | (v >> 8);
}
