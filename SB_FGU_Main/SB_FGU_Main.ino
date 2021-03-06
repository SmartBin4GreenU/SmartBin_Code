/*
  Programer : Sarawut Waiyakorn  5635512044
  Project: SmartBin4GreenUniversity
*/

#include <Servo.h>
#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>

/**********************************/

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

/**********************************/

#include <time.h>

/**********************************/

void beep(int Delay);
String Time_Stamp();
void ConnectWiFi();
String Time_Stamps_Split_time();
String Time_Stamps_Split_date();

/*******************************************************************************************************************************************************************/

int timezone = 7;
const String DAY[7] =   {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const String MONTH[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const String SEC[60] =  {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30",
                         "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59"
                        };
const String MIN[60] =  {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30",
                         "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59"
                        };
const String HOUR[24] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
char ntp_server1[20] = "ntp.ku.ac.th";
char ntp_server2[20] = "fw.eng.ku.ac.th";
char ntp_server3[20] = "time.uni.net.th";
char ntp_server4[20] = "3.asia.pool.ntp.org";
int dst = 0;
int st = 0;
/********************************************************************************************************************************************************************/

/**comfig I2C Module**/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
/**********************************/

///*Config  connect to Firebase*/
#define FIREBASE_HOST "sb-fgu.firebaseio.com"
#define FIREBASE_AUTH "1CMvHocG4TPd2ivn1YTWKDcI7B9f3AMLNGI0vKHz"

/*Config  connect to Firebase*/
//#define FIREBASE_HOST "smartbin4greenuniversity.firebaseio.com"
//#define FIREBASE_AUTH "qeparmANzkz2h60KJ986rT1Y3D3VsvfpCCylbNdR"

Servo myservo;

/*Setup Interupt Pin*/
const byte IF_1 = D1;
const byte IF_2 = D2;
const byte SV = D3;
const byte Bt_Pin = D8;
//const byte ButtonPin = D7 ;

/*State Program*/
int STATE = 0;
int ST_Gencode = 0;
int LogIn_Status = 0;
int StandBy_State  = 0;

/*Count Bottle*/
int CheckState_Bottle = 0;
int CheckState_Open = 0;
int Count = 0;

/****Time_Checkup******/
int Mark_time = 0;
int Count_time = 0;
int Reset_Time = 0;

int State_checktime = 0;
int Count_state = 0;

int Deadline = 20;
int Deadline1 = 30;
/*********************/

//Defined Input-Output Devices//
//Input//
//int buttonState;

//Output
const byte Buzzer = D4;

/*Parameter Random Code*/
long randNumber;
long prevRand = 0;

/*Check interrupt  IF1 & IF2*/
void Count_Bot() {
  CheckState_Bottle  = 1;
  State_checktime = 1;
  if (Reset_Time == 0) {
      Mark_time = 1;
      Reset_Time = 1;
  }
}

void Check_Bot() {
  CheckState_Open  = 1;        
}

/*Check state */
void Gen_Code() {
  ST_Gencode = 1;
}

void setup() {
  Serial.begin(115200);
  
  /*Setup Servo Pin  & Position*/
  myservo.attach(SV);
  myservo.write(180);
  /*********************/

  /*Setup I2C Pin  & Cursor Position*/
  Wire.begin(D6, D5); //(SDA,SCL)
  lcd.begin();
  /*Setup infared input*/
  pinMode(IF_1, INPUT);
  pinMode(IF_2, INPUT);
  pinMode(Bt_Pin, INPUT);
  
  /*Setup Servo Output*/
  pinMode(SV, OUTPUT);
  /*Setup Buzzer Output*/
  pinMode(Buzzer, OUTPUT);
  /*Setup Random Number*/
  randomSeed(analogRead(0)); // nothing connected to 0 so read sees noise

  WiFiManager wifiManager;
  wifiManager.autoConnect("SB_FGU Main AP1");
  delay(1000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    ConnectWiFi();
  }
  Serial.println("Connect");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   connected   ");
  delay(200);
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  /*Setup Time*/
  configTime(timezone * 3600, dst, ntp_server1, ntp_server3, ntp_server4);
  Serial.println("Waiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Time Connect");

  /*Setup Interupt Functions*/
  attachInterrupt(digitalPinToInterrupt(IF_1),  Check_Bot, CHANGE);
  attachInterrupt(digitalPinToInterrupt(IF_2),  Count_Bot, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Bt_Pin), Gen_Code , HIGH);

  // Firebase Connect database
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

String getLastUser(void){
      FirebaseObject Data  = Firebase.get("LogUser/Lasted");
      String  Uid = Data.getString("/Uid");
   return Uid;
}

int getStatus(void){
      FirebaseObject Data  = Firebase.get("LogUser/Lasted");
      int Status  = Data.getInt("/StatusDevice");
    return Status;
}

int getStatus_Repush(void){
      FirebaseObject Data = Firebase.get("LogUser/CodeGen/Repush_state");
      int Repush = Data.getInt("/Repush");
    return Repush;
} 
int getAuthSuccess(void){
      FirebaseObject Data = Firebase.get("/LogUser/CodeGen/AuthenCode");
      int Auth_Success =  Data.getInt("/Status");
    return Auth_Success;
}

int getStatusrange(void){
      FirebaseObject Data = Firebase.get("/LogUser/Ultrasonic");
      int Range =  Data.getInt("/Distance");
      if(Range <= 3){
         return true;
      }
      else
         return false;    
}

void loop() {
  
   String Uid = getLastUser();
   int Status = getStatus();
   int Repush =  getStatus_Repush();
   int Auth_Success = getAuthSuccess();
   int Range_Status = getStatusrange();
   if (STATE == 0) {
      Serial.print("Status :");
      Serial.print(Status);
      Serial.print("   Repush :");
      Serial.print(Repush);
      Serial.print("   Auth_Success  :");
      Serial.println(Auth_Success);
        if (Repush == 1) {
              ST_Gencode = 2 ;
        }
        if(Range_Status == true){
              Status = 2;
        }
              if (Status == 0) {   
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("   Welcome!!!   ");
                lcd.setCursor(0, 1);
                lcd.print(" Smart Bin FGU ");
                delay(2000);
                for (int i = 0; i < 3; i++) {
                      lcd.clear();
                      lcd.noDisplay();
                      delay(500);
                      lcd.setCursor(0, 0);
                      lcd.print("  Please LogIn! ");
                      lcd.setCursor(0, 1);
                      lcd.print("  Scan QR code  ");
                      lcd.display();
                      delay(500);
                }//end for
                lcd.clear();
                delay(1000);       
              }//end if
              else if (Status == 1) {
//                int ST_Gencode = digitalRead(Bt_Pin);
                switch (ST_Gencode) {
                  case 1: {
                            do {
                              // generate random number between 100000 & 999999 (minimum is inclusive, maximum is exclusive)
                              randNumber = random(100000, 999999); 
                              Serial.println(randNumber); // show the value in Serial Monitor
                              Firebase.setString("/LogUser/CodeGen/Uid", Uid);
                              Firebase.setInt("/LogUser/CodeGen/Code", randNumber);
                              if (Firebase.failed()) {
                                  Serial.print("Set Code failed");
                                  Serial.println(Firebase.error());
                                  prevRand = randNumber;
                                  return;
                              }
                              else {
                                  lcd.clear();
                                  lcd.setCursor(0, 0);
                                  lcd.print("Gencode  Success");
                                  lcd.setCursor(0, 1);
                                  lcd.print("Code: ");
                                  lcd.setCursor(6, 1);
                                  lcd.print(randNumber);
                                  beep(200);
                                  delay(3000);
                                  }
                              delay(1000);
                          } while (randNumber == prevRand);
                          prevRand = randNumber;
                          ST_Gencode = 0;
                          STATE = 1;
                          delay(1000);
                        } break;
                  case 2: {
                              lcd.clear();
                              lcd.noDisplay();
                              delay(500);
                              lcd.setCursor(0, 0);
                              lcd.print(">>>PUT BOTTLE<<<");
                              lcd.setCursor(0, 1);
                              lcd.print(">>>>>AGIAN!<<<<<");
                              lcd.display();
                              delay(500);
                              beep(500);
                              ST_Gencode = 0;
                              STATE = 1;
                              delay(1000);
                        } break;
                  default: {
                              lcd.noDisplay();
                              delay(500);
                              lcd.setCursor(0, 0);
                              lcd.print("  Press Button  ");
                              lcd.setCursor(0, 1);
                              lcd.print("   To  Gencode  ");
                              lcd.display();
                              delay(500);
                              beep(200);
                              beep(200);
                              lcd.clear();
                        } break;
                }
           }
           else if(Status == 2){
                 STATE = 2;
                 CheckState_Open   = 0;
                 ST_Gencode = 0;
                 CheckState_Bottle  = 0;
                 State_checktime = 0; 
              beep(100);
              beep(100);
              beep(700);         
           }
  }//End  STATE = 0
  if (STATE == 1) {  
    if (Auth_Success == 1) {
            if (StandBy_State == 0) {
              Ready_ToPush();
            }
            else{
                 //Check state to Open
                 if(CheckState_Open) {
                 beep(500);
                 myservo.write(90);
                 delay(500);
                 myservo.write(180);
                 delay(200);
                 CheckState_Open  = 0;
              }
              switch (CheckState_Bottle) {
                case 1: {
                    Count++;
                    beep(200);
                    beep(200);
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Bottle Insert   ");
                    lcd.setCursor(0, 1);
                    lcd.print(">>>  ");
                    lcd.setCursor(5, 1);
                    lcd.print(Count);
                    lcd.setCursor(10, 1);
                    lcd.print("Bottle");
                    CheckState_Bottle = 0;
                    delay(500);
                  } break;
                default:
                  break;
              }
              
              /******************************** Check Time Out  Without push bottles  20 seconds  ************************************/
              if (State_checktime == 0) {
                Count_time++;
                Serial.print("Count :  ");
                Serial.println(Count_time);
                lcd.setCursor(14, 0);
                lcd.print(Count_time);
                      if (Count_time  >= Deadline ) {
            //            Insert_Data();
                          Firebase.setInt("/LogUser/CodeGen/AuthenCode/Status" , 0);
                          Firebase.setInt("/LogUser/CodeGen/Repush_state/Repush" , 0);
                          Firebase.setInt("LogUser/Lasted/StatusDevice" , 0);
                          Count = 0;
                          beep(500);
                          Auto_ToPush();
                          lcd.clear();
                          delay(1000);
                          for (int i = 0; i < 3 ; i++) {
                                lcd.noDisplay();
                                delay(500);
                                lcd.setCursor(0, 0);
                                lcd.print("    SUCCESS!    ");
                                lcd.setCursor(0, 1);
                                lcd.print(" See You Agian! ");
                                lcd.display();
                                delay(500);
                          }
                          beep(100);
                          beep(100);
                          beep(100);
                          lcd.clear();
                          STATE = 0;
                          StandBy_State = 0;
                          Count_time = 0;
                          delay(2000);
                      }
                lcd.setCursor(14, 0);
                lcd.print(Count_time);
                delay(500);
              }
              /**********************************  End  Check Time Out  Without push bottles  20 seconds  ************************************/
              /******************************************  Check Time Out  push bottles 30 seconds  ******************************************/
              if (State_checktime == 1) {
                       Count_time++;
                       if (Mark_time == 1) {
                               Reset_Time = 0;
                               Mark_time = 0;
                               Count_time = 1;
                       }
                       Serial.print("Count :  ");
                       Serial.println(Count_time);
                       lcd.setCursor(14, 0);
                       lcd.print(Count_time);
                      if (Count_time  >= Deadline1 ) {
                                Insert_Data();
                                beep(500);
                                Auto_ToPush();
                                lcd.clear();
                                delay(1000);
                                for (int i = 0; i < 3 ; i++) {
                                      lcd.noDisplay();
                                      delay(500);
                                      lcd.setCursor(0, 0);
                                      lcd.print("    SUCCESS!    ");
                                      lcd.setCursor(0, 1);
                                      lcd.print(" See You Agian! ");
                                      lcd.display();
                                      delay(500);
                                }
                                beep(100);
                                beep(100);
                                beep(100);
                                lcd.clear();
                                STATE = 0;
                                Count_time = 0;
                                StandBy_State = 0;
                                State_checktime  = 0;
                                delay(2000);
                      }
                 delay(1000);
              }
              
              /****************************************** End Check Time Out  push bottles 30 seconds  ******************************************/ 
//              int ST_Gencode = digitalRead(Bt_Pin);
              if (ST_Gencode) {
                    Insert_Data();
                    beep(500);
                    for (int i = 0; i < 3 ; i++) {
                          lcd.noDisplay();
                          delay(500);
                          lcd.setCursor(0, 0);
                          lcd.print("    SUCCESS!    ");
                          lcd.setCursor(0, 1);
                          lcd.print(" See You Agian! ");
                          lcd.display();
                          delay(500);
                    }
                    beep(100);
                    beep(100);
                    beep(100);
                    lcd.clear();
                    STATE = 0;
                    StandBy_State = 0;
                    delay(1000);
             }//End if ButtonState
           delay(1000);
          }//End else
    }//End if Auth_Success
  }//End STATE = 1
  if(STATE == 2) {
              if(Status == 0){
                   STATE = 0;  
                   beep(700);
                   beep(100);
                   beep(100);
                }
               lcd.setCursor(0, 0);
               lcd.print(" Out Of Service ");  
               lcd.setCursor(0, 1);
               lcd.print(Time_Stamps_Split_date());
               lcd.setCursor(8, 1);
               lcd.print(Time_Stamps_Split_time());   
              
    }

}

void Insert_Data(void) {
  
  String Time =  Time_Stamp();
  FirebaseObject Data = Firebase.get("LogUser/Lasted");
  String UID = Data.getString("/Uid");

  FirebaseObject LogData = Firebase.get("users/" + UID);
  String Username = LogData.getString("/username");
  String Bin_ID = LogData.getString("/SBNumber");

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& valueObject = jsonBuffer.createObject();
  valueObject["UID"] = UID;
  valueObject["Username"] = Username;
  valueObject["SBNumber"] = Bin_ID;
  valueObject["Logbottle"] = Count;
  valueObject["Logtime"] = Time;
  //Push Data to Firebase Realtime Database
  Firebase.push("History", valueObject);

  if (Firebase.failed()) {
      Serial.print("Set" + UID + "FAILED");
      Serial.println(Firebase.error());
      return ;
  }
  delay(1000);
  Firebase.setInt("/LogUser/CodeGen/AuthenCode/Status" , 0);
  Firebase.setInt("/LogUser/CodeGen/Repush_state/Repush" , 0);
  Firebase.setInt("LogUser/Lasted/StatusDevice" , 0);
  Count = 0;
  
}

void Ready_ToPush() {
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Ready!!!      ");
  for (int i = 0; i < 2 ; i++) {
        lcd.noDisplay();
        delay(300);
        lcd.setCursor(0, 1);
        lcd.print("V              V");
        lcd.display();
        delay(300);
        lcd.noDisplay();
        delay(300);
        lcd.setCursor(0, 1);
        lcd.print("  V         V   ");
        lcd.display();
        delay(300);
        lcd.noDisplay();
        delay(300);
        lcd.setCursor(0, 1);
        lcd.print("    V      V    ");
        lcd.display();
        delay(300);
        lcd.noDisplay();
        delay(300);
        lcd.setCursor(0, 1);
        lcd.print("       VV       ");
        lcd.display();
        delay(300);
  }
  lcd.setCursor(0, 1);
  lcd.print(" Put The Bottle ");
  beep(500);
  StandBy_State = 1;
  
}

void Auto_ToPush() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Auto Confirm  ");
  for(int i =0;i < 16; i+2){
    lcd.noDisplay();
    delay(200);
        lcd.setCursor(i, 1);
        lcd.print(">>");  
    lcd.display();
    delay(200);
  }
  
}

void beep(int Delay) {
  
  digitalWrite(Buzzer , HIGH);
  delay(Delay);
  digitalWrite(Buzzer , LOW);
  delay(Delay);
  
}

void ConnectWiFi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   connecting   ");
  for(int i =0;i < 16; i+2){
    lcd.noDisplay();
    delay(200);
        lcd.setCursor(i, 1);
        lcd.print(">>");
    lcd.display();
    delay(200);
  }
}

String Time_Stamp() {
  
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);

  String tmpNow = "";
    tmpNow += String(DAY[newtime->tm_wday]) + " " +  String(newtime->tm_mday) + " " + String(MONTH[newtime->tm_mon]) + " " + String(newtime->tm_year + 1900);
    tmpNow += "  ";
    tmpNow += String(HOUR[newtime->tm_hour]) + ":" +  String(MIN[newtime->tm_min]) + ":" + String(SEC[newtime->tm_sec]);
  return tmpNow;
  
}

String Time_Stamps_Split_time() {
  
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);

  String tmpNow = "";
    tmpNow += String(HOUR[newtime->tm_hour]) + ":" +  String(MIN[newtime->tm_min]) + ":" + String(SEC[newtime->tm_sec]);
    tmpNow += "";
  return tmpNow;
  
}

String Time_Stamps_Split_date() {
  
  time_t now = time(nullptr);
  struct tm* newtime = localtime(&now);

  String tmpNow = "";
    tmpNow += String(newtime->tm_mday) + "/" + String(newtime->tm_mon + 1) + "/" + String((newtime->tm_year + 1900) % 2000);
    tmpNow += "";
  return tmpNow;
  
}
