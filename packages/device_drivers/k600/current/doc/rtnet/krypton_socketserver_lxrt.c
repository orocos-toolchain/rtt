/* simpleserver_thesis.c
 *
 * Adapted from simpleserver - a simple server for demonstration of rtnet_lxrt
 * 06/2003 by Hans-Peter Bock <hpbock@avaapgh.de>
 *

 For reading krypton measurements

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sched.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <rtnet.h>

static struct sockaddr_in local_addr;

#define MAX_NUM_COORD 50 
// The maximum of coordinates is set to 50, that makes up for 16 leds
// x 3 coordinates (doubles) per led
double* coordinates[MAX_NUM_COORD];

void printdouble(char * msg, 
		 unsigned short * a, unsigned short * b, unsigned short * c, unsigned short * d,
		 char * mychar, 
		 unsigned short * e, unsigned short * f, unsigned short * g, 
		 double** k);

int main(int argc, char *argv[]) {
    int sockfd = 0;
    int ret    = 0;
    int i = 0;

    // Maximum message size
    // Suppose we have 16 leds x 3 coordinates (doubles) / led x 8
    // bytes / double + 15 bytes of other information in the message
    // This boils down to plus minus 400 chars
    unsigned char msg[500];

    unsigned short a,b,c,d,e,f,g;
    char mychar;

    RT_TASK *lxrtnettsk;

    for (i = 0; i < MAX_NUM_COORD; i++)
      {
	coordinates[i] = (double *) malloc(sizeof(double));
      }
    

    /* Set variables to zero.  */
    // memset(msg, 0, sizeof(struct msg_t));
    memset(&local_addr, 0, sizeof (struct sockaddr_in));

    printf("RTnet, simpleserver for NEWLXRT\n");

    /* Check arguments and set addresses. */
    if (argc == 2) {
        local_addr.sin_family      = AF_INET;
        local_addr.sin_addr.s_addr = INADDR_ANY;
        local_addr.sin_port        = htons(atoi(argv[1]));
    } else {
        fprintf(stderr,
                "Usage: "
                "%s <local-port>\n",
                argv[0]);
        exit(1);
    }

    /* Lock allocated memory into RAM. */
    mlockall(MCL_CURRENT|MCL_FUTURE);

    /* Create new socket. */
    sockfd = rt_socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {

        printf("Error opening socket: %d\n", sockfd);
        exit(1);
    }

    /* Initialize a real time buddy. */
    lxrtnettsk = rt_task_init(4900, 1, 0, 0);
    if (NULL == lxrtnettsk) {
        rt_socket_close(sockfd);
        printf("CANNOT INIT MASTER TASK\n");
        exit(1);
    }

    /* Switch over to hard realtime mode. */
    rt_make_hard_real_time();

    /* Bind socket to local address specified as parameter. */
    ret = rt_socket_bind(sockfd, (struct sockaddr *) &local_addr,
                         sizeof(struct sockaddr_in));

    /* Block until packet is received. */
    ret = rt_socket_recv(sockfd, msg, sizeof(msg), 0);
    printdouble(msg,&a,&b,&c,&d,&mychar,&e,&f,&g,&coordinates[0]);

    /* Switch over to soft realtime mode. */
    rt_make_soft_real_time();

    /* Close socket.
     * Note: call must be in soft-mode because socket was created as non-rt! */
    rt_socket_close(sockfd);

    /* Stop the timer. */
    stop_rt_timer();

    /* Delete realtime buddy. */
    rt_task_delete(lxrtnettsk);
    printf("Received message: %u, %u, %u, %u, %c, %u, %u, %u, ", a, b, c, d, mychar, e, f, g);
    for (i = 0 ; i < g*3 ; i++)
      {
	printf("%f, ",*(coordinates[i]));
      }
    printf("\n");
    return 0;
}

void printdouble(char * msg, 
		 unsigned short * a, unsigned short * b, unsigned short * c, unsigned short * d,
		 char * mychar, 
		 unsigned short * e, unsigned short * f, unsigned short * g, 
		 double** k)
{
  /*
  *a = (unsigned short) msg[0] + ((unsigned short) msg[1] * 256);
  *b = (unsigned short) msg[2] + ((unsigned short) msg[3] * 256);
  *c = (unsigned short) msg[4] + ((unsigned short) msg[5] * 256);
  *d = (unsigned short) msg[6] + ((unsigned short) msg[7] * 256);
  *mychar = msg[8];
  *e = (unsigned short) msg[9] + ((unsigned short) msg[10] * 256);
  *f = (unsigned short) msg[11] + ((unsigned short) msg[12] * 256);
  *g = (unsigned short) msg[13] + ((unsigned short) msg[14] * 256);
  */

  unsigned short * unshtmp;
  char * chartmp;
  unsigned short i;
  unsigned int index = 15;

  unshtmp = &msg[0];
  *a = *unshtmp;
  unshtmp = &msg[2];
  *b = *unshtmp;
  unshtmp = &msg[4];
  *c = *unshtmp;
  unshtmp = &msg[6];
  *d = *unshtmp;

  chartmp = &msg[8];
  *mychar = *chartmp;

  unshtmp = &msg[9];
  *e = *unshtmp;
  unshtmp = &msg[11];
  *f = *unshtmp;
  unshtmp = &msg[13];
  *g = *unshtmp;

  for (i = 0; i < (*g * 3) ; i++)
    {
      *k = &msg[index];
      index += 8; // A double is 8 bytes (chars)
      ++k;
    }
}

