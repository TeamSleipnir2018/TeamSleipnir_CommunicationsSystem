#include <SPI.h>
#include <RH_RF95.h>

#define RF95_FREQ 434.0
#define RFM95_CS 15
#define RFM95_RST 25
#define RFM95_INT 2

RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Message definitions
#define REPLY_ACK 0x01
#define REPLY_ERROR 0x03

//the command I will be sending: nr.1-command type, nr.2-value and nr.3-error check.
const int COMMAND_size = RH_RF95_MAX_MESSAGE_LEN;
uint8_t COMMAND[COMMAND_size];

// Pit Control caller ID
const uint8_t Pit_ID = 7;

// Car Computer caller ID
const uint8_t Car_ID = 6;

void setup()
{
    delay(500);
    //Serial.println("Pit Lane Setup");
    Serial.begin(9600);
    while (!Serial)
        ; // Wait for serial port to be available
    if (!rf95.init())
        Serial.println("init failed");
}

void sendMessage(uint8_t CMD)
{
    rf95.setHeaderId(Car_ID);
    uint8_t data[COMMAND_size] = {CMD};
    uint8_t len = sizeof(data);

    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
    uint8_t buf[COMMAND_size];

    if (rf95.waitAvailableTimeout(4000))
    {
        if (rf95.recv(buf, &len))
        {
            Serial.print("Got a reply: ");
            //Serial.println((char *)buf);
            //Serial.print("RSSI: ");
            //Serial.println(rf95.lastRssi(), DEC);
            for (int i = 0; i < len; i++)
            {
                Serial.print((char)buf[i]);
            }
            Serial.println("");
        }
        else
        {
            Serial.println("recv failed");
        }
    }
    else
    {
        Serial.println("No reply, is the Car sender running?");
    }
    delay(10);
}

void recieveMessage()
{
    uint8_t buf[COMMAND_size];
    uint8_t len = sizeof(buf);

    //Serial.println("Waiting for reply...");

    // MAX wait for DATA is set 1 sec.
    if (rf95.waitAvailableTimeout(400))
    {
        if (rf95.recv(buf, &len))
        {
            if (rf95.headerId() == Car_ID)
            {
                for (int i = 0; i < len; i++)
                {
                    Serial.print((char)buf[i]);
                }

                switch (buf[0])
                {
                case 0x01:
                    Serial.print("#ACK COMMAND: ");
                    Serial.println(buf[1]);
                    //digitalWrite(ERRORLED, LOW);
                    break;
                
                default:
                    //Serial.println("#FAILED");
                    //digitalWrite(ERRORLED, HIGH);

                    break;
                }
                sendMessage(REPLY_ACK);
                //Serial.println((char *)buf);
                //Serial.print("RSSI: ");
                //Serial.println(rf95.lastRssi(), DEC);
            }
            else
            {
                Serial.println("Not my message, ID: ");
                Serial.println(rf95.headerId());
            }
        }
        else
        {
            Serial.println("Receive failed");
        }
    }
    else
    {
        Serial.println("ACK not received");
    }
    delay(1000);
}

void loop()
{
    //Serial.println("____________________________________________________________");
    delay(10);
    recieveMessage();
}