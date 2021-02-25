#ifndef REGISTRATOR_SETTINGS_H
#define REGISTRATOR_SETTINGS_H

/**
 *  Настройки под конкретную плату. 
 *  Перед прошивкой нужно выставить нужные значения.
 */

#define PLAT_NUM  1                /* Номер платы */
#define SERVER_IP 172,22,3,197     /* IP Адрес назначения */
//#define SERVER_IP 172,31,54,147  /* IP Адрес назначения */

#define PORT_DST  3000             /* Порт назначения */
#define SEND_DELAY     (10 * 1000) /* Через какое время повторять отправку (миллисек.) */
#define WAIT_RCV       (1000) /* Как часто проверять принятый пакет */
#define UDP_BIND_PORT   4000 
#endif // REGISTRATOR_SETTINGS_H
