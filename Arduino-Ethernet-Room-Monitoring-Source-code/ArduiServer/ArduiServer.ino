#include <SPI.h>
#include <Ethernet.h>

// ethernet configuration
byte mac[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
IPAddress ip(192,168,1,111); // P1 --> { 10, 1, 1, 5 };
EthernetServer server(3178);              // port 80 is default for HTTP

// initial
int LED = 3;          // led is connected to digital pin 3
int PIR = 2;          // PIR sensor is connected to digital pin 2
int LDR = 5;          // LDR sensor is connected to analog in 5
int PIRstate = 0;     // variable for PIR sensor status
float photocell = 0;  // variable for photocell (LDR) analog value
char c = 0;           // received data
char command[2] = "\0";  // command


void setup()
{
  Ethernet.begin(mac, ip);
  server.begin();
  pinMode(LED, OUTPUT);
  pinMode(PIR, INPUT);
}


void loop()
{
  EthernetClient client = server.available();
  // detect if current is the first line
  boolean current_line_is_first = true;

  if (client) {
    // an http request ends with a blank line
    boolean current_line_is_blank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if we've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so we can send a reply
        if (c == '\n' && current_line_is_blank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          // auto reload webpage every 5 second
          client.println("<META HTTP-EQUIV=REFRESH CONTENT=5 URL=>");
          
          // webpage title
          client.println("<center><p><h1>Arduino + Ethernet Shield Application v1.0</h1></p><center><hr><br />");

          // read analog pin 1 for the value of photocell
          photocell = analogRead(LDR);
          client.print("<p><h2>Light reading = <font color=indigo>");
          client.println(photocell, 2);
          client.println("</font></h2></p>");
          
          // read digital pin 13 for the state of PIR sensor
          PIRstate = digitalRead(2);
          if (PIRstate == HIGH) { // PIR sensor detected movement
            client.println("<p><h2><font color=red>Motion Detected!</font></h2></p>");
          }
          else { // No movement is detected
            client.println("<p><h2><font color=green>No Movement</font></h2></p>"); 
          }  
          
          // button functions
          client.println("<form  method=get name=form>");
          client.println("<button name=b value=1 type=submit style=height:80px;width:150px>LED On</button>");
          client.println("<button name=b value=2 type=submit style=height:80px;width:150px>LED Off</button>");
          client.println("</form><br />");
          
          // webpage footer
          client.println("<hr><center><a href=http://www.robothead2toe.com.my>Robot.Head to Toe</a><br />");
          client.println("<p>P.S.: This page will automatically refresh every 5 seconds.</p></center>");
          
          break;
        }
        if (c == '\n') {
          // we're starting a new line
          current_line_is_first = false;
          current_line_is_blank = true;
        } 
        else if (c != '\r') {
          // we've gotten a character on the current line
          current_line_is_blank = false;
        }
        // get the first http request
        if (current_line_is_first && c == '=') {
          for (int i = 0; i < 1; i++) {
            c = client.read();
            command[i] = c;
          }
          // LED control
          if (!strcmp(command, "1")) {
            digitalWrite(LED, HIGH);
          }
          else if (!strcmp(command, "2")) {
            digitalWrite(LED, LOW);
          }
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    client.stop();
  }
}

