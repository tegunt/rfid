/*****************************
 *      TEGUNT RFID SHIELD
 *        Sample Script 
 ****************************/
#include <SoftwareSerial.h>

/*
 * Using 10 and 11 as virtual serial pins . Sometimes if there is a wire connected to default serial input (Pin #0) 
 * prevent sketch from uploading to Arduino
 */
SoftwareSerial mySerial(10, 11); // RX, TX
int rfidBUZZERin = A4;
int rfidLEDin = A3;
int BUZZERout = A2;
int LEDout = A5;

String cardData ="";
int maxCardDataLength = 34;

int dataRead;
int dataReadLength =0; 

long leddelay=50000,loopi=0; /// set led delay here
long buzzerdelay= 2000;///set buzzer delay from here
bool tagRead = false;

//Sends and waits for response. Must wait for complete response 
//before sending the next message
void txrx(unsigned char * message, unsigned char messageLen)
{
  mySerial.write(message, messageLen);
  
  //Wait for response to arrive
  while(!mySerial.available()) {
  }
  
  while (mySerial.available())
  {
    //Added delay
    delay(10);
    mySerial.read();
  }

}

//Calculate the checksum of the message and added as the last byte before sending it over
//to RFID reader
unsigned char CheckSum (unsigned char * uBuff, unsigned char uBuffLen)  
{  
  unsigned char i, uSum = 0;  
  for (i = 0; i <uBuffLen; i++)  
  {  
    uSum = uSum + uBuff [i];  
  }  
  uSum = (~ uSum) + 1;  
  return uSum;  
} 


void setup() {
  pinMode(rfidBUZZERin, INPUT);
  pinMode(rfidLEDin, INPUT);
  
  pinMode(BUZZERout, OUTPUT);
  pinMode(LEDout, OUTPUT);
  digitalWrite(LEDout,HIGH);
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial ) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  Serial.println("RFID initializing");
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);

  byte reset_message[] = {0xA0,0x03,0x65,0x00,0xF8};
  reset_message[4] = CheckSum(reset_message, 4);

  //Set reader to RS485 mode
  //1=>RS485
  //2=>Wiegand
  //3=>RS232
  byte com_mode_message[] = {0xA0,0x06,0x60,0x00,0x00,0x72,0x01,0x88};
  com_mode_message[7] = CheckSum(com_mode_message, 7);
  txrx(com_mode_message,8);
  
  //0x02 => Timer mode
  //other mode is
  //0x01 => master slave mode
  byte mode_message[] = {0xA0,0x06,0x60,0x00,0x00,0x70,0x02,0x88};
  mode_message[7] = CheckSum(mode_message, 7);
  txrx(mode_message,8);
  
  //Set the timing to 1-sec (0x64 * 10=100*10) in ms, Range is 1-100 (10ms - 1sec) 
  byte timing_message[] = {0xA0,0x06,0x60,0x00,0x00,0x71,0x64,0x25};
  //This will set it to measure every 10 ms
  //byte timing_message[] = {0xA0,0x06,0x60,0x00,0x00,0x71,0x0A,0x7F};
  
  timing_message[7] = CheckSum(timing_message, 7);
  txrx(timing_message,8);
  
  //Send reset to apply the changes
  txrx(reset_message,5);
  
  Serial.println("Waiting for RFID TAG");
  
}

void loop() { // run over and over
  if (mySerial.available()) {
    String strReadData = "";
   tagRead = true;
  // Serial.println(mySerial.readString());
    dataRead = mySerial.read();
    strReadData = String(dataRead,HEX);
    if (strReadData.length() == 1)
    {
     strReadData = "0"+strReadData;
    }

     dataReadLength += strReadData.length();
      cardData += strReadData;
    
    //Serial.print(strReadData);
    // Serial.print("|");
  //  Serial.println("read done");
     if(digitalRead(rfidBUZZERin)== LOW)
       {
        digitalWrite(BUZZERout,HIGH);
       }
       
     if(digitalRead(rfidLEDin)== LOW)
       {
        digitalWrite(LEDout,LOW);
        loopi=0;
       }
      
   }
   else
   {
    if (dataReadLength >= maxCardDataLength)
     {
       String newData = "";
       Serial.println(cardData + " - Length =" + String(dataReadLength));
       Serial.println("Head Sign - Fixed: " +cardData.substring(0,2));
       Serial.println("Device number - Fixed - 00): " + cardData.substring(2,4));
       Serial.println("Card ID:" + cardData.substring(4,28));
       Serial.println("Antenna - Fixed - 00 :" + cardData.substring(28,30));
       Serial.println("Check Sum :" + cardData.substring(30,32));
       Serial.println("Flag - Fixed - FF :" + cardData.substring(32)); //checksum, calculates: From first byte to from the bottom the third byte, altogether 15 bytes
       Serial.println("========== ============== =========== ==========");
       dataReadLength = 0;
       cardData = "";
     }
      tagRead = false;
    
   }
 
  if(loopi>leddelay)
  {
    digitalWrite(LEDout,HIGH);
   // loopi=200;
  }
  if(loopi>buzzerdelay)
  {
    digitalWrite(BUZZERout,LOW);
   // loopi=200;
  }
loopi++;
}

