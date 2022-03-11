 // Copyright (c) 2014, 2022 Robert Harris.

#ifndef	RFM70_H
#define	RFM70_H

typedef unsigned char byte;

class RFM70
{
	public:
		RFM70(byte slaveselect, byte ce, byte irq);
		void begin(void);
		void config_magic(void);
		byte command(byte command, byte address, byte *data, byte len);
		void read(byte reg, byte *data, byte len);
		byte read(byte reg);
		void write(byte reg, byte *data, byte len);
		void write(byte reg, byte data);
		void write(byte reg, byte d0, byte d1, byte d2, byte d3);
		void write(byte reg, byte d0, byte d1, byte d2, byte d3, byte d4);
		void write(byte reg, byte d0, byte d1, byte d2, byte d3, byte d4,
		    byte d5, byte d6, byte d7, byte d8, byte d9, byte d10);
		void set_mode(int mode);
		void dump_reg(byte reg, byte val);
		void set_power_level(byte level);
	private:
		byte _slaveselect;
		byte _ce;
		byte _irq;
		byte _bank;
};

#endif	// RFM70_H
