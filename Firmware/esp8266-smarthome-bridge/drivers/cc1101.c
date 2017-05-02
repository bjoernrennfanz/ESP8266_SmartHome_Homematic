/*
* cc1101.c
*
*  Created on: 29.04.2017
*      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
*      License: MIT, see LICENSE file for more details.
*/

#include "cc1101.h"

#include <esp/spi.h>
#include <esp/gpio.h>
#include <espressif/esp_misc.h> // sdk_os_delay_us

#define SPI_BUS 1

#define CC1101_DEBUG

#ifdef CC1101_DEBUG
#	include <stdio.h>
#	define debug(fmt, ...) printf("%s: " fmt "\n", "CC1101", ## __VA_ARGS__)
#else
#	define debug(fmt, ...)
#endif

static const spi_settings_t cc1101_bus_settings = {
	.mode = SPI_MODE0,
	.freq_divider = SPI_FREQ_DIV_4M,
	.msb = true,
	.minimal_pins = true,
	.endianness = SPI_LITTLE_ENDIAN
};

// Internal prototypes
void cc1101_strobe(cc1101_driver_config_t *config, uint8_t cmd);										// send command strobe to the CC1101 IC via SPI
void cc1101_read_burst(cc1101_driver_config_t *config, uint8_t * buf, uint8_t regAddr, uint8_t len);	// read burst data from CC1101 via SPI
void cc1101_write_burst(cc1101_driver_config_t *config, uint8_t regAddr, uint8_t* buf, uint8_t len);	// write multiple registers into the CC1101 IC via SPI
uint8_t cc1101_read_reg(cc1101_driver_config_t *config, uint8_t regAddr, uint8_t regType);			// read CC1101 register via SPI
void cc1101_write_reg(cc1101_driver_config_t *config, uint8_t regAddr, uint8_t val);

void cc1101_wait_miso(cc1101_driver_config_t *config)
{
	// Avoid unused warning
	(void)config;

	// Wait until MISO goes low
	while (gpio_read(12) == 1);
}

void cc1101_set_idle(cc1101_driver_config_t *config)
{
	// Coming from RX state, we need to enter the IDLE state first
	cc1101_strobe(config, CC1101_CMD_SIDLE);
	cc1101_strobe(config, CC1101_CMD_SFRX);
	cc1101_strobe(config, CC1101_CMD_SPWD);
}

uint8_t cc1101_detect_burst(cc1101_driver_config_t *config)
{
	// Power on cc1101 configule and set to RX confige
	gpio_write(config->cs_pin, false);
	cc1101_wait_miso(config);
	gpio_write(config->cs_pin, true);

	// Wait for wakeup of CC1101
	for (uint8_t i = 0; i < 200; i++) 
	{	
		// instead of delay, check the really needed time to wakeup
		if (cc1101_read_reg(config, CC1101_REG_MARCSTATE, CC1101_TYPE_STATUS) != 0xff) break;
		sdk_os_delay_us(10);
	}

	// set RX confige again
	cc1101_strobe(config, CC1101_CMD_SRX);								

	uint8_t bTmp;
	for (uint8_t i = 0; i < 200; i++) 
	{																			// check if we are in RX confige
		bTmp = cc1101_read_reg(config, CC1101_REG_PKTSTATUS, CC1101_TYPE_STATUS);	// read the status of the line
		if ((bTmp & 0x10) || (bTmp & 0x40)) break;								// check for channel clear, or carrier sense
		sdk_os_delay_us(10);													// wait a bit
	}

	return (bTmp & 0x40) ? 1 : 0;												// return carrier sense bit
}

void cc1101_init(cc1101_driver_config_t *config)
{
	debug("Initialize CC1101 using chip select on GPIO%d", config->cs_pin);

	// Set chip select to high
	gpio_enable(config->cs_pin, GPIO_OUTPUT);
	gpio_write(config->cs_pin, true);

	// Initializing sequence
	sdk_os_delay_us(5);
	gpio_write(config->cs_pin, false);
	sdk_os_delay_us(10);
	gpio_write(config->cs_pin, true);
	sdk_os_delay_us(41);

	// Send reset
	cc1101_strobe(config, CC1101_CMD_SRES);															
	sdk_os_delay_us(10000);

	// define init settings for TRX868
	static const uint8_t initVal[] = {
		CC1101_REG_IOCFG2,    0x2E,		// non inverted GDO2, high impedance tri state
		// CC1101_REG_IOCFG1, 0x2E,		// low output drive strength, non inverted GD=1, high impedance tri state
		CC1101_REG_IOCFG0,    0x06,		// packet CRC oK; disable temperature sensor, non inverted GDO0,
		CC1101_REG_FIFOTHR,   0x0D,		// 0 ADC retention, 0 close in RX, TX FIFO = 9 / RX FIFO = 56 byte
		CC1101_REG_SYNC1,     0xE9,		// Sync word
		CC1101_REG_SYNC0,     0xCA,
		CC1101_REG_PKTLEN,    0x3D,		// packet length 61
		CC1101_REG_PKTCTRL1,  0x0C,		// PQT = 0, CRC auto flush = 1, append status = 1, no address check
		CC1101_REG_FSCTRL1,   0x06,		// frequency synthesizer control

		// 868.299866 MHz
		//CC1101_REG_FREQ2,   0x21,
		//CC1101_REG_FREQ1,   0x65,
		//CC1101_REG_FREQ0,   0x6A,

		// 868.2895508
		CC1101_REG_FREQ2,     0x21,
		CC1101_REG_FREQ1,     0x65,
		CC1101_REG_FREQ0,     0x50,

		CC1101_REG_MDMCFG4,	  0xC8,
		CC1101_REG_MDMCFG3,   0x93,
		CC1101_REG_MDMCFG2,   0x03,
		CC1101_REG_DEVIATN,   0x34,		// 19.042969 kHz
		CC1101_REG_MCSM2,     0x01,
		// CC1101_REG_MCSM1,  0x30,		// always go into IDLE
		CC1101_REG_MCSM0,     0x18,
		CC1101_REG_FOCCFG,    0x16,
		CC1101_REG_AGCCTRL2,  0x43,
		//CC1101_REG_WOREVT1, 0x28,		// tEVENT0 = 50 ms, RX timeout = 390 us
		//CC1101_REG_WOREVT0, 0xA0,
		//CC1101_REG_WORCTRL, 0xFB,	    // EVENT1 = 3, WOR_RES = 0
		CC1101_REG_FREND1,    0x56,
		CC1101_REG_FSCAL1,    0x00,
		CC1101_REG_FSCAL0,    0x11,
		CC1101_REG_TEST1,     0x35,
		CC1101_REG_PATABLE,   0xC3
	};

	for (uint8_t i = 0; i < sizeof(initVal); i += 2)
	{	// Write init value to TRX868
		cc1101_write_reg(config, initVal[i], initVal[i + 1]);
		debug("%d: %02x %02x", i, initVal[i], initVal[i + 1]);
	}

	cc1101_strobe(config, CC1101_CMD_SCAL);	// calibrate frequency synthesizer and turn it off
	while (cc1101_read_reg(config, CC1101_REG_MARCSTATE, CC1101_TYPE_STATUS) != 1)
	{	// waits until configule gets ready
		sdk_os_delay_us(1);
	}

	cc1101_write_reg(config, CC1101_REG_PATABLE, CC1101_PA_MAXPOWER);	// configure PATABLE
	cc1101_strobe(config, CC1101_CMD_SRX);								// flush the RX buffer
	cc1101_strobe(config, CC1101_CMD_SWORRST);							// reset real time clock

	debug("%s", "CC1101 is ready\n");
}

uint8_t cc1101_snd_data(cc1101_driver_config_t *config, uint8_t *buf, uint8_t burst)
{
	// Going from RX to TX does not work if there was a reception less than 0.5
	// sec ago. Due to CCA? Using IDLE helps to shorten this period(?)
	cc1101_strobe(config, CC1101_CMD_SIDLE);			// go to idle confige
	cc1101_strobe(config, CC1101_CMD_SFRX);			// flush RX buffer
	cc1101_strobe(config, CC1101_CMD_SFTX);			// flush TX buffer
	debug("%s", "Switch to transmit confige");

	if (burst)										// BURST-bit set?
	{
		cc1101_strobe(config, CC1101_CMD_STX);			// send a burst
		sdk_os_delay_us(36000);						// according to ELV, devices get activated every 300ms, so send burst for 360ms
		debug("%s", "Send burst sequence");
	}
	else 
	{
		sdk_os_delay_us(1000);						// wait a short time to set TX confige
	}

	// write in TX FIFO
	cc1101_write_burst(config, CC1101_REG_TXFIFO, buf, buf[0] + 1);

	cc1101_strobe(config, CC1101_CMD_SFRX);			// flush the RX buffer
	cc1101_strobe(config, CC1101_CMD_STX);				// send a burst

	for (uint8_t i = 0; i < 200; i++) 
	{	
		// after sending out all bytes the chip should go automatically in RX confige
		if (cc1101_read_reg(config, CC1101_REG_MARCSTATE, CC1101_TYPE_STATUS) == CC1101_CMS_RX)
			break;									//now in RX confige, good
		if (cc1101_read_reg(config, CC1101_REG_MARCSTATE, CC1101_TYPE_STATUS) != CC1101_CMS_TX)
			break;									//neither in RX nor TX, probably some error

		sdk_os_delay_us(10);
	}
	
	debug("%s", "Switch to receive confige");
	return true;
}

uint8_t cc1101_rcv_data(cc1101_driver_config_t *config, uint8_t *buf)
{
	// how many bytes are in the buffer
	uint8_t rxBytes = cc1101_read_reg(config, CC1101_REG_RXBYTES, CC1101_TYPE_STATUS);

	// any byte waiting to be read and no overflow?
	if ((rxBytes & 0x7F) && !(rxBytes & 0x80)) 
	{
		// read data length
		buf[0] = cc1101_read_reg(config, CC1101_REG_RXFIFO, CC1101_TYPE_CONFIG);

		// if packet is too long
		if (buf[0] > CC1101_DATA_LEN) 
		{				
			// discard packet
			buf[0] = 0;	
		}
		else 
		{
			// read data packet
			cc1101_read_burst(config, &buf[1], CC1101_REG_RXFIFO, buf[0]);	

			// read RSSI
			config->rssi = cc1101_read_reg(config, CC1101_REG_RXFIFO, CC1101_TYPE_CONFIG);

			if (config->rssi >= 128) 
				config->rssi = 255 - config->rssi;
			
			config->rssi /= 2; 
			config->rssi += 72;

			// read LQI and CRC_OK
			uint8_t val = cc1101_read_reg(config, CC1101_REG_RXFIFO, CC1101_TYPE_CONFIG);

			config->lqi = val & 0x7F;
			config->crc_ok = val & 0x80;
		}
	}
	else
	{
		// nothing to do, or overflow
		buf[0] = 0;
	}

	cc1101_strobe(config, CC1101_CMD_SFRX);	// flush Rx FIFO
	cc1101_strobe(config, CC1101_CMD_SIDLE);	// enter IDLE state
	cc1101_strobe(config, CC1101_CMD_SRX);		// back to RX state
	cc1101_strobe(config, CC1101_CMD_SWORRST);	// reset real time clock

	// Debug outputs

#ifdef CC1101_DEBUG
	if (buf[0] > 0) debug("%d bytes read", buf[0] + 1);
#endif

	return buf[0];
}

void cc1101_strobe(cc1101_driver_config_t *config, uint8_t cmd)
{
	// Store current spi settings
	spi_settings_t old_settings;
	spi_get_settings(SPI_BUS, &old_settings);
	spi_set_settings(SPI_BUS, &cc1101_bus_settings);

	// Send command strobe to the CC1101 IC via SPI
	gpio_write(config->cs_pin, false);					// select CC1101
	cc1101_wait_miso(config);							// wait until MISO goes low
	
	spi_transfer_8(SPI_BUS, cmd);					// send strobe command

	gpio_write(config->cs_pin, true);					// deselect CC1101
	spi_set_settings(SPI_BUS, &old_settings);		// Restore old spi settings																	
}

void cc1101_read_burst(cc1101_driver_config_t *config, uint8_t * buf, uint8_t regAddr, uint8_t len)
{
	// Store current spi settings
	spi_settings_t old_settings;
	spi_get_settings(SPI_BUS, &old_settings);
	spi_set_settings(SPI_BUS, &cc1101_bus_settings);

	gpio_write(config->cs_pin, false);							// select CC1101
	cc1101_wait_miso(config);									// wait until MISO goes low

	spi_transfer_8(SPI_BUS, regAddr | CC1101_READ_BURST);	// send register address
	for (uint8_t i = 0; i<len; i++) 
		buf[i] = spi_transfer_8(SPI_BUS, 0x00);				// read result byte by byte

	gpio_write(config->cs_pin, true);							// deselect CC1101
	spi_set_settings(SPI_BUS, &old_settings);				// Restore old spi settings	
}

void cc1101_write_burst(cc1101_driver_config_t *config, uint8_t regAddr, uint8_t* buf, uint8_t len)
{
	// Store current spi settings
	spi_settings_t old_settings;
	spi_get_settings(SPI_BUS, &old_settings);
	spi_set_settings(SPI_BUS, &cc1101_bus_settings);

	gpio_write(config->cs_pin, false);							// select CC1101
	cc1101_wait_miso(config);									// wait until MISO goes low

	spi_transfer_8(SPI_BUS, regAddr | CC1101_WRITE_BURST);	// send register address
	for (uint8_t i = 0; i<len; i++) 
		spi_transfer_8(SPI_BUS, buf[i]);					// send value

	gpio_write(config->cs_pin, true);							// deselect CC1101
	spi_set_settings(SPI_BUS, &old_settings);				// Restore old spi settings	
}

uint8_t cc1101_read_reg(cc1101_driver_config_t *config, uint8_t regAddr, uint8_t regType)
{
	// Store current spi settings
	spi_settings_t old_settings;
	spi_get_settings(SPI_BUS, &old_settings);
	spi_set_settings(SPI_BUS, &cc1101_bus_settings);

	gpio_write(config->cs_pin, false);							// select CC1101
	cc1101_wait_miso(config);									// wait until MISO goes low

	spi_transfer_8(SPI_BUS, regAddr | regType);				// send register address
	uint8_t val = spi_transfer_8(SPI_BUS, 0x00);			// read result

	gpio_write(config->cs_pin, true);							// deselect CC1101
	spi_set_settings(SPI_BUS, &old_settings);				// Restore old spi settings	

	return val;
}

void cc1101_write_reg(cc1101_driver_config_t *config, uint8_t regAddr, uint8_t val)
{
	// Store current spi settings
	spi_settings_t old_settings;
	spi_get_settings(SPI_BUS, &old_settings);
	spi_set_settings(SPI_BUS, &cc1101_bus_settings);

	gpio_write(config->cs_pin, false);							// select CC1101
	cc1101_wait_miso(config);									// wait until MISO goes low

	spi_transfer_8(SPI_BUS, regAddr);						// send register address
	spi_transfer_8(SPI_BUS, val);							// send value

	gpio_write(config->cs_pin, true);							// deselect CC1101
	spi_set_settings(SPI_BUS, &old_settings);				// Restore old spi settings	
}