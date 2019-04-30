#include <SPI.h>
#include <RH_RF95.h>

#define RF95_FREQ 434.0
#define RFM95_CS 15
#define RFM95_RST 25
#define RFM95_INT 2
#define ERRORLED 3

RH_RF95 rf95(RFM95_CS, RFM95_INT);  

// Message definitions
#define REPLY_ACK 0x01
#define REPLY_ERROR 0x03

//the command I will be sending: nr.1-command type, nr.2-value and nr.3-error check.
const int COMMAND_size = RH_RF95_MAX_MESSAGE_LEN;
uint8_t COMMAND[COMMAND_size];
uint8_t CMD_TYPE;
uint8_t CMD_VALUE;
uint8_t CMD_CHECK;

//the data I will be recieving from the Flight Computer
uint8_t DATA_TYPE;
uint8_t DATA_VALUE;
uint8_t DATA_ERROR;
bool connectionEstablishedToPit = false;

//Data
uint8_t CurrentState = 0;
uint8_t test = 0;

// Pit Control caller ID
const uint8_t Pit_ID = 7;

// Car Computer caller ID
const uint8_t Car_ID = 6;

void setup()
{
    delay(500);
    Serial.println("Race Car Setup");
    Serial.begin(9600);
    while (!Serial)
        ; // Wait for serial port to be available
    if (!rf95.init())
        Serial.println("init failed");
    sendMessage(REPLY_ACK);
}

void recieveMessage()
{
    uint8_t buf[COMMAND_size];
    uint8_t len = sizeof(buf);
    Serial.println("Waiting for reply...");

    // Max wait for data is 1 sec..
    if (rf95.waitAvailableTimeout(1000))
    {
        if (rf95.recv(buf, &len))
        {
            if (rf95.headerId() == Pit_ID)
            {
                connectionEstablishedToPit = true;
                DATA_TYPE = buf[0];
                DATA_VALUE = buf[1];
                DATA_ERROR = buf[2];

                switch (DATA_TYPE)
                {
                case 0x01:
                    Serial.print("#ACK COMMAND: ");
                    Serial.println(buf[1]);
                    break;

                default:
                    connectionEstablishedToPit = false;
                    Serial.println("#FAILED");
                    break;
                }
                sendMessage(REPLY_ACK);

                Serial.println((char *)buf);
                Serial.print("RSSI: ");
                Serial.println(rf95.lastRssi(), DEC);
            }
            else
            {
                Serial.println("Not my message, ID:");
                Serial.println(rf95.headerId());
            }
        }
        else
        {
            Serial.println("ERROR: Receive message failed");
        }
    }
    else
    {
        Serial.println("WARNING: ACK not received");
    }
    delay(1000);
}

bool establishConnection()
{
    sendMessage(REPLY_ACK);

    if (establishConnectionReply())
    {
        return true;
    }
    return false;
}

bool establishConnectionReply()
{
    if (connectionEstablishedToPit)
    {
        return true;
    }
    return false;
}

void sendMessage(uint8_t CMD)
{
    rf95.setHeaderId(Car_ID);
    uint8_t data[RH_RF95_MAX_MESSAGE_LEN] = {CMD};
    uint8_t len = sizeof(data);
  
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    
    if (rf95.waitAvailableTimeout(4000))
    {
        if (rf95.recv(buf, &len))
        {
            Serial.print("got reply: ");
            //Serial.println((char *)buf);
            //Serial.print("RSSI: ");
            //Serial.println(rf95.lastRssi(), DEC);
            for ( int i = 0; i < len; i++)
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
        Serial.println("No reply, is rf95_server running?");
    }
    delay(10);
}

void loop()
{
    //Serial.println("____________________________________________________________");
    delay(10);
}