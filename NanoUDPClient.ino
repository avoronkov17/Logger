
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

#define PLAT_NUM 1  /* номер платы */
#define LED 13      /* Пин светодиода. */
#define BUFF_SIZE 340 /* Размер буфера Ethernet. Если меньше 334 - не принимает сервер*/

const char SERVER_IP[] PROGMEM = "192.168.3.51";
const int srcPort PROGMEM = (1000 + PLAT_NUM);
const int dstPort PROGMEM = 3000;

/* коды ошибок */
#define ETHERNET_ERROR 3 // Failed to access Ethernet controller
#define DHCP_ERROR     4 // DHCP failed
#define DNS_ERROR      5 // DNS lookup failed for the apihost

uint32_t  g_timeMS = 0;
struct    st_parameters g_main_parameters; /* Структура с параметрами. В ней всё хранится */
const byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x30 };
byte Ethernet::buffer[BUFF_SIZE];

/* местные функции */
int8_t init_ethernet();
void emergency(uint8_t errnum, uint8_t blockCount);

void setup (void)
{
  Serial.begin(9600);
  Parametrist_setup(&g_main_parameters);
  init_ethernet();
  g_timeMS = millis();
}


void loop (void)
{ 
    if (millis() > g_timeMS )
    { 
     Parametrist_update(&g_main_parameters);
     ether.sendUdp((const char*)(&g_main_parameters), 
              sizeof(g_main_parameters), srcPort, ether.hisip, dstPort );
    
    //ether.sendUdp(Parametrist_HTTP_string(), strlen(Parametrist_HTTP_string()), srcPort, ether.hisip, dstPort );
    g_timeMS = millis() + 10;
    }
}



/*инициализация сетевой карточки. 
 * Возвращает 0, в случае успешной инициализации , или код ошибки.
*/
int8_t init_ethernet()
{
  if (ether.begin(sizeof (Ethernet::buffer), mymac, SS) == 0) 
  {
    emergency(ETHERNET_ERROR, 0);
    Serial.println(F("ethernet fail"));
    return ETHERNET_ERROR;
  }
  if (!ether.dhcpSetup())
  {
    emergency(DHCP_ERROR, 0);
    Serial.println(F("DHCP fail"));
    return DHCP_ERROR;
  }

  if (!ether.dnsLookup(SERVER_IP))
    Serial.println(F("DNS fail"));
  
  //ether.parseIp(ether.hisip, SERVER_IP);
  ether.printIp("My IP: ", ether.myip);
  //ether.printIp("GW: ", ether.gwip);
  //ether.printIp("DNS: ", ether.dnsip);
  //ether.printIp("SRV ip: ", ether.hisip);
  //ether.hisport = dstPort;
 
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
