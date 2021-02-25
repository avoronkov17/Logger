#ifndef SHARED_CODE_H
#define SHARED_CODE_H 

#define LED 13      /* Пин светодиода */


#define ERROR_RESTART(errCode, errText)\
do { \
   Serial.println(errCode); \
   Serial.println(F(errText)); \
   emergency(errCode, 1);\
   Serial.println(F("Reboot after 2 sec...")); \
   wdt_enable(WDTO_2S); \
   delay(4000);\
}while(0)

unsigned char TestBit(const unsigned char *array,  unsigned char k );

/*мигание светодиода с целью показания ошибок */
void emergency(uint8_t errnum, int8_t blockCount );

#endif // SHARED_CODE_H 
