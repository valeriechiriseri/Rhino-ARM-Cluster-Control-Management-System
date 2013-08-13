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


//List of methods available to developer
int numRhinos(); //1
struct Register * listRegisters(); //2)this will list all of the available registers on the cluster via iterations
struct Register * listDeviceRegisters(int rhinoNameNumber); //3)this will list the specific registers in just one rhino
void clusterStatus(); //4) this method prints out the status of the all the RhinoNodes on the cluster
int loadBofFile(uint32_t rhinoNumber, char *bofProcessName, char *serverUserName); //5) this method will essence assume that all RHINOs have access to the same file system hence just tell a particular RHINO to start running the .bof file
int regWrite(uint32_t rhinoNumber,char *regName,uint32_t data,uint32_t programUserId); //6) returns a 0 if its been successful in writing the to register and -1 if it hasnt
unsigned int regRead(uint32_t rhinoNumber,char *regName,uint32_t programUserId);//7) returns the data from the named register
void connectToRhino(); //8) Connects the RHINOClusterClient to the RHINOClusterServer Sitting on the RHINO selects IP address from some table ..thinking on this one
int rhinoSelect (uint32_t rhinoNumber,uint32_t programUserId); //9) this allows a user to be able to pick a rhino and lock it from the rest of the cluster (returns 0 when successful and -1 when unsuccessful)
int rhinoRelease(uint32_t rhinoNumber,uint32_t programUserId); //10) Releases a RHINO after a user is done using it back to become available to the user
void connectToSpecificRhino(uint32_t rhinoNumber, char *ipAddress,char *portno);//11) 
void disconnectToRhino(uint32_t rhinoNumber, char *ipAddress); //12)
int killBofProcess(uint32_t rhinoNumber, char *bofProcessName); //13)This Method kill a bof process running on the RHINO
uint32_t parseIPV4string(char *ipAddress); //THIS METHOD CONVERTS A STRING INTO UNSIGNED 32bit int
void shutDownSystem(); //This method will just be used to safely shutdown the system/API usage


//characteristics of a register that is accessible in the rhinoboard
typedef struct {

	char regName[16];
	uint32_t readWriteBit;
	uint32_t rhinoNameNumber;
	uint32_t size;

}Register;

//characteristics of a rhinoboard
typedef struct {

	uint32_t nameNumber;
	char ipAddress[16];
	uint32_t busyBit;
	Register *registersAccessible;
}Rhino;

//defining a structure that holds the Rhino Status Table
typedef struct {
	int rhinoNum;
	//think about the kind of variable that the ipaddress is going to be for now ill make it into an int but ill think on this one for a bit more
	char rhinoIpAdd[16]; 
	int activeBit;
	int programUuserId;
	int portno;
	Rhino *rhinoBoard;

	}RhStatTable;


struct client_msg{
	uint32_t messageID;
	uint32_t rhinoID;
	char regName[16];
	uint32_t data;
  
}__attribute__((packed));


struct server_msg{
	Rhino rhinoBoard;
	uint32_t successBit;
	uint32_t data;
  
}__attribute__((packed));
	
	

#endif /* CLIENTLIBRARYHEADER_H_ */
