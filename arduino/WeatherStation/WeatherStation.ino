    #include <DHT.h>
 
    #include <stdarg.h>  
    #include <Wire.h>  
    #include <Adafruit_BMP085.h>  
      
    #include <RH_ASK.h>  
    #include <SPI.h> // Not actually used but needed to compile  
      
    /*************************************************** 
    Reads temp and pressure from the BMP180 sensor 
    and transmits a formated string in the following format 
     
    ID is a numeric identifier in case you have several transmitters 
    T is temperature in Celcius 
    P is pressure in pascals 
     
    ID=1 T=26.0 P=100100 
    ****************************************************/  
      
    // Sensor uses https://github.com/adafruit/Adafruit-BMP085-Library  
    // BMP180 is compatible with BMP085  
    // VCC to 3.3V (not 5.0V)  
    // GND to Ground  
    // SCL to i2c clock Analog 5  
    // SDA to i2c data Analog 4  
    // EOC is not used (signifies End Of Conversion)  
    // XCLR reset pin, not used  
      
    // Transmitter uses http://www.airspayce.com/mikem/arduino/RadioHead/ library  
    // The driver used is RH_ASK  
    // Three pins, VCC 3.3v, GND and DATA to pin 12  
      
    Adafruit_BMP085 bmp;  
      
    RH_ASK driver(1200, 11, 12, 10, false);  
    
      float minTemp;
  float maxTemp;
  float minHum;
  float maxHum;
  long minPre, maxPre;
      
    // Unique id for this Arduino so receiver can identify different transmitters  
    int id=1;  
                 
    /* 
      Uses sprintf to format arguments into a string buffer 
      Sends to serial port and transmitter       
    */  
    void transmit(char *fmt, ...) {    
        char buf[64]; // resulting string limited to 128 chars 
        // initialise buffer will all spaces   
        for (int i=0; i<sizeof(buf); i++) {    
            buf[i] = 32;    
        }    
        va_list args;    
        va_start (args, fmt );    
        vsnprintf(buf, 56, fmt, args);    
        va_end (args);    
//        Serial.print("STRLEN=");
//        Serial.println(strlen(buf));
//        Serial.println(buf);    
        // replace the 0 terminator with a space
        buf[strlen(buf)] = 32;
        driver.send((uint8_t *)buf, 56);    
        driver.waitPacketSent();    
    }    
    
    int iPart(float f) {
        return (int)f;
    }
    int fPart(float f) {
        return (f-iPart(f))*10;    
    }
    /* 
      the %f not supported in STD lib 
      Format the temperature into int and decimal parts 
      and transmit it 
    */  
    void sendMeasurement(float t, long p, float h) {    
        int iP = iPart(t);    
        int fP = fPart(t);    
        transmit("ID=%d T=%d.%d P=%lu H=%d", id, iP, fP, p, (int)h); 
        delay(500);
        
        // @TODO should the sensor controller keep min/max values?
//        transmit("ID=%d TR=(%d.%d,%d.%d) PR=(%lu,%lu) HR=(%d,%d)", id, 
//                 iPart(minTemp), fPart(minTemp), iPart(maxTemp), fPart(maxTemp),
//                 minPre, maxPre, (int)minHum, (int)maxHum);
        
        // ID=1 TR=(15.6,15.6) PR=(102315,102320) HR=(40,40)
        // ID=1 T=13.60 P=102252 H=41 TR=(0.0,20.0) PR=(100000,101325) HR=(35,76)

     float t1 = maxTemp;   
    }    
        
#define DHTPIN 2  
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);



// ID=1 T=8.0 P=102336 H=44 W=99.9 D=999 R=99.9


            
    void setup() {    
        Serial.begin(115200);   
       
        if (!bmp.begin()) {    
           Serial.println("Could not find a valid BMP085 sensor, check wiring!");    
            while (1) {}    
        }

        dht.begin();               

        minTemp = bmp.readTemperature(); 
        Serial.print("MINTEMP=");
        Serial.println(minTemp);
        maxTemp = minTemp;
        minHum = dht.readHumidity();
        maxHum = minHum;
        minPre = bmp.readPressure();
        maxPre = minPre;

        if (!driver.init())    
          Serial.println("ASK init failed");    
    }    
    
    float toCelcius(float f) {     
        return ((f-32.0)*5)/9; 
    }
    
    float toFahrenheit(float c) {
      return (c*9/5)+32;
    }
            
    void loop() {    
               
        float t = bmp.readTemperature();    
        long p = bmp.readPressure();    
              
        // @TODO query other sensors such as humidity, wind speed/direction, rain  
        
        
      float dh = dht.readHumidity();
      float dt = dht.readTemperature(true);
      
      if (t < minTemp) {
        minTemp = t;
      }
      if (t > maxTemp) {
        maxTemp = t;
      }
      
      if (dh < minHum) {
        minHum = dh;
      }
      if (dh > maxHum) {
        maxHum = dh;
      }
      if (p > maxPre) {
        maxPre = p;
      }
      if (p < minPre) {
        minPre = p;
      }
      
      if (isnan(dt) || isnan(dh)) {
          Serial.println("Failed to read from DHT");
      } else {
        Serial.print("F=");
        Serial.print(dt);
        Serial.print(" C=");
        Serial.print(toCelcius(dt));
        Serial.print(" F=");
        Serial.println(toFahrenheit(toCelcius(dt)));
        Serial.print("Humidity=");
        Serial.println(dh);
      }

      Serial.print("Min/Max Temp=(");
      Serial.print(minTemp);
      Serial.print(",");
      Serial.print(maxTemp);      
      Serial.print(") Hum=(");
      Serial.print(minHum);
      Serial.print(",");
      Serial.print(maxHum);  
      Serial.print(") Pre=");
      Serial.print(minPre);
      Serial.print(",");
      Serial.print(maxPre);
      Serial.println(")");      
            
            
        // transmit the results    
        sendMeasurement(t, p, dh);    
        Serial.println();    
        Serial.println();    
        delay(3000);    
    }    
