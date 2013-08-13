#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include  <stdint.h>
#include <signal.h>
#include "serverLibraryHeader.h"


#define RHINO_NUMBER		1
#define BOARD_ACTIVE 			1
#define BOARD_INACTIVE		0
#define LOAD_BOF_FILE 		3
#define REGISTER_WRITE 		1
#define REGISTER_READ 		2
#define KILL_BOF_PROCESS 	4
#define LIST_REGISTERS 		5
#define SUCCESS    		1
#define FAILURE	   		0
#define RHINOID			0
#define FPGA_UNPROGRAMMED 	0
#define FPGA_PROGRAMMED		1
#define SERVER_MESSAGE_HEADER  12

#define SERVER_MAGIC 	0xDEADBEEF

int regCounter;

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

void destroy_server(struct server_state *st)
{
  if(st == NULL){
    return;
  }

  if(st->s_magic != SERVER_MAGIC){
    printf("bad magic on server state\n");
  }

  if(st->master_socket >= 0){
    close(st->master_socket);
    st->master_socket = (-1);
  }

  if(st->client_socket >= 0){
    close(st->client_socket);
    st->client_socket = (-1);
  }

  free(st);
}


struct server_state *create_server()
{
  struct server_state *st;

 st = malloc(sizeof(struct server_state));
  if(st == NULL){
    return NULL;
  }

  st->s_magic = SERVER_MAGIC;
  st->master_socket = (-1);
  st->client_socket = (-1);

  return st;
}

    
//defining s Constructor for every RHINO node that runs the ClusterServer Application
int main(int argc, char *argv[])
{
	//defining variables
	int pid;
	int portno;
	char buffer[256];

	RhinoT rhinoDevice, *rhinoDevicePtr;
	RegisterT *rhinoRegistersPtr, rhinoRegisters;
	server_stateT *st;
	/*
	   Is the structure containing an internet address and is defined in netinet/in.h
	   Will contain the address of the client and serv_addr will contain address of the server
	 */
	printf("Entering main\n");

	rhinoDevicePtr = &rhinoDevice;
	rhinoRegistersPtr = &rhinoRegisters;

	//if no arguments have been passed in on the console
	if (argc < 2)
	{
		error("Error, no port provided\n");
	}

	st = create_server();
	if(st == NULL){
		error("unable to allocate local server state");
	}

	portno = atoi(argv[1]); //getting the port number from the console
	//MAYBE INITIALIZE THE RHINO HERE BEFORE USED
	initializeMethod(rhinoDevicePtr, rhinoRegistersPtr);
	
	if(connect_sockets(portno, rhinoDevicePtr,rhinoRegistersPtr, st) < 0)
	{
		error("Error connecting socket\n");
	}

	

	destroy_server(st);

	return 0; //this terminates the whole main program but we never get here
}

int connect_sockets(int portno, RhinoT *rhinoDevicePtr,  RegisterT *rhinoRegistersPtr, server_stateT *st)
{
	int pid;
	int z = 0;
	int ret = 0;

	struct sockaddr_in serv_addr, cli_addr; 
	socklen_t servlen;
	socklen_t clilen;

	if(st->s_magic != SERVER_MAGIC){
		printf("%s:bad magic on server state\n", __func__);
	}
	/* 
	 ** Create a TCP/IP socket to use
	 ** SOCK_STREAM is for defining what type of socket openned
	 ** the 0 allows the operating system to choose the approriate protocol to use 
	 ** eg TCP when used with streaming in data
	 */
	st->master_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(st->master_socket < 0)
	{
		error("Error openning socket\n");
	}

	/* 
	 ** Create a server socket address
	 ** sets all values in the buffer to zero, takes two arguments a pointer to the buffer 
	 ** and the size of the buffer 
	 ** Type of socket created
	 ** AF_INET allows for an IPv4 internet protocol
	 ** Contains IP address of host,sever code contains the IP address of the machine 
	 ** on which server is running
	 ** The functino htons converts a port number to network byte order
	 */

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_addr.s_addr = INADDR_ANY; 
	serv_addr.sin_port = htons(portno); 

	/*
	 ** Bind the socket to address of current host and portno
	 */
	if(bind(st->master_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{
		error("Error on binding\n");
		exit(EXIT_FAILURE);/*TODO*/
	}
	/*
	 ** Making the socket a listening one
	 ** Allows the process to listen to the socket for a connection, 
	 ** the second argument is the number connections that can be waiting 
	 ** while a prcoess is handling a connection
	 */
	if(listen(st->master_socket, 5) < 0)/*TODO*/  
	{
		error("Error on binding\n");
	}

	clilen = sizeof(cli_addr); 


	for(;;)
	{
		printf("WAITING FOR CLIENT TO CONNECT\n");
		/*
		 ** Starting the server loop
		 ** client_socket is a file descriptor that should be used for the rest 
		 ** of the connection
		 */
		/*
		 ** accept call makes process to block until a client connects to server 
		 ** and only wakes up to when a connection to a client has been made
		 */

		st->client_socket = accept(st->master_socket, (struct sockaddr *)&cli_addr, &clilen); 
		if(st->client_socket < 0)
		{
			error("Error on accept");
			exit(EXIT_FAILURE);/*TODO*/
		}

		z = getpeername(st->client_socket, (struct sockaddr *)&cli_addr, &clilen);
		if(z < 0)
		{
			error("Failed to get IP address of client");
		}

		printf("SERVER CONNECTED WITH CLIENT [%s], USING SOCKET:[%d]\n", (char *)inet_ntoa(cli_addr.sin_addr), st->client_socket);

		//forking a new process to handle the connection
		pid = fork();

		if(pid > 0)
		{
			printf("I AM THE PARENT OF THE PID: [%d]\n", pid);
			close(st->client_socket);
		} 
		else if(pid == 0)
		{
			close(st->master_socket);/* Client doesnt need the listener */
			/*
			 ** maybe have an action button here that is checked to close conn 
			 ** or timer solution or see how tcpborphserver handles this./
			 */
			for(;;)
			{

				ret = rhinoProcessing(rhinoDevicePtr,rhinoRegistersPtr, st);
				
				if(ret != 2){
				  
					printf("%s:Expected return after rhinoProcessing not matched\n", __func__);
					close (st->client_socket);
					break;
				}
			}
			
			close (st->client_socket);
			printf("WAITING FOR CLIENT TO CONNECT\n");
			exit(0);
/*			int ret = kill(getpid(), SIGKILL);
			if(ret<0)
			{
			  printf("ERROR KILLING A PROCESS\n");
			    switch(errno){
			      case EINVAL:
				error("An invalid signal was specified.");
				break;
			      case EPERM:
				break;
			      case ESRCH:
				error("The pid or process group does not exist.");
			      default:
				error("UNKNOW KILL ERROR");
			    }
			}
*/		
		}
		else if(pid == -1) 
		{
			close(st->client_socket);
			error("FORK ERROR\n");
		}
		
	}

	return 0;
}

/*
** THIS METHOD INITIALIZES INFORMATION ABOUT THE RHINO BOARD  
*/
void initializeMethod(RhinoT *rhinoDevicePtr, RegisterT *rhinoRegistersPtr)
{
	int i;
	network_infoT *rhino_info, rhino_infoObj;
	
	rhino_info = &rhino_infoObj;
	
	printf("GETTING INTO THE INIT METHOD\n");

	rhinoDevicePtr->nameNumber		= RHINOID;
	rhino_info = getIpAddress(RHINOID);
	
	printf("THE MAC ADDRESS OF THIS RHINO IS: [%s]\n", rhino_info->macAddress);
	printf("THE IP ADDRESS OF THIS RHINO IS: [%s]\n", rhino_info->ipAddress);
	
	
	strncpy(rhinoDevicePtr->ipAddress, rhino_info->ipAddress , sizeof(rhinoDevicePtr->ipAddress)-1);
	//rhinoDevicePtr->ipAddress[16]		= getIpAddress();
	rhinoDevicePtr->ipAddress[16]	='\0';
	
	rhinoDevicePtr->busyBit			= FPGA_UNPROGRAMMED;
	//rhinoDevicePtr->registersAccessible	= NULL;
      
	
	
	rhinoRegistersPtr->readWriteBit  	= 0;
	rhinoRegistersPtr->rhinoNameNumber  	= 0;
	rhinoRegistersPtr->size  		= 0;
	memset(&rhinoRegistersPtr->regName[0], 0, sizeof(rhinoRegistersPtr->regName));
	//memset(&rhinoDevicePtr->ipAddress[0], 0, sizeof(rhinoDevicePtr->ipAddress));
    //printf("THE RHINO REGISTER NAME IS: [%s]\n",rhinoRegistersPtr->regName);
}


#define NONE 20
	      
int rhinoProcessing(RhinoT *rhinoDevicePtr, RegisterT *rhinoRegistersPtr, server_stateT *st)
{
	printf("GETTING INTO RHINO PROCESSING\n");
	//RegisterT *rhinoRegistersPtr;
	client_msg_T *cm, readMessage;
	server_msg_T *sm, writeMessage;
	reg_msg_T *rm,regMessage;
	int rr, wr;
	int counter=0;
	int successBit;
	uint16_t dataFromReg;
	int state = 0;
	RegisterT dummyRegister;
	cm = &readMessage;
	sm = &writeMessage;
	rm = &regMessage;
	
	int client_send;
	

    for(;;)
    {
	if(st->s_magic != SERVER_MAGIC){
		printf("%s: bad magic on server state\n", __func__);
	}
      
	printf("%s:SERVER ABOUT TO RECEIVE\n", __func__);
	rr = recv(st->client_socket, cm, sizeof(readMessage), 0);
	//printf("CLIENT PACKET RECIEVED\n");
	//client_send = send(st->client_socket, cm, sizeof(readMessage), MSG_NOSIGNAL);
	//printf("CLIENT PACKET SENT\n");
	

	if(rr < 0){
		switch(errno){
			case EAGAIN :
			case EINTR  :
				return 0;
				
			case ECONNRESET:
			  printf("A connection was forcibly closed by a peer\n");
			  return 0;
			  
			case ENOTCONN:
			  printf("A receive is attempted on a connection-mode socket that is not connected\n");
			  return 0;
			  
			case ETIMEDOUT:
			  printf("The connection timed out during connection establishment, or due to a transmission timeout on active connection\n");
			  return 0;
			  
			default :
				error("tcp receive failed");
				return 0;
		}
	}
	
	else if (rr ==0)
	{
	  printf("CLIENT DISCONNECTED GRACEFULLY\n");
	  return 0;
	}
	
	printf("%s:server receive finished\n", __func__);

	cm->rhinoID 	= ntohl(cm->rhinoID);

	strncpy(cm->regName, cm->regName, sizeof(cm->regName)-1);
	cm->regName[32] = '\0';
	
	cm->data 	= ntohs(cm->data);
	cm->messageID 	= ntohl(cm->messageID);
	printf("USERNAME FROM THE NETWORK: [%s]\n",cm->userName);
	strncpy(cm->userName, cm->userName, sizeof(cm->userName)-1);
	cm->userName[16] = '\0';

	printf("Client_MsgID IS: %d\n",cm->messageID);
	printf("Client_RHINOID IS: %d\n",cm->rhinoID);
	printf("Client_REG_NAME IS: %s\n",cm->regName);
	printf("Client_DATA IS: %d\n",cm->data);
	 printf("Client_USER_NAME: %s\n", cm->userName);
	printf("\n");

	state = cm->messageID;

	switch (state){
		case LOAD_BOF_FILE: //THIRD CASE IS startBofFile
		
			printf("%s:server case tested SUCCESS OVERAALL\n", __func__);
			printf("\n");
			successBit = -1;
			rhinoDevicePtr->successBit = -1;
			
			
			rhinoDevicePtr = (RhinoT *)startBofFile(cm->rhinoID, cm->userName, cm->regName, rhinoDevicePtr, st); 
			
			if(rhinoDevicePtr == NULL)
			{
				printf("%s:WARNING NULL return\n", __func__);
				return 1;
				break;
			}
			
			//printf("I RETURNED AND ABOUT TO SLEEP\n");
			//sleep(10);
			
			//NEED TO SEND UPDATED FPGA PROGRAMMED BIT TO RHINO
			
			strncpy(sm ->rhinoBoard.errorMsg, rhinoDevicePtr->errorMsg, sizeof(sm->rhinoBoard.errorMsg)-1);
			sm->rhinoBoard.errorMsg[256] = '\0';
			
			printf("RHINO ERROR MSG [%s]\n", sm->rhinoBoard.errorMsg);
			
			sm->rhinoBoard.busyBit = htonl(rhinoDevicePtr->busyBit);
			printf("RHINO BUSYBIT FROM RHINODEVICEPTR: [%d]\n",rhinoDevicePtr->busyBit);
			
			//printf("RHINO BUSYBIT: [%d]\n",sm->rhinoBoard.busyBit);
			
			sm->rhinoBoard.nameNumber = htonl(rhinoDevicePtr->nameNumber);
			
			printf("RHINO NAME NUMBER:[%d]\n",sm->rhinoBoard.nameNumber);
			
			
			//sm->rhinoBoard.ipAddress = rhinoDevicePtr->ipAddress;
			strncpy(sm->rhinoBoard.ipAddress,rhinoDevicePtr->ipAddress,sizeof(sm->rhinoBoard.ipAddress)-1);
			sm->rhinoBoard.ipAddress[16] = '\0';
			printf("THE IP ADDRESS OF THE RHINO IS: [%s]\n", sm->rhinoBoard.ipAddress);
			
			
			sm->rhinoBoard.successBit = htonl(rhinoDevicePtr->successBit);
			printf("RHINO SUCCESS BIT: [%d]\n",sm->rhinoBoard.successBit);
			
			
			sm->data = htons(0);
			
			
			wr = send(st->client_socket, sm, sizeof(writeMessage), MSG_NOSIGNAL); 
			if(wr < sizeof(writeMessage))
			{
					switch(errno){
						case EAGAIN :
						case EINTR  :
							return 0;
						default :
							printf("unable to send request: %s", strerror(errno));
							break;
					}
				
			}
			    
			state = NONE;
			printf("MESSAGE JUST SENT\n");
			printf("\n");
			
			

		break;
	      
		case REGISTER_WRITE:
	     
		  printf("%s:server case tested SUCCESS OVERAALL\n", __func__); //WHAT DOES THIS DO???????????????ASK????/
		  printf("\n");
		  
			successBit = -1;
			rhinoDevicePtr->successBit = -1;
			
			rhinoDevicePtr = (RhinoT *)regWrite(cm->rhinoID, cm->regName, cm->data, rhinoDevicePtr, st);
			
			if(rhinoDevicePtr == NULL)
			{
				printf("%s:WARNING NULL return\n", __func__);
				return 1;
				break;
			}
			//printf("I RETURNED AND ABOUT TO SLEEP\n");
			//sleep(10);
			
			//NEED TO SEND UPDATED FPGA PROGRAMMED BIT TO RHINO
			sm->rhinoBoard.busyBit = htonl(rhinoDevicePtr->busyBit);
			printf("RHINO BUSYBIT: [%d]\n",sm->rhinoBoard.busyBit);
			
			sm->rhinoBoard.nameNumber = htonl(rhinoDevicePtr->nameNumber);
			printf("RHINO NAME NUMBER:[%d]\n",sm->rhinoBoard.nameNumber);
			
			//sm->rhinoBoard.ipAddress = rhinoDevicePtr->ipAddress;
			strncpy(sm->rhinoBoard.ipAddress,rhinoDevicePtr->ipAddress,sizeof(sm->rhinoBoard.ipAddress)-1);
			sm->rhinoBoard.ipAddress[16] = '\0';
			printf("THE IP ADDRESS OF THE RHINO IS: [%s]\n", sm->rhinoBoard.ipAddress);
			
			sm->rhinoBoard.registersAccessible.readWriteBit = rhinoRegistersPtr->readWriteBit;
			printf("THE RHINO REGISTER READWRITE BIT IS: [%d]\n", sm->rhinoBoard.registersAccessible.readWriteBit);
			
			strncpy(sm->rhinoBoard.registersAccessible.regName,rhinoRegistersPtr->regName,sizeof(sm->rhinoBoard.registersAccessible.regName)-1);
			sm->rhinoBoard.registersAccessible.regName[32] = '\0';
			printf("THE RHINO REGISTER NAME IS: [%s]\n", sm->rhinoBoard.registersAccessible.regName);
			
			
			sm->rhinoBoard.registersAccessible.rhinoNameNumber = rhinoRegistersPtr->rhinoNameNumber;
			
			sm->rhinoBoard.successBit = htonl(rhinoDevicePtr->successBit);
			printf("RHINO SUCCESS BIT: [%d]\n",sm->rhinoBoard.successBit);
			sm->data = htons(0);
			
			
			wr = send(st->client_socket, sm, sizeof(writeMessage), MSG_NOSIGNAL); 
			
			if(wr < sizeof(writeMessage))
			{
					switch(errno){
						case EAGAIN :
						case EINTR  :
							return 0;
						default :
							printf("unable to send request: %s", strerror(errno));
							break;
					}
				
			}
			    
			state = NONE;
			printf("MESSAGE JUST SENT\n");
			printf("\n");
		  
		break;
	      
		case REGISTER_READ:
	     
			printf("%s:server case tested SUCCESS OVERAALL\n", __func__); //WHAT DOES THIS DO???????????????ASK????/  
			printf("\n");
		      
			dataFromReg = (uint16_t)regRead(cm->rhinoID,cm->regName , rhinoDevicePtr, st);
			
			if(dataFromReg < 0)
			{
			  printf("%s:WARNING NULL return\n", __func__);
			  return 1;
			  break;
			}
			
			
			
			//printf("I RETURNED AND ABOUT TO SLEEP\n");
			//sleep(10);
			
			//NEED TO SEND UPDATED FPGA PROGRAMMED BIT TO RHINO
			sm->rhinoBoard.busyBit = htonl(rhinoDevicePtr->busyBit);
			printf("RHINO BUSYBIT: [%d]\n",sm->rhinoBoard.busyBit);
			
			sm->rhinoBoard.nameNumber = htonl(rhinoDevicePtr->nameNumber);
			printf("RHINO NAME NUMBER:[%d]\n",sm->rhinoBoard.nameNumber);
			
			//sm->rhinoBoard.ipAddress = rhinoDevicePtr->ipAddress;
			strncpy(sm->rhinoBoard.ipAddress,rhinoDevicePtr->ipAddress,sizeof(sm->rhinoBoard.ipAddress)-1);
			sm->rhinoBoard.ipAddress[16] = '\0';
			printf("THE IP ADDRESS OF THE RHINO IS: [%s]\n", sm->rhinoBoard.ipAddress);
			
			sm->rhinoBoard.registersAccessible.readWriteBit = rhinoRegistersPtr->readWriteBit;
			printf("THE RHINO REGISTER READWRITE BIT IS: [%d]\n", sm->rhinoBoard.registersAccessible.readWriteBit);
			
			strncpy(sm->rhinoBoard.registersAccessible.regName,rhinoRegistersPtr->regName,sizeof(sm->rhinoBoard.registersAccessible.regName)-1);
			sm->rhinoBoard.registersAccessible.regName[32] = '\0';
			printf("THE RHINO REGISTER NAME IS: [%s]\n", sm->rhinoBoard.registersAccessible.regName);
			
			
			sm->rhinoBoard.registersAccessible.rhinoNameNumber = rhinoRegistersPtr->rhinoNameNumber;
			
			sm->rhinoBoard.successBit = htonl(1);
			printf("RHINO SUCCESS BIT: [%d]\n",sm->rhinoBoard.successBit);
			sm->data = htons(dataFromReg);
			
			wr = send(st->client_socket, sm, sizeof(writeMessage), MSG_NOSIGNAL); 
			if(wr < sizeof(writeMessage))
			{
					switch(errno){
						case EAGAIN :
						case EINTR  :
							return 0;
						default :
							printf("unable to send request: %s", strerror(errno));
							break;
					}
				
			}
			    
			state = NONE;
			printf("MESSAGE JUST SENT\n");
			printf("\n");
		  		  
		  
		break;
	      
		case LIST_REGISTERS:
		  
		  counter = 0;
		  printf("%s:server case tested SUCCESS OVERAALL\n", __func__); //WHAT DOES THIS DO???????????????ASK????/ 
		  printf("\n");
		  
		  rhinoRegistersPtr = listRegisters(cm->rhinoID, cm->userName, cm->regName, st);
			
			if(rhinoRegistersPtr == NULL)
			{
				printf("%s:WARNING NULL return\n", __func__);
				return 1;
				break;
			}
			//printf("I RETURNED AND ABOUT TO SLEEP\n");
			//sleep(10);
			
			
			
			//NEED TO SEND UPDATED FPGA PROGRAMMED BIT TO RHINO
			sm->rhinoBoard.busyBit = htonl(rhinoDevicePtr->busyBit);
			printf("RHINO BUSYBIT: [%d]\n",sm->rhinoBoard.busyBit);
			
			sm->rhinoBoard.nameNumber = htonl(rhinoDevicePtr->nameNumber);
			printf("RHINO BUSYBIT: [%d]\n",sm->rhinoBoard.busyBit);
			
			//sm->rhinoBoard.ipAddress = rhinoDevicePtr->ipAddress;
			strncpy(sm->rhinoBoard.ipAddress,rhinoDevicePtr->ipAddress,sizeof(sm->rhinoBoard.ipAddress)-1);
			sm->rhinoBoard.ipAddress[16] = '\0';
			printf("THE IP ADDRESS OF THE RHINO IS: [%s]\n", sm->rhinoBoard.ipAddress);
			
			strncpy(sm->rhinoBoard.registersAccessible.regName,rhinoRegistersPtr[counter].regName,sizeof(sm->rhinoBoard.registersAccessible.regName)-1);
			sm->rhinoBoard.registersAccessible.regName[32] = '\0';
			printf("THE RHINO REGISTER NAME IS: [%s]\n", sm->rhinoBoard.registersAccessible.regName);
			
			sm->rhinoBoard.registersAccessible.readWriteBit =htonl(rhinoRegistersPtr[counter].readWriteBit);
			printf("THE RHINO REGISTER READWRITEBIT IS: [%d]\n", sm->rhinoBoard.registersAccessible.readWriteBit);
			
			
			sm->rhinoBoard.registersAccessible.rhinoNameNumber = htonl(rhinoRegistersPtr[counter].rhinoNameNumber);
			printf("THE RHINO REGISTER NAMENUMBER IS: [%d]\n", sm->rhinoBoard.registersAccessible.rhinoNameNumber);
			
			sm->rhinoBoard.registersAccessible.size = htonl(rhinoRegistersPtr[counter].size);
			printf("THE RHINO REGISTER SIZE IS: [%x]\n", sm->rhinoBoard.registersAccessible.size);
			
			sm->rhinoBoard.successBit = htonl(1);
			printf("RHINO SUCCESS BIT: [%d]\n",sm->rhinoBoard.successBit);
			sm->data = htons(dataFromReg);
			
			//FIRST CHECKING THAT THE REGISTER IS NOT NULL
			if(rhinoRegistersPtr == NULL)
			{
				wr = send(st->client_socket, sm, sizeof(writeMessage), MSG_NOSIGNAL); 
				if(wr < sizeof(writeMessage))
				{
						switch(errno){
							case EAGAIN :
							case EINTR  :
								return 0;
							default :
							      printf("unable to send request: %s", strerror(errno));
								break;
						}
				}
				state = NONE;
				printf("MESSAGE JUST SENT WITH FIRST REGISTER\n");
			    
			}
			else if(rhinoRegistersPtr!= NULL) // THERE ARE REGISTERS ON THE PROCESS AND LOOP TILL THEY ARE NULL
			{
			      wr = send(st->client_socket, sm, sizeof(writeMessage), MSG_NOSIGNAL); 
			      if(wr < sizeof(writeMessage))
			      {
					      switch(errno){
						      case EAGAIN :
						      case EINTR  :
							      return 0;
						      default :
							      printf("unable to send request: %s", strerror(errno));
							      break;
					      }
			      }
			      
			      printf("MESSAGE JUST SENT WITH FIRST REGISTER\n");
			      
			      for(counter +=1 ; counter< regCounter+1; counter++) //SENDS EVERY PACKET INCLUDING THE NULL ONE AS WELL
			      {
				
				rm->registersAccessible.readWriteBit = htonl(rhinoRegistersPtr[counter].readWriteBit);
				printf("THE RHINO REGISTER READWRITE BIT IS: [%d]\n", rm->registersAccessible.readWriteBit);
				
				strncpy(rm->registersAccessible.regName,rhinoRegistersPtr[counter].regName,sizeof(rm->registersAccessible.regName)-1);
				rm->registersAccessible.regName[32] = '\0';
				printf("THE RHINO REGISTER NAME IS: [%s]\n", rm->registersAccessible.regName);
				
				
				rm->registersAccessible.rhinoNameNumber = htonl(rhinoRegistersPtr[counter].rhinoNameNumber);
				printf("THE RHINO REGISTER NAMENUMBER IS: [%d]\n", rm->registersAccessible.rhinoNameNumber);
				
				rm->registersAccessible.size = htonl(rhinoRegistersPtr[counter].size);
				printf("THE RHINO REGISTER SIZE IS: [%x]\n", rm->registersAccessible.size);
				
				 wr = send(st->client_socket, rm, sizeof(regMessage), MSG_NOSIGNAL); 
				 if(wr < sizeof(regMessage))
				 {
						switch(errno){
							case EAGAIN :
							case EINTR  :
								return 0;
							default :
								printf("unable to send request: %s", strerror(errno));
								break;
						}
				 }
				 state = NONE;
				 printf("MESSAGE JUST SENT WITH ANOTHER REGISTER\n");
				
			      }
					  
			  }
			    
			printf("SENT ALL THE REGISTERS\n");
			printf("\n");
			
			
		break;
	    
		case KILL_BOF_PROCESS:
		  
			printf("%s:server case tested SUCCESS OVERAALL\n", __func__);
			printf("\n");

			successBit = -1;
			rhinoDevicePtr->successBit = -1;
			
			rhinoDevicePtr = (RhinoT *)stopBofFile(cm->rhinoID, cm->regName, rhinoDevicePtr, st); 
			if(rhinoDevicePtr == NULL)
			{
				printf("%s:WARNING NULL return\n", __func__);
				return 1;
				break;
			}
			//printf("I RETURNED AND ABOUT TO SLEEP\n");
			//sleep(10);
			
			//NEED TO SEND UPDATED FPGA PROGRAMMED BIT TO RHINO
			sm->rhinoBoard.busyBit = htonl(rhinoDevicePtr->busyBit);
			printf("RHINO BUSYBIT: [%d]\n",sm->rhinoBoard.busyBit);
			
			sm->rhinoBoard.nameNumber = htonl(rhinoDevicePtr->nameNumber);
			
			printf("RHINO NAME NUMBER:[%d]\n",sm->rhinoBoard.nameNumber);
			
			//sm->rhinoBoard.ipAddress = rhinoDevicePtr->ipAddress;
			strncpy(sm->rhinoBoard.ipAddress,rhinoDevicePtr->ipAddress,sizeof(sm->rhinoBoard.ipAddress)-1);
			sm->rhinoBoard.ipAddress[16] = '\0';
			printf("THE IP ADDRESS OF THE RHINO IS: [%s]\n", sm->rhinoBoard.ipAddress);
			
			sm->rhinoBoard.registersAccessible.readWriteBit = rhinoRegistersPtr->readWriteBit;
			printf("THE RHINO REGISTER READWRITE BIT IS: [%d]\n", sm->rhinoBoard.registersAccessible.readWriteBit);
			
			strncpy(sm->rhinoBoard.registersAccessible.regName,rhinoRegistersPtr->regName,sizeof(sm->rhinoBoard.registersAccessible.regName)-1);
			sm->rhinoBoard.registersAccessible.regName[16] = '\0';
			printf("THE RHINO REGISTER NAME IS: [%s]\n", sm->rhinoBoard.registersAccessible.regName);
			
			
			sm->rhinoBoard.registersAccessible.rhinoNameNumber = rhinoRegistersPtr->rhinoNameNumber;
			
			sm->rhinoBoard.successBit = htonl(rhinoDevicePtr->successBit);
			printf("RHINO SUCCESS BIT: [%d]\n",sm->rhinoBoard.successBit);
			sm->data = htons(0);
			
			wr = send(st->client_socket, sm, sizeof(writeMessage), MSG_NOSIGNAL); 
			if(wr < sizeof(writeMessage))
			{
					switch(errno){
						case EAGAIN :
						case EINTR  :
							return 0;
						default :
							printf("unable to send request: %s", strerror(errno));
							break;
					}
				
			}
			    
			state = NONE;
			printf("MESSAGE JUST SENT\n");
			printf("\n");

		  					  
		break;
	          
		default:
	      
			printf("NO CASE IN RHINOPROCESSING \n");
			break;
	    
	}
    
      
    }
	return 2;
}




RhinoT *startBofFile(uint32_t nameNumber, uint8_t *serverUserName, uint8_t *bofProcessName, RhinoT *rhinoDevicePtr, server_stateT *st)
{	
	int len;
	char command[255];
	int res;
	FILE *fp;
	char *errorMsg;

	if(st->s_magic != SERVER_MAGIC){
		printf("%s: bad magic on server state\n", __func__);
	}

	
	
	 // *****NEED TO ADD SOME ERROR HANDLING JUST IN CASE THERE IS A BOF PROCESS ALREADY RUNNING ON THE SYSTEM*****
	 
	 printf("IN START BOF METHOD\n");
	 
	
	len = snprintf(command,sizeof(command),"./../%s/%s/%s.bof&",serverUserName, bofProcessName, bofProcessName);

	//printf("Command from snprintf: %s\n",command);
	//printf("Returning Value from snprintf: %d\n",len);

	if(nameNumber!= RHINOID)
	{
		printf("SORRY MESSAGE IS AT WRONG RHINO NODE\n");
		
		errorMsg = "SORRY MESSAGE IS AT WRONG RHINO NODE";
		
		strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
		rhinoDevicePtr->errorMsg[256] = '\0';
		
		rhinoDevicePtr->busyBit = rhinoDevicePtr->busyBit;
		rhinoDevicePtr->successBit = FAILURE;
		
		return rhinoDevicePtr;
	}
	else if(nameNumber == RHINOID)//starting actual .bof file on the RHINO as a background process
	{
	      printf("MESSAGE AT CORRECT RHINO STARTING .BOF PROCESS\n");

#if 0
		res = system(command);
		if(res < 0)
		{
			error("SYSTEM CALL FAILED");
		}
#endif
	      printf("THE RHINO BUSYBIT BEFORE CHECK IS: [%d]\n",rhinoDevicePtr->busyBit);
	      //ERROR HANDLE1: CHECKING IF THERE IS ALREADY ANOTHER BOF PROCESS RUNNING
	      if(rhinoDevicePtr->busyBit == FPGA_UNPROGRAMMED)
	      {
		
		  res = system(command);
		  if(res < 0)
		  {
			  error("SYSTEM CALL FAILED");
		  }
		  
		  memset(&rhinoDevicePtr->errorMsg[0], 0, sizeof(rhinoDevicePtr->errorMsg));
		  
		  rhinoDevicePtr->busyBit = FPGA_PROGRAMMED;
		  rhinoDevicePtr->successBit = SUCCESS;
		  
		  printf("THE SUCCESS BIT IN THE START BOF FUNCTION: [%d]\n", rhinoDevicePtr->successBit);
		  
		  return rhinoDevicePtr;
	      }
	      
	      else if (rhinoDevicePtr->busyBit == FPGA_PROGRAMMED)
	      {
		errorMsg = "THE RHINO IS ALREADY PROGRAMMED, TRY ANOTHER BOARD";
		
		strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
		rhinoDevicePtr->errorMsg[256] = '\0';
		
		rhinoDevicePtr->busyBit = rhinoDevicePtr->busyBit;
		
		rhinoDevicePtr->successBit = FAILURE;
		
		return rhinoDevicePtr;
		
	      }
	}
	else
	{
		printf("SORRY BOF PROCESS DID NOT START\n");
		
		errorMsg = "SORRY BOF PROCESS DID NOT START";
		
		strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
		rhinoDevicePtr->errorMsg[256] = '\0';
		
		rhinoDevicePtr->busyBit = rhinoDevicePtr->busyBit;
		
		rhinoDevicePtr->successBit = FAILURE;
		
		return rhinoDevicePtr;

	}

	return rhinoDevicePtr;
}

  //defining a register write method for arbitrary data lengths to a named register
/***************************************************************************************************************************
NB: THIS METHOD IS ONLY USEFUL ONCE THE FPGA HAS BEEN PROGRAMMED and THIS PART CAN SIT ON THE SERVER SIDE OF THE CLUSTER
*****************************************************************************************************************************/
RhinoT *regWrite(uint32_t nameNumber, uint8_t *registerName, uint16_t data, RhinoT *rhinoDevicePtr, server_stateT *st)
{
	FILE *fp;
	char command[255];
	char processorData[16];
	char processorId[8];
	int len;
	int ret;
	int res;
	char *errorMsg;
	
	
	if(st->s_magic != SERVER_MAGIC){
		printf("%s: bad magic on server state\n", __func__);
	}
	
	
	if(nameNumber!= RHINOID)
	{
		printf("SORRY MESSAGE IS AT WRONG RHINO NODE\n");
		
		errorMsg = "SORRY MESSAGE IS AT WRONG RHINO NODE";
		
		strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
		rhinoDevicePtr->errorMsg[256] = '\0';
		
		rhinoDevicePtr->successBit = FAILURE;
		
		return rhinoDevicePtr;
	}
	else if(nameNumber == RHINOID)
	{
	  
	  //CHECK IF THE BOARD IS ACTUALLY PROGRAMMED
	  if(rhinoDevicePtr->busyBit == FPGA_UNPROGRAMMED)
	  {
		printf("SORRY THE RHINO IS NOT PROGRAMMED\n");
	    
	    
		errorMsg = "SORRY THE RHINO IS NOT PROGRAMMED";
		
		strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
		rhinoDevicePtr->errorMsg[256] = '\0';
		
		rhinoDevicePtr->successBit = FAILURE;
		
		return rhinoDevicePtr;
		
	  }
	  
	  else if(rhinoDevicePtr->busyBit == FPGA_PROGRAMMED)
	    {
		fp = popen("(ps ax | awk '/[b]of/ {print $1}')","r");
		
		if(!fp)
		{
		  printf("Error on openning the pipe to get process name \n");
		  
		  rhinoDevicePtr->successBit = FAILURE;
		  
		  return rhinoDevicePtr;
		}
		
		
		ret = fscanf(fp,"%s",processorId);
		//printf("ProcessId :%s \n",processorId);

	      
		len = snprintf(command,sizeof(command),"echo -e -n \"\\x%d\\x00\" > /proc/%s/hw/ioreg/%s",data,processorId,registerName);
		//printf("Command from snprintf: %s\n",command);
		
		if(len <= sizeof(command))
		{
		}

		res = system(command);
		    
		if(res < 0)
		{
		      
		  rhinoDevicePtr->successBit = FAILURE;
		  printf("SYSTEM CALL FAILED");
		  return rhinoDevicePtr;
		      
		}
		    
		//NEED ERROR HANDLING FOR IF THE BOARD IS ALREADY PROGRAMMED
		    
		printf("WRITE SUCCESSFUL\n");
		memset(&rhinoDevicePtr->errorMsg[0], 0, sizeof(rhinoDevicePtr->errorMsg));
		fclose(fp);
		    
		rhinoDevicePtr->successBit = SUCCESS;
		  
	      
	   }
	}
	else
	{
	    printf("COULD NOT WRITE TO REGISTER\n");
	    
	    errorMsg = "COULD NOT WRITE TO REGISTER";
		  
	    strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
	    rhinoDevicePtr->errorMsg[256] = '\0';
	    
	    rhinoDevicePtr->successBit = FAILURE;
	    fclose(fp);
	    return rhinoDevicePtr; 
	}
	
      return rhinoDevicePtr;
}

/*******************************************************************************************************************************************************************************
THIS METHOD PERFORMAS THE ACTUAL READ ON THE FPGA USING BORPH FILE SYSTEM
NB: THIS METHOD IS ONLY USEFUL ONCE THE FPGA HAS BEEN PROGRAMMED and THIS PART CAN SIT ON THE SERVER SIDE OF THE CLUSTER
*****************************************************************************************************************************************************************************/	
uint16_t regRead(uint32_t nameNumber,uint8_t *registerName, RhinoT *rhinoDevicePtr, server_stateT *st)
{
	FILE *fp;
	int len;
	int ret;
	int regStartAdd;
	int regEndAdd;
	uint16_t dataFromReg;
	char processorId[8];
	char command[64];
	char processorData[256];
	char charDataFromReg[64];
	char *errorMsg;
	
	
	if(st->s_magic != SERVER_MAGIC){
		printf("%s: bad magic on server state\n", __func__);
	}
	
	
	if(nameNumber!= RHINOID)
	{
		printf("SORRY MESSAGE IS AT WRONG RHINO NODE\n");
		
		errorMsg = "SORRY MESSAGE IS AT WRONG RHINO NODE";
		
		strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
		rhinoDevicePtr->errorMsg[256] = '\0';  //(THINK ABOUT HOW YOU ARE GOING TO GET THIS INFORMATION BACK TO THE SWITCH STATEMENT)
		
		return -1;
	}
	else if(nameNumber == RHINOID)
	{
	  
	  //CHECKS THAT THE BOARD IS ACTUALLY PROGRAMMED BEFORE ATTEMPTING TO READ FROM THE REGISTER
	 if(rhinoDevicePtr->busyBit == FPGA_UNPROGRAMMED)
	 {
	    printf("THE RHINO BOARD IS NOT PROGRAMMED\n");
	   
	    errorMsg = "THE RHINO BOARD IS NOT PROGRAMMED";
		    
	    strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
	    rhinoDevicePtr->errorMsg[256] = '\0';  //(THINK ABOUT HOW YOU ARE GOING TO GET THIS INFORMATION BACK TO THE SWITCH STATEMENT)
	    
	    rhinoDevicePtr->successBit = FAILURE;
	    
	    return -1;
	    
	 }
	 
	 else if(rhinoDevicePtr->busyBit == FPGA_PROGRAMMED)
	 {  
	      printf("THE RHINO BOARD IS PROGRAMMED\n");
	      
	      //GETS THE BOF PROCESS ID
	      fp = popen("(ps ax | awk '/[b]of/ {print $1}')","r");
	      
	      if(!fp)
	      {
		error("Error on openning the pipe to rhino_pwr \n");
		return -1;
	      }
	      
	      ret = fscanf(fp,"%s",processorData);
	      //printf("ProcessId :%s \n",processorData);

	      len = snprintf(command,sizeof(command),"od -d /proc/%s/hw/ioreg/%s",processorData,registerName);
	      //printf("COMMAND FROM snprintf: %s\n",command);
	      
	      if(len <= sizeof(command))
	      {
		fp = popen(command,"r");
		
		if(!fp)
		{
		    error("Error on openning the pipe to rhino_pwr \n");
		    return -1;
		}
		
		
		/**************************************************************************************************************************************************************************
		  Have to run the writing command function twice due to the issue that data is only available to read from the bus after the 2nd clock cycle (include this in the write up
		**************************************************************************************************************************************************************************/
		fp = popen(command,"r");

		//error handling
		if(!fp)
		{
		  error("Error on openning the pipe to rhino_pwr \n");
		  return -1;
		}
		
	      }

	      //READIN 
	      while(fscanf(fp,"%d %d %d",&regStartAdd,&dataFromReg,&regEndAdd)==3)
	      {
		  //printf("Data from the register: %d\n",dataFromReg);
		  len = snprintf(charDataFromReg,sizeof(charDataFromReg),"%d\n",dataFromReg);
		  printf("The data from the register %s is: %s ",registerName,charDataFromReg);
	      }
	       fclose(fp);
		return dataFromReg;
	  }  
	}
	else
	{
	  printf("COULD NOT READ THE REGISTER\n");
	  
	  errorMsg = "COULD NOT READ THE REGISTER";
		
	  strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
	  rhinoDevicePtr->errorMsg[256] = '\0';  //(THINK ABOUT HOW YOU ARE GOING TO GET THIS INFORMATION BACK TO THE SWITCH STATEMENT)
	  fclose(fp);
	  
	  return -1; 
	}
  
  
}

RegisterT *listRegisters(uint32_t nameNumber, uint8_t *serverUserName, uint8_t *bofProcessName, server_stateT *st)
{
     
      int len;
      int buf_size = 0;
      int buf_counter = 0;
      regCounter = 0;
      unsigned int memoryLocation;
      char symbolFileRegister[64];
      char symbolFileCommand[64];
      char *errorMsg;
      RegisterT *processRegister = NULL;
      RegisterT *tmpReg = NULL;
      FILE *fp;
     
  
  
  
  
	if(st->s_magic != SERVER_MAGIC){
		      printf("%s: bad magic on server state\n", __func__);
	}
	
	
	if(nameNumber!= RHINOID)
	{
		printf("SORRY MESSAGE IS AT WRONG RHINO NODE\n");
		return NULL;
	}
	
	else if(nameNumber == RHINOID)
	{
	  
	   len = snprintf(symbolFileCommand,sizeof(symbolFileCommand),"more /home/%s/%s/%s.sym", serverUserName, bofProcessName, bofProcessName);
	   //printf("Command from snprintf: %s\n",symbolFileCommand);
	   
	   if(len <= sizeof(symbolFileCommand))
	   {
	      fp = popen(symbolFileCommand,"r");
	      
	      if(!fp)
	      {
		printf("COULD NOT OPEN OR FIND THE SYMBOL FILE \n");
		return NULL;
				  
	      }
	   }  
	      /**********************************************************************************************************************
		* SYMOBOL FILE STRUCTURE
		* REGISTERNAME            READWRITE BIT			MEMORYBANK			SIZE
		******************************************************************************************************************/
	       while(fgets(symbolFileRegister,256,fp)!=NULL)
	       {
		 //DYNAMICALLY ALLOCATING SPACE TO THE REGISTER ARRAY 5 BLOCKS AT A TIME
		 if(buf_counter == buf_size)
		 {
		    buf_size +=5; 
		    tmpReg = (RegisterT *) realloc(processRegister,buf_size*(sizeof(RegisterT)));
		    
		    if(!tmpReg)
		    {
		      printf("COULD NOT DYNAMICALLY CHANGE THE SIZE OF REGISTER ARRAY\n");
		      return NULL;
		    }
		   processRegister = tmpReg;
		   
		 }
		 
		 strncpy(processRegister[buf_counter].regName, strtok(symbolFileRegister,"\t"), sizeof(processRegister[buf_counter].regName)-1);
		 processRegister[buf_counter].regName[32] = '\0';
		 //printf("REGNAME: [%s]\n",processRegister[buf_counter].regName);
		 
		 
		 char *readWriteBitString = strtok(NULL,"\t");
		 //printf("READWRITE BIT AS STRING: [%s]\n",readWriteBitString);
		 processRegister[buf_counter].readWriteBit = atoi(readWriteBitString);
		 //printf("READWRITE BIT: [%d]\n",processRegister[buf_counter].readWriteBit);
		 
		 processRegister[buf_counter].rhinoNameNumber = RHINOID;
		 
		 char *memoryLocationString = strtok(NULL,"\t ");
		  //printf("MEMORYLOCATION AS STRING: [%s]\n",memoryLocationString);
		 memoryLocation = strtol(memoryLocationString,&memoryLocationString,0);
		// printf("MEMORYLOCATION: [%x]\n",memoryLocation);
		 
		 char *sizeString = strtok(NULL,"\t");
		 //printf("SIZE: [%s]\n",sizeString);
		 processRegister[buf_counter].size = strtol(sizeString, &sizeString,0);
		// printf("SIZE: [%x]\n",processRegister[buf_counter].size);
		 
		 buf_counter++;
	       }
	       
	       if(buf_counter == buf_size)
	       {
		    buf_size +=1; 
		    tmpReg = realloc(processRegister,buf_size);
		    
		    if(!tmpReg)
		    {
		      printf("COULD NOT DYNAMICALLY CHANGE THE SIZE OF REGISTER ARRAY\n");
		      return NULL;
		    }
		    processRegister = tmpReg;
		    printf("Verification that both pointers point to same location:processRegister:%p, tmpReg:%p\n", processRegister, tmpReg);
		}  
		  	
		  //memset(&processRegister[buf_counter+1].regName[0],0, sizeof(processRegister[buf_counter+1].regName));
		  processRegister[buf_counter].regName[0] = '\0';
		  processRegister[buf_counter].readWriteBit = 0;
		  processRegister[buf_counter].rhinoNameNumber = -1;
		  processRegister[buf_counter].size = 0;
		  
		  regCounter = buf_counter;
	       
	   return processRegister;	
	}
	else
	{
	  printf("COULD NOT LIST THE REGISTERS\n");
	  return NULL;
	}
}

 /****************************************************************************************************************************************************************
STOPS A RUNNING BOF PROCESS ON THAT RHINO
********************************************************************************************************************************************************************/
	  
RhinoT *stopBofFile(uint32_t nameNumber,uint8_t *bofProcessName, RhinoT *rhinoDevicePtr, server_stateT *st)
{
  
      int len;
      int status;
      int counter;
      char *dummyString;
      char killCommand[255];
      char runningBofProcessString[255];
      char *runningBofProcess;
      char processorId[8];
      char *errorMsg;
      FILE *fp;
      
      
      
      
      
      if(st->s_magic != SERVER_MAGIC){
	printf("%s: bad magic on server state\n", __func__);
      }
      
      
      
      if(nameNumber!= RHINOID)
      {
	    printf("SORRY MESSAGE IS AT WRONG RHINO NODE\n");
	    
	    errorMsg = "SORRY MESSAGE IS AT WRONG RHINO NODE";
		    
	    strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
	    rhinoDevicePtr->errorMsg[256] = '\0';  //(THINK ABOUT HOW YOU ARE GOING TO GET THIS INFORMATION BACK TO THE SWITCH STATEMENT)
	    
	    rhinoDevicePtr->successBit = FAILURE;
	    
	    return rhinoDevicePtr;
      }
      
      else if(nameNumber == RHINOID)
      {
	    printf("MESSAGE AT CORRECT RHINO PREPARING TO DELETE .BOF PROCESS\n");
	    
	    //CHECKING THAT THE RHINO IS ACTUALLY PROGRAMMED BEFORE TRYING TO STOP THE PROCESS
	 if(rhinoDevicePtr->busyBit == FPGA_UNPROGRAMMED)
	 {
	    printf("THE RHINO BOARD IS NOT PROGRAMMED\n");
	   
	    errorMsg = "THE RHINO BOARD IS NOT PROGRAMMED";
		    
	    strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
	    rhinoDevicePtr->errorMsg[256] = '\0';  //(THINK ABOUT HOW YOU ARE GOING TO GET THIS INFORMATION BACK TO THE SWITCH STATEMENT)
	    
	    rhinoDevicePtr->successBit = FAILURE;
	    
	    return rhinoDevicePtr;
	    
	 }
	 
	 else if(rhinoDevicePtr->busyBit == FPGA_PROGRAMMED)
	 {  
	    printf("THE RHINO BOARD IS PROGRAMMED\n");
	   
	    fp = popen("(ps ax | awk '/[b]of/ {print $3}')","r");
	    int ret = fscanf(fp,"%s",runningBofProcessString);
	    
	    
	    dummyString = strtok(runningBofProcessString,"/");
	    
	    for(counter =0; counter<3; counter++)
	    {
	      dummyString = strtok(NULL,"/");
	      
	    }
	    
	    runningBofProcess = strtok(NULL,"/");
	    //printf("RUNNING BOF PROCESS NAME :%s \n",runningBofProcess);
	    
	    if(!fp)
	    {
		printf("Error on opening the pipe to write the command to write to the register \n");
		rhinoDevicePtr->successBit = FAILURE;
		
		return rhinoDevicePtr;
	    }
	    
	    
	    
	    
	     if(strcmp(runningBofProcess,bofProcessName))
	     {
		  printf("THE GIVEN BOF PROCESS IS THE ONE RUNNING\n");
		  fp = popen("(ps ax | awk '/[b]of/ {print $1}')","r");
		  ret = fscanf(fp,"%s",processorId);
		  //printf("ProcessId :%s \n",processorId);
		  
		  if(!fp)
		  {
		    printf("Error on opening the pipe to the pid command \n");
		    rhinoDevicePtr->successBit = FAILURE;
		    
		    return rhinoDevicePtr;
		  }
		  
		  
		  len = snprintf(killCommand,sizeof(killCommand),"kill -9 %s",processorId);
		  //printf("Command from snprintf: %s\n",killCommand);
		  fp = popen(killCommand,"r");
		  
		  if(!fp)
		  {
		    printf("Error on opening the pipe to the kill command \n");
		    rhinoDevicePtr->successBit = FAILURE;
		    
		    return rhinoDevicePtr;
		  }
		  
		  printf("BOF PROCESS HAS BEEN SUCCESSFULLY STOPPED\n");
		  rhinoDevicePtr->busyBit  = FPGA_UNPROGRAMMED;
		  rhinoDevicePtr->successBit = SUCCESS;
		
		
	     }
	     
	     else
	     {
		printf("The BOF PROCESS GIVEN IS NOT THE SAME AS THE ONE RUNNING ON THE BOARD\n");
		
		errorMsg = "THE RHINO BOARD IS NOT PROGRAMMED";
		    
		strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
		rhinoDevicePtr->errorMsg[256] = '\0';  //(THINK ABOUT HOW YOU ARE GOING TO GET THIS INFORMATION BACK TO THE SWITCH STATEMENT)
	    
		
		rhinoDevicePtr->successBit = FAILURE;
		return rhinoDevicePtr;
	     }
	 }
	
      }
      
      else
      {
	printf("SORRY BOF PROCESS DID NOT STOP\n");
	
	errorMsg = "SORRY BOF PROCESS DID NOT STOP";
		    
	strncpy(rhinoDevicePtr->errorMsg, errorMsg, sizeof(rhinoDevicePtr->errorMsg)-1);
	rhinoDevicePtr->errorMsg[256] = '\0';  //(THINK ABOUT HOW YOU ARE GOING TO GET THIS INFORMATION BACK TO THE SWITCH STATEMENT)
	    
	rhinoDevicePtr->successBit = FAILURE;
	
	return rhinoDevicePtr;

      }

	return rhinoDevicePtr;
          
}
 

/**************************************************************************************************************************************************************************
 * This function retrieves the IP and MAC address of each unique RHINO board
 *
 *************************************************************************************************************************************************************************************/ 
network_infoT *getIpAddress(uint32_t nameNumber)
{
      uint8_t ipAddress;
      uint8_t macAddress;
      network_infoT *rhino_info, rhino_infoObj;
      char data[256];
      char macAdd[18];
      int counter;
      FILE *fp1;
      FILE *fp;
     rhino_info = &rhino_infoObj;
     
	
      if(RHINOID!=nameNumber)
      {
	
	printf("SORRY MESSAGE IS AT WRONG RHINO NODE\n");
	
	return NULL;
	
      }
      
      else if(RHINOID == nameNumber)
      {
	     printf("EXTRACTING THE IP ADDRESS AND MAC ADDRESS\n");
	     
	     fp1 = popen("more /sys/class/net/eth0/address","r");
	     
	     //printf("GETTING BEYOND POPEN COMMAND\n");
	     
	     if(!fp1)
	     {
	       printf("ERROR ON OPENNING THE FILE TO READ THE MAC ADDRESS\n");
	       return NULL;
	     }
	    
	     if(fgets(macAdd, 18, fp1)!= NULL)
	     {
		//printf("GETTING AFTER FGETS THE FILE AND THE MAC ADDRESS IS: [%s]\n",macAdd);
		//rhino_info->macAddress[sizeof(rhino_info->macAddress)] = '\0';
		strncpy(rhino_info->macAddress, macAdd, sizeof(rhino_info->macAddress));
		//printf("GETTING HERE AFTER STRNCPY\n");
		printf("THE MAC ADDRESS OF THE MACHINE USING THE SYSTEM FILE IS: [%s]\n", rhino_info->macAddress);
		
	     }
	     else
	     {
	       printf("THE FGETS COMMAND ENCOUNTERED A NULL ELEMENT\n");
	     }
	      
	     fp = popen("ifconfig | grep 'inet addr' | cut -d ':' -f2 | cut -d ' ' -f1","r");
	      
	      if(fp==NULL)
	      {
		  printf("Error on openning the pipe to rhino_pwr \n");
		  return NULL;
	      }
	      
	
	     if(fgets(data,256,fp)!=NULL)  
	     {
		// printf("GETTING INTO IP ADDRESS\n");
		 strncpy(rhino_info->ipAddress, strtok(data,"\n"), sizeof(rhino_info->ipAddress)-1);
		 rhino_info->ipAddress[sizeof(rhino_info->ipAddress)] = '\0';
		// printf("THE IP ADDRESS OF THE RHINO IS: [%s]\n", rhino_info->ipAddress);
	     }
	   // fclose(fp);
	     return rhino_info;
	
      }
      else
      {
	printf("UNABLE TO RETREIVE REQUIRED INFORMATION\n");
	
	return NULL;
      }
   
  
}
