'''
Team Sleipnir telemetry ECU reader

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
            data = file.readlines()
            print('\nReading values from {}\n'.format(filename))
            for entry in data:
                cursor.execute('''
                INSERT INTO vehicledata
                VALUES({})
                '''.format(entry))
                conn.commit()
        except Exception as e:
            print(e)

if __name__ == "__main__":
    main()


