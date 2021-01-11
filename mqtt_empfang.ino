// ************************         hier kommen die MQTT befehle von Fhem an **********************************************

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  payload[length] = '\0';
  String message = (char*)payload;

   Serial.print("payload: [");
   Serial.print((char *)payload);
   Serial.println("]");
 
 
 //Serial.println(message);
 //Serial.println(message);




 

        if(message.substring(0, 12)== "on-for-timer")
        {
          fhem_on_for_timer_aktiv = true;
          Laufzeit = message.substring(13);
          timerwert = Laufzeit.toInt() * 60;
        
           Serial.print("laufzeit: ");
           Serial.println(Laufzeit);
           Serial.println(" min on-for-timer");
          previousMillis_on_for_timer = millis();   // save the time when we switched on the output
                                         
                                         Serial.println("connectedmqtt"); delay(80);
                                         mqttClient.publish("last_action","on-for-timer"); 
                                         mqttClient.publish("Pumpe","1"); 
                                         mqttClient.publish("state","on"); 

                                         
                    Serial.print("tgs eingang: ");
                    Serial.println(StateEingang_TGS);
                    
                                     
                                        
                                          
                                  
                                  pumpe_soll_laufen = true;
                                  pumpe_rest_laufzeit = timerwert;
                                 
                                  if (mqttClient.connect("Niv_Pool")) {   
                                  dtostrf(pumpe_rest_laufzeit/60, 1, 0, result); // Leave room for too large numbers!
                                  mqttClient.publish("Pumpe_restlaufzeit",result);
                                  }

                                  
                           if (StateEingang_TGS == 0) {
                                 mqttClient.publish("last_action","on for timer kein Wasser (TGS)");
                                 mqttClient.publish("state","off");
                                 mqttClient.publish("Pumpe","0");  
                               }
                               else
                               {
                                 
                               }
          }



  // in fhem als minuten angeben wird dann hier mal 60 in secunden gewandelt
      if(message.substring(0, 13)== "max_fuellzeit")
        {
          max_fuellzeit = message.substring(14).toInt() ;
          
          Serial.print("max_fuellzeit: ");
          Serial.println(max_fuellzeit);
          
          
          
          
          //SetWertEEPROM(EEPROM_max_fuellzeit, max_fuellzeit); 
          // EEPROM.write(0, max_fuellzeit);
          eepromWriteInt(EEPROM_max_fuellzeit,max_fuellzeit); 
           
           
           Serial.println("max_fuellzeit ins eeprom geschrieben");
                            if (mqttClient.connect("Niv_Pool")) {
                            mqttClient.publish("last_action","max_fuellzeit set by fhem");
                            itoa(max_fuellzeit, cstr, 10); 
                               mqttClient.publish("max_fuellzeit",cstr);  delay(50);
                                               }                         
             }








// wert wieviel die sonde in sensor_intervall ansteigen muss um pumpe einzuschalten
      if(message.substring(0, 21)== "sensor_mindestanstieg")
        {
          sensor_mindestanstieg = message.substring(22).toInt() ;
          Serial.print("sensor_mindestanstieg: ");
          Serial.println(sensor_mindestanstieg);
          eepromWriteInt(EEPROM_sensor_mindestanstieg,sensor_mindestanstieg); 
          Serial.println("sensor_mindestanstieg ins eeprom geschrieben");
                            if (mqttClient.connect("Niv_Pool")) {
                            mqttClient.publish("last_action","sensor_mindestanstieg set by fhem");
                            itoa(sensor_mindestanstieg, cstr, 10); 
                               mqttClient.publish("sensor_mindestanstieg",cstr);  delay(50);
                                               }                         
             }







      if(message.substring(0, 15)== "sensor_interval")
        {
          sensor_interval = message.substring(16).toInt() ;
          Serial.print("sensor_interval: ");
          Serial.println(sensor_interval);
          eepromWriteInt(EEPROM_sensor_interval,sensor_interval); 
          Serial.println("sensor_interval ins eeprom geschrieben");
                            if (mqttClient.connect("Niv_Pool")) {
                            mqttClient.publish("last_action","sensor_interval set by fhem");
                            itoa(sensor_interval, cstr, 10); 
                               mqttClient.publish("sensor_interval",cstr);  delay(50);
                                               }                         
             }










  if(message.substring(0, 24)== "laufzeit_pumpe_uebervoll")
        {
          laufzeit_pumpe_uebervoll = message.substring(25).toInt();
          
          Serial.print("laufzeit_pumpe_uebervoll: ");
          Serial.println(laufzeit_pumpe_uebervoll);
                 
          
          //SetWertEEPROM(EEPROM_max_fuellzeit, max_fuellzeit); 
          // EEPROM.write(0, max_fuellzeit);
          eepromWriteInt(EEPROM_laufzeit_pumpe_uebervoll,laufzeit_pumpe_uebervoll); 
           
           
           Serial.println("laufzeit_pumpe_uebervoll ins eeprom geschrieben");
                            if (mqttClient.connect("Niv_Pool")) {
                            mqttClient.publish("last_action","laufzeit_pumpe_uebervoll set by fhem"); delay(50);
                               itoa(laufzeit_pumpe_uebervoll, cstr, 10); 
                               mqttClient.publish("laufzeit_pumpe_uebervoll",cstr);  delay(50);
                                               }                         
             }





 if(message.substring(0, 30)== "laufzeit_pumpe_bei_tasterdruck")
        {
          laufzeit_pumpe_bei_tasterdruck = message.substring(31).toInt();
          
          Serial.print("laufzeit_pumpe_bei_tasterdruck: ");
          Serial.println(laufzeit_pumpe_bei_tasterdruck);
                 
          
          //SetWertEEPROM(EEPROM_max_fuellzeit, max_fuellzeit); 
          // EEPROM.write(0, max_fuellzeit);
          eepromWriteInt(EEPROM_laufzeit_pumpe_bei_tasterdruck,laufzeit_pumpe_bei_tasterdruck); 
           
           
           Serial.println("laufzeit_pumpe_bei_tasterdruck ins eeprom geschrieben");
                            if (mqttClient.connect("Niv_Pool")) {
                            mqttClient.publish("last_action","laufzeit_pumpe_bei_tasterdruck set by fhem"); delay(50);
                               itoa(laufzeit_pumpe_bei_tasterdruck, cstr, 10); 
                               mqttClient.publish("laufzeit_pumpe_bei_tasterdruck",cstr);  delay(50);
                                               }                         
             }

  

                               
if(message.substring(0, 26)== "laufzeit_pumpe_bei_fhem_on")
        {
          laufzeit_pumpe_bei_fhem_on = message.substring(27).toInt();
          
          Serial.print("laufzeit_pumpe_bei_fhem_on: ");
          Serial.println(laufzeit_pumpe_bei_fhem_on);
                 
          
          //SetWertEEPROM(EEPROM_laufzeit_pumpe_bei_fhem_on, laufzeit_pumpe_bei_fhem_on); 
          // EEPROM.write(0, max_fuellzeit);
          eepromWriteInt(EEPROM_laufzeit_pumpe_bei_fhem_on,laufzeit_pumpe_bei_fhem_on); 
           
           
           Serial.println("laufzeit_pumpe_bei_fhem_on ins eeprom geschrieben");
                            if (mqttClient.connect("Niv_Pool")) {
                            mqttClient.publish("last_action","laufzeit_pumpe_bei_fhem_on set by fhem"); delay(50);
                               itoa(laufzeit_pumpe_bei_fhem_on, cstr, 10); 
                               mqttClient.publish("laufzeit_pumpe_bei_fhem_on",cstr);  delay(50);
                                               }                         
             }

  


if(message.substring(0, 28)== "laufzeit_pumpe_wasserschwall")
        {
          laufzeit_pumpe_wasserschwall = message.substring(29).toInt();
          
          Serial.print("laufzeit_pumpe_wasserschwall: ");
          Serial.println(laufzeit_pumpe_wasserschwall);
                 
          
          //SetWertEEPROM(EEPROM_laufzeit_pumpe_bei_fhem_on, laufzeit_pumpe_bei_fhem_on); 
          // EEPROM.write(0, max_fuellzeit);
          eepromWriteInt(EEPROM_laufzeit_pumpe_wasserschwall,laufzeit_pumpe_wasserschwall); 
           
           
           Serial.println("laufzeit_pumpe_wasserschwall ins eeprom geschrieben");
                            if (mqttClient.connect("Niv_Pool")) {
                            mqttClient.publish("last_action","laufzeit_pumpe_wasserschwall set by fhem"); delay(50);
                               itoa(laufzeit_pumpe_wasserschwall, cstr, 10); 
                               mqttClient.publish("laufzeit_pumpe_wasserschwall",cstr);  delay(50);
                                               }                         
             }








 
              if (message.substring(0, 5) == "reset")
             {
               error = 0;
               ist_fuellzeit = 0;
               Serial.println("reset");
               if (mqttClient.connect("Niv_Pool")) {
                     mqttClient.publish("Error","reset by MQTT");
                      mqttClient.publish("last_action","reset by MQTT");
                     mqttClient.publish("Error","ok");
                     }
            }

        
          
          
          if (message == "on")
            {
               Serial.println("on");
                                  
               if (mqttClient.connect("Niv_Pool")) {
                   mqttClient.publish("last_action","on");
                   mqttClient.publish("Pumpe","1");
                   mqttClient.publish("state","on");
                       }
                     fhem_on_for_timer_aktiv = true;  // weil  max 3 std an wenn manuell eingeschaltet
                     timerwert = laufzeit_pumpe_bei_fhem_on * 60 ; // max 3 stunden an wenn manuel eingeschaltet


                                   pumpe_rest_laufzeit = timerwert;
                                 
                                  if (mqttClient.connect("Niv_Pool")) {   
                                  dtostrf(pumpe_rest_laufzeit/60, 1, 0, result); // Leave room for too large numbers!
                                  mqttClient.publish("Pumpe_restlaufzeit",result);
                                  }
                            
                              Serial.print("on ");
                              Serial.print(timerwert);
                              Serial.println(" max minuten durch fhem on");
                            
                            previousMillis_on_for_timer = millis();   // save the time when we switched on the output
                            pumpe_soll_laufen = true;

                            



                     
                           if (StateEingang_TGS == LOW) {
                              mqttClient.publish("last_action","kein Wasser (TGS)");
                              mqttClient.publish("state","wait");  
                           }
            }
            

          
            
            
            
              if (message.substring(0, 3) == "off")
            {
            Serial.println("Pumpe aus");
            pumpe_soll_laufen = false;
               if (mqttClient.connect("Niv_Pool")) {
                     mqttClient.publish("last_action","off");
                     mqttClient.publish("Pumpe","0");
                     mqttClient.publish("state","off");
                     mqttClient.subscribe("inTopic");
                     }
            }
  
  
}
