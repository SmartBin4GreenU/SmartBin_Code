/*
  Programer : Sarawut Waiyakorn  5635512044
  Project: SmartBin4GreenUniversity
*/

#include <NewPingESP8266.h>
#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>

/**********************************************************************/

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>    

/**********************************************************************/

#define FIREBASE_HOST "sb-fgu.firebaseio.com"
#define FIREBASE_AUTH "1CMvHocG4TPd2ivn1YTWKDcI7B9f3AMLNGI0vKHz"

///*Config  connect to Firebase*/
//#define FIREBASE_HOST "smartbin4greenuniversity.firebaseio.com"
//#define FIREBASE_AUTH "qeparmANzkz2h60KJ986rT1Y3D3VsvfpCCylbNdR"

#define TRIGGER_PIN  D3
#define ECHO_PIN     D4
#define MAX_DISTANCE 300
#define TRIGGER_PIN2  D5
#define ECHO_PIN2     D6
#define MAX_DISTANCE2 300

/*********************************************************************/
NewPingESP8266 sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
NewPingESP8266 sonar2(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCE2);
 
void setup() 
{ 
     // attaches the servo on GIO2 to the servo object 
      Serial.begin(115200);  
      pinMode(LED_BUILTIN,OUTPUT);
      WiFiManager wifiManager;
      wifiManager.autoConnect("SB_FGU Ultrasonic AP1");
      while (WiFi.status() != WL_CONNECTED) 
      {
           delay(250);
           Serial.print(".");
      }
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
        
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
} 
int MAX = 10;
int Dis1[10] = {};
int Dis2[10] = {};
int Count = 0;
float Sum[10] = {};
int Average = 0;
int Round = 0;

void loop() 
{ 
   unsigned int Distance1   = sonar.ping_cm(); 
   unsigned int Distance2   = sonar2.ping_cm();
   
   
   Serial.print("Ping_1: ");
   Serial.print(Distance1); // Send ping, get distance in cm and print result (0 = outside set distance range)
   Serial.print(" cm"); 
   Serial.print("    ");
   Serial.print("Ping_2: ");
   Serial.print(Distance2); // Send ping, get distance in cm and print result (0 = outside set distance range)
   Serial.print(" cm");  
   Serial.println("  ");
   
     if(Count >= MAX ) {
          for(int i = 0; i < MAX; i++){
              Sum[i] = Dis1[i] + Dis2[i];
              Average += Sum[i]/2;
          }
          Average = Average/MAX;
          Serial.print("Average : ");
          Serial.println(Average);
          Firebase.setInt("/Ultrasonic/Distance",Average);
          Count = 0;
          Round = 0;
      }
    else {
        if(Round < MAX){
          Dis1[Round] = Distance1 ;
          Dis2[Round] = Distance2 ;
          Serial.print("Dis1["); 
          Serial.print(Round);
          Serial.print("]");
          Serial.print("  ==>  ");
          Serial.print(Dis1[Round]);     
          Serial.print("      ");
          Serial.print("Dis2["); 
          Serial.print(Round);
          Serial.print("]");  
          Serial.print("  ==>  ");
          Serial.println(Dis2[Round]);
        }
        Round++;
      }
     Count++;
     delay(1000);
} 

