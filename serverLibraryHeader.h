//characteristics of a register that is accessible in the rhinoboard
typedef struct Register{
	char regName[32];
	uint16_t readWriteBit;
	uint32_t rhinoNameNumber;
	unsigned int size;
	
}RegisterT;

//characteristics of a rhinoboard
typedef struct Rhino{
	uint32_t nameNumber;
	char ipAddress[16];
	uint32_t busyBit;
	uint32_t successBit;
	char errorMsg[256];
	RegisterT registersAccessible;
	
}RhinoT;

//defining a structure that holds the Rhino Status Table
typedef struct RhStatTable{
	uint32_t rhinoNum;
	//think about the kind of variable that the ipaddress is going to be for now ill make it into an int but ill think on this one for a bit more
	char *rhinoIpAdd; 
	uint32_t activeBit;
	uint32_t programUuserId;
	RhinoT *rhinoBoard;
	
}RhStatTableT;


struct client_msg{
	uint32_t messageID;    /*Specifies the API method to be called in on the ARM Server Application*/
	uint8_t userName[16];  /*This is the users Name on the Control Computer that indicates the users folder*/
	uint32_t rhinoID;	/*This is a unique number allocated to each individual RHINO*/
	uint8_t regName[32];   /*This specifies the .bof process register to read or write OR a bof process NAME*/
	uint16_t data;		/*This is the data to be written to a given register*/
}__attribute__((packed));

typedef struct client_msg client_msg_T;


struct server_msg{
	RhinoT rhinoBoard;
	uint32_t successBit;
	uint16_t data;
	//char errorMsg[256];
  
}__attribute__((packed));

typedef struct server_msg server_msg_T;


struct reg_msg{
      RegisterT registersAccessible;
      
}__attribute__((packed));

typedef struct reg_msg reg_msg_T;

struct server_state
{
	unsigned int s_magic;
	int master_socket;
	int client_socket;
};

typedef struct server_state server_stateT;

struct network_info
{
  char macAddress[18];
  char ipAddress[16];
  
};

typedef struct network_info network_infoT;





void initializeMethod(RhinoT *rhinoDevicePtr, RegisterT *rhinoRegistersPtr);
network_infoT *getIpAddress(uint32_t nameNumber);
int connect_sockets(int portno, RhinoT *rhinoDevicePtr, RegisterT *rhinoRegistersPtr, server_stateT *st);
int rhinoProcessing(RhinoT *rhinoDevicePtr, RegisterT *rhinoRegistersPtr, server_stateT *st);
RhinoT *startBofFile(uint32_t nameNumber,uint8_t *serverUserName, uint8_t *bofProcessName, RhinoT *rhinoDevicePtr, server_stateT *st);
RhinoT *regWrite(uint32_t nameNumber, uint8_t *registerName, uint16_t data, RhinoT *rhinoDevicePtr, server_stateT *st);
uint16_t regRead(uint32_t nameNumber,uint8_t *registerName, RhinoT *rhinoDevicePtr, server_stateT *st); 
RegisterT *listRegisters(uint32_t nameNumber, uint8_t *serverUserName, uint8_t *bofProcessName, server_stateT *st);
RhinoT *stopBofFile(uint32_t nameNumber,uint8_t *bofProcessName, RhinoT *rhinoDevicePtr, server_stateT *st);



