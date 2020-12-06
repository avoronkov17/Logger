#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#define MAX_REQUEST_STRING_LEN 255


#pragma pack(push, 1)
/* время */
struct st_curr_time
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

/*Параметры для мониторинга */

typedef struct  st_parameters
{
  struct st_curr_time curr_date_time;
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

/**
 * Инициализирует работу считывания параметров и выполняет их первое сканирование.
 */
void Parametrist_setup(struct st_parameters *params);

/**
 * Обновляет показания параметров.
 */
int8_t Parametrist_update(struct st_parameters *params);

/**
 * Возвращает указатель на строку, содержащую параметры в виде HTTP запроса.
 */
const char* Parametrist_HTTP_string(void);

#ifdef __cplusplus
}
#endif
#endif // PARAMETERS_H
