import time
import os
import sys
import signal
import re
import json
import psycopg2
from digi.xbee.devices import XBeeDevice
from digi.xbee.exception import TimeoutException

if os.name == "windows" or os.name == "nt" :
    PORT = "COM{}".format(sys.argv[1])
elif os.name == "linux" or os.name == "posix" :
    PORT = "/dev/ttyS{}".format(sys.argv[1])
BAUD_RATE = 9600

xbee = XBeeDevice(PORT, BAUD_RATE)
message = None

def signal_handler(signal, frame):
    xbee.close()
    sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)

try:
    conn = psycopg2.connect(sys.argv[2])
    cursor = conn.cursor()
except:
    print("Unable to connect to the database")

def main():

    cursor.execute('''
    CREATE TABLE IF NOT EXISTS vehicledata
    (
        time TIMESTAMP,
        rpm INTEGER,
        speed INTEGER,
        oilTemp REAL,
        waterTemp REAL,
        volt INTEGER
    );
    ''')
    conn.commit()

    print(" +------------------------+")
    print(" | Reading XBee data      |")
    print(" +------------------------+\n")

    while True :
        try:
            if not xbee.is_open() :
                xbee.open()
            time.sleep(1)
            message = xbee.read_data()

            if message is not None :

                data = json.loads(re.split(r"(?=\{)(.*?)(?<=\})", str(message.data))[1])
                print(data)

                cursor.execute('''
                INSERT INTO vehicledata (time, rpm, speed, oiltemp, watertemp, volt)
                VALUES (%s, %s, %s, %s, %s, %s)
                ''', (time.ctime(), data['rpm'], data['speed'], data['oilTemp'], data['waterTemp'], data['volt']))
                conn.commit()

        except TimeoutException as e:
            print(e)
    xbee.close()

if __name__ == "__main__":
    main()