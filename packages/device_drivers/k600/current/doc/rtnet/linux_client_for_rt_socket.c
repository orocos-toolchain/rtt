/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:16 CET 2005  linux_client_for_rt_socket.c 

                        linux_client_for_rt_socket.c -  description
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
 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

struct sockaddr_in local_addr;
struct sockaddr_in server_addr;

int main (int argc, char *argv[])
{

  int sockfd    = 0;
  int ret       = 0;

  char msg[] = "This message was sent using gnu/linux.";

  /* Set address structures to zero.  */
  memset(&local_addr, 0, sizeof(struct sockaddr_in));
  memset(&server_addr, 0, sizeof(struct sockaddr_in));

  printf("RTnet, simple linux client for LXRT server\n");

  if (argc==4) 
    {
      local_addr.sin_family      = AF_INET;
      local_addr.sin_addr.s_addr = INADDR_ANY;
      local_addr.sin_port        = htons(atoi(argv[1]));
      
      server_addr.sin_family = AF_INET;
      inet_aton(argv[2], &server_addr.sin_addr);
      server_addr.sin_port = htons(atoi(argv[3]));
      // server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    }
  else
    {
      fprintf(stderr, "Usage: %s <local-port> <server-ip> <server-port>\n", 
	      argv[0]);
      exit(1); 
    }

  /* Create new socket. */
  printf("Creating socket\n");
  sockfd = socket(PF_INET, SOCK_DGRAM, 0);
  if (sockfd<0) { perror("socket()"); exit(1); }

  /* Bind socket to local address specified as parameter. */
  printf("Binding socket\n");
  ret = bind(sockfd, (struct sockaddr *) &local_addr, 
	     sizeof(struct sockaddr_in));
  if (ret<0) { perror("bind()"); exit(1); }

  /* Specify destination address for socket; needed for send(). */
  printf("Connecting socket\n");
  ret = connect(sockfd, (struct sockaddr *) &server_addr,
	  sizeof(struct sockaddr_in));
  if (ret<0) { perror("connect()"); exit(1); }

  /* Send a message. */
  printf("Sending...\n");
  ret = send(sockfd, msg, sizeof(msg), 0);
  printf("ret = %d\n", ret);
  if (ret<0) { perror("send()"); exit(1); }

  /* close socket */
  printf("Closing socket\n");
  close(sockfd);
  
  return 0;
}



