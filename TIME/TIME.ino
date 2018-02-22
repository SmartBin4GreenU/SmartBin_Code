#include <time.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>    
String Time_Stamps();
const String DAY[7] ={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
const String MONTH[12] ={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
const String SEC[60] = {"00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19",
                        "20","21","22","23","24","25","26","27","28","29","30","31","32","33","34","35","36","37","38","39",
                        "40","41","42","43","44","45","46","47","48","49","50","51","52","53","54","55","56","57","58","59"};                       
const String MIN[60] = {"00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19",
                        "20","21","22","23","24","25","26","27","28","29","30","31","32","33","34","35","36","37","38","39",
                        "40","41","42","43","44","45","46","47","48","49","50","51","52","53","54","55","56","57","58","59"};                       
const String HOUR[24] = {"00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19",
                         "20","21","22","23"};
int timezone = 7; 
char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";
char ntp_server4[20] = "3.asia.pool.ntp.org";
int dst = 0;
int st = 0;

void setup() 
{ 
     // attaches the servo on GIO2 to the servo object 
      Serial.begin(115200);  
      pinMode(LED_BUILTIN,OUTPUT);
      WiFiManager wifiManager;
      wifiManager.autoConnect("SB_FGU AP1");
      while (WiFi.status() != WL_CONNECTED) 
      {
           delay(250);
           Serial.print(".");
      }
       digitalWrite(LED_BUILTIN, HIGH);
       delay(1000);
       Serial.println("WiFi connected");
       Serial.println("IP address: ");
       Serial.println(WiFi.localIP());
      
       configTime(timezone * 3600, dst, ntp_server1, ntp_server3, ntp_server4);
       Serial.println("Waiting for time");
       while (!time(nullptr)) {
          Serial.print(".");
          delay(500);
        }    
} 
 
void loop() 
{ 
    Serial.println(Time_Stamps());    
    delay(1000);
} 

String Time_Stamps() {
    time_t now = time(nullptr);
    struct tm* newtime = localtime(&now);
    
    String tmpNow = "";
    tmpNow += String(DAY[newtime->tm_wday]) + " " +  String(newtime->tm_mday) + " " + String(MONTH[newtime->tm_mon])+ " " + String(newtime->tm_year + 1900);
    tmpNow += "  ";
    tmpNow += String(HOUR[newtime->tm_hour]) + ":" +  String(MIN[newtime->tm_min]) + ":" + String(SEC[newtime->tm_sec]);
    return tmpNow;
}
