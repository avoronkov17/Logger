// This demo shows how to send a notification to the Notify My Android service
//
// Warning: Due to the limitations of the Arduino, this demo uses insecure
// HTTP to interact with the nma api (not HTTPS). The API key WILL be sent
// accross the wire in plain text.
//
// 2015-04-10 <jc@wippler.nl>
//
// License: GPLv2
/* 
POST /consum HTTP/1.1
Host: 192.168.7.126:3000
Content-Type: application/json

 {"consumData": 
     { "dt":"2020-06-28T03:28:00",
       "Eq":"1",
       "Reg":"2",
       "I_A":"1200",
       "I_B":"1200",
       "I_C":"1000",
       "V_A":"1200",
       "V_B":"0",
       "V_C":"0"}}


 sscanf_P(str, (const char *)F("%*s %d-%d-%d %d:%d:%d"), &y, &m, &d, &hh, &mm, &ss)
  */

#include <EtherCard.h>
#include "parameters.h"

#define WITHOUT_ETHERNET /* Закомментируйте для реальной работы, и расскоментируйте для имитации */

#define LED 13 /* Пин светодиода. */
#define BUFF_SIZE 900 /* Размер буфера Ethernet. */
#define SERVER_ADDR "192.168.3.51" /* Адрес сервера, с которым будет взаимодействие. */

/* Если вместо Ethernet отлаживаем данные по ком порту, то разрешаем печати */
#ifdef WITHOUT_ETHERNET 
  #define DEBUG_PRINT_TO_SERIAL(text)\
    do { \
      Serial.print(Parametrist_HTTP_string()); \
    } while (0)

  #define DEBUG_PRINT_TO_SERIAL_LN(text)\
    do { \
      Serial.println(Parametrist_HTTP_string()); \
    } while (0)
    
#else 
  #define DEBUG_PRINT_TO_SERIAL_LN(text)
  #define DEBUG_PRINT_TO_SERIAL(text)
#endif

long    g_timeMS = 0;
uint8_t g_i_can_work = 0; /* Признак, что можем работать */
struct  st_parameters g_main_parameters; /* Структура с параметрами. В ней всё хранится */

static byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x30 };
byte Ethernet::buffer[BUFF_SIZE];

Stash stash;
static byte session;

//const char Data[250] ="{\"consumData\": { \"dt\":\"$0\",\"Eq\":\"$1\",\"Reg\":\"$2\",\"I_A\":\"$3\",\"I_B\":\"$4\",\"I_C\":\"$5\",\"V_A\":\"$6\",\"V_B\":\"$7\",\"V_C\":\"$8\"}}";
//const char Data[250]  ="{\"consumData\": { \"dt\":\"2020-06-24T03:32:30.134567890\",\"Eq\":\"1\",\"Reg\":\"2\",\"I_A\":\"3\",\"I_B\":\"4\",\"I_C\":\"5\",\"V_A\":\"6\",\"V_B\":\"7\",\"V_C\":\"8\"}}\0";

void notifyMyAndroid () {
  byte sd = stash.create();
  
  Parametrist_update(&g_main_parameters);
  
  DEBUG_PRINT_TO_SERIAL_LN(F("For send:"));
  DEBUG_PRINT_TO_SERIAL_LN(Parametrist_HTTP_string());
  
  //stash.print(DataToSend);
  stash.print(Parametrist_HTTP_string());
  
  stash.save();
  int stash_size = stash.size();

  // Compose the http POST request, taking the headers below and appending
  // previously created stash in the sd holder.
  Stash::prepare(PSTR("POST /consum1 HTTP/1.1" "\r\n"
                      "Host: $F" "\r\n"
                      "Content-Length: $D" "\r\n"
                      "Content-Type: application/json" "\r\n"
                      "\r\n"
                      "$H"),
                 SERVER_ADDR, stash_size, sd);

  // send the packet - this also releases all stash buffers once done
  // Save the session ID so we can watch for it in the main loop.
  session = ether.tcpSend();
}

void setup (void)
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  //digitalWrite(LED, LOW);

  Parametrist_setup(&g_main_parameters);
  DEBUG_PRINT_TO_SERIAL_LN("Starting Notify My Android Example");

  g_timeMS = millis();
  
#ifndef WITHOUT_ETHERNET
  // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(BUFF_SIZE, mymac, SS) == 0)
  {
    DEBUG_PRINT_TO_SERIAL_LN(F("Failed to access Ethernet controller"));
    return;
  }
  if (!ether.dhcpSetup())
  {
    DEBUG_PRINT_TO_SERIAL_LN(F("DHCP failed"));
    return;
  }

  ether.printIp("IP: ", ether.myip);
  ether.printIp("GW: ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  if (!ether.dnsLookup(SERVER_ADDR))
  {
    DEBUG_PRINT_TO_SERIAL_LN(F("DNS lookup failed for the apihost"));
    return;
  }
  ether.printIp("SRV: ", ether.hisip);
  notifyMyAndroid();
#endif //  WITHOUT_ETHERNET

  g_i_can_work = 1; // сигналим, что можем работать
}

void loop (void)
{
  repeat:;
  if (g_i_can_work == 0) /* пока Ethernet не будет инициализирован правильно, работа не осуществляется */
  {
    delay(100);
    goto repeat; /* там можно сделать реинициализацию девайсов */
  }

#ifdef WITHOUT_ETHERNET
    if (millis() > (g_timeMS + 1000) )
    {
      Parametrist_update(&g_main_parameters);
      //Serial.print(F("\nRequest:"));
      DEBUG_PRINT_TO_SERIAL_LN(Parametrist_HTTP_string());
      digitalWrite(LED, !digitalRead(LED));
      g_timeMS = millis();
    }
#else
    ether.packetLoop(ether.packetReceive());
    const char* reply = ether.tcpReply(session);
    if (reply != NULL)
    {
      //Serial.println(F("Response:"));
      //Serial.println(reply);
      DEBUG_PRINT_TO_SERIAL_LN(reply);
    }
#endif // WITHOUT_ETHERNET
}
