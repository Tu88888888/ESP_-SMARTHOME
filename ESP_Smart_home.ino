
#include "UART_ARDUINO.h"
#include <String.h>
//#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
void  Control_Light();
void  Request_LocalIP();
void  Rain_Update();
WiFiServer server(80);
UART Nhan;

uint8_t Temp, Humidity, Flame;
uint16_t Gas_value;
uint8_t status_Light_Livingroom;
uint8_t phoido;
String Status_LightLivingroom;
String Status_Lightgara;
String Status_Lightkitchen;
String Status_LightBedroom;
int b = 0;
// Define information. Can be change
#define WIFI_SSID "BietThu"
#define WIFI_PASSWORD "tubo170520"
#define FIREBASE_HOST "smarthome-tienhung.firebaseio.com"
#define FIREBASE_AUTH "X2HQJtgLHlAdCnquAfnSIUxfdJE6ktqnID4wNuzU"

#define Light_Livingroom D1
#define Light_gara D2
#define Light_kitchen D3
#define Light_Bedroom D4
#define Button_Livingroom D5
#define Button_gara D6
#define Button_kitchen D7
#define Button_Bedroom D8
#define MotorB1 9
#define MotorB2 10
#define Sensor_Rain D0
void setup() {
  // put your setup code here, to run once:
  Nhan.begin(25600);
  Serial.begin(9600);
  // Connect wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting to wifi ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("connected: ");
  // Connect firebase database
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  //  Firebase.reconnectWiFi(true);

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP()); //Gets the WiFi shield's IP address and Print the IP address of serial monitor
  Serial.println("/");
  // Mode pinout
  pinMode(Button_Livingroom, INPUT);
  pinMode(Button_gara, INPUT);
  pinMode(Button_kitchen, INPUT);
  pinMode(Button_Bedroom, INPUT);
  pinMode(Light_Livingroom, OUTPUT);
  pinMode(Light_gara, OUTPUT);
  pinMode(Light_kitchen, OUTPUT);
  pinMode(Light_Bedroom, OUTPUT);
  pinMode(Sensor_Rain, INPUT);
  pinMode(MotorB1, OUTPUT);
  pinMode(MotorB2, OUTPUT);

  digitalWrite(Light_Livingroom, LOW);
  digitalWrite(Light_gara, LOW);
  digitalWrite(Light_kitchen, LOW);
  digitalWrite(Light_Bedroom, LOW);

}

void loop() {
  // while ( Nhan.check_pass("abc", 3) == true) {
  //if(Nhan.read_uint8_t()==123){
  Status_LightLivingroom = Firebase.getString("Light_Livingroom");
  Status_Lightgara = Firebase.getString("Light_gara");
  Status_Lightkitchen = Firebase.getString("Light_kitchen");
  Status_LightBedroom = Firebase.getString("Light_Bedroom");
  Serial.print(Status_LightLivingroom);
  Serial.print(Status_Lightgara);
  Serial.print(Status_Lightkitchen);
  Serial.println(Status_LightBedroom);

  Control_Light();
  Request_LocalIP();
  Rain_Update();
  /*****************************/
  Nhan.send_pass("abc", 3);
  Recive_Data(&Temp, &Humidity, &Gas_value, &Flame);
  /* Serial.print(Temp);
    Serial.print(" ");
    Serial.print(Humidity);
    Serial.print(" ");
    Serial.print(Gas_value);
    Serial.print(" ");
    Serial.println(Flame);
  */
  Firebase.setInt("Temp", Temp);
  Firebase.setInt("Humidity", Humidity);
  Firebase.setInt("Gas", Gas_value);
  Firebase.setInt("Fire", Flame);
}
void Recive_Data(uint8_t* a, uint8_t* b, uint16_t* c, uint8_t* d)
{
  //b1: tìm kích thước để kiểm tra gói tin truyền về đã đủ chưa
  uint32_t kich_co_goi_tin = sizeof(*a) + sizeof(*b) + sizeof(*c) + sizeof(*d)  ;
  if (Serial.available() >= kich_co_goi_tin) {
    //đã nhận đủ gói tin
    /////////////
    (*a) = Nhan.read_uint8_t();
    (*b) = Nhan.read_uint8_t();
    (*c) = Nhan.read_uint16_t();
    (*d) = Nhan.read_uint8_t();
  }
}
void Send_Data(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
  // lưu ý : gói tin truyền đi có kích cỡ lớn nhất là 64 byte
  Nhan.write_uint8_t(a); // cần 1 byte
  Nhan.write_uint8_t(b);
  Nhan.write_uint8_t(c);
  Nhan.write_uint8_t(d);

}
int checkRain = 1;
void Rain_Update() {
  int c = digitalRead(Sensor_Rain);
  Serial.print(phoido);
  if (b == 1) {
    if ( checkRain != c) {
      checkRain = c;
      if (c == 0) {
        digitalWrite(MotorB1, LOW);
        digitalWrite(MotorB2, HIGH);
        delay(200);
        Firebase.setString("Phoi_Do", "0");
        digitalWrite(MotorB1, LOW);
        digitalWrite(MotorB2, LOW);
        
      }
    }
  }
  if (b != phoido) {
    b = phoido;

    if (b == 1 )
    {
     
      digitalWrite(MotorB1, HIGH);
      digitalWrite(MotorB2, LOW);
      delay(200);
      Firebase.setString("Phoi_Do", "1");
      digitalWrite(MotorB1, LOW);
      digitalWrite(MotorB2, LOW);
    }
    if (b == 0)
    {
      digitalWrite(MotorB1, LOW);
      digitalWrite(MotorB2, HIGH);
      delay(200);
      Firebase.setString("Phoi_Do", "0");
      digitalWrite(MotorB1, LOW);
      digitalWrite(MotorB2, LOW);
    }
  }
  else {
    digitalWrite(MotorB1, LOW);
    digitalWrite(MotorB2, LOW);
  }
  
}
void Control_Light() {
  if (Status_LightLivingroom == "1")
  {
    digitalWrite(Light_Livingroom, HIGH);
    delay(10);
  }
  else if (Status_LightLivingroom == "0")
  {
    digitalWrite(Light_Livingroom, LOW);
    delay(10);
  }
  if (Status_Lightgara == "1")
  {
    digitalWrite(Light_gara, HIGH);
    delay(10);
  }
  else if (Status_Lightgara == "0")
  {
    digitalWrite(Light_gara, LOW);
    delay(10);
  }
  if (Status_Lightkitchen == "1")
  {
    digitalWrite(Light_kitchen, HIGH);
    delay(10);
  }
  else if (Status_Lightkitchen == "0")
  {
    digitalWrite(Light_kitchen, LOW);
    delay(10);
  }
  if (Status_LightBedroom == "1")
  {
    digitalWrite(Light_Bedroom, HIGH);
    delay(10);
  }
  else if (Status_LightBedroom == "0")
  {
    digitalWrite(Light_Bedroom, LOW);
    delay(10);
  }
  //val_Pkhach = digitalRead(touchPin_Pkhach);
  int a1 = Status_LightLivingroom.toInt();
  int c1 = digitalRead(Button_Livingroom);
  if (digitalRead(Button_Livingroom) == 1)
  {
    int b1 = 1 - a1;
    if (b1 == 0) {
      digitalWrite(Light_Livingroom, LOW);
      Firebase.setString("Light_Livingroom", "0");
    }

    else {
      digitalWrite(Light_Livingroom, HIGH);
      Firebase.setString("Light_Livingroom", "1");
    }
  }

  int a2 = Status_Lightgara.toInt();
  int c2 = digitalRead(Button_gara);
  if (digitalRead(Button_gara) == 1)
  {
    int b2 = 1 - a2;
    if (b2 == 0) {
      digitalWrite(Light_gara, LOW);
      Firebase.setString("Light_gara", "0");
    }

    else {
      digitalWrite(Light_gara, HIGH);
      Firebase.setString("Light_gara", "1");
    }
  }

  int a3 = Status_Lightkitchen.toInt();
  int c3 = digitalRead(Button_Livingroom);
  if (digitalRead(Button_kitchen) == 1)
  {
    int b3 = 1 - a3;
    if (b3 == 0) {
      digitalWrite(Light_kitchen, LOW);
      Firebase.setString("Light_kitchen", "0");
    }

    else {
      digitalWrite(Light_kitchen, HIGH);
      Firebase.setString("Light_kitchen", "1");
    }
  }

  int a4 = Status_LightBedroom.toInt();
  int c4 = digitalRead(Button_Bedroom);
  if (digitalRead(Button_Bedroom) == 1)
  {
    int b4 = 1 - a4;
    if (b4 == 0) {
      digitalWrite(Light_Bedroom, LOW);
      Firebase.setString("Light_Bedroom", "0");
    }

    else {
      digitalWrite(Light_Bedroom, HIGH);
      Firebase.setString("Light_Bedroom", "1");
    }
  }
}

void Request_LocalIP() {
  String i = WiFi.localIP().toString();
  delay(10);
  Firebase.setString("IP_Local", i);
  delay(10);
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  // Match the request
  if (request.indexOf("/OnLightPkhach") != -1)  {

    digitalWrite(Light_Livingroom, HIGH);
    Firebase.setString("Light_Livingroom", "1");


    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("Relay 2 is ON");
    client.println("</html>");
    client.stop();
    delay(1);
  }

  if (request.indexOf("/OffLightPkhach") != -1)  {

    digitalWrite(Light_Livingroom, LOW);
    Firebase.setString("Light_Livingroom", "0");


    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("Relay 2 is OFF");
    client.println("</html>");
    client.stop();
    delay(1);
  }


  if (request.indexOf("/OnLightPngu") != -1)  {

    digitalWrite(Light_Bedroom, HIGH);
    Firebase.setString("Light_Bedroom", "1");


    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("Relay 2 is ON");
    client.println("</html>");
    client.stop();
    delay(1);
  }

  if (request.indexOf("/OffLightPngu") != -1)  {

    digitalWrite(Light_Bedroom, LOW);
    Firebase.setString("Light_Bedroom", "0");


    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("Relay 2 is OFF");
    client.println("</html>");
    client.stop();
    delay(1);
  }


  if (request.indexOf("/OnLightPbep") != -1)  {

    digitalWrite(Light_kitchen, HIGH);
    Firebase.setString("Light_kitchen", "1");


    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("Relay 2 is ON");
    client.println("</html>");
    client.stop();
    delay(1);
  }

  if (request.indexOf("/OffLightPbep") != -1)  {

    digitalWrite(Light_kitchen, LOW);
    Firebase.setString("Light_kitchen", "0");

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("Relay 2 is OFF");
    client.println("</html>");
    client.stop();
    delay(1);
  }


  if (request.indexOf("/OnLightGara") != -1)  {

    digitalWrite(Light_gara, HIGH);
    Firebase.setString("Light_gara", "1");

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("Relay 2 is ON");
    client.println("</html>");
    client.stop();
    delay(1);
  }

  if (request.indexOf("/OffLightGara") != -1)  {

    digitalWrite(Light_gara, LOW);
    Firebase.setString("Light_gara", "0");

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("Relay 2 is OFF");
    client.println("</html>");
    client.stop();
    delay(1);
  }

  if (request.indexOf("/Phoido") != -1)  {

    phoido = 1;


    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("Relay 2 is ON");
    client.println("</html>");
    client.stop();
    delay(1);
  }



  if (request.indexOf("/Laydo") != -1)  {

    phoido = 0;

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("Relay 2 is OFF");
    client.println("</html>");
    client.stop();
    delay(1);
  }

}
