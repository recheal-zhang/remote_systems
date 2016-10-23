/****************************************************
 *
 * Copyright: CVTE
 *
 * Author: Lichao Zhang
 *
 * Date: 2016-10-18
 *
 * Description: main thread for client
 *
 * **************************************************/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "recv_UDPHeart.h"
#include "send_TCP_heart_packet.h"
#include "glog/logging.h"
#include "recv_TCP_cmd.h"

using namespace std;
//char* g_serverIpAddr;
in_addr addr;
int udpHeartFlag = 0;
int g_TCP_Opened = 0;
int g_fd;

/*
 * summary: UDP recv continously
 *
 * Param:
 *      void
 *
 * return: no return
 * */
void *Recv_UDP_HeartPacket_Thread_Func(void*);

/*
 * summary: if receive heart packet from server via UDP,
 *         client sent heart packet to server via TCP
 *
 * Param:
 *      time_interval: time interval length
 *
 * return: no return
 *
 * */
void *Send_TCP_HeartPacket_Thread_Func(void* time_interval);

/*
 * summary: recv cmd from TCP and build another thread to send file to server via TCP2
 *
 * Param:
 *      void
 * return: no return
 *
 * */
void *Recv_TCP_Cmd_Thread_Func(void*);

/*
 * summary: send file to server via TCP2
 *
 * Param:
 *      path
 *
 * return: no return
 *
 * */
//void *Send_File_To_Server(char* path);

int main(int argc, char **argv){
    //run in the background
    //daemon(0, 0);
    //TODO: build TCP1
	pthread_t thread_recv_udpPacket;/* thread variables */
	pthread_t thread_send_tcp_heart;
	pthread_t thread_recv_tcp_cmd;

	google::InitGoogleLogging("log");    // log init
	FLAGS_log_dir = "./.log";   //log dir

	pthread_create(&thread_recv_udpPacket,
			NULL,
			Recv_UDP_HeartPacket_Thread_Func,
			(void*) NULL);


	//send tcp heart thread start
	int time_interval = 10;
	const int* time_interval_p = &time_interval;
	pthread_create(&thread_send_tcp_heart, NULL,
			Send_TCP_HeartPacket_Thread_Func, (void*) time_interval_p);

	pthread_create(&thread_recv_tcp_cmd,
				NULL,
				Recv_TCP_Cmd_Thread_Func,
				(void*) NULL);

	pthread_join(thread_recv_udpPacket, NULL);
	pthread_join(thread_send_tcp_heart, NULL);
	pthread_join(thread_recv_tcp_cmd, NULL);

	//google::ShutdownGoogleLogging();
	return 0;
}

/*
 * summary: UDP recv continously
 *
 * Param:
 *      void
 *
 * return: no return
 * */
void *Recv_UDP_HeartPacket_Thread_Func(void* ){
	//pthread_detach(pthread_self());

	Recv_UDP_Heart(addr, udpHeartFlag);
	return NULL;
}

/*
 * summary: if receive heart packet from server via UDP,
 *         client sent heart packet to server via TCP
 *
 * Param:
 *      time_interval: time interval length
 *
 * return: no return
 *
 * */
void *Send_TCP_HeartPacket_Thread_Func(void* time_interval){
	//pthread_detach(pthread_self());

	while(!udpHeartFlag);
	send_TCP_heart_main(addr, *(int*)time_interval, g_TCP_Opened,g_fd);
	return NULL;
}


/*
 * summary: recv cmd from TCP and build another thread to send file to server via TCP2
 *
 * Param:
 *      void
 * return: no return
 *
 * */
void *Recv_TCP_Cmd_Thread_Func(void*){
	cout << "tcp recv ready" << endl;
	while(!g_TCP_Opened);
	//cout << "tcp recv start" << endl;
	Recv_Cmd(g_fd);
}
