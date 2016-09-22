import serial
import time
import csv
from threading import Timer
from threading import Lock

stationid = 1
temp = 0
pressure = 0
humidity = 0

# count an average the readings received during a period
readingCount = 0 
avgTemp = 0.0
avgHum = 0
avgPre = 0

normalisedTime = 0

interval = 60

lock = Lock()


csvfile = open('readings.csv', 'ab', 1)
of = csv.writer(csvfile)

def update_readings():
    global readingCount, avgTemp, avgHum, avgPre 
    lock.acquire()
    readingCount += 1
    avgTemp += temp
    avgPre += pressure
    avgHum += humidity
    lock.release()

tick_count = 0
def tick():
    global readingCount, avgTemp, avgHum, avgPre, tick_count, normalisedTime, of
    lock.acquire()
    if (readingCount == 0):
        print "No readings received during interval", interval
        return
    tick_count += 1
    print "TICK", tick_count, time.gmtime(time.time())
    print "TICK SAVE", avgTemp/readingCount, avgPre/readingCount, avgHum/readingCount
    weather_record = [normalisedTime,  round((avgTemp/readingCount)+0.001, 2), avgPre/readingCount, avgHum/readingCount, -1.0, -1.0 -1.0]
    print "Saving reading", now, weather_record 
    of.writerow(weather_record)
    readingCount = 0
    avgTemp = 0
    avgPre = 0
    avgHum = 0
    Timer(interval, tick, ()).start()
    lock.release()

def format_min_sec(tm):
    gmt = time.gmtime(tm)
    mins = ""
    secs = ""
    if (gmt.tm_min < 10):
        mins = "0" + str(gmt.tm_min)
    else:
        mins = str(gmt.tm_min)
    if (gmt.tm_sec < 10):
        secs = "0" + str(gmt.tm_sec)
    else:
        secs = str(gmt.tm_sec)
    return mins +":" + secs

def parse(line):
    global id
    global temp
    global pressure
    global humidity

    #['ID=1', 'T=16.3', 'P=102272', 'H=39']
#    print 
#    print "Parsing '", line 
    for reading in line.split(" "):
        if (reading.find("TR=") != -1):
#            print "min/max reading", line
            return False
        if (reading.find("ID") == 0):
            id = int(reading.split("=")[1])
#            print "ID is", id
            if (stationid != id):
               print "Unknown ID", id
               return
            
        if (reading.find("T=") == 0):
            temp = float(reading.split("=")[1])
#            print "Temperature is", temp

        if (reading.find("P=") == 0):
            pressure = int(reading.split("=")[1])
#            print "Pressure is", pressure

        if (reading.find("H=") == 0):
            humidity = int(reading.split("=")[1])
#            print "Humidity is", humidity
    return True


now = int(time.time())
sl = interval - (now % interval)
#print time.gmtime(now)
#print "SLEEPING FOR", sl
#time.sleep(sl)
#now = int(time.time())
#print time.gmtime(now)

# set the timer to got off at the next interval
print "Timer set to go in", sl
Timer(sl, tick, ()).start()


print "Starting weather logger"
lastreading = int(time.time()/interval)*interval
#csvfile = open('readings.csv', 'ab', 1)
#of = csv.writer(csvfile)

s = serial.Serial('/dev/ttyUSB0', 9600)
# read a couple of lines to clear the buffer
s.readline()
s.readline()


while True:
    line = s.readline().strip()
    if (parse(line)):
        now = int(time.time())
        gmt = time.gmtime(now)
        normalisedTime = int(now/interval)*interval
        timediff = normalisedTime - lastreading
        update_readings()
        print readingCount, format_min_sec(now), "temp=", avgTemp/readingCount, "pressure=", avgPre/readingCount, "Humidity=", avgHum/readingCount

        # TODO save all intermediate readings an record the average
        # only save a reading each interval
"""
        if (timediff >= interval):
            reading = [normalisedTime, temp, pressure, humidity, -1.0, -1.0, -1.0]
            print "Saving reading", now, reading 
            of.writerow(reading)
            lastreading = normalisedTime
            avgTemp = 0.0
            avgPre = 0
            avgHum = 0 
            readingCount = 0
"""

