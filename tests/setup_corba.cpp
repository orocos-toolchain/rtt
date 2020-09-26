/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  test-main.cpp

                        test-main.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <cstdlib>
#include <rtt-config.h>
#include "rtt/os/fosi.h"

int main(int argc, char** argv)
{
    int i = system("[ ! -r ./corba-ipc-server ] || ./corba-ipc-server &");
    i = system("[ ! -r ./corba-mqueue-ipc-server ] || ./corba-mqueue-ipc-server &");
    usleep(500000);
    return i;
}
