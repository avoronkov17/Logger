#include <UIPEthernet.h>
#include <avr/wdt.h>
#include "settings.h"
#include "shared_code.h"
#include "parameters.h"

#define LINK_OFF           1
#define ERR_LINK           2
#define ETHERNET_ERROR     3  /* Не удалось инициализировать Ethernet */
#define UDP_RECEIVER_ERROR 4  /* Ошибка инициализации UDP приёмника*/
#define WAIT_SEND       5000  /* Ожидание отправки дейтаграммы */
#define MSG_SIZE           1  /*Размер сообщения от сервера*/

EthernetUDP udpServ;
EthernetUDP udpClient;

int32_t    g_serverNotAnswer = 0; /*сколько сервер нам не отвечает*/
uint32_t   g_timeMS = 0;
bool       g_mustReadReply = false;
struct     st_parameters g_logger_data; /* Структура с параметрами. В ней всё хранится */
const byte mymac[] PROGMEM = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x34 };

char stringWithParams[64];

/**** местные функции ****/
int8_t init_ethernet(void);
uint8_t resetFlags __attribute__ ((section(".noinit")));
void check_watch_dog(void);
int check_udp_reply(); /* Приём квитанции от сервера  */
/*************************/


/*Для работы с WatchDog*/
uint8_t resetFlagsInit (void) __attribute__ ((naked))
                       __attribute__ ((used))
                       __attribute__ ((section(".init0")));
   

void setup (void)
{
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
    check_watch_dog();
    
    Parametrist_setup(&g_logger_data);
    Serial.println(F("Wait ether..."));
    int err = 0;
    if ( (err = init_ethernet()) != 0 )
    {   
        ERROR_RESTART(ETHERNET_ERROR, "Etherner init error");
    }
    Serial.println(F("Ethernet OK"));
    
    
  int success = udpServ.begin(UDP_BIND_PORT);
  if (success == 0)
     ERROR_RESTART(UDP_RECEIVER_ERROR, "UDP bind error");
  
  Serial.print(F("UDP Server OK"));
  g_timeMS = millis();
  wdt_enable(WDTO_4S);
}

void loop (void)
{   
    wdt_reset();
    int success;

    if (g_mustReadReply) // Ожидаем ответ на отправку
    { 
       if ( check_udp_reply() > 0 )
          g_serverNotAnswer = 0; 
    }
    if (g_serverNotAnswer >= 10 )
    { 
        Serial.println(F("Server not answer. Rebooting nano..."));
        wdt_enable(WDTO_2S);
        delay(4000);
    }
    
    if (millis() > g_timeMS )
    { 
     Parametrist_update(&g_logger_data);
     // Формат (параметры разделены запятой): номер_платы,eq,reg,токи,напряжения
     m_add_to_string(stringWithParams, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                     PLAT_NUM,
                     g_logger_data.is_eq, g_logger_data.is_reg,
                     g_logger_data.i_a, g_logger_data.i_b, 
                     g_logger_data.i_c,
                     g_logger_data.v_a, g_logger_data.v_b,
                     g_logger_data.v_c);

     do
        {
          //beginPacket fails if remote ethaddr is unknown. In this case an
          //arp-request is send out first and beginPacket succeeds as soon
          //the arp-response is received.
          success = udpClient.beginPacket(IPAddress(SERVER_IP), PORT_DST);
        }
      while (!success && ((long)(millis() - WAIT_SEND ))<0);
      if (!success )
        goto stop;

      Serial.println(F("Sending..."));
      success = udpClient.write((const char*)(&g_logger_data), sizeof(g_logger_data));
      //success = udp.write(stringWithParams, strlen(stringWithParams));
      if (!success )
        goto stop;
      success = udpClient.endPacket();
      if (!success)
         goto stop;

      success = 1;
      g_mustReadReply = true;
      g_serverNotAnswer++; /*Увеличиваем счётчик неподтверждённых пакетов */
      stop:
      if (!success)
        Serial.print(F("Send failed"));
      udpClient.stop();
      g_timeMS = millis() + SEND_DELAY;
    }
}

/*Ethernet.begin(mac,IPAddress(192,168,0,6));
 * Инициализация сетевой карточки. 
 * Возвращает 0, в случае успешной инициализации , или код ошибки.
*/
int8_t init_ethernet(void)
{    
    //Ethernet.begin( mymac, IPAddress(172,31,54,254));
    if (Ethernet.begin( mymac) == 0) 
    {
      emergency(ETHERNET_ERROR, 1);
      return ETHERNET_ERROR;
    }
   //Serial.print("IP: ");
   Serial.println(IpAddress2String(Ethernet.localIP()));
   //Serial.print("Mask: ");
   Serial.println(IpAddress2String(Ethernet.subnetMask()));
   //Serial.print("Gateway: ");
   Serial.println(IpAddress2String(Ethernet.gatewayIP()));
  return 0;
}


/* проверка причины перезапуска  */
void check_watch_dog(void)
{ 
  /*if (TestBit((const uint8_t* )(&resetFlags) , WDRF ))
  {
      Serial.println(F("WATDGDGDGDG"));
  }
  if (TestBit((const uint8_t* )(&resetFlags) , BORF ))
  {
      Serial.println(F("BOOOORF"));
  }*/
  
  if (resetFlags & (1 << WDRF))
  {
     resetFlags &= ~(1 << WDRF);
     Serial.println(F("Watchdog"));
  }
  if (resetFlags & (1 << BORF))
  {
     resetFlags &= ~(1 << BORF);
     Serial.println(F("Brownout"));
  }
  if (resetFlags & (1 << EXTRF))
  {
     resetFlags &= ~(1 << EXTRF);
     Serial.println(F("External"));
  }
  if (resetFlags & (1 << PORF))
  {
     resetFlags &= ~(1 << PORF);
     Serial.println(F("PowerOn"));
  }
  if (resetFlags != 0x00)
  {
     Serial.println(F("Unknown"));
  }
}

/* Приём квитанции от сервера  */
int check_udp_reply()
{
    int size = udpServ.parsePacket();
    if (size < 1) 
      return 0;
 
    do
      {
        char msg[MSG_SIZE + 1];
        int len = udpServ.read(msg, MSG_SIZE + 2);
        msg[len]=0;
        int8_t cmd;
        memcpy((void*)(&cmd), msg, sizeof(cmd));
        Serial.print("cmd:");
        Serial.print(cmd);
      }
    while ( (size = udpServ.available()) > 0);
    
    //Заканчиваем чтение пакета:
    udpServ.flush();

    /*Serial.print(F("remote ip:"));
    Serial.println(udpServ.remoteIP());
    Serial.print(F("remote port:"));
    Serial.println(udpServ.remotePort());*/
    udpServ.stop();
    
    Serial.print(F("restart connection: "));
    Serial.println (udpServ.begin(UDP_BIND_PORT) ? "OK" : "FAIL");
    g_mustReadReply = false;
    return 1;
  }

uint8_t resetFlagsInit (void)
{
  __asm__ __volatile__ ("sts %0, r2\n" : "=m" (resetFlags) :);
}
