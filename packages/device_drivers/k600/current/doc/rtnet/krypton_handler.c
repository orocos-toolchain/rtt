/***
 *
 *  examples/krypton/krypton_handler.c
 *
 *  server part - listens signals to Userspace that a message from
 *  K600 has arrived
 *
 *  based on Ulrich Marx's module, adopted to rtmac
 *  based on examples/rtt/rt_server.c
 *
 *  Copyright (C) 2002 Ulrich Marx <marx@kammer.uni-hannover.de>
 *                2002 Marc Kleine-Budde <kleine-budde@gmx.de>
 *
 *  Copyright (C) 2004 Klaas Gadeyne
 *  Adapted to RTnet 0.8 by Panagiotis Issaris
 *
 *  rtnet - real-time networking example
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>

#include <net/ip.h>

#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>
#include <rtai_sem.h>
#include <rtai_mbx.h>
#include <rtnet.h>

static char *local_ip_s  = "192.168.1.230";
static unsigned int rcv_port = 65432;

MODULE_LICENSE("GPL");
MODULE_PARM (local_ip_s ,"s");
MODULE_PARM_DESC (local_ip_s, "local ip-addr");
MODULE_PARM (rcv_port ,"i");
MODULE_PARM_DESC (rcv_port, "Port to which server is listening");

static struct sockaddr_in local_addr;
static int sock;

#define MAX_NUM_LEDS 20 // Maximum number of leds
#define NUM_COORD_PER_LED 3 // Number of coordinates per led 
#define MAX_NUM_COORD (MAX_NUM_LEDS*NUM_COORD_PER_LED)
#define MAX_MESSAGE_LENGTH (15 + MAX_NUM_COORD * 8)

#define BUFSIZE MAX_MESSAGE_LENGTH
static char buffer[BUFSIZE];

static SEM * alert_encoderdriver_sem;
static MBX * UDP_mbx;

int echo_rcv(int s,void *arg)
{
  int			ret=0;
  int                   i=0;
  struct msghdr		msg;
  struct iovec		iov;
  struct sockaddr_in	addr;

  memset(&msg, 0, sizeof(msg));
  iov.iov_base=&buffer;
  iov.iov_len=BUFSIZE;
  msg.msg_name=&addr;
  msg.msg_namelen=sizeof(addr);
  msg.msg_iov=&iov;
  msg.msg_iovlen=1;
  msg.msg_control=NULL;
  msg.msg_controllen=0;

  // ret= rt_socket_recvmsg(sock, &msg, 0);
  ret= recv_rt(sock, buffer, sizeof(buffer), 0);
  if ( (ret>0) && (msg.msg_namelen==sizeof(struct sockaddr_in)) ) 
    {
      // rt_printk("Krypton Handler: Received Message: %s\n",msg.msg_iov->iov_base);
      /*
      rt_printk("Krypton Handler: Received Message: Pass it to MBX and Let userspace know\n");
      for (i = 0; i < BUFSIZE ; i++)
	{
	  rt_printk("%x, ", (unsigned char) buffer[i]);
	}
      rt_printk("\n");
      */
      // Only send if sender is not blocked
      ret = rt_mbx_send_if(UDP_mbx,(void *) &buffer, sizeof(buffer));
      // Signal Semaphore so userspace can read
      rt_sem_signal(alert_encoderdriver_sem);
  }
  return 0;
}


int init_module(void)
{
  int ret;
  unsigned long local_ip  = rt_inet_aton(local_ip_s);
  struct rtnet_callback   callback  = {echo_rcv, NULL};
  
  rt_printk ("Loading module krypton_handler.o: ");
  rt_printk ("local ip address = %s, ", local_ip_s);
  rt_printk ("local port = %d\n", rcv_port);

  /* create rt-socket */
  rt_printk("Krypton Handler: creating rtsocket\n");
  if ((sock=socket_rt(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    rt_printk("socket not created\n");
    return -ENOMEM;
  }

  /* bind the rt-socket to local_addr */
  rt_printk("Krypton Handler: binding rtsocket to local address:port\n");
  memset(&local_addr, 0, sizeof(struct sockaddr_in));
  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(rcv_port);
  local_addr.sin_addr.s_addr = local_ip;
  if ( (ret=bind_rt(sock, (struct sockaddr *) &local_addr, sizeof(struct sockaddr_in)))<0 ) {
    rt_printk("can't bind rtsocket\n");
    return ret;
  }

  /* Creating Semaphore for letting Userspace know something has
     arrived */
  alert_encoderdriver_sem = rt_named_sem_init("KEDSEM", 0);
  /* Creating MBX for passing the message on to userspace */
  UDP_mbx = rt_named_mbx_init("KEDMBX", BUFSIZE);

  /* set up receiving */
  //rt_socket_callback(sock, echo_rcv, NULL);
  ioctl_rt(sock, RTNET_RTIOC_CALLBACK, &callback);
  
  /* Signalling once just for fun */
  // rt_sem_signal(alert_encoderdriver_sem);

  return ret;
}




void cleanup_module(void)
{
  while (close_rt(sock) == -EAGAIN) {
    set_current_state(TASK_INTERRUPTIBLE);
    schedule_timeout(1*HZ); /* wait a second */
  }
  rt_sem_delete(alert_encoderdriver_sem);
  rt_printk ("Unloading module krypton_handler\n");
}




















