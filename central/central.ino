#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
String apiKey = "RZ6VWJ5T3P8350QH";  // api key of Thingsspeak channel
int x = 0; // shifting between sensor node and internet 
unsigned long tim;
char pass[255];
int l;
int flag = 0; //making esp sleep
void ThingSpeak();// function to send data to the Thingspeak
// Access point

const char *ssid_ap = "ESP";   //SSID of the central node
const char *password_ap = "damnitsworking"; // its password
const char* server = "api.thingspeak.com";
int convertedvalue;
WiFiClient client;
WiFiUDP Udp[7];  // 7 is the number of sensor nodes....can be changed
unsigned int localUdpPort ;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacekt[] = "H";  // a reply string to send back
int j=0;

void configAP()
// to config central node as acces point
{
  WiFi.mode(WIFI_AP);
  Serial.printf("Status of ESP is %d",WiFi.mode(WIFI_AP));

  WiFi.softAP(ssid_ap,password_ap);

  IPAddress ip = WiFi.softAPIP();

  Serial.print("Ip address of AP is :");
  Serial.print(ip);
  int i , port =8000;
// loop to recieve the dada from 7 sensors nodes
  for(i=-0;i<7;i++){
    Udp[i].begin(port);
    port += 100;
  }
  Serial.println("started listening");
}






// As client

IPAddress ip (192,168,4,1);  //ip address of the central node
WiFiUDP udp;

uint8_t buf1[50];
//uint8_t i1=0;
const char *ssid = "HUAWEI-A8F0";               // SSID of your home WiFi
const char *password = "agnext123";

void configClient()
{
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid,password);
  Serial.print("Waiting for WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    x=0;
    j++;
    if (j==9) return;
  }

 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());

 udp.begin(localUdpPort);
 delay(500);  
}




void setup() {
  // put your setup code here, to run once:
  pass[0] = 0;
 pinMode(D7,OUTPUT);// Relay in put
 digitalWrite(D7,HIGH); // to close the solinoid valve initially
  delay(100);
  Serial.begin(9600);// for debug
}




void loop() {

  if(x == 0){

    configAP();
       int i ,packetSize , port =8000;
       for(i=0;i<7;i++){         // 7 is the number os sensor nodes
        packetSize = Udp[i].parsePacket();
    
        if (packetSize)
        {
          // receive incoming UDP packets
          localUdpPort = port + 100*i ;
          Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp[i].remoteIP().toString().c_str(), Udp[i].remotePort());
          int len = Udp[i].read(incomingPacket, 255);
          // check for input from all sensor nodes and stores the data in incoming Packet
          if (len > 0)
          {
            incomingPacket[len] = 0;
          }
          Serial.printf("UDP packet contents: %s\n", incomingPacket);
          strcpy(pass,incomingPacket);
          l = strlen(pass);
          
          
          // send back a reply, to the IP address and port we got the packet from
          Udp[i].beginPacket(Udp[i].remoteIP(), Udp[i].remotePort());
          Udp[i].write(replyPacekt);
          Udp[i].endPacket();
          x=1; // condition to switch as client
          // Controlling of Solenoid valve according to the sensor input
           if (atoi(incomingPacket) > 800 && atoi(incomingPacket) <= 1026) 
           {
            digitalWrite(D7, LOW);
              }
            else if ( atoi(incomingPacket) >0 && atoi(incomingPacket) < 800) 
            {
            digitalWrite(D7, HIGH);
            Serial.println("now im in control");
            
           }
        }
       }
  }


 
  // Remove below comment to connect to the router and to send data to the Things speak
 /*else {
    
      Serial.println("Client mode:");
      configClient();

    if(WiFi.status() != WL_CONNECTED) {
      configClient();
    }
    ThingSpeak();
        // now send to WiFi:  
        
     
}*/
}
void ThingSpeak()
{
              if (isnan((int)incomingPacket))//checking for input
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }

                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            // concatination of string which needs to send to the Thingspeak
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(incomingPacket);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
                             Serial.print(postStr);
                             Serial.print("moisture: ");
                             Serial.print(incomingPacket);
                             Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(10000);
}

