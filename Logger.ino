#include <UIPEthernet.h>
#include <avr/wdt.h>
#include "settings.h"

#include "parameters.h"

#define LED 13      /* Пин светодиода */
#define ETHERNET_ERROR 3  /* Не удалось инициализировать Ethernet */
#define WAIT_SEND      5000 /* Ожидание отправки дейтаграммы */


EthernetUDP udp;

uint32_t   g_timeMS = 0;
struct     st_parameters g_main_parameters; /* Структура с параметрами. В ней всё хранится */
const byte mymac[] PROGMEM = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x32 };

/**** местные функции ****/
int8_t init_ethernet(void);
void emergency(uint8_t errnum, uint8_t blockCount);
uint8_t resetFlags __attribute__ ((section(".noinit")));
/*************************/


/*Для работы с WatchDog*/
uint8_t resetFlagsInit (void) __attribute__ ((naked))
                       __attribute__ ((used))
                       __attribute__ ((section(".init0")));

uint8_t resetFlagsInit (void)
{
  __asm__ __volatile__ ("sts %0, r2\n" : "=m" (resetFlags) :);
}

void check_watch_dog(void)
{
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

void setup (void)
{
    Serial.begin(9600);
    check_watch_dog();
    Parametrist_setup(&g_main_parameters);
    Serial.println(F("Init ethernet..."));
    if ( init_ethernet() != 0 )
    {   
        Serial.println(F("Ethernet Fail. Reboot after 2 sec..."));
        wdt_enable(WDTO_2S);
        delay(4000);
    }
    Serial.println(F("Ethernet OK"));
    g_timeMS = millis();
    wdt_enable(WDTO_4S);
}

void loop (void)
{   
    wdt_reset();
    int success;
    if (millis() > g_timeMS )
    { 
     Parametrist_update(&g_main_parameters);
     
     do
        {
          //beginPacket fails if remote ethaddr is unknown. In this case an
          //arp-request is send out first and beginPacket succeeds as soon
          //the arp-response is received.
          success = udp.beginPacket(IPAddress(SERVER_IP), PORT_DST);
        }
      while (!success && ((long)(millis() - WAIT_SEND ))<0);
      if (!success )
        goto stop;

      Serial.println(F("Sending..."));
      success = udp.write((const char*)(&g_main_parameters), sizeof(g_main_parameters));
      if (!success )
        goto stop;
      success = udp.endPacket();
      if (!success)
         goto stop;

      success = 1;
      stop:
      if (!success)
        Serial.print(F("Send failed"));
      udp.stop();
      g_timeMS = millis() + SEND_DELAY;
    }
}

/*
 * Инициализация сетевой карточки. 
 * Возвращает 0, в случае успешной инициализации , или код ошибки.
*/
int8_t init_ethernet(void)
{ 
    if (Ethernet.begin( mymac) == 0) 
    {
      emergency(ETHERNET_ERROR, 0);
      return ETHERNET_ERROR;
    }
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
