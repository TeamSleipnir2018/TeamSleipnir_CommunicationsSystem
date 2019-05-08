// -------------------------------------------------------------
// CANtest for Teensy 3.6 dual CAN bus
// by Collin Kidder, Based on CANTest by Pawelsky (based on CANtest by teachop)
//
// Both buses are left at default 250k speed and the second bus sends frames to the first
// to do this properly you should have the two buses linked together. This sketch
// also assumes that you need to set enable pins active. Comment out if not using
// enable pins or set them to your correct pins.
//
// This sketch tests both buses as well as interrupt driven Rx and Tx. There are only
// two Tx buffers by default so sending 5 at a time forces the interrupt driven system
// to buffer the final three and send them via interrupts. All the while all Rx frames
// are internally saved to a software buffer by the interrupt handler.
//

#include <FlexCAN.h>


static CAN_message_t msg;
static uint8_t hex[17] = "0123456789abcdef";


// -------------------------------------------------------------
void setup(void)
{
  delay(1000);
  Serial.begin(9600);
  Serial.println("Hello Teensy CAN Send Test.");

  Can0.begin(500000);

  msg.ext = 0;
  msg.id = 0x100;
  msg.len = 8;
  msg.buf[0] = 10;
  msg.buf[1] = 20;
  msg.buf[2] = 0;
  msg.buf[3] = 100;
  msg.buf[4] = 128;
  msg.buf[5] = 64;
  msg.buf[6] = 32;
  msg.buf[7] = 16;
}


// -------------------------------------------------------------
void loop(void)
{

  msg.buf[0]++;
  Can0.write(msg);
  msg.buf[0]++;
  Can0.write(msg);
  msg.buf[0]++;
  Can0.write(msg);
  msg.buf[0]++;
  Can0.write(msg);
  msg.buf[0]++;
  Can0.write(msg);  

  
  delay(20);
  Serial.println("Im in a sending loop.");
}
