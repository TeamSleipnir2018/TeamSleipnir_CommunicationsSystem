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
    ();
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
            data = json.load(file)
            cursor.execute('''
            ALTER TABLE vehicledata
            ADD duration REAL
            ''')
            conn.commit()
            for entry in data:
                cursor.execute('''
                ALTER TABLE vehicledata
                ADD {} {}
                '''.format(entry['name'], entry['type']))
                conn.commit()
        except Exception as e:
            print(e)


if __name__ == "__main__":
    main()
