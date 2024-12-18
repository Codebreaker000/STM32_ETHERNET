/*
 * udpserver.h
 *
 *  Created on: Feb 26, 2024
 *      Author: Administrator
 */

#ifndef INC_UDPSERVER_H_
#define INC_UDPSERVER_H_

#define MAX_BUF_SIZE 20



void udpServer_init(void);

#include "udp.h"




void udpserver_send(struct udp_pcb *upcb,uint8_t* response_buffer,uint32_t response_buffer_size);
void process_received_data(struct udp_pcb *upcb,uint8_t* receive_buffer,uint32_t receive_buffer_size);

#endif /* INC_UDPSERVER_H_ */
