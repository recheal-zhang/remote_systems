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
 * wait_user_cmd_exec.h
 *
 *  Created on: Oct 18, 2016
 *      Author: zlc
 */

#ifndef WAIT_USER_CMD_EXEC_H_
#define WAIT_USER_CMD_EXEC_H_

#include <iostream>
#include <string>
#include <map>
#include "recv_tcp_heartPacket.h"
#include "stdlib.h"
#include "unistd.h"
#include <stdio.h>
#include "pthread.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "fcntl.h"

void download(std::string path, int sockfd) {
	pid_t p1, p2, p3;
	int status;
	char ch;
	if ((p1 = fork()) == 0) {
		close(1);
		dup(sockfd);
		execl("/bin/echo", "/bin/echo", "7E457E45", NULL);
		exit(0);
	}
	else
	{
		wait(&status);
		std::cout << "send start" << std::endl;
		if ((p2 = fork()) == 0) {
			close(1);
			dup(sockfd);
			execl("/bin/cat", "/bin/cat", path.c_str(), NULL);
			exit(0);
		} else {
			wait(&status);
			std::cout << "send all" << std::endl;
			if ((p3 = fork()) == 0) {
				close(1);
				dup(sockfd);
				execl("/bin/echo", "/bin/echo", "7E257E25", NULL);
				exit(0);
			} else {
				wait(&status);
			}
		}
	}
}

void GetUserCmd_Exec(){
	std::string str;
	std::map<int, std::string> mymap;
	while(getline(std::cin, str)){
		if(!str.compare("lsOCDev")){
			mymap = GetControlledDev();
			if(mymap.size() == 0){
				std::cout << "map size = " << mymap.size() << std::endl;
				//continue;
			}
			for(std::map<int, std::string>::iterator it = mymap.begin();
					it != mymap.end(); it++){
				std::cout << "fd = " <<  it->first
						<< "\t IP = " << it->second << std::endl;
			}
		}
		else if(str.size() > 4){
			std::string ping = str.substr(0,4);
			if(ping.compare("ping") != 0){
				continue;
			}

			size_t first_blank = str.find(' ');
			size_t second_blank = str.find(' ', first_blank + 1);
			std::string s_fd = str.substr(first_blank + 1, second_blank - first_blank);

			if(mymap.find(atoi(s_fd.c_str())) == mymap.end()){
				std::cout << "input fd =" << s_fd << std::endl;
				std::cout << "fd cannot find" << std::endl;
				continue;
			}

			//std::cout << s_fd << std::endl;

			std::string cmd = str.substr(second_blank+1);
			//std::cout << cmd << std::endl;

			std::string cmd_msg = "7E45" + str + " 7E";
			//TODO: SEND exe file
			if(cmd.find("send") != std::string::npos){
				size_t cmdSendPos = cmd.find(' ');
				std::string cmdPath = cmd.substr(cmdSendPos + 1);
				download(cmdPath, atoi(s_fd.c_str()));
				continue;
			}

			write(atoi(s_fd.c_str()), static_cast<const void*>(cmd_msg.c_str()),
					cmd_msg.size() + 1);

			//std::cout << "has write" << std::endl;
		}
	}
}



#endif /* WAIT_USER_CMD_EXEC_H_ */
