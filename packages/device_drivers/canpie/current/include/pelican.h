#ifndef PELICAN_H
#define PELICAN_H

#include "lvd_can_mod.h"
#include <asm/io.h>

/************************************************************************/
/*																		*/
/*																		*/
/* 			COMPACT INCLUDE FILE FOR PELICAN-MODE 					    */
/*																		*/
/*			Cedric Herreman  05/01/01									*/
/*																		*/
/*																		*/
/************************************************************************/

/* oscillator frequence and interrupt line */
#define OSC_16MHZ               /* def for can timing */
#define CAN_IRQ_LINE		7

/* Return values (BYTE_t) */
#define SUCCESS		   0
#define FAILURE		0xff

/* Extract bytes from word */
#define word_to_lsb(w)	((BYTE_t)((w)&0x00ff))
#define word_to_msb(w)	((BYTE_t)((w)>>8))

/*----------------------------------------------------------------------*/
/* Definitions of registers												*/
/*----------------------------------------------------------------------*/
/* 82C200 and SJA1000 in BasicCAN Mode */

/* PCA82C200 Address Allocation */
#define BCAN_CR		 0	/* control register */
#define BCAN_CMR	 1	/* command register */
#define BCAN_SR		 2	/* status register */
#define BCAN_IR		 3	/* interrupt register */
#define BCAN_AC		 4	/* acceptance code register */
#define BCAN_AM		 5	/* acceptance mask register */
#define BCAN_BT0	 6	/* bus timing register 0 */
#define BCAN_BT1	 7	/* bus timing register 1 */
#define BCAN_OCR	 8	/* output control register */
#define BCAN_TDESC1	10	/* first descriptor transmit buffer */
#define BCAN_TDESC2	11	/* second descriptor transmit buffer */
#define BCAN_TDATA	12	/* start data transmit buffer */
#define BCAN_RDESC1	20	/* first descriptor receive buffer */
#define BCAN_RDESC2	21	/* second descriptor receive buffer */
#define BCAN_RDATA	22	/* start data receive buffer */
#define BCAN_CDR	31	/* clock divider */

/* Control Register Bits */
#define BCAN_CR_OIE	0x10	/* Overrun Interrupt Enable */
#define BCAN_CR_EIE	0x08	/* Error Interrupt Enable */
#define BCAN_CR_TIE	0x04	/* Transmit Interrupt Enable */
#define BCAN_CR_RIE	0x02	/* Receive Interrupt Enable */
#define BCAN_CR_RR 	0x01	/* Reset Request */

/* Command Register Bits */
#define BCAN_CMR_GTS	0x10	/* Goto Sleep */
#define BCAN_CMR_COS	0x08	/* Clear Overrun Status */
#define BCAN_CMR_RRB	0x04	/* Release Receive Buffer */
#define BCAN_CMR_AT	0x02	/* Abort Transmission */
#define BCAN_CMR_TR	0x01	/* Transmission Request */

/* Status Register Bits */
#define BCAN_SR_BS	0x80	/* Bus Status */
#define BCAN_SR_ES	0x40	/* Error Status */
#define BCAN_SR_TS	0x20	/* Transmit Status */
#define BCAN_SR_RS	0x10	/* Receive Status */
#define BCAN_SR_TCS	0x08	/* Transmission Complete Status */
#define BCAN_SR_TBS	0x04	/* Transmit Buffer Status */
#define BCAN_SR_DO	0x02	/* Data Overrun */
#define BCAN_SR_RBS	0x01	/* Receive Buffer Status */

/* Interrupt Register Bits */
#define BCAN_IR_WUI	0x10	/* Wake-Up Interrupt */
#define BCAN_IR_OI	0x08	/* Overrun Interrupt */
#define BCAN_IR_EI	0x04	/* Error Interrupt */
#define BCAN_IR_TI	0x02	/* Transmit Interrupt */
#define BCAN_IR_RI	0x01	/* Receive Interrupt */


/* SJA1000 in PeliCAN mode */

/* PeliCAN mode address allocation */
#define PCAN_MODR        0      /* Mode register (rw) */
#define PCAN_CMR         1	/* Command register (wo) */
#define PCAN_SR          2      /* Status register (ro) */
#define PCAN_IR          3	/* Interrupt register (ro) */
#define PCAN_IER         4      /* Interrupt enable register (rw) */
#define PCAN_BTR0        6	/* Bus timing register 0 (ro, rw) */
#define PCAN_BTR1        7	/* Bus timing register 1 (ro, rw) */
#define PCAN_OCR         8	/* Output control register 1 (ro, rw) */
#define PCAN_TESTR       9	/* Test register */
#define PCAN_ALCR       11	/* Arbitration lost capture reg (ro) */
#define PCAN_ECCR       12      /* Error code capture register (ro) */
#define PCAN_EWLR       13      /* Error warning limit register (ro, rw) */
#define PCAN_RXERR      14	/* Rx error counter register (ro, rw) */
#define PCAN_TXERR      15      /* Tx error counter register (ro, rw) */
#define PCAN_ACR0	16	/* acceptance code register 0 (-, rw) */
#define PCAN_ACR1	17	/* acceptance code register 1 (-, rw) */
#define PCAN_ACR2	18	/* acceptance code register 2 (-, rw) */
#define PCAN_ACR3	19	/* acceptance code register 3 (-, rw) */
#define PCAN_AMR0	20	/* acceptance mask register 0 (-, rw) */
#define PCAN_AMR1	21	/* acceptance mask register 1 (-, rw) */
#define PCAN_AMR2	22	/* acceptance mask register 2 (-, rw) */
#define PCAN_AMR3	23	/* acceptance mask register 3 (-, rw) */
#define PCAN_RXFI       16      /* Rx Frame info   SFF, EFF (ro, -) */
#define PCAN_RXID1      17      /* Rx Identifier 1 SFF, EFF (ro, -) */
#define PCAN_RXID2      18      /* Rx Identifier 2 SFF, EFF (ro, -) */
#define PCAN_RXID3      19      /* Rx Identifier 3      EFF (ro, -) */
#define PCAN_RXID4      20      /* Rx Identifier 4      EFF (ro, -) */
#define PCAN_RXSFFD     19      /* Rx standard frame data   (ro, -) */
#define PCAN_RXEFFD     21      /* Rx extended frame data   (ro, -) */
#define PCAN_TXFI       16      /* Tx Frame info   SFF, EFF (wo, -) */
#define PCAN_TXID1      17      /* Tx Identifier 1 SFF, EFF (wo, -) */
#define PCAN_TXID2      18      /* Tx Identifier 2 SFF, EFF (wo, -) */
#define PCAN_TXID3      19      /* Tx Identifier 3      EFF (wo, -) */
#define PCAN_TXID4      20      /* Tx Identifier 4      EFF (wo, -) */
#define PCAN_TXSFFD     19      /* Tx standard frame data   (wo, -) */
#define PCAN_TXEFFD     21      /* Tx extended frame data   (wo, -) */
#define PCAN_RXMCR      29	/* Rx message counter register (ro) */
#define PCAN_RXBSAR     30	/* Rx buffer start address register (ro, rw) */
#define PCAN_CDR	31	/* Clock divider register ('rw', rw)*/

/* Mode Register Bits */
#define PCAN_MODR_SM    (1<<4)	/* Sleep mode */
#define PCAN_MODR_AFM   (1<<3)	/* Acceptance filter mode */
#define PCAN_MODR_STM   (1<<2)	/* Self test mode */
#define PCAN_MODR_LOM   (1<<1)	/* Listen only mode */
#define PCAN_MODR_RM    (1<<0)	/* Reset mode */

/* Command Register Bits */
#define PCAN_CMR_SRR	(1<<4)	/* Self reception request */
#define PCAN_CMR_CDO	(1<<3)	/* Clear data overrun */
#define PCAN_CMR_RRB	(1<<2)	/* Release receive buffer */
#define PCAN_CMR_AT	(1<<1)	/* Abort transmission */
#define PCAN_CMR_TR	(1<<0)	/* Transmission request */

/* Status Register Bits */
#define PCAN_SR_BS	(1<<7)	/* Bus status */
#define PCAN_SR_ES	(1<<6)	/* Error status */
#define PCAN_SR_TS	(1<<5)	/* Transmit status */
#define PCAN_SR_RS	(1<<4)	/* Receive status */
#define PCAN_SR_TCS	(1<<3)	/* Transmission complete status */
#define PCAN_SR_TBS	(1<<2)	/* Transmit buffer status */
#define PCAN_SR_DOS	(1<<1)	/* Data overrun status */
#define PCAN_SR_RBS	(1<<0)	/* Receive buffer status */

/* Interrupt Register Bits */
#define PCAN_IR_BEI	(1<<7)	/* Bus-eror interrupt */
#define PCAN_IR_ALI	(1<<6)	/* Arbitration lost interrupt */
#define PCAN_IR_EPI	(1<<5)	/* Error-passive interrupt */
#define PCAN_IR_WUI	(1<<4)	/* Wake-up interrupt */
#define PCAN_IR_DOI	(1<<3)	/* Data-overrun interrupt */
#define PCAN_IR_EI	(1<<2)	/* Error interrupt */
#define PCAN_IR_TI	(1<<1)	/* Transmit interrupt */
#define PCAN_IR_RI	(1<<0)	/* Receive interrupt */

/* Interrupt enable register bits */
#define PCAN_IER_BEIE	(1<<7)	/* Bus-eror interrupt enable */
#define PCAN_IER_ALIE	(1<<6)	/* Arbitration lost interrupt enable */
#define PCAN_IER_EPIE	(1<<5)	/* Error-passive interrupt enable */
#define PCAN_IER_WUIE	(1<<4)	/* Wake-up interrupt enable */
#define PCAN_IER_DOIE	(1<<3)	/* Data-overrun interrupt enable */
#define PCAN_IER_EIE	(1<<2)	/* Error warning interrupt enable */
#define PCAN_IER_TIE	(1<<1)	/* Transmit interrupt enable */
#define PCAN_IER_RIE	(1<<0)	/* Receive interrupt enable */

/* Clock divider register bits */
#define PCAN_CDR_PCAN	(1<<7)	/* Enable PCAN mode */
#define PCAN_CDR_CBP	(1<<6)	/* Enable Rx input comparator bypass */
#define PCAN_CDR_RXINTEN (1<<5)	/* Enable RXINT output at TX1 */
#define PCAN_CDR_CLKOFF	(1<<3)	/* Disable clock output */

/* Bit definitions for Rx/Tx Frame info */
#define PCAN_FINFO_FF	(1<<7)	/* Frame format bit */
#define PCAN_FINFO_EFF	(1<<7)	/* Extended frame indication */
#define PCAN_FINFO_RTR	(1<<6)	/* RTR frame bit */
#define PCAN_FINFO_DLC_MASK (0x0f) /* Data length code mask */


/*---------------------------------------------------------------------*/
/* BasicCAN, PeliCAN common definitions                                */
/*---------------------------------------------------------------------*/

/* Values of acceptance code/mask registers */
#define ACM_ALL	word_from_bytes(0x00,0xff)	/* accept all ids */

/* Values of output control register */
#define OCR_PUSHPULL	0xfa	/* push/pull */

/* Admissable bus timing values (BTR1=msb, BTR0=lsb) */
#ifdef OSC_16MHZ		/* if using a 16 MHz oscillator */
#define BTR_1MB		0x2300	/* 1 MBaud */
#if 0
#define BTR_1MB		0x1400	/* 1 MBaud */	 /* The better value */
#endif
#define BTR_500KB	0x1c00	/* 500 KBaud */
#define BTR_250KB	0x1c01	/* 250 KBaud */
#define BTR_125KB	0x1c03	/* 125 KBaud */
#define BTR_100KB	0x34c7	/* 100 KBaud */
#define BTR_50KB	0x34cf	/*  50 KBaud */
#define BTR_20KB	0x7fcf	/*  20 KBaud */
#endif

/* Admissable values in clock divider register */
#ifdef OSC_16MHZ		/* if using a 16 MHz oscillator */
#define CDR_16MHZ	0x07	/* 16 MHz output */
#define CDR_8MHZ	0x00	/* 8 MHz output (division by 2) */
#define CDR_4MHZ	0x01	/* 4 MHz output (division by 4) */
#define CDR_2MHZ	0x03	/* 2 MHz output (division by 8) */
#endif


/*---------------------------------------------------------------------*/
/* General Definitions                                                 */
/*---------------------------------------------------------------------*/
/*
 * Register access
 */
#define REG8(addr)  *((volatile unsigned char * const) (addr))
#define REG16(addr) *((volatile unsigned short * const) (addr))
#define REG32(addr) *((volatile unsigned long * const) (addr))


/*----------------------------------------------------------------------*/
/* Address map definition                                               */
/*----------------------------------------------------------------------*/
#define BCAN_AS 0x100   /* hardware  IO address of PHYTEC CAN 0*/

/*----------------------------------------------------------------------*/
/* PCA82C200 Basic CAN controller (BCAN)--write to/read from registers  */								
/*---------------------------------------------------------------------- */

/* Location of BCAN registers in external memory */
#define BCANREG(a)	(XADDR_t)(a)	/* address BCAN register */

#define read_reg_bcan(reg) inb(BCAN_AS | reg)

#define write_reg_bcan(reg,val) outb(val,(BCAN_AS | reg))

/* Read data bytes from BCAN receive buffer into memory */
#define read_data_bcan(to, from, len) for (;(len)>0;(len)--,(from)++) *(to)++ = read_reg_bcan((from))

/* Write data bytes from memory into BCAN transmit buffer */
#define write_data_bcan(to, from, len) for (;(len)>0;(len)--,(to)++,(from)++) write_reg_bcan((to), *(from))

/*----------------------------------------------------------------------*/
/* Message specifiers													*/
/*----------------------------------------------------------------------*/

#define M_TIMER_START_req	0x20  /* start timer request */
#define M_TIMER_STOP_req	0x21  /* stop timer request */
#define M_TIMER_SET_req		0x22  /* set timer request */
#define M_TIMER_SET_con		0x23  /* confirm setting of timer */
#define M_TIMER_TO_ind		0x24  /* indicate timeout event */
#define M_TIMER_CLEAR_req	0x25  /* clear timer request */
#define M_BCAN_TX_req		0x30  /* transmit request */
#define M_BCAN_TXC_req		0x31  /* transmit request with pos.conf. */
#define M_BCAN_TXE_req		0x32  /* transmit request with echo */
#define M_BCAN_TXCE_req		0x33  /* transmit request w.con(+), echo */
#define M_BCAN_TXAT_req		0x34  /* abort transmission request */
#define M_BCAN_TX_con		0x35  /* confirm (+/-) transmission */
#define M_BCAN_RX_ind		0x36  /* receive indication */
#define M_BCAN_EVENT_ind	0x37  /* BCAN event (error, overrun) */
#define M_BCAN_SET_ACM_req	0x40  /* set acceptance code and mask */
#define M_BCAN_SET_BTR_req	0x41  /* set bus timing parameters */
#define M_BCAN_BUSOFF_req	0x42  /* switch to bus-off state */
#define M_BCAN_BUSON_req	0x43  /* switch to bus-on state */
#define M_BCAN_SETREG_req	0x44  /* set BCAN registers request */
#define M_BCAN_GETREG_req	0x45  /* get BCAN registers request */
#define M_BCAN_GETREG_con	0x46  /* get BCAN registers confirmation */
#define M_BCAN_CONF_req		0x47  /* Special CANbus configurations */
#define M_AFIL_OPEN_req		0x50  /* accept CAN-Id */
#define M_AFIL_ROPEN_req	0x51  /* accept CAN-Id range */
#define M_AFIL_CLOSE_req	0x52  /* reject CAN-Id */
#define M_AFIL_RCLOSE_req	0x53  /* reject CAN-Id range */
#define	M_CYC_LIST_CREATE_req	0x60  /* cr. a list of cycl. CANbus messages */
#define	M_CYC_LIST_CREATE_con	0x61  /* cr. a list of cycl. CANbus messages */
#define	M_CYC_LIST_DELETE_req	0x62  /* del. a list of cycl. CANbus messages */
#define	M_CYC_LIST_DELETE_con	0x63  /* del. a list of cycl. CANbus messages */
#define M_CYC_MSG_INSERT_req	0x64  /* insert a message in cyc. send list */
#define M_CYC_MSG_INSERT_con	0x65  /* insert a message in cyc. send list */

/* Function for setting bus timing registers */
void write_btr_bcan(WORD_t btr)
{
	BYTE_t b;

	b = word_to_lsb(btr);
	write_reg_bcan(PCAN_BTR0,b);
	b = word_to_msb(btr);
	write_reg_bcan(PCAN_BTR1,b);
}

#endif
