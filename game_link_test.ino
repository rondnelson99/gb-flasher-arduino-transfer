#include "game_link_test.h"



CmdStatus cmdStatus;
XModem xmodem(cmdStatus);

volatile byte transferBuffer[256];
volatile bool SpiTransferReady = false;
volatile bool SpiTransferRequested = false;
volatile bool SpiTransferInProgress = false;
volatile unsigned int TransferIndex = 0;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(500000);
  Serial.println("resetting");
  pinMode(PIN_SPI_SCK, INPUT);
  pinMode(PIN_SPI_MOSI, INPUT);
  pinMode(PIN_SPI_MISO, OUTPUT);
  pinMode(14,OUTPUT);
  pinMode(15,OUTPUT);
  SPCR = (1<<SPIE) | (1<<SPE) | (1<<CPOL) | (1<<CPHA) | (1<<SPR1) | (1<<SPR0);

  

}

void loop() {
  uint32_t numBytes = xmodem.ReceiveFile(0);


   
  if (numBytes)
  {
      cmdStatus.info("Success writing to EEPROM device.");
      cmdStatus.setValueDec(0, "NumBytes", numBytes);
  }
  /*else
  {
      xmodem.Cancel();
  }*/

}  





bool prepBlockWrite(byte buffer[]){
  static bool onHighHalf = false;
  memcpy(transferBuffer + 128*onHighHalf,buffer, 128); //get the data into the SPI block
  if (onHighHalf){ //that means we're ready to transfer a block to the gameboy
    SpiTransferReady = true;
    digitalWrite(15, HIGH);
    onHighHalf = false;
    
     

    
    while(SpiTransferReady){}
  }
  else{
      onHighHalf = true;
  }
  
  return 0;
}


#define XFER_INIT 42
#define XFER_READY 43
#define XFER_START 44
#define XFER_WAIT 45


ISR (SPI_STC_vect)
{
  
  if(SpiTransferInProgress){
    SPDR = transferBuffer[TransferIndex];
    TransferIndex++;
    if (TransferIndex == 256 + 1){// we're using 256 byte blocks, but the gb asks for one two many bytes and discards the last one
      SpiTransferRequested = false; // now this thread stops sending anything until the other thread sets us up again.
      SpiTransferInProgress = false;
      SpiTransferReady = false;
      digitalWrite(14, LOW);
      digitalWrite(15, LOW);
    }
  }
  else if (SPDR == XFER_INIT){// to init a transfer, the gameboy sends a 42. Then we send it a 43 at the same time as it sends us a 44. If all that happens, we're good to go.
    //Serial.println("starting");
    if(SpiTransferReady){
      SPDR = XFER_READY;
      SpiTransferRequested = true;  
    }
    else{
      SPDR = XFER_WAIT; //tell the gameboy to wait a moment so we can get the block from the computer
    }
  }
  else if (SPDR == XFER_START && SpiTransferRequested){
    if(SpiTransferReady){
      SpiTransferInProgress = true;
      digitalWrite(14,HIGH);
      SPDR = transferBuffer[0]; //send out the first byte (byte #0)
      TransferIndex = 1;  //send out byte #1 next time
    }
  }
  else{  
  }
}
