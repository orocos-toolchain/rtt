/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:16 CET 2005  input.c 

                        input.c -  description
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
 | Modulname : input.c           | Version  : 2.96 (SuSE 7.1 - 2.2.18)   |
 +-------------------------------+---------------------------------------+
 | Author    : STOLZ, Eric       | Date     : 21.10.2001                 |
 +-----------------------------------------------------------------------+
 | Description : APCI1032 header INPUT/OUTPUT                      |
 +-----------------------------------------------------------------------+
 |                             UPDATE'S                                  |
 +-----------------------------------------------------------------------+
 |   Date   |   Author  |          Description of updates                |
 +----------+-----------+------------------------------------------------+
 |          |           |                                                |
 +-----------------------------------------------------------------------+
*/


#include "apci1032.h"

/*
+----------------------------------------------------------------------------+
| Function name     :  INT      i_APCI1032_Read1DigitalInput                 |
|                                               (INT      i_BoardHandle,     |
|                                                BYTE_    b_Channel,         |
|                                                PBYTE  pb_ChannelValue)     |
+----------------------------------------------------------------------------+
| Task              : Indicates the state of an input. The variable b_Channel|
|                     passes the input to be read (1 to 32). A value is      |
|                     returned with the variable pb_ChannelValue :           |
|                     0 (low) or 1 (high).                                   |
+----------------------------------------------------------------------------+
| Input Parameters: int       i_BoardHandle   : Handle of board APCI-1032    |
|                   unsigned char b_Channel   : The number of the input to   |
|                                                 be read (1 to 32)          |
|                   unsigned char * pb_ChannelValue : State of the digital input:|
|                                                       0 -> low             |
|                                                       1 -> high            |
+----------------------------------------------------------------------------+
| Return Value      : 0: No error                                            |
|                    -1: The handle parameter of the board is wrong          |
|                    -2: The input number is not between 1 and 32            |
+----------------------------------------------------------------------------+
*/

int i_APCI1032_Read1DigitalInput ( int i_BoardHandle,
                                   unsigned char b_Channel,
                                   unsigned char * pb_ChannelValue )
{
    int i_Ret; /* Variable for the ReturnValue */
    unsigned char arg; /* Saves the b_channel to read and the Read Value pb_ChannelValue */

    /* Test the b_channnel parameter */

    if ( b_Channel >= 1 && b_Channel <= 16 )
    {
        /* Saves the b_Channel for the IOCTL function */
        arg = b_Channel;
        /* Run the CMD_APCI1032_Read1DigitalInput IOCTL function and test if OK */

        if ( ( i_Ret = ioctl( i_BoardHandle,
                              CMD_APCI1032_Read1DigitalInput,
                              &arg ) ) == 0 )
        {
            *pb_ChannelValue = arg;
        }

        else
        {
            /*--------------------------------------------*/
            /* The handle parameter of the board is wrong */
            /*--------------------------------------------*/
            return -1;
        }
    }

    else
    {
        /*-------------------------------------*/
        /* b_ChannelValue not between 1 and 16 */
        /*-------------------------------------*/
        return -2;
    }

    return i_Ret;
}

/*
+----------------------------------------------------------------------------+
| Function name     : int i_APCI1032_Read8DigitalInput                       |
|                                          (int i_BoardHandle,               |
|                                           unsigned char * pb_PortValue)    |
+----------------------------------------------------------------------------+
| Task              : Indicates the state of the port. A value is returned   |
|                     with the variable pb_PortValue.                        |
+----------------------------------------------------------------------------+
| Input Parameters: int       i_BoardHandle     : Handle of board APCI-1032  |
|                   unsigned char *pb_PortValue : State of the digital input |
|                                                 port (1 to 4)              |
+----------------------------------------------------------------------------+
| Return Value      : 0: No error                                            |
|                    -1: The handle parameter of the board is wrong          |
|                    -2: The number of the port is not between 1 and 4       |
+----------------------------------------------------------------------------+
*/
int i_APCI1032_Read8DigitalInput ( int i_BoardHandle,
                                   unsigned char b_Port,
                                   unsigned char * pb_PortValue )
{
    int i_Ret; /* Variable for the ReturnValue */
    unsigned char arg; /* Saves the b_Port to read and the Read Value pb_PortValue */

    /* Test the b_Port parameter */

    if ( b_Port >= 1 && b_Port <= 4 )
    {
        /* Saves the b_Port for the IOCTL function */
        arg = b_Port;
        /* Run the CMD_APCI1032_Read8DigitalInput IOCTL function and test if OK */

        if ( ( i_Ret = ioctl( i_BoardHandle,
                              CMD_APCI1032_Read8DigitalInput,
                              &arg ) ) == 0 )
        {
            *pb_PortValue = arg;
        }

        else
        {
            /*--------------------------------------------*/
            /* The handle parameter of the board is wrong */
            /*--------------------------------------------*/
            return -1;
        }
    }

    else
    {
        /*---------------------------------*/
        /* b_PortValue not between 1 and 4 */
        /*---------------------------------*/
        return -2;
    }

    return i_Ret;
}

/*
+----------------------------------------------------------------------------+
| Function name     :  INT      i_APCI1032_Read16DigitalInput                |
|                                          (INT      i_BoardHandle,          |
|                                          (BYTE     b_Port,                 |
|                                           PULONG pul_InputValue)           |
+----------------------------------------------------------------------------+
| Task              : Indicates the state of all inputs . A value is returned|
|                     with the variable pl_InputValue.                       |
+----------------------------------------------------------------------------+
| Input Parameters: int        i_BoardHandle  : Handle of board APCI-1032    |
|                   unsigned char b_Port      : Number of the port (1 or 2)  |
|                   unsigned long *pul_InputValue: State of the digital inputs|
|                                                 (1 to 16) or (17 to 32).   |
+----------------------------------------------------------------------------+
| Return Value      : 0: No error                                            |
|                    -1: The handle parameter of the board is wrong          |
|                    -2: The port number is wrong (not 1 or 2)               |
+----------------------------------------------------------------------------+
*/
int i_APCI1032_Read16DigitalInput ( int i_BoardHandle, unsigned char b_Port, unsigned long * pul_InputValue )
{
    int i_Ret;  /* Variable for the ReturnValue */
    long arg;   /* Saves the Read Value l_InputValue */

    if ( b_Port >= 1 && b_Port <= 2 )
    {
        /* Saves the b_Port for the IOCTL function */
        arg = ( unsigned long ) b_Port;
        /* Run the CMD_APCI1032_Read8DigitalInput IOCTL function and test if OK */

        if ( ( i_Ret = ioctl( i_BoardHandle,
                              CMD_APCI1032_Read16DigitalInput,
                              &arg ) ) == 0 )
        {
            *pul_InputValue = arg;
        }

        else
        {
            /*--------------------------------------------*/
            /* The handle parameter of the board is wrong */
            /*--------------------------------------------*/
            return -1;
        }
    }

    else
    {
        /*---------------------------------*/
        /* b_PortValue not between 1 and 2 */
        /*---------------------------------*/
        return -2;
    }

    return i_Ret;
}

/*
+----------------------------------------------------------------------------+
| Function name     :  INT      i_APCI1032_Read32DigitalInput                |
|                                          (INT     i_BoardHandle            |
|                                           PULONG pul_InputValue)           |
+----------------------------------------------------------------------------+
| Task              : Indicates the state of all inputs . A value is returned|
|                     with the variable pl_InputValue.                       |
+----------------------------------------------------------------------------+
| Input Parameters  : INT        i_BoardHandle  : Handle of board APCI-1032  |
|                     PBYTE     pl_PortValue    : State of the digital inputs|
|                                                 (0 to 32)                  |
+----------------------------------------------------------------------------+
| Return Value      : 0: No error                                            |
|                    -1: The handle parameter of the board is wrong          |
+----------------------------------------------------------------------------+
*/
int i_APCI1032_Read32DigitalInput ( int i_BoardHandle, unsigned long * pul_InputValue )
{
    int i_Ret;  /* Variable for the ReturnValue */
    unsigned long arg;   /* Saves the Read Value l_InputValue */

    /* Variable is used to get the pl_InputValue */
    arg = 0;

    /* Run the CMD_APCI1032_Read32DigitalInput IOCTL function and test if OK */

    if ( ( i_Ret = ioctl( i_BoardHandle,
                          CMD_APCI1032_Read32DigitalInput,
                          &arg ) ) == 0 )
    {
        *pul_InputValue = arg;
    }

    else
    {
        /*--------------------------------------------*/
        /* The handle parameter of the board is wrong */
        /*--------------------------------------------*/
        return -1;
    }

    return i_Ret;
}

