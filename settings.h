#ifndef REGISTRATOR_SETTINGS_H
#define REGISTRATOR_SETTINGS_H

/**
 *  Настройки под конкретную плату
 */

#define PLAT_NUM 1  /* Номер платы */
#define SERVER_IP 192,168,3,51   /* IP Адрес назначения */
//#define SERVER_IP 172,31,54,147   /* IP Адрес назначения */

#define PORT_DST  3000              /* Порт назначения */

//#define PORT_SRC  (1000 + PLAT_NUM) /* Порт отправки. Пока не нужно*/
#define SEND_DELAY     (2 * 1000)  /* Через какое время повторять отправку (миллисек.) */


#endif // REGISTRATOR_SETTINGS_H
