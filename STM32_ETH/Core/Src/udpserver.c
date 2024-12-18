/*
 * udpserver.c
 *
 *  Created on: Feb 26, 2024
 *      Author: Administrator
 */



#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwipopts.h"
#include "stdio.h"
#include "lwipopts.h"
#include "udpserver.h"

#include "stdint.h"
#include "main.h"
#include "string.h"




extern uint8_t receive_buffer[MAX_BUF_SIZE];
extern uint32_t receive_buffer_size;
extern uint8_t  response_buffer[30];

struct udp_pcb* upcb;

ip4_addr_t CTS_IPADDR;
uint16_t CTS_PORT;


void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);



/* IMPLEMENTATION FOR UDP Server :   source:https://www.geeksforgeeks.org/udp-server-client-implementation-c/

1. Create UDP socket.
2. Bind the socket to server address.
3. Wait until datagram packet arrives from client.
4. Process the datagram packet and send a reply to client.
5. Go back to Step 3.
*/

void udpServer_init(void)
{
	// UDP Control Block structure
   //struct udp_pcb *upcb;
   err_t err;

   /* 1. Create a new UDP control block  */
   upcb = udp_new();

   /* 2. Bind the upcb to the local port */
   ip_addr_t myIPADDR;
   IP_ADDR4(&myIPADDR, 192, 168, 0, 10);

   err = udp_bind(upcb, &myIPADDR, 8080);  // 8080 is the server UDP port


   /* 3. Set a receive callback for the upcb */
   if(err == ERR_OK)
   {
	   udp_recv(upcb, udp_receive_callback, NULL);
   }
   else
   {
	   udp_remove(upcb);
   }
}

// udp_receive_callback will be called, when the client sends some data to the server
/* 4. Process the datagram packet and send a reply to client. */

void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	/* Connect to the remote client */
	udp_connect(upcb, addr, port);

	uint16_t copied_ret;

	if (p != NULL) {
		if (p->tot_len <= MAX_BUF_SIZE) {
			receive_buffer_size = p->tot_len;
		} else {
			receive_buffer_size = MAX_BUF_SIZE;
		}

		if (receive_buffer != NULL) {
			// Copy the data from pbuf to the receive_buffer
			copied_ret = pbuf_copy_partial(p, receive_buffer, receive_buffer_size, 0);
			if (copied_ret > 0) {
				// Process the received data
				process_received_data(upcb, receive_buffer, receive_buffer_size);
			} else {
				// Handle error: no data copied
			}
		} else {
			// Handle error: receive_buffer is NULL
		}

		// Free the received pbuf
		pbuf_free(p);
	}

}

void process_received_data(struct udp_pcb *upcb,uint8_t* receive_buffer,uint32_t receive_buffer_size){



	if(receive_buffer[0] == 0xAB && receive_buffer[1] == 0xAB){                       //TM_ITP MESSAGE (M10)
		if(receive_buffer[3] == 0xCD && receive_buffer[4] == 0xCD){
			if(receive_buffer[2] == 0x01){
				strcpy((char*)response_buffer, "Received 0x01");
				udpserver_send(upcb, response_buffer, 13);
			}
			else if(receive_buffer[2] == 0x02){
				strcpy((char*)response_buffer, "Received 0x02");
				udpserver_send(upcb, response_buffer, 13);
			}
			else if(receive_buffer[2] == 0x03){
				strcpy((char*)response_buffer, "Received 0x03");
				udpserver_send(upcb, response_buffer, 13);
			}

		}
	}
		else{
			strcpy((char*)response_buffer, "Header & Footer not matched");
			udpserver_send(upcb, response_buffer, 27);
		}

}

void udpserver_send(struct udp_pcb *upcb,uint8_t* response_buffer,uint32_t response_buffer_size){
	struct pbuf *txBuf;



	/* allocate pbuf from RAM*/
	txBuf = pbuf_alloc(PBUF_TRANSPORT,response_buffer_size, PBUF_RAM);

	/* copy the data into the buffer  */
	pbuf_take(txBuf, response_buffer, response_buffer_size);


//	/* Send a Reply to the Client */
	udp_send(upcb, txBuf);

//
//	/* Free the p_tx buffer */
	pbuf_free(txBuf);

}

void udp_serial_data(struct udp_pcb *upcb,uint8_t* response_buffer,uint32_t response_buffer_size){
		struct pbuf *serialBuf;

		/* allocate pbuf from RAM*/
		serialBuf = pbuf_alloc(PBUF_TRANSPORT,response_buffer_size, PBUF_RAM);

		/* copy the data into the buffer  */
		pbuf_take(serialBuf, response_buffer, response_buffer_size);

		/* Connect to the remote client */
		udp_connect(upcb, &CTS_IPADDR, CTS_PORT);
	//
	//	/* Send a Reply to the Client */
		udp_send(upcb, serialBuf);
	//
	//	/* free the UDP connection, so we can accept new clients */
		udp_disconnect(upcb);
	//
	//	/* Free the p_tx buffer */
		pbuf_free(serialBuf);
}

