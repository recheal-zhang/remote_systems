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
 * recv_tcp_heartPacket.h
 *
 *  Created on: Oct 18, 2016
 *      Author: zlc
 */

#ifndef RECV_TCP_HEARTPACKET_H_
#define RECV_TCP_HEARTPACKET_H_

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include "glog/logging.h"
#include <string>
#include "sys/types.h"
#include "sys/wait.h"
#include "sys/stat.h"

#define MAX_EVENTS 500

int g_filefd[MAX_EVENTS] = {0};

struct myevent_s {
	int fd;
	char* ip;
	unsigned short port;
	void (*call_back)(int fd, int events, void *arg);
	int events;
	void *arg;
	int status; // 1: in epoll wait list, 0 not in
	char buff[20]; // recv data buffer
	int len, s_offset;
	long last_active; // last active time
};
// set event
void EventSet(myevent_s *ev, int fd, char* ip, unsigned short port,
		void (*call_back)(int, int, void*), void *arg) {
	ev->fd = fd;
	ev->ip = ip;
	ev->port = port;
	ev->call_back = call_back;
	ev->events = 0;
	ev->arg = arg;
	ev->status = 0;
	bzero(ev->buff, sizeof(ev->buff));
	ev->s_offset = 0;
	ev->len = 0;
	ev->last_active = time(NULL);
}
// add/mod an event to epoll
void EventAdd(int epollFd, int events, myevent_s *ev) {
	struct epoll_event epv = { 0, { 0 } };
	int op;
	epv.data.ptr = ev;
	epv.events = ev->events = events;
	if (ev->status == 1) {
		op = EPOLL_CTL_MOD;
	} else {
		op = EPOLL_CTL_ADD;
		ev->status = 1;
	}
	if (epoll_ctl(epollFd, op, ev->fd, &epv) < 0) {
	}
//		printf("Event Add failed[fd=%d], evnets[%d]\n", ev->fd, events);

	else {
	}
//		printf("Event Add OK[fd=%d], op=%d, evnets[%0X]\n", ev->fd, op, events);
}
// delete an event from epoll
void EventDel(int epollFd, myevent_s *ev) {
	struct epoll_event epv = { 0, { 0 } };
	if (ev->status != 1)
		return;
	epv.data.ptr = ev;
	ev->status = 0;
	epoll_ctl(epollFd, EPOLL_CTL_DEL, ev->fd, &epv);
}
int g_epollFd;
myevent_s g_Events[MAX_EVENTS + 1]; // g_Events[MAX_EVENTS] is used by listen fd
void RecvData(int fd, int events, void *arg);
void SendData(int fd, int events, void *arg);
// accept new connections from clients
void AcceptConn(int fd, int events, void *arg) {
	struct sockaddr_in sin;
	socklen_t len = sizeof(struct sockaddr_in);
	int nfd, i;
	// accept
	if ((nfd = accept(fd, (struct sockaddr*) &sin, &len)) == -1) {
		if (errno != EAGAIN && errno != EINTR) {
		}
		//printf("%s: accept, %d", __func__, errno);
		//LOG(INFO)<< ": accept " << errno;
		return;
	}
	do {
		for (i = 0; i < MAX_EVENTS; i++) {
			if (g_Events[i].status == 0) {
				break;
			}
		}
		if (i == MAX_EVENTS) {
			//printf("%s:max connection limit[%d].", __func__, MAX_EVENTS);
			LOG(WARNING)<< ":max connection limit " << MAX_EVENTS;
			break;
		}
		// set nonblocking
		int iret = 0;
		if ((iret = fcntl(nfd, F_SETFL, O_NONBLOCK)) < 0) {
			//printf("%s: fcntl nonblocking failed:%d", __func__, iret);
			LOG(WARNING)<< ": fcntl nonblocking failed:" << iret;
			break;
		}
		getsockname(nfd, (struct sockaddr *) &sin, &len);
		char* _serverIp = inet_ntoa(sin.sin_addr);
		unsigned short _port = htons(sin.sin_port);
		// add a read event for receive data
		EventSet(&g_Events[i], nfd, _serverIp, _port, RecvData, &g_Events[i]);
		EventAdd(g_epollFd, EPOLLIN, &g_Events[i]);
	} while (0);
//	printf("new conn[%s:%d][time:%d], pos[%d]\n", inet_ntoa(sin.sin_addr),
//			ntohs(sin.sin_port), g_Events[i].last_active, i);
	//LOG(INFO)<< "new line" << std::string(inet_ntoa(sin.sin_addr));
}
// receive data
void RecvData(int fd, int events, void *arg) {
	struct myevent_s *ev = (struct myevent_s*) arg;
	int len;
	struct sockaddr_in sin;
	socklen_t sockLen = sizeof(struct sockaddr_in);
	// receive data
	len = recv(fd, ev->buff, sizeof(ev->buff), 0);
	//EventDel(g_epollFd, ev);
	if (len > 0) {
		ev->len = len;
		//ev->buff[len] = '\0';
		ev->last_active = time(NULL);
		//printf("C[%d]:%s\n", fd, ev->buff);
		LOG(INFO)<< fd << ev->buff;
		std::stringstream ss;
		ss << fd;
		std::string strfd;
		ss >> strfd;
		std::string filename = strfd +"_temp";
		if(std::string(ev->buff).find("7E45007E") != std::string::npos){
		}
		else if(std::string(ev->buff).find("7E457E45") != std::string::npos){//!g_filefd[fd] && len == 4
			std::cout << "make file for file" << std::endl;
			g_filefd[fd] = open(filename.c_str(),  O_WRONLY | O_CREAT | O_TRUNC,
						S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			//write(g_filefd[fd], ev->buff, ev->len);
		}
		else if(std::string(ev->buff).find("7E257E25") != std::string::npos){//len == 2
			close(g_filefd[fd]);
			g_filefd[fd] = 0;
			std::cout << "get done, check" << std::endl;
//			write(g_filefd[fd], ev->buff, ev->len);
		}
		else{
			write(g_filefd[fd], ev->buff, ev->len);
		}

	} else if (len == 0) {
		close(ev->fd);
		EventDel(g_epollFd, ev);
		//printf("[fd=%d] pos[%d], closed gracefully.\n", fd, ev - g_Events);
		//LOG(INFO) << fd <<"closed gracefully";
	} else {
		close(ev->fd);
		EventDel(g_epollFd, ev);
		//printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
	}
}
// send data
void SendData(int fd, int events, void *arg) {
	struct myevent_s *ev = (struct myevent_s*) arg;
	int len;
	struct sockaddr_in sin;
	socklen_t sockLen = sizeof(struct sockaddr_in);
	// send data
	len = send(fd, ev->buff + ev->s_offset, ev->len - ev->s_offset, 0);
	if (len > 0) {
		//printf("send[fd=%d], [%d<->%d]%s\n", fd, len, ev->len, ev->buff);
		ev->s_offset += len;
		if (ev->s_offset == ev->len) {
			// change to receive event
			//EventDel(g_epollFd, ev);
			getsockname(fd, (struct sockaddr *) &sin, &sockLen);
			char* _serverIp = inet_ntoa(sin.sin_addr);
			unsigned short _port = htons(sin.sin_port);
			EventSet(ev, fd, _serverIp, _port, RecvData, ev);
			EventAdd(g_epollFd, EPOLLIN, ev);
		}
	} else {
		close(ev->fd);
		EventDel(g_epollFd, ev);
		//printf("send[fd=%d] error[%d]\n", fd, errno);
	}
}
void InitListenSocket(int epollFd, short port) {
	int listenFd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(listenFd, F_SETFL, O_NONBLOCK); // set non-blocking
	//printf("server listen fd=%d\n", listenFd);

	sockaddr_in sin;
	socklen_t sockLen = sizeof(struct sockaddr_in);
	getsockname(listenFd, (struct sockaddr *) &sin, &sockLen);
	char* _serverIp = inet_ntoa(sin.sin_addr);
	unsigned short _port = htons(sin.sin_port);
	EventSet(&g_Events[MAX_EVENTS], listenFd, _serverIp, _port, AcceptConn,
			&g_Events[MAX_EVENTS]);
	// add listen socket
	EventAdd(epollFd, EPOLLIN, &g_Events[MAX_EVENTS]);
	// bind & listen
	//sockaddr_in sin;
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	bind(listenFd, (const sockaddr*) &sin, sizeof(sin));
	listen(listenFd, 5);
}

int Recv_TCP_Heart_Main() {
	unsigned short port = 12345; // default port

	for(int i = 0; i < MAX_EVENTS; i++){
		g_filefd[i] = 0;
	}
	// create epoll
	g_epollFd = epoll_create(MAX_EVENTS);
	if (g_epollFd <= 0){
		LOG(WARNING) << "create epoll failed";
	}
		//printf("create epoll failed.%d\n", g_epollFd);
	// create & bind listen socket, and add to epoll, set non-blocking
	InitListenSocket(g_epollFd, port);
	// event loop
	struct epoll_event events[MAX_EVENTS];
	//printf("server running:port[%d]\n", port);
	int checkPos = 0;
	while (1) {
		// a simple timeout check here, every time 100, better to use a mini-heap, and add timer event
		long now = time(NULL);
		for (int i = 0; i < 100; i++, checkPos++) // doesn't check listen fd
		{
			if (checkPos == MAX_EVENTS)
				checkPos = 0; // recycle
			if (g_Events[checkPos].status != 1)
				continue;
			long duration = now - g_Events[checkPos].last_active;
			if (duration >= 15) // 15s timeout
			{
				close(g_Events[checkPos].fd);
				//printf("[fd=%d] timeout[%d--%d].\n", g_Events[checkPos].fd,
				//g_Events[checkPos].last_active, now);
				EventDel(g_epollFd, &g_Events[checkPos]);
			}
		}
		// wait for events to happen
		int fds = epoll_wait(g_epollFd, events, MAX_EVENTS, 1000);
		if (fds < 0) {
			//printf("epoll_wait error, exit\n");
			LOG(WARNING)<< "epoll_wait error";
			break;
		}
		for (int i = 0; i < fds; i++) {
			myevent_s *ev = (struct myevent_s*) events[i].data.ptr;
			if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) // read event
					{
				ev->call_back(ev->fd, events[i].events, ev->arg);
			}
			if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) // write event
					{
				ev->call_back(ev->fd, events[i].events, ev->arg);
			}
		}
	}
	// free resource
	return 0;
}

std::map<int, std::string> GetControlledDev(){
	long now = time(NULL);
	int checkPos = 0;
	std::map<int, std::string> returnMap;
	for (int i = 0; i < 100; i++, checkPos++)
	{
		if (checkPos == MAX_EVENTS)
			checkPos = 0; // recycle
		if (g_Events[checkPos].status != 1)
			continue;
		long duration = now - g_Events[checkPos].last_active;
		if (duration >= 15){
			close(g_Events[checkPos].fd);
			//printf("[fd=%d] timeout[%d--%d].\n", g_Events[checkPos].fd,
			//g_Events[checkPos].last_active, now);
			EventDel(g_epollFd, &g_Events[checkPos]);
		}
		else{
			returnMap[g_Events[checkPos].fd] = std::string(g_Events[checkPos].ip);
		}
	}
	return returnMap;
}

#endif /* RECV_TCP_HEARTPACKET_H_ */
