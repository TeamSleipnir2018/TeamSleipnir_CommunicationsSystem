#include <SPI.h>
#include <RH_RF95.h>

#define RF95_FREQ 434.0
#define RFM95_CS 15
#define RFM95_RST 25
#define RFM95_INT 2

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

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

//Values
uint8_t CurrentState = 0;

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

void sendMessage(uint8_t CMD_TYPE, uint8_t CMD_VALUE)
{
    // check is the XOR of TYPE and VALUE
    CMD_CHECK = CMD_VALUE ^ CMD_TYPE;
    COMMAND[0] = CMD_TYPE;
    COMMAND[1] = CMD_VALUE;
    COMMAND[2] = CMD_CHECK;

    if (!rf95.send((uint8_t *)COMMAND, COMMAND_size))
        Serial.println("ERROR: Message not able to send!");

    Serial.println("Reply Sent!");
    delay(10);
    rf95.waitPacketSent();

    //loop();
}

void recieveMessage()
{
    uint8_t buf[COMMAND_size] = {0, 0, 0};
    uint8_t len = sizeof(buf);

    Serial.println("Waiting for reply...");

    // MAX wait for DATA is set 1 sec.
    if (rf95.init()) //.waitAvailableTimeout(10))
    {
        if (rf95.recv(buf, &len))
        {
            Serial.println("Recv is true!");
            if (rf95.headerId() == Car_ID)
            {
                DATA_TYPE = buf[0];
                DATA_VALUE = buf[1];
                DATA_ERROR = buf[2];

                /*switch (DATA_TYPE)
          
          case 0x01:
          Serial.print("#ACK COMMAND: ");
          Serial.println(buf[1]);
          //digitalWrite(ERRORLED, LOW);
          

          default:
          Serial.println("#FAILED");
          //digitalWrite(ERRORLED, HIGH);
          break;*/

                //sendMessage(0, REPLY_ACK);

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
            Serial.println("Receive failed");
            Serial.println(buf[0]);
            Serial.println(buf[1]);
            Serial.println(buf[2]);
        }
    }
    else
    {
        Serial.println("ACK not received");
    }
    delay(1000);
    //loop();
}

void loop()
{
    Serial.println("____________________________________________________________");

    delay(1000);
    if (!rf95.available())
    {
        Serial.println("No message available from Car...");
    }
    recieveMessage();
    //sendMessage(Pit_ID, REPLY_ACK);
    //sendMessage(Car_ID, REPLY_ACK);
}