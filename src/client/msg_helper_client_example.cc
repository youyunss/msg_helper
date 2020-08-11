#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include "msg_helper.h"

#define MAXBUF 1*1024*1024+48

//例子：client发送类型为3的消息包含完整的BlockIndex数据
using namespace std;

int main(int argc, char **argv)
{
	int sockfd, len, new_fd;
	struct sockaddr_in dest;
	fd_set rfds;
	struct timeval tv;
	int retval, maxfd=-1;
	socklen_t sendlen, recvlen;
	
	char buf[MAXBUF + 1];
	string sendmsg;
	msg_head_t *sendmsghdr = new msg_head_t;
	host_head *sendhosthdr = new host_head;
	MsgHelper sendhelper;

	sendhosthdr->msgtype = ask_for_block;
	sendhosthdr->fileid = 1;
	sendhosthdr->blockid = 1;
	string str1 = "1234123412341234";
        strncpy(sendhosthdr->filemd5, str1.c_str(), 16);
        string str2 = "4321432143214321";
	strncpy(sendhosthdr->blockmd5, str2.c_str(), 16);

	Block_Index *sendblockindex = new Block_Index[2];
	sendhosthdr->length = 2*sizeof(Block_Index);
	string str3 = "AAAABBBBCCCCDDDD";
        strncpy(sendblockindex[0].blockmd5, str3.c_str(), 16);
        sendblockindex[1].blockid = 4;
        string str4 = "EEEEFFFFGGGGHHHH";
	strncpy(sendblockindex[1].blockmd5, str4.c_str(), 16);

	if (argc != 3)
	{
		printf(" error format, it must be:\n\t\t%s IP port\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Socket");
		exit(errno);
	}
	printf("socket craeted\n");
	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;					
	dest.sin_port = htons(atoi(argv[2]));			
	if (inet_aton(argv[1], (struct in_addr*) &dest.sin_addr.s_addr) == 0)			
	{
		perror(argv[1]);
		exit(errno);
	}
	if (connect(sockfd, (struct sockaddr*)&dest, sizeof(dest)) == -1)
	{
		perror("Connect ");
		exit(errno);
	}
	printf("server connected\nget ready pls chat\n");
	//while (1)
	//{
	/*	FD_ZERO(&rfds);
		FD_SET(0, &rfds);       //将stdin加入文件描述符集合
		FD_SET(sockfd, &rfds);  //将new_fd加入文件描述符集合
		maxfd=sockfd;
		tv.tv_sec=1;
		tv.tv_usec=0;           //阻塞时间1s
		retval = select(maxfd+1, &rfds, NULL, NULL, &tv);     //多路复用，监测stdin和new_fd
		if (retval == -1)
		{
			perror("select");
			exit(EXIT_FAILURE);
		}
		else if (retval == 0)      //超时，继续执行
		{
			continue;
		}
		else
		{
			if (FD_ISSET(0, &rfds))    //监听到stdin有异常
			{*/
				cout<<"send begin"<<endl;
				bzero(buf, MAXBUF + 1);
				sendhelper.set_msg_head(sendmsghdr, sendhosthdr);
				sendhelper.set_msg(sendmsg, sendblockindex, 2*sizeof(Block_Index));
				int data_length = sendhelper.pack_msg(buf, MAXBUF, sendmsghdr, sendmsg);
				sendlen = write(sockfd, buf, data_length);
				cout<<"blockid1\t"<<sendblockindex[1].blockid<<endl;
				char testblockmd5[17];
				memcpy(testblockmd5, sendblockindex[1].blockmd5, 16);
				memcpy(testblockmd5+16, "\0", 1);
				cout<<"blockmd51\t"<<testblockmd5<<endl;
				cout<<"send successful\t"<<sendlen<<endl;
			//}
		//}
	//}
	delete sendmsghdr;
	delete sendhosthdr;
	delete sendblockindex;
	close(new_fd);
	close(sockfd);
	return 0;
}
