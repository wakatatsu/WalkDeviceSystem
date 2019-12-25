#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
 
const char *ssid = "ESP32WiFi";
const char *password = "12345678";
IPAddress ip(192, 168, 1, 4);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
 
/* TCP server at port 80 will respond to HTTP requests */
WiFiServer server(80);
 
///////////////////////////
void setup() {
  Serial.begin(115200);
  
  
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAPConfig(ip, gateway, subnet);
  delay(100);
  //アクセスポイントを起動する
  WiFi.softAP(ssid, password);
  delay(100);
 
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //
  //サーバーセットアップ
  //
  /* Set up mDNS */
  if (!MDNS.begin("esp32")) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  Serial.println("mDNS responder started");
  /* Start Web Server server */
  server.begin();
  Serial.println("Web server started");
 
  /* Add HTTP service to MDNS-SD */
  MDNS.addService("http", "tcp", 80);
}
 
void loop() {
  /* Check if a client has connected */
    WiFiClient client = server.available();
    if (!client) {
        return;
    }
    Serial.println("");
    Serial.println("New client");
    if (client) {                   
      Serial.println("new client");         
      /* check client is connected */           
      while (client.connected()) {     
          /* client send request? */     
          if (client.available()) {
              /* request end with '\r' -> this is HTTP protocol format */
              String req = client.readStringUntil('\r');
              /* First line of HTTP request is "GET / HTTP/1.1"  
                here "GET /" is a request to get the first page at root "/"
                "HTTP/1.1" is HTTP version 1.1
              */
              /* now we parse the request to see which page the client want */
              int addr_start = req.indexOf(' ');
              int addr_end = req.indexOf(' ', addr_start + 1);
              if (addr_start == -1 || addr_end == -1) {
                  Serial.print("Invalid request: ");
                  Serial.println(req);
                  return;
              }
              req = req.substring(addr_start + 1, addr_end);
              Serial.print("Request: ");
              Serial.println(req);
              client.flush();
          
              String s;
              /* if request is "/" then client request the first page at root "/" -> we process this by return "Hello world"*/
              if (req == "/")
              {
                  IPAddress ip = WiFi.localIP();
                  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
                  s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP32 at ";
                  s += ipStr;
                  s += "</html>\r\n\r\n";
                  Serial.println("Sending 200");
              }
              else
              {
                  /* if we can not find the page that client request then we return 404 File not found */
                  s = "HTTP/1.1 404 Not Found\r\n\r\n";
                  Serial.println("Sending 404");
              }
              /* send response back to client and then close connect since HTTP do not keep connection*/
              client.print(s);
              client.stop();
          
            }
        }          
    }
    Serial.println("Done with client");
}
