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
/**************************************************************************
 *
 * Copyright:CVTE
 *
 * Author: Lichao Zhang
 *
 * Date:2016-10-18
 *
 * Description: main thread for server
 *
 * **************************************************************************/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <map>
#include <time.h>
#include <string.h>
#include "broadcast_udpHeart.h"
#include "recv_tcp_heartPacket.h"
#include "wait_user_cmd_exec.h"
#include "glog/logging.h"

using namespace std;

int g_recvTCPOpened = 0;
/*
 * summary: broadcast heart Packet via UDP
 *
 * param:
 *      time_interval: time interval length
 *
 * return:  no return
 * */
void *Broadcast_UDP_HeartPacket_Thread_Func(void* time_interval);

/*
 * summary: recv packet via TCP, if the packet is heart packet,
 *          update the IP's time in map
 *
 * Param:
 *      void
 *
 * return: no return
 *
 * */
void *Recv_TCP_Packet_Thread_Func(void*);

/*
 * summary: recv cmd from user's stdout, build another send_cmd_thread,
 *          send cmd to client via TCP, and build another TCP2 to wait user's ACK
 *
 * Param:
 *      void
 *
 * return: no return
 *
 * */
void *RecvUserCmd_BuildCmdThread(void*);

/*
 * summary: send cmd to client via TCP and build another TCP2 to wait and recv user's ACK
 *
 * Param:
 *      cmd
 *
 * return: no return
 *
 * */
//void *SendCmdToClient_RecvUserACK(string cmd);

int main(int argc, char** argv){
    //TODO: build UDP socket
    //TODO: build TCP1 socket
    pthread_t thread_broadcast_udpPacket;/* thread variables */
    pthread_t thread_TCP_recv_heart;
    pthread_t thread_wait_user_cmd_exec;

    google::InitGoogleLogging(argv[0]);    // log init
    FLAGS_log_dir="./.log";   //log dir

    //broadcast thread start
    int time_interval = 5;
    const int* time_interval_p = &time_interval;
    pthread_create (&thread_broadcast_udpPacket,
    		NULL,
			Broadcast_UDP_HeartPacket_Thread_Func,
			(void*)time_interval_p);


    pthread_create (&thread_TCP_recv_heart,
        		NULL,
				Recv_TCP_Packet_Thread_Func,
    			(void*)NULL);

    pthread_create (&thread_wait_user_cmd_exec,
            		NULL,
					RecvUserCmd_BuildCmdThread,
        			(void*)NULL);

    //pthread join or get
    pthread_join(thread_broadcast_udpPacket, NULL);
    pthread_join(thread_TCP_recv_heart, NULL);
    pthread_join(thread_wait_user_cmd_exec, NULL);
    google::ShutdownGoogleLogging();
    return 0;
}

/*
 * summary: broadcast heart Packet via UDP
 *
 * param:
 *      time_interval: time interval length
 *
 * return:  no return
 * */
void *Broadcast_UDP_HeartPacket_Thread_Func(void* time_interval){

	//LOG(WARNING)<< "Broadcast_UDP_HeartPacket_Thread_Func";
	char broadcast_msg[20] ;
	sprintf(broadcast_msg, "7E45FF7E");
	int len = 18;
	Broadcast(broadcast_msg, len, (*(int*)time_interval));
	return NULL;
}

/*
 * summary: recv packet via TCP, if the packet is heart packet,
 *          update the IP's time in map
 *
 * Param:
 *      void
 *
 * return: no return
 *
 * */
void *Recv_TCP_Packet_Thread_Func(void*){
	LOG(WARNING)<< "Recv_TCP_Packet_Thread_Func";

	g_recvTCPOpened = 1;
	Recv_TCP_Heart_Main();
	//sleep(100000000);
	return NULL;
}

/*
 * summary: recv cmd from user's stdout, build another send_cmd_thread,
 *          send cmd to client via TCP, and build another TCP2 to wait user's ACK
 *
 * Param:
 *      void
 *
 * return: no return
 *
 * */
void *RecvUserCmd_BuildCmdThread(void*){
	while(!g_recvTCPOpened);
	cout << "user can input cmd:" << endl;
	GetUserCmd_Exec();
}
