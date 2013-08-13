/*
 * RhinoClusterClient.c
 *
 *  Created on: Nov 9, 2012
 *      Author: valerie
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include  <stdint.h>
#include <netdb.h> 
#include <errno.h>
#include <ctype.h>
#include <sys/time.h>
#include "clientLibraryHeader.h"

#define BOARD_ACTIVE 		1
#define BOARD_INACTIVE 		0
#define LOAD_BOF_FILE 		3
#define REGISTER_WRITE 		1
#define REGISTER_READ 		2
#define KILL_BOF_PROCESS 	4
#define LIST_REGISTERS 		5
#define SUCCESS    		1
#define FAILURE	   		0
#define FPGA_UNPROGRAMMED 	0
#define FPGA_PROGRAMMED		1
#define MAX_PACKET		468
#define CLIENT_MESSAGE_HEADER  12




int numberOfClusterRhinos;
RhStatTableT *rhinoStatTable = NULL;
int RhinoClusterCounter=0; 
struct sockaddr_in serv_addr;
struct hostent *server; //the struct is defined in netdb.h



void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main (int argc, char *argv[])
{
	/***********************************************************************************************************************************************************************************************
	 * THIS IS WHERE I WOULD NEED TO DEFINE MY GUI INTERATCTION WITH THE USERS
	 ***************************************************************************************************************************************************************************************************/
	int methodNum;
	int rhinoNameNumber;
	char *regName;
	char *bofProcess;
	char *ipAddress;
	int *ipAddress2;
	int programUserId = getpid(); //gets this programs program ID
	uint16_t data;
	int numberOfRhinosOnCluster;
	RegisterT *returnedRegisters;
	int i;
	int successBit;
	char *serverUserName; //MAYBE THIS COULD BE TAKEN BEHIND THE SCENES?????!!!!!! [HOW]
	char *portno;
	int testTimes;
	
	
	
	
	
      for(;;)
      {
	printf("MAIN MENU:\n");
	printf("1: numRhino\n");
	printf("2: listDeviceRegisters\n");
	printf("3: clusterStatus\n");
	printf("4: loadBofFile\n");
	printf("5: regWrite\n");
	printf("6: regRead\n");
	printf("7: connectToRhino\n");
	printf("8: rhinoSelect\n");
	printf("9: rhinoRelease\n");
	printf("10: connectToSpecificRhino\n");
	printf("11: disconnectToRhino (NOT YET WORKING STILL TO BE IMPLEMENTED!!!)\n");
	printf("12: killBofProcess\n");
	printf("\n");


	
	printf("PLEASE ENTER IN THE COMMAND YOU WOULD LIKE TO CALL:\n");
	scanf("%d",&methodNum);
	
	switch (methodNum)
	{
	  case 1:
	    
	    numberOfRhinosOnCluster =-1;
	    
	    printf("YOU HAVE CHOOSEN THE METHOD numRhino\n"); 
	    printf("PLEASE NOTE YOU CANT USE THIS METHOD UNTILL THE CONNECT METHOD HAS BEEN CALLED\n");
	    
	    numberOfRhinosOnCluster = numRhinos();
	    
	    printf("THE NUMBER OF RHINOS ON THE CLUSTER IS: [%d]\n",numberOfRhinosOnCluster);
	    printf("\n");
	    break;
	    
	  /*case 2:
	    printf("YOU HAVE CHOOSEN THE METHOD listRegisters\n");
	    printf("PLEASE NOTE YOU CANT USE THIS METHOD UNTILL THE CONNECT METHOD HAS BEEN CALLED AND A BOF PROCESS HAS BEEN LOADED\n");
	    returnedRegisters = listRegisters();
	    
	    for(i=0;i<sizeof(returnedRegisters);i++)
	    {
	      printf("REGISTER NAME: [%s]\n",returnedRegisters[i].regName);
	      printf("REGISTER READ/WRITE BIT: [%d]\n", returnedRegisters[i].readWriteBit);
	      printf("RHINO REGISTER BELONGS TO: [%d]\n", returnedRegisters[i].rhinoNameNumber); 
	    }
	    
	    break;
	 */   
	  case 2:
	    
	    printf("YOU HAVE CHOOSEN THE METHOD listDeviceRegisters\n");
	    printf("PLEASE ENTER THE NAME OF THE BOFPROCESS YOU WANT TO PRINT REGISTERS FROM, THE RHINO AND YOUR USERNAME \n");
	    
	    scanf("%s %d %s",&bofProcess, &rhinoNameNumber,&serverUserName);
	    printf("THE BOFPROCESS IS: [%s]\n", &bofProcess);
	    
	    returnedRegisters = listDeviceRegisters(rhinoNameNumber, &bofProcess, &serverUserName);
	    
	    printf("FINISHED EXECUTING THE LIST DEVICE REGISTER METHOD\n");
	    printf("\n");
	    /*for(i=0;i<sizeof(returnedRegisters);i++)
	    {
	      printf("REGISTER NAME: [%s]\n",returnedRegisters[i].regName);
	      printf("REGISTER READ/WRITE BIT: [%d]\n", returnedRegisters[i].readWriteBit);
	      printf("RHINO REGISTER BELONGS TO: [%d]\n", returnedRegisters[i].rhinoNameNumber); 
	    }*/
	    break;
	    
	  case 3:
	    
	    printf("YOU HAVE CHOOSEN THE METHOD clusterStatus\n");
	    
	    clusterStatus();
	    printf("\n");
	    break;
	    
	  case 4:
	    
	    successBit =-1;
	    
	    printf("YOU HAVE CHOOSEN THE METHOD loadBofFile\n");
	    printf("PLEASE ENTER IN THE BOFPROCESS NAME THE RHINO NUMBER AND THE USERS SERVERNAME\n");
	    
	    scanf("%s %d %s",&bofProcess, &rhinoNameNumber, &serverUserName);
	    
	    printf("THE BOFPROCESSNAME IS: [%s]\n",&bofProcess);
	    printf("THE SERVERUSERNAME IS: [%s]\n",&serverUserName);
	    
	    successBit = loadBofFile(rhinoNameNumber,&bofProcess,&serverUserName,programUserId);
	    
	    printf("IF 1 BOFPROCESS HAS SUCESSFULLY STARTED, ELSE IF 0 BOFPROCESS HAS BOFPROCESS HAS NOT STARTED: [%d]\n",successBit);
	    printf("\n");
	    break;
	    
	  case 5:
	    
	    successBit = -1;
	   
	    printf("YOU HAVE CHOOSEN THE METHOD regWrite\n");
	    printf("PLEASE ENTER THE RHINO YOU WANT TO WRITE TO, THE NAME OF THE REGISTER, AND THE DATA YOU WANT TO WRITE\n");
	    
	    scanf("%d %s %d",&rhinoNameNumber,&regName,&data);
	    
	    printf("REGISTER NAME IS:[%s]\n",&regName);
	    printf("DATA TO BE WRITTEN: [%d]\n",data);
	    
	    successBit =regWrite(rhinoNameNumber,&regName,data,programUserId);
	    
	    printf("IF 1 WRITE HAS SUCCESSFULLY OCCURED, ELSE IF 0 WRITE HAS BEEN UNSUCCESSFUL: [%d]\n",successBit);
	    printf("\n");
	    break;
	    
	  case 6:
	    
	    printf("YOU HAVE CHOOSEN THE METHOD regRead\n");
	    printf("PLEASE ENTER THE RHINO YOU WANT TO READ TO, THE NAME OF THE REGISTER\n");
	    
	    scanf("%d %s",&rhinoNameNumber,&regName);
	    printf("REGISTER NAME IS:[%s]\n",&regName);
	    
	    data =regRead(rhinoNameNumber,&regName,programUserId);
	    
	    printf("DATA IN REGISTER IS: [%d]\n",data);
	    printf("\n");
	    break;
	    
	  case 7:
	    
	    successBit = -1;
	    
	    printf("YOU HAVE CHOOSEN THE METHOD connectToRhino\n");
	    
	    successBit = connectToRhino();
	    
	    printf("IF 1 CONNECTION HAS SUCCESSFULLY EXECUTED ELSE SOME ERROR HAS OCCURED WHEN ATTEMPTING TO CONNECT\n");
	    printf("SUCCESSBIT [%d]\n",successBit);
	    printf("PLEASE CHECK THE STATUS OF THE BOARDS ON THE CLUSTER\n");
	    printf("\n");
	    break;
	    
	  case 8:
	    
	    successBit = -1;
	    
	    printf("YOU HAVE CHOOSEN THE METHOD rhinoSelect\n");
	    printf("PLEASE ENTER IN THE RHINO YOU WANT TO SELECT\n");
	    
	    scanf("%d",&rhinoNameNumber);
	    
	    successBit = rhinoSelect (rhinoNameNumber,programUserId);
	    
	    printf("IF 1 SUCCESS RHINOSELECT HAS BEEN SUCCESSFUL, ELSE IF 0 RHINOSELECT HAS BEEN UNSUCCESSFUL: [%d]\n",successBit);
	    printf("\n");
	    break;
	    
	  case 9:
	    
	     successBit = -1;
	    
	     printf("YOU HAVE CHOOSEN THE METHOD rhinoRelease\n");
	     printf("PLEASE ENTER IN THE RHINO YOU WANT TO RELEASE\n");
	     
	     scanf("%d",&rhinoNameNumber);
	    
	     successBit = rhinoRelease(rhinoNameNumber,programUserId);
	    
	     printf("IF 1 SUCCESS RHINORELEASE HAS BEEN SUCCESSFUL, ELSE IF 0 RHINORELEASE HAS BEEN UNSUCCESSFUL: [%d]\n",successBit);
	     printf("\n");
	    break;
	    
	  case 10:
	    
	    successBit = -1;
	    
	    printf("YOU HAVE CHOOSEN THE METHOD connectToSpecificRhino\n");
	    printf("PLEASE ENTER IN THE IPADDRESS AND THE PORTNO OF THE RHINO YOU WOULD LIKE TO USE\n");
	    
	    scanf("%s %s",&ipAddress,&portno);
	    
	    //printf("THE RHINONAMENUMBER IS: [%d]\n",rhinoNameNumber);
	    printf("THE IPADDRESS IS:[%s]\n",&ipAddress);
	    printf("THE PORTNO IS: [%s]\n",&portno);
	    
	    successBit = connectToSpecificRhino(&ipAddress,&portno);
	    
	    printf("IF 1 RHINO HAS BEEN SUCCESFULLY CONNECTED, ELSE IF 0 RHINO CONNECT HAS BEEN UNSUCCESSFUL: [%d]\n",successBit);
	    printf("\n");
	    
	    break;
	    
	  case 11:
	    
	     printf("YOU HAVE CHOOSEN THE METHOD disconnectToRhino\n");
	     printf("PLEASE ENTER IN THE RHINO NUMBER YOU WOULD LIKE TO DISCONNECT FROM AND THE IPADDRESS \n");
	     
	     scanf("%d %s",&rhinoNameNumber,ipAddress);
	    
	     disconnectToRhino(rhinoNameNumber, ipAddress);
	     printf("\n");
	    break;
	    
	  case 12:
	    
	    successBit = -1;
	    
	    printf("YOU HAVE CHOOSEN THE METHOD killBofProcess\n");
	    printf("PLEASE ENTER THE RHINONUMBER THE BOF PROCESS IS RUNNING ON AND THE BOFPROCESS NAME YOU WOULD LIKE TO STOP\n");
	    
	    scanf("%d %s",&rhinoNameNumber, &bofProcess);
	    
	    successBit = killBofProcess(rhinoNameNumber, &bofProcess,programUserId);
	    
	    printf("IF 1 KILLING BOF PROCESS HAS BEEN SUCCESSFUL, ELSE IF 0 KILLING BOF PROCESS HAS NOT BEEN SUCCESSFUL: [%d]\n",successBit);
	    printf("\n");
	    break;
	    
	  case 13:
	    
	    printf("YOU HAVE CHOOSEN TO TEST THE NETWORK USING CLIENT PACKETS\n");
	    printf("ENTER THE RHINONUMBER, TESTIMES AND THE DATAToTest\n");
	    scanf("%d %d %d", &rhinoNameNumber, &testTimes, &data);
	    clientPacketTest(rhinoNameNumber, testTimes, data);
	    
	    break;
	  case 14:
	    printf("YOU HAVE CHOOSEN TO TEST THE NETWORK USING SERVER PACKETS\n");
	    printf("ENTER THE RHINONUMBER, AND TEST TIMES\n");
	    scanf("%d %d", &rhinoNameNumber, &testTimes);
	    serverPacketTest(rhinoNameNumber, testTimes);
	    break;
	    
	    default:
	      printf("NONE OF THE METHODS YOU HAVE SELECTED ARE IN THE API\n"); 
	      printf("\n");
	      break;
	}
      }
	
	return 0;
	    
}
    //declarations of the method numRhinos
/*******************************************************************************************************************************************************************************
 *1) THIS METHOD RETURNS THE NUMBER OF RHINOS CONNECTED ON THE CLUSTER
*******************************************************************************************************************************************************************************/
int numRhinos()
{
	int numberOfAvailableRhinos = 0;
	int counter;
	    
	for(counter=0; counter<RhinoClusterCounter; counter++)
	{
	    if(rhinoStatTable[counter].activeBit==BOARD_ACTIVE)
	    {
	      numberOfAvailableRhinos++;
	    }
	}
	
   return numberOfClusterRhinos;
	    
}

      
/*******************************************************************************************************************************************************************************************
	    *2) THIS METHOD DISPLAYS ALL AVAILABLE REGISTERS ON THE CLUSTER ON THE AVAILABLE RHINOS ONLY USEFUL IF THE FPGA IS PROGRAMMED PRINTS TO SCREEN
*******************************************************************************************************************************************************************************************/
    
#if 0
	  struct RegisterT  listRegisters()
	  {
	    int counter; 
	    int len;
	    int client_len, server_len;
	    struct client_msg *cm,writeMessage;
	    struct server_msg *sm,readMessage;
	    
	    cm = &writeMessage;
	    sm = &readMessage;
	    
	    for(counter=0; counter<RhinoClusterCounter;counter++)
	    {
	      //First Check if the RHINO is active on the clusterStatus
		if(rhinoStatTable[counter].activeBit==BOARD_ACTIVE)
		{
		  printf("RHINO %d IS ACTIVE\n",rhinoStatTable[counter].rhinoNum);
		  
		  //2) CHECKING IF THE RHINO BOARD IS PROGRAMMED
		  if(rhinoStatTable[counter].rhinoBoard->busyBit==FPGA_PROGRAMMED)
		  {
		    printf("RHINO IS ACTIVE AND PROGRAMMED WITH BOF FILE\n");
		    
		     /******************************************************************************************************************************
		      * USING MESSAGE PACKET STRUCTURES AND TCP SEND AND RECIEVE AS APPOSSED TO READ AND WRITE
		      * 
		     ****************************************************************************************************************************/
		   cm->messageID = LIST_REGISTERS;
		     // cm->messageID = htons(LIST_REGISTERS);
		    cm->rhinoID   = htons(rhinoStatTable[counter].rhinoNum);
		    cm->reg_name[32]   = '\0'; //HOW TO CHANGE THE REGISTER NAME TO A STRING
		    cm->data      = htonl(0);

		     printf("Client_MsgID IS: %d\n",&cm->messageID);
		     printf("Client_RHINOID IS: %d\n",cm->rhinoID);
		     printf("Client_REG_NAME IS: %s\n",cm->reg_name);
		     printf("Client_DATA IS: %d\n",cm->data);
		    
		    client_len = send(rhinoStatTable[counter].sockfd, cm,CLIENT_MESSAGE_HEADER, MSG_NOSIGNAL); //THINK OF ANOTHER WAY OF GETTING THE SIZE OF THE PACKET TO BE SENT
			  
		   //ADD SOME ERROR HANDLING HERE FOR THE SEND
			  
		   server_len = recv(rhinoStatTable[counter].sockfd,sm,MAX_PACKET, MSG_DONTWAIT); //THINK ON THIS PART AND HOW YOU ARE GOING TO SET THE SIZE OF THE MESSAGE TO BE REICEVED
			  
		  
				  sm->rhinoBoard.ipAddress[15]	= sm->rhinoBoard.ipAddress[15];//HOW TO CONVERT THIS STRING INTO A NUMBER SO AS TO HAVE A BINARY PROTOCOL
				   sm->rhinoBoard.ipAddress[16] ='\0';
				  sm->rhinoBoard.busyBit	= ntohl(sm->rhinoBoard.busyBit);
				  sm->rhinoBoard.registersAccessible.regName[15] 		= sm->rhinoBoard.registersAccessible.regName[15];//HOW TO CONVERT THIS STRING TO A NUMBER TO HAVE A BINARY PROTOCOL
				  sm->rhinoBoard.registersAccessible.regName[16]		='\0';
				  sm->rhinoBoard.registersAccessible.readWriteBit 	= ntohl(sm->rhinoBoard.registersAccessible.readWriteBit);
				  sm->rhinoBoard.registersAccessible.rhinoNameNumber = ntohl(sm->rhinoBoard.registersAccessible.rhinoNameNumber);
			  
		   sm->data = ntohs(0); //IN THE WRITE MESSAGE THE RESPONSE FROM THE SERVER SHOULD HAVE NOTHING IN THE DATA ELEMENT
		   sm->successBit = ntohs(sm->successBit);
		    
		    	    
		    
/****************************************************************************************************************************************************************************************************************************************************************************/		    
		     //WRAPPING MESSAGE ID INTO PACKET AND SENDING TO RHINO USING CORRESPONDING SOCKET =SOCKFD
	/*	    bzero(protocolPacket,256);
		    len = snprintf(protocolPacket,sizeof(protocolPacket),"%d,%d,%c,%d",LIST_REGISTERS,0,"0",0);
		    printf("protocolPacket from snprintf: %s\n",protocolPacket);
		    n = write(rhinoStatTable[counter].sockfd,protocolPacket, 256);
		    bzero(protocolPacket,256);
		    n = read(rhinoStatTable[counter].sockfd, protocolPacket, 256);
		    bzero(protocolPacket,256);
		    
		    //FIND WAY TO UNWRAPP THE PACKET AND STORE THE DATA FROM THE REGISTERS......Decided to print to string with GUI
		    //First Need to decode the protocolPacket
		  //Use strchr to decode protocolPacket
		    char *stringMessageId = strchr(protocolPacket,"[MID]");
		    char *stringRhinoId = strchr(protocolPacket,"[RID]");//NEED TO SEE IF IAM NOT WASTING SPACE WITH THIS STRING
		    char *registerName = strchr(protocolPacket,"[RN]");
		    char *stringRegData = strchr(protocolPacket,"[D]");
		    
		    unsigned long messageId =strtol(stringMessageId,NULL,10);
		    unsigned long rhinoId = strtol(stringRhinoId,NULL,10);
		    unsigned long regData = strtol(stringRegData,NULL,10);
		    */
		    
/**************************************************************************************************************************************************************************************************************/		    
		    return   sm->rhinoBoard.registersAccessible;
		    
		  }
		  else if(rhinoStatTable[counter].rhinoBoard->busyBit==FPGA_UNPROGRAMMED)
		  {
		    printf("RHINO IS ACTIVE BUT IS NOT PROGRAMMED WITH A BOF FILE\n");
		    return NULL;
		  }
			
		}
		else if(rhinoStatTable[counter].activeBit==BOARD_INACTIVE)
		{
		  printf("RHINO %d IS INACTIVE IN THE CLUSTER AT THE MOMENT\n",rhinoStatTable[counter].rhinoNum);
		  return NULL;
		}
	    }
	  }
#endif
/******************************************************************************************************************************************************************************************
*3) THIS METHOD DISPLAYS THE AVAILABLE REGISTER ON A NAMED RHINO ONLY USEFUL IF THE FPGA IS PROGRAMMED 
******************************************************************************************************************************************************************************************/
RegisterT  *listDeviceRegisters(uint32_t rhinoNameNumber, char *bofProcessName, char *serverUserName)
{
	      int counter;
	      int len;
	      int buf_size = 0;
	      int buf_counter = 0; 
	      RegisterT *rhinoReg, *tmpReg, rhinoRegisters;
	      client_msg_T *cm,	readMessage;
	      server_msg_T *sm,	writeMessage;
	      reg_msg_T *rm, registerMessage;
	      int client_len, server_len;
	      
	      rhinoReg =NULL;
	      tmpReg = NULL;
	      cm  = &readMessage;
	      sm = &writeMessage;
	      rm = &registerMessage;
	      
	      for(counter=0; counter<RhinoClusterCounter;counter++)
	      {
			//1) First IF statement checking to see if named RHINO is in the cluster
			if(rhinoStatTable[counter].rhinoNum == rhinoNameNumber)
			{
				  printf("THE RHINO IS IN THE CLUSTER\n");
				  //FOUND RHINO IS IN THE CLUSTER 2)Checking to see if the RHINO is active or Not
				  if(rhinoStatTable[counter].activeBit==BOARD_INACTIVE)
				  {
					    printf("RHINO %d IS INACTIVE",rhinoStatTable[counter].rhinoNum);
					    
					  //CHECKING TO SEE IF THE RHINO IS PROGRAMMED BEFORE CHECKING FOR THE REGISTERS
					    if(rhinoStatTable[counter].rhinoBoard.busyBit ==FPGA_PROGRAMMED)
					    {
					      printf("RHINO IS ACTIVE AND PROGRAMMED\n");
					      
					      /******************************************************************************************************************************
					      * USING MESSAGE PACKET STRUCTURES AND TCP SEND AND RECIEVE AS APPOSSED TO READ AND WRITE
					      * 
					    ****************************************************************************************************************************/
					    cm->messageID = htonl(LIST_REGISTERS);
					    cm->rhinoID   = htonl(rhinoNameNumber);
					    strncpy(cm->reg_name, bofProcessName,sizeof(cm->reg_name)-1);//HOW TO CHANGE THE REGISTER NAME TO A STRING
					    cm->reg_name[32]   = '\0';
					    cm->data      =   htons(0);
					    strncpy(cm->user_name, serverUserName, sizeof(cm->user_name)-1);
					    cm->user_name[16] = '\0';
					    
					    printf("Client_MsgID IS: %d\n",cm->messageID);
					    printf("Client_RHINOID IS: %d\n",cm->rhinoID);
					    printf("Client_REG_NAME IS: %s\n",cm->reg_name);
					    printf("Client_DATA IS: %d\n",cm->data);
					    printf("Client_USER_NAME: [%s]\n", cm->user_name);
					    
					    

					    client_len = send(rhinoStatTable[counter].sockfd, cm,sizeof(readMessage), MSG_NOSIGNAL); //THINK OF ANOTHER WAY OF GETTING THE SIZE OF THE PACKET TO BE SENT
					    printf("JUST SENT THE CLIENT MSG\n");
					  //ADD SOME ERROR HANDLING HERE FOR THE SEND
						  
					  server_len = recv(rhinoStatTable[counter].sockfd, sm, sizeof(writeMessage), 0); //THINK ON THIS PART AND HOW YOU ARE GOING TO SET THE SIZE OF THE MESSAGE TO BE REICEVED
						  
					  if(server_len < 0){
					    switch(errno){
						    case EAGAIN :
						    case EINTR  :
							    return 0;
						    default :
							    error("tcp receive failed");
							    return 0;
					    }
					  }
					  printf("RECIEVED THE SERVER MSG\n");
					  
					  sm->rhinoBoard.nameNumber 	= ntohl( sm->rhinoBoard.nameNumber);
					  printf("RHINO NAME NUMBER:[%d]\n",sm->rhinoBoard.nameNumber);
					  
					  strncpy(sm->rhinoBoard.ipAddress, sm->rhinoBoard.ipAddress,sizeof(sm->rhinoBoard.ipAddress)-1);//HOW TO CONVERT THIS STRING INTO A NUMBER SO AS TO HAVE A BINARY PROTOCOL
					  sm->rhinoBoard.ipAddress[16] ='\0';
					  printf("THE RHINO IP ADDRESS IS: [%s]\n",sm->rhinoBoard.ipAddress);
					  
					  sm->rhinoBoard.busyBit  = ntohl(sm->rhinoBoard.busyBit);
					  printf("RHINO BUSYBIT: [%d]\n",sm->rhinoBoard.busyBit);
					  
					  strncpy(sm->rhinoBoard.registersAccessible.regName, sm->rhinoBoard.registersAccessible.regName, sizeof(sm->rhinoBoard.registersAccessible.regName)-1);//HOW TO CONVERT THIS STRING TO A NUMBER TO HAVE A BINARY PROTOCOL
					  sm->rhinoBoard.registersAccessible.regName[32]	='\0';
					  //printf("THE RHINO REGISTER NAMES AS IS: [%s]\n",sm->rhinoBoard.registersAccessible.regName);
					  
					  
					  //printf("THE RHINO REGISTER READWRITEBIT AS IS: [%d]\n",sm->rhinoBoard.registersAccessible.readWriteBit);
					  sm->rhinoBoard.registersAccessible.readWriteBit 	= ntohs(sm->rhinoBoard.registersAccessible.readWriteBit);
					  //printf("THE RHINO REGISTER READWRITEBIT AS IS: [%d]\n",sm->rhinoBoard.registersAccessible.readWriteBit);
					  
					  
					  //printf("THE RHINO REGISTER RHINONAMENUMBER AS IS: [%d]\n",sm->rhinoBoard.registersAccessible.rhinoNameNumber);
					  sm->rhinoBoard.registersAccessible.rhinoNameNumber = ntohl(sm->rhinoBoard.registersAccessible.rhinoNameNumber);
					 
					  
					  //printf("THE RHINO REGISTER SIZE IS: [%d]\n",sm->rhinoBoard.registersAccessible.size);
					  sm->rhinoBoard.registersAccessible.size = ntohl(sm->rhinoBoard.registersAccessible.size);
					  
					  sm->data = ntohs(0); //IN THE WRITE MESSAGE THE RESPONSE FROM THE SERVER SHOULD HAVE NOTHING IN THE DATA ELEMENT
					  printf("RHINO DATA IS: [%d]\n", sm->data);
					   
					  sm->rhinoBoard.successBit = ntohl(0);
					  printf("RHINO SUCCESS BIT: [%d]\n", sm->rhinoBoard.successBit);
					  
					  
					  
					  if(buf_counter == buf_size)
					     {
						buf_size +=1; 
						tmpReg = realloc(rhinoReg, buf_size);
							
						if(!tmpReg)
						{
						  printf("COULD NOT DYNAMICALLY CHANGE THE SIZE OF REGISTER ARRAY\n");
						  return NULL;
						}
						rhinoReg = tmpReg;
					     }
						    
						
					   
					    buf_counter++;
					    rhinoReg[buf_counter].rhinoNameNumber = sm->rhinoBoard.registersAccessible.rhinoNameNumber;
					    printf("THE RHINO REGISTER RHINONAMENUMBER IS: [%d]\n",rhinoReg[buf_counter].rhinoNameNumber);
					    
					    strncpy(rhinoReg[buf_counter].regName, sm->rhinoBoard.registersAccessible.regName, sizeof(rhinoReg[buf_counter+1].regName)-1);
					    rhinoReg[buf_counter].regName[32] = '\0';
					    printf("THE RHINO REGISTER NAME IS: [%s]\n",sm->rhinoBoard.registersAccessible.regName);
					    
					    rhinoReg[buf_counter].readWriteBit = sm->rhinoBoard.registersAccessible.readWriteBit;
					    printf("THE RHINO REGISTER READWRITEBIT IS: [%d]\n",rhinoReg[buf_counter].readWriteBit);
					    
					    rhinoReg[buf_counter].size = sm->rhinoBoard.registersAccessible.size;
					    printf("THE RHINO REGISTER SIZE IS: [%x]\n", rhinoReg[buf_counter].size);
					  
					  
					  //IF THE REGNAME IS EMPTY THAT MEANS THERE IS NO INFORMATION IN THE REST OF THE REGISTER
					  if(rhinoReg[buf_counter].regName == NULL) 
					  {
					    printf("NO REGISTERS ON BOF PROCESS\n");
					   
					    return rhinoReg;
					  }
					  
					  else if(rhinoReg[buf_counter].regName != NULL)
					  {
					   // server_len = recv(rhinoStatTable[counter].sockfd, rm, sizeof(registerMessage), 0);
					    
					    //1: CHECK IF THE PACKET IS EMPY OR NULL
					     do 
					      {
						    printf("THE NEXT REGISTER ON THE BOARD IS\n");
						    
						    server_len = recv(rhinoStatTable[counter].sockfd, rm, sizeof(registerMessage), 0);
						   
					
						    if(buf_counter == buf_size)
						    {
							buf_size +=5; 
							tmpReg = realloc(rhinoReg, buf_size);
							
							if(!tmpReg)
							{
							  printf("COULD NOT DYNAMICALLY CHANGE THE SIZE OF REGISTER ARRAY\n");
							  return NULL;
							}
							rhinoReg = tmpReg;
						    }
						    
						   // buf_counter++;
						     
						    rhinoReg[buf_counter].rhinoNameNumber = ntohl(rm->registersAccessible.rhinoNameNumber);
						    printf("THE RHINO REGISTER RHINONAMENUMBER IS: [%d]\n",rhinoReg[buf_counter].rhinoNameNumber);
						    
						    strncpy(rhinoReg[buf_counter].regName, rm->registersAccessible.regName, sizeof(rhinoReg[buf_counter].regName)-1);
						    rhinoReg[buf_counter].regName[32] = '\0';
						    printf("THE RHINO REGISTER NAMES IS: [%s]\n",rhinoReg[buf_counter].regName);
						     
						    rhinoReg[buf_counter].readWriteBit = ntohs(rm->registersAccessible.readWriteBit);
						    printf("THE RHINO REGISTER READWRITEBIT IS: [%d]\n",rhinoReg[buf_counter].readWriteBit);
						    
						    rhinoReg[buf_counter].size = ntohl(rm->registersAccessible.size);
						    printf("THE RHINO REGISTER SIZE IS: [%d]\n",rhinoReg[buf_counter].size);
						    
						   
					      }   while(rhinoReg[buf_counter].regName[0] !='\0');
						  
						  printf("THERE ARE NO MORE REGISTERS TO RECIEVE\n");
						  return rhinoReg;
					    }     
					      
				      }
				      else if(rhinoStatTable[counter].rhinoBoard.busyBit == FPGA_UNPROGRAMMED)
				      {
					printf("RHINO IS ACTIVE AND YOU HAVE ACCESS BUT IT IS UNPROGRAMMED\n");
					return NULL;
				      }
					    
			}
			else
			{
			    printf("RHINO %d IS INACTIVE IN THE CLUSTER AT THE MOMENT\n",rhinoStatTable[counter].rhinoNum);
			    return NULL;
			}
				      //break;
		}
		
	 }
	//THIS MEANS THAT THE RHINO WAS NOT FOUND ON THE CLUSTER
	    
	error("THE RHINO WAS NOT FOUND ON THE CLUSTER"); 
	return NULL;
	    
}
    
	    
/*******************************************************************************************************************************************************************************************
*4) THIS METHOD LISTS THE STATUS OF ALL THE AVAILABLE RHINOS ON THE CLUSTER
@#$%#$# NB: IN KATCP, If the FPGA is programmed a string path to the hardware registers is returned eg "!Status ok, /proc.335/hw/ioreg #$@%#
********************************************************************************************************************************************************************************************/
void clusterStatus() //This method will use the RHINO Status Table and the Active Bit to determine this information output would be to screen in form of GUI
{
	      int counter;
	      //loop through the RHINO Stat table printing out the State of each RHINO
	      printf("_________________________________________________________\n");
	      printf("RHINO NUMBER\t|RHINO IP Address\t|STATUS\t|BUSYBIT|\n");
	      printf("_________________________________________________________\n");
	      
	      for(counter=0;counter<RhinoClusterCounter;counter++)
	      {
		/****************************************************************************************************************************************************************************************
		VERY IFFY ABOUT THIS PRINTING METHOD MAY NEED TO LOOK THROUGH IT AGAIN AND REWRITE THIS PRINTF STATEMENT!!!!!!!!!!!!!!!!!!!!!!
		********************************************************************************************************************************************************************************************/
		  
		  printf(" %d\t\t| %s\t\t| %d\t| %d\t|\n",rhinoStatTable[counter].rhinoNum,rhinoStatTable[counter].rhinoIpAdd,rhinoStatTable[counter].activeBit, rhinoStatTable[counter].rhinoBoard.busyBit );
		
	      }
	       printf("_________________________________________________________\n");
	    }

	    
 /****************************************************************************************************************************************************************************************************************************
*5) THIS METHOD ALLOWS A USER TO START A BOF FILE ON A GIVEN RHINO NODE.IT TAKES IN THE RHINOS IPADDRESS, THE NAME OF THE BOF PROCESS TO BE EXECUTED (THIS BOF FILE SHOULD ALREADY EXIST IN THE NFS FILE SYSTEM ON THE SERVER
AND LASTLY THE USERS NAME AS IS ON THE SERVER.
NB ASSUMES USER HAS ALREADY SELECTED NAMED RHINO FOR USE USING RHINO_SELECT METHOD
****************************************************************************************************************************************************************************************************************************/    
	   
int loadBofFile(uint32_t rhinoNumber, char *bofProcessName, char *serverUserName,uint32_t programUserId)
{
	      
		char command[255];
		char serverUserNameCommand[255];
		char bofNfsFiles[64];
		size_t ln;
		int len;
		char bofProcessName2[255]; 
		int counter;
		FILE *userNfsFile, *fp;
		int statusBit;
		client_msg_T *cm, writeMessage;
		server_msg_T *sm, readMessage;
		int client_len, server_len;
		struct timeval begin, end;
	        long sec, usecs, mtime;
	        unsigned int timeElapsed;
		
		cm = &writeMessage;
		sm = &readMessage;
		fp = fopen("loadBofFile.txt","w");
		
		/*#####################################################################################################################################################################
	      //First Check that the bofProcess being programmed actually exists on the file system before going further by openning the /home directory under users name on server
	      
	      $Need to also check that the .bof files are executable....THINK ON THAT ONE
	      $also needs to fist select the RHINO and check that it is actually available to have something run on it
	      ########################################################################################################################################################################*/
	       
	      bzero(serverUserNameCommand,255);
	      //printf("GETTING TO THIS POINT\n");
	     
	     // bofProcessName = "rhino_adder";
	      strcpy(bofProcessName2,bofProcessName);
	      strcat(bofProcessName2,".bof");
	      printf("BOFPROCESSNAME2: [%s]\n", bofProcessName2);
	      
	     // char *bofProcessName2 ="rhino_adder.bof";

	      len = snprintf(serverUserNameCommand,sizeof(serverUserNameCommand),"ls /srv/rhinoNFS/home/%s/%s/ |grep .bof$",serverUserName,bofProcessName); //cding into the directory that contains pressumably just .bof files
	     
	      //printf("Command from snprintf: %s\n",serverUserNameCommand);
	      
	     
	     if(len <= sizeof(serverUserNameCommand))
	      {
		userNfsFile= popen(serverUserNameCommand,"r");
		
		//NEED ERROR HANDLING HERE JUST IN CASE USER DOESNT HAVE READ PRIVILAGES OR THE USER CANT BE FOUND ON THE SERVER
		
		if(!userNfsFile)
		{
		  error("Error on openning the pipe to write the command to cd into diretory \n");
		  return FAILURE;
		}
		else if(userNfsFile==NULL)
		{
		  error("ERROR THE userNfsFile is NULL");
		}
	      }
	      
		// printf("GETTING TO POINT AFTER THE POPEN COMMAND\n"); 
	      
		/**************************************************************************************************************
		 * 1)CHECK THAT THE BOF PROCESS IS ACTUALLY ON THE SERVER FILE SYSTEM
		 ***************************************************************************************************************/
		gettimeofday(&begin, NULL);	
		
		while(fgets(bofNfsFiles,255,userNfsFile)!=NULL)
		{
		  //printf("GETTING INTO THE WHILE LOOP\n");
		  //printf("FILE FROM THE FOLDER:[%s]\n",bofNfsFiles);
		  
		  ln = strlen(bofNfsFiles);
		  
		  if (bofNfsFiles[ln-1] == '\n')
		  {
		      bofNfsFiles[ln-1] = '\0';
		  }
		    //printf("FILE AFTER NEWLINE REMOVED:[%s]\n",bofNfsFiles);
		  /**************************************************************************************************************
		 * 2)CHECK IF USER HAS ACCESS TO THE RHINO BOARD REQUESTED TO RUN
		 ***************************************************************************************************************/
		  if(strcmp(bofNfsFiles,bofProcessName2) == 0)
		  {
		    //printf("BOFPROCESS IS ON THE SERVER: [%s]\n",bofProcessName);
		    
		      for(counter=0; counter<RhinoClusterCounter; counter++)
			{
			  //printf("GETTING INTO THE FOR LOOP\n");
			  if(rhinoNumber == rhinoStatTable[counter].rhinoNum)
			  {
			    printf("FOUND THE RHINO REQUESTED\n");
			    //printf("THE PROGRAMUSERID FOR THE RHINO IS: [%d]\n",rhinoStatTable[counter].programUserId);
			    //printf("THE PROGRAMUSERID FROM THE USER IS: [%d]\n",programUserId);
			    
			    
			    
			    //Checking if the IpAddress is valid that the RHINO is not being utilized by another User
			    if(rhinoStatTable[counter].activeBit == BOARD_ACTIVE)
			    {
			      printf("RHINO IPADDRESS EXISTS AND THE BOARD ACTIVE, BUT YOU HAVE NOT SELECTED IT\n");
			      return FAILURE;
			    }
			    else if ((rhinoStatTable[counter].activeBit == BOARD_INACTIVE ) && (rhinoStatTable[counter].programUserId == programUserId))
			    {
			      
				printf("RHINO IPADDRESS DOES EXIST AND IS BEING PROGRAMMED\n");
		/**************************************************************************************************************
		* 3)SEND THE USERNAME AND BOFPROCESS NAME TO THE SERVER
		***************************************************************************************************************/
				  
				  cm->messageID  = htonl(LOAD_BOF_FILE);
				  
				  cm->rhinoID 	 = htonl(rhinoStatTable[counter].rhinoNum);
				  
				  strncpy(cm->reg_name, bofProcessName, sizeof(cm->reg_name)-1);
				  cm->reg_name[32]  ='\0';
				  
				  strncpy(cm->user_name, serverUserName, sizeof(cm->user_name)-1);
				  cm->user_name[16] = '\0';
				  
				  cm->data 	 = htons(0);
				  
				  printf("Client_MsgID IS: [%d]\n", cm->messageID);
				  printf("Client_RHINOID IS: [%d]\n", cm->rhinoID);
				  printf("Client_REG_NAME IS: [%s]\n", cm->reg_name);
				  printf("Client_DATA IS: %d\n", cm->data);
				  printf("Client_USER_NAME: [%s]\n", cm->user_name);
				
				  
				 // printf("THE SIZE OF THE INFORMATION BEING SENT: [%d]\n",sizeof(
				  
				  client_len = send(rhinoStatTable[counter].sockfd, cm, sizeof(writeMessage), MSG_NOSIGNAL); 
				  printf("JUST SENT THE CLIENT MSG\n");
				  //ADD SOME ERROR HANDING CODE HERE
				    
				    
				  server_len = recv(rhinoStatTable[counter].sockfd, sm, sizeof(readMessage), 0); 
				  
				  if(server_len < 0){
				    switch(errno){
					    case EAGAIN :
					    case EINTR  :
						    return 0;
					    default :
						    error("tcp receive failed");
						    return 0;
				    }
				  }
				   printf("RECIEVED THE SERVER MSG\n");
				   
				  sm->rhinoBoard.nameNumber 	= ntohl( sm->rhinoBoard.nameNumber);
				  printf("RHINO NAME NUMBER:[%d]\n",sm->rhinoBoard.nameNumber);
				  
				  strncpy(sm->rhinoBoard.ipAddress, sm->rhinoBoard.ipAddress,sizeof(sm->rhinoBoard.ipAddress)-1);//HOW TO CONVERT THIS STRING INTO A NUMBER SO AS TO HAVE A BINARY PROTOCOL
				   sm->rhinoBoard.ipAddress[16] ='\0';
				   printf("THE RHINO IP ADDRESS IS: [%s]\n",sm->rhinoBoard.ipAddress);
				   
				  sm->rhinoBoard.busyBit	= ntohl(sm->rhinoBoard.busyBit);
				  rhinoStatTable[counter].rhinoBoard.busyBit = sm->rhinoBoard.busyBit;
				  printf("THE RHINO BUSY BIT IS: [%d]\n",sm->rhinoBoard.busyBit);
				  
				  strncpy(sm->rhinoBoard.registersAccessible.regName, sm->rhinoBoard.registersAccessible.regName, sizeof(sm->rhinoBoard.registersAccessible.regName)-1);//HOW TO CONVERT THIS STRING TO A NUMBER TO HAVE A BINARY PROTOCOL
				  sm->rhinoBoard.registersAccessible.regName[32]		='\0';
				  printf("THE RHINO REGISTER NAMES AS IS: [%s]\n",sm->rhinoBoard.registersAccessible.regName);
				  
				  sm->rhinoBoard.registersAccessible.readWriteBit 	= ntohs(sm->rhinoBoard.registersAccessible.readWriteBit);
				  sm->rhinoBoard.registersAccessible.rhinoNameNumber = ntohl(sm->rhinoBoard.registersAccessible.rhinoNameNumber);
				  
				  strncpy(sm->rhinoBoard.errorMsg, sm->rhinoBoard.errorMsg, sizeof(sm->rhinoBoard.errorMsg)-1);
				  sm->rhinoBoard.errorMsg[256] = '\0';
				  
				  printf("ERROR MSG FROM THE RHINO BOARD: [%s]\n", sm->rhinoBoard.errorMsg);
				  
				  
				  sm->data = ntohs(sm->data); //IN THE WRITE MESSAGE THE RESPONSE FROM THE SERVER SHOULD HAVE NOTHING IN THE DATA ELEMENT
				  sm->rhinoBoard.successBit = ntohl(sm->rhinoBoard.successBit);
				 
				  
				  printf("RHINO BUSYBIT: [%d]\n",sm->rhinoBoard.busyBit);
				  printf("RHINO SUCCESS BIT: [%d]\n",sm->rhinoBoard.successBit);
				  gettimeofday(&end, NULL);
		      
				  sec = end.tv_sec - begin.tv_sec;
				  usecs = end.tv_usec - begin.tv_usec;
				  
				  mtime = ((sec)*1000+ usecs/1000.0) + 0.5;
				  
				  printf("THE ELAPSED TIME FOR LOAD_BOF_FILE IS: [%1d]\n",mtime);
				  fprintf(fp, "%d\n",mtime);
				  fclose(fp);
				  
				 
				   
			      return sm->rhinoBoard.successBit;
			    }
			     else if ((rhinoStatTable[counter].activeBit==BOARD_INACTIVE)&&(rhinoStatTable[counter].programUserId != programUserId))
			     {
			       printf("RHINOBOARD IS INACTIVE MEANING ITS NOT RUNNING OR IS SELECTED BY ANOTHER USER, PLEASE SELECT ANOTHER RHINOBOARD\n");
			       return FAILURE;
			     }
			    else 
			    {
			      printf("UNKNOW RHINOSTATTABLE STATE\n");
			      return FAILURE;
			    }
			   
			  }
			}
			      
		    
		  }		
		}
		printf("COULD NOT FIND THE BOF FILE REQUESTED BY USER,UNABLE TO EXECUTE COMMAND\n");
		return FAILURE;  
	      
}
	 

	 
 /*******************************************************************************************************************************************************************************************
    *6) THIS METHOD ALLOWS FOR THE RHINOCLUSTERCLIENT SITTING ON THE PC SERVER TO CONNECT TO THE RHINO NODES GIVEN THIER STATIC IP ADDRESSES ON THE DHCP client
 ********************************************************************************************************************************************************************************************/
int connectToRhino()
{
	FILE *fileIpAdd2 =NULL; //this is the file that the servers running on the rhinos update when they start hence it contains the number of rhinos that have registered with it
	char rhinoInfoChar[256];
	uint32_t rhinoInfo;
	char ping[255];
	char *rhinoIpString;
	int counter;
	int len;
	int length;
	int size;
	int n;
	char numberOfClusterRhinoChar[256];
	char *portNoString;
	int status = 0;
	RhStatTableT *tempRhinoStatusTable = NULL;
	
	//The Client will connect to the Server through A TEXT FILE with the RHINOs static IP addresses and socket Numbers
	  fileIpAdd2 = fopen("RHINOIPAdd.txt","r"); 
		     
	      
	  if(fileIpAdd2==NULL)
	  {
	      error("Failed to open fileIpAdd.txt file\n");
			
	  }
	  
	  
	  //CASE 1 ... CHECK IF THERE IS NO RHINOSTATUS TABLE AREADY CREATED
	  if(rhinoStatTable !=NULL)
	  {
	    printf("RHINOSTATUSTABLE ALREADY EXISTS: CASE 1\n");
	    
	      if (fgets(numberOfClusterRhinoChar,256,fileIpAdd2)!= NULL)
	      {
				  
		numberOfClusterRhinos = atoi(numberOfClusterRhinoChar);
		//printf("NUMBER OF RHINOS ON THE CLUSTER AVAILABLE OR NOT ARE:[%d]\n",numberOfClusterRhinos);
	      }
	      else
	      {
		  error("File Refused to Open");
	      }
	      
	      
	      
	      
	     while(fgets(rhinoInfoChar,256,fileIpAdd2))
	     {
	       
		    rhinoIpString= strtok(rhinoInfoChar," ");
		    portNoString=strtok(NULL," \n");
		    status = 0;
		    
		    //3. CHECK THAT THE IP ADDRESS ISNT ALREADY IN THE TABLE
		    for(counter=0; counter<RhinoClusterCounter;counter++)
		    {
		      if(strncmp(rhinoStatTable[counter].rhinoIpAdd, rhinoIpString, sizeof(rhinoStatTable[counter].rhinoIpAdd)) == 0)
		      {
			
			  printf("RHINO WITH IP [%s] ADDRESS ALREADY IN CLUSTER\n",rhinoStatTable[counter].rhinoIpAdd);
			  //2. CHECK IF THE RHINO IS ACTIVE
			  if(rhinoStatTable[counter].activeBit==BOARD_ACTIVE)
			  {
			    printf("BOARD [%s] IS ALREADY ACTIVE ON CLUSTER\n",rhinoStatTable[counter].rhinoIpAdd);
			    
			    status = 1;
			    
			    break;
			    
			  }
			  else if(rhinoStatTable[counter].activeBit==BOARD_INACTIVE)// MAYBE BOARD IS JUST BEING USED BUT IT CONNECTED ON THE CLUSTER NEED TO CHECK THAT
			  {
			      printf("BOARD IS INACTIVE AND ATTEMPTING TO RECONNECT\n");
			      rhinoStatTable[counter].portno = atoi(portNoString);
			      rhinoStatTable[counter].sockfd = socket (AF_INET, SOCK_STREAM, 0);
			      
			      if(rhinoStatTable[counter].sockfd <0)
			      {
				  error("Error openning socket"); //be not so dramatic when shutting down when not openning socket of one rhino
			      }
			      
			      //ERROR HANDLING IS REQUIRED HERE CAUSE THIS THROWS AN EXCEPTION
						  
			      bzero((char *) &serv_addr, sizeof(serv_addr));//sets all values in the buffer to zero, takes two arguments a pointer to the buffer and the size of the buffer  
			      serv_addr.sin_family = AF_INET;//AF_INET allows for an IPv4 internet protocol
			      serv_addr.sin_addr.s_addr = inet_addr(rhinoStatTable[counter].rhinoIpAdd);
						  
						  
			      serv_addr.sin_port = htons(rhinoStatTable[counter].portno);
				//ATTEMPTING TO CONNECT TO THE RHINO GIVEN THE IP ADDRESS AND UPDATING THE RHINOS ACTIVE BIT IN THE RHINOSTAT TABLE
			      if(connect(rhinoStatTable[counter].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //this function returns 0 on success and -1 if its fails
			      {
			    
				  //ERROR Handling
				  switch(errno){
				    case EACCES:
					    printf("Write permission is denied on the socket file, or search permission is denied for one of the directories in the path prefix");
					    //return FAILURE;
					    break;
				    case EFAULT:
					    printf("The socket structure address is outside the user's address space: %d\n",rhinoStatTable[counter].sockfd);
					    // return FAILURE;
					    break;
				    case EADDRINUSE:
					    printf("Local address is already in use: %d\n",rhinoStatTable[counter].sockfd);
					    //return FAILURE;
					    break;
				    case EAFNOSUPPORT:
					    printf("The passed address didn't have the correct address family in its sa_family field\n");
					    //return FAILURE;
					    break;
				    case EAGAIN:
					    printf("No more free local ports or insufficient entries in the routing cache\n");
					    // return FAILURE;
					    break;
				    case EALREADY:
					    printf("The socket is nonblocking and a previous connection attempt has not yet been completed.\n");
					    // return FAILURE;
					    break;
				    case ENOTSOCK:
					    printf("The file descriptor is not associated with a socket\n");
							//return FAILURE;
				    case ENETUNREACH:
					    printf("Network is unreachable\n");
					    //return FAILURE;
					    break;
				    case EISCONN:
					    printf("The socket is already connected\n");
					    //return FAILURE;
					    break;
				    case EBADF:
					    printf("The file descriptor is not a valid index in the descriptor table\n");
					    // return FAILURE;
					    break;
				    case ECONNREFUSED:
					    printf("No-one listening on the remote address: %s\n", rhinoStatTable[counter].rhinoIpAdd);
					    //return FAILURE;
					    break;
				    case EINTR:
					    printf("The system call was interrupted by a signal that was caught\n");
					    //return FAILURE;
					    break;
				    default:
					    printf("UNKNOWN CONNECTION ERROR\n");
							//return FAILURE;
						      
				  }
					
				  	
				  rhinoStatTable[counter].activeBit = BOARD_INACTIVE;
				  printf("Error connecting\n"); //THINK OF ANOTHER WAY TO TRY RECONNECT TO THE BOARD ONE MORE TIME
				  //printf("THE RHINO ACTIVEBIT IS [%d]\n",rhinoStatTable[counter].activeBit);
				  printf("THE RHINO [%s] IS ON THE CLUSTER BUT NOT ACTIVE\n",rhinoStatTable[counter].rhinoIpAdd);
				  
				  status = 1;
				  
				  break;
			      }
			      else
			      {
				  connect(rhinoStatTable[counter].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
				  rhinoStatTable[counter].activeBit = BOARD_ACTIVE;
				  printf("THE RHINO [%s] IS ON THE CLUSTER AND IS ACTIVE\n",rhinoStatTable[counter].rhinoIpAdd);
						      
				  /*******************************************************************************************************************************************************
				  ADD INFORMATION ABOUT THE RHINO HERE ASSOCIATED WITH THAT PARTICULAR POSITION ON THE RHINO STATUS TABLE FROM THE RHINO ITSELF USING THE READ AND WRITE
				  *******************************************************************************************************************************************************/ 
				  //rhinoStatTable[counter].rhinoBoard.nameNumber = counter;
				  //rhinoStatTable[counter].rhinoBoard.ipAddress=rhinoIpString;
				  //rhinoStatTable[counter].rhinoBoard.busyBit = UNPROGRAMMED;
				  //rhinoStatTable[counter].rhinoBoard.registersAccessible = NULL;
				  
				  status = 1;
							
				  break;
				}
			      
			      
			  }
			  else
			  {
			    printf("STATUS OF THE RHINO IS UNKNOWN\n");
			    break;
			  }
			
		      }
		      
		    }
	  
		  if(status == 0)
		  {
		    printf("RHINO [%s] NOT INITIALLY IN THE RHINO STATUS TABLE CREATING NEW ELEMENT \n",rhinoIpString);
		  
		    //Increasing the size of the RHINO status table to accomodate one more RHINO
		    
		    tempRhinoStatusTable = (RhStatTableT *) realloc(rhinoStatTable,((RhinoClusterCounter+1)*sizeof(RhStatTableT)));
			      
		    rhinoStatTable = tempRhinoStatusTable;
			  
		    //ADDING RHINO TO THE STATUS TABLE
		    rhinoStatTable[RhinoClusterCounter].rhinoNum = RhinoClusterCounter;
		    printf("NEW RHINO NUMBER: [%d]\n",rhinoStatTable[RhinoClusterCounter].rhinoNum);
		    
		    rhinoStatTable[RhinoClusterCounter].programUserId = -1;
		    printf("NEW RHINO PROGRAM USER ID: [%d]\n", rhinoStatTable[RhinoClusterCounter].programUserId); 
		    
		    strncpy(rhinoStatTable[RhinoClusterCounter].rhinoIpAdd, rhinoIpString, sizeof(rhinoStatTable[RhinoClusterCounter].rhinoIpAdd)-1);
		    
		    rhinoStatTable[RhinoClusterCounter].rhinoIpAdd[16] = '\0';
		    printf("NEW RHINO IPADDRESS: [%s]\n", rhinoStatTable[RhinoClusterCounter].rhinoIpAdd);
		    
		    rhinoStatTable[RhinoClusterCounter].portno = atoi(portNoString);
		    printf("NEW RHINO PORT#: [%d]\n",rhinoStatTable[RhinoClusterCounter].portno);
		    
		    rhinoStatTable[RhinoClusterCounter].sockfd = socket (AF_INET, SOCK_STREAM, 0);
			      
		    if(rhinoStatTable[RhinoClusterCounter].sockfd <0)
		    {
			error("Error openning socket"); //be not so dramatic when shutting down when not openning socket of one rhino
		    }
			      
			      //ERROR HANDLING IS REQUIRED HERE CAUSE THIS THROWS AN EXCEPTION
						  
		    bzero((char *) &serv_addr, sizeof(serv_addr));//sets all values in the buffer to zero, takes two arguments a pointer to the buffer and the size of the buffer  
		    serv_addr.sin_family = AF_INET;//AF_INET allows for an IPv4 internet protocol
		    serv_addr.sin_addr.s_addr = inet_addr(rhinoStatTable[RhinoClusterCounter].rhinoIpAdd);
						  
						  
		    serv_addr.sin_port = htons(rhinoStatTable[RhinoClusterCounter].portno);
		  //ATTEMPTING TO CONNECT TO THE RHINO GIVEN THE IP ADDRESS AND UPDATING THE RHINOS ACTIVE BIT IN THE RHINOSTAT TABLE
		    if(connect(rhinoStatTable[RhinoClusterCounter].sockfd ,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //this function returns 0 on success and -1 if its fails
		    {
			    
		      //ERROR Handling
		      switch(errno){
				    case EACCES:
					    printf("Write permission is denied on the socket file, or search permission is denied for one of the directories in the path prefix");
					    //return FAILURE;
					    break;
				    case EFAULT:
					    printf("The socket structure address is outside the user's address space: %d\n",rhinoStatTable[counter].sockfd);
					    // return FAILURE;
					    break;
				    case EADDRINUSE:
					    printf("Local address is already in use: %d\n",rhinoStatTable[counter].sockfd);
					    //return FAILURE;
					    break;
				    case EAFNOSUPPORT:
					    printf("The passed address didn't have the correct address family in its sa_family field\n");
					    //return FAILURE;
					    break;
				    case EAGAIN:
					    printf("No more free local ports or insufficient entries in the routing cache\n");
					    // return FAILURE;
					    break;
				    case EALREADY:
					    printf("The socket is nonblocking and a previous connection attempt has not yet been completed.\n");
					    // return FAILURE;
					    break;
				    case ENOTSOCK:
					    printf("The file descriptor is not associated with a socket\n");
							//return FAILURE;
				    case ENETUNREACH:
					    printf("Network is unreachable\n");
					    //return FAILURE;
					    break;
				    case EISCONN:
					    printf("The socket is already connected\n");
					    //return FAILURE;
					    break;
				    case EBADF:
					    printf("The file descriptor is not a valid index in the descriptor table\n");
					    // return FAILURE;
					    break;
				    case ECONNREFUSED:
					    printf("No-one listening on the remote address: %s\n", rhinoStatTable[counter].rhinoIpAdd);
					    //return FAILURE;
					    break;
				    case EINTR:
					    printf("The system call was interrupted by a signal that was caught\n");
					    //return FAILURE;
					    break;
				    default:
					    printf("UNKNOWN CONNECTION ERROR\n");
							//return FAILURE;
						      
				  }
						    
		      rhinoStatTable[RhinoClusterCounter].activeBit = BOARD_INACTIVE;
		      printf("Error connecting\n"); //THINK OF ANOTHER WAY TO TRY RECONNECT TO THE BOARD ONE MORE TIME
		      //printf("THE RHINO ACTIVEBIT IS [%d]\n",rhinoStatTable[counter].activeBit);
		      
		      //INITIALIZING THE RHINOBOARD
		      initRhinoBoard(rhinoStatTable[RhinoClusterCounter].rhinoNum, rhinoStatTable[RhinoClusterCounter].rhinoIpAdd);
		      
		      printf("THE RHINO [%s] IS ON THE CLUSTER BUT NOT ACTIVE\n",rhinoStatTable[counter].rhinoIpAdd);
		      
		      RhinoClusterCounter = RhinoClusterCounter+1;
		      break;
		    }
		    else
		    {
		      connect(rhinoStatTable[RhinoClusterCounter].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
		      rhinoStatTable[counter].activeBit = BOARD_ACTIVE;
		      
		      //INITIALIZING THE RHINO BOARD
		      initRhinoBoard(rhinoStatTable[RhinoClusterCounter].rhinoNum, rhinoStatTable[RhinoClusterCounter].rhinoIpAdd);
		      
		      printf("THE RHINO [%s] IS ON THE CLUSTER AND IS ACTIVE\n",rhinoStatTable[counter].rhinoIpAdd);
						      
		      /*******************************************************************************************************************************************************
		      ADD INFORMATION ABOUT THE RHINO HERE ASSOCIATED WITH THAT PARTICULAR POSITION ON THE RHINO STATUS TABLE FROM THE RHINO ITSELF USING THE READ AND WRITE
		      *******************************************************************************************************************************************************/ 
		      //rhinoStatTable[counter].rhinoBoard.nameNumber = counter;
		      //rhinoStatTable[counter].rhinoBoard.ipAddress=rhinoIpString;
		      //rhinoStatTable[counter].rhinoBoard.busyBit = UNPROGRAMMED;
		      //rhinoStatTable[counter].rhinoBoard.registersAccessible = NULL;
		      RhinoClusterCounter = RhinoClusterCounter+1;
		      
		      break;
		    }
		  }
			
			
	      }
	    
	    
	  }
	  
	  //CASE 2.....NO INITIAL RHINO STATUS TABLE
	  else if(rhinoStatTable == NULL)
	  {
	      printf("RHINOSTATUSTABLE DOESNT ALREADY EXIST: CASE 2\n");
	      
	      //FIRST LINE IN THE FILEIPADD IS THE NUMBER OF RHINOS THAT ARE IN THE CLUSTER
	      if (fgets(numberOfClusterRhinoChar,256,fileIpAdd2)!= NULL)
	      {
				  
		numberOfClusterRhinos = atoi(numberOfClusterRhinoChar);
		//printf("NUMBER OF RHINOS ON THE CLUSTER AVAILABLE OR NOT ARE:[%d]\n",numberOfClusterRhinos);
	      }
	      else
	      {
		  error("File Refused to Open");
	      }

	      n= numberOfClusterRhinos;
	      size = sizeof(RhStatTableT);
	      length = size * n;
	      //NOW ACTUALLY ALLOCATING THE MEMORY FOR THE AVAILABLE RHINOS
	      rhinoStatTable = (RhStatTableT*)malloc( length);
	    
	      if (!rhinoStatTable){
		error("malloc");
	      }
			    
			    
	      //printf("THE SIZE OF RHINOSTATTABLE IS:[%d]\n",RhinoClusterCounter);
	      //FILLING THE RHINOSTAT TABLE WITH INFORMATION FROM THE RHINOS AND CONNECTING THEM TO THE CLIENT
	      counter =0;
			      
	      while(fgets(rhinoInfoChar,256,fileIpAdd2))
	      {
		  //printf("IN THE WHILE LOOP FOR READING THE\n");
		  //Making sure that counter remains in the bounds of the rhinoStatTable
		  if((counter <= RhinoClusterCounter)||(RhinoClusterCounter==0))
		  {
					
			rhinoStatTable[counter].rhinoNum = counter;
			RhinoClusterCounter++;
			// printf("THE RHINO IPAddress AND SOCKET NUMBER FROM FILEIP IS: %s\n",rhinoInfoChar);
					
			//THINK OF WAY OF SPLITTING THE RHINOIPADDRESS AND THE PORTNO TO BE USED ONCE READ FROM THE LINE
			rhinoIpString= strtok(rhinoInfoChar," ");
			//printf("THE IP ADDRESS OF THE RHINO FROM RHINOSTATTABLE:[%s]\n",rhinoIpString);
					
			strncpy(rhinoStatTable[counter].rhinoIpAdd, rhinoIpString,sizeof(rhinoStatTable[counter].rhinoIpAdd)-1);
			rhinoStatTable[counter].rhinoIpAdd[16] = '\0';
					
			portNoString=strtok(NULL," \n");
			rhinoStatTable[counter].portno = atoi(portNoString); //getting socket from the console
			// printf("THE PORTNO THAT IS NOW AN INTERGER:[%d]\n",rhinoStatTable[counter].portno); 
					
			rhinoStatTable[counter].sockfd = socket (AF_INET, SOCK_STREAM, 0);
		  
					
			//printf("THE SOCKFD OF THE RHINO IS:[%d]\n",rhinoStatTable[counter].sockfd);
			// printf("THE SIZE OF RHINOSTATTABLE AFTER EXECUTING LOOP IS:[%d]\n",RhinoClusterCounter);
					
			if(rhinoStatTable[counter].sockfd <0)
			{
			    error("Error openning socket"); //be not so dramatic when shutting down when not openning socket of one rhino
			}
					  
			// printf("GETTING HERE AND RHINOSTATIPADDRESS IS: %s\n",rhinoStatTable[counter].rhinoIpAdd);
			//GETHOSTBYNAME USES A STRING OF AN IPADDRESS 
					  
					  
			//ERROR HANDLING IS REQUIRED HERE CAUSE THIS THROWS AN EXCEPTION
					    
			bzero((char *) &serv_addr, sizeof(serv_addr));//sets all values in the buffer to zero, takes two arguments a pointer to the buffer and the size of the buffer  
			serv_addr.sin_family = AF_INET;//AF_INET allows for an IPv4 internet protocol
			serv_addr.sin_addr.s_addr = inet_addr(rhinoStatTable[counter].rhinoIpAdd);
					    
					    
			serv_addr.sin_port = htons(rhinoStatTable[counter].portno);
					    
			//ATTEMPTING TO CONNECT TO THE RHINO GIVEN THE IP ADDRESS AND UPDATING THE RHINOS ACTIVE BIT IN THE RHINOSTAT TABLE
			if(connect(rhinoStatTable[counter].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //this function returns 0 on success and -1 if its fails
			{
					      
					      
			    //ERROR Handling
			    switch(errno){
			      case EACCES:
				      printf("Write permission is denied on the socket file, or search permission is denied for one of the directories in the path prefix");
				      //return FAILURE;
				      break;
			      case EFAULT:
				      printf("The socket structure address is outside the user's address space: %d\n",rhinoStatTable[counter].sockfd);
				      // return FAILURE;
				      break;
			      case EADDRINUSE:
				      printf("Local address is already in use: %d\n",rhinoStatTable[counter].sockfd);
				      //return FAILURE;
				      break;
			      case EAFNOSUPPORT:
				      printf("The passed address didn't have the correct address family in its sa_family field\n");
				      //return FAILURE;
				      break;
			      case EAGAIN:
				      printf("No more free local ports or insufficient entries in the routing cache\n");
				      // return FAILURE;
				      break;
			      case EALREADY:
				      printf("The socket is nonblocking and a previous connection attempt has not yet been completed.\n");
				      // return FAILURE;
				      break;
			      case ENOTSOCK:
				      printf("The file descriptor is not associated with a socket\n");
						  //return FAILURE;
			      case ENETUNREACH:
				      printf("Network is unreachable\n");
				      //return FAILURE;
				      break;
			      case EISCONN:
				      printf("The socket is already connected\n");
				      //return FAILURE;
				      break;
			      case EBADF:
				      printf("The file descriptor is not a valid index in the descriptor table\n");
				      // return FAILURE;
				      break;
			      case ECONNREFUSED:
				      printf("No-one listening on the remote address: %s\n", rhinoStatTable[counter].rhinoIpAdd);
				      //return FAILURE;
				      break;
			      case EINTR:
				      printf("The system call was interrupted by a signal that was caught\n");
				      //return FAILURE;
				      break;
			      default:
				      printf("UNKNOWN CONNECTION ERROR\n");
						  //return FAILURE;
				      break;
						
			    }
					      
			    rhinoStatTable[counter].activeBit = BOARD_INACTIVE;
			    printf("Error connecting\n"); //THINK OF ANOTHER WAY TO TRY RECONNECT TO THE BOARD ONE MORE TIME
			    //printf("THE RHINO ACTIVEBIT IS [%d]\n",rhinoStatTable[counter].activeBit);
			    
			    //INITIALIZE THE RHINO
			     initRhinoBoard(rhinoStatTable[counter].rhinoNum, rhinoStatTable[counter].rhinoIpAdd);
			    
			    printf("THE RHINO [%s] IS ON THE CLUSTER BUT NOT ACTIVE\n",rhinoStatTable[counter].rhinoIpAdd);
			    
			    counter++;
			}
			else
			{
			    connect(rhinoStatTable[counter].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
			    rhinoStatTable[counter].activeBit = BOARD_ACTIVE;
			    
			    //INITIALIZE THE RHINO
			    initRhinoBoard(rhinoStatTable[counter].rhinoNum, rhinoStatTable[counter].rhinoIpAdd);
			    
			    printf("THE RHINO [%s] IS ON THE CLUSTER AND IS ACTIVE\n",rhinoStatTable[counter].rhinoIpAdd);
						
			    /*******************************************************************************************************************************************************
			    ADD INFORMATION ABOUT THE RHINO HERE ASSOCIATED WITH THAT PARTICULAR POSITION ON THE RHINO STATUS TABLE FROM THE RHINO ITSELF USING THE READ AND WRITE
			    *******************************************************************************************************************************************************/ 
			    //rhinoStatTable[counter].rhinoBoard.nameNumber = counter;
			    //rhinoStatTable[counter].rhinoBoard.ipAddress=rhinoIpString;
			    //rhinoStatTable[counter].rhinoBoard.busyBit = UNPROGRAMMED;
			    //rhinoStatTable[counter].rhinoBoard.registersAccessible = NULL;
						  
			    counter++;
			  }
		    }
				    
		    else
		    {
			printf("COUNT IS OUT OF BOUNDS WITH RHINOSTATTABLE\n ");
			return FAILURE;
		    }
		 }
		
		
	  }
	  
	  else //RHINOSTATUSTABLE CHECK
	  {
	    printf("RHINOSTATUSTABLE STATUS UNKNOWN\n");
	    return FAILURE;
	  }
  
  return SUCCESS;
}

/************************************************************************************************************************************************************************************************
* 7) THIS METHOD WILL ALLOW A USER TO CONNECT TO A PARTICULAR RHINO GIVEN AN IP ADDRESS. NB THE RHINO NEEDS TO ALREADY BE ON THE REGISTERED ON THE CLUSTER...
************************************************************************************************************************************************************************************************/
int connectToSpecificRhino(char *ipAddress,char *portno)
{
	int counter =0;
	RhStatTableT *tempRhinoStatusTable = NULL;
	
	
	//NEED TO FIRST BE ABLE TO CHECK THAT THE RHINO NUMBER IS ACTUALLY STILL IN THE BOUNDS OF THE STATEMENT
	 printf("IN ConnectToSpecificRhino DETAILS ARE: IPADD: [%s], PORTNO:[%s]\n",ipAddress,portno);
	 
	//1. FIRST CHECK THAT RHINO IS NOT ALREADY ON THE CLUSTER
	  for(counter=0 ;counter < RhinoClusterCounter; counter++)
	  {
	    if(strncmp(rhinoStatTable[counter].rhinoIpAdd, ipAddress, sizeof(rhinoStatTable[counter].rhinoIpAdd)) ==0)
	    {
	      
		printf("RHINO WITH IP ADDRESS ALREADY IN CLUSTER\n");
		//2. CHECK IF THE RHINO IS ACTIVE
		 if(rhinoStatTable[counter].activeBit==BOARD_ACTIVE)
		 {
		   printf("BOARD IS ALREADY ACTIVE ON CLUSTER\n");
		   return FAILURE;
		   
		 }
		 else if(rhinoStatTable[counter].activeBit == BOARD_INACTIVE)// MAYBE BOARD IS JUST BEING USED BUT IT CONNECTED ON THE CLUSTER NEED TO CHECK THAT
		 {
		    printf("BOARD IS INACTIVE AND ATTEMPTING TO RECONNECT\n");
		    rhinoStatTable[counter].portno = atoi(portno);
		    rhinoStatTable[counter].sockfd = socket (AF_INET, SOCK_STREAM, 0);
		    
		    if(rhinoStatTable[counter].sockfd <0)
		    {
			error("Error openning socket"); //be not so dramatic when shutting down when not openning socket of one rhino
		    }
		    
		    //ERROR HANDLING IS REQUIRED HERE CAUSE THIS THROWS AN EXCEPTION
					
		    bzero((char *) &serv_addr, sizeof(serv_addr));//sets all values in the buffer to zero, takes two arguments a pointer to the buffer and the size of the buffer  
		    serv_addr.sin_family = AF_INET;//AF_INET allows for an IPv4 internet protocol
		    serv_addr.sin_addr.s_addr = inet_addr(rhinoStatTable[counter].rhinoIpAdd);
					
					
		    serv_addr.sin_port = htons(rhinoStatTable[counter].portno);
		      //ATTEMPTING TO CONNECT TO THE RHINO GIVEN THE IP ADDRESS AND UPDATING THE RHINOS ACTIVE BIT IN THE RHINOSTAT TABLE
		    if(connect(rhinoStatTable[counter].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //this function returns 0 on success and -1 if its fails
		    {
		  
			//ERROR Handling
			switch(errno){
			  case EACCES:
				  printf("Write permission is denied on the socket file, or search permission is denied for one of the directories in the path prefix");
				  //return FAILURE;
				  break;
			  case EFAULT:
				  printf("The socket structure address is outside the user's address space: %d\n",rhinoStatTable[counter].sockfd);
				  // return FAILURE;
				  break;
			  case EADDRINUSE:
				  printf("Local address is already in use: %d\n",rhinoStatTable[counter].sockfd);
				  //return FAILURE;
				  break;
			  case EAFNOSUPPORT:
				  printf("The passed address didn't have the correct address family in its sa_family field\n");
				  //return FAILURE;
				  break;
			  case EAGAIN:
				  printf("No more free local ports or insufficient entries in the routing cache\n");
				  // return FAILURE;
				  break;
			  case EALREADY:
				  printf("The socket is nonblocking and a previous connection attempt has not yet been completed.\n");
				  // return FAILURE;
				  break;
			  case ENOTSOCK:
				  printf("The file descriptor is not associated with a socket\n");
					      //return FAILURE;
			  case ENETUNREACH:
				  printf("Network is unreachable\n");
				  //return FAILURE;
				  break;
			  case EISCONN:
				  printf("The socket is already connected\n");
				  //return FAILURE;
				  break;
			  case EBADF:
				  printf("The file descriptor is not a valid index in the descriptor table\n");
				  // return FAILURE;
				  break;
			  case ECONNREFUSED:
				  printf("No-one listening on the remote address: %s\n", rhinoStatTable[counter].rhinoIpAdd);
				  //return FAILURE;
				  break;
			  case EINTR:
				  printf("The system call was interrupted by a signal that was caught\n");
				  //return FAILURE;
				  break;
			  default:
				  printf("UNKNOWN CONNECTION ERROR\n");
					      //return FAILURE;
					    
			}
					   
			rhinoStatTable[counter].activeBit = rhinoStatTable[counter].activeBit;
			printf("Error connecting\n"); //THINK OF ANOTHER WAY TO TRY RECONNECT TO THE BOARD ONE MORE TIME
			//printf("THE RHINO ACTIVEBIT IS [%d]\n",rhinoStatTable[counter].activeBit);
			printf("THE RHINO [%s] IS ON THE CLUSTER BUT NOT ACTIVE\n",rhinoStatTable[counter].rhinoIpAdd);
			
			return FAILURE;
		    }
		    else
		    {
			connect(rhinoStatTable[counter].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
			rhinoStatTable[counter].activeBit = BOARD_ACTIVE;
			printf("THE RHINO [%s] IS ON THE CLUSTER AND IS ACTIVE\n",rhinoStatTable[counter].rhinoIpAdd);
					    
			/*******************************************************************************************************************************************************
			ADD INFORMATION ABOUT THE RHINO HERE ASSOCIATED WITH THAT PARTICULAR POSITION ON THE RHINO STATUS TABLE FROM THE RHINO ITSELF USING THE READ AND WRITE
			*******************************************************************************************************************************************************/ 
			//rhinoStatTable[counter].rhinoBoard.nameNumber = counter;
			//rhinoStatTable[counter].rhinoBoard.ipAddress=rhinoIpString;
			//rhinoStatTable[counter].rhinoBoard.busyBit = UNPROGRAMMED;
			//rhinoStatTable[counter].rhinoBoard.registersAccessible = NULL;
					      
			return SUCCESS;
		      }
		    
		    
		}
		 else
		 {
		   printf("STATUS OF THE RHINO IS UNKNOWN\n");
		   return FAILURE;
		 }
	      
	    }
	    
	  }
	  
	  
	  printf("RHINO NOT INITIALLY IN THE RHINO STATUS TABLE CREATING NEW ELEMENT \n");
	
	  //Increasing the size of the RHINO status table to accomodate one more RHINO
	  
	  tempRhinoStatusTable = (RhStatTableT *) realloc(rhinoStatTable,((RhinoClusterCounter+1)*sizeof(RhStatTableT)));
		    
	  rhinoStatTable = tempRhinoStatusTable;
		
	  //ADDING RHINO TO THE STATUS TABLE
	  rhinoStatTable[RhinoClusterCounter].rhinoNum = RhinoClusterCounter;
	  printf("NEW RHINO NUMBER: [%d]\n",rhinoStatTable[RhinoClusterCounter].rhinoNum);
	  
	  rhinoStatTable[RhinoClusterCounter].programUserId = -1;
	  printf("NEW RHINO PROGRAM USER ID: [%d]\n", rhinoStatTable[RhinoClusterCounter].programUserId); 
	  
	  strncpy(rhinoStatTable[RhinoClusterCounter].rhinoIpAdd, ipAddress, sizeof(rhinoStatTable[RhinoClusterCounter].rhinoIpAdd)-1);
	  
	  rhinoStatTable[RhinoClusterCounter].rhinoIpAdd[16] = '\0';
	  printf("NEW RHINO IPADDRESS: [%s]\n", rhinoStatTable[RhinoClusterCounter].rhinoIpAdd);
	  
	  rhinoStatTable[RhinoClusterCounter].portno = atoi(portno);
	  printf("NEW RHINO PORT#: [%d]\n",rhinoStatTable[RhinoClusterCounter].portno);
	  
	  rhinoStatTable[RhinoClusterCounter].sockfd = socket (AF_INET, SOCK_STREAM, 0);
		    
	  if(rhinoStatTable[RhinoClusterCounter].sockfd <0)
	  {
	      error("Error openning socket"); //be not so dramatic when shutting down when not openning socket of one rhino
	  }
		    
		    //ERROR HANDLING IS REQUIRED HERE CAUSE THIS THROWS AN EXCEPTION
					
	  bzero((char *) &serv_addr, sizeof(serv_addr));//sets all values in the buffer to zero, takes two arguments a pointer to the buffer and the size of the buffer  
	  serv_addr.sin_family = AF_INET;//AF_INET allows for an IPv4 internet protocol
	  serv_addr.sin_addr.s_addr = inet_addr(rhinoStatTable[RhinoClusterCounter].rhinoIpAdd);
					
					
	  serv_addr.sin_port = htons(rhinoStatTable[RhinoClusterCounter].portno);
	//ATTEMPTING TO CONNECT TO THE RHINO GIVEN THE IP ADDRESS AND UPDATING THE RHINOS ACTIVE BIT IN THE RHINOSTAT TABLE
	  if(connect(rhinoStatTable[RhinoClusterCounter].sockfd ,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //this function returns 0 on success and -1 if its fails
	  {
		  
	    //ERROR Handling
	    switch(errno){
			  case EACCES:
				  printf("Write permission is denied on the socket file, or search permission is denied for one of the directories in the path prefix");
				  //return FAILURE;
				  break;
			  case EFAULT:
				  printf("The socket structure address is outside the user's address space: %d\n",rhinoStatTable[counter].sockfd);
				  // return FAILURE;
				  break;
			  case EADDRINUSE:
				  printf("Local address is already in use: %d\n",rhinoStatTable[counter].sockfd);
				  //return FAILURE;
				  break;
			  case EAFNOSUPPORT:
				  printf("The passed address didn't have the correct address family in its sa_family field\n");
				  //return FAILURE;
				  break;
			  case EAGAIN:
				  printf("No more free local ports or insufficient entries in the routing cache\n");
				  // return FAILURE;
				  break;
			  case EALREADY:
				  printf("The socket is nonblocking and a previous connection attempt has not yet been completed.\n");
				  // return FAILURE;
				  break;
			  case ENOTSOCK:
				  printf("The file descriptor is not associated with a socket\n");
					      //return FAILURE;
			  case ENETUNREACH:
				  printf("Network is unreachable\n");
				  //return FAILURE;
				  break;
			  case EISCONN:
				  printf("The socket is already connected\n");
				  //return FAILURE;
				  break;
			  case EBADF:
				  printf("The file descriptor is not a valid index in the descriptor table\n");
				  // return FAILURE;
				  break;
			  case ECONNREFUSED:
				  printf("No-one listening on the remote address: %s\n", rhinoStatTable[counter].rhinoIpAdd);
				  //return FAILURE;
				  break;
			  case EINTR:
				  printf("The system call was interrupted by a signal that was caught\n");
				  //return FAILURE;
				  break;
			  default:
				  printf("UNKNOWN CONNECTION ERROR\n");
					      //return FAILURE;
					    
			}
					   
	    rhinoStatTable[RhinoClusterCounter].activeBit = BOARD_INACTIVE;
	    printf("Error connecting\n"); //THINK OF ANOTHER WAY TO TRY RECONNECT TO THE BOARD ONE MORE TIME
	    //printf("THE RHINO ACTIVEBIT IS [%d]\n",rhinoStatTable[counter].activeBit);
	    printf("THE RHINO [%s] IS ON THE CLUSTER BUT NOT ACTIVE\n",rhinoStatTable[counter].rhinoIpAdd);
	    
	    RhinoClusterCounter = RhinoClusterCounter+1;
	    
	    return FAILURE;
	  }
	  else
	  {
	    connect(rhinoStatTable[RhinoClusterCounter].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
	    rhinoStatTable[counter].activeBit = BOARD_ACTIVE;
	    printf("THE RHINO [%s] IS ON THE CLUSTER AND IS ACTIVE\n",rhinoStatTable[counter].rhinoIpAdd);
					    
	    /*******************************************************************************************************************************************************
	    ADD INFORMATION ABOUT THE RHINO HERE ASSOCIATED WITH THAT PARTICULAR POSITION ON THE RHINO STATUS TABLE FROM THE RHINO ITSELF USING THE READ AND WRITE
	    *******************************************************************************************************************************************************/ 
	    //rhinoStatTable[counter].rhinoBoard.nameNumber = counter;
	    //rhinoStatTable[counter].rhinoBoard.ipAddress=rhinoIpString;
	    //rhinoStatTable[counter].rhinoBoard.busyBit = UNPROGRAMMED;
	    //rhinoStatTable[counter].rhinoBoard.registersAccessible = NULL;
	    RhinoClusterCounter = RhinoClusterCounter+1;
	    
	    return SUCCESS;
	  }
	      
}



/************************************************************************************************************************************************************************************************
*8) THIS METHOD ALLOWS A USER TO SELECT AND "LOCK" THE RHINO during duration of the program running
****************************************************************************************************************************************************************************************************/

int rhinoSelect (uint32_t rhinoNumber,uint32_t programUserId) //MIGHT LOOK INTO CHANGING THIS TO TAKE A NUMBER RATHER THAN THE IPADDRESS
{
	      int counter;
	      
	     // printf("THE RHINONUMBER IS TO SELECT IS: [%d]\n",rhinoNumber);
	     // printf("THE PROGRAMUSERID IS TO SELECT IS: [%d]\n",programUserId);
	     // printf("THE RHINOCLUSTERCOUNTER IS: [%d]\n",RhinoClusterCounter);
	      
	      
	      for(counter=0; counter<RhinoClusterCounter;counter++)
	      {
		//printf("THE RHINOSTATTABLE RHINO NUMBER IS: %d\n",rhinoStatTable[counter].rhinoNum);
		  if(rhinoNumber == rhinoStatTable[counter].rhinoNum)
		  {
		    printf("RHINO %d IS AVAILABLE ON THE CLUSTER\n",rhinoStatTable[counter].rhinoNum);
		    // printf("THE RHINO ACTIVE BIT IS: %d\n",rhinoStatTable[counter].activeBit); 
		    
		    
		      //First Check if the RHINO is active on the clusterStatus
		    if(rhinoStatTable[counter].activeBit == BOARD_INACTIVE)
		    {
			//Lock the RHINO from use from other users
			printf("RHINO %d IS AVAILABLE ON THE CLUSTER BUT IS BEING USED, TRY ANOTHER RHINO\n",rhinoStatTable[counter].rhinoNum);
			return FAILURE;
		    }
		    else if(rhinoStatTable[counter].activeBit == BOARD_ACTIVE)
		    {
			rhinoStatTable[counter].programUserId = programUserId;
			rhinoStatTable[counter].activeBit = BOARD_INACTIVE;
			//printf("THE PROGRAMUSERID FOR THE RHINO IS: [%d]\n",rhinoStatTable[counter].programUserId);
		      
			return SUCCESS;
		    }
		 }
		  //printf("GETTING INTO FOR LOOP\n");
	      }
		
	    printf("RHINO [%d] IS NOT AVAILABLE ON THE CLUSTER, TRY ANOTHER RHINO\n",rhinoNumber);
	    return FAILURE;
	      
		
}
	  
	  
/*************************************************************************************************************************************************************************************************
*9) RELEASES A RHINO AFTER A USER IS DONE USING IT (WORKS FINE)
****************************************************************************************************************************************************************************************************/
int rhinoRelease(uint32_t rhinoNumber,uint32_t programUserId)
{
	      int counter;
	      
	      
	      //First Make sure that RHINO being acccesed is on the cluster.
	      for(counter=0; counter<RhinoClusterCounter;counter++)
	      {
		  if(rhinoNumber == rhinoStatTable[counter].rhinoNum)
		  {
		    printf("RHINO [%d] IS AVAILABLE ON THE CLUSTER\n",rhinoStatTable[counter].rhinoNum);
		    
		    //CHECKING THAT THE USER HAS ACCESS TO THAT PARTICULAR RHINO
		    if(rhinoStatTable[counter].programUserId == programUserId)
		    {
		      //First Check if the RHINO is active on the clusterStatus
			if(rhinoStatTable[counter].activeBit==BOARD_INACTIVE)
			{
			  //RELEASING THE RHINO FOR USE BACK ON THE CLUSTER
			  printf("RHINO %d IS AVAILABLE ON THE CLUSTER AND IS BEING RELEASED\n",rhinoStatTable[counter].rhinoNum);
			  rhinoStatTable[counter].activeBit = BOARD_ACTIVE;
			  rhinoStatTable[counter].programUserId = -1;
			  printf("THE NEW ACTIVE BIT FOR FOR THE RHINO IS: [%d]\n",rhinoStatTable[counter].activeBit);
			  return SUCCESS;
			}
			
		    }
		    
		    else if(rhinoStatTable[counter].programUserId == programUserId)
		    {
		      printf("THIS USER DOES NOT ACCESS TO RHINO [%d] AT THIS POINT\n",rhinoStatTable[counter].rhinoNum);
		      return FAILURE;
		    }
		    
		    else if(rhinoStatTable[counter].activeBit==BOARD_ACTIVE)
		    {
			//HOW DO YOU KNOW WHICH USER HAS SELECTED A PARTICULAR RHINO????
			printf("RHINO [%d] IS AVAILABLE ON THE CLUSTER IS ALREADY ACTIVE\n",rhinoStatTable[counter].rhinoNum);
			return FAILURE;
		      }
		  }
		}
		
	      
		    printf("RHINO [%d] IS NOT AVAILABLE ON THE CLUSTER, TRY ANOTHER RHINO\n",rhinoStatTable[counter].rhinoNum);
		    return FAILURE;
		
	  }
	  
/******************************************************************************************************************************************************************************************************
 *10) RegWrite METHOD ALLOWS A USER TO BE ABLE TO WRITE TO A REGISTER ON A GIVEN RHINOCLUSTERCLIENT
 RETURNS A 0 IF IT HAS BEEN SUCCESFUL IN WRITING TO THE REGISTER AND -1 IF IT HASNT
*******************************************************************************************************************************************************************************************************/
int regWrite(uint32_t rhinoNumber,char *regName,uint16_t data,uint32_t programUserId)
{
      int client_len,server_len;
      int counter;
      unsigned long registerData;
      client_msg_T *cm, writeMessage; //THIS CREATES AN OBJECT OF THE MESSAGE
      server_msg_T *sm, readMessage;
      struct timeval begin, end, msg_begin, msg_end;
      long sec, usecs, mtime, msg_time, msg_sec, msg_usecs;
      unsigned int timeElapsed;
      int len;
      int i;
      FILE *fp;
	  
	      
      sm = &readMessage;
      cm = &writeMessage;
      //fp = fopen("regWriteTimes.txt","a+");
    
      
     /* if(!fp)
      {
	printf("Unable to open the regWriteTimes file\n");
      }*/
      
      
      //To test the reg write method
    
    // for(i=0; i<501; i++){
	//printf("STARTING THE TEST\n");		
      gettimeofday(&begin, NULL);
          
      for(counter=0; counter<RhinoClusterCounter;counter++)
      {
	   	
	  //1) First IF statement checking to see if named RHINO is in the cluster
	   if(rhinoStatTable[counter].rhinoNum == rhinoNumber)
	   {
		printf("THE RHINO IS IN THE CLUSTER\n");
		  
	      //2) CHECKING IF USER HAS ACCESS TO THE RHINO AND THE RHINO IS NOT BEING USED
	      if((rhinoStatTable[counter].activeBit==BOARD_INACTIVE)&&(rhinoStatTable[counter].programUserId==programUserId))
	      {
		  printf("THE RHINO IS IS BEING USED BY YOU\n");
		  //3) CHECKING IF THE RHINO IS PROGRAMMED ?????? POSSIBLY USE THE RHINOS BUSY BIT
		  if(rhinoStatTable[counter].rhinoBoard.busyBit == FPGA_PROGRAMMED)
		  {
		    
		      printf("THE RHINO BOARD IS PROGRAMMED AND AN ATTEMPT TO WRITE TO A REGISTER WILL START\n");
		      /******************************************************************************************************************************
			* USING MESSAGE PACKET STRUCTURES AND TCP SEND AND RECIEVE AS APPOSSED TO READ AND WRITE
			* 
		      ****************************************************************************************************************************/
		      
		      cm->messageID = htonl(REGISTER_WRITE);
		      cm->rhinoID   = htonl(rhinoStatTable[counter].rhinoNum);
		      strncpy(cm->reg_name, regName, sizeof(cm->reg_name)-1);//HOW TO CHANGE THE REGISTER NAME TO A STRING
		      cm->reg_name[32] 	= '\0';
		      printf("DATA BEFORE CONVERSION: [%d]\n", data);
		      cm->data      =   htons(data);
		      memset(cm->user_name, '\0', sizeof(cm->user_name));
			  
		      printf("Client_MsgID IS: %d\n",cm->messageID);
		      printf("Client_RHINOID IS: %d\n",cm->rhinoID);
		      printf("Client_REG_NAME IS: %s\n",cm->reg_name);
		      printf("Client_DATA IS: %d\n",cm->data);
		      printf("Client_USER_NAME: [%s]\n", cm->user_name);
		    
		      gettimeofday(&msg_begin, NULL);
		      
		      client_len = send(rhinoStatTable[counter].sockfd, cm, sizeof(writeMessage), MSG_NOSIGNAL); //THINK OF ANOTHER WAY OF GETTING THE SIZE OF THE PACKET TO BE SENT
		      printf("JUST SENT THE CLIENT MSG\n");
		      //ADD SOME ERROR HANDLING HERE FOR THE SEND
		      
		      server_len = recv(rhinoStatTable[counter].sockfd, sm, sizeof(readMessage), 0); //THINK ON THIS PART AND HOW YOU ARE GOING TO SET THE SIZE OF THE MESSAGE TO BE REICEVED
		      
		      gettimeofday(&msg_end, NULL);	  
		       
		      if(server_len < 0){
				    switch(errno){
					    case EAGAIN :
					    case EINTR  :
						    return 0;
					    default :
						    error("tcp receive failed");
						    return 0;
				    }
			}
		      printf("RECIEVED THE SERVER MSG\n");
			 
				   
		      sm->rhinoBoard.nameNumber 	= ntohl( sm->rhinoBoard.nameNumber);
		      printf("RHINO NAME NUMBER:[%d]\n",sm->rhinoBoard.nameNumber);
				  
		      strncpy(sm->rhinoBoard.ipAddress, sm->rhinoBoard.ipAddress, sizeof(sm->rhinoBoard.ipAddress)-1);//HOW TO CONVERT THIS STRING INTO A NUMBER SO AS TO HAVE A BINARY PROTOCOL
		      sm->rhinoBoard.ipAddress[16] ='\0';
		      printf("THE RHINO IP ADDRESS IS: [%s]\n",sm->rhinoBoard.ipAddress);
			  
		      sm->rhinoBoard.busyBit	= ntohl(sm->rhinoBoard.busyBit);
		      printf("RHINO BUSYBIT: [%d]\n",sm->rhinoBoard.busyBit);
			  
		      strncpy(sm->rhinoBoard.registersAccessible.regName, sm->rhinoBoard.registersAccessible.regName,sizeof(sm->rhinoBoard.registersAccessible.regName)-1);//HOW TO CONVERT THIS STRING TO A NUMBER TO HAVE A BINARY PROTOCOL
		      sm->rhinoBoard.registersAccessible.regName[32]	='\0';
		      printf("THE RHINO REGISTER NAMES AS IS: [%s]\n",sm->rhinoBoard.registersAccessible.regName);
				  
		      sm->rhinoBoard.registersAccessible.readWriteBit 	= ntohl(sm->rhinoBoard.registersAccessible.readWriteBit);
		      sm->rhinoBoard.registersAccessible.rhinoNameNumber = ntohl(sm->rhinoBoard.registersAccessible.rhinoNameNumber);
		  
		      sm->data = ntohs(sm->data); //IN THE WRITE MESSAGE THE RESPONSE FROM THE SERVER SHOULD HAVE NOTHING IN THE DATA ELEMENT
		      printf("RHINO DATA IS: [%d]\n", sm->data);
			  
		      sm->rhinoBoard.successBit = ntohl(sm->rhinoBoard.successBit);
		      printf("RHINO SUCCESS BIT: [%d]\n", sm->rhinoBoard.successBit);
		      
		      gettimeofday(&end, NULL);
		      
		      sec = end.tv_sec - begin.tv_sec;
		      usecs = end.tv_usec - begin.tv_usec;
		      msg_sec = msg_end.tv_sec - msg_begin.tv_sec;
		      msg_usecs = msg_end.tv_usec - msg_begin.tv_usec;
		      
		      mtime = ((sec)*1000+ usecs/1000.0) + 0.5;
		      msg_time = ((msg_sec)*1000 + msg_usecs/1000.0) + 0.5;
		      
		     // printf("THE ELAPSED TIME WRITE IS: [%1d] and the corresponding MSG_TIME [%1d] \n",mtime, msg_time);
		      
		      //len = fprintf(fp, "%1d \n",mtime);
		      
		   /*    if(len<0)
		      {
			printf("NO TIME VALUES WRITTEN TO FILE\n");
		      }
		      
		      //len = fprintf(fp, "%1d\n", msg_time);
		      
		     if(len<0)
		      {
			printf("NO TIME VALUES WRITTEN TO FILE\n");
		      }
		      else
		      {
			printf("WROTE [%d] length\n",len);
			
		      }*/
		      
		    
		     //ADD SOME ERROR HANDLING CODE HERE AS WELL
		     
		     //if(i==500)
		    // {
		       //fclose(fp);
		      return sm->rhinoBoard.successBit;
		     //}
			      
		}
		else if(rhinoStatTable[counter].rhinoBoard.busyBit == FPGA_UNPROGRAMMED)
		{
		   printf("THE RHINO IS NOT PROGRAMMED WITH A BOF FILE HENCE NO WRITE CAN OCCUR\n");
		   return FAILURE;
		}
		    
	      }
	      //2) CHECKING IF USER HAS ACCESS TO THE RHINO AND THE RHINO IS NOT BEING USED
	      else if(rhinoStatTable[counter].activeBit==BOARD_ACTIVE)
	      {
		  printf("RHINO IS ACTIVE AND HENCE HAS NOT BEEN SELECTED\n"); 
		  return FAILURE;
	      }
	      else if((rhinoStatTable[counter].programUserId!=programUserId)&&(rhinoStatTable[counter].activeBit==BOARD_INACTIVE))
	      {
		  printf("RHINO IS ACTIVE BUT IS NOT ASSIGNED TO THIS USER\n");
		  return FAILURE;
	      }
	      else
	      {
		  printf("STATE OF RHINO UNKNOWN\n");
		  return FAILURE;
	      }
		  	  
	 }
	   
	   
	  
      }
      //}//End of Test loop
      printf("THE RHINO WITH THE NUMBER [%d], WAS NOT FOUND ON THE CLUSTER\n",rhinoNumber);
      return FAILURE;
      
 
  // return sm->rhinoBoard.successBit;
	    
}
	  
/*******************************************************************************************************************************************************************************************************
*11) RegRead METHOD ALLOWS A USER TO BE ABLE TO READ FROM A GIVEN RHINOCLUSTERCLIENT
********************************************************************************************************************************************************************************************************/
uint16_t regRead(uint32_t rhinoNumber,char *regName,uint32_t programUserId)
{
	int client_len,server_len;
	int len;
	int i;
	unsigned long registerData;
	int counter;
	client_msg_T *cm, writeMessage;
	server_msg_T *sm, readMessage;
	cm = &writeMessage;
	sm = &readMessage;
	char time[16];
	char *time2;
	FILE *fp;
	struct timeval begin, end;
	long sec, usecs, mtime;
	unsigned int timeElapsed;
	
	//fp = fopen("regReadTimes.txt","w");
		  
	//MIGHT NEED TO CHECK IF THE BOARD IS STILL CONNECTED 
	
	// for(i=0; i<501; i++){
	   
	//printf("STARTING THE TEST\n");
	gettimeofday(&begin, NULL); 
	
	for(counter=0; counter<RhinoClusterCounter;counter++)
	{
		//1) First IF statement checking to see if named RHINO is in the cluster
		if(rhinoStatTable[counter].rhinoNum == rhinoNumber)
		{
		  printf("THE RHINO IS IN THE CLUSTER\n");
		  
		  /*****************************************************************************************************************************
		  * NEED TO MAKE SURE THAT THE RHINO IS PROGRAMMED BEFORE YOU CAN PERFORM THIS OPERATION 
		  ***************************************************************************************************************************************/
		  //2) CHECKING IF USER HAS ACCESS TO THE RHINO AND THE RHINO IS NOT BEING USED
		  if((rhinoStatTable[counter].activeBit==BOARD_INACTIVE) && (rhinoStatTable[counter].programUserId == programUserId))
		  {
		    printf("THE RHINO IS IS BEING USED BY YOU\n");
		    //3) CHECKING IF THE RHINO IS PROGRAMMED ?????? POSSIBLY USE THE RHINOS BUSY BIT
		    if(rhinoStatTable[counter].rhinoBoard.busyBit == FPGA_PROGRAMMED)
		    {
			  printf("THE RHINO BOARD IS PROGRAMMED AND AN ATTEMPT TO READ A REGISTER WILL START\n");
			  
			   /******************************************************************************************************************************
			   * USING MESSAGE PACKET STRUCTURES AND TCP SEND AND RECIEVE AS APPOSSED TO READ AND WRITE
			   * 
			   ****************************************************************************************************************************/
			  
			  cm->messageID = htonl(REGISTER_READ);
			  cm->rhinoID = htonl(rhinoNumber);
			  strncpy(cm->reg_name, regName,sizeof(cm->reg_name)-1); //HOW TO CONVERT A STRING TO A NUMBER SO ITS A BINARY PROTOCOL
			  cm->reg_name[32] = '\0';
			  cm->data = htons(0);
			  memset(cm->user_name, '\0', sizeof(cm->user_name));
			    
			  printf("Client_MsgID IS: %d\n",cm->messageID);
			  printf("Client_RHINOID IS: %d\n",cm->rhinoID);
			  printf("Client_REG_NAME IS: %s\n",cm->reg_name);
			  printf("Client_DATA IS: %d\n",cm->data);
			  printf("Client_USER_NAME: [%s]\n", cm->user_name);
			  
			  
			  
			  client_len =send(rhinoStatTable[counter].sockfd, cm, sizeof(writeMessage),MSG_NOSIGNAL);
			  printf("JUST SENT THE CLIENT MSG\n");
			  //ADD SOME ERROR HANDING CODE HERE
			  
			  
			  server_len = recv(rhinoStatTable[counter].sockfd, sm, sizeof(readMessage), 0); //THINK ON THIS PART AND HOW YOU ARE GOING TO SET THE SIZE OF THE MESSAGE TO BE REICEVED
			  
			   
			  if(server_len < 0){
				    switch(errno){
					    case EAGAIN :
					    case EINTR  :
						    return 0;
					    default :
						    error("tcp receive failed");
						    return 0;
				  }
			  }
			  printf("RECIEVED THE SERVER MSG\n");
				   
			  sm->rhinoBoard.nameNumber 	= ntohl( sm->rhinoBoard.nameNumber);
			  printf("RHINO NAME NUMBER:[%d]\n",sm->rhinoBoard.nameNumber); 
			  
			  strncpy(sm->rhinoBoard.ipAddress, sm->rhinoBoard.ipAddress, sizeof(sm->rhinoBoard.ipAddress)-1);//HOW TO CONVERT THIS STRING INTO A NUMBER SO AS TO HAVE A BINARY PROTOCOL
			  sm->rhinoBoard.ipAddress[16] ='\0';
			  printf("THE RHINO IP ADDRESS IS: [%s]\n",sm->rhinoBoard.ipAddress);
			 
			  sm->rhinoBoard.busyBit	= ntohl(sm->rhinoBoard.busyBit);
			  printf("RHINO BUSYBIT: [%d]\n",sm->rhinoBoard.busyBit);
			
			  strncpy(sm->rhinoBoard.registersAccessible.regName, sm->rhinoBoard.registersAccessible.regName, sizeof(sm->rhinoBoard.registersAccessible.regName)-1);//HOW TO CONVERT THIS STRING TO A NUMBER TO HAVE A BINARY PROTOCOL
			  sm->rhinoBoard.registersAccessible.regName[32]		='\0';
			  printf("THE RHINO REGISTER NAMES AS IS: [%s]\n",sm->rhinoBoard.registersAccessible.regName);
			
			  sm->rhinoBoard.registersAccessible.readWriteBit 	= ntohs(sm->rhinoBoard.registersAccessible.readWriteBit);
			  sm->rhinoBoard.registersAccessible.rhinoNameNumber = ntohl(sm->rhinoBoard.registersAccessible.rhinoNameNumber);
				  
			  sm->data = ntohs(sm->data); //IN THE WRITE MESSAGE THE RESPONSE FROM THE SERVER SHOULD HAVE NOTHING IN THE DATA ELEMENT
			  printf("RHINO DATA IS: [%d]\n", sm->data);
			  
			  sm->successBit = ntohl(1);
			  printf("RHINO SUCCESS BIT: [%d]\n", sm->rhinoBoard.successBit);
			  
			  gettimeofday(&end, NULL);
		      
			  sec = end.tv_sec - begin.tv_sec;
			  usecs = end.tv_usec - begin.tv_usec;
			  
			  mtime = ((sec)*1000+ usecs/1000.0) + 0.5;
			//  len = snprintf(time, sizeof(time),"%d\n",mtime);
			  
			 /* printf("THE ELAPSED TIME FOR A READ IS: [%1d]\n", mtime);
			 len = fprintf(fp, "%1d\n",mtime);
		      
			  if(len<0)
			    {
			      printf("NO TIME VALUES WRITTEN TO FILE\n");
			    }
			    else
			    {
			      printf("WROTE [%d] length\n",len);
			      
			    }
			    */
			  
			  //ADD SOME ERROR HANDLING CODE HERE AS WELL
			  
			// if(i==500)
			// {
			 //  fclose(fp);						
			   return sm->data;
			 //}	
			
		    }
		    else if(rhinoStatTable[counter].rhinoBoard.busyBit == FPGA_UNPROGRAMMED)
		    {
		      printf("THE RHINO IS NOT PROGRAMMED WITH A BOF FILE HENCE NO WRITE CAN OCCUR\n");
		      return FAILURE;
		    }
		    
		  }
		  
		  else if(rhinoStatTable[counter].activeBit==BOARD_ACTIVE)
		  {
		      printf("RHINO IS ACTIVE AND HENCE HAS NOT BEEN SELECTED\n"); 
		      return FAILURE;
		  }
		  
		  else if((rhinoStatTable[counter].programUserId!=programUserId)&&(rhinoStatTable[counter].activeBit==BOARD_INACTIVE))
		  {
		    printf("RHINO IS ACTIVE BUT IS NOT ASSIGNED TO THIS USER\n");
		    return FAILURE;
		  }
		  else
		  {
		    printf("STATE OF RHINO UNKNOWN\n");
		    return FAILURE;
		  }
		}
	  // }//End of Test Loop
	}
	printf("THE RHINO WITH THE NUMBER WAS NOT FOUND ON THE CLUSTER");
	return FAILURE;
	    
}
	    
/***************************************************************************************************************************************************************************************************************
* 12) KILL BOF PROCESS: THIS METHOD KILLS A BOF PROCESS RUNNING ON THE RHINO BOARD
* ************************************************************************************************************************************************************************************************************/
int killBofProcess(uint32_t rhinoNumber, char *bofProcessName, uint32_t programUserId)
{
	      
    int client_len,server_len;
    int counter;
    char *data = NULL;
    int len;
    client_msg_T *cm, writeMessage; //THIS CREATES AN OBJECT OF THE MESSAGE
    server_msg_T *sm, readMessage;
	  
	      
    sm = &readMessage;
    cm = &writeMessage;
	      
    for( counter=0; counter<RhinoClusterCounter;counter++)
    {
	if(rhinoNumber == rhinoStatTable[counter].rhinoNum)
	{
	    printf("FOUND THE RHINO [%d] TO KILL THE PROCESS",rhinoNumber);
		      
	    //2) CHECKING IF USER HAS ACCESS TO THE RHINO AND THE RHINO IS NOT BEING USED
	    if((rhinoStatTable[counter].activeBit==BOARD_INACTIVE)&&(rhinoStatTable[counter].programUserId==programUserId))
	    {
		/*****************************************************************************************************************************
		* NEED TO MAKE SURE THAT THE RHINO IS PROGRAMMED BEFORE YOU CAN PERFORM THIS OPERATION 
		***************************************************************************************************************************************/
		if(rhinoStatTable[counter].rhinoBoard.busyBit == FPGA_PROGRAMMED)
		{
		    printf("RHINO IS PROGRAMMED AND PREPARING TO KILL THE RUNNING BOF PROCESS\n");
				  
				  
		    cm->messageID = htonl(KILL_BOF_PROCESS);
		    cm->rhinoID   = htonl(rhinoStatTable[counter].rhinoNum);
		    strncpy(cm->reg_name, bofProcessName, sizeof(cm->reg_name)-1);//HOW TO CHANGE THE REGISTER NAME TO A STRING
		    cm->reg_name[32] 	= '\0';
		    cm->data      =   htons(0);
		    memset(cm->user_name, '\0', sizeof(cm->user_name));
				  
		    printf("Client_MsgID IS: [%d]\n",cm->messageID);
		    printf("Client_RHINOID IS: [%d]\n",cm->rhinoID);
		    printf("Client_REG_NAME IS: [%s]\n",cm->reg_name);
		    printf("Client_DATA IS: [%d]\n",cm->data);
		    printf("Client_USER_NAME: [%s]\n", cm->user_name);
		   

		    client_len = send(rhinoStatTable[counter].sockfd, cm, sizeof(writeMessage), MSG_NOSIGNAL); //THINK OF ANOTHER WAY OF GETTING THE SIZE OF THE PACKET TO BE SENT
		    printf("JUST SENT THE CLIENT MSG\n");
		    server_len = recv(rhinoStatTable[counter].sockfd, sm, sizeof(readMessage), 0); //THINK ON THIS PART AND HOW YOU ARE GOING TO SET THE SIZE OF THE MESSAGE TO BE REICEVED
				  
		    if(server_len < 0){
			  switch(errno){
			    case EAGAIN :
			    case EINTR  :
				      return 0;
			    default :
				      error("tcp receive failed");
				      return 0;
			    }
		    }
		    printf("RECIEVED THE SERVER MSG\n");
				
					  
		    sm->rhinoBoard.nameNumber 	= ntohl( sm->rhinoBoard.nameNumber);
		    printf("RHINO NAME NUMBER:[%d]\n",sm->rhinoBoard.nameNumber);
					  
		    strncpy(sm->rhinoBoard.ipAddress, sm->rhinoBoard.ipAddress, sizeof(sm->rhinoBoard.ipAddress)-1);//HOW TO CONVERT THIS STRING INTO A NUMBER SO AS TO HAVE A BINARY PROTOCOL
		    sm->rhinoBoard.ipAddress[16] ='\0';
		    printf("THE RHINO IP ADDRESS IS: [%s]\n",sm->rhinoBoard.ipAddress);
				  
		    sm->rhinoBoard.busyBit	= ntohl(sm->rhinoBoard.busyBit);
		    printf("RHINO BUSYBIT: [%d]\n",sm->rhinoBoard.busyBit);
				  
		    strncpy(sm->rhinoBoard.registersAccessible.regName, sm->rhinoBoard.registersAccessible.regName,sizeof(sm->rhinoBoard.registersAccessible.regName)-1);//HOW TO CONVERT THIS STRING TO A NUMBER TO HAVE A BINARY PROTOCOL
		    sm->rhinoBoard.registersAccessible.regName[32]	='\0';
		    printf("THE RHINO REGISTER NAMES AS IS: [%s]\n",sm->rhinoBoard.registersAccessible.regName);
					  
		    sm->rhinoBoard.registersAccessible.readWriteBit 	= ntohs(sm->rhinoBoard.registersAccessible.readWriteBit);
		    sm->rhinoBoard.registersAccessible.rhinoNameNumber = ntohl(sm->rhinoBoard.registersAccessible.rhinoNameNumber);
				  
		    sm->data = ntohs(sm->data); //IN THE WRITE MESSAGE THE RESPONSE FROM THE SERVER SHOULD HAVE NOTHING IN THE DATA ELEMENT
		    printf("RHINO DATA IS: [%d]\n", sm->data);
				  
		    sm->rhinoBoard.successBit = ntohl(sm->rhinoBoard.successBit);
		    printf("RHINO SUCCESS BIT: [%d]\n", sm->rhinoBoard.successBit);
				  
				  
		    //ADD SOME ERROR HANDLING CODE HERE AS WELL
				  
		    return sm->rhinoBoard.successBit;
				  
		}
			      
		else if(rhinoStatTable[counter].rhinoBoard.busyBit ==FPGA_UNPROGRAMMED)
		{
		    printf("RHINO IS NOT PROGRAMMED WITH A BOF PROCESS\n");
		    return FAILURE;
		}
				
				
				
	    }
	    //2) CHECKING IF USER HAS ACCESS TO THE RHINO AND THE RHINO IS NOT BEING USED
	    else if(rhinoStatTable[counter].activeBit==BOARD_ACTIVE)
	    {
		printf("RHINO IS ACTIVE AND HENCE HAS NOT BEEN SELECTED\n"); 
		return FAILURE;
	    }
	    else if((rhinoStatTable[counter].programUserId!=programUserId)&&(rhinoStatTable[counter].activeBit==BOARD_INACTIVE))
	    {
		printf("RHINO IS ACTIVE BUT IS NOT ASSIGNED TO THIS USER\n");
		return FAILURE;
	    }
	    else
	    {
	      printf("STATE OF RHINO UNKNOWN\n");
	      return FAILURE;
	    }
		      
		  

	}
	else
	{
	  printf("COULD NOT FIND THE RHINO [%d] TO KILL THE PROCESS\n",rhinoNumber);
	  return FAILURE;
	}
    }
}
	    
/**********************************************************************************************************************************************************************************************************************
* 13) DISCONNECT FROM FROM A PARTICULAR RHINO 
***********************************************************************************************************************************************************************************************************************/
void disconnectToRhino(uint32_t rhinoNumber, char *ipAddress)
{
	int counter;
	int len;
	      
	for(counter=0; counter<RhinoClusterCounter;counter++)
	{
	  //1) First IF statement checking to see if named RHINO is in the cluster
	  if(rhinoStatTable[counter].rhinoNum == rhinoNumber)
	  {
	    printf("DISCONNECT METHOD HAS FOUND THE RHINO [%d]\n",rhinoNumber);
		  
	    //2) .................................................................????
	  }
	}
	      
	printf("COULD NOT FIND THE RHINO TO DISCONNECT FROM\n");
}
	   
	   
/*******************************************************************************************************************************************************************************************************************
* THIS METHOD CONVERTS AN IPADDRESS INTO AN unsigned int 
****************************************************************************************************************************************************************************************************************************/ 
uint32_t parseIPV4string(char *ipAddress)
{
	char ipbytes[4];
	    	    
	// printf("GETTING INTO ParseIPV4String METHOD\n");
	// printf("THE STRING IPADDRESS IN THE ParseIPV4String is: %s\n", ipAddress);
	    
	sscanf(ipAddress,"%d.%d.%d.%d",(int *)&ipbytes[3],(int *)&ipbytes[2],(int *)&ipbytes[1],(int *)&ipbytes[0]);
	printf("THE IPADDRESS IS AFTER USING SSCANF: %d.%d.%d.%d \n",ipbytes[0],ipbytes[1],ipbytes[2],ipbytes[3]);
	//printf("GETTING PAST THE FOR LOOP\n");
	return ipbytes[0] | ipbytes[1] | ipbytes[2] | ipbytes[3];
}

/**************************************************************************************************************************************************************************************************************************
 * INITIALIZES EACH RHINOBOARD ADDED TO THE CLUSTER
 *************************************************************************************************************************************************************************************************************************/
void initRhinoBoard(int rhinoNumber, char *ipAddress)
{
  rhinoStatTable[rhinoNumber].rhinoBoard.nameNumber = rhinoNumber;
  
  strncpy(rhinoStatTable[rhinoNumber].rhinoBoard.ipAddress, ipAddress, sizeof(rhinoStatTable[rhinoNumber].rhinoBoard.ipAddress)-1);
  rhinoStatTable[rhinoNumber].rhinoBoard.ipAddress[16] = '\0';
  
  rhinoStatTable[rhinoNumber].rhinoBoard.busyBit = -1;
  
  rhinoStatTable[rhinoNumber].rhinoBoard.successBit = -1; 
  
  memset(&rhinoStatTable[rhinoNumber].rhinoBoard.registersAccessible.regName[0], 0, sizeof(rhinoStatTable[rhinoNumber].rhinoBoard.registersAccessible.regName));
  
  rhinoStatTable[rhinoNumber].rhinoBoard.registersAccessible.readWriteBit = -1;
  
  rhinoStatTable[rhinoNumber].rhinoBoard.registersAccessible.rhinoNameNumber = rhinoNumber;
  
  rhinoStatTable[rhinoNumber].rhinoBoard.registersAccessible.size = -1;
  
}

void clientPacketTest(int rhinoNumber, int testTimes, uint16_t dataToTest)
{
      int counter = 0;
      struct timeval begin, end;
      long sec, usecs, mtime;
      int len;
      unsigned int timeElapsed;
      FILE *fp;
      client_msg_T *cm, writeMessage; //THIS CREATES AN OBJECT OF THE MESSAGE
      cm = &writeMessage;
      int client_sent,client_recv;
      //fp = fopen("clientTimings.txt","w");
      
      //constructing the client packet
       cm->messageID = htonl(REGISTER_WRITE);
       cm->rhinoID   = htonl(rhinoStatTable[rhinoNumber].rhinoNum);
       strncpy(cm->reg_name, "A", sizeof(cm->reg_name)-1);//HOW TO CHANGE THE REGISTER NAME TO A STRING
       cm->reg_name[32] 	= '\0';
       cm->data      =   htonl(dataToTest);
       memset(cm->user_name, '\0', sizeof(cm->user_name));
       printf("Client_MsgID IS: %d\n",cm->messageID);
       printf("Client_RHINOID IS: %d\n",cm->rhinoID);
       printf("Client_REG_NAME IS: %s\n",cm->reg_name);
       printf("Client_DATA IS: %d\n",cm->data);
       printf("Client_USER_NAME: [%s]\n", cm->user_name);
       fp = fopen ("clientTimings.txt","w");
       //printf("THE RHINO SOCKFD NUMBER IS: [%d]\n", rhinoStatTable[rhinoNumber].sockfd);
      
      for(counter; counter<testTimes; counter++)
      {
	gettimeofday(&begin, NULL);
	//begin = clock();
	//printf("THE BEGIN CLOCK IS: [%s]\n", begin);
	
	client_sent = send(rhinoStatTable[rhinoNumber].sockfd, cm, sizeof(writeMessage), MSG_NOSIGNAL);
	//printf("MESSAGE SENT\n");
	client_recv = recv(rhinoStatTable[rhinoNumber].sockfd, cm, sizeof(writeMessage), 0);
	//printf("MESSAGE RECIEVED\n");
	
	gettimeofday(&end, NULL);
	sec = end.tv_sec - begin.tv_sec;
	usecs = end.tv_usec - begin.tv_usec;
	
	mtime = ((sec)*1000+ usecs/1000.0) + 0.5;
	//end = clock();
	//printf("THE END CLOCK IS: [%s]\n", end);
	//timeElapsed = ((unsigned int)(end -begin))*1000/ CLOCKS_PER_SEC;
	len = fprintf(fp, "%1d\n", mtime);
	printf("TIME ELAPSED: [%1d]\n", mtime);
	
	//Place time in file
	//fwrite(&timeElapsed, sizeof(unsigned int), 1, fp);
	
      }
      printf("GETTING HERE\n");
      fclose(fp);
}

void serverPacketTest(int rhinoNumber, int testTimes)
{
      int counter = 0;
      struct timeval begin, end;
      long sec, usecs, mtime;
      int len;
      unsigned int timeElapsed;
      FILE *fp;
      server_msg_T *cm, readMessage, *sm, readMessage1; //THIS CREATES AN OBJECT OF THE MESSAGE
      cm = &readMessage;
      int server_sent,server_recv;
      cm = &readMessage;
      sm = &readMessage1;
      fp = fopen ("server_Packet_Timings.txt","w");
      
      cm->rhinoBoard.nameNumber = htonl(0);
      strncpy(cm->rhinoBoard.ipAddress, rhinoStatTable[rhinoNumber].rhinoIpAdd, sizeof(cm->rhinoBoard.ipAddress)-1);
      cm->rhinoBoard.ipAddress[16] = '\0';
      cm->rhinoBoard.busyBit = htonl(0);
      cm->rhinoBoard.successBit = htonl(0);
      memset(&cm->rhinoBoard.errorMsg[0], 0 , sizeof(cm->rhinoBoard.errorMsg));
      strncpy(cm->rhinoBoard.registersAccessible.regName, "A", sizeof(cm->rhinoBoard.registersAccessible)-1);
      cm->rhinoBoard.registersAccessible.regName[16] = '\0';
      cm->rhinoBoard.registersAccessible.readWriteBit = htonl(3);
      cm->rhinoBoard.registersAccessible.rhinoNameNumber = htonl(0);
      cm->rhinoBoard.registersAccessible.size = htonl(3);
      
      cm->successBit = htonl(0);
      cm->data = htons(8);
      printf("RHINO BUSYBIT: [%d]\n",cm->rhinoBoard.busyBit);
      printf("RHINO NAMENUMBER: [%d]\n", cm->rhinoBoard.nameNumber);
      printf("THE IP ADDRESS OF THE RHINO IS: [%s]\n", cm->rhinoBoard.ipAddress);
      printf("THE RHINO REGISTER NAME IS: [%s]\n", cm->rhinoBoard.registersAccessible.regName);
      printf("THE RHINO REGISTER READWRITEBIT IS: [%d]\n", cm->rhinoBoard.registersAccessible.readWriteBit);
      printf("THE RHINO ERROR MSG IS: [%s]\n", cm->rhinoBoard.errorMsg);
      printf("THE RHINO REGISTER NAMENUMBER IS: [%d]\n", cm->rhinoBoard.registersAccessible.rhinoNameNumber);
      printf("THE RHINO REGISTER SIZE IS: [%x]\n", cm->rhinoBoard.registersAccessible.size);
      printf("THE RHINO REGISTER DATA IS: [%d]\n", cm->data);
      
      
      for(counter; counter<testTimes; counter++)
      {
	gettimeofday(&begin, NULL);
	
	
	server_sent = send(rhinoStatTable[rhinoNumber].sockfd, cm, sizeof(readMessage), MSG_NOSIGNAL);
	//printf("MESSAGE SENT\n");
	server_recv = recv(rhinoStatTable[rhinoNumber].sockfd, cm, sizeof(readMessage), 0);
	//printf("MESSAGE RECIEVED\n");
	
	gettimeofday(&end, NULL);
			  
	sec = end.tv_sec - begin.tv_sec;
	usecs = end.tv_usec - begin.tv_usec;
	
	mtime = ((sec)*1000+ usecs/1000.0) + 0.5;
	
	len = fprintf(fp, "%1d\n", mtime);
	printf("TIME ELAPSED: [%1d]\n", mtime);
	
	
      }
      printf("GETTING HERE\n");
      fclose(fp);
}
