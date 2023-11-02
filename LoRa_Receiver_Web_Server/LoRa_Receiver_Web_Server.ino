//LoraReceiverQueDeuCerto//
#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <iostream>
#include <string>

// Import Wi-Fi library
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
// Libraries to get time from NTP Server
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <SPIFFS.h>
 
/* Definicoes para comunicação com radio LoRa */
#define SCK_LORA           5
#define MISO_LORA          19
#define MOSI_LORA          27
#define RESET_PIN_LORA     14
#define SS_PIN_LORA        18
 
#define HIGH_GAIN_LORA     20  /* dBm */
#define BAND               915E6  /* 915MHz de frequencia */
 
/* Definicoes do OLED */
#define OLED_SDA_PIN    4
#define OLED_SCL_PIN    15
#define SCREEN_WIDTH    128 
#define SCREEN_HEIGHT   64  
#define OLED_ADDR       0x3C 
#define OLED_RESET      16
 
/* Offset de linhas no display OLED */
#define OLED_LINE1     0
#define OLED_LINE2     10
#define OLED_LINE3     20
#define OLED_LINE4     30
#define OLED_LINE5     40
#define OLED_LINE6     50
 
/* Definicoes gerais */
#define DEBUG_SERIAL_BAUDRATE    115200

// Replace with your network credentials
const char* ssid     = "raquel";
const char* password = "izbq4733";

// Initialize variables to get and save LoRa data
int rssi;
//String loRaMessage;
//String deltaTempo;
String minutos2="";
String segundos2="";
String centesimos2="";

//String readingID;

// Variables to save date and time
String formattedDate;
String day;
String hour;
String timestamp;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
/* Variaveis e objetos globais */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if (var == "MINUTOS"){
    return minutos2;
  }
  else if (var == "SEGUNDOS"){
    return segundos2;
  }
  else if (var == "CENTESIMOS"){
    return centesimos2;
  }
  else if(var == "TIMESTAMP"){
    return timestamp;
    
  }
  else if (var == "RRSI"){
    return String(rssi);
  }
  return String();
}

/* Local prototypes */
void display_init(void);
bool init_comunicacao_lora(void);
bool cavaloJaPassou = false; //Indica se o cronometro começou a contar ou não
bool pareiCronometro=false;// variável que vamos usar para parar o cronometro
long tempo;
 
/* Funcao: inicializa comunicacao com o display OLED
 * Parametros: nenhnum
 * Retorno: nenhnum
*/ 
void display_init(void)
{
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) 
    {
        Serial.println("[LoRa Receiver] Falha ao inicializar comunicacao com OLED");        
    }
    else
    {
        Serial.println("[LoRa Receiver] Comunicacao com OLED inicializada com sucesso");
     
        /* Limpa display e configura tamanho de fonte */
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
    }
}
 
/* Funcao: inicia comunicação com chip LoRa
 * Parametros: nenhum
 * Retorno: true: comunicacao ok
 *          false: falha na comunicacao
*/
bool init_comunicacao_lora(void)
{
    bool status_init = false;
    Serial.println("[LoRa Receiver] Tentando iniciar comunicacao com o radio LoRa...");
    SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA, SS_PIN_LORA);
    LoRa.setPins(SS_PIN_LORA, RESET_PIN_LORA, LORA_DEFAULT_DIO0_PIN);
     
    if (!LoRa.begin(BAND)) 
    {
        Serial.println("[LoRa Receiver] Comunicacao com o radio LoRa falhou. Nova tentativa em 1 segundo...");        
        delay(1000);
        status_init = false;
    }
    else
    {
        /* Configura o ganho do receptor LoRa para 20dBm, o maior ganho possível (visando maior alcance possível) */ 
        LoRa.setTxPower(HIGH_GAIN_LORA); 
        Serial.println("[LoRa Receiver] Comunicacao com o radio LoRa ok");
        status_init = true;
    }
 
    return status_init;
}
void connectWiFi(){
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to get date and time from NTPClient
void getTimeStamp() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  // Extract date
  int splitT = formattedDate.indexOf("T");
  day = formattedDate.substring(0, splitT);
  Serial.println(day);
  // Extract time
  hour = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.println(hour);
  timestamp = day + " " + hour;
}
 
/* Funcao de setup */
void setup() 
{
    connectWiFi();
    
    /* Configuracao da I²C para o display OLED */
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
 
    /* Display init */
    display_init();
 
    /* Print message telling to wait */
    display.clearDisplay();    
    display.setCursor(0, OLED_LINE1);
    display.print("Aguarde...");
    display.display();
     
    Serial.begin(DEBUG_SERIAL_BAUDRATE);
    while (!Serial);
 
    /* Tenta, até obter sucesso, comunicacao com o chip LoRa */
    while(init_comunicacao_lora() == false); 
   
   if(!SPIFFS.begin()){
   Serial.println("An Error has occurred while mounting SPIFFS");
   return;
    }
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/minutos2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", minutos2.c_str());
  });
  server.on("/segundos2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", segundos2.c_str());
  });
  server.on("/centesimos2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", centesimos2.c_str());
  });
  server.on("/timestamp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", timestamp.c_str());
  });
  server.on("/rssi", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(rssi).c_str());
  });
 /* server.on("/winter", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/winter.jpg", "image/jpg");
  });*/
  // Start server
  server.begin();
  
  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(0);      
}
 
/* Programa principal */
void loop() 
{
    
    if(!cavaloJaPassou && !pareiCronometro){
      display.clearDisplay();    
      display.setCursor(0, OLED_LINE1);
      display.print("Cronometro");
      display.setCursor(0, OLED_LINE2);
      display.print("00:00:00");
      display.display();
    }
    /* Verifica se chegou alguma informação do tamanho esperado */
      int packetSize = LoRa.parsePacket();
      if (packetSize) {
        //getLoRaData();
        getTimeStamp();
        // received a packet
        // read packet
        while (LoRa.available()) {
          Serial.print((char)LoRa.read());
          tempo=millis();
          if(!cavaloJaPassou){
          cavaloJaPassou=true;
          }
          else
          {
            cavaloJaPassou=false;
            pareiCronometro=true;
          }
        }
        // print RSSI of packet
        rssi = LoRa.packetRssi();
        Serial.print("' with RSSI ");
        Serial.println(LoRa.packetRssi());

      }
      long deltaTempo=(millis()-tempo);
      long minutos=deltaTempo/60000; //Calcula quantos minutos se passaram
      long segundos=(deltaTempo/1000)%60; //Calcula quantos segundos se passaram
      long centesimos=(deltaTempo/10)%100; //Calcula quantos centesimos se passaram
      
      //deltaTempo=String(deltaTempo2); //converte long pra string e salva o tempo2 no tempo
   
      
      if(cavaloJaPassou){
        display.clearDisplay();
        display.setCursor(0, OLED_LINE1);
        display.print("Cronometro");
        display.setCursor(0, OLED_LINE2);
        if(minutos<10){
          display.print("0");
          minutos2=String( "0" + String(minutos));
        }
        else{
          minutos2=String(minutos);
          }
        display.print(minutos);
        display.print(":");
        if(segundos<10){
          display.print("0");
          segundos2=String("0"+ String(segundos));
        }
        else{
          segundos2=String(segundos);
          }
        display.print(segundos);
        display.print(":");
        display.print(centesimos);
        display.display();
        minutos2=String(minutos);
        segundos2=String(segundos);
        centesimos2=String(centesimos);  
      }
     
      
        
}
