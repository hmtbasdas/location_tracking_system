#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define FIREBASE_HOST "nodemcuneo6m-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "QdGyBZPZ3sZietuvQW2yOQWffevAQK7UrxiC3kXf"
#define WIFI_SSID "TurkTelekom_ZT3NT"
#define WIFI_PASSWORD "979E362712914"

FirebaseData firebaseData;

FirebaseJson json;

const int RXPin = 4, TXPin = 5;
SoftwareSerial neo6m(RXPin, TXPin);
TinyGPSPlus gps;

void setup()
{

  Serial.begin(115200);

  neo6m.begin(9600);
  
  wifiConnect();

  Serial.println("Firebase bağlanıyor.....");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Serial.println("Firebase bağlantı sağlandı.");

}

void loop() {
  
  smartdelay_gps(1000);

  if(gps.location.isValid()) 
  {
    float latitude = gps.location.lat();
    float longitude = gps.location.lng();
    
    if(Firebase.setFloat(firebaseData, "/GPS/f_latitude", latitude))
      {
        print_ok();
      }
    else
      {
        print_fail();
      }
      
    if(Firebase.setFloat(firebaseData, "/GPS/f_longitude", longitude))
      {
        print_ok();
      }
    else
      {
        print_fail();
      }
  }
  else
  {
    Serial.println("Gps verisi bulunamadı.");
  }
  
  delay(10000);
}

static void smartdelay_gps(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (neo6m.available())
      gps.encode(neo6m.read());
  } while (millis() - start < ms);
}

void wifiConnect()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Wi-Fi'ye bağlanıyor...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("IP adresine bağlandı: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void print_ok()
{
    Serial.println("------------------------------------");
    Serial.println("OK");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
}

void print_fail()
{
    Serial.println("------------------------------------");
    Serial.println("Başarısız");
    Serial.println("Sebep: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
}

void firebaseReconnect()
{
  Serial.println("Tekrar bağlanmayı deniyor....");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
