#include <Arduino.h>
#include "parameters.h"

//#include <stdarg.h>
//#include "pins_arduino.h"
//#include "RTClib.h"
//RTC_DS1307 RTC;


static int m_add_to_string(char* stringPtr, char *format, ...)
{
  va_list ptr;
  va_start(ptr, format);
  vsprintf(stringPtr, format, ptr);
  va_end(ptr);
  return (strlen(stringPtr));
}

static char m_buffer_for_string [MAX_REQUEST_STRING_LEN];

/************* *прототипы закрытых функций ***********************/
static void m_make_string(const struct st_parameters *params);
static void m_update_time(struct st_parameters *params);
/*****************************************************************/

void Parametrist_setup(struct  st_parameters *params)
{
  /*настраиваем время, например: 12 НОЯ 1955 22ч 3м 30 сек. */
  params->curr_date_time.day = 12;
  params->curr_date_time.month = 11;
  params->curr_date_time.year = 1955;
  params->curr_date_time.hour = 22;
  params->curr_date_time.minute = 3;
  params->curr_date_time.second = 30;

  params->is_eq = 2;
  params->is_reg = 5;
  Parametrist_update(params);
}

int8_t Parametrist_update(struct  st_parameters *params)
{
  m_update_time(params);
  params->i_a = analogRead(A6);
  params->i_b = analogRead(A5);
  params->i_c = analogRead(A4);
  params->v_a = analogRead(A7);
  params->v_b = 0;
  params->v_c = -1; /*как бы, не удалось получить (имитация)*/

  m_make_string(params);
  return 0; // здесь можно потом возвращать ошибку, если что-то пошло не так.
}

const char* Parametrist_HTTP_string(void)
{
  return m_buffer_for_string;
}


/**************************************************************
***************  Реализация закрытых функции  *****************
**************************************************************/

/**
   Формирует HTTP строку из параметров (params).
*/
static void m_make_string(const struct st_parameters *params)
{
  //memset(m_buffer_for_string, '\0', sizeof(m_buffer_for_string));
  
  int offset = m_add_to_string((char*)(m_buffer_for_string), "{\"consumData\": { \"dt\":\"");
  offset += m_add_to_string((char*)(m_buffer_for_string + offset),  "%u-%u-%uT%u:%u:%u,",
          params->curr_date_time.year, params->curr_date_time.month, params->curr_date_time.day,
          params->curr_date_time.hour, params->curr_date_time.minute, params->curr_date_time.second
  );
  
  offset += m_add_to_string((char*) ( m_buffer_for_string + offset ),  "\"Eq\":\"%d\",", params->is_eq );
  offset += m_add_to_string((char*) (m_buffer_for_string + offset),  "\"Reg\":\"%d\",", params->is_reg);
  offset += m_add_to_string((char*) (m_buffer_for_string + offset),  "\"I_A\":\"%d\",", params->i_a);
  offset += m_add_to_string((char*) (m_buffer_for_string + offset),  "\"I_B\":\"%d\",", params->i_b);
  offset += m_add_to_string((char*) (m_buffer_for_string + offset),  "\"I_C\":\"%d\",", params->i_c);
  offset += m_add_to_string((char*) (m_buffer_for_string + offset), "\"V_A\":\"%d\",", params->v_a);
  offset += m_add_to_string((char*) (m_buffer_for_string + offset), "\"V_B\":\"%d\",", params->v_b);
  offset += m_add_to_string((char*) (m_buffer_for_string + offset), "\"V_C\":\"%d\"}}\0", params->v_c);

}

/**
   Обновляем время (сейчас просто имитация).
*/
static void m_update_time(struct st_parameters *params)
{
  //DateTime now = RTC.now();
  ++params->curr_date_time.second;
  if (params->curr_date_time.second >= 60)
  {
    params->curr_date_time.second = 0;
    ++params->curr_date_time.minute;
  }
  if (params->curr_date_time.minute >= 60)
  {
    params->curr_date_time.minute = 0;
    ++params->curr_date_time.hour;
  }
  /*ну, и т.д. */

}
