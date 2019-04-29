#include <SPI.h>
#include <RH_RF95.h>

// Change to 434.0 or other frequency, must match RX's freq!
#define rf95_FREQ 915.0
#define RFM69_CS 15
#define RFM69_RST 25
#define RFM69_INT 2

#define ERRORLED 3
#define REPLY_ACK 0x01
//#define REPLY_ERROR 0x03

// Pit Control caller ID
const uint8_t Pit_ID = 9;

// Car Computer caller ID
const uint8_t Car_ID = 8;

//two types of commands: set state & set throttle
//uint8_t CMD_SET_STATE = 0x01;		 // B 0000 0001;
//uint8_t CMD_SET_THROTTLE = 0x02; // B 0000 0010;

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

enum STATE_NAMES
{
    TEST,
    START,
    READY,
    RESET
};

// Singleton instance of the radio driver
RH_RF95 rf95(RFM69_CS, RFM69_INT);

void setup()
{
    delay(10000);
    Serial.begin(115200);
    Serial.println("Setup start");
    Serial.println("Pit Control Sending Unit!");
    Serial.println("Initializing");
    while (!rf95.init())
    {
        Serial.println("Initialise failed!");
        while (1)
            ;
    }
    Serial.println("Initialise achieved!");

    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
    if (!rf95.setFrequency(rf95_FREQ))
    {
        Serial.println("setFrequency failed");
        while (1)
            ;
    }

    Serial.print("Frequency set to: ");
    Serial.println(rf95_FREQ);

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on The default transmitter power is 13dBm, using PA_BOOST. If you are using RFM69/96/97/98 modules which uses the PA_BOOST transmitter pin, then you can set transmitter powers from 5 to 23 dBm:
    rf95.setModeRx();
    delay(100);
    rf95.setTxPower(23, true);
    Serial.println("Setup Complete.");
    Serial.println("____________________________________________________________");

    recieveMessage();
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

    loop();
}

void recieveMessage()
{
    uint8_t buf[COMMAND_size];
    uint8_t len = sizeof(buf);

    Serial.println("Waiting for reply...");

    // MAX wait for DATA is set 1 sec.
    if (rf95.waitAvailableTimeout(10))
    {
        if (rf95.recv(buf, &len))
        {
            if (rf95.headerId() == Car_ID)
            {
                if (buf[2] == (buf[0] ^ buf[1]))
                {
                    DATA_TYPE = buf[0];
                    DATA_VALUE = buf[1];
                    DATA_ERROR = buf[2];

                    switch (DATA_TYPE)
                    {
                    case 0x01:
                        Serial.print("#ACK COMMAND: ");
                        Serial.println(buf[1]);
                        //digitalWrite(ERRORLED, LOW);
                        break;

                    default:
                        Serial.println("#FAILED");
                        //digitalWrite(ERRORLED, HIGH);
                        break;
                    }
                    sendMessage(0, REPLY_ACK);
                }
                else
                {
                    Serial.println("CheackSum ERROR");
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
            Serial.println("Receive failed");
        }
    }
    else
    {
        Serial.println("ACK not received");
    }
}

void loop()
{
    Serial.println("____________________________________________________________");
    recieveMessage();
    delay(100000);
    if (!rf95.available())
        Serial.println("No message available from Car...");
}