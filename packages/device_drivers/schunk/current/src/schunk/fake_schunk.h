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
 * author  Pascal Deklerck (pascal.deklerck@student.kuleuven.ac.be)
 *   Jef De Geeter (jef@zazof.org)
 * date  July 2001
 *
 * Revision: $Revision: 1.2 $
 * Last edited: $Date: 2002/10/01 17:01:39 $
 * Log:  $Log: fake_schunk.h,v $
 * Log:  Revision 1.2  2002/10/01 17:01:39  pissaris
 * Log:  Major reindent and reformat of sourcecode.
 * Log:
 * Log:  Revision 1.1.1.1  2002/07/12 14:06:13  gdraelan
 * Log:  schunk import
 * Log:  
 * Log:  Revision 1.2  2001/08/07 12:07:16  jef
 * Log:  - added GPL license
 * Log:  - added cvs keywords
 * Log:  
 */

#ifndef FAKE_SCHUNK_H
#define FAKE_SCHUNK_H

#include <rtl.h>
#include <pthread.h>
#include "schunksensor.h"

void fake_schunk_open( sensor_t* s );
void fake_schunk_write( char* command, int len );
int fake_schunk_read( char* buf, int len );
void fake_schunk_close( void );

#endif // FAKE_SCHUNK_H
