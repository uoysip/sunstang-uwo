/*Description: Code for the Auxillary Control Unit
 *Version: 2
 *Author: Steven Lawrence
 *Date: 01/06/19
 */
 
//Include libraries
#include <mcp_can.h>                  //library for CAN communication
#include <SPI.h>                      //library for SPI

//Initialize pins
const int mainSwGate = 7;             //OUTPUT: Pin for Main-Switched MOSFET
const int suppSwGate = 8;             //OUTPUT: Pin for Supp-Switched MOSFET
const int SPI_CS_PIN = 10;            //OUTPUT: CAN chip select pin 

MCP_CAN CAN(SPI_CS_PIN);              //Initialize CAN object

//CAN IDs
unsigned long Battery_OK_ID = 0x10;   //CAN ID for battery status message

//CAN receive buffers
byte BattOk[8];                       //Buffer for battery ok message

bool batteryOk=0;                     //Switched power line MOSFET selectin variable

void setup() {
//Initialize Serial Monitor
  Serial.begin(115200);                           //Baudrate: 115200

//Initialize CAN bus 
  while(CAN_OK != CAN.begin(CAN_500KBPS)){        //Baudrate: 500K
    Serial.println("CAN BUS init fail");
    Serial.println("Init CAN BUS again");
    delay(100);
  }Serial.println("CAN BUS init ok!");            //CAN bus initialization successful

//pinmodes 
  pinMode(mainSwGate, OUTPUT);                    //Main-Switched MOSFET gate output
  pinMode(suppSwGate, OUTPUT);                    //Supp-Switched MOSFET gate output
  
//initialize CAN masks
  CAN.init_Mask(0,0,0x7ff);                       //Mask 1: 0111 1111 1111
  CAN.init_Mask(1,0,0x7ff);                       //Mask 2: 0111 1111 1111

//initialize CAN filters
  CAN.init_Filt(0,0,0x7D1);                       //Filter 1: 0111 1101 0001
  CAN.init_Filt(1,0,0x7D1);                       //Filter 2: 0111 1101 0001
  CAN.init_Filt(2,0,0x7D1);                       //Filter 3: 0111 1101 0001
  CAN.init_Filt(3,0,0x7D1);                       //Filter 4: 0111 1101 0001
  CAN.init_Filt(4,0,0x7D1);                       //Filter 5: 0111 1101 0001
  CAN.init_Filt(5,0,0x7D1);                       //Filter 6: 0111 1101 0001

  digitalWrite(suppSwGate, HIGH);                 //Turn on the supp-Switched MOSFET
}

void loop() {

//Setup variables for receiving CAN messages
  unsigned char lenReceive = 0;                   //Data length
  unsigned char bufReceive[8];                    //Receive data buffer
  unsigned long canID=0;                          //CAN ID

//Receiving messages
  if(CAN_MSGAVAIL == CAN.checkReceive()){
    CAN.readMsgBuf(&lenReceive, bufReceive);      //Read data,  lenReceive: data length, bufReceive: data buffer
    canID = CAN.getCanId();                       //Read the message id associated with this CAN message (also called a CAN frame)
    
    Serial.print("Sensor Value from ID: ");       //Print the ID of the CAN message
    Serial.print(canID);
    Serial.print(" = ");
    
    for (int i = 0; i < lenReceive; i++){         //Print the data from the CAN message
      Serial.print(bufReceive[i]);
      Serial.print("\t");
    }Serial.println();

//Processing received messages
    if(canID ==  Battery_OK_ID){                  //Check if message is from BPS
      for(unsigned i=0; i<8; i++){
        BattOk[i]=bufReceive[i];                  //Load receive buffer into battery ok buffer
      }
    }
  }

  batteryOk=BattOk[0];                            //Update batteryOk variable

//Switched power line MOSFETs control
  if(batteryOk){                                  //Check status of main battery pack
    digitalWrite(mainSwGate, HIGH);               //Turn on main-Switched MOSFET
    digitalWrite(suppSwGate, LOW);                //Turn off supp-Switched MOSFET
  }else{
    digitalWrite(suppSwGate,HIGH);                //Turn on supp-Switched MOSFET
    digitalWrite(mainSwGate, LOW);                //Turn off main-Switched MOSFET
  }
}
