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


#include <stdio.h>
#include <unistd.h>
#define ORO_MEMORY_POOL
#define	TLSF_USE_LOCKS 0
#define	TLSF_STATISTIC 0
#define	USE_MMAP 0
#define	USE_SBRK 0
#define _DEBUG_TLSF_  (0)
#include <rtt/os/tlsf/tlsf.h>

#define POOL_SIZE 1024 * 1024
char pool[POOL_SIZE];

int main(int argc, char** argv)
{
    printf("Simple main\n");
    int *ptr[100];
    int i, free_mem;

    free_mem = init_memory_pool(POOL_SIZE, pool);
    printf("Total free memory= %d\n", free_mem);
    for (i=0; i< 100; i++)
      if (!(ptr[i]=static_cast<int *>(malloc_ex(1024, pool)))){
        printf("Error\n");
      }
    for (i=0; i< 100; i++)
      free_ex(ptr[i], pool);

    destroy_memory_pool(pool);
    printf("Test OK\n");

    usleep(1E6);
    return 0;
}
