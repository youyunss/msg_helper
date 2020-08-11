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

//例子：写入消息3读出消息3。
using namespace std;

int main(int argc, char **argv)
{
	int fd;
	int recvlen, sendlen;	
	char buf[MAXBUF + 1];
	string sendmsg;
	string recvmsg;
	msg_head_t *sendmsghdr = new msg_head_t;
	msg_head_t *recvmsghdr = new msg_head_t;
	host_head *sendhosthdr = new host_head;
	host_head *recvhosthdr = new host_head;
	MsgHelper sendhelper;
	MsgHelper recvhelper;

	sendhosthdr->msgtype = ask_for_block;
	sendhosthdr->fileid = 1;
	sendhosthdr->blockid = 1;
	string str1 = "12341234123412341234123412341234";
	strncpy(sendhosthdr->filemd5, str1.c_str(), 32); 
	string str2 = "43214321432143214321432143214321";
	strncpy(sendhosthdr->blockmd5, str2.c_str(), 32); 

	Block_Index *sendblockindex = new Block_Index[2];
	Block_Index *recvblockindex = new Block_Index[2];
	sendblockindex[0].blockid = 3;
	string str3 = "AAAABBBBCCCCDDDDAAAABBBBCCCCDDDD";
	strncpy(sendblockindex[0].blockmd5, str3.c_str(), 32); 
	sendblockindex[1].blockid = 4;
	string str4 = "EEEEFFFFGGGGHHHHAAAABBBBCCCCDDDD";
	strncpy(sendblockindex[1].blockmd5, str4.c_str(), 32); 
	sendhosthdr->length = 2*sizeof(Block_Index);
	string str5 = "This is luzhiqiang's file";
	strncpy(sendhosthdr->filename, str5.c_str(), 32);

	//发送数据包是需要依次调用set_msg_head, set_msg, pack_msg. 而且需要先给host_head赋值,定义msg_head_t, string msg,发送的数据
	cout<<"send data"<<endl;
	bzero(buf, MAXBUF + 1);
	sendhelper.set_msg_head(sendmsghdr, sendhosthdr);
	sendhelper.set_msg(sendmsg, sendblockindex, sendhosthdr->length);
	int data_length = sendhelper.pack_msg(buf, MAXBUF, sendmsghdr, sendmsg);
	fd=open("tmp1", O_RDWR|O_CREAT, 0644);
	sendlen = write(fd, buf, data_length);
	lseek(fd, 0, SEEK_SET);
	cout<<"sendfileid\t"<<sendhosthdr->fileid<<endl;
	cout<<"sendfilemd5\t"<<sendhosthdr->filemd5<<endl;
	cout<<"sendblockid1\t"<<sendblockindex[1].blockid<<endl;
	char testsendblockmd5[33];
	memcpy(testsendblockmd5, sendblockindex[1].blockmd5, 32);
	memcpy(testsendblockmd5+32, "\0", 1);
	cout<<"sendblockmd51\t"<<testsendblockmd5<<endl;
	cout<<"sendfilename"<<sendhosthdr->filename<<endl;
	cout<<"send successful\t"<<sendlen<<endl;

	//解析收取的包需要依次调用check_msg，get_msg_head, unpack_msg, get_msg. 而且需要先定义string msg, 接收的数据类型
	cout<<"recv data"<<endl;
	bzero(buf, MAXBUF + 1);
	recvlen = read(fd, buf, sizeof(msg_head_t));
	int recvmsg_length=recvhelper.check_msg(buf, recvmsghdr, recvlen);
	bzero(buf, recvmsg_length);
	recvhelper.get_msg_head(recvmsghdr, recvhosthdr);
	cout<<"recvfileid\t"<<recvhosthdr->fileid<<endl;
	cout<<"recvfilemd5\t"<<recvhosthdr->filemd5<<endl;
	recvlen = read(fd, buf, recvmsg_length);
	recvhelper.unpack_msg(buf, recvlen, recvmsg);
	recvhelper.get_msg(recvmsg, recvblockindex, 2*sizeof(Block_Index));
	cout<<"blockid1\t"<<recvblockindex[1].blockid<<endl;
	char testblockmd5[33];
	memcpy(testblockmd5, recvblockindex[1].blockmd5, 32);
	memcpy(testblockmd5+32, "\0", 1);
	cout<<"blockmd51\t"<<testblockmd5<<endl;
	cout<<"sendfilename"<<recvhosthdr->filename<<endl;
	cout<<"recv successful\t"<<recvhosthdr->length+MSG_HEAD_LENGTH<<endl;
			
	delete sendmsghdr;
	delete recvmsghdr;
	delete sendhosthdr;
	delete recvhosthdr;
	delete[] sendblockindex;
	delete[] recvblockindex;
	close(fd);
	return 0;
}
