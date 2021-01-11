

//vor jeder änderung hier hochzählen
const String versions = " 1.04";





/*
 Vers 0.01
Pool Niveau Steuerung Jost Mario 12/2020
Once uploaded, open the serial monitor, set the baud rate to 9600 and append "Carriage return"
*/

#define _SIMPLEDEBUG
#include "SimpleDebug.h"




// #include "LiquidCrystal.h"                            //header file for liquid crystal display (lcd)

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h> // http://knolleary.net/arduino-client-for-mqtt/
#include <EEPROM.h>


//  uhrzeit
#include <Time.h>
#include <EthernetUdp.h>
unsigned int localPort = 8888;
const char timeServer[] = "fritz.box"; // time.nist.gov NTP server
//byte timeServer[]  = { 192,168,4,1 };   // fritz box
const int NTP_PACKET_SIZE= 48;           // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE];     //buffer to hold incoming and outgoing packets
EthernetUDP UDP;
int s=0;
int h=1;   //here you set the hour
int m=1;
int Stunde = 0;
int Minute = 1;
int Sekunde = 1;
int UhrzeitAktualisierung = 0;



//oled
//#include <Wire.h>
//#include "SSD1306Ascii.h"
//#include "SSD1306AsciiWire.h"
//#define I2C_ADDRESS 0x3C
//
//SSD1306AsciiWire oled;

// EEPROM speicherstellen
  #define EEPROM_max_fuellzeit 0
  #define EEPROM_laufzeit_pumpe_uebervoll 5
  #define EEPROM_laufzeit_pumpe_bei_tasterdruck 10
  #define EEPROM_laufzeit_pumpe_bei_fhem_on 15
  #define EEPROM_sensor_mindestanstieg 25
  #define EEPROM_sensor_interval 35
  #define EEPROM_laufzeit_pumpe_wasserschwall 45
//uhrzeit



int val = 2;
//millis variblen
long myTimer = 0;
long myTimeout = 1000;




long myTimerrelay = 0;
long myTimeoutrelay = 3000;


// für alle 24 std ausführen
unsigned long last_Del_Time;

// 24 std erkennung für wasser zudosier ventil nur eine von beiden aktiv
//const unsigned long _24h = 86400000;   // 24 stunden
const unsigned long _24h =  8600000;       // jede stunde
                              


String Laufzeit = "0";
bool pumpe_soll_laufen = false;

uint32_t previousMillis_on_for_timer = 0;
int timerwert = 0;
bool fhem_on_for_timer_aktiv = false;

int lastStateTGS = 1; 
int lastStateS1 = 1;
int lastStateS2 = 1;
int lastStateS3 = 1;
int lastStateS4 = 1;
// int lastStateS5 = 1;
int laststate_buttonPinPumpeAus = 0;
int laststate_buttonPinPumpeAn = 0;


int relay = 0;
int vreconnect = 0;
int error = 0;
char result[8]; // Buffer big enough for 7-character float

int i = 0;

float wertfuerrelay = 0;
float u = 0;
float b = 0;
float pHWert = 0;

// wasser nachfüllen weil S1 kein Wasser meldet

bool darf_nachfuellen = true;   // bis sonde S2 oder wenn max füllzeit abgelaufen
bool jetzt_nachfuellen = false;


int max_fuellzeit = 20;  // Maximale Zeit in secunden die das wasserventil innerhalb 24 std füllen darf
int ist_fuellzeit = 0;   // gelaufene Zeit
int laufzeit_pumpe_uebervoll = 30;     // S4 Zeit (ueberfuelzeit Einstellung, wie lange  das  NIVPOOL  maximal  die  Filterpumpezwangsweise  einschaltet,  wenn  der  Behälterbis über die oberste Sonde S4 gefüllt bleibt
int ist_laufzeit_pumpe_uebervoll = 0;  // gelaufene Zeit
int laufzeit_pumpe_bei_tasterdruck = 10;  // pumpe für xx minuten ein bei Tastendruck on
int laufzeit_pumpe_bei_fhem_on = 10;      // pumpe für xx minuten ein bei fhem on kommando 
int laufzeit_pumpe_wasserschwall = 5;

int pumpe_rest_laufzeit = 0;
int nachspeiseventil_rest_laufzeit = 0;

// MAC Adresse des Ethernet Shields
// byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xFE };
const byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0x0D, 0xA2};
// IP des MQTT Servers

byte server[] = { 192, 168, 4, 5 };  // fhem
// EthernetServer serverweb(80);

// IPAddress ip(192,168,4,128);


// Callback function header
void callback(char* topic, byte* payload, unsigned int length);
void parse_cmd(char* string);


// Ethernet Client zur Kommunikation des MQTT Clients
EthernetClient ethClient;
EthernetServer websrv(80);
//EthernetServer ethclient(80);
PubSubClient mqttClient(ethClient);
String readString; 


// MQTT Client zur Kommunikation mit dem Server
// Server - Variable des Types byte mit Serveradresse
// 1883 - Ist der Standard TCP Port
// callback - Function wird aufgerufen wen MQTT Nachrichten eintreffen. Am ende des Sketches
// ethClient - Angabe des Ethernet Clients




const int buttonPinHand = 13;     // the number of the pushbutton pin
const int buttonPinAuto = 14;     // the number of the pushbutton pin
const int buttonPinPumpeAn = 43;     // the number of the pushbutton pin
const int buttonPinPumpeAus = 41;     // the number of the pushbutton pin

const int Eingang_TGS = 2;
const int Eingang_S1 = 7;
const int Eingang_S2 = 5;
const int Eingang_S3 = 6;
const int Eingang_S4 = 8;
//const int Eingang_S5 = 9;
const int Niveausonde_Analog = A2;
const int Ausgang_pumpe = 11;
const int Ausgang_wasserventil = 37;
const int Schalter_Auto = 32;
const int Schalter_Hand = 33;


// Analoger sensor alle xx sec schauen ob wasserstand sprungartig gestiegen
// timer für Analog Niveausonde
long myTimer_sensor = 0;
long sensor_interval = 5000;  // per mqtt setzbar
int sensorwert = 0;
int alter_sensorwert = 0;
int sensor_mindestanstieg = 10;    // per mqtt setzbar




// variables will change:
int state_buttonPinHand = 0;         // variable for reading the pushbutton status
int state_buttonPinAuto = 0;         // variable for reading the pushbutton status
int state_buttonPinPumpeAn = 0;         // variable for reading the pushbutton status
int state_buttonPinPumpeAus = 0;         // variable for reading the pushbutton status

int StateEingang_TGS = 0;
int StateEingang_S1 = 0;
int StateEingang_S2 = 0;
int StateEingang_S3 = 0;
int StateEingang_S4 = 0;



void reconnect() {
  // Loop until we're reconnected
 
  if (!mqttClient.connected())  {
   
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect("Niv_Pool")) {
      Serial.println("online");
      delay(10);
      // Once connected, publish an announcement...
      mqttClient.publish("status","online");
     
      // ... and resubscribe
      mqttClient.subscribe("NivPool_inTopic");
      mqttClient.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2000);  //5000
    }
  }
}



String inputstring = "";                              //a string to hold incoming data from the PC
boolean input_string_complete = false;                //a flag to indicate have we received all the data from the PC
char inputstring_array[10];                           //a char array needed for string parsing


// momentan hard codiert da kein temperatur fühler
float temperature = 28;



char cstr[16];





void setup() {

randomSeed(analogRead(0));

temperature = random(0,32);


Serial.begin(9600);
  while (!Serial) {
                      ; // wait for serial port to connect. Needed for native USB port only
                  }
 // *************************************************** EEPROM *************************
//max_fuellzeit = EEPROM.read(0);
max_fuellzeit = eepromReadInt(EEPROM_max_fuellzeit); 
laufzeit_pumpe_uebervoll = eepromReadInt(EEPROM_laufzeit_pumpe_uebervoll); 
laufzeit_pumpe_bei_tasterdruck = eepromReadInt(EEPROM_laufzeit_pumpe_bei_tasterdruck);
laufzeit_pumpe_bei_fhem_on = eepromReadInt(EEPROM_laufzeit_pumpe_bei_fhem_on);
sensor_mindestanstieg = eepromReadInt(EEPROM_sensor_mindestanstieg);
sensor_interval = eepromReadInt(EEPROM_sensor_interval);
laufzeit_pumpe_wasserschwall = eepromReadInt(EEPROM_laufzeit_pumpe_wasserschwall);


// ***********************************************************************************

        pinMode(buttonPinHand, INPUT_PULLUP);
        pinMode(buttonPinAuto, INPUT_PULLUP);
        pinMode(buttonPinPumpeAn, INPUT_PULLUP);
        pinMode(buttonPinPumpeAus, INPUT_PULLUP);
         
        pinMode(Eingang_TGS, INPUT_PULLUP);
        pinMode(Eingang_S1, INPUT_PULLUP);
        pinMode(Eingang_S2, INPUT_PULLUP);
        pinMode(Eingang_S3, INPUT_PULLUP);
        pinMode(Eingang_S4, INPUT_PULLUP);
        pinMode(Ausgang_pumpe, OUTPUT);
        pinMode(Ausgang_wasserventil, OUTPUT);
        
     delay(100);
  StateEingang_TGS = !digitalRead(Eingang_TGS);   
  StateEingang_S1 = !digitalRead(Eingang_S1);
  StateEingang_S2 = !digitalRead(Eingang_S2);
  StateEingang_S3 = !digitalRead(Eingang_S3);
  StateEingang_S4 = !digitalRead(Eingang_S4);
  sensorwert =analogRead(Niveausonde_Analog); //Die Spannung am Drehregler wird auslesen und wie im vorherigen Sketch als Zahl zwischen 0 und 1023 unter der Variable „sensorwert“ gespeichert.
        alter_sensorwert = sensorwert;
        
                  Serial.print("NivPool Jost Mario");
                  Serial.println(versions);
    
    mqttClient.setServer(server, 1883);
    mqttClient.setCallback(callback);

    Serial.println("Wait for Ethernet begin ....");

      //feste IP
      // Ethernet.begin(mac, ip);
      //oder dhcp
 
      Ethernet.begin(mac);
        // serverweb.begin();
       // Serial.println("Ethernet Connected");


       UDP.begin(localPort);

 
   
    byte macBuffer[6];  // create a buffer to hold the MAC address
     Ethernet.MACAddress(macBuffer); // fill the buffer
    // Serial.println("");
     Serial.print("The MAC address is: ");
      for (byte octet = 0; octet < 6; octet++) {
        Serial.print(macBuffer[octet], HEX);
        if (octet < 5) {
        Serial.print('-');
      }
      
      }
      Serial.println("");
      Serial.print("Server IP Adresse: ");
      Serial.println(Ethernet.localIP());
      Serial.print("MQTT Server IP Adresse: ");
      Serial.print(server[0]); Serial.print("."); 
      Serial.print(server[1]); Serial.print("."); 
      Serial.print(server[2]); Serial.print(".");
      Serial.println(server[3]);      
      Serial.print(F("server available at http://"));
      Serial.println(Ethernet.localIP());
        
      delay(50);
  
  
    Serial.println("+++++++++++++++ INFO +++++++++++++++++++++");
    Serial.println(F("Use Serial commands \"on,off,reboot,reset")); 
    Serial.println(F("Use MQTT command \"on,off,reset,reboot,on-for-timer xx,max_fuellzeit min,laufzeit_pumpe_uebervoll min"));
    Serial.println("+++++++++++++ Parameter ++++++++++++++++++");
    Serial.print("max_fuellzeit: ");
    Serial.print(max_fuellzeit);
    Serial.println(" min ");
    Serial.print("laufzeit_pumpe_uebervoll: ");
    Serial.print(laufzeit_pumpe_uebervoll);
    Serial.println(" min ");
    Serial.print("laufzeit_pumpe_bei_tasterdruck: ");
    Serial.print(laufzeit_pumpe_bei_tasterdruck);  
    Serial.println(" min "); 
    Serial.print("laufzeit_pumpe_bei_fhem_on: ");
    Serial.print(laufzeit_pumpe_bei_fhem_on);
    Serial.println(" min ");
    Serial.print("laufzeit_pumpe_wasserschwall: ");
    Serial.print(laufzeit_pumpe_wasserschwall);
    Serial.println(" min ");
    
    
    Serial.print("sensor_mindestanstieg: ");
    Serial.print(sensor_mindestanstieg);
    Serial.println(" einheiten (0-1024)");
    
    Serial.print("sensor_interval: ");
    Serial.print(sensor_interval);
    Serial.println(" millisecunden");

    
    
    Serial.println("+++++++++++++ Parameter Ende++++++++++++++++++ ");  
    

     // string in char umwandeln um es per mqtt senden zu können
      
      

      
  if (mqttClient.connect("Niv_Pool")) {
        Serial.println("MQTT is connected");
           mqttClient.subscribe("inTopic");
           mqttClient.subscribe("NivPool_inTopic");
           
           mqttClient.publish("status","online"); delay(150);
           mqttClient.publish("Error","ok"); delay(150);
           mqttClient.publish("IP",ip2CharArray(Ethernet.localIP())); delay(150);
           mqttClient.publish("MAC","90-A2-DA-D-D-A2");   delay(150); // geht noch nicht
           mqttClient.publish("last_action","newstart"); delay(150); 
           mqttClient.publish("state","off"); delay(150);
           mqttClient.publish("Pumpe","0"); delay(150);
           mqttClient.publish("Nachspeiseventil","0"); delay(180);
              itoa(max_fuellzeit, cstr, 10);
           mqttClient.publish("max_fuellzeit",cstr);  delay(180);
              itoa(laufzeit_pumpe_uebervoll, cstr, 10); 
           mqttClient.publish("laufzeit_pumpe_uebervoll",cstr);  delay(180);
           itoa(laufzeit_pumpe_bei_tasterdruck, cstr, 10); 
           mqttClient.publish("laufzeit_pumpe_bei_tasterdruck",cstr);  delay(180);
           itoa(laufzeit_pumpe_bei_fhem_on, cstr, 10);
           mqttClient.publish("laufzeit_pumpe_bei_fhem_on",cstr);  delay(180);
           itoa(sensor_mindestanstieg, cstr, 10);
           mqttClient.publish("sensor_mindestanstieg",cstr);  delay(180);
           itoa(sensor_interval, cstr, 10);
           mqttClient.publish("sensor_interval",cstr);  delay(180);
             itoa(laufzeit_pumpe_wasserschwall, cstr, 10);
           mqttClient.publish("laufzeit_pumpe_wasserschwall",cstr);  delay(180);
                
              
                
                if (StateEingang_TGS)  {mqttClient.publish("TGS","1");  delay(180);} else {mqttClient.publish("TGS","0"); delay(180); }
                if (StateEingang_S1)  {mqttClient.publish("S1","1");  delay(180);} else {mqttClient.publish("S1","0"); delay(180);}
                if (StateEingang_S2)  {mqttClient.publish("S2","1");  delay(180);} else {mqttClient.publish("S2","0"); delay(180);  }
                if (StateEingang_S3)  {mqttClient.publish("S3","1");  delay(180);} else {mqttClient.publish("S3","0"); delay(180);  }
                if (StateEingang_S4)  {mqttClient.publish("S4","1");  delay(180);} else {mqttClient.publish("S4","0");  delay(180); }
                
                  


           
           // noch hard codierter Temperatur wert
           itoa(temperature, cstr, 10);
           mqttClient.publish("temperature",cstr);  delay(150);
                         
           mqttClient.subscribe("inTopic");
           mqttClient.subscribe("NivPool_inTopic");
           }


// Check for Ethernet hardware present (optional )

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {

    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");

//    while (true) {
//
//      delay(1); // do nothing, no point running without Ethernet hardware
//    }
  }
  // sendNTPpacket(timeServer);
delay(100);



  // hard codiert später über sensor
  temperature = random(0,32);
 }





// +++++++++++++++++++++++++++++++++++++++++++++++++++++++Uhrzeit +++++++++++++++++++++++++++++++++++++++++++++++++++




// send an NTP request to the time server at the given address
void sendNTPpacket(const char * address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); // NTP requests are to port 123
  UDP.write(packetBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}






/*-------- utility code from jurs ----------*/
boolean summertime_EU(int year, byte month, byte day, byte hour, byte tzHours)
// European Daylight Savings Time calculation by "jurs" for German Arduino Forum
// input parameters: "normal time" for year, month, day, hour and tzHours (0=UTC, 1=MEZ)
// return value: returns true during Daylight Saving Time, false otherwise
{
  if (month<3 || month>10) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
  if (month>3 && month<10) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
  if (month==3 && (hour + 24 * day)>=(1 + tzHours + 24*(31 - (5 * year /4 + 4) % 7)) || month==10 && (hour + 24 * day)<(1 + tzHours + 24*(31 - (5 * year /4 + 1) % 7)))
    return true;
  else
    return false;
}





void incommingTimeNtp(){

if (UDP.parsePacket()) {
    // We've received a packet, read the data from it
    UDP.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = ");
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);
   
   

 
//     Serial.print( day(56566)) ;
//     Serial.print( day(epoch)) ;
//     Serial.print( day(epoch)) ;
//     Serial.print( day(epoch)) ;

    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    h = ((epoch  % 86400L) / 3600);
    Serial.print(':');
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    m= ((epoch  % 3600) / 60);
    Serial.print(':');
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
    s = (epoch % 60);
  }

             
}


void uhrzeit() {
  s +=1;
  if(s>=60){ s=s-60; m +=1;}
  if(m==60){ m=0; h +=1;}
  if(h==24){h=1;}
  Stunde = ((int(h/10)) + (h%10));
  Minute =  ((int(m/10)) + (m%10));
  Sekunde = ((int(s/10)) + (s%10));
//  Serial.print(int(h/10));
//  Serial.print(h%10);
//  Serial.print(":"); 
//  Serial.print(int(m/10));
//  Serial.print(m%10);
//  Serial.print(":");
//  Serial.print(int(s/10));
//  Serial.println(s%10);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// hier war mqtt emfang






void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
}








void printoled(){
  
// oled.setFont(fixed_bold10x15); // Auswahl der Schriftart
//  oled.clear(); //Löschen der aktuellen Displayanzeige
//  oled.println("TGS 1 2 3 4"); //Text in der ersten Zeile. "Println" sorgt dabei für einen Zeilensprung.
//  oled.print(" ");
//  oled.print(StateEingang_TGS); // Text in der zweiten Zeile. Da es keine dritte Zeile gibt, wird hier kein Zeilenumsprung benötigt.
//  oled.print("  ");
//  
//  oled.print(StateEingang_S1); // Text in der zweiten Zeile. Da es keine dritte Zeile gibt, wird hier kein Zeilenumsprung benötigt.
//  oled.print(" ");
//  oled.print(StateEingang_S2); // Text in der zweiten Zeile. Da es keine dritte Zeile gibt, wird hier kein Zeilenumsprung benötigt.
//  oled.print(" ");
//  oled.print(StateEingang_S3); // Text in der zweiten Zeile. Da es keine dritte Zeile gibt, wird hier kein Zeilenumsprung benötigt.
//  oled.print(" ");
//  oled.println(StateEingang_S4); // Text in der zweiten Zeile. Da es keine dritte Zeile gibt, wird hier kein Zeilenumsprung benötigt.
//   oled.print("Mode-> AUTO");
//  oled.println("");
//  oled.print("Pumpe -->");
//  oled.print(relay);
//  
  //delay (2000);

  }




void checksonde(){
                              error = 0;
                                    // if (StateEingang_S5 == 1 and  StateEingang_S4 == 0) error = 1;
                                    if (StateEingang_S4 == 1 and  StateEingang_S3 == 0) error = 2;
                                    if (StateEingang_S3 == 1 and  StateEingang_S2 == 0) error = 3;
                                    if (StateEingang_S2 == 1 and  StateEingang_S1 == 0) error = 4;
                                    if (StateEingang_S1 == 1 and  StateEingang_TGS == 0) error = 5;
                        
                        if (error > 0) {
                                      
                                      switch (error) {
                                      case 0:
                                         // mqttClient.publish("Error","ok");
                                        break;
                                        case 1:
                                       mqttClient.publish("Error","sensorfehler S5");  // gibt es nicht
                                        break;
                                      case 2:
                                        mqttClient.publish("Error","sensorfehler S4");
                                        break;
                                        case 3:
                                        mqttClient.publish("Error","sensorfehler S3");
                                        break;
                                        case 4:
                                        mqttClient.publish("Error","sensorfehler S2");
                                        break;
                                        case 5:
                                        mqttClient.publish("Error","sensorfehler S1");
                                        break;
                                        case 6:
                                        mqttClient.publish("Error","sensorfehler TGS");
                                        break;    
                                      default:
                                         //mqttClient.publish("Error","ok");
                                        break; // Wird nicht benötigt, wenn Statement(s) vorhanden sind
                                    }
                         }                 
}










void pumpenrelay(int zeit)
{
 
   if (StateEingang_TGS == 1) {
                               if (pumpe_soll_laufen == true) 
                                 {
                                     //mqttClient.publish("Pumpe","on");
                                     // mqttClient.publish("state","on");  
                                    digitalWrite(Ausgang_pumpe, HIGH);
                                    // mqttClient.publish("Pumpe","on");
                                  }
                                  else
                                  {
                                     digitalWrite(Ausgang_pumpe, LOW);
                                     // mqttClient.publish("state","off");  
                                    // mqttClient.publish("Pumpe","off");
                                    //pumpe_soll_laufen =false;
                                  }
                                                             
                                }
                                else
                                {
                                 //  mqttClient.publish("state","off");  
                                  digitalWrite(Ausgang_pumpe, LOW);
                                  // pumpe_soll_laufen =false;
                                //  mqttClient.publish("Pumpe","off");
                                 }
}




void nachfuellen()
{
//  Serial.print("jetzt nachfüllen ");
// Serial.println(jetzt_nachfuellen);
//  Serial.print("ist fuelzeit ");
//Serial.println(ist_fuellzeit);
//  Serial.print("max fuelzeit ");
// Serial.println(max_fuellzeit);
//  Serial.print("ausgang wasserventil ");
// Serial.println(digitalRead(Ausgang_wasserventil));
// Serial.print("");
      
     if ((jetzt_nachfuellen == true) && (ist_fuellzeit < (max_fuellzeit * 60)) && (digitalRead(Ausgang_wasserventil) == LOW)){
               Serial.print("Nachspeiseventil auf, Ausgangs Pin: ");
               Serial.println(Ausgang_wasserventil);
               mqttClient.publish("Nachspeiseventil","1");
               mqttClient.publish("last_action","wasser nachfüllen ventil ein");
               digitalWrite(Ausgang_wasserventil, HIGH); 
          }
          
     if  (((digitalRead(Ausgang_wasserventil) == HIGH ) && (jetzt_nachfuellen == false)) || ((digitalRead(Ausgang_wasserventil) == HIGH) && (ist_fuellzeit > (max_fuellzeit * 60)))) {
               Serial.print("Nachspeiseventil zu, Ausgangs Pin: ");
               Serial.println(Ausgang_wasserventil);
               mqttClient.publish("Nachspeiseventil","0");
               mqttClient.publish("last_action","wasser nachfüllen ventil aus");
               digitalWrite(Ausgang_wasserventil, LOW); 
            }
}



  


void loop() {



// dies wird alle 24 std einmal ausgeführt um die füllzeit rückzusetzen   nur einmal alle 24 std füllen
 if (millis() - last_Del_Time > _24h){
    last_Del_Time = millis();
    Serial.println("24 std sind abgelaufen");
    ist_fuellzeit = 0;
  }  



// webserver hier aktivieren
 webserver();



//on for timer funktion nach xx minuten ausschalten 
 if ((millis() - previousMillis_on_for_timer > timerwert * 1000UL) && digitalRead(Ausgang_pumpe) == HIGH && fhem_on_for_timer_aktiv == true)  
  {
    Serial.println("off durch on_for_timer zeitablauf");
    digitalWrite(Ausgang_pumpe, LOW);
    mqttClient.publish("Pumpe","0");
    mqttClient.publish("state","off");
    pumpe_soll_laufen = false;
    fhem_on_for_timer_aktiv = false;
    timerwert = 0;
    pumpe_rest_laufzeit = 0;
  }

  


//if (!ethClient.connected()) {
//    Serial.println("Etherne not connected");
//       reconnect();
//}


//if (!mqttClient.connected()) {
//       Serial.println("mqtt not connected");
//       reconnect();
//  }







  state_buttonPinHand = !digitalRead(buttonPinHand);
  state_buttonPinAuto = !digitalRead(buttonPinAuto);
  state_buttonPinPumpeAn = !digitalRead(buttonPinPumpeAn);
  state_buttonPinPumpeAus = !digitalRead(buttonPinPumpeAus);
  // Analoger Füllstandssensor
  sensorwert =analogRead(Niveausonde_Analog); //Die Spannung am Drehregler wird auslesen und wie im vorherigen Sketch als Zahl zwischen 0 und 1023 unter der Variable „sensorwert“ gespeichert.
  // Serial.println(sensorwert);
  
  StateEingang_TGS = !digitalRead(Eingang_TGS);   
  StateEingang_S1 = !digitalRead(Eingang_S1);
  StateEingang_S2 = !digitalRead(Eingang_S2);
  StateEingang_S3 = !digitalRead(Eingang_S3);
  StateEingang_S4 = !digitalRead(Eingang_S4);
  //StateEingang_S5 = !digitalRead(Eingang_S5);


pumpenrelay(1);
nachfuellen();
incommingTimeNtp();


// ************************** Manuelle Taster Pumpe an und aus**************************
  state_buttonPinPumpeAn = !digitalRead(buttonPinPumpeAn);
  state_buttonPinPumpeAus = !digitalRead(buttonPinPumpeAus);

if (laststate_buttonPinPumpeAus == !state_buttonPinPumpeAus) {
                                           Serial.println("Pumpe aus taster gedrückt");
                                           mqttClient.publish("state","off");
                                           mqttClient.publish("Pumpe","0");
                                           mqttClient.publish("last_action","off durch Hardware Taster off");
                                           pumpe_soll_laufen = false;
                                       }
    laststate_buttonPinPumpeAus = state_buttonPinPumpeAus;                                   



// wenn hardware taster ein dann wird on for timer genutzt das pumpe max 3 std an ist
if (laststate_buttonPinPumpeAn == !state_buttonPinPumpeAn) {
                       
                       Serial.println("Pumpe ein taster gedrückt");
                                    if (StateEingang_TGS == 1) {
                                      mqttClient.publish("state","on");  
                                      mqttClient.publish("Pumpe","1");
                                                      
                             fhem_on_for_timer_aktiv = true;  // weil  max xx std an wenn manuell eingeschaltet
                             timerwert = laufzeit_pumpe_bei_tasterdruck * 60 ; // max 3 stunden an wenn manuel eingeschaltet
                             pumpe_rest_laufzeit = timerwert;
                             dtostrf(pumpe_rest_laufzeit/60, 1, 0, result); // Leave room for too large numbers!
                             mqttClient.publish("Pumpe_restlaufzeit",result);
                                  
                              Serial.print("on-for-timer ");
                              Serial.print(timerwert);
                              Serial.println(" durch Taster ein");
                            
                            previousMillis_on_for_timer = millis();   // save the time when we switched on the output
                            pumpe_soll_laufen = true;
                                   if (mqttClient.connect("Niv_Pool")) {
                                         mqttClient.publish("last_action","on-for-timer durch Hardware Taster on");
                                                                    }                       
                         }
                        }
 laststate_buttonPinPumpeAn = state_buttonPinPumpeAn; 

// ***********************************************************************************************************************

 
mqttClient.loop();




// *********************************************  Füllstands sensoren an fhem übermitteln und steueraufgaben ***********************

if (lastStateTGS == !StateEingang_TGS) {
                                          dtostrf(StateEingang_TGS, 1, 0, result); // Leave room for too large numbers!
                                          mqttClient.publish("TGS",result);
                                           if (StateEingang_TGS == 0){
                                             mqttClient.publish("last_action","kein Wasser (TGS1)");
                                             digitalWrite(Ausgang_pumpe, LOW);
                                                mqttClient.publish("state","off");  
                                                mqttClient.publish("Pumpe","0");
                                           } else if ( pumpe_soll_laufen == true) {                                               
                                            digitalWrite(Ausgang_pumpe, HIGH);
                                             mqttClient.publish("last_action","on Wasser wieder da");
                                             mqttClient.publish("state","on");  
                                             mqttClient.publish("Pumpe","1");
                                           }
                                           Serial.print("Eingang TGS: ");
                                           Serial.println(StateEingang_TGS);
                                       }

if (lastStateS1 == !StateEingang_S1) {
                                          dtostrf(StateEingang_S1, 1, 0, result); // Leave room for too large numbers!
                                          mqttClient.publish("S1",result);
                                         
                                           Serial.print("Eingang S1: ");
                                            Serial.println(StateEingang_S1);


                                            
                                            // Wenn sensor abfällt ist zu wenig wasser drin und sollte nachgefüllt werden
                                            if (!StateEingang_S1) {
                                            jetzt_nachfuellen = true;
                                            Serial.print("jetzt_nachfuellen = ");
                                            Serial.println(jetzt_nachfuellen);
                                            }
                                       }


if (lastStateS2 == !StateEingang_S2) {
                                          dtostrf(StateEingang_S2, 1, 0, result); // Leave room for too large numbers!
                                          mqttClient.publish("S2",result);
                                           
                                           Serial.print("Eingang S2: ");
                                           Serial.println(StateEingang_S2);
                                            // evtl negieren mit ! 
                                           if (StateEingang_S2) {
                                            jetzt_nachfuellen = false;
                                            }
                                       }

if (lastStateS3 == !StateEingang_S3) {
                                          dtostrf(StateEingang_S3, 1, 0, result); // Leave room for too large numbers!
                                          mqttClient.publish("S3",result);
                                          if (StateEingang_S3) jetzt_nachfuellen = false;   // zusätzlich als sicherheit könnte auch hier weg
                                            Serial.print("Eingang S3: ");
                                            Serial.println(StateEingang_S3);
                                       }



// Wenn S4 aktiv geschaltet wird dann pumpe für die eingestellte zeit (laufzeit_pumpe_uebervoll) laufen lassen
if (lastStateS4 == !StateEingang_S4) {
                                          dtostrf(StateEingang_S4, 1, 0, result); // Leave room for too large numbers!
                                          mqttClient.publish("S4",result);
                                          Serial.print("Eingang S4: ");
                                          Serial.println(StateEingang_S4);
                                           if (StateEingang_S3) jetzt_nachfuellen = false;    // zusätzlich als sicherheit könnte auch hier weg
                                     if ((error == 0) && (StateEingang_S4 == HIGH)) {
                                   Serial.print("Pumpe ein für ");
                                   Serial.print(laufzeit_pumpe_uebervoll);
                                   Serial.println(" minuten da übervoll");
                                   mqttClient.publish("state","on");  
                                     mqttClient.publish("Pumpe","1");
                                                      
                            fhem_on_for_timer_aktiv = true;  // weil  max 3 std an wenn manuell eingeschaltet
                            timerwert = laufzeit_pumpe_uebervoll * 60 ; // max 3 stunden an wenn manuel eingeschaltet
                             pumpe_rest_laufzeit = timerwert;
                             dtostrf(pumpe_rest_laufzeit/60, 1, 0, result); // Leave room for too large numbers!
                             mqttClient.publish("Pumpe_restlaufzeit",result);
                                  

                             
                              Serial.print("on-for-timer ");
                              Serial.print(timerwert);
                              Serial.println(" durch S4 (Schwallbehälter übervoll)");
                            
                            previousMillis_on_for_timer = millis();   // save the time when we switched on the output
                            pumpe_soll_laufen = true;
                                   if (mqttClient.connect("Niv_Pool")) {
                                         mqttClient.publish("last_action","on-for-timer durch S4 Schwallbehälter übervoll");
                                                                    }                       

                                     }
                                                                               


                                             
                                       }



    // pumpenrelay(1);

                                       
lastStateTGS = StateEingang_TGS;
lastStateS1 = StateEingang_S1;
lastStateS2 = StateEingang_S2;
lastStateS3 = StateEingang_S3;
lastStateS4 = StateEingang_S4;
// lastStateS5 = StateEingang_S5;


  // ********************************************************************************************

  
// *************************** Schleife zählt im secundentkt hoch bis 300  (5 min ) und dann wieder von vorne
// schleife läuft im secunden Takt
if (millis() > myTimeout + myTimer ) {
      myTimer = millis();
      
       
        i++;
        UhrzeitAktualisierung ++;
        if (UhrzeitAktualisierung == 20) {
          uhrzeit();
          UhrzeitAktualisierung = 0;
        }
              // Serial.print("i = ");
              // Serial.println(i);
//              Serial.print("pumpe_rest_laufzeit ");
//              Serial.println(pumpe_rest_laufzeit);
//              Serial.print("TGS eingang ");
//              Serial.println(StateEingang_TGS);  
                         if ((digitalRead(Ausgang_pumpe) == HIGH) && (StateEingang_TGS == 1)) {   
                                 pumpe_rest_laufzeit = pumpe_rest_laufzeit - 1;
                                     }
                                 
                        if ((i == 60) || (i==120) || (i==180) || (i==240) || (i==300)) {
                           
                           if ((digitalRead(Ausgang_pumpe) == HIGH) && (StateEingang_TGS == 1)) {   
                                 dtostrf(pumpe_rest_laufzeit/60, 1, 0, result); // Leave room for too large numbers!
                                 mqttClient.publish("Pumpe_restlaufzeit",result); delay(3);
                                 }    
                     }

        

       
             
       if ((digitalRead(Ausgang_wasserventil) == HIGH)) ist_fuellzeit++;  // ist_fuellzeit wird alle 24 std auf null gesetzt

       
       if ((i == 65) || (i==125) || (i==185)|| (i==245) || (i==305))  checksonde(); 
       if (i == 300) reconnect(); 
       if (i == 300) sendNTPpacket(timeServer); // send an NTP packet to a time server


        // Sonden status Seriell ausgeben
        if (i == 300){
              Serial.print("Eingang TGS: ");
              Serial.println(StateEingang_TGS);
              Serial.print("Eingang S1: ");
              Serial.println(StateEingang_S1);
              Serial.print("Eingang S2: ");
              Serial.println(StateEingang_S2);
              Serial.print("Eingang S3: ");
              Serial.println(StateEingang_S3);
              Serial.print("Eingang S4: ");
              Serial.println(StateEingang_S4);
            }


              
              


  


      // alle 300 sec alle sensoren an Fhem senden
         if (i == 300){
                 if (mqttClient.connect("Niv_Pool")) {
                                          dtostrf(StateEingang_TGS, 1, 0, result); // Leave room for too large numbers!
                                          mqttClient.publish("TGS",result); delay(10);
                                          dtostrf(StateEingang_S1, 1, 0, result); // Leave room for too large numbers!
                                          mqttClient.publish("S1",result);delay(10);
                                          dtostrf(StateEingang_S2, 1, 0, result); // Leave room for too large numbers!
                                          mqttClient.publish("S2",result);delay(10);
                                          dtostrf(StateEingang_S3, 1, 0, result); // Leave room for too large numbers!
                                          mqttClient.publish("S3",result);delay(10);
                                          dtostrf(StateEingang_S4, 1, 0, result); // Leave room for too large numbers!
                                          mqttClient.publish("S4",result);delay(10);
                                           
                                         
                                          if (digitalRead(Ausgang_wasserventil) == HIGH)   mqttClient.publish("Nachspeiseventil","1"); delay(3);
                                          if (digitalRead(Ausgang_wasserventil) == LOW)   mqttClient.publish("Nachspeiseventil","0"); delay(3);
                                          
                                 }    
                                          

                                          
                                            
                      }

          if (i >= 301) i = 0;
            

      }




//serielle daten
  if (input_string_complete == true) {                //check if data received
    inputstring.toCharArray(inputstring_array, 30);   //convert the string to a char array
    parse_cmd(inputstring_array);                     //send data to pars_cmd function
    input_string_complete = false;                    //reset the flag used to tell if we have received a completed string from the PC
    inputstring = "";                                 //clear the string
  }
  

//if (millis() > myTimeout + myTimer ) {
//    myTimer = millis();
//}


 
  
//  char result[8]; // Buffer big enough for 7-character float
//  dtostrf(u, 6, 2, result); // Leave room for too large numbers!
//  
//  if (mqttClient.connect("Niv_Pool")) {
//      Serial.println("Sende MQTT pH-WertKorr");
//    mqttClient.publish("pH-wertKorr",result);
//    mqttClient.subscribe("inTopic");
//     }
//   
//    delay(5);  
  
  
  
  if (millis() > sensor_interval + myTimer_sensor ) {
    myTimer_sensor = millis();

    if ((sensorwert > alter_sensorwert +5) ||(sensorwert < alter_sensorwert -5))  {
          dtostrf(sensorwert, 1, 0, result); // Leave room for too large numbers!
          mqttClient.publish("SensorwertSchwall",result); delay(10);
       }
       
    if (sensorwert > alter_sensorwert + sensor_mindestanstieg) {
      Serial.println("Sprunghafter Wasseranstieg im Schwallbehälter");
      if (digitalRead(Ausgang_wasserventil) == HIGH){
        Serial.println("keine Aktion (Wasser zudosierung aktiv");
         } 
         else
             {
             if (StateEingang_TGS) {
                                              Serial.println("on durch sprungartigen Wasseranstieg in Schwallbehälter");
                                                    
                                      if (mqttClient.connect("Niv_Pool")) {
                                           mqttClient.publish("last_action","on");
                                           mqttClient.publish("Pumpe","1");
                                           mqttClient.publish("state","on");
                                         }
                                       fhem_on_for_timer_aktiv = true;  // weil  max 3 std an wenn manuell eingeschaltet
                                       timerwert = laufzeit_pumpe_wasserschwall * 60 ; // max 3 stunden an wenn manuel eingeschaltet
                                       pumpe_rest_laufzeit = timerwert;   // Anzeige in fhem

                                       itoa(pumpe_rest_laufzeit, cstr, 10); 
                                       mqttClient.publish("Pumpe_restlaufzeit",cstr);  delay(10);

                                     
                                     if (mqttClient.connect("Niv_Pool")) {
                                            itoa(pumpe_rest_laufzeit, cstr, 10); 
                                              mqttClient.publish("pumpe_rest_laufzeit",cstr);  delay(10);
                                              mqttClient.publish("last_action","sprungartiger Wasseranstieg im schwallwasser Behälter");
                                               }                         
                                       
                                                Serial.print("on ");
                                                Serial.print(timerwert);
                                                Serial.println(" max minuten durch sprungartiger Wasseranstieg im schwallwasser Behälter"); 
                                              
                                              previousMillis_on_for_timer = millis();   // save the time when we switched on the output
                                              pumpe_soll_laufen = true;
                  
                                       
                                             if (StateEingang_TGS == LOW) {
                                                mqttClient.publish("last_action","kein Wasser (TGS)"); delay(10);
                                                                    }
                                   }
                   }
      
                                                          Serial.print("Alter Wert: ");
                                                          Serial.println(alter_sensorwert);
                                                          Serial.print("Neuer Wert: ");
                                                          Serial.println(sensorwert);
                                                    }
    alter_sensorwert = sensorwert;
  }
  
 

  
    if (millis() > myTimeoutrelay + myTimerrelay ) {
    myTimerrelay = millis();
        // pumpenrelay(1);
              }


}






// ****************************************Daten über die serielle schnittstelle  ****************************************************
void parse_cmd(char* string) {                      //For calling calibration functions
  strupr(string);                                   //convert input string to uppercase
Serial.println(char(strcmp));
  if (strcmp(string, "CAL,4") == 0) {               //compare user input string with CAL,4 and if they match, proceed
                                     //call function for low point calibration
    Serial.println("LOW CALIBRATED");
        if (mqttClient.connect("Niv_Pool")) {
           mqttClient.publish("last_action","LOW CALIBRATED 4");
           mqttClient.subscribe("inTopic");
           }
  
  }
  else if (strcmp(string, "CAL,7") == 0) {          //compare user input string with CAL,7 and if they match, proceed
                                //call function for midpoint calibration
    Serial.println("MID CALIBRATED");
         if (mqttClient.connect("Niv_Pool")) {
           mqttClient.publish("last_action","MID CALIBRATED 7");
           mqttClient.subscribe("inTopic");
           }

    
  }
  else if (strcmp(string, "CAL,10") == 0) {         //compare user input string with CAL,10 and if they match, proceed
                                 //call function for highpoint calibration
    Serial.println("HIGH CALIBRATED");
      if (mqttClient.connect("Niv_Pool")) {
           mqttClient.publish("last_action","HIGH CALIBRATED 10");
           mqttClient.subscribe("inTopic");
           }
  }
  else if (strcmp(string, "CAL,CLEAR") == 0) {      //compare user input string with CAL,CLEAR and if they match, proceed
                                  //call function for clearing calibration
    Serial.println("CALIBRATION CLEARED");

     if (mqttClient.connect("Niv_Pool")) {
           mqttClient.publish("last_action","CALIBRATION CLEARED");
           mqttClient.subscribe("inTopic");
           }
  }
}



char* ip2CharArray(IPAddress IP) {
  static char a[16];
  sprintf(a, "%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);
  return a;
}






/*
Fhem

attr MQTT2_Niv_Pool setlist 
reset NivPool_inTopic reset
on NivPool_inTopic on
off NivPool_inTopic off
on-for-timer NivPool_inTopic on-for-timer
max_fuellzeit NivPool_inTopic max_fuellzeit
laufzeit_pumpe_uebervoll NivPool_inTopic  laufzeit_pumpe_uebervoll
laufzeit_pumpe_bei_tasterdruck NivPool_inTopic  laufzeit_pumpe_bei_tasterdruck
laufzeit_Pumpe_bei_fhem_on NivPool_inTopic laufzeit_Pumpe_bei_fhem_on
sensor_mindestanstieg NivPool_inTopic sensor_mindestanstieg
sensor_interval NivPool_inTopic sensor_interval
laufzeit_pumpe_wasserschwall NivPool_inTopic laufzeit_pumpe_wasserschwall



attr webCmd reset:on:off

*/
