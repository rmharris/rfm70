// Copyright (c) 2014, Robert Harris.

#include <RFM70.h>
#include <RFM70_impl.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <stdlib.h>


RFM70::RFM70(byte slaveselect, byte ce, byte irq)
{
	//_slaveselect = slaveselect;
	_ce = ce;
	_irq = irq;
	_bank = BANK_UNINITIALISED;
}

void RFM70::begin(void)
{
	//pinMode(_slaveselect, OUTPUT);
	pinMode(_ce, OUTPUT);
	//digitalWrite(_slaveselect, HIGH);

	/* The sample code implies that one needs to delay for > 50 ms. */
	delay(200);
}

/*
 * The datasheet directs one to initialise various registers  with specific
 * values but gives no explanation. Note that arrays are processed LSB
 * first except for registers 0 through 8 in register bank 1 which are MSB
 * first (!).
 */
void RFM70::config_magic(void)
{
	write(R_RB1_00, 0x40, 0x4b, 0x01, 0xe2);
	write(R_RB1_01, 0xc0, 0x4b, 0x00, 0x00);
	write(R_RB1_02, 0xd0, 0xfc, 0x8c, 0x02);
	write(R_RB1_03, 0x99, 0x00, 0x39, 0x41);
	write(R_RB1_04, 0xf9, 0x9e, 0x86, 0x0b);
	write(R_RB1_05, 0x24, 0x06, 0x7f, 0xa6);
	write(R_RB1_06, 0x00, 0x00, 0x00, 0x00);
	write(R_RB1_07, 0x00, 0x00, 0x00, 0x00);
	//write(R_CHIP_ID, 0x00, 0x00, 0x00, 0x00);

	write(R_RB1_09, 0x00, 0x00, 0x00, 0x00);
	write(R_RB1_0A, 0x00, 0x00, 0x00, 0x00);
	write(R_RB1_0B, 0x00, 0x00, 0x00, 0x00);
	write(R_RB1_0C, 0x00, 0x12, 0x73, 0x00);
	write(R_NEW_FEATURE, 0x36, 0xb4, 0x80, 0x00);
	write(R_RAMP, 0x41, 0x20, 0x08, 0x04,
	    0x81, 0x20, 0xcf, 0xf7, 0xfe, 0xff, 0xff);
	
	/*
	 * The manufacturer's sample code concludes by toggling bits 25 and
	 * 26 in the most significant byte of register 4 in bank 1.
	 */
	write(R_RB1_04, 0xff, 0x9e, 0x86, 0x0b);
	write(R_RB1_04, 0xf9, 0x9e, 0x86, 0x0b);
}

/*
 * This is the only function that interacts with the RFM70 directly. It
 * implements the entire command set described in its datasheet.
 */
byte RFM70::command(byte command, byte address, byte *data, byte len)
{
	byte write;
	byte val;
	unsigned int i;
	byte bigbuf[40];
	
	/*
	 * We determine whether or not the command will require a read from
	 * or a write to an array, and perform any special preparation.
	 */
	switch (command) {
	case C_R_REGISTER:
	case C_W_REGISTER:
		/*
		 * A C_R_REGISTER or C_W_REGISTER command operates on a
		 * specific register; indeed, the register address must be
		 * OR'd into the command itself.
		 *
		 * There are two banks of registers and the device expects
		 * the consumer to switch to whichever is appropriate. We
		 * conceal this detail from the user by using a bit in the
		 * register address to indicate its register bank; we check
		 * here that the correct bank is selected.
	 	 *
		 * The current bank is stored in _bank; if _bank is
		 * uninitialised then we call ourselves to determine the
	 	 * current bank by reading register 0x07. The same address
		 * works for both register banks; by using R_STATUS (i.e.
		 * the bank 0 version) and temporarily setting _bank to 0
		 * we avoid recursing indefinitely.
		 */
		byte bank;
#ifndef nRF24L01
		if (_bank == BANK_UNINITIALISED) {
			byte status;
			_bank = 0;
			(void) RFM70::command(C_R_REGISTER, R_STATUS,
			    &status, 1);
			_bank = (status & B_STATUS_RBANK) ? 1 : 0;
		}
		
		bank = (address & BANK_FLAG) ? 1 : 0;
		if (bank != _bank) {
			byte activate = ACTIVATE_TOGGLE;
			(void) RFM70::command(C_ACTIVATE, 0, &activate, 1);
			_bank = bank;
		}
#endif

		write = (command == C_W_REGISTER) ? 1 : 0;
		command |= (address & ~BANK_FLAG);
		break;
	case C_W_ACK_PAYLOAD:
		/*		
		 * The C_W_ACK_PAYLOAD command also operates on a pipe
		 * address and this too is OR'd into the command itself.
		 */
	 	write = 1;
		command |= address;
		break;
	case C_W_TX_PAYLOAD:
	case C_ACTIVATE:
	case C_W_TX_PAYLOAD_NO_ACK:
		write = 1;
		break;
	default:
		write = 0;
		break;
	}

	
	bigbuf[0] = command;
	
	for (i = 0; i < len; i++)
		bigbuf[i + 1] = (write == 1) ? data[i] : 0;
	
	
	//printf("sending ");
	//for (i = 0; i < len + 1; i++)
	//	printf("0x%hhx ", bigbuf[i]);
	//printf("\n");
	
	wiringPiSPIDataRW(0, bigbuf, len + 1);
	//printf("recevied ");
	//for (i = 0; i < len + 1; i++)
	//	printf("0x%hhx ", bigbuf[i]);
	//printf("\n");
	
	if (write == 0) {
	for (i = 0; i < len; i++)
		data[i] = bigbuf[i + 1];
	}
	
	
	
	/* Send the command and write or read the data. */
	//digitalWrite(_slaveselect, LOW);
	/*
	printf("Writing one byte: 0x%hhx\n", command);
	wiringPiSPIDataRW(0, &command, 1);
	printf("swapped with: 0x%hhx\n", command);
	
	//val = SPI.transfer(command);
	
	
	if (write == 0) {
		for (i = 0; i < len; i++)
			data[i] = 0;
	}
		wiringPiSPIDataRW(0, data, len);
	
	
	for (i = 0; i < len; i++)
		printf("0x%hhx ", data[i]);
	printf("\n");
	
	
	
	while (len--) {
		if (write)
			(void) SPI.transfer(*data++);
		else
			*data++ = SPI.transfer(0x00);
	}
	 */
	
	//digitalWrite(_slaveselect, HIGH);

	
	
	delay(20);

	return (command);
}

void RFM70::read(byte reg, byte *data, byte len)
{
	(void) command(C_R_REGISTER, reg, data, len);
}

byte RFM70::read(byte reg)
{
	byte val;
	(void) command(C_R_REGISTER, reg, &val, 1);
	return (val);
}

void RFM70::write(byte reg, byte *data, byte len)
{
	(void) command(C_W_REGISTER, reg, data, len);
}

void RFM70::write(byte reg, byte data)
{
	(void) command(C_W_REGISTER, reg, &data, 1);
}

/* XXX Kill these three ASAP. */
void RFM70::write(byte reg, byte d0, byte d1, byte d2, byte d3)
{
	byte data[4];

	data[0] = d0;
	data[1] = d1;
	data[2] = d2;
	data[3] = d3;
	(void) command(C_W_REGISTER, reg, data, 4);
}

void RFM70::write(byte reg, byte d0, byte d1, byte d2, byte d3, byte d4)
{
	byte data[5];

	data[0] = d0;
	data[1] = d1;
	data[2] = d2;
	data[3] = d3;
	data[4] = d4;
	(void) command(C_W_REGISTER, reg, data, 5);
}

void RFM70::write(byte reg, byte d0, byte d1, byte d2, byte d3, byte d4,
    byte d5, byte d6, byte d7, byte d8, byte d9, byte d10)
{
	byte data[11];

	data[0] = d0;
	data[1] = d1;
	data[2] = d2;
	data[3] = d3;
	data[4] = d4;
	data[5] = d5;
	data[6] = d6;
	data[7] = d7;
	data[8] = d8;
	data[9] = d9;
	data[10] = d10;
	(void) command(C_W_REGISTER, reg, data, 11);
}

void RFM70::set_mode(int mode)
{
	switch (mode) {
	case MODE_POWER_DOWN:
		digitalWrite(_ce, LOW);
		write(R_CONFIG, read(R_CONFIG) & ~B_CONFIG_PWR_UP);
		break;
	case MODE_STANDBY_ONE:
		digitalWrite(_ce, LOW);
		write(R_CONFIG, read(R_CONFIG) | B_CONFIG_PWR_UP);
		break;
	case MODE_RX:
		write(R_STATUS, read(R_STATUS));
		digitalWrite(_ce, LOW);
		write(R_CONFIG, read(R_CONFIG) | B_CONFIG_PWR_UP | B_CONFIG_PRIM_RX);
		digitalWrite(_ce, HIGH);
		break;
	case MODE_TX:
		digitalWrite(_ce, LOW);
		write(R_CONFIG, read(R_CONFIG) & (B_CONFIG_PWR_UP | ~B_CONFIG_PRIM_RX));
		digitalWrite(_ce, HIGH);
		break;
	default:
		break;
	}
}

void RFM70::dump_reg(byte reg, byte val)
{

	byte rxpno;

	printf("----------\n");
	switch(reg) {
	case R_CONFIG:
		printf("R_CONFIG: 0x%hhx\n", val);
		if (val & B_CONFIG_MASK_RX_DR)
			printf("MASK_RX_DR ");
		if (val & B_CONFIG_MASK_TX_DS)
			printf("MASK_TX_DS ");
		if (val & B_CONFIG_MASK_MAX_RT)
			printf("MASK_MAX_RT ");
		if (val & B_CONFIG_EN_CRC)
			printf("EN_CRC ");
		if (val & B_CONFIG_CRCO)
			printf("CRCO ");
		if (val & B_CONFIG_PWR_UP)
			printf("PWR_UP ");
		if (val & B_CONFIG_PRIM_RX)
			printf("PRIM_RX ");
		if (val)
			printf("\n");
		break;
	case R_STATUS:
		printf("R_STATUS: 0x%hhx\n", val);
		printf("Register bank %d selected\n", (val & B_STATUS_RBANK) ? 1 : 0);
		if (val & B_STATUS_RX_DR)
			printf("Data Ready RX FIFO interrupt\n");
		if (val & B_STATUS_TX_DS)
			printf("Data Sent TX FIFO interrupt\n");
		if (val & B_STATUS_MAX_RT)
			printf("Maximum TX retries interrupt\n");
		rxpno = val & M_STATUS_RX_P_NO;
		rxpno >>= 1;
		if (rxpno == 7) {
			printf("RX FIFO empty\n");
		} else {
			printf("Available payload in pipe %d.", rxpno);
		}
		break;
	case R_FIFO_STATUS:
		printf("R_FIFO_STATUS: 0x%hhx\n", val);
		if (val & B_FIFO_STATUS_TX_FULL)
			printf("TX_FULL ");
		if (val & B_FIFO_STATUS_TX_EMPTY)
			printf("TX_EMPTY ");
		if (val & B_FIFO_STATUS_RX_FULL)
			printf("RX_FULL ");
		if (val & B_FIFO_STATUS_RX_EMPTY)
			printf("RX_EMPTY ");
		if (val)
			printf("\n");
		
		break;
	default:
		break;
	}
	printf("----------\n");
}

void
RFM70::set_power_level(byte level)
{
	if (level < V_RF_SETUP_PWR_N)
		write(R_RF_SETUP,
		    (read(R_RF_SETUP) & ~M_RF_SETUP_RF_PWR) | (level < 1));
}

