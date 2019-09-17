// Demo: Dual CAN-BUS Shields, Data Pass-through
// Written by: Cory J. Fowler
// January 31st 2014
// This examples the ability of this library to support more than one MCP2515 based CAN interface.

#include <mcp_can.h>
#include <SPI.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];                        // Array to store serial string

#define CAN0_INT 2                              // Set CAN0 INT to pin 2
MCP_CAN CAN0(10);                               // Set CAN0 CS to pin 10

#define CAN1_INT 3                              // Set CAN1 INT to pin 2
MCP_CAN CAN1(9);                               //  Set CAN1 CS to pin 9



void setup()
{
  Serial.begin(115200);
  
  // Initialize MCP2515 running at 16MHz with a baudrate of 1000kb/s and the masks and filters disabled.
  while(CAN0.begin(MCP_ANY, CAN_1000KBPS, MCP_16MHZ) != CAN_OK)
    {
      Serial.println("MCP2515 0 Initialize Failed!");
      delay(100);
    }

    Serial.println("MCP2515 0 Initialized Successfully");
  
  CAN0.setMode(MCP_NORMAL);                           // Set operation mode to normal so the MCP2515 sends acks to received data.
  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input

delay(200);

  // Initialize MCP2515 running at 16MHz with a baudrate of 1000kb/s and the masks and filters disabled.
  while(CAN1.begin(MCP_ANY, CAN_1000KBPS, MCP_16MHZ) != CAN_OK)
    {
      Serial.println("MCP2515 1 Initialize Failed!");
      delay(100);
    }

    Serial.println("MCP2515 1 Initialized Successfully");
  
  CAN1.setMode(MCP_NORMAL);                           // Set operation mode to normal so the MCP2515 sends acks to received data.
  pinMode(CAN1_INT, INPUT);                            // Configuring pin for /INT input

}

void loop(){  

//CAN0 Read when the interupt pin is pulled LOW

  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);            // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)           // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  
    Serial.print(msgString);
  
    if((rxId & 0x40000000) == 0x40000000){          // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
      }
    }
        
    Serial.println();
  }

  //CAN1 Read when the interupt pin is pulled LOW

  if(!digitalRead(CAN1_INT))                        // If CAN1_INT pin is low, read receive buffer
  {
    CAN1.readMsgBuf(&rxId, &len, rxBuf);            // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)           // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  
    Serial.print(msgString);
  
    if((rxId & 0x40000000) == 0x40000000){          // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
      }
    }
        
    Serial.println();
  }

}
