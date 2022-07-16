#pragma once
#include "Arduino.h"











/*****************************************************************************/
/*****************************************************************************/
/**
 *
 * XMODEM CRC Communication
 *
 * Simple implementation of read and write using XMODEM CRC.  This is tied
 * directly to the PROM code, so the receive function writes the data to the
 * PROM device as each packet is received.  The complete file is not kept
 * in memory.
 */




//class PromDevice;
class CmdStatus;

class XModem
{
  public:
    XModem( CmdStatus & cs) : cmdStatus(cs) {}
    uint32_t ReceiveFile(uint32_t address);
    bool SendFile(uint32_t address, uint32_t fileSize);
    void Cancel();

  private:
    CmdStatus & cmdStatus;

    int GetChar(int msWaitTime = 3000);
    uint16_t UpdateCrc(uint16_t crc, uint8_t data);
    bool StartReceive();
    int ReceivePacket(uint8_t buffer[], unsigned bufferSize, uint8_t seq, uint32_t destAddr);
    void SendPacket(uint32_t address, uint8_t seq);
};
