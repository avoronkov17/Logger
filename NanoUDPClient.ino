#include <EtherCard.h>
#include "parameters.h"

#define PLAT_NUM 1  /* номер платы */
#define LED 13      /* Пин светодиода. */
#define BUFF_SIZE 340 /* Размер буфера Ethernet. Если меньше 334 - не принимает сервер*/

//const char SERVER_IP[] PROGMEM = "192.168.3.101";
//const char SERVER_IP[] PROGMEM = "172.31.54.147";
//const char SERVER_IP[] PROGMEM = "172.22.3.191";
//const char SERVER_IP[] PROGMEM = "retc.vniizht.lan";
//const char SERVER_DNS[] = "vniizht.lan";
//const char SERVER_DNS[] = "172.31.54.7";
//const char SERVER_DNS[] = "172.31.54.147";
const char SERVER_DNS[] = "0.0.0.0";

//const char SERVER_DNS[] = "192.168.3.101";

//const int srcPort  = (1000 + PLAT_NUM);
//const int dstPort  = 3000;

/* коды ошибок */
#define ETHERNET_ERROR 3 // Failed to access Ethernet controller
#define DHCP_ERROR     4 // DHCP failed
#define DNS_ERROR      5 // DNS lookup failed for the apihost

uint32_t  g_timeMS = 0;
struct    st_parameters g_main_parameters; /* Структура с параметрами. В ней всё хранится */
const byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x32 };
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
     Serial.println(F("Sending..."));

     int srcPort  = (1000 + PLAT_NUM);
     int dstPort  = 2000;
     uint8_t ipDestinationAddress[IP_LEN];

     // 172.22.3.204
     // 192.168.4.75
     // 172.31.54.147
 
     ipDestinationAddress[0] =  192;
     ipDestinationAddress[1] =  168;
     ipDestinationAddress[2] =  4;
     ipDestinationAddress[3] =  75;
  
     ether.sendUdp((const char*)(&g_main_parameters), 
              sizeof(g_main_parameters), srcPort, ipDestinationAddress, dstPort );
     g_timeMS = millis() + 1000;
    }
}



/*инициализация сетевой карточки. 
 * Возвращает 0, в случае успешной инициализации , или код ошибки.
*/
int8_t init_ethernet()
{
 
    if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0) 
    {
      emergency(ETHERNET_ERROR, 0);
      Serial.println(F("ethernet fail"));
      return ETHERNET_ERROR;
    }
    Serial.println(F("ethernet OK"));

    if (!ether.dhcpSetup())
    {
      emergency(DHCP_ERROR, 0);
      Serial.println(F("DHCP fail"));
      return DHCP_ERROR;
    }
    Serial.println(F("DHCP OK"));
    
    ether.printIp("IP:   ", ether.myip); // output IP address to Serial
    ether.printIp("GW:   ", ether.gwip); // output gateway address to Serial
    ether.printIp("Mask: ", ether.netmask); // output netmask to Serial
    ether.printIp("DHCP server: ", ether.dhcpip); // output IP address of the DHCP server
  
    if (!ether.dnsLookup(SERVER_DNS))
      Serial.println(F("DNS fail"));
    else
      Serial.println(F("DNS OK"));
  
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
