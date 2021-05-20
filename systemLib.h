#ifndef _SYSTEMLIB_H_
#define _SYSTEMLIB_H_

#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
//s#include <wiringPi.h>
#include "pseudoWiringPi.h"

#include "kbhit.h" // para poder detectar teclas pulsadas sin bloqueo y leer las teclas pulsadas
#include "fsm.h"
#include "tmr.h"

//#define __SIN_PSEUDOWIRINGPI__
#define CLK_MS 					1

// INTERVALO DE GUARDA ANTI-REBOTES
#define	DEBOUNCE_TIME			160
#define TIMEOUT_ACTUALIZA_JUEGO 1000

#define MAX_PERIFERICOS_CONECTADOS 2

// A 'key' which we can lock and unlock - values are 0 through 3
//	This is interpreted internally as a pthread_mutex by wiringPi
//	which is hiding some of that to make life simple.

// CLAVES PARA MUTEX
#define	KEYBOARD_KEY		0
#define	SYSTEM_FLAGS_KEY	1
#define	MATRIX_KEY			2
#define	STD_IO_BUFFER_KEY	3

// Distribucion de pines GPIO empleada para el teclado y el display
#define GPIO_KEYBOARD_COL_1 	0
#define GPIO_KEYBOARD_COL_2 	1
#define GPIO_KEYBOARD_COL_3 	2
#define GPIO_KEYBOARD_COL_4 	3
#define GPIO_KEYBOARD_ROW_1 	5
#define GPIO_KEYBOARD_ROW_2 	6
#define GPIO_KEYBOARD_ROW_3 	12
#define GPIO_KEYBOARD_ROW_4 	13

#define GPIO_LED_DISPLAY_COL_1	11
#define GPIO_LED_DISPLAY_COL_2	14
#define GPIO_LED_DISPLAY_COL_3	17
#define GPIO_LED_DISPLAY_COL_4	0
#define GPIO_LED_DISPLAY_ROW_1	4
#define GPIO_LED_DISPLAY_ROW_2	7
#define GPIO_LED_DISPLAY_ROW_3	8
#define GPIO_LED_DISPLAY_ROW_4	10
#define GPIO_LED_DISPLAY_ROW_5	22
#define GPIO_LED_DISPLAY_ROW_6	23
#define GPIO_LED_DISPLAY_ROW_7	24

// FLAGS FSM CONTROL DE ARKANOPI Y GESTION JUEGO
#define FLAG_MOV_ARRIBA 	0x0001 //0000000000000001
#define FLAG_MOV_ABAJO		0x0002 //0000000000000010
#define FLAG_MOV_DERECHA 	0x0004 //0000000000000100
#define FLAG_MOV_IZQUIERDA 	0x0008 //0000000000001000
#define FLAG_TIMER_JUEGO	0x0010 //0000000000010000
#define FLAG_BOTON 			0x0020 //0000000000100000
#define FLAG_FIN_JUEGO		0x0040 //0000000001000000
#define FLAG_PAUSA			0x0080 //0000000010000000
#define FLAG_MENU_PELOTAS	0x0100 //0000000100000000
#define FLAG_MENU_PAREDES	0x0200 //0000001000000000
#define FLAG_MENU_TCP		0x0400 //0000010000000000
#define FLAG_MENU_AYUDA		0x0800 //0000100000000000
#define FLAG_MAS			0x1000 //0001000000000000
#define FLAG_MENOS			0x2000 //0010000000000000
#define FLAG_MENU   		0x4000 //0100000000000000

enum fsm_state {
	WAIT_MENU,
	WAIT_PELOTAS,
	WAIT_PAREDES,
	WAIT_TCP,
	WAIT_AYUDA,
	WAIT_START,
	WAIT_PUSH,
	WAIT_PAUSE,
	WAIT_END};

extern int flags[MAX_PERIFERICOS_CONECTADOS + 1];

#endif /* SYSTEMLIB_H_ */
