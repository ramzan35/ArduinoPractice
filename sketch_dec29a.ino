#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
 
// Enter the IP address for Arduino, as mentioned we will use 192.168.0.16
// Be careful to use , insetead of . when you enter the address here
IPAddress ip(192,168,8,155);

int photocellPin = 0; // Analog input pin on Arduino we connected the SIG pin from sensor
int photocellReading; // Here we will place our reading

// Initialize the Ethernet server library
EthernetServer server(80);

void setup() {
 
 // Serial.begin starts the serial connection between computer and Arduino
  Serial.begin(9600);
 
 // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Arduino server IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
 
 
  photocellReading = random(10,20); // Fill the sensorReading with the information from sensor
 
  EthernetClient client = server.available();  // Listen for incoming clients
 
  if (client) {
    
   // When a client sends a request to a webserver, that request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
       // This line is used to send communication information between Arduino and your browser over Serial Monitor
        Serial.write(c);
        
       // When the request has ended send the client a reply
        if (c == '\n' && currentLineIsBlank) {
          
         // We send the HTTP response code to the client so it knows that we will send him HTML data
         // and to refresh the webpage every 5 seconds
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 5");
          client.println();
         // Here we write HTML data (for the page itself) which will be sent to the client.
         // The HTML includes Javascript which fills the data
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Arduino sensor data</title>");
          client.println("<script>");
          client.println("window.onload=function rfsh(){");
          client.println("document.getElementById('value').innerHTML =");
          client.print(photocellReading);
          client.println(";}");
          client.println("</script>");
          client.println("</head>");
          client.println("<body>");
          client.println("<br>");
          client.println("<h1>Light measured from the sensor is:</h1> ");
          client.println("<p id='value'></p>");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // Check if a new line is started
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // If a new line was not strated
          currentLineIsBlank = false;
        }
      }
    }
    // Give the client some time to recieve the data (1ms)
    delay(100);
    // In the end close the connection
    client.stop();
  }
}
