/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:16 CET 2005  sja_can_defs.h 

                        sja_can_defs.h -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
/************************************************************************/
/*																		*/
/*																		*/
/* 			INCLUDE FILE FOR SJA CAN COMMUNICATION 					    */
/*																		*/
/*			Cedric Herreman  18/06/02									*/
/*																		*/
/*																		*/
/************************************************************************/

/*----------------------------------------------------------------------*/
/* Basic Data Types														*/
/*----------------------------------------------------------------------*/
#define BYTE_t unsigned char
#define WORD_t unsigned short
#define LWORD_t unsigned long
/* Data types of external memory addresses and of CAN message data length */
#define	  XADDR_t  	unsigned char	 /* index for latching */
#define   DLEN_t	int		/* CAN message data length type */

/*----------------------------------------------------------------------*/
/* Message Data Format													*/
/*----------------------------------------------------------------------*/

/* Receive and Transmit buffer size */
#define R_BUFFERSIZE	20
#define S_BUFFERSIZE	20

/* Message frame format */
#define EFF_MESSAGE		(1<<7)

/* Result bit : CiA/DS202-2 */
#define FAIL_BIT  	    (1<<7)

/* Mode or format type of a message */
#define MMODE_LOCAL		0x01	/* internal compiler specific format */
#define MMODE_TRANSFER		0x02	/* external format for transfer */

/* Maximum length definitions */
#define BCAN_LEN	 8	/* max data length in CAN message */
#define MB_LEN		16	/* max data length in bounded sized message */

/* Message object definition */
typedef struct  
{
	BYTE_t		spec;	/* message contents specifier */
	BYTE_t		frame_info;	/* mode (message format type) */
	WORD_t		r;		/* request id */
	DLEN_t 		len;
	unsigned int desc;		/* descriptor */
	BYTE_t		data[BCAN_LEN]; /* data bytes  */
} ttv_message;

/* Message buffer definition */
typedef struct
{
	int R_Free;
	int R_Next;
	int S_Free;
	int S_Next;
	ttv_message Rec_Message[R_BUFFERSIZE];
	ttv_message Snd_Message[S_BUFFERSIZE];
} ttv_can_data;  

/* shared memory name */
#define SHMEM_CAN 	    0xAA01


#define NIL_MESSAGE	(struct message *)0

/* frame info byte */
#define EFF_RTR			0xC0
#define EFF_0BYTE		0x80
#define EFF_1BYTE		0x81
#define EFF_2BYTE		0x82
#define EFF_3BYTE		0x83
#define EFF_4BYTE		0x84
#define EFF_5BYTE		0x85
#define EFF_6BYTE		0x86
#define EFF_7BYTE		0x87
#define EFF_8BYTE		0x88

#define SFF_RTR			0x40
#define SFF_0BYTE		0x00
#define SFF_1BYTE		0x01
#define SFF_2BYTE		0x02
#define SFF_3BYTE		0x03
#define SFF_4BYTE		0x04
#define SFF_5BYTE		0x05
#define SFF_6BYTE		0x06
#define SFF_7BYTE		0x07
#define SFF_8BYTE		0x08
