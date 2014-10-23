/*

Name: Syed Sumair Zafar Student Id: 7099347
Name: Mandeep Singh Student Id: 7163738

*/
#define _CRT_SECURE_NO_DEPRECATE
char* getmessage(char *);
#pragma comment (lib, "Ws2_32.lib")
#include <conio.h>
#include <winsock.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <windows.h>
#include<istream>
#include<stdlib.h>
#include <sstream>
#include <string>
using namespace std;


//user defined port number
#define REQUEST_PORT 5001;

int port=REQUEST_PORT;

//socket data types
SOCKET s;
SOCKADDR_IN sa;         // filled by bind
SOCKADDR_IN sa_in;      // fill with server info, IP, port

//buffer data types
char szbuffer[128];

char *buffer;
char recbuffer[128];
int ibufferlen=0;

int ibytessent=0;
int ibytesrecv=0;


char option;
//host data types
HOSTENT *hp;
HOSTENT *rp;

char localhost[11],
     remotehost[11];


HANDLE test;
DWORD dwtest;

void receiveFile(SOCKET, char *, int);
void sendFile(SOCKET, char *, int);
void listFiles(SOCKET, char *, int);
void deleteFile(SOCKET, char *, int);

string getFilename();
struct in_addr addr = { 0 };


int main(void)
{
	  
	WSADATA wsadata;

	try {

		if (WSAStartup(0x0202,&wsadata)!=0)
		{  
			cout<<"Error in starting WSAStartup()" << endl;
		} 
		else 
		{
			buffer="WSAStartup was successful\n";   
			WriteFile(test,buffer,sizeof(buffer),&dwtest,NULL); 

			/* Display the wsadata structure */
			cout<< endl
				<< "wsadata.wVersion "       << wsadata.wVersion       << endl
				<< "wsadata.wHighVersion "   << wsadata.wHighVersion   << endl
				<< "wsadata.szDescription "  << wsadata.szDescription  << endl
				<< "wsadata.szSystemStatus " << wsadata.szSystemStatus << endl
				<< "wsadata.iMaxSockets "    << wsadata.iMaxSockets    << endl
				<< "wsadata.iMaxUdpDg "      << wsadata.iMaxUdpDg      << endl;
		}  

		
		
		//Display name of local host.
		gethostname(localhost,10);
		cout<<"Local host name is \"" << localhost << "\"" << endl;

		if((hp=gethostbyname(localhost)) == NULL) 
			throw "gethostbyname failed\n";

		//Ask for name of remote server
		cout << "\nplease enter your remote server name :" << flush ;   
		cin >> remotehost ;
		cout << "Remote host name is: \"" << remotehost << "\"" << endl;

		if((rp=gethostbyname(remotehost)) == NULL)
			throw "remote gethostbyname failed\n";
		

		//Create the socket
		if((s = socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET) 
			throw "Socket failed\n";
		/* For UDP protocol replace SOCK_STREAM with SOCK_DGRAM */
		
		//Specify server address for client to connect to server.
		memset(&sa_in,0,sizeof(sa_in));
		memcpy(&sa_in.sin_addr,rp->h_addr_list[0],rp->h_length);
		sa_in.sin_family = rp->h_addrtype;   
		sa_in.sin_port = htons(port);

		//Display the host machine internet address
		cout << "Connecting to remote host:";
		cout << inet_ntoa(sa_in.sin_addr) << endl;

		//Connect Client to the server
		if (connect(s,(LPSOCKADDR)&sa_in,sizeof(sa_in)) == SOCKET_ERROR)
			throw "connect failed\n";
		else 
			cout << "Connection to the server has been successfully made" << endl;
		
		
		while(1)
		{
			int opt;
			string opt1;
			char option[10];

			cout << "\n\tTo send the file please type send: ";
			cout << "\n\tTo get the file please type get: ";
			cout << "\n\tTo see all files please type list: ";
			cout << "\n\tTo delete a file please type delete: ";
			cout << "\n\tTo exit please type quit: ";
			cin >> opt1;
			
			if(opt1 == "send")
				opt = 1;
			else if(opt1 == "get")
				opt = 2;
			else if(opt1 == "quit")
				opt = 3;
			else if(opt1 == "list")
				opt = 4;
			else if(opt1 == "delete")
				opt = 5;
			else
				opt = 99;

			int length;

			if(opt < 10)
			{
				_itoa(opt, option, 10);
				length	= strlen(option);

				switch(opt)
				{
				case 1:
					sendFile(s, option, length);
					break;
				case 2:
					receiveFile(s, option, length);
					break;
				case 3:
					exit(0);
					break;
				case 4:
					listFiles(s,option,length);
					break;
				case 5:
					deleteFile(s,option,length);
					break;
				default :
					cout << "Invalid option!" << endl;
					cin.clear(); // Clears the input stream fail flag
					cin.sync(); // Ignores any characters left in the stream
					break;
				}
			}
			else
			{
				cout << "Invalid option!" << endl;
				cin.clear(); // Clears the input stream fail flag
				cin.sync(); // Ignores any characters left in the stream
			}
		}
				
	} // try loop

	   //Display any needed error response.
	catch (char *str) { cerr<<str<<":"<<dec<<WSAGetLastError()<<endl;}

	//close the client socket
	closesocket(s);

	/* When done uninstall winsock.dll (WSACleanup()) and exit */ 
	WSACleanup();  
	_getch();
	return 0;
}

void deleteFile(SOCKET s,char* option,int length)
{
	string filename;
	char * name_send;

	cout << "\n\n*-----------------------------------------*"<<endl;
	cout << "\nPlease enter the filename: ";
	cin >> filename;

	name_send = &filename[0];

	sprintf(szbuffer, option);
	ibytessent=0;    
		ibufferlen = strlen(szbuffer);
		ibytessent = send(s,szbuffer,ibufferlen,0);
		if (ibytessent == SOCKET_ERROR)
			cout << "Delete failed\n"; 
		else
		{
			ibytessent = send(s,name_send, 100, 0);
			if (ibytessent == SOCKET_ERROR)
				cout << "Delete failed\n";						
		}

		/*ibytesrecv=0; 
		if((ibytesrecv = recv(s,szbuffer,400,0)) == SOCKET_ERROR)
			throw "Connection error\n";
		else 
		{	
			printf("%s",szbuffer);				
		}*/		
		cout << " \n****************************************************" << endl;
		cout << "File '" << name_send << "' is deleted from the server." << endl;
		cout << "****************************************************\n" << endl;
		return;
}

void listFiles(SOCKET s,char* option,int length)
{
	sprintf(szbuffer,"4"); 

		ibytessent=0;    
		ibufferlen = strlen(szbuffer);
		ibytessent = send(s,szbuffer,ibufferlen,0);
		if (ibytessent == SOCKET_ERROR)
			throw "Send failed\n";  

		ibytesrecv=0; 
			if((ibytesrecv = recv(s,szbuffer,400,0)) == SOCKET_ERROR)
				throw "Receive failed\n";
			else 
			{				
				cout << "\n********** Files on Server **********\n";
				printf("%s",szbuffer);
				cout << "\n **************************** ";
			}			
}

void sendFile(SOCKET s, char* option, int length) 		
{
		char * name_send;
		string filename;
		cout << "\n\n*-----------------------------------------*"<<endl;
		cout<<"\n Enter the name of file to send: ";
		cin >> filename;
		name_send = &filename[0];

		long len;
		char* file_buf = NULL;
		FILE* filePtr = NULL;
		filePtr = fopen(name_send, "rb" );

		if(!filePtr) 
		{ 
			cout<<"\nthe file does not exist : ";			
			return;
		 }

		send (s,option,length,0);    //sending option to send or receive file

		//send filename to the server
		if( (send(s,name_send, 100, 0))==SOCKET_ERROR)
		{
			fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
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
		

		//sending the file size
		int size_buf;
		char file_size [33];
		_itoa (len,file_size,10);
		size_buf=strlen(file_size);
		int bytesnd;
		
		if( (bytesnd= send(s, file_size, size_buf,0))==SOCKET_ERROR)
		{
			cout<< "Error sending data "<<endl;
		}

		cout<<"Bytes needs to be send "<<len<<endl;
		//sending contents of file
		int bytecount;
		
		char* mfcc;
		long sizeCheck = 0;
		filePtr = fopen(name_send, "rb" );

		if(len > 1500)
		{
			mfcc = (char*)malloc(1500);
			while(sizeCheck < len) {
				int Read = fread_s(mfcc, 1500, sizeof(char), 1500, filePtr);
				int Sent = send(s, mfcc, Read, 0);
				sizeCheck += Sent;
				//cout << Sent << endl;
			}
		}
		else if( (bytecount=send(s,file_buf, len,0))==SOCKET_ERROR)
		{
			cout<< "Error sending data " << endl;
		}
		cout<<"file " << name_send<<" of size " << len << " bytes has been successfully send" << endl;
			fclose(filePtr);
			Sleep(10000);
			free(mfcc);
		return;
		
}
	
	

void receiveFile(SOCKET s, char* option, int length)
{
		listFiles(s,0,0);
	    char * name_send;
		string filename;
		cout << "\n\n*-----------------------------------------*"<<endl;
		cout << " Enter the name of file to receive : ";
		cin >> filename;
		name_send = &filename[0];

		send (s,option,length,0);    //sending option to send or receive file

		//send filename to the server
		if( (send(s,name_send, 100, 0))==SOCKET_ERROR)
		{
			fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
			return;
		}

		//prepearing buffer to receive the file length
		//Buffer to receive the file
		char file_size[100];
		long bufferLen = 100; 
		long recvByteSize;
		//Prepare a memory size for file
		memset(file_size, 0, bufferLen);
		
		//receiving file size
		if((recvByteSize = recv(s,file_size, bufferLen, 0)) == SOCKET_ERROR)
			fprintf(stderr, "Error occurred while receiving data %d\n", WSAGetLastError());
		//if file not found at the server side
		int size = atoi(file_size); //convert the size to integer  
		if (size==-1){
			
			cout <<"\n\t The file was not found"<<endl;
		     return;
		}
		printf("\nBytes to be received %d\n", size);

		long sizeCheck = 0;
		FILE* fileptr = fopen(name_send,"w");
		char* mfcc;
		if(size > 1500)
		{
			mfcc = (char*)malloc(1500);
				while(sizeCheck < size){				
					int Received = recv(s, mfcc, 1500, 0);
					sizeCheck += Received;
					fwrite(mfcc, 1, Received, fileptr);
					fflush(fileptr);
					printf("Filesize: %d\nSizecheck: %d\nReceived: %d\n\n", size, sizeCheck, Received);
				}
		}
		else
		{
		   //Prepare a buffer that loads the contents of the file 
			char *buffer = NULL;
			buffer = (char*)malloc(size);
			long buffer_len = size;
			long byteRecv;

			memset(buffer, 0, buffer_len);
			if((byteRecv = recv(s, buffer, buffer_len, 0))==SOCKET_ERROR)
				fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
		
			//create a File pointer
			FILE* fileptr = NULL;
			fileptr = fopen(name_send, "wb" );  //open the received file

			//check written file size
			if (!fwrite (buffer, 1, byteRecv, fileptr))
			{
				fclose( fileptr );
				throw "could not write file\n";
			}
			else 
			{
				fclose( fileptr );
				cout << "\n File " << name_send << " has been downloaded "<< endl;
			}
		}
		//end receiving the file from server

		fclose(fileptr); // close file
	return;
}
