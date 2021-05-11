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

// ATENCION: Valores a modificar por el alumno
// INTERVALO DE GUARDA ANTI-REBOTES
#define	DEBOUNCE_TIME			160
#define TIMEOUT_ACTUALIZA_JUEGO 1000

// A 'key' which we can lock and unlock - values are 0 through 3
//	This is interpreted internally as a pthread_mutex by wiringPi
//	which is hiding some of that to make life simple.

// CLAVES PARA MUTEX
// ATENCION: Valores a modificar por el alumno
#define	KEYBOARD_KEY		0
#define	SYSTEM_FLAGS_KEY	1
#define	MATRIX_KEY			2
#define	STD_IO_BUFFER_KEY	3

// Distribucion de pines GPIO empleada para el teclado y el display
// ATENCION: Valores a modificar por el alumno
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

// FLAGS FSM CONTROL DE SERPIENTE Y GESTION JUEGO
// ATENCION: Valores a modificar por el alumno
#define FLAG_MOV_ARRIBA 	0x00001 //00000000000000000001
#define FLAG_MOV_ABAJO		0x00002 //00000000000000000010
#define FLAG_MOV_DERECHA 	0x00004 //00000000000000000100
#define FLAG_MOV_IZQUIERDA 	0x00008 //00000000000000001000
#define FLAG_TIMER_JUEGO	0x00010 //00000000000000010000
#define FLAG_BOTON 			0x00020 //00000000000000100000
#define FLAG_FIN_JUEGO		0x00040 //00000000000001000000
#define FLAG_PAUSA			0x00080 //00000000000010000000
#define FLAG_MENU_PELOTAS	0x00100 //00000000000100000000
#define FLAG_MENU_PAREDES	0x00200 //00000000001000000000
#define FLAG_MENU_TCP		0x00400 //00000000010000000000
#define FLAG_MENU_AYUDA		0x00800 //00000000100000000000
#define FLAG_MAS			0x01000 //00000001000000000000
#define FLAG_MENOS			0x02000 //00000010000000000000
#define FLAG_NUM_1			0x04000 //00000100000000000000
#define FLAG_NUM_2			0x08000 //00001000000000000000
#define FLAG_NUM_3			0x10000 //00010000000000000000
#define FLAG_NUM_4			0x20000 //00100000000000000000
#define FLAG_INICIO_JUEGO   0x40000 //01000000000000000000

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

extern int flags;

#endif /* SYSTEMLIB_H_ */
