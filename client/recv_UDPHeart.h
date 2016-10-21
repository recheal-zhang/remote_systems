/* Copyright (C) 
 * 2016 - Lichao Zhang
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */
/*
 * recv_UDPHeart.h
 *
 *  Created on: Oct 17, 2016
 *      Author: zlc
 */

#ifndef RECV_UDPHEART_H_
#define RECV_UDPHEART_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "glog/logging.h"
#include <string>
#include <iostream>

void Recv_UDP_Heart(in_addr& addr, int& udpHeartFlag) {
	//TODO: build UDP
	int sock, status, buflen;
	unsigned sinlen;
	char buffer[128];
	struct sockaddr_in sock_in;
	int yes = 1;


	sinlen = sizeof(struct sockaddr_in);
	memset(&sock_in, 0, sinlen);

	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_in.sin_port = htons(3777);
	sock_in.sin_family = AF_INET;

	status = bind(sock, (struct sockaddr *) &sock_in, sinlen);
	status = getsockname(sock, (struct sockaddr *) &sock_in, &sinlen);
	addr = sock_in.sin_addr;
//	LOG(INFO) << "SOCK IP" ;

	buflen = 128;
	while (1) {
		memset(buffer, 0, buflen);
		status = recvfrom(sock, buffer, buflen, 0, (struct sockaddr *) &sock_in,
				&sinlen);

		//std::cout << std::string(inet_ntoa(sock_in.sin_addr)) << std::endl;
//		LOG(INFO) << std::string(inet_ntoa(sock_in.sin_addr)) ;
		udpHeartFlag = 1;


	}

}

#endif /* RECV_UDPHEART_H_ */
