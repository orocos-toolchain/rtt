/**
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * author  Jef De Geeter (jef@zazof.org)
 *  Pascal Deklerck (pascal.deklerck@student.kuleuven.ac.be) 
 * date  July 2001
 *
 * Revision: $Revision: 1.2 $
 * Last edited: $Date: 2002/10/01 17:01:39 $
 * Log:  $Log: schunkioctls.h,v $
 * Log:  Revision 1.2  2002/10/01 17:01:39  pissaris
 * Log:  Major reindent and reformat of sourcecode.
 * Log:
 * Log:  Revision 1.1.1.1  2002/07/12 14:06:13  gdraelan
 * Log:  schunk import
 * Log:  
 * Log:  Revision 1.3  2001/08/07 10:33:11  jef
 * Log:  added GPL license
 * Log:  added cvs keywords
 * Log:  
 */

#ifndef SCHUNK_IOCTLS_H
#define SCHUNK_IOCTLS_H

/** @name Schunk sensor io control commands */
//@{

/** @memo Schunk configuration mode
 * @doc In configuration mode, all communication is directly to and from the physical sensor
  */
#define SCHUNK_CONFIG_MODE     0 
/** @memo Schunk sample mode
 * @doc In sample mode, samples are written into the logical schunk sensor for reading by other components */
#define SCHUNK_SAMPLE_MODE     1 
/** @memo Schunk ASCII mode
 * @doc In ASCII mode, all valid samples are 45 bytes long and made up out of tokens of the ASCII-table */
#define SCHUNK_ASCII           2 
/** @memo Schunk binary mode
 * @doc In binary mode, all valid samples are 13 bytes long */
#define SCHUNK_BINARY          3

//@}

typedef struct
{
    double values[ 6 ];
    short overload;
}

schunkdata_t;

#endif // SCHUNK_IOCTLS_H
