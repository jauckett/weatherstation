    #include <RH_ASK.h>  
    #include <SPI.h> // Not actualy used but needed to compile  
    #include <stdarg.h>
      
//    RH_ASK driver(1200, 11, 12, 10, false);  
    RH_ASK driver(1200, 11, 12, 10, false);  
    int count;  
    unsigned long bytes=0;  
      
    void setup()  
    {  
        Serial.begin(115200); // Debugging only  
        if (!driver.init())  
             Serial.println("init failed");           
         Serial.println("Initialised\n");  
    }  
    
    void p(char *fmt, ... ){
        char buf[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(buf, 128, fmt, args);
        va_end (args);
        Serial.print(buf);
}

    char* bytesToChars(uint8_t* b) {
       char c[sizeof(b)]; 
       for (int i=0; i<RH_ASK_MAX_MESSAGE_LEN; i++) {
           c[i] = (char)b[i];
       }
       return c;  
    }
    
    int length(uint8_t* b) {
        for (int i=0; i<RH_ASK_MAX_MESSAGE_LEN; i++) {
            if (b[i] == 0) {
              return i;
            }
        }  
        return RH_ASK_MAX_MESSAGE_LEN;
    }
    
//String Length=24
//ID=1 T=7.5 P=102323 H=44                                

//String Length=48
//ID=1 TR=(5.5,14.1) PR=(102273,102335) HR=(39,45)
      bool debug = true;
    void loop()  
    {  
        uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];  
        uint8_t buflen = sizeof(buf);  
        
        if (driver.recv(buf, &buflen)) // Non-blocking  
        {  
            count++;      
            bytes=bytes+buflen;       
              // driver.printBuffer("\nGot:", buf, buflen);  
              
            // @TODO send the data to a RaspberryPi or PC    
            // for now, send some metrics and the data to the serial port 
            if (debug) {
              Serial.print("String Length=");
              Serial.println(length(buf)); 
                        
              Serial.print("TOTAL BYTES=");  
              Serial.println(bytes);     
              Serial.print("Got message number ");  
              Serial.println(count); 
            } 
            Serial.write(buf, buflen);  
            Serial.println("");  
        }    
    }  
