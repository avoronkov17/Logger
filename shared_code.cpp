#include <Arduino.h>
#include "shared_code.h"

/**
 * Проверка бита в массиве. 
 * Вход: arr - Массив байтов.
 *       k - Номер бита, который нужно проверить.
 *       Возврашает 0, если бит сброшен, и 1, если бит установлен.
 */
unsigned char TestBit(const unsigned char *arr,  unsigned char k )
{
  return ( (arr[k/8] & (1 << (k%8) )) != 0 ) ;
}

 /* Индикация ошибки. Мигает светодиодом в зависимости от номера ошибки.
  *  Вход: errnum - Номер ошибки.
  *        blockCount - Количество повторов мигания светодиода.
 */
void emergency(uint8_t errnum, int8_t blockCount )
{
    digitalWrite(LED, HIGH);
    delay(2000);
    digitalWrite(LED, LOW);
    delay(1000);
    do
    {
      for (int8_t i = 0; i < errnum; i++)  
      {
        digitalWrite(LED, HIGH);
        delay(100);
        digitalWrite(LED, LOW);
        delay(500);
      }
      delay(2000);
    }
    while(( --blockCount) > 0);
}


String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}
