'''
Team Sleipnir telemetry sd card reader

Written by Einar Arnason
'''

import time
import os
import sys
import re
import json
import psycopg2

try:
    conn = psycopg2.connect(sys.argv[1])
    cursor = conn.cursor()
except:
    print("Unable to connect to the database")

def main():

    cursor.execute('''
    CREATE TABLE IF NOT EXISTS vehicledata
    (
        time TIMESTAMP PRIMARY KEY,
        rpm INTEGER,
        speed INTEGER,
        oilTemp REAL,
        waterTemp REAL,
        ecuTemp REAL,
        volt REAL,
        map INTEGER,
        gear INTEGER,
        airTemp REAL,
        fuelPressure REAL,
        fanOn BOOLEAN,
        fuelPumpOn BOOLEAN,
        latitude REAL,
        longitude REAL,
        gpsSpeed REAL,
        gpsFixQuality INTEGER,
        cylcontrib1 INTEGER,
        cylcontrib2 INTEGER,
        cylcontrib3 INTEGER,
        cylcontrib4 INTEGER
    );
    ''')
    conn.commit()

    filenames = []
    if sys.argv[2] == '-d':
        for dirname in sys.argv[3:]:
            for filename in os.listdir(dirname):
                filenames.append('{}{}'.format(dirname, filename))
    else:
        for filename in sys.argv[2:]:
            filenames.append(filename)
    for filename in filenames:
        try:
            file = open(filename)
            data = file.read()
            data = re.split(r"(?=\{)(.*?)(?<=\})", data)
            for entry in data:
                if entry is not '!' and entry is not '':
                    try:
                        values = json.loads(entry)
                        MessageTime = values['time']

                        cursor.execute("SELECT time FROM vehicledata WHERE time = to_timestamp({})".format(MessageTime))
                        conn.commit()

                        if cursor.fetchone() is None :
                            cursor.execute('''
                            INSERT INTO vehicledata (time)
                            VALUES (to_timestamp({}))
                            '''.format(MessageTime))

                        for key, value in values.items() :
                            if key != "time":
                                cursor.execute('''
                                UPDATE vehicledata 
                                SET {} = {}
                                WHERE time = to_timestamp({})
                                '''.format(key, value, MessageTime))
                                conn.commit()
                    except Exception as e:
                        print(e)
                        print(entry)
        except Exception as e:
            print(e)

if __name__ == "__main__":
    main()
