/*

Name: Syed Sumair Zafar Student Id: 7099347
Name: Mandeep Singh Student Id: 7163738

*/
#define _CRT_SECURE_NO_DEPRECATE
#include <winsock.h>
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include "Thread.h"
#include <vector>
#include <direct.h>
#include <stdio.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")
using namespace std;

#define SER_TCP_H

#define HOSTNAME_LENGTH 20
#define RESP_LENGTH 40
#define FILENAME_LENGTH 20
#define REQUEST_PORT 5001
#define BUFFER_LENGTH 1024 
#define MAXPENDING 10
#define MSGHDRSIZE 8 //Message Header Size

typedef enum{
	REQ_SIZE=1,REQ_TIME,RESP //Message type
} Type;

typedef struct  
{
	char hostname[HOSTNAME_LENGTH];
	char filename[FILENAME_LENGTH];
} Req;  //request

typedef struct  
{
	char response[RESP_LENGTH];
} Resp; //response


typedef struct 
{
	Type type;
	int  length; //length of effective bytes in the buffer
	char buffer[BUFFER_LENGTH];
} Msg; //message format used for sending and receiving


//buffer data types

char szbuffer[128];
char sendbuffer[128];
char *buffer;
int ibufferlen;
int ibytesrecv;

int ibytessent;
int serverSock,clientSock;
//host data types

char localhost[11];

HOSTENT *hp;

//wait variables

int nsa1;
int r,infds=1, outfds=0;
struct timeval timeout;
const struct timeval *tp=&timeout;

fd_set readfds;  // . For each socket, the caller can request information on read, write, or error status. The set of sockets for which a given status is requested is indicated by an fd_set structure

//others

HANDLE test;
DWORD dwtest;

class Tcpserver 
{
	 /* Socket descriptor for server and client*/
	struct sockaddr_in ClientAddr; /* Client address */
	struct sockaddr_in ServerAddr; /* Server address */
	unsigned short ServerPort;     /* Server port */
	int clientLen;            /* Length of Server address data structure */
	char servername[HOSTNAME_LENGTH];

	public:
		 Tcpserver();
		~Tcpserver();
		void Tcpserver :: start();
};

class Tcpthread: public Thread
{
	int cs;
	SOCKET clientSock;

	public:
	  Tcpthread (SOCKET cs):clientSock(cs) {} 	  
	  virtual void run();

	private:
	  void sendFile();
	  void receiveFile();
	  void listFiles();
	  void deleteFile();
};

	
Tcpserver :: Tcpserver()
{	
	WSADATA wsadata;

	if (WSAStartup(0x0202,&wsadata)!=0)
	{  
		cout<<"Error in starting WSAStartup()\n";
	}
	else
	{
		buffer="WSAStartup was suuccessful\n";   
		WriteFile(test,buffer,sizeof(buffer),&dwtest,NULL); 

		/* display the wsadata structure */
		cout<< endl
			<< "wsadata.wVersion "       << wsadata.wVersion       << endl
			<< "wsadata.wHighVersion "   << wsadata.wHighVersion   << endl
			<< "wsadata.szDescription "  << wsadata.szDescription  << endl
			<< "wsadata.szSystemStatus " << wsadata.szSystemStatus << endl
			<< "wsadata.iMaxSockets "    << wsadata.iMaxSockets    << endl
			<< "wsadata.iMaxUdpDg "      << wsadata.iMaxUdpDg      << endl;
	}  

	//Display info of local host
	gethostname(servername,HOSTNAME_LENGTH);
	cout << "hostname: " << servername << endl;

	if((hp=gethostbyname(servername)) == NULL) 
	{ //hp is hostent variable.
		cout << "gethostbyname() cannot get local host info?"
			<< WSAGetLastError() << endl; //The WSAGetLastError function returns the error status for the last Windows Sockets operation that failed
		exit(1);
	}

	//Create the server socket

	//AF_INET A socket type that provides sequenced, reliable, two-way, connection-based byte streams with an OOB data transmission mechanism
	//this tell that TCP protocol is using but, why is that 0.
		
      if ((serverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)			
		throw "can't initialize socket";
		// For UDP protocol replace SOCK_STREAM with SOCK_DGRAM 


	ServerPort=REQUEST_PORT;
    memset(&ServerAddr, 0, sizeof(ServerAddr));      /* Zero out structure */
    ServerAddr.sin_family = AF_INET;                 /* Internet address family */
    ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    ServerAddr.sin_port = htons(ServerPort);         /* Local port */

	//Bind the server port

	 if (bind(serverSock, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr)) < 0)
		throw "can't bind the socket";

	cout << "Bind was successful" << endl;

	//Successfull bind, now listen for client requests.

	 //it indicates that the server is ready for connection and listen over socket s, then "10" specifies that 10 number of requests that can be queued.
	if (listen(serverSock, MAXPENDING) < 0)
		throw "couldn't  set up listen on socket";
	else cout << "Listen was successful" << endl;
}
	
			
void Tcpserver :: start()
{
		 
	for (;;) /* Run forever */
	{
		/* Set the size of the result-value parameter */
		clientLen = sizeof(ServerAddr);
		
		/* Wait for a Server to connect */
		if ((clientSock = accept(serverSock, (struct sockaddr *) &ClientAddr, &clientLen)) < 0)
			cout << "error in listining";
		
		//Connection request accepted.
		cout<<"accepted connection from "<<inet_ntoa(ClientAddr.sin_addr)<<endl;


   		Tcpthread * pt = new Tcpthread(clientSock);
		pt->start();
	}     
				
 }
		
       
 void Tcpthread :: run()
 {
	bool break_condition = false;
    while(break_condition == false)
	{
		char option[10] = {NULL};
		if ((ibytesrecv=recv(clientSock,option,10,0))==SOCKET_ERROR) //receivng option to send or receive file
		{
			//throw "error in receiving";
			break_condition = true;
		}

		int opt = atoi(option);
		switch(opt)
		{
			case 1:
				receiveFile();
				break;
			case 2:
				sendFile();
				break;
			case 4:
				listFiles();
				break;
			case 5:
				deleteFile();
				break;
            default :
		       cout << "Wating for client request." << endl;
			   break;
		}
	}
 }
  
vector<string> get_all_files_within_folder(string folder)
{
    vector<string> names;
    char search_path[200];
    sprintf_s(search_path, "%s*.", folder.c_str());
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(".\\*", &fd); 
    if(hFind != INVALID_HANDLE_VALUE) 
    { 
        do 
        { 
            // read all (real) files in current folder, delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
            {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
	return names;
}

void Tcpthread ::listFiles()
{
	char * dir = _getcwd(NULL, 0);
	string p;
	char getval[1];

	for(int i=0;dir[i] != 0;i++)
	{
		p += dir[i];
	}
	
	string c;
		
	vector<string> names_got = get_all_files_within_folder(p);

	for (vector<string>::iterator n = names_got.begin();n != names_got.end();++n)
	{
		c = c + *n + "\n";
	}
	
	char *cstr = &c[0];	
	
	if((send(clientSock,cstr,strlen(cstr),0)) == SOCKET_ERROR)
	{
		cout << "Error sending data to Client ::" << SOCKET_ERROR << endl;
	}
}

void Tcpthread ::deleteFile()
{
	char * dir = _getcwd(NULL, 0);	
	char name_rec[100];
	memset(name_rec, 0, 100);

	if((recv(clientSock,name_rec, 100, 0))==SOCKET_ERROR)
	{
		cout<<"\nError receiving filename "<<endl;
	}
	else
	{
		long len;
		char* file_buf = NULL;
		FILE* filePtr = NULL;
		string mystr = string(name_rec);
		string slashes = "\\";
		string path = dir + slashes + mystr;
		
		WIN32_FIND_DATA FindFileData;
	    HANDLE hFind;

		hFind = FindFirstFile(path.c_str(), &FindFileData);
	   if (hFind == INVALID_HANDLE_VALUE) 
	   {
		  printf ("FindFirstFile failed (%d)\n", GetLastError());
		  return;
	   } 
	   else 
	   {
		   cout << "\nClient is deleting '" << name_rec << "' file." << endl;
		   if(!DeleteFile(FindFileData.cFileName))
		   {
			   char text[30] = "\nError deleting file\n\n";
			   //send(clientSock,text,strlen(text),0);
		   }
		   else
		   {
				char text[30] = "\nFile successfully deleted\n\n";
				//send(clientSock,text,strlen(text),0);
		   }			
		  FindClose(hFind);
	   }
	}
}

void Tcpthread ::receiveFile()
{
	char name_rec[100];
	memset(name_rec, 0, 100);

	if((recv(clientSock,name_rec, 100, 0))==SOCKET_ERROR)
	{
		cout<<"Error receiving data filename "<<endl;
	}

	//prepearing buffer to receive the file length
	//Buffer to receive the file
	char file_size[100];
	long bufferLen = 100; 
	long recvByteSize;
	//Prepare a memory size for file
	memset(file_size, 0, bufferLen);
	
	//receiving file size
	if((recvByteSize = recv(clientSock,file_size, bufferLen, 0)) == SOCKET_ERROR)
		fprintf(stderr, "Error occurred while receiving data %d\n", WSAGetLastError());
	//if file not found at the server side
	int size = atoi(file_size); //convert the size to integer  
		
	printf("\nBytes to be received %d\n", size);

	
	long sizeCheck = 0;
	FILE* fileptr = fopen(name_rec,"w");
	char* mfcc;
	if(size > 1500)
	{
		mfcc = (char*)malloc(1500);
			while(sizeCheck < size){				
				int Received = recv(clientSock, mfcc, 1500, 0);
				sizeCheck += Received;
				fwrite(mfcc, 1, Received, fileptr);
				fflush(fileptr);
				printf("Filesize: %d\nSizecheck: %d\nReceived: %d\n\n", size, sizeCheck, Received);
			}
	}
	else
	{
	   //Prepare a buffer that loads the contents of the file 
		//char buffer[100];
		char *buffer = NULL;
		buffer = (char*)malloc(size);
		long buffer_len = size;
		long byteRecv;

		memset(buffer, 0, buffer_len);
		if((byteRecv = recv(clientSock, buffer, buffer_len, 0))==SOCKET_ERROR)
		{
			fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
		}
	
		//create a File pointer
		FILE* fileptr = NULL;
		fileptr = fopen(name_rec, "wb" );  //open the received file

		//check written file size
		if (!fwrite (buffer, 1, byteRecv, fileptr))
		{
			fclose(fileptr);
			cout<< "could not write file\n"<< endl;
		}
		else 
		{
			fclose( fileptr );
			cout << "\n File "<<name_rec<< " has been downloaded "<< endl;
		}
		//end receiving the file from server
	}

	fclose(fileptr); // close file
	return;				
				
}	        
	
//function to send file to the clien
void Tcpthread :: sendFile()
{
		char name_rec[100];
		memset(name_rec, 0, 100);
		if((recv(clientSock,name_rec, 100, 0))==SOCKET_ERROR)
		{
			cout<<"Error receiving data filename "<<endl;
		}

		long len;
		char* file_buf = NULL;
		FILE* filePtr = NULL;
		filePtr = fopen(name_rec, "rb" );
		cout<<"client requested file "<< name_rec <<" to be sent..";

		if(!filePtr) 
		{ 
			cout<<"\nthe file is not exist : ";
			if(send(clientSock,"-1", strlen("-1"), 0) == SOCKET_ERROR)
			{ 
				cout << "Error in sending data "<<endl;
			}
			return;
		 }

		if (fseek( filePtr, 0, SEEK_END ) != 0)
		{
			fclose( filePtr );
		}
		//telling the length of fileptr
		
		len = ftell( filePtr );
		rewind( filePtr );
		file_buf = (char*)malloc( len );
		if (!buffer){ fclose( filePtr ); }
		if (!fread(file_buf, len, 1, filePtr)){ fclose( filePtr ); }
		fclose( filePtr );

		//sending the file size
		int size_buf;
		char file_size [33];
		_itoa (len,file_size,10);
		size_buf=strlen(file_size);
		int bytesnd;
		if( (bytesnd= send(clientSock, file_size, size_buf,0))==SOCKET_ERROR)
		{
			cout<< "Error sending data "<<endl;
		}

		cout<<"Bytes needs to be send "<<len<<endl;

		char* mfcc;
		long sizeCheck = 0;
		filePtr = fopen(name_rec, "rb" );
		int bytecount;

		if(len > 1500)
		{
			mfcc = (char*)malloc(1500);
			while(sizeCheck < len) {
				int Read = fread_s(mfcc, 1500, sizeof(char), 1500, filePtr);
				int Sent = send(clientSock, mfcc, Read, 0);
				sizeCheck += Sent;
				//cout << Sent << endl;
			}
		}
		else if((bytecount=send(clientSock,file_buf, len,0))==SOCKET_ERROR)
		{
			fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
		}
		printf("\nSent bytes %d\n", len);
		fclose(filePtr);
		Sleep(10000);
		free(mfcc);
		return;
}
				
 
Tcpserver::~Tcpserver()
{
	closesocket(clientSock);		

	closesocket(serverSock);

	WSACleanup();	
}



int main(void)
{
	
    Tcpserver ts;
	ts.start();
	_getch();

	return 0;
}	
