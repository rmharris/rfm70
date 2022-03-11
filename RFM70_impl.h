// Copyright (c) 2014, 2022 Robert Harris.

#ifndef RFM70_IMPL_H
#define RFM70_IMPL_H

/* RMF70 SPI commands. */
#define	C_R_REGISTER		0x00
#define	C_W_REGISTER		0x20
#define	C_R_RX_PAYLOAD		0x61
#define	C_W_TX_PAYLOAD		0xa0
#define	C_FLUSH_TX		0xe1
#define	C_FLUSH_RX		0xe2
#define	C_REUSE_TX_PL		0xe3
#define	C_ACTIVATE		0x50
#define	C_R_RX_PL_WID		0x60
#define	C_W_ACK_PAYLOAD		0xa8
#define	C_W_TX_PAYLOAD_NO_ACK	0xb0
#define	C_NOP			0xff

/* Sub commands for C_ACTIVATE. */
#define	ACTIVATE_TOGGLE		0x53
#define	ACTIVATE_FEATURES	0x73

/* Modes representing the various loci in the state machine. */
#define	MODE_POWER_DOWN		1
#define	MODE_STANDBY_ONE	2
#define	MODE_RX			3
#define	MODE_TX			4

#define	MAX_PACKET_LEN		32

/*
 * The RFM70 has two banks of registers, and the client must switch between
 * them as required. We conceal this detail from the user by setting
 * BANK_FLAG for register addresses in bank 1 and looking for this bit in
 * command().
 */
#define	BANK_FLAG		0x80
#define	BANK_UNINITIALISED	0xff

/*
 * R_<name>	The register with the mnemonic <name>.
 * B_<name>	Bits within R_<name>.
 * M_<name>	Masks of bits with R_<name>.
 * V_<name>	Particular values of R_<name>.
 */
#define	R_CONFIG		0x00
#define	B_CONFIG_MASK_RX_DR	0x40
#define	B_CONFIG_MASK_TX_DS	0x20
#define	B_CONFIG_MASK_MAX_RT	0x10
#define	B_CONFIG_EN_CRC		0x08
#define	B_CONFIG_CRCO		0x04
#define	B_CONFIG_PWR_UP		0x02
#define	B_CONFIG_PRIM_RX	0x01
#define	V_CONFIG_ALL		0x7f

#define	R_EN_AA			0x01
#define	B_EN_AA_P5		0x20
#define	B_EN_AA_P4		0x10
#define	B_EN_AA_P3		0x08
#define	B_EN_AA_P2		0x04
#define	B_EN_AA_P1		0x02
#define	B_EN_AA_P0		0x01
#define	V_EN_AA_NONE		0x00
#define	V_EN_AA_ALL		0x3f

#define	R_EN_RXADDR		0x02
#define	B_EN_RXADDR_P5		0x20
#define	B_EN_RXADDR_P4		0x10
#define	B_EN_RXADDR_P3		0x08
#define	B_EN_RXADDR_P2		0x04
#define	B_EN_RXADDR_P1		0x02
#define	B_EN_RXADDR_P0		0x01
#define	V_EN_RXADDR_ALL		0x3f

#define	R_SETUP_AW		0x03
#define	V_SETUP_AW_3		0x01
#define V_SETUP_AW_4		0x02
#define V_SETUP_AW_5		0x03
#define	M_SETUP_AW_AW		0x03

#define	R_SETUP_RETR		0x04
#define	V_SETUP_RETR_ARD_250	0x00
#define	V_SETUP_RETR_ARD_500	0x10
#define	V_SETUP_RETR_ARD_750	0x20
#define	V_SETUP_RETR_ARD_1000	0x30
#define	V_SETUP_RETR_ARD_1250	0x40
#define	V_SETUP_RETR_ARD_1500	0x50
#define	V_SETUP_RETR_ARD_1750	0x60
#define	V_SETUP_RETR_ARD_2000	0x70
#define	V_SETUP_RETR_ARD_2250	0x80
#define	V_SETUP_RETR_ARD_2500	0x90
#define	V_SETUP_RETR_ARD_2750	0xa0
#define	V_SETUP_RETR_ARD_3000	0xb0
#define	V_SETUP_RETR_ARD_3250	0xc0
#define	V_SETUP_RETR_ARD_3500	0xd0
#define	V_SETUP_RETR_ARD_3750	0xe0
#define	V_SETUP_RETR_ARD_4000	0xf0
#define	V_SETUP_RETR_ARC_DIS	0x00
#define	V_SETUP_RETR_ARC_1	0x01
#define	V_SETUP_RETR_ARC_2	0x02
#define	V_SETUP_RETR_ARC_3	0x03
#define	V_SETUP_RETR_ARC_4	0x04
#define	V_SETUP_RETR_ARC_5	0x05
#define	V_SETUP_RETR_ARC_6	0x06
#define	V_SETUP_RETR_ARC_7	0x07
#define	V_SETUP_RETR_ARC_8	0x08
#define	V_SETUP_RETR_ARC_9	0x09
#define	V_SETUP_RETR_ARC_10	0x0a
#define	V_SETUP_RETR_ARC_11	0x0b
#define	V_SETUP_RETR_ARC_12	0x0c
#define	V_SETUP_RETR_ARC_13	0x0d
#define	V_SETUP_RETR_ARC_14	0x0e
#define	V_SETUP_RETR_ARC_15	0x0f

#define	R_RF_CH			0x05
#define	R_RF_CH_RF_CH		0x7f

#define	R_RF_SETUP		0x06
#define	B_RF_SETUP_RF_DR	0x08
#define	B_RF_SETUP_LNA_HCURR	0x01
#ifndef	nRF24L01
#define	V_RF_SETUP_MAGIC	0x10
#else
#define V_RF_SETUP_MAGIC        0x00
#endif
#define	V_RF_SETUP_DR_1MBPS	0x00
#define	V_RF_SETUP_DR_2MBPS	0x08
#define	V_RF_SETUP_PWR_m10DBM	0x00
#define	V_RF_SETUP_PWR_m5DBM	0x02
#define	V_RF_SETUP_PWR_0DBM	0x04
#define	V_RF_SETUP_PWR_5DBM	0x06
#define	V_RF_SETUP_PWR_N	4
#define	V_RF_SETUP_LNA_LOW	0x00
#define	V_RF_SETUP_LNA_HIGH	0x01
#define	M_RF_SETUP_RF_PWR	0x06

#define	R_STATUS		0x07
#define	B_STATUS_RBANK		0x80
#define	B_STATUS_RX_DR		0x40
#define	B_STATUS_TX_DS		0x20
#define	B_STATUS_MAX_RT		0x10
#define	B_STATUS_TX_FULL	0x01
#define	V_STATUS_RX_P_NO_0	0x00
#define	V_STATUS_RX_P_NO_1	0x02
#define	V_STATUS_RX_P_NO_2	0x04
#define	V_STATUS_RX_P_NO_3	0x06
#define	V_STATUS_RX_P_NO_4	0x08
#define	V_STATUS_RX_P_NO_5	0x0a
#define	V_STATUS_RX_FIFO_EMPTY	0x0e
#define	M_STATUS_RX_P_NO	0x0e

#define	R_OBSERVE_TX		0x08
#define	M_OBSERVE_TX_PLOS_CNT	0xf0
#define	M_OBSERVE_TX_ARC_CNT	0x0f

#define	R_CD			0x09
#define	B_CD_CD			0x01

#define	R_RX_ADDR_P0		0x0a

#define	R_RX_ADDR_P1		0x0b

#define	R_RX_ADDR_P2		0x0c

#define	R_RX_ADDR_P3		0x0d

#define	R_RX_ADDR_P4		0x0e

#define	R_RX_ADDR_P5		0x0f

#define	R_TX_ADDR		0x10

#define	R_RX_PW_P0		0x11
#define	M_RX_PW_P0		0x3f

#define	R_RX_PW_P1		0x12
#define	M_RX_PW_P1		0x3f

#define	R_RX_PW_P2		0x13
#define	M_RX_PW_P2		0x3f

#define	R_RX_PW_P3		0x14
#define	M_RX_PW_P3		0x3f

#define	R_RX_PW_P4		0x15
#define	M_RX_PW_P4		0x3f

#define	R_RX_PW_P5		0x16
#define	M_RX_PW_P5		0x3f

#define	R_FIFO_STATUS		0x17
#define B_FIFO_STATUS_TX_REUSE	0x40
#define B_FIFO_STATUS_TX_FULL	0x20
#define B_FIFO_STATUS_TX_EMPTY	0x10
#define B_FIFO_STATUS_RX_FULL	0x02
#define B_FIFO_STATUS_RX_EMPTY	0x01

#define	R_DYNPD			0x1c
#define	B_DYNPD_DPL_P0		0x01
#define	B_DYNPD_DPL_P1		0x02
#define	B_DYNPD_DPL_P2		0x04
#define	B_DYNPD_DPL_P3		0x08
#define	B_DYNPD_DPL_P4		0x10
#define	B_DYNPD_DPL_P5		0x20
#define	V_DYNPD_DPL_ALL		0x3f

#define	R_FEATURE		0x1d
#define B_EN_DPL		0x04
#define	B_EN_ACK_PAY		0x02
#define	B_EN_DYN_ACK		0x01

#define	R_RB1_00		0x80

#define	R_RB1_01		0x81

#define	R_RB1_02		0x82

#define	R_RB1_03		0x83

#define	R_RB1_04		0x84

#define	R_RB1_05		0x85

#define	R_RB1_06		0x86

#define	R_RB1_07		0x87

#define	R_CHIP_ID		0x88
#define	R_CHIP_ID_RFM70_ID	0x63

#define	R_RB1_09		0x89

#define	R_RB1_0A		0x8a

#define	R_RB1_0B		0x8b

#define	R_RB1_0C		0x8c

#define	R_NEW_FEATURE		0x8d

#define	R_RAMP			0x8e

#endif // RFM70_IMPL_H
