#include <SPI.h>
#include <RH_RF95.h>

#define RF95_FREQ 434.0
#define RFM95_CS 15
#define RFM95_RST 25
#define RFM95_INT 2

RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Message definitions
#define ERRORLED 3
#define REPLY_ACK 0x01
#define REPLY_ERROR 0x03
#define REPLAY_INITAL 0xFE

//the command I will be sending: nr.1-command type, nr.2-value and nr.3-error check.
const int COMMAND_size = 3;
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
    Serial.begin(9600);
    while (!Serial)
        ; // Wait for serial port to be available
    if (!rf95.init())
        Serial.println("init failed");
}

void recieveMessage()
{
    uint8_t buf[COMMAND_size];
    uint8_t len = sizeof(buf);
    Serial.println("Waiting for reply...");

    // Max wait for data is 1 sec..
    if (rf95.init()) //rf95.waitAvailableTimeout(100))
    {
        if (rf95.recv(buf, &len))
        {
            Serial.println("Recv is true!");
            if (rf95.headerId() == Pit_ID)
            {
                if (buf[2] == (buf[0] ^ buf[1]))
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
                    sendMessage(0, REPLY_ACK);
                }
                else
                {
                    Serial.println("CheakSum ERROR");
                    //digitalWrite(ERRORLED, HIGH);
                }
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
            Serial.println(buf[0]);
            Serial.println(buf[1]);
            Serial.println(buf[2]);
        }
    }
    else
    {
        Serial.println("WARNING: ACK not received");
    }
    delay(1000);
    //loop();
}

bool establishConnection()
{
    sendMessage(Car_ID, REPLAY_INITAL);

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
void sendMessage(uint8_t CMD_TYPE, uint8_t CMD_VALUE)
{
    rf95.setHeaderId(6);
    uint8_t data = "testing thingy";
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.waitAvailableTimeout(3000))
    {
        if (rf95.recv(buf, &len))
        {
            Serial.print("got reply: ");
            Serial.println((char *)buf);
            Serial.print("RSSI: ");
            Serial.println(rf95.lastRssi(), DEC);
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
    delay(400);
}
void loop()
{
    Serial.println("____________________________________________________________");
    delay(1000);
    //recieveMessage();
    sendMessage(72, 27);
    //loop();
}