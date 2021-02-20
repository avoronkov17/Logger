#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "parameters_datatypes.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Инициализирует работу считывания параметров и выполняет их первое сканирование.
 */
void Parametrist_setup(struct st_parameters *params);

/**
 * Обновляет показания параметров.
 */
int8_t Parametrist_update(struct st_parameters *params);

#ifdef WITH_STRING
/**
 * Возвращает указатель на строку, содержащую параметры в виде HTTP запроса.
 */
const char* Parametrist_HTTP_string(void);

#endif

int m_add_to_string(char* stringPtr, char *format, ...);

#ifdef __cplusplus
}
#endif
#endif // PARAMETERS_H
