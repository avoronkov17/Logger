#ifndef PARAMETERS_DATATYPES_H
#define PARAMETERS_DATATYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#pragma pack(push, 1)
/* время */

/*Параметры для мониторинга */

typedef struct  st_parameters
{
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
