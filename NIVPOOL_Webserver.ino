String buffer = "";


void webserver(){
// listen for incoming clients
  EthernetClient ethClient = websrv.available();
if (ethClient) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (ethClient.connected()) {
      if (ethClient.available()) {
        char c = ethClient.read();
         buffer+=c;
         Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
       
        if (c == '\n' && currentLineIsBlank) {

          while(ethClient.available())
          {
             Serial.write(ethClient.read());
          }
          Serial.println();

          
          // send a standard http response header
          ethClient.println("HTTP/1.1 200 OK");
          ethClient.println("Content-Type: text/html");
          ethClient.println("Connection: close");  // the connection will be closed after completion of the response
          ethClient.println("Refresh: 300 ");  // refresh the page automatically every 5 sec
          ethClient.println();
          ethClient.println("<!DOCTYPE HTML><html lang='de'>");
          ethClient.println("<head>");
          ethClient.println("<title>NIVPOOL Steuerung</title>");
          ethClient.println("</head> ");
          ethClient.println("<html>");
          // output the value of each analog input pin
          
          ethClient.print("<h1> NIVPool Jost Mario </h1>");
          ethClient.println("<br />"); 
          ethClient.println("<br />"); 
          ethClient.println("<br />"); 
          ethClient.println("INFO"); 
          ethClient.println("<br />"); 
          ethClient.print("max_fuellzeit = ");
          ethClient.println(max_fuellzeit);
          ethClient.println("<br />");
          ethClient.print("laufzeit_pumpe_uebervoll = ");
          ethClient.println(laufzeit_pumpe_uebervoll);
          ethClient.println("<br />");
          ethClient.print("laufzeit_pumpe_bei_tasterdruck = ");
          ethClient.println(laufzeit_pumpe_bei_tasterdruck);
          ethClient.println("<br />");
          ethClient.print("laufzeit_pumpe_bei_fhem_on = ");
          ethClient.println(laufzeit_pumpe_bei_fhem_on);
           ethClient.println("<br />");
           ethClient.println("<br />");
           ethClient.println("<br />");
           
          ethClient.println("TGS = ");
          ethClient.print(digitalRead(Eingang_TGS));
          ethClient.println("<br />");
          ethClient.println("S1 = ");
          ethClient.print(digitalRead(Eingang_S1));
          ethClient.println("<br />");
          ethClient.println("S2 = ");
          ethClient.print(digitalRead(Eingang_S2));
          ethClient.println("<br />");
          ethClient.println("S3 = ");
          ethClient.print(digitalRead(Eingang_S3));
          ethClient.println("<br />");
          ethClient.println("S4 = ");
          ethClient.print(digitalRead(Eingang_S4));
          ethClient.println("<br />");
          ethClient.println("<br />");
          ethClient.println("<br />");


           ethClient.println(" Pumpenstatus: ");
           ethClient.println(digitalRead(Ausgang_pumpe));



           ethClient.println("<form action='/button' method='POST'>");
           ethClient.println("<button style='width:120px' name=\"status\" value=\"1\"> Pumpe Ein </button> ");
           ethClient.println("<button style='width:120px' name='status' value=\"2\"> Pumpe Aus </button> ");
           ethClient.println("<input type='text' name='TEXT' value='text1' size='20'><br><br>");
           ethClient.println("Zahl eingeben ");
           ethClient.println("<input type='text' name='TEXT2' value='wert' size='10'><br><br>");
           ethClient.println("<input type='submit' name='SUBMIT' value='senden'><br>");
           ethClient.println("</form></body></html>");


ethClient.println("<FORM ACTION=\"http://192.168.1.102:84\" METHOD=\"post\">");
ethClient.println("Name: <INPUT TYPE=\"TEXT\" NAME=\"Name\" VALUE=\"\" SIZE=\"25\" MAXLENGTH=\"50\"><BR>");
ethClient.println("Email: <INPUT TYPE=\"TEXT\" NAME=\"Email\" VALUE=\"\" SIZE=\"25\" MAXLENGTH=\"50\"><BR>");
ethClient.println("<INPUT TYPE=\"SUBMIT\" NAME=\"submit\" VALUE=\"Sign Me Up!\">");
ethClient.println("</FORM>");
ethClient.println("<BR>");



           
         ethClient.println("<br />");
       ethClient.println("</html>");
          
          for (int digitalChannel = 0; digitalChannel < 8; digitalChannel++) {
            int sensorReading = digitalRead(digitalChannel);
            float voltage = sensorReading * (5.0 / 1023.0);

            
            ethClient.print("digital input ");
            ethClient.print(digitalChannel);
            ethClient.print(" is     ");
            ethClient.print(sensorReading);
            ethClient.println("<br />");
          }
          ethClient.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r')
                 {

          
              
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    ethClient.stop();
    Serial.println("client disconnected");
  }
}
