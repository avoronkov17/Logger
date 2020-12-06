
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

#define LED 13 /* Пин светодиода. */
#define BUFF_SIZE 700 /* Размер буфера Ethernet. */

#define SERVER_IP  "192.168.3.51"
#define SERVER_PORT  3000
const char HOST_NAME[] = {"nano1\0"}; /* Адрес сервера, с которым будет взаимодействие. */
#define DNS_ADDR "192.168.3.1"

/* коды ошибок */
#define ETHERNET_ERROR 3 // Failed to access Ethernet controller
#define DHCP_ERROR     4 // DHCP failed
#define DNS_ERROR      5 // DNS lookup failed for the apihost

uint32_t  g_timeMS = 0;
struct    st_parameters g_main_parameters; /* Структура с параметрами. В ней всё хранится */
const byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x30 };
byte Ethernet::buffer[BUFF_SIZE];

Stash stash;
static byte session;

int8_t init_ethernet();

/* местные функции */
void emergency(uint8_t errnum, uint8_t blockCount);
void my_callback (uint8_t status, uint16_t off, uint16_t len);
void notify_server ();

//const char Data[250] ="{\"consumData\": { \"dt\":\"$0\",\"Eq\":\"$1\",\"Reg\":\"$2\",\"I_A\":\"$3\",\"I_B\":\"$4\",\"I_C\":\"$5\",\"V_A\":\"$6\",\"V_B\":\"$7\",\"V_C\":\"$8\"}}";
//const char Data[250]  ="{\"consumData\": { \"dt\":\"2020-06-24T03:32:30.134567890\",\"Eq\":\"1\",\"Reg\":\"2\",\"I_A\":\"3\",\"I_B\":\"4\",\"I_C\":\"5\",\"V_A\":\"6\",\"V_B\":\"7\",\"V_C\":\"8\"}}\0";


void setup (void)
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Parametrist_setup(&g_main_parameters);
  g_timeMS = millis();
  init_ethernet();
  notify_server ();
}

void loop (void)
{
    if (millis() > (g_timeMS + 1000) )
    {
      //Parametrist_update(&g_main_parameters);
      ether.packetLoop(ether.packetReceive());
      const char* reply = ether.tcpReply(session);
      if (reply != NULL)
      {
        Serial.println(F("Responce:"));
        Serial.println(reply);
/*POST * HTTP/1.1" "\r\n" ///consum1 
                      "Host: $F" "\r\n"
                      "Content-Length: $D" "\r\n"
                      "Content-Type: application/json" "\r\n"
                      "\r\n"
                      "$H"*/
      }
      //Serial.println(Parametrist_HTTP_string());
      //notifyMyAndroid ();
     
    }
    g_timeMS = millis();
}

void notify_server ()
{
  byte sd = stash.create();
  
  Parametrist_update(&g_main_parameters);
  
  Serial.println(F("send:"));
  Serial.println(Parametrist_HTTP_string());
  
  //stash.print(DataToSend);
  stash.print(Parametrist_HTTP_string());
  
  stash.save();

  // Compose the http POST request, taking the headers below and appending
  // previously created stash in the sd holder.
  Stash::prepare(PSTR("POST /* HTTP/1.1" "\r\n" ///consum1 
                      "Host: $F" "\r\n"
                      "Content-Length: $D" "\r\n"
                      "Content-Type: application/json" "\r\n"
                      "\r\n"
                      "$H"),
                 HOST_NAME, stash.size(), sd);

  // send the packet - this also releases all stash buffers once done
  // Save the session ID so we can watch for it in the main loop.
  session = ether.tcpSend();
}


/*инициализация сетевой карточки. 
 * Возвращает 0, в случае успешной инициализации , или код ошибки.
*/
int8_t init_ethernet()
{
    // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  if (ether.begin(BUFF_SIZE, mymac, SS) == 0) 
  {
    emergency(ETHERNET_ERROR, 0);
    //Serial.println("failed ethernet start");
    return ETHERNET_ERROR;
  }
  if (!ether.dhcpSetup())
  {
    emergency(DHCP_ERROR, 0);
    //Serial.println("failed DHCP");
    return DHCP_ERROR;
  }
  
  /*if (!ether.dnsLookup(DNS_ADDR))
  {
    emergency(DNS_ERROR, DO_NOT_BLOCK_FIRMWARE);
    return DNS_ERROR;
  }*/

  ether.parseIp(ether.hisip, SERVER_IP);
  ether.printIp("My IP: ", ether.myip);
  ether.printIp("GW: ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
  ether.printIp("SRV ip: ", ether.hisip);
  ether.hisport = SERVER_PORT;
  return 0;
}

void emergency(uint8_t errnum, uint8_t blockCount )
{
  digitalWrite(LED, LOW);
  int8_t step = 1;
  if( blockCount == 0 ) /*чтобы всегда моргала ошибка*/
  {
    blockCount = 1;
    step = 0;
  }
  do
  {
    for (int8_t i=0; i<errnum; i++)  
    {
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
      delay(500);
    }
    delay(2000);
  }
  while(( blockCount - step) > 0);
}

void my_callback (uint8_t status, uint16_t off, uint16_t len)
{
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}
