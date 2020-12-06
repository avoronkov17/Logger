#ifndef PARAMETERS_DATATYPES_H
#define PARAMETERS_DATATYPES_H

#include <stdint.h>

//#define WITH_STRING
//#define WITH_TIME

#ifdef __cplusplus
extern "C" {
#endif


#pragma pack(push, 1)
/* время */

#ifdef WITH_TIME
struct st_curr_time
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};
#endif 

/*Параметры для мониторинга */

typedef struct  st_parameters
{
  #ifdef WITH_TIME
  struct st_curr_time curr_date_time;
  #endif
  uint8_t is_eq;
  uint8_t is_reg;
  
  int16_t i_a;
  int16_t i_b;
  int16_t i_c;

  int16_t v_a;
  int16_t v_b;
  int16_t v_c;
}st_parameters;
#pragma pack(pop)


#ifdef __cplusplus
}
#endif
#endif // PARAMETERS_DATATYPES_H
