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
 * send_TCP_heart_packet.h
 *
 *  Created on: Oct 18, 2016
 *      Author: zlc
 */

#ifndef SEND_TCP_HEART_PACKET_H_
#define SEND_TCP_HEART_PACKET_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "glog/logging.h"
int send_TCP_heart_main(in_addr addr, int time, int& g_TCP_Opened, int& sockfd) {
	unsigned short port = 12345;             // 服务器的端口号
	int optval;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);       // 创建通信端点：套接字
	if (sockfd < 0) {
		perror("socket");
		exit(-1);
	}
	setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &optval, sizeof optval);
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr)); // 初始化服务器地址
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	//inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
	//inet_aton("127.0.0.1", &server_addr.sin_addr);
	server_addr.sin_addr = addr;
	int err_log = connect(sockfd, (struct sockaddr*) &server_addr,
			sizeof(server_addr));      // 主动连接服务器
	if (err_log != 0) {
		perror("connect");
		close(sockfd);
		exit(-1);
	}
	g_TCP_Opened = 1;
	while (1){
		char broadcast_msg[20] ;
		sprintf(broadcast_msg, "7E45007E");
		int len = 8;
		write(sockfd, broadcast_msg, len);
		sleep(time);
	}

	return 0;
}

#endif /* SEND_TCP_HEART_PACKET_H_ */
