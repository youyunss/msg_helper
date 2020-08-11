#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "msg_helper.h"

#define MAXBUF 1*1024*1024+48
#define MAXFDS 8

//例子：proxy接收client发来的类型为3的消息解析出完整的BlockIndex数据

using namespace std;

int main(int argc, char **argv)
{
	int sockfd, new_fd, epollfd;
	socklen_t recvlen, sendlen, len;
	struct sockaddr_in my_addr, their_addr;
	unsigned int myport, lisnum;

	int ndfs = -1;
	int quitflag = 0;
	struct epoll_event ev0, ev1, events[MAXFDS];

	char buf[MAXBUF + 1];
	string recvmsg;
	msg_head_t *recvmsghdr = new msg_head_t;
	host_head *recvhosthdr = new host_head;
	Block_Index *recvblockindex =new Block_Index[2];
	MsgHelper recvhelper;

	if (argv[2])
	{
		myport = atoi(argv[2]);
	}
	else
	{
		myport = 7575;
	}
	if (argv[3])
	{
		lisnum = atoi(argv[3]);
	}
	else
	{
		lisnum = 5;
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	bzero(&my_addr, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(myport);
	if (argv[1])
	{
		my_addr.sin_addr.s_addr = inet_addr(argv[1]);
	}
	else
	{
		my_addr.sin_addr.s_addr = INADDR_ANY;
	}

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}
	if (listen(sockfd, lisnum) == -1)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

/*	while (1)
	{
		printf("\n-----wait for new connect\n");
		len = sizeof(struct sockaddr);
*/		if ((new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &len)) == -1)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("server: got connection from %s, port %d, socket %d\n", inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), new_fd);
		}
		/*while (1)
		{
			epollfd = epoll_create(MAXFDS); //epoll实例句柄
			ev0.data.fd = 0;
			ev0.events = EPOLLIN | EPOLLET; //边沿触发
			epoll_ctl(epollfd, EPOLL_CTL_ADD, 0, &ev0);   //将new_fd加入epoll句柄
			ev1.data.fd = new_fd;
			ev1.events = EPOLLIN | EPOLLET; //边沿触发
			epoll_ctl(epollfd, EPOLL_CTL_ADD, new_fd, &ev1);   //将new_fd加入epoll句柄
			ndfs = epoll_wait(epollfd, events, 2, -1);   //监听事件,永久阻塞
			if (ndfs == -1)
			{
				perror("epoll");
				exit(EXIT_FAILURE);
			}
			else if (ndfs == 0)
			{
				continue;
			}
			else
			{
				for (int i = 0; i<ndfs; i++)
				{
					if (events[i].data.fd == new_fd)//监听到new_fd有异常
					{*/
						//解析收取的包需要依次调用check_msg，get_msg_head, unpack_msg, get_msg
						cout<<"recv begin"<<endl;
						bzero(buf, MAXBUF + 1);
						recvlen = read(new_fd, buf, sizeof(msg_head_t));
						int recvmsg_length=recvhelper.check_msg(buf, recvmsghdr, recvlen);
						recvhelper.get_msg_head(recvmsghdr, recvhosthdr);
						cout<<"fileid\t"<<recvhosthdr->fileid<<endl;
						cout<<"filemd5\t"<<recvhosthdr->filemd5<<endl;
						bzero(buf, recvlen);
						recvlen = recv(new_fd, buf, recvmsg_length, 0);
						recvhelper.unpack_msg(buf, recvlen, recvmsg);
    					        recvhelper.get_msg(recvmsg, recvblockindex, 2*sizeof(Block_Index));
    					        cout<<"blockid1\t"<<recvblockindex[1].blockid<<endl;
						char testblockmd5[17];
						memcpy(testblockmd5, recvblockindex[1].blockmd5, 16);
						memcpy(testblockmd5+16, "\0", 1);
						cout<<"blockmd51\t"<<testblockmd5<<endl;
					/*}
				}
				if (quitflag)
				{
					quitflag = 0;
					break;
				}
			}
		}*/
		close(new_fd);
		//close(epollfd);
		//printf("need other connect (no->quit)\n");
		//fflush(stdout);       //刷新标准输出
		//bzero(buf, MAXBUF+1);
		//fgets(buf, MAXBUF, stdin);
		/*if (!strncasecmp(buf, "no", 2))
		{
			printf("quit!\n");
			break;
		}*/
	delete recvmsghdr;
	delete recvhosthdr;
	delete recvblockindex;
	close(sockfd);
	return 0;
}
