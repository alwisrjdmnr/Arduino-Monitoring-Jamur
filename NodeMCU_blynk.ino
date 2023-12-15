#define BLYNK_TEMPLATE_ID "TMPL6-vQLGQzC"
#define BLYNK_TEMPLATE_NAME "Hydroshroom"
#define BLYNK_AUTH_TOKEN "WNjE_zeGVgWk1BM7gwDE0q-XF4C-AGbI"

// ================= CONNECT TO WIFI ================
char ssid[] = "Kuprit";
char pass[] = "Johaneko1";

bool fetch_blynk_state = true;  //true or false

#define BLYNK_PRINT Serial
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht1(D3, DHT22); //(sensor pin,sensor type)
DHT dht2(D4, DHT22); //(sensor pin,sensor type)

// define the GPIO connected with Relays and switches
#define relay2_pin 4 //D2
#define relay3_pin 13 //D7
#define relay4_pin 15 //D8

#define button3_pin 1 //TX
#define button4_pin 3 //RX

#define wifiLed   16   //D0

//Change the virtual pins according the rooms
#define button3_vpin    V2
#define button4_vpin    V3 

int relay2_state = 0;
int relay3_state = 0;
int relay4_state = 0;

int wifiFlag = 0;

char auth[] = "WNjE_zeGVgWk1BM7gwDE0q-XF4C-AGbI";

BlynkTimer timer;

//------------------------------------------------------------------------------
// This function is called every time the device is connected to the Blynk.Cloud
// Request the latest state from the server
BLYNK_CONNECTED() {
  Blynk.syncVirtual(button3_vpin);
  Blynk.syncVirtual(button4_vpin);
}

//--------------------------------------------------------------------------
// This function is called every time the Virtual Pin state change
//i.e when web push switch from Blynk App or Web Dashboard
BLYNK_WRITE(button3_vpin) {
  relay3_state = param.asInt();
  digitalWrite(relay2_pin, !relay3_state);
  digitalWrite(relay3_pin, !relay3_state);
  
}

//--------------------------------------------------------------------------
BLYNK_WRITE(button4_vpin) {
  relay4_state = param.asInt();
  digitalWrite(relay4_pin, !relay4_state);
}
//--------------------------------------------------------------------------

void checkBlynkStatus() { // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wifiFlag = 1;
    Serial.println("Blynk Not Connected");
    digitalWrite(wifiLed, HIGH);
  }
  if (isconnected == true) {
    wifiFlag = 0;
    if (!fetch_blynk_state){
    Blynk.virtualWrite(button3_vpin, relay3_state);
    Blynk.virtualWrite(button4_vpin, relay4_state);
    }
    digitalWrite(wifiLed, LOW);
    Serial.println("Blynk Connected");
  }
}

void sendSensor() {
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();

  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();


   if (isnan(h1) || isnan(t1) || isnan(h2) || isnan(t2)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  lcd.setCursor(2, 0);
  lcd.print("|SENSOR 1|SENSOR 2");
  lcd.setCursor(2, 1);
  lcd.print("|--------|--------");
  lcd.setCursor(0, 2);
  lcd.print("H:|");
  lcd.print(h1);
  lcd.print(" % ");
  lcd.print("|");
  lcd.print(h2);
  lcd.print(" %");
  lcd.setCursor(0, 3);
  lcd.print("T:|");
  lcd.print(t1);
  lcd.print(" C ");
  lcd.print("|");
  lcd.print(t2);
  lcd.print(" C");

  Blynk.virtualWrite(V0, h1);
  Blynk.virtualWrite(V1, t1);
  Blynk.virtualWrite(V4, h2);
  Blynk.virtualWrite(V5, t2);
}

void setup() {

  Serial.begin(115200);

  //--------------------------------------------------------------------
  pinMode(button3_pin, INPUT_PULLUP);
  pinMode(button4_pin, INPUT_PULLUP);
  //--------------------------------------------------------------------
  pinMode(relay2_pin, OUTPUT);
  pinMode(relay3_pin, OUTPUT);
  pinMode(relay4_pin, OUTPUT);
  //--------------------------------------------------------------------
  //During Starting all Relays should TURN OFF
  digitalWrite(relay2_pin, LOW);
  digitalWrite(relay3_pin, LOW);
  digitalWrite(relay4_pin, LOW);
  //--------------------------------------------------------------------
  
  Blynk.begin(auth, ssid, pass);
   //Blynk.begin(auth, ssid, pass);
  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  Blynk.config(auth);
  delay(1000);
  
  if (!fetch_blynk_state){
    Blynk.virtualWrite(button3_vpin, relay3_state);
    Blynk.virtualWrite(button4_vpin, relay4_state);
  }

  Wire.begin(D2, D1);
  lcd.init();
  lcd.backlight();
  dht1.begin();
  dht2.begin();
  timer.setInterval(500L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  
  listen_push_buttons();
}

void listen_push_buttons(){
    //--------------------------------------------------------------------------
    if (digitalRead(button3_pin) == LOW){
      delay(200);
      control_relay(3);
      Blynk.virtualWrite(button3_vpin, !relay2_state); //update button state
      Blynk.virtualWrite(button3_vpin, !relay3_state); //update button state
    }
    //--------------------------------------------------------------------------
    else if (digitalRead(button4_pin) == LOW){
      delay(200);
      control_relay(4);
      Blynk.virtualWrite(button4_vpin, !relay4_state); //update button state
    }
    //--------------------------------------------------------------------------
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM




//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void control_relay(int relay){
  //------------------------------------------------
  if(relay == 2 || relay == 3){
    relay2_state = !relay3_state;
    relay3_state = !relay3_state;
    digitalWrite(relay2_pin, relay2_state);
    digitalWrite(relay3_pin, relay3_state);
    delay(50);
  }
  //------------------------------------------------
  else if(relay == 4){
    relay4_state = !relay4_state;
    digitalWrite(relay4_pin, relay4_state);
    delay(50);
  }
  //------------------------------------------------
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
