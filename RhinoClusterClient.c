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
#include "clientLibraryHeader.h"

#define BOARD_ACTIVE 			1
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
#define NTP_MAX_PACKET		468
#define CLIENT_MESSAGE_HEADER  12






FILE *fileIpAdd; //this is the file that the servers running on the rhinos update when they start hence it contains the number of rhinos that have registered with it
char *numberOfClusterRhinos;
int numberOfAvailableRhinos;
Rhino *availableRhinos; //looks like i will have to dynamically allocate memory
RhStatTable *rhinoStatTable = NULL;
int sockfd;
int portno;
int n;
int RhinoClusterCounter=0;
char protocolPacket [256] ;
  
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
	char ipAddress[64];
	int *ipAddress2;
	int programUserId = getpid(); //gets this programs program ID
	unsigned int data;
	int numberOfRhinosOnCluster;
	Register *returnedRegisters;
	int i;
	int successBit;
	char *serverUserName; //MAYBE THIS COULD BE TAKEN BEHIND THE SCENES?????!!!!!! [HOW]
	char *portno;
	
	
	while(1)
      {
	printf("PLEASE ENTER IN THE COMMAND YOU WOULD LIKE TO CALL:\n");
	scanf("%d",&methodNum);
	
	switch (methodNum)
	{
	  case 1:
	    printf("YOU HAVE CHOOSEN THE METHOD numRhino\n"); 
	    printf("PLEASE NOTE YOU CANT USE THIS METHOD UNTILL THE CONNECT METHOD HAS BEEN CALLED\n");
	    numberOfRhinosOnCluster = numRhinos();
	    printf("THE NUMBER OF RHINOS ON THE CLUSTER IS: [%d]\n",numberOfRhinosOnCluster);
	    break;
	    
	  case 2:
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
	    
	  case 3:
	    printf("YOU HAVE CHOOSEN THE METHOD listDeviceRegisters\n");
	    printf("PLEASE ENTER THE RHINO YOU WOULD LIKE TO PRINT THE REGISTERS OF\n");
	    scanf("%d",&rhinoNameNumber);
	    returnedRegisters = listDeviceRegisters(rhinoNameNumber);
	    
	    for(i=0;i<sizeof(returnedRegisters);i++)
	    {
	      printf("REGISTER NAME: [%s]\n",returnedRegisters[i].regName);
	      printf("REGISTER READ/WRITE BIT: [%d]\n", returnedRegisters[i].readWriteBit);
	      printf("RHINO REGISTER BELONGS TO: [%d]\n", returnedRegisters[i].rhinoNameNumber); 
	    }
	    break;
	    
	  case 4:
	    printf("YOU HAVE CHOOSEN THE METHOD clusterStatus\n");
	    clusterStatus();
	    break;
	    
	  case 5:
	    printf("YOU HAVE CHOOSEN THE METHOD loadBofFile\n");
	    printf("PLEASE ENTER IN THE RHINO NUMBER THE BOFPROCESS NAME AND THE USERS SERVERNAME\n");
	    scanf("%d %s %s",&rhinoNameNumber,&bofProcess,&serverUserName);
	    successBit = loadBofFile(rhinoNameNumber,&bofProcess,&serverUserName);
	    printf("IF 0 BOFPROCESS HAS SUCESSFULLY STARTED, ELSE IF -1 BOFPROCESS HAS BOFPROCESS HAS NOT STARTED: [%d]\n",successBit);
	    break;
	    
	  case 6:
	    printf("YOU HAVE CHOOSEN THE METHOD regWrite\n");
	    printf("PLEASE ENTER THE RHINO YOU WANT TO WRITE TO, THE NAME OF THE REGISTER, AND THE DATA YOU WANT TO WRITE\n");
	    scanf("%d %s %d",&rhinoNameNumber,&regName,&data);
	    successBit =regWrite(rhinoNameNumber,&regName,data,programUserId);
	    printf("IF 0 SUCCESS WRITE HAS OCCURED, ELSE IF -1 WRITE HAS NOT BEEN SUCCESSFUL: [%d]\n",successBit);
	    break;
	    
	  case 7:
	    printf("YOU HAVE CHOOSEN THE METHOD regRead\n");
	    printf("PLEASE ENTER THE RHINO YOU WANT TO WRITE TO, THE NAME OF THE REGISTER, AND THE DATA YOU WANT TO WRITE\n");
	    scanf("%d %s",&rhinoNameNumber,&regName);
	    data =regRead(rhinoNameNumber,&regName,programUserId);
	    printf("DATA IN REGISTER %s IS: [%d]\n",data);
	    break;
	    
	  case 8:
	    printf("YOU HAVE CHOOSEN THE METHOD connectToRhino\n");
	    connectToRhino();
	    printf("CHECK IF CONNECTION HAS BEEN MADE\n");
	    break;
	  case 9:
	    printf("YOU HAVE CHOOSEN THE METHOD rhinoSelect\n");
	    printf("PLEASE ENTER IN THE RHINO YOU WANT TO SELECT\n");
	    scanf("%d",&rhinoNameNumber);
	    successBit = rhinoSelect (rhinoNameNumber,programUserId);
	    printf("IF 0 SUCCESS RHINOSELECT HAS BEEN SUCCESSFUL, ELSE IF -1 RHINOSELECT HAS NOT BEEN SUCCESSFUL: [%d]\n",successBit);
	    break;
	  case 10:
	    printf("YOU HAVE CHOOSEN THE METHOD rhinoRelease\n");
	     printf("PLEASE ENTER IN THE RHINO YOU WANT TO RELEASE\n");
	    scanf("%d",&rhinoNameNumber);
	    successBit = rhinoRelease(rhinoNameNumber,programUserId);
	     printf("IF 0 SUCCESS RHINORELEASE HAS BEEN SUCCESSFUL, ELSE IF -1 RHINORELEASE HAS NOT BEEN SUCCESSFUL: [%d]\n",successBit);
	    break;
	  case 11:
	    printf("YOU HAVE CHOOSEN THE METHOD connectToSpecificRhino\n");
	    printf("PLEASE ENTER IN THE RHINO NUMBER YOU WOULD LIKE TO CONNECT TO AND THE IPADDRESS AND LASTLY THE PORTNO\n");
	    scanf("%d %s %s",&rhinoNameNumber,&ipAddress,&portno);
	    printf("THE RHINONAMENUMBER IS: [%d]\n",rhinoNameNumber);
	    printf("THE IPADDRESS IS:[%s]\n",&ipAddress);
	    printf("THE PORTNO IS: [%s]\n",&portno);
	    connectToSpecificRhino(rhinoNameNumber,&ipAddress,&portno);
	    break;
	  case 12:
	     printf("YOU HAVE CHOOSEN THE METHOD disconnectToRhino\n");
	     printf("PLEASE ENTER IN THE RHINO NUMBER YOU WOULD LIKE TO DISCONNECT FROM AND THE IPADDRESS \n");
	    scanf("%d %s %s",&rhinoNameNumber,&ipAddress);
	     disconnectToRhino(rhinoNameNumber,&ipAddress);
	    break;
	  case 13:
	    printf("YOU HAVE CHOOSEN THE METHOD killBofProcess\n");
	    printf("PLEASE ENTER THE RHINONUMBER THE BOF PROCESS IS RUNNING ON AND THE BOFPROCESS NAME YOU WOULD LIKE TO STOP\n");
	    scanf("%d %s",&rhinoNameNumber,&bofProcess);
	    successBit = killBofProcess(rhinoNameNumber,&bofProcess);
	    printf("IF 0 KILLING BOF PROCESS HAS BEEN SUCCESSFUL, ELSE IF -1 KILLING BOF PROCESS HAS NOT BEEN SUCCESSFUL: [%d]\n",successBit);
	    break;
	    default:
	      printf("NONE OF THE METHODS YOU HAVE SELECTED ARE IN THE API\n"); 
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
	  struct Register * listRegisters()
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
		    cm->messageID = htons(LIST_REGISTERS);
		    cm->rhinoID   = htons(rhinoStatTable[counter].rhinoNum);
		    cm->regName   = NULL; //HOW TO CHANGE THE REGISTER NAME TO A STRING
		    cm->data      = htonl(0);

		    client_len = send(rhinoStatTable[counter].sockfd, cm,CLIENT_MESSAGE_HEADER, MSG_NOSIGNAL); //THINK OF ANOTHER WAY OF GETTING THE SIZE OF THE PACKET TO BE SENT
			  
		   //ADD SOME ERROR HANDLING HERE FOR THE SEND
			  
		   server_len = recv(rhinoStatTable[counter].sockfd,sm,NTP_MAX_PACKET, MSG_DONTWAIT); //THINK ON THIS PART AND HOW YOU ARE GOING TO SET THE SIZE OF THE MESSAGE TO BE REICEVED
			  
		   sm->rhinoBoard.nameNumber 	= ntohs(sm->rhinoBoard.nameNumber);
		   sm->rhinoBoard.ipAddress	= sm->rhinoBoard.ipAddress;//HOW TO CONVERT THIS STRING INTO A NUMBER SO AS TO HAVE A BINARY PROTOCOL
		   sm->rhinoBoard.busyBit	= ntohs(sm->rhinoBoard.busyBit);
		   sm->rhinoBoard.registersAccessible->regName 	= sm->rhinoBoard.registersAccessible->regName; //HOW TO CONVERT THIS STRING TO A NUMBER TO HAVE A BINARY PROTOCOL
		   sm->rhinoBoard.registersAccessible->readWriteBit 	= ntohs(sm->rhinoBoard.registersAccessible->readWriteBit);
		   sm->rhinoBoard.registersAccessible->rhinoNameNumber = ntohs(sm->rhinoBoard.registersAccessible->rhinoNameNumber);
			  
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

	    /******************************************************************************************************************************************************************************************
	    *3) THIS METHOD DISPLAYS THE AVAILABLE REGISTER ON A NAMED REGISTER ONLY USEFUL IF THE FPGA IS PROGRAMMED PRINTS OUT TO SCREEN
	    ******************************************************************************************************************************************************************************************/
	    struct Register * listDeviceRegisters(int rhinoNameNumber)
	    {
	      int counter;
	      int len;
	      Register *rhinoReg;
	      struct client_msg *cm,readMessage;
	      struct server_msg *sm,writeMessage;
	      int client_len, server_len;
	      
	      for(counter=0; counter<RhinoClusterCounter;counter++)
	      {
		//1) First IF statement checking to see if named RHINO is in the cluster
		if(rhinoStatTable[counter].rhinoNum == rhinoNameNumber)
		{
		  printf("THE RHINO IS IN THE CLUSTER\n");
		  //FOUND RHINO IS IN THE CLUSTER 2)Checking to see if the RHINO is active or Not
		  if(rhinoStatTable[counter].activeBit==BOARD_ACTIVE)
		  {
		    printf("RHINO %d IS ACTIVE",rhinoStatTable[counter].rhinoNum);
		    
		   //CHECKING TO SEE IF THE RHINO IS PROGRAMMED BEFORE CHECKING FOR THE REGISTERS
		    if(rhinoStatTable[counter].rhinoBoard->busyBit ==FPGA_PROGRAMMED)
		    {
		      printf("RHINO IS ACTIVE AND PROGRAMMED\n");
		      
		       /******************************************************************************************************************************
		      * USING MESSAGE PACKET STRUCTURES AND TCP SEND AND RECIEVE AS APPOSSED TO READ AND WRITE
		      * 
		     ****************************************************************************************************************************/
		    cm->messageID = htons(LIST_REGISTERS);
		    cm->rhinoID   = htons(rhinoNameNumber);
		    cm->regName   = &cm->regName//HOW TO CHANGE THE REGISTER NAME TO A STRING
		    cm->data      =   htonl(0);

		    client_len = send(rhinoStatTable[counter].sockfd, cm,CLIENT_MESSAGE_HEADER, MSG_NOSIGNAL); //THINK OF ANOTHER WAY OF GETTING THE SIZE OF THE PACKET TO BE SENT
			  
		   //ADD SOME ERROR HANDLING HERE FOR THE SEND
			  
		   server_len = recv(rhinoStatTable[counter].sockfd,sm,NTP_MAX_PACKET, MSG_DONTWAIT); //THINK ON THIS PART AND HOW YOU ARE GOING TO SET THE SIZE OF THE MESSAGE TO BE REICEVED
			  
		   sm->rhinoBoard.nameNumber 	= ntohs(sm->rhinoBoard.nameNumber);
		   sm->rhinoBoard.ipAddress	= sm->rhinoBoard.ipAddress;//HOW TO CONVERT THIS STRING INTO A NUMBER SO AS TO HAVE A BINARY PROTOCOL
		   sm->rhinoBoard.busyBit	= ntohs(sm->rhinoBoard.busyBit);
		   sm->rhinoBoard.registersAccessible->regName 	= sm->rhinoBoard.registersAccessible->regName ;//HOW TO CONVERT THIS STRING TO A NUMBER TO HAVE A BINARY PROTOCOL
		   sm->rhinoBoard.registersAccessible->readWriteBit 	= ntohs(sm->rhinoBoard.registersAccessible->readWriteBit);
		   sm->rhinoBoard.registersAccessible->rhinoNameNumber = ntohs(sm->rhinoBoard.registersAccessible->rhinoNameNumber);
			  
		   sm->data = ntohs(0); //IN THE WRITE MESSAGE THE RESPONSE FROM THE SERVER SHOULD HAVE NOTHING IN THE DATA ELEMENT
		   sm->successBit = ntohs(0);
		      

		      
/**********************************************************************************************************************************************************************************************************************************************************************************     
		    /*****************************************************************************************************************************************************************************************************
		    //THINK ABOUT ADDING THIS PART HERE
		    //this part of the code retrieves the registers available on the named rhino
						for(counter=0; counter<sizeof(rhinoName.registersAccessible);counter++) //for that given rhino the inner loop goes through every single one of its registers
						{
							rhinoReg.rhinoNameNumber= rhinoName.registersAccessible[counter].rhinoNameNumber;
							rhinoReg.regName = rhinoName.registersAccessible[counter].regName;
							rhinoReg.readWriteBit = rhinoName.registersAccessible[counter].readWriteBit;

						}
		    **************************************************************************************************************************************************************************************************/
		    //WRAPPING MESSAGE ID INTO PACKET AND SENDING TO RHINO USING CORRESPONDING SOCKET =SOCKFD
		  /*  bzero(protocolPacket,256);
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
		    
/***************************************************************************************************************************************************************************************************************************************************/		    
		    return sm->rhinoBoard.registersAccessible;
		      
		      
		    }
		    else if(rhinoStatTable[counter].rhinoBoard->busyBit == FPGA_UNPROGRAMMED)
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
	      for(counter=0;counter<RhinoClusterCounter;counter++)
	      {
		/****************************************************************************************************************************************************************************************
		VERY IFFY ABOUT THIS PRINTING METHOD MAY NEED TO LOOK THROUGH IT AGAIN AND REWRITE THIS PRINTF STATEMENT!!!!!!!!!!!!!!!!!!!!!!
		********************************************************************************************************************************************************************************************/
		  printf("RHINO NUMBER: %d, RHINO IP Address: %s, STATUS: %s \n",rhinoStatTable[counter].rhinoNum,rhinoStatTable[counter].rhinoIpAdd,rhinoStatTable[counter].activeBit);
		
	      }
	    }

	    
	    /****************************************************************************************************************************************************************************************************************************
	    *5) THIS METHOD ALLOWS A USER TO START A BOF FILE ON A GIVEN RHINO NODE.IT TAKES IN THE RHINOS IPADDRESS, THE NAME OF THE BOF PROCESS TO BE EXECUTED (THIS BOF FILE SHOULD ALREADY EXIST IN THE NFS FILE SYSTEM ON THE SERVER
	    AND LASTLY THE USERS NAME AS IS ON THE SERVER.
	    ****************************************************************************************************************************************************************************************************************************/
	    int loadBofFile(int rhinoNumber, char *bofProcessName, char *serverUserName)
	    {
	      
		char *command;
		char *serverUserNameCommand;
		char *bofNfsFiles;
		int len;
		int counter;
		FILE *userNfsFile;
		int statusBit;
		struct client_msg *cm,writeMessage;
		struct server_msg *sm,readMessage;
		int client_len,server_len;
		
		cm = &writeMessage;
		sm = &readMessage;
		
		/*#####################################################################################################################################################################
	      //First Check that the bofProcess being programmed actually exists on the file system before going further by openning the /home directory under users name on server
	      
	      $Need to also check that the .bof files are executable....THINK ON THAT ONE
	      $also needs to fist select the RHINO and check that it is actually available to have something run on it
	      ########################################################################################################################################################################*/	  
	      
	      bzero(serverUserNameCommand,255);
	      len = snprintf(serverUserNameCommand,sizeof(serverUserNameCommand),"cd /home/%s/rhino/bof/",serverUserName); //cding into the directory that contains pressumably just .bof files
	      printf("Command from snprintf: %s\n",serverUserNameCommand);
	      printf("Returning Value from snprintf: %d\n",len);
	      
	      
	      if(len <= sizeof(serverUserNameCommand))
	      {
		userNfsFile= popen(serverUserNameCommand,"r");
		if(!userNfsFile)
		{
		  error("Error on openning the pipe to write the command to cd into diretory /home/serverUserName/rhino/bof \n");
		  return FAILURE;
		}
	      }
	    
	      bzero(serverUserNameCommand,255);
	      len = snprintf(serverUserNameCommand,sizeof(serverUserNameCommand),"grep -l .bof *",serverUserName);//serching the /rhino/bof directory for the list of all possible .bof files to run
	      printf("Command from snprintf: %s\n",serverUserNameCommand);
	      printf("Returning Value from snprintf: %d\n",len);
	      
	      
	      if(len <= sizeof(serverUserNameCommand))
	      {
		userNfsFile= popen(serverUserNameCommand,"r");
		if(!userNfsFile)
		{
		error("Error on openning the pipe to write the command to get .bof processes");
		return FAILURE;
		}
		
		//Prints the list of .bof files in a variable
		//reading from the pipe
		  printf(".BOF FILES IN SERVERUSERNAME'S FILE \n");
		while(fgets(bofNfsFiles,256,userNfsFile)!=NULL)
		{
		  //printing out information from the pipe
		
		  printf("%s\n",bofNfsFiles);
		  
		  //checking to see if the fpga is actually off
		
		}
	      }
	      
	      /*userNfsFile = fopen("/home/%s",serverUserName);*/
	      
	      /*#############################################################################################################
		//Second Check that the RHINO IP is actually still functional and available
	      ###############################################################################################################*/	  
	      
	    
	      //loop through the RHINO Stat table printing out the State of each RHINO
	      for(counter=0;counter<RhinoClusterCounter;counter++)
	      {
		if(rhinoNumber == rhinoStatTable[counter].rhinoNum)
		{
		  //Checking if the IpAddress is valid that the RHINO is not being utilized by another User
		  if(rhinoStatTable[counter].activeBit==BOARD_ACTIVE)
		  {
		    printf("RHINO IPADDRESS EXISTS, BUT IS BEING USED BY SOMEONE ELSE TRY ANOTHER RHINO");
		    return FAILURE;
		  }
		  else if (rhinoStatTable[counter].activeBit==BOARD_INACTIVE)
		  {
		    
		      printf("RHINO IPADDRESS DOES EXIST AND IS BEING PROGRAMMED");
		      
		      rhinoSelect(rhinoStatTable[counter].rhinoNum,getuid()); //this selects the RHINO and "locks" it for the user
		       /******************************************************************************************************************************
			* USING MESSAGE PACKET STRUCTURES AND TCP SEND AND RECIEVE AS APPOSSED TO READ AND WRITE
			* 
			****************************************************************************************************************************/
		        cm->messageID = htons(LOAD_BOF_FILE);
			cm->rhinoID = htons(rhinoNumber);
			cm->regName = bofProcessName; //HAVE TO SOME HOW CONVERT THIS INTO NUMBER TO HAVE A BINARY PROTOCOL
			cm->data = htons(0);
			  
			client_len =send(rhinoStatTable[counter].sockfd,cm,CLIENT_MESSAGE_HEADER,MSG_NOSIGNAL);
			  
			//ADD SOME ERROR HANDING CODE HERE
			  
			  
			server_len = recv(rhinoStatTable[counter].sockfd,sm,NTP_MAX_PACKET, MSG_DONTWAIT); //THINK ON THIS PART AND HOW YOU ARE GOING TO SET THE SIZE OF THE MESSAGE TO BE REICEVED
			  
			sm->rhinoBoard.nameNumber 	= ntohs(sm->rhinoBoard.nameNumber);
			sm->rhinoBoard.ipAddress	= sm->rhinoBoard.ipAddress;//HOW TO CONVERT THIS STRING INTO A NUMBER SO AS TO HAVE A BINARY PROTOCOL
			sm->rhinoBoard.busyBit	= ntohs(sm->rhinoBoard.busyBit);
			sm->rhinoBoard.registersAccessible->regName 		= sm->rhinoBoard.registersAccessible->regName;//HOW TO CONVERT THIS STRING TO A NUMBER TO HAVE A BINARY PROTOCOL
			sm->rhinoBoard.registersAccessible->readWriteBit 	= ntohs(sm->rhinoBoard.registersAccessible->readWriteBit);
			sm->rhinoBoard.registersAccessible->rhinoNameNumber = ntohs(sm->rhinoBoard.registersAccessible->rhinoNameNumber);
			  
			sm->data = ntohl(0); //IN THE WRITE MESSAGE THE RESPONSE FROM THE SERVER SHOULD HAVE NOTHING IN THE DATA ELEMENT
			sm->successBit = ntohs(sm->successBit);
		      
		      
		      
		      
/*************************************************************************************************************************************************************************************************************************************************/
	   
		      /***************************************************************************************************************************************************  
		    //NEED TO DEFINE MY OWN PACKET STRUCTURE CAUSE ALL PROTOCOLS HAVE THEIR OWN PACKET STRUCTURE
		    [Field(1)] Message ID(TELLS THE RHINO WHICH METHOD IS BEING CALLED)  [Field(2)] RHINOID [Field(3)] BOFPROCESSNAME [Field(4)] Data 
		    ***************************************************************************************************************************************************/
		    //How to generate the Message ID for the protocolPacket!!!!!!!
		   /* bzero(protocolPacket,256);
		    len = snprintf(protocolPacket,sizeof(protocolPacket),"[MID]%d,[RID]%d,[RN]%c,[D]%d",LOAD_BOF_FILE,rhinoStatTable[counter].rhinoNum,bofProcessName,0);
		    printf("protocolPacket from snprintf: %s\n",protocolPacket);
		    n = write(rhinoStatTable[counter].sockfd, protocolPacket, 256);
		    bzero(protocolPacket,256);
		    n = read(rhinoStatTable[counter].sockfd, protocolPacket, 256);
		    //first extract information from the packet information before setting the protocolPacket back to 0
		    
		    statusBit = 0; //this is just a dummy value for now
		    
		    bzero(protocolPacket,256);
		   */ 
		    
/*****************************************************************************************************************************************************************************************************************/		    
		    return sm->successBit;
		  }
		}
	      }
	      
	      
	    }


	  /*******************************************************************************************************************************************************************************************
	    *6) THIS METHOD ALLOWS FOR THE RHINOCLUSTERCLIENT SITTING ON THE PC SERVER TO CONNECT TO THE RHINO NODES GIVEN THIER STATIC IP ADDRESSES ON THE DHCP client
	  ********************************************************************************************************************************************************************************************/
	    void connectToRhino()
	    {
		
		FILE *fileIpAdd2 =NULL; //this is the file that the servers running on the rhinos update when they start hence it contains the number of rhinos that have registered with it
		char rhinoInfoChar[256];
		uint32_t rhinoInfo;
		char ping[255];
		char *rhinoIpString;
		int counter;
		int len;
		char numberOfClusterRhinoChar[256];
		char *portNoString;
		//RhStatTable *rhinoStatTable;
		
		
		//The Client will connect to the Server through A TEXT FILE with the RHINOs static IP addresses and socket Numbers
		  fileIpAdd2 = fopen("RHINOIPAdd.txt","r"); 
		     
		      
			if(fileIpAdd2==NULL)
			{
			  error("Failed to open fileIpAdd.txt file\n");
			
			}
			printf("GETTING PAST INITIAL FILE CHECK\n");
			
			//FIRST LINE IN THE FILEIPADD IS THE NUMBER OF RHINOS THAT ARE IN THE CLUSTER
			if (fgets(numberOfClusterRhinoChar,256,fileIpAdd2)!= NULL)
			    {
			      
				    numberOfClusterRhinos = atoi(numberOfClusterRhinoChar);
				    printf("NUMBER OF RHINOS ON THE CLUSTER AVAILABLE OR NOT ARE:[%d]\n",numberOfClusterRhinos);
			    }

			     
			    //NOW ACTUALLY ALLOCATING THE MEMORY FOR THE AVAILABLE RHINOS
			    rhinoStatTable = (RhStatTable*)malloc(numberOfClusterRhinos);
			 printf("THE SIZE OF RHINOSTATTABLE IS:[%d]\n",RhinoClusterCounter);
			//FILLING THE RHINOSTAT TABLE WITH INFORMATION FROM THE RHINOS AND CONNECTING THEM TO THE CLIENT
			  counter =0;
			    while(fgets(rhinoInfoChar,256,fileIpAdd2))
			    {
			       printf("IN THE WHILE LOOP FOR READING THE\n");
			      //Making sure that counter remains in the bounds of the rhinoStatTable
			      if((counter <= RhinoClusterCounter)||(RhinoClusterCounter==0))
			      {
				    
				    rhinoStatTable[counter].rhinoNum = counter;
				    RhinoClusterCounter++;
				    printf("THE RHINO IPAddress AND SOCKET NUMBER FROM FILEIP IS: %s\n",rhinoInfoChar);
				    
				    //THINK OF WAY OF SPLITTING THE RHINOIPADDRESS AND THE PORTNO TO BE USED ONCE READ FROM THE LINE
				    rhinoIpString= strtok(rhinoInfoChar," ");
				     printf("THE IP ADDRESS OF THE RHINO FROM RHINOSTATTABLE:[%s]\n",rhinoIpString);
				     rhinoStatTable[counter].rhinoIpAdd= rhinoIpString;
				     
				     portNoString=strtok(NULL," \n");
				    rhinoStatTable[counter].portno = atoi(portNoString); //getting socket from the console
				    printf("THE PORTNO THAT IS NOW AN INTERGER:[%d]\n",rhinoStatTable[counter].portno); 
				    
				    rhinoStatTable[counter].sockfd = socket (AF_INET, SOCK_STREAM, 0);
	      
				     
				   printf("THE SOCKFD OF THE RHINO IS:[%d]\n",rhinoStatTable[counter].sockfd);
				   printf("THE SIZE OF RHINOSTATTABLE AFTER EXECUTING LOOP IS:[%d]\n",RhinoClusterCounter);
				    
				      if(rhinoStatTable[counter].sockfd <0)
				      {
					error("Error openning socket"); //be not so dramatic when shutting down when not openning socket of one rhino
				      }
				      
				      //GETHOSTBYNAME USES A STRING OF AN IPADDRESS
					server = gethostbyname(rhinoStatTable[counter].rhinoIpAdd); //gets the name of a host on the internet (this function takes a names as an arguments and returns a pointer to a hostent containing information about that host
					printf("GETTING TO THIS STAGE\n");
					bzero((char *) &serv_addr, sizeof(serv_addr));//sets all values in the buffer to zero, takes two arguments a pointer to the buffer and the size of the buffer  
					serv_addr.sin_family = AF_INET;//AF_INET allows for an IPv4 internet protocol  
				      
					bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
					serv_addr.sin_port = htons(rhinoStatTable[counter].portno);
				    
					//ATTEMPTING TO CONNECT TO THE RHINO GIVEN THE IP ADDRESS AND UPDATING THE RHINOS ACTIVE BIT IN THE RHINOSTAT TABLE
					if(connect(rhinoStatTable[counter].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //this function returns 0 on success and -1 if its fails
					{
					   
					    rhinoStatTable[counter].activeBit = BOARD_INACTIVE;
					     printf("THE RHINO ACTIVEBIT IS [%d]\n",rhinoStatTable[counter].activeBit);
					     printf("THE RHINO IS ON THE CLUSTER BUT NOT ACTIVE\n");
					     error("Error connecting"); //THINK OF ANOTHER WAY TO TERMINATE WITHOUT BEING SO DRASTIC
					     counter++;
					}
					else
					{
					    connect(rhinoStatTable[counter].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
					    rhinoStatTable[counter].activeBit = BOARD_ACTIVE;
					    printf("THE RHINO IS ON THE CLUSTER AND IS ACTIVE\n");
					    
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
				error("COUNT IS OUT OF BOUNDS WITH RHINOSTATTABLE ");
			      }
			    }
			   
		//fclose(fileIpAdd2);
		 printf("GETTING OUT OF THE CONNECT METHOD\n");
		return;
	      }

	  /************************************************************************************************************************************************************************************************
	  * 7) THIS METHOD WILL ALLOW A USER TO CONNECT TO A PARTICULAR RHINO GIVEN AN IP ADDRESS. NB THE RHINO NEEDS TO ALREADY BE ON THE REGISTERED ON THE CLUSTER...
	  ************************************************************************************************************************************************************************************************/
	    void connectToSpecificRhino(int rhinoNumber,char *ipAddress,char *portno)
	    {
	    //NEED TO FIRST BE ABLE TO CHECK THAT THE RHINO NUMBER IS ACTUALLY STILL IN THE BOUNDS OF THE STATEMENT
	      printf("RHINONUMBER IN ConnectToSpecificRhino IS:[%d], IPADD: [%s], PORTNO:[%s]\n",rhinoNumber,ipAddress,portno);
	      
	      //MIGHT NEED TO CHECK FIRST IF THE RHINOSTATTABLE IS EMPTY???
	      
	      
		if(rhinoNumber >= RhinoClusterCounter)
		{
		    printf("THE SIZE OF THE RHINOSTATTABLE IS: [%d]\n",RhinoClusterCounter);
		     
		      rhinoStatTable[rhinoNumber].rhinoNum = rhinoNumber;
		     RhinoClusterCounter++;
		     printf("THE SIZE OF THE RHINOSTATTABLE AFTER INCREMENT IS: [%d]\n",RhinoClusterCounter);
		      //THINK OF WAY OF SPLITTING THE RHINOIPADDRESS AND THE PORTNO TO BE USED ONCE READ FROM THE LINE
		      rhinoStatTable[rhinoNumber].rhinoIpAdd= ipAddress;
		      portno= portno;
		      rhinoStatTable[rhinoNumber].portno = atoi(portno); //getting socket from the console
					
		      rhinoStatTable[rhinoNumber].sockfd = socket (AF_INET, SOCK_STREAM, 0);
		  
		      if(rhinoStatTable[rhinoNumber].sockfd <0)
		      {
			error("Error openning socket");
		      }
					  
			server = gethostbyname(rhinoStatTable[rhinoNumber].rhinoIpAdd); //gets the name of a host on the internet (this function takes a names as an arguments and returns a pointer to a hostent containing information about that host
			bzero((char *) &serv_addr, sizeof(serv_addr));//sets all values in the buffer to zero, takes two arguments a pointer to the buffer and the size of the buffer  
			serv_addr.sin_family = AF_INET;//AF_INET allows for an IPv4 internet protocol  
					  
			bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
			serv_addr.sin_port = htons(rhinoStatTable[rhinoNumber].portno);
					
			//ATTEMPTING TO CONNECT TO THE RHINO GIVEN THE IP ADDRESS AND UPDATING THE RHINOS ACTIVE BIT IN THE RHINOSTAT TABLE
			if(connect(rhinoStatTable[rhinoNumber].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //this function returns 0 on success and -1 if its fails
			{
			    rhinoStatTable[rhinoNumber].activeBit = BOARD_INACTIVE;
			     printf("FAILED TO CONNECT TO RHINO DUE TO NETWORK FAILURE\n");
			     printf("THE SIZE OF RHINOSTATTABLE AFTER THE CONNECTION ATTEMPT IS:[%d]\n",RhinoClusterCounter);
			      RhinoClusterCounter = RhinoClusterCounter-1;
			       printf("THE SIZE OF RHINOSTATTABLE AFTER A FAILED CONNECTION ATTEMPT IS:[%d]\n",RhinoClusterCounter);
			    error("Error connecting"); //THINK OF ANOTHER WAY TO TERMINATE WITHOUT BEING SO DRASTIC
			   
			    
			}
		      else
			{
			    connect(rhinoStatTable[rhinoNumber].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
			    rhinoStatTable[rhinoNumber].activeBit = BOARD_ACTIVE;
			    printf("RHINO CONNECTED AND IS ACTIVE\n");
			    
			}
		}
		else if(RhinoClusterCounter==0) //THIS MEANS THAT THE RHINOSTATTABLE HAS NOT YET BEEN CREATED
		{
		  
		  printf("GETTING INTO ELSEIF SECTION\n");
		       //NOW ACTUALLY ALLOCATING THE MEMORY FOR THE AVAILABLE RHINOS
		      rhinoStatTable = (RhStatTable*)malloc(1);
		      printf("THE SIZE OF THE RHINOSTATTABLE IS: [%d]\n",RhinoClusterCounter);
		     
		      rhinoStatTable[rhinoNumber].rhinoNum = rhinoNumber;
		      RhinoClusterCounter++;
		      //THINK OF WAY OF SPLITTING THE RHINOIPADDRESS AND THE PORTNO TO BE USED ONCE READ FROM THE LINE
		      rhinoStatTable[rhinoNumber].rhinoIpAdd= ipAddress;
		      portno= portno;
		      rhinoStatTable[rhinoNumber].portno = atoi(portno); //getting socket from the console
					
		      rhinoStatTable[rhinoNumber].sockfd = socket (AF_INET, SOCK_STREAM, 0);
		  
		      if(rhinoStatTable[rhinoNumber].sockfd <0)
		      {
			error("Error openning socket");
		      }
					  
			server = gethostbyname(rhinoStatTable[rhinoNumber].rhinoIpAdd); //gets the name of a host on the internet (this function takes a names as an arguments and returns a pointer to a hostent containing information about that host
			bzero((char *) &serv_addr, sizeof(serv_addr));//sets all values in the buffer to zero, takes two arguments a pointer to the buffer and the size of the buffer  
			serv_addr.sin_family = AF_INET;//AF_INET allows for an IPv4 internet protocol  
					  
			bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
			serv_addr.sin_port = htons(rhinoStatTable[rhinoNumber].portno);
					
			//ATTEMPTING TO CONNECT TO THE RHINO GIVEN THE IP ADDRESS AND UPDATING THE RHINOS ACTIVE BIT IN THE RHINOSTAT TABLE
			if(connect(rhinoStatTable[rhinoNumber].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //this function returns 0 on success and -1 if its fails
			{
			    rhinoStatTable[rhinoNumber].activeBit = BOARD_INACTIVE;
			     printf("FAILED TO CONNECT TO RHINO DUE TO NETWORK FAILURE\n");
			      printf("THE SIZE OF RHINOSTATTABLE AFTER THE CONNECTION ATTEMPT IS:[%d]\n",RhinoClusterCounter);
			      RhinoClusterCounter = RhinoClusterCounter-1;
			    error("Error connecting"); //THINK OF ANOTHER WAY TO TERMINATE WITHOUT BEING SO DRASTIC
			   
			    
			}
		      else
			{
			    connect(rhinoStatTable[rhinoNumber].sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
			    rhinoStatTable[rhinoNumber].activeBit = BOARD_ACTIVE;
			    printf("RHINO CONNECTED AND IS ACTIVE\n");
			    
			}
		}
		else
		{
		  error("RHINONUMBER IS OUT OF BOUNDS WITH RHINOSTATTABLE\n ");
		}
	    
	  }
	  /************************************************************************************************************************************************************************************************
	  *8) THIS METHOD ALLOWS A USER TO SELECT AND "LOCK" THE RHINO during duration of the program running
	  ****************************************************************************************************************************************************************************************************/

	    int rhinoSelect (int rhinoNum,int programUserId) //MIGHT LOOK INTO CHANGING THIS TO TAKE A NUMBER RATHER THAN THE IPADDRESS
	    {
	      int counter;
	      
	      printf("THE RHINONUMBER IS TO SELECT IS: [%d]\n",rhinoNum);
	      printf("THE PROGRAMUSERID IS TO SELECT IS: [%d]\n",programUserId);
	      printf("THE RHINOCLUSTERCOUNTER IS: [%d]\n",RhinoClusterCounter);
	      
	      
	      for(counter=0; counter<RhinoClusterCounter;counter++)
	      {
		printf("THE RHINOSTATTABLE RHINO NUMBER IS: %d\n",rhinoStatTable[counter].rhinoNum);
		  if(rhinoNum == rhinoStatTable[counter].rhinoNum)
		  {
		    printf("RHINO %d IS AVAILABLE ON THE CLUSTER\n",rhinoStatTable[counter].rhinoNum);
		    printf("THE RHINO ACTIVE BIT IS: %s\n",rhinoStatTable[counter].activeBit);  
		    //First Check if the RHINO is active on the clusterStatus
		    if(rhinoStatTable[counter].activeBit==BOARD_ACTIVE)
		    {
		      //Lock the RHINO from use from other users
		      printf("RHINO %d IS AVAILABLE ON THE CLUSTER BUT IS BEING USED, TRY ANOTHER RHINO\n",rhinoStatTable[counter].rhinoNum);
		      return FAILURE;
		    }
		    else if(rhinoStatTable[counter].activeBit==BOARD_INACTIVE)
		    {
		      
		      //HOW DO YOU KNOW WHICH USER HAS SELECTED A PARTICULAR RHINO????
		      rhinoStatTable[counter].programUserId = programUserId;
		      rhinoStatTable[counter].activeBit = BOARD_ACTIVE;
		      printf("RHINO %d IS AVAILABLE ON THE CLUSTER AND HAS BEEN SELECTED\n",rhinoStatTable[counter].rhinoNum);
		      return SUCCESS;
		    }
		  }
		  printf("GETTING INTO FOR LOOP\n");
		}
		
		    printf("RHINO [%d] IS NOT AVAILABLE ON THE CLUSTER, TRY ANOTHER RHINO\n",rhinoNum);
		    return FAILURE;
	      
		
	      }
	  
	  
	  /*************************************************************************************************************************************************************************************************
	  *9) RELEASES A RHINO AFTER A USER IS DONE USING IT (WORKS FINE)
	  ****************************************************************************************************************************************************************************************************/
	    int rhinoRelease(int rhinoNumber,int programUserId)
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
			if(rhinoStatTable[counter].activeBit==BOARD_ACTIVE)
			{
			  //RELEASING THE RHINO FOR USE BACK ON THE CLUSTER
			  printf("RHINO %d IS AVAILABLE ON THE CLUSTER AND IS BEING RELEASED\n",rhinoStatTable[counter].rhinoNum);
			  rhinoStatTable[counter].activeBit = BOARD_INACTIVE;
			  printf("THE NEW ACTIVE BIT FOR FOR THE RHINO IS: [%d]\n",rhinoStatTable[counter].activeBit);
			  return SUCCESS;
			}
			
		    }
		    
		    else if(rhinoStatTable[counter].programUserId == programUserId)
		    {
		      printf("THIS USER DOES NOT ACCESS TO RHINO [%d] AT THIS POINT\n",rhinoStatTable[counter].rhinoNum);
		      return FAILURE;
		    }
		    
		    else if(rhinoStatTable[counter].activeBit==BOARD_INACTIVE)
		    {
			//HOW DO YOU KNOW WHICH USER HAS SELECTED A PARTICULAR RHINO????
			printf("RHINO [%d] IS AVAILABLE ON THE CLUSTER BUT IS NOT ACTIVE\n",rhinoStatTable[counter].rhinoNum);
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
	    int regWrite(int rhinoNumber,char *regName,unsigned int data,int programUserId)
	    {
	      int client_len,server_len;
	      int counter;
	      unsigned long registerData;
	      struct client_msg *cm, writeMessage; //THIS CREATES AN OBJECT OF THE MESSAGE
	      struct server_msg *sm, readMessage;
	  
	      
	      sm = &readMessage;
	      cm = &writeMessage;
	    
	      for(counter=0; counter<RhinoClusterCounter;counter++)
	      {
		//1) First IF statement checking to see if named RHINO is in the cluster
		if(rhinoStatTable[counter].rhinoNum == rhinoNumber)
		{
		  printf("THE RHINO IS IN THE CLUSTER\n");
		  
		  //2) CHECKING IF USER HAS ACCESS TO THE RHINO AND THE RHINO IS NOT BEING USED
		  if((rhinoStatTable[counter].activeBit==BOARD_ACTIVE)&&(rhinoStatTable[counter].programUserId==programUserId))
		  {
		    printf("THE RHINO IS IS BEING USED BY YOU\n");
		    //3) CHECKING IF THE RHINO IS PROGRAMMED ?????? POSSIBLY USE THE RHINOS BUSY BIT
		    if(rhinoStatTable[counter].rhinoBoard->busyBit == FPGA_PROGRAMMED)
		    {
			  printf("THE RHINO BOARD IS PROGRAMMED AND AN ATTEMPT TO WRITE TO A REGISTER WILL START\n");
			  /******************************************************************************************************************************
			   * USING MESSAGE PACKET STRUCTURES AND TCP SEND AND RECIEVE AS APPOSSED TO READ AND WRITE
			   * 
			   ****************************************************************************************************************************/
			  cm->messageID = htons(REGISTER_WRITE);
			  cm->rhinoID   = htons(rhinoStatTable[counter].rhinoNum);
			  cm->regName   = cm->regName;//HOW TO CHANGE THE REGISTER NAME TO A STRING
			  cm->data      =   htonl(data);

			  client_len = send(rhinoStatTable[counter].sockfd, cm, CLIENT_MESSAGE_HEADER, MSG_NOSIGNAL); //THINK OF ANOTHER WAY OF GETTING THE SIZE OF THE PACKET TO BE SENT
			  
			  //ADD SOME ERROR HANDLING HERE FOR THE SEND
			  
			  server_len = recv(rhinoStatTable[counter].sockfd,sm,NTP_MAX_PACKET, MSG_DONTWAIT); //THINK ON THIS PART AND HOW YOU ARE GOING TO SET THE SIZE OF THE MESSAGE TO BE REICEVED
			  
			  sm->rhinoBoard.nameNumber 	= ntohs(sm->rhinoBoard.nameNumber);
			  sm->rhinoBoard.ipAddress	= sm->rhinoBoard.ipAddress;//HOW TO CONVERT THIS STRING INTO A NUMBER SO AS TO HAVE A BINARY PROTOCOL
			  sm->rhinoBoard.busyBit	= ntohs(sm->rhinoBoard.busyBit);
			  sm->rhinoBoard.registersAccessible->regName 		= sm->rhinoBoard.registersAccessible->regName;//HOW TO CONVERT THIS STRING TO A NUMBER TO HAVE A BINARY PROTOCOL
			  sm->rhinoBoard.registersAccessible->readWriteBit 	= ntohs(sm->rhinoBoard.registersAccessible->readWriteBit);
			  sm->rhinoBoard.registersAccessible->rhinoNameNumber = ntohs(sm->rhinoBoard.registersAccessible->rhinoNameNumber);
			  
			  sm->data = ntohs(0); //IN THE WRITE MESSAGE THE RESPONSE FROM THE SERVER SHOULD HAVE NOTHING IN THE DATA ELEMENT
			  sm->successBit = ntohs(sm->successBit);
			  
			  
			  //ADD SOME ERROR HANDLING CODE HERE AS WELL
			  
			  	  
/**********************************************************************************************************************************************************************/	  
			  
			/***********************************************************************************************************************************************************************************************************
			REMEMBER PACKET STRUCTURE: 
			[Field(1)] int Message ID(TELLS THE RHINO WHICH METHOD IS BEING CALLED)  [Field(2)] int RHINOID [Field(3)] char [] RegName [Field(4)] unsigned int Data 
			*************************************************************************************************************************************************************************************************************/
			//How to generate the Message ID for the protocolPacket!!!!!!!
			/*bzero(protocolPacket,256);
			len = snprintf(protocolPacket,sizeof(protocolPacket),"[MID]%d,[RID]%d,[RN]%c,[D]%d",REGISTER_WRITE,rhinoNumber,regName,data);
			printf("protocolPacket from snprintf: %s\n",protocolPacket);
			n = write(rhinoStatTable[counter].sockfd, protocolPacket, 256);
			bzero(protocolPacket,256);
			n = read(rhinoStatTable[counter].sockfd, protocolPacket, 256);
			bzero(protocolPacket,256);
		      
			//First Need to decode the protocolPacket
			//Use strchr to decode protocolPacket
			char *stringMessageId = strchr(protocolPacket,"[MID]");
			char *stringRhinoId = strchr(protocolPacket,"[RID]");//NEED TO SEE IF IAM NOT WASTING SPACE WITH THIS STRING
			char *registerName = strchr(protocolPacket,"[RN]");
			char *stringRegData = strchr(protocolPacket,"[D]");
			
			unsigned long SeverMessageId =strtol(stringMessageId,NULL,10);
			unsigned long rhinoId = strtol(stringRhinoId,NULL,10);
			registerData = strtol(stringRegData,NULL,10);*/
			  
/**********************************************************************************************************************************************************************************************************/			
			
			  return sm->successBit;
			      
		      // break; 
		    }
		    else if(rhinoStatTable[counter].rhinoBoard->busyBit == FPGA_UNPROGRAMMED)
		    {
		      printf("THE RHINO IS NOT PROGRAMMED WITH A BOF FILE HENCE NO WRITE CAN OCCUR\n");
		      return FAILURE;
		    }
		    
		  }
		  //2) CHECKING IF USER HAS ACCESS TO THE RHINO AND THE RHINO IS NOT BEING USED
		  else
		  {
		    if(rhinoStatTable[counter].activeBit==BOARD_INACTIVE)
		    {
		      printf("RHINO IS INACTIVE AND HENCE CANT NOT BE ACCESSED\n"); 
		      return FAILURE;
		    }
		    else if(rhinoStatTable[counter].programUserId==programUserId)
		    {
		      printf("RHINO IS ACTIVE BUT IS NOT ASSIGNED TO THIS USER\n");
		      return FAILURE;
		    }
		  }	  
		}
		
	      }

		  error("THE RHINO WITH THE NUMBER %d, WAS NOT FOUND ON THE CLUSTER");
		  return FAILURE;
	    
	    
	  }
	  
	  /*******************************************************************************************************************************************************************************************************
	  *11) RegRead METHOD ALLOWS A USER TO BE ABLE TO READ FROM A GIVEN RHINOCLUSTERCLIENT
	  ********************************************************************************************************************************************************************************************************/
	    unsigned int regRead(int rhinoNumber,char *regName,int programUserId)
	    {
	    
	      int client_len,server_len;
	      int len;
	      unsigned long registerData;
	      int counter;
	      struct client_msg *cm, writeMessage;
	      struct server_msg *sm, readMessage;
	      cm = &writeMessage;
	      sm = &readMessage;
	      
	      //MIGHT NEED TO CHECK IF THE BOARD IS STILL CONNECTED 
	      
	      
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
		  if((rhinoStatTable[counter].activeBit==BOARD_ACTIVE)&&(rhinoStatTable[counter].programUserId==programUserId))
		  {
		    printf("THE RHINO IS IS BEING USED BY YOU\n");
		    //3) CHECKING IF THE RHINO IS PROGRAMMED ?????? POSSIBLY USE THE RHINOS BUSY BIT
		    if(rhinoStatTable[counter].rhinoBoard->busyBit == FPGA_PROGRAMMED)
		    {
			  printf("THE RHINO BOARD IS PROGRAMMED AND AN ATTEMPT TO READ A REGISTER WILL START\n");
			  
			   /******************************************************************************************************************************
			   * USING MESSAGE PACKET STRUCTURES AND TCP SEND AND RECIEVE AS APPOSSED TO READ AND WRITE
			   * 
			   ****************************************************************************************************************************/
			  
			  cm->messageID = htons(REGISTER_READ);
			  cm->rhinoID = htons(rhinoNumber);
			  cm->regName =cm->regName; //HOW TO CONVERT A STRING TO A NUMBER SO ITS A BINARY PROTOCOL
			  cm->data = htons(0);
			  
			  client_len =send(rhinoStatTable[counter].sockfd,cm,CLIENT_MESSAGE_HEADER,MSG_NOSIGNAL);
			  
			  //ADD SOME ERROR HANDING CODE HERE
			  
			  
			  server_len = recv(rhinoStatTable[counter].sockfd,sm,NTP_MAX_PACKET, MSG_DONTWAIT); //THINK ON THIS PART AND HOW YOU ARE GOING TO SET THE SIZE OF THE MESSAGE TO BE REICEVED
			  
			  sm->rhinoBoard.nameNumber 	= ntohs(sm->rhinoBoard.nameNumber);
			  sm->rhinoBoard.ipAddress	= sm->rhinoBoard.ipAddress;//HOW TO CONVERT THIS STRING INTO A NUMBER SO AS TO HAVE A BINARY PROTOCOL
			  sm->rhinoBoard.busyBit	= ntohs(sm->rhinoBoard.busyBit);
			  sm->rhinoBoard.registersAccessible->regName 		= sm->rhinoBoard.registersAccessible->regName;//HOW TO CONVERT THIS STRING TO A NUMBER TO HAVE A BINARY PROTOCOL
			  sm->rhinoBoard.registersAccessible->readWriteBit 	= ntohs(sm->rhinoBoard.registersAccessible->readWriteBit);
			  sm->rhinoBoard.registersAccessible->rhinoNameNumber = ntohs(sm->rhinoBoard.registersAccessible->rhinoNameNumber);
			  
			  sm->data = ntohl(sm->data); //IN THE WRITE MESSAGE THE RESPONSE FROM THE SERVER SHOULD HAVE NOTHING IN THE DATA ELEMENT
			  sm->successBit = ntohs(0);
			  
			  			  
/*********************************************************************************************************************************************************************************************************************************************************************/			  
			  
			  /************************************************************************************************************************************************************************************************************
			  REMEMBER PACKET STRUCTURE: 
			  [Field(1)] int Message ID(TELLS THE RHINO WHICH METHOD IS BEING CALLED)  [Field(2)] int RHINOID [Field(3)] char [] RegName [Field(4)] unsigned int Data 
			  *************************************************************************************************************************************************************************************************************/
			  
			/*  bzero(protocolPacket,256);
			  len = snprintf(protocolPacket,sizeof(protocolPacket),"[MID]%d,[RID]%d,[RN]%c,[D]%d",REGISTER_READ,rhinoNumber,regName,0);
			  printf("protocolPacket from snprintf: %s\n",protocolPacket);
			  n = write(rhinoStatTable[counter].sockfd, protocolPacket, 256);
			  bzero(protocolPacket,256);
			  n = read(rhinoStatTable[counter].sockfd, protocolPacket, 256);
			  bzero(protocolPacket,256);
			  
			  //First Need to decode the protocolPacket
			  //Use strchr to decode protocolPacket
			  char *stringMessageId= strchr(protocolPacket,"[MID]");
			  char *stringRhinoId = strchr(protocolPacket,"[RID]");//NEED TO SEE IF IAM NOT WASTING SPACE WITH THIS STRING
			  char *registerName = strchr(protocolPacket,"[RN]");
			  char *stringRegData = strchr(protocolPacket,"[D]");
			  
			  unsigned long ServerMessageId =strtol(stringMessageId,NULL,10);
			  unsigned long rhinoId = strtol(stringRhinoId,NULL,10);
			  registerData = strtol(stringRegData,NULL,10); 
			  */
			
			  
			  
/***************************************************************************************************************************************************************************************************************************************/			  
			  return sm->data;
				
			// break;
		    }
		    else if(rhinoStatTable[counter].rhinoBoard->busyBit == FPGA_UNPROGRAMMED)
		    {
		      printf("THE RHINO IS NOT PROGRAMMED WITH A BOF FILE HENCE NO WRITE CAN OCCUR\n");
		      return FAILURE;
		    }
		    
		  }
		}
		
	      }

		  error("THE RHINO WITH THE NUMBER %d, WAS NOT FOUND ON THE CLUSTER");
		  return FAILURE;
	    
	    }
	    
	  /***************************************************************************************************************************************************************************************************************
	    * 12) KILL BOF PROCESS: THIS METHOD KILLS A BOF PROCESS RUNNING ON THE RHINO BOARD
	    * ************************************************************************************************************************************************************************************************************/
	    int killBofProcess(int rhinoNumber, char *bofProcessName)
	    {
	      int counter;
	      char *data = NULL;
	      int len;
	      
	      for( counter=0; counter<RhinoClusterCounter;counter++)
	      {
		if(rhinoNumber == rhinoStatTable[counter].rhinoNum)
		{
		  printf("FOUND THE RHINO [%d] TO KILL THE PROCESS",rhinoNumber);
		  
		  /*****************************************************************************************************************************
		   * NEED TO MAKE SURE THAT THE RHINO IS PROGRAMMED BEFORE YOU CAN PERFORM THIS OPERATION 
		   ***************************************************************************************************************************************/
		  
		  
		  //PACKETIZE THE DATA
		  /***********************************************************************************************************************************************************************************************************
		  REMEMBER PACKET STRUCTURE: 
		  [Field(1)] int Message ID(TELLS THE RHINO WHICH METHOD IS BEING CALLED)  [Field(2)] int RHINOID [Field(3)] char [] RegName [Field(4)] unsigned int Data 
		  *************************************************************************************************************************************************************************************************************/
		    //How to generate the Message ID for the protocolPacket!!!!!!!
		    bzero(protocolPacket,256);
		    len = snprintf(protocolPacket,sizeof(protocolPacket),"[MID]%d,[RID]%d,[RN]%c,[D]%d",KILL_BOF_PROCESS,rhinoNumber,bofProcessName,data);
		    printf("protocolPacket from snprintf: %s\n",protocolPacket);
		    n = write(rhinoStatTable[counter].sockfd, protocolPacket, 256);
		    bzero(protocolPacket,256);
		    n = read(rhinoStatTable[counter].sockfd, protocolPacket, 256);
		    bzero(protocolPacket,256);
		  
		  return SUCCESS;
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
	   void disconnectToRhino(int rhinoNumber, char *ipAddress)
	   {
	      int counter;
	      int len;
	      
	      for(counter=0; counter<(sizeof(rhinoStatTable)/sizeof(RhStatTable));counter++)
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
