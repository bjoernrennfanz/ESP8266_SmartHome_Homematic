/*
* command.h
*
*  Created on: 29.04.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#ifndef CC1101_H_
#define CC1101_H_

#define CC1101_DATA_LEN				60		//!< Maximum length of received bytes

// CC1101 config register					Description
#define CC1101_REG_IOCFG2			0x00	//!< (0x29) GDO2 Output Pin Configuration
#define CC1101_REG_IOCFG1			0x01	//!< (0x2E) GDO1 Output Pin Configuration
#define CC1101_REG_IOCFG0			0x02	//!< (0x3F) GDO0 Output Pin Configuration
#define CC1101_REG_FIFOTHR			0x03	//!< (0x07) RX FIFO and TX FIFO Thresholds
#define CC1101_REG_SYNC1			0x04	//!< (0xD3) Sync Word, High Byte
#define CC1101_REG_SYNC0			0x05	//!< (0x91) Sync Word, Low Byte
#define CC1101_REG_PKTLEN			0x06	//!< (0xFF) Packet Length
#define CC1101_REG_PKTCTRL1			0x07	//!< (0x04) Packet Automation Control
#define CC1101_REG_PKTCTRL0			0x08	//!< (0x45) Packet Automation Control
#define CC1101_REG_ADDR				0x09	//!< (0x00) Device Address
#define CC1101_REG_CHANNR			0x0A	//!< (0x00) Channel Number
#define CC1101_REG_FSCTRL1			0x0B	//!< (0x0F) Frequency Synthesizer Control
#define CC1101_REG_FSCTRL0			0x0C	//!< (0x00) Frequency Synthesizer Control
#define CC1101_REG_FREQ2			0x0D	//!< (0x1E) Frequency Control Word, High Byte
#define CC1101_REG_FREQ1			0x0E	//!< (0xC4) Frequency Control Word, Middle Byte
#define CC1101_REG_FREQ0			0x0F	//!< (0xEC) Frequency Control Word, Low Byte
#define CC1101_REG_MDMCFG4			0x10	//!< (0x8C) Modem Configuration
#define CC1101_REG_MDMCFG3			0x11	//!< (0x22) Modem Configuration
#define CC1101_REG_MDMCFG2			0x12	//!< (0x02) Modem Configuration
#define CC1101_REG_MDMCFG1			0x13	//!< (0x22) Modem Configuration
#define CC1101_REG_MDMCFG0			0x14	//!< (0xF8) Modem Configuration
#define CC1101_REG_DEVIATN			0x15	//!< (0x47) Modem Deviation Setting
#define CC1101_REG_MCSM2			0x16	//!< (0x07) Main Radio Control State Machine Configuration
#define CC1101_REG_MCSM1			0x17	//!< (0x30) Main Radio Control State Machine Configuration
#define CC1101_REG_MCSM0			0x18	//!< (0x04) Main Radio Control State Machine Configuration
#define CC1101_REG_FOCCFG			0x19	//!< (0x36) Frequency Offset Compensation Configuration
#define CC1101_REG_BSCFG			0x1A	//!< (0x6C) Bit Synchronization Configuration
#define CC1101_REG_AGCCTRL2			0x1B	//!< (0x03) AGC Control
#define CC1101_REG_AGCCTRL1			0x1C	//!< (0x40) AGC Control
#define CC1101_REG_AGCCTRL0			0x1D	//!< (0x91) AGC Control
#define CC1101_REG_WOREVT1			0x1E	//!< (0x87) High Byte Event0 Timeout
#define CC1101_REG_WOREVT0			0x1F	//!< (0x6B) Low Byte Event0 Timeout
#define CC1101_REG_WORCTRL			0x20	//!< (0xF8) Wake On Radio Control
#define CC1101_REG_FREND1			0x21	//!< (0x56) Front End RX Configuration
#define CC1101_REG_FREND0			0x22	//!< (0x10) Front End RX Configuration
#define CC1101_REG_FSCAL3			0x23	//!< (0xA9) Frequency Synthesizer Calibration
#define CC1101_REG_FSCAL2			0x24	//!< (0x0A) Frequency Synthesizer Calibration
#define CC1101_REG_FSCAL1			0x25	//!< (0x20) Frequency Synthesizer Calibration
#define CC1101_REG_FSCAL0			0x26	//!< (0x0D) Frequency Synthesizer Calibration
#define CC1101_REG_RCCTRL1			0x27	//!< (0x41) RC Oscillator Configuration
#define CC1101_REG_RCCTRL2			0x28	//!< (0x00) RC Oscillator Configuration
#define CC1101_REG_FSTEST			0x29	//!< (0x59) Frequency Synthesizer Calibration Control
#define CC1101_REG_PTEST			0x2A	//!< (0x7F) Production Test
#define CC1101_REG_AGCTEST			0x2B	//!< (0x3F) AGC Test
#define CC1101_REG_TEST2			0x2C	//!< (0x88) Various Test Settings
#define CC1101_REG_TEST1			0x2D	//!< (0x31) Various Test Settings
#define CC1101_REG_TEST0			0x2E	//!< (0x0B) Various Test Settings

#define CC1101_REG_PARTNUM			0x30	//!< (0x00) Readonly: Chip ID
#define CC1101_REG_VERSION			0x31	//!< (0x04) Readonly: Chip ID
#define CC1101_REG_FREQEST			0x32	//!< (0x00) Readonly: Frequency Offset Estimate from Demodulator
#define CC1101_REG_LQI				0x33	//!< (0x00) Readonly: Demodulator Estimate for Link Quality
#define CC1101_REG_RSSI				0x34	//!< (0x00) Readonly: Received Signal Strength Indication
#define CC1101_REG_MARCSTATE		0x35	//!< (0x00) Readonly: Main Radio Control State Machine State
#define CC1101_REG_WORTIME1			0x36	//!< (0x00) Readonly: High Byte of WOR Time
#define CC1101_REG_WORTIME0			0x37	//!< (0x00) Readonly: Low Byte of WOR Time
#define CC1101_REG_PKTSTATUS		0x38	//!< (0x00) Readonly: Current GDOx Status and Packet Status
#define CC1101_REG_VCO_VC_DAC		0x39	//!< (0x00) Readonly: Current Setting from PLL Calibration Module
#define CC1101_REG_TXBYTES			0x3A	//!< (0x00) Readonly: Underflow and Number of Bytes
#define CC1101_REG_RXBYTES			0x3B	//!< (0x00) Readonly: Overflow and Number of Bytes
#define CC1101_REG_RCCTRL1_STATUS   0x3C	//!< (0x00) Readonly: Last RC Oscillator Calibration Result
#define CC1101_REG_RCCTRL0_STATUS   0x3D	//!< (0x00) Readonly: Last RC Oscillator Calibration Result

#define CC1101_REG_PATABLE          0x3E	//!< PATABLE address
#define CC1101_REG_TXFIFO           0x3F	//!< TX FIFO address
#define CC1101_REG_RXFIFO           0x3F	//!< RX FIFO address

#define CC1101_REG_PA_TABLE0        0x40	//!< (0x00) PA table, entry 0
#define CC1101_REG_PA_TABLE1        0x41	//!< (0x00) PA table, entry 1
#define CC1101_REG_PA_TABLE2        0x42	//!< (0x00) PA table, entry 2
#define CC1101_REG_PA_TABLE3        0x43	//!< (0x00) PA table, entry 3
#define CC1101_REG_PA_TABLE4        0x44	//!< (0x00) PA table, entry 4
#define CC1101_REG_PA_TABLE5        0x45	//!< (0x00) PA table, entry 5
#define CC1101_REG_PA_TABLE6        0x46	//!< (0x00) PA table, entry 6
#define CC1101_REG_PA_TABLE7        0x47	//!< (0x00) PA table, entry 7

// Register definitions for TRX868 communication
#define CC1101_READ_SINGLE          0x80	//!< type of transfers
#define CC1101_READ_BURST           0xC0
#define CC1101_WRITE_BURST          0x40

#define CC1101_TYPE_CONFIG          0x80	//!< type of register
#define CC1101_TYPE_STATUS          0xC0

#define CC1101_CMD_SRES             0x30	//!< reset CC1101_REG chip
#define CC1101_CMD_SFSTXON          0x31	//!< enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1). if in RX (with CCA): Go to a wait state where only the synthesizer is running (for quick RX / TX turnaround).
#define CC1101_CMD_SXOFF            0x32	//!< turn off crystal oscillator
#define CC1101_CMD_SCAL             0x33	//!< calibrate frequency synthesizer and turn it off. SCAL can be strobed from IDLE mode without setting manual calibration mode (MCSM0.FS_AUTOCAL=0)
#define CC1101_CMD_SRX              0x34	//!< enable RX. perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1
#define CC1101_CMD_STX              0x35	//!< in IDLE state: enable TX. perform calibration first if MCSM0.FS_AUTOCAL=1. if in RX state and CCA is enabled: only go to TX if channel is clear
#define CC1101_CMD_SIDLE            0x36	//!< exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable
#define CC1101_CMD_SWOR             0x38	//!< start automatic RX polling sequence (Wake-on-Radio) as described in Section 19.5 if WORCTRL.RC_PD=0
#define CC1101_CMD_SPWD             0x39	//!< enter power down mode when CSn goes high
#define CC1101_CMD_SFRX             0x3A	//!< flush the RX FIFO buffer. only issue SFRX in IDLE or RXFIFO_OVERFLOW states
#define CC1101_CMD_SFTX             0x3B	//!< flush the TX FIFO buffer. only issue SFTX in IDLE or TXFIFO_UNDERFLOW states
#define CC1101_CMD_SWORRST          0x3C	//!< reset real time clock to Event1 value
#define CC1101_CMD_SNOP             0x3D	//!< no operation. may be used to get access to the chip status byte

// CC1101 Control machine state
#define CC1101_CMS_SLEEP			0x00
#define CC1101_CMS_IDLE				0x01
#define CC1101_CMS_XOFF				0x02
#define CC1101_CMS_VCOON_MC			0x03
#define CC1101_CMS_REGON_MC			0x04
#define CC1101_CMS_MANCAL			0x05
#define CC1101_CMS_VCOON			0x06
#define CC1101_CMS_REGON			0x07
#define CC1101_CMS_STARTCAL			0x08
#define CC1101_CMS_BWBOOST			0x09
#define CC1101_CMS_FS_LOCK			0x0A
#define CC1101_CMS_IFADCON			0x0B
#define CC1101_CMS_ENDCAL			0x0C
#define CC1101_CMS_RX				0x0D
#define CC1101_CMS_RX_END			0x0E
#define CC1101_CMS_RX_RST			0x0F
#define CC1101_CMS_TXRX_SWITCH		0x10
#define CC1101_CMS_RXFIFO_OFLOW		0x11
#define CC1101_CMS_FSTXON			0x12
#define CC1101_CMS_TX				0x13
#define CC1101_CMS_TX_END			0x14
#define CC1101_CMS_RXTX_SWITCH		0x15
#define CC1101_CMS_TXFIFO_UFLOW		0x16

// CC1101 Power mode
#define CC1101_PA_LOWPOWER          0x03
#define CC1101_PA_NORMAL            0x50
#define CC1101_PA_MAXPOWER			0xC0

#include <stdint.h>

/**
* CC1101 module descriptor
*/
typedef struct
{
	uint8_t crc_ok;				//!< CRC OK for received message
	
	uint8_t rssi;				//!< Signal strength
	uint8_t lqi;				//!< Link quality

	uint8_t cs_pin;				//!< GPIO connected to CS/LOAD pin
} cc1101_module_t;

// CC1101 prototypes
void cc1101_set_idle(cc1101_module_t *mod);			// put CC1101 into power-down state
uint8_t cc1101_detect_burst(cc1101_module_t *mod);	// detect burst signal, sleep while no signal, otherwise stay awake

void cc1101_init(cc1101_module_t *mod);											// initialize CC1101
uint8_t cc1101_snd_data(cc1101_module_t *mod, uint8_t *buf, uint8_t burst);		// send data packet via RF
uint8_t cc1101_rcv_data(cc1101_module_t *mod, uint8_t *buf);					// read data packet from RX FIFO

#endif /* CC1101_H_ */