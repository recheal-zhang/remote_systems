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
 * broadcast_udpHeart.h
 *
 *  Created on: Oct 17, 2016
 *      Author: zlc
 */

#ifndef BROADCAST_UDPHEART_H_
#define BROADCAST_UDPHEART_H_

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

void Broadcast(char broadcast_msg[], int len, int time_interval){
    int inet_sock;
    struct sockaddr_in adr_bc;
    int so_broadcast = 1;

    if((inet_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Broadcast UDP created socket error");
    }
    if(setsockopt(inet_sock, SOL_SOCKET, SO_BROADCAST, &so_broadcast,
                sizeof(so_broadcast)) < 0)
    {
        perror("Broadcast UDP set socket error");
        close(inet_sock);
        return;
    }

    adr_bc.sin_family = AF_INET;
    adr_bc.sin_port = htons(3777);
    adr_bc.sin_addr.s_addr = inet_addr("255.255.255.255");

    while(1){
        if(sendto(inet_sock, broadcast_msg, strlen(broadcast_msg), 0,
                    (struct sockaddr*)&adr_bc, sizeof(adr_bc)) < 0){
            perror("Broadcast send error");
            close(inet_sock);
            return;
        }
        sleep(time_interval);
    }

}

#endif /* BROADCAST_UDPHEART_H_ */
