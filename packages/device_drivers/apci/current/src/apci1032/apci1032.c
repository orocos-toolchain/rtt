/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:16 CET 2005  apci1032.c 

                        apci1032.c -  description
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
 
 
/*
 +-----------------------------------------------------------------------+
 | (C) ADDI-DATA GmbH          Dieselstrasse 3      D-77833 Ottersweier  |
 +-----------------------------------------------------------------------+
 | Tel : +49 (0) 7223/9493-0     | email    : info@addi-data.com         |
 | Fax : +49 (0) 7223/9493-92    | Internet : http://www.addi-data.com   |
 +-----------------------------------------------------------------------+
 | Project   : API APCI1032      | Compiler : GCC                        |
 | Modulname : APCI1032.C        | Version  : 2.96 (SuSE 7.1 - 2.2.18)   |
 +-------------------------------+---------------------------------------+
 | Author    : STOLZ, Eric       | Date     : 21.10.2001                 |
 +-----------------------------------------------------------------------+
 |                   Description : APCI1032 main module                  |
 +-----------------------------------------------------------------------+
 |                             UPDATE'S                                  |
 +-----------------------------------------------------------------------+
 |   Date   |   Author  |          Description of updates                |
 +----------+-----------+------------------------------------------------+
 |          |           |                                                |
 +-----------------------------------------------------------------------+
*/

#define DriverName "APCI1032"

#include "apci1032.h"
#include <linux/module.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#ifndef CONFIG_PCI
#   error "This driver needs PCI support to be available"
#endif


/*MODULE_PARM(video_nr, "i");*/
MODULE_DESCRIPTION( "APCI-1032 driver" );
MODULE_LICENSE("GPL");

/* Prototypes of the used functions */

int APCI1032_open ( struct inode *inode, struct file *filp );

int APCI1032_release( struct inode *inode, struct file *filp );

int APCI1032_ioctl ( struct inode *inode,

                     struct file *filp,
                     unsigned int cmd,
                     unsigned long arg );


/* Global declarations */
int i_MajorNumber; /* Variable for the used Minor number of the device (board) */
int i_MinorNumber; /* Variable for the used Major number of the device (board) */

/* Hardware board information structure */
str_APCI1032_DriverStructure s_APCI1032_DriverStructure;

str_BoardInformations *BoardInformations;

struct file_operations APCI1032_fops =
    {
        .ioctl = APCI1032_ioctl,
        .open = APCI1032_open,
        .release = APCI1032_release
    };

int APCI1032_open ( struct inode *inode, struct file *filp )
{
    /* Saves the Minor number */
    i_MinorNumber = MINOR( inode->i_rdev );
    /* Increases the using number of the driver by 1 */
    //MOD_INC_USE_COUNT;
    return 0;
}

int APCI1032_release ( struct inode *inode, struct file *filp )
{
    /* Free the IRQ from the board */
    free_irq ( s_APCI1032_DriverStructure.
               s_BoardInformations[ MINOR( inode->i_rdev ) ].
               s_BaseInformations.b_Interrupt,
               BoardInformations );
    /* Decreases the using number of the driver by 1 */
    //MOD_DEC_USE_COUNT;
    return 0;
}

int i_APCI1032_CheckAndGetPCISlotNumber( unsigned char *slotarray )
{
    int i;
    //unsigned char slotarray[APCI1032_MAX_BOARD_NBR+1];

    for ( i = 0; i < s_APCI1032_DriverStructure.b_NumberOfBoard; i++ )
    {
        slotarray[ i ] = s_APCI1032_DriverStructure.
                         s_BoardInformations[ i ].
                         s_BaseInformations.
                         b_SlotNumber;
    }

    slotarray[ APCI1032_MAX_BOARD_NBR ] = i_MajorNumber;
    return s_APCI1032_DriverStructure.b_NumberOfBoard;
}

int i_APCI1032_SetBoardInformation( unsigned char b_SlotNumber, int * pi_BoardHandle )
{
    /* This function can be used to set parameters and */
    /* initializes the board to a definite status      */
    return 0;
}

int i_APCI1032_GetHardwareInformation( int i_BoardHandle, unsigned int * pui_BaseAddress, unsigned char * pb_InterruptNbr, unsigned char * pb_SlotNumber )
{
    return 0;
}

int i_APCI1032_CloseBoardHandle( int i_BoardHandle )
{
    return 0;
}

int i_APCI1032_Read1DigitalInput( int i_BoardHandle, unsigned char b_Channel, unsigned long * pb_ChannelValue )
{
    unsigned long ul_InputValue;
    BoardInformations = &s_APCI1032_DriverStructure.s_BoardInformations[ i_BoardHandle ];

    if ( BoardInformations )
    {
        /* Reads the input value from the I/O Base Address */
        ul_InputValue = inl( BoardInformations->s_BaseInformations.ui_BaseAddress[ 1 ] );
        /* Shifts the input value for the number of channels right */
        /* and masks the leaving input value with 0x1              */
        ul_InputValue = ( ul_InputValue >> ( b_Channel - 1 ) ) & 0x1;
        /* Puts the modified input value to the user */
        *pb_ChannelValue = ul_InputValue;
        return 0;
    }

    return -1;
}

int i_APCI1032_Read8DigitalInput( int i_BoardHandle, unsigned char b_Port, unsigned long * pb_PortValue )
{
    unsigned long ul_InputValue;
    BoardInformations = &s_APCI1032_DriverStructure.s_BoardInformations[ i_BoardHandle ];

    if ( BoardInformations )
    {
        /* Reads the input value from the I/O Base Address */
        ul_InputValue = inl( BoardInformations->s_BaseInformations.ui_BaseAddress[ 1 ] );

        /* Masks the input value with a for the number of b_Port */
        /* left shifted 0xFF mask to get only the wished inputs */
        ul_InputValue = ( ul_InputValue & ( 0xFF << ( 8 * ( b_Port - 1 ) ) ) );

        /* Shifts the modified ul_InputValue for the number */
        /* of b_Port right */
        ul_InputValue = ( ul_InputValue >> ( 8 * ( b_Port - 1 ) ) );

        *pb_PortValue = ul_InputValue;
        return 0;
    }

    return -1;
}

int i_APCI1032_Read16DigitalInput( int i_BoardHandle, unsigned char b_Port, unsigned long * pul_InputValue )
{
    unsigned long ul_InputValue;
    BoardInformations = &s_APCI1032_DriverStructure.s_BoardInformations[ i_BoardHandle ];

    if ( BoardInformations )
    {
        /* Reads the input value from the I/O Base Address */
        ul_InputValue = inl( BoardInformations->s_BaseInformations.ui_BaseAddress[ 1 ] );

        /* Masks the input value with a for the number of b_Port  */
        /* left shifted 0xFFFF mask to get only the wished inputs */
        ul_InputValue = ( ul_InputValue & ( 0xFFFF << ( 16 * ( b_Port - 1 ) ) ) );

        /* Shifts the modified ul_InputValue for the number */
        /* of b_Port right */
        ul_InputValue = ( ul_InputValue >> ( 16 * ( b_Port - 1 ) ) );

        *pul_InputValue = ul_InputValue;
        return 0;
    }

    return -1;
}

int i_APCI1032_Read32DigitalInput( int i_BoardHandle, unsigned long * pul_InputValue )
{
    unsigned long ul_InputValue;
    BoardInformations = &s_APCI1032_DriverStructure.s_BoardInformations[ i_BoardHandle ];

    if ( BoardInformations )
    {
        /* Reads the input value from the I/O Base Address */
        ul_InputValue = inl( BoardInformations->s_BaseInformations.ui_BaseAddress[ 1 ] );
        *pul_InputValue = ul_InputValue;
        return 0;
    }
    return -1;
}

/*---------------------------------------------------------------------------*/
/*              IOCTL functions of the module for the APCI-1032              */
/*---------------------------------------------------------------------------*/

int APCI1032_ioctl ( struct inode *inode,

                     struct file *filp,
                     unsigned int cmd,
                     unsigned long arg )
{
    unsigned char b_SlotArray[ APCI1032_MAX_BOARD_NBR + 1 ];
    int i = 0;  /* Simple counting variable */
    int i_ReturnValue = 0;  /* Saves the return value of the functions */
    unsigned char b_Port = 0;  /* Saves the port number to read */
    unsigned char b_Channel = 0;  /* Saves the channel number to read */
    unsigned long ul_InputValue = 0;  /* Saves and returns the input value */

    BoardInformations = &s_APCI1032_DriverStructure.
                        s_BoardInformations[ MINOR( inode->i_rdev ) ];

    switch ( cmd )
    {

        case CMD_APCI1032_CheckAndGetPCISlotNumber:
#if 0

            APCI1032_check_and_get_pci_slot_number( b_SlotArray );
#else

            for ( i = 0; i < s_APCI1032_DriverStructure.b_NumberOfBoard; i++ )
            {
                b_SlotArray[ i ] = s_APCI1032_DriverStructure.
                                   s_BoardInformations[ i ].
                                   s_BaseInformations.
                                   b_SlotNumber;
            }

            b_SlotArray[ APCI1032_MAX_BOARD_NBR ] = i_MajorNumber;
#endif

            copy_to_user ( ( unsigned char * ) arg,
                           b_SlotArray,
                           sizeof ( BYTE[ APCI1032_MAX_BOARD_NBR + 1 ] ) );
            i_ReturnValue = s_APCI1032_DriverStructure.b_NumberOfBoard;
            break;


        case CMD_APCI1032_SetBoardInformation:

            /* This function can be used to set parameters and */
            /* initializes the board to a definite status      */
            i_ReturnValue = 0;
            break;


        case CMD_APCI1032_GetHardwareInformation:

            /* Returns the informations of base address, IRQ */
            /* to the user */
            i_ReturnValue = copy_to_user
                            ( ( str_BaseInformations* ) arg,
                              &BoardInformations->s_BaseInformations,
                              sizeof( str_BaseInformations ) );
            break;


        case CMD_APCI1032_Read1DigitalInput:

            /* Reads the input value from the I/O Base Address */
            ul_InputValue = inl ( BoardInformations->s_BaseInformations.
                                  ui_BaseAddress[ 1 ] );
            /* Gets the b_Channel value from the user */
            get_user ( b_Channel, ( unsigned char * ) arg );
            /* Shifts the input value for the number of channels right */
            /* and masks the leaving input value with 0x1              */
            ul_InputValue = ( ul_InputValue >> ( b_Channel - 1 ) ) & 0x1;
            /* Puts the modified input value to the user */
            put_user ( ( unsigned char ) ul_InputValue, ( unsigned char * ) arg );

            i_ReturnValue = 0;
            break;


        case CMD_APCI1032_Read8DigitalInput:

            /* Reads the input value from the I/O Base Address */
            ul_InputValue = inl( BoardInformations->s_BaseInformations.ui_BaseAddress[ 1 ] );

            /* Gets the b_Port value from the user */
            get_user( b_Port, ( unsigned char * ) arg );

            /* Masks the input value with a for the number of b_Port */
            /* left shifted 0xFF mask to get only the wished inputs */
            ul_InputValue = ( ul_InputValue & ( 0xFF << ( 8 * ( b_Port - 1 ) ) ) );

            /* Shifts the modified ul_InputValue for the number */
            /* of b_Port right */
            ul_InputValue = ( ul_InputValue >> ( 8 * ( b_Port - 1 ) ) );

            /* Puts the modified input value to the user */
            put_user( ( unsigned char ) ul_InputValue, ( unsigned char * ) arg );

            i_ReturnValue = 0;
            break;


        case CMD_APCI1032_Read16DigitalInput:

            /* Reads the input value from the I/O Base Address */
            ul_InputValue = inl( BoardInformations->s_BaseInformations.ui_BaseAddress[ 1 ] );

            /* Gets the b_Port value from the user */
            get_user ( b_Port, ( unsigned char * ) arg );

            /* Masks the input value with a for the number of b_Port  */
            /* left shifted 0xFFFF mask to get only the wished inputs */
            ul_InputValue = ( ul_InputValue & ( 0xFFFF << ( 16 * ( b_Port - 1 ) ) ) );

            /* Shifts the modified ul_InputValue for the number */
            /* of b_Port right */
            ul_InputValue = ( ul_InputValue >> ( 16 * ( b_Port - 1 ) ) );

            /* Puts the modified input value to the user */
            put_user( ( unsigned long ) ul_InputValue, ( unsigned long * ) arg );

            i_ReturnValue = 0;
            break;

        case CMD_APCI1032_Read32DigitalInput:

            /* Reads the input value from the I/O Base Address */
            ul_InputValue = inl( BoardInformations->s_BaseInformations.ui_BaseAddress[ 1 ] );

            /* Puts the modified input value to the user */
            put_user( ( unsigned long ) ul_InputValue, ( unsigned long * ) arg );

            i_ReturnValue = 0;
            break;
    }

    return i_ReturnValue;
}

/*------------------------------------------------------*/
/* Initialisation of the APCI-1032 module in the kernel */
/*------------------------------------------------------*/

int init_module( void )
{
    int i = 0;

    /* Sets the pointer to NULL, neccessary for the querry */

    struct pci_dev *pPCILinuxStruct = NULL;

    /* Initializes the Number of boards with 0 */
    s_APCI1032_DriverStructure.b_NumberOfBoard = 0;

    printk( "\n +------------------------------+\n" );
    printk( " | ADDI-DATA GmbH -+- APCI-1032 |\n" );
    printk( " +------------------------------+\n" );
    printk( " |      Module installation     |\n" );
    printk( " +------------------------------+\n" );

    
    /* Registration of driver */
    i_MajorNumber = register_chrdev( 0, DriverName, &APCI1032_fops );

    if ( i_MajorNumber < 0 )
    {
        printk( "<1> Can't get major number.\n" );
        return i_MajorNumber;
    }

    /* Filling str_PCI_Information structure */
    printk( "PCI_BASE_ADDRESS_IO_MASK = %X\n", ( UINT ) PCI_BASE_ADDRESS_IO_MASK );

    while ( ( pPCILinuxStruct = pci_find_device( APCI1032_BOARD_VENDOR_ID,
                                APCI1032_BOARD_DEVICE_ID,
                                pPCILinuxStruct ) ) != 0 )
    {
        for ( i = 0;i < 5;i++ )
        {
            /* Gets all available base addresses and save it */
            s_APCI1032_DriverStructure.
            s_BoardInformations[ s_APCI1032_DriverStructure.b_NumberOfBoard ].
            s_BaseInformations.ui_BaseAddress[ i ] = ( UINT )
                    PCI_BASE_ADDRESS_IO_MASK & pci_resource_start( pPCILinuxStruct, i );
        }

        /* Saves the IRQ in the structure */
        s_APCI1032_DriverStructure.
        s_BoardInformations[ s_APCI1032_DriverStructure.b_NumberOfBoard ].
        s_BaseInformations.b_Interrupt =
            ( unsigned char ) pPCILinuxStruct->irq;

        /* Saves the PCI Bus number in the structure */
        s_APCI1032_DriverStructure.
        s_BoardInformations[ s_APCI1032_DriverStructure.b_NumberOfBoard ].
        s_BaseInformations.b_SlotNumber =
            ( unsigned char ) PCI_SLOT( pPCILinuxStruct->devfn );

        printk( " |      NumberOfBoard: %2d       |\n", s_APCI1032_DriverStructure.b_NumberOfBoard );

        printk( " +------------------------------+\n" );

        printk( " | Baseaddress[0,1,2]:          |\n" );

        printk( " |      %4lX, %4lX, %4lX        |\n", pci_resource_start( pPCILinuxStruct, 0 ), pci_resource_start( pPCILinuxStruct, 1 ), pci_resource_start( pPCILinuxStruct, 2 ) );
        printk( " | Interrupt No.:               |\n" );

        printk( " |     %2lX                       |\n", pPCILinuxStruct->irq );

        printk( " +------26.09.2001---09:29------+\n" );

        /* Increases the number of detected boards by one */
        s_APCI1032_DriverStructure.b_NumberOfBoard++;
    }
    

    return 0;
}

/*--------------------------------------------------------*/
/* Deinitialisation of the APCI-1032 module in the kernel */
/*--------------------------------------------------------*/
void cleanup_module( void )
{
    /* Cleans the module from the kernel */
    printk( "Cleanup module.\n" );
    unregister_chrdev( i_MajorNumber, DriverName );
}

EXPORT_SYMBOL(i_APCI1032_Read32DigitalInput);
EXPORT_SYMBOL(i_APCI1032_Read16DigitalInput);
EXPORT_SYMBOL(i_APCI1032_Read1DigitalInput);
EXPORT_SYMBOL(i_APCI1032_Read8DigitalInput);
