///*				TCPRCV.C
//*/
///* This is the receiver program. It opens a socket, sets the receive
//window as requested, and then begins an infinite loop. Each time through
//the loop it accepts a connection and prints out messages from it. When
//the connection breaks or a termination message comes through it, the
//program accepts a new connection.  Compile the program on a unix system
//by using gcc -o tcprcv tcprcv.c
//The format of the command line is:  tcprcv
//*/
//
//
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <sys/sendfile.h>
//#include <fcntl.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <stdio.h>
//#include <errno.h>
//#include <sys/time.h>
//#include <stdlib.h>
//#include <strings.h>
//
//
//#define TRUE 1
//#define SERVER_PORT 50102
//#define LENGTH 1000			//1K 
//#define SEQ_LEN 24			//sequence number bytes 
//
//
//main()
//{
//	int sd, length;
//	struct sockaddr_in rcvr;
//	int sda;
//	int buflen = 512;
//	char buf[512];
//	int rcvwin, optlen;
//	int rval;
//	int seqno;
//	ssize_t len;
//
//	rcvwin = buflen;
//
//	/* create socket */
//	sd = socket(AF_INET, SOCK_STREAM, 0);
//	if (sd < 0) {
//		perror("opening stream socket");
//		exit(1);
//	}
//
//	/* name socket using wildcards */
//	rcvr.sin_family = AF_INET;
//	rcvr.sin_addr.s_addr = INADDR_ANY;
//	rcvr.sin_port = htons(SERVER_PORT);
//	if (bind(sd, (struct sockaddr *)&rcvr, sizeof(rcvr))) {
//		perror("binding socket name");
//		exit(1);
//	}
//
//	/* find out assigned port number and print out */
//	length = sizeof(rcvr);
//	if (getsockname(sd, (struct sockaddr *)&rcvr, &length)) {
//		perror("getting socket name");
//		exit(1);
//	}
//	printf("Socket has port #%d\n", ntohs(rcvr.sin_port));
//
//
//	/* Can get same effect by doing reads of the correct size. */
//	rcvwin = buflen;
//
//	/* Accept connections from the transmitter */
//	listen(sd, 5);
//	do {
//
//		sda = accept(sd, 0, 0);
//		if (sda == -1)
//			perror("accept");
//		else
//			do {
//				bzero(buf, sizeof(buf));
//				if ((rval = recv(sda, buf, rcvwin, MSG_WAITALL))<0)
//					perror("reading message stream");
//				if (rval == 0)
//					printf("Ending connection\n");
//				else {
//					sscanf(buf, "%d", &seqno);
//					printf("Received packeta: seqno = %d length = %d\n, buf = %s\n ", seqno, rval, buf);
//
//					//Send File to windows Client
//					char* fileName = "/home/ad.csueastbay.edu/fw4793/projects/linuxSocket/data10K.txt";
//					char sdbuf[LENGTH];
//					printf("Server Sending file to the Client...\n");
//					FILE *fs = fopen(fileName, "r");
//					if (fs == NULL)
//					{
//						printf("ERROR: File %s not found.\n", fileName);
//						exit(1);
//					}
//
//					bzero(sdbuf, LENGTH);
//					int fs_block_sz;
//					int seqNum = 666;
//					char totalBuf[LENGTH + SEQ_LEN];
//					char seqArray[SEQ_LEN];
//					while ((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs))>0)
//					{
//						//add sequence number at the front.
//						sprintf(seqArray, "SeqNum:(%d)", seqNum);
//						strcpy(totalBuf, seqArray);
//						strcpy(totalBuf + SEQ_LEN, sdbuf);
//						if (send(sda, totalBuf, fs_block_sz + SEQ_LEN, 0) < 0)
//						{
//							printf("ERROR: Failed to send file %s.\n", fileName);
//							break;
//						}
//						bzero(sdbuf, LENGTH);
//						seqNum += 1;
//						usleep(250);
//					}
//					//printf("File %s was Sent!\n", fileName);
//					printf("File was Sent!\n");
//					//end of Send File to windows Client
//
//
//				}
//			} while (rval != 0);
//			close(sda);
//
//	} while (TRUE);
//
//}