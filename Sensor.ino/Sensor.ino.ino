#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

IPAddress ip (192,168,4,1);  //ip address of AP
WiFiUDP udp;
char incomingPacket[255];   //buffer for storing incoming packet
char buf[50];
int flag=0;
int moisturevalue=0;
void setup(){
   Serial.begin(9600);
   delay(10);
   WiFi.mode(WIFI_STA);
   pinMode(A0,INPUT);// taking input from moisture sensor
   while(1)
   {
    {
      WiFi.begin("ESP","damnitsworking");// WiFi.begin("ssid","password"); 
        Serial.print("Connecting to AP");
        while(WiFi.status() != WL_CONNECTED)
        {
          Serial.print("."); //waiting for connection
          delay(500);
        }
      break;
    }
   }

 
   Serial.println("WiFi connected");
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
   Serial.println("Moisture---------------\n\n");
   udp.begin(8100); //add 100 to get a new port

}
 
void loop(){
  //Start of Program
     moisturevalue=analogRead(A0);
    if(WiFi.status()!=WL_CONNECTED)
    {
      ESP.restart(); // trying to connect to the UDP Servr.
    }
    String m=String(moisturevalue);
    
    Serial.println(moisturevalue);
    m.toCharArray(buf,m.length()+1);
    Serial.println("my string in buf is ");
    Serial.println(buf);
  
    udp.beginPacket(ip, 8100); // remote IP and port
    udp.write(buf);
    udp.endPacket();
    //i = 0;
    int packetSize = udp.parsePacket();
   // to recieve conformation from server when data is recieved 
    if(packetSize)
    {
      Serial.printf("received reply from %s, port %d\n",udp.remoteIP().toString().c_str(), udp.remotePort());
      int len = udp.read(incomingPacket, 255);
     
      if(len>0)
      {
        incomingPacket[len]=0;
      }
      Serial.printf("udp reply: %s\n",incomingPacket);
      Serial.println(buf);
      delay(100);
    //Serial.println(count);
    //Serial.println(val1);
     flag++;
     Serial.println(flag);
    }
    // to send Sensor node to the Sleeping mode
    if(flag==1)
    {
      Serial.println("Entering DeepSleep Mode for 1800 sec(30min)");
      ESP.deepSleep(1800e6);// DeepSleep Mode for 1800 sec(30min)
    }
}
