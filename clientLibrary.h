/*
 * clientLibraryHeader.h
 *
 *  Created on: Nov 9, 2012
 *      Author: valerie
 */

#ifndef CLIENTLIBRARYHEADER_H_
#define CLIENTLIBRARYHEADER_H_
#define MAX_STRING_LEN 9
#include  <stdint.h>



typedef struct {

	char regName[32];
	uint16_t readWriteBit;
	uint32_t rhinoNameNumber;
	unsigned int size;
	
}RegisterT;

//characteristics of a rhinoboard
typedef struct {
	uint32_t nameNumber;
	char ipAddress[16];
	uint32_t busyBit;
	uint32_t successBit;
	char errorMsg[256];
	RegisterT registersAccessible;
	
}RhinoT;


//defining a structure that holds the Rhino Status Table
//defining a structure that holds the Rhino Status Table
typedef struct RhStatTable{
	uint32_t rhinoNum;
	//think about the kind of variable that the ipaddress is going to be for now ill make it into an int but ill think on this one for a bit more
	char rhinoIpAdd[16]; 
	uint32_t activeBit;
	uint32_t programUserId;
	uint32_t sockfd;
	int portno;
	RhinoT rhinoBoard;
	
}RhStatTableT;



struct client_msg{
	uint32_t messageID;
	char user_name[16];
	uint32_t rhinoID;
	char reg_name[32];
	uint16_t data;
  
}__attribute__((packed));

typedef struct client_msg client_msg_T;

struct server_msg{
	RhinoT rhinoBoard;
	uint32_t successBit;
	uint16_t data;
  
}__attribute__((packed));

typedef struct server_msg server_msg_T;

struct reg_msg{
      RegisterT registersAccessible; /*This */
      
}__attribute__((packed));

typedef struct reg_msg reg_msg_T;





//List of methods available to developer
int numRhinos(); //1
struct RegisterT  listRegisters(); //2)this will list all of the available registers on the cluster via iterations
RegisterT  *listDeviceRegisters(uint32_t rhinoNameNumber, char *bofProcessName, char *serverUserName); //3)this will list the specific registers in just one rhino
void clusterStatus(); //4) this method prints out the status of the all the RhinoNodes on the cluster
int loadBofFile(uint32_t rhinoNumber, char *bofProcessName, char *serverUserName,uint32_t programUserId); //5) this method will essence assume that all RHINOs have access to the same file system hence just tell a particular RHINO to start running the .bof file
int regWrite(uint32_t rhinoNumber,char *regName,uint16_t data,uint32_t programUserId); //6) returns a 0 if its been successful in writing the to register and -1 if it hasnt
uint16_t regRead(uint32_t rhinoNumber,char *regName,uint32_t programUserId);//7) returns the data from the named register
int connectToRhino(); //8) Connects the RHINOClusterClient to the RHINOClusterServer Sitting on the RHINO selects IP address from some table ..thinking on this one
int rhinoSelect (uint32_t rhinoNumber,uint32_t programUserId); //9) this allows a user to be able to pick a rhino and lock it from the rest of the cluster (returns 0 when successful and -1 when unsuccessful)
int rhinoRelease(uint32_t rhinoNumber,uint32_t programUserId); //10) Releases a RHINO after a user is done using it back to become available to the user
int connectToSpecificRhino(char *ipAddress,char *portno);//11) 
void disconnectToRhino(uint32_t rhinoNumber, char *ipAddress); //12)
int killBofProcess(uint32_t rhinoNumber, char *bofProcessName, uint32_t programUserId); //13)This Method kill a bof process running on the RHINO
uint32_t parseIPV4string(char *ipAddress); //THIS METHOD CONVERTS A STRING INTO UNSIGNED 32bit int
void shutDownSystem(); //This method will just be used to safely shutdown the system/API usage
int loadBofFile2(uint32_t rhinoNumber, char *bofProcessName, char *serverUserName,uint32_t programUserId);
void initRhinoBoard(int rhinoNumber, char *ipAddress);
void clientPacketTest(int rhinoNumber, int testTimes, uint16_t dataToTest);
void serverPacketTest(int rhinoNumber, int testTimes);


#endif /* CLIENTLIBRARYHEADER_H_ */
