
#include "arkanoPi.h"

int flags = 0;

TipoSistema sistema;

// Declaracion del objeto teclado
TipoTeclado teclado = {
	.columnas = {
			GPIO_KEYBOARD_COL_1,
			GPIO_KEYBOARD_COL_2,
			GPIO_KEYBOARD_COL_3,
			GPIO_KEYBOARD_COL_4
		// A completar por el alumno...
		// Hecho
	},
	.filas = {
			GPIO_KEYBOARD_ROW_1,
			GPIO_KEYBOARD_ROW_2,
			GPIO_KEYBOARD_ROW_3,
			GPIO_KEYBOARD_ROW_4
		// A completar por el alumno...
		// Hecho
	},
	.rutinas_ISR = {
		teclado_fila_1_isr,
		teclado_fila_2_isr,
		teclado_fila_3_isr,
		teclado_fila_4_isr
		// A completar por el alumno...
		// Hecho
	},
	.debounceTime = {0, 0, 0, 0},
	.columna_actual = COLUMNA_1,
	.teclaPulsada = {-1, -1},
	.flags = 0

	// A completar por el alumno...
	// Hecho
};

// Declaracion del objeto display
TipoLedDisplay led_display = {
	.pines_control_columnas = {
			GPIO_LED_DISPLAY_COL_1,
			GPIO_LED_DISPLAY_COL_2,
			GPIO_LED_DISPLAY_COL_3
		// A completar por el alumno...
		// Hecho
	},
	.filas = {
			GPIO_LED_DISPLAY_ROW_1,
			GPIO_LED_DISPLAY_ROW_2,
			GPIO_LED_DISPLAY_ROW_3,
			GPIO_LED_DISPLAY_ROW_4,
			GPIO_LED_DISPLAY_ROW_5,
			GPIO_LED_DISPLAY_ROW_6,
			GPIO_LED_DISPLAY_ROW_7
		// A completar por el alumno...
		// Hecho
	},
	// A completar por el alumno...
	// ...
};

//------------------------------------------------------
// FUNCIONES DE CONFIGURACION/INICIALIZACION
//------------------------------------------------------

// int ConfiguracionSistema (TipoSistema *p_sistema): procedimiento de configuracion
// e inicializacion del sistema.
// Realizará, entra otras, todas las operaciones necesarias para:
// configurar el uso de posibles librerías (e.g. Wiring Pi),
// configurar las interrupciones externas asociadas a los pines GPIO,
// configurar las interrupciones periódicas y sus correspondientes temporizadores,
// la inicializacion de los diferentes elementos de los que consta nuestro sistema,
// crear, si fuese necesario, los threads adicionales que pueda requerir el sistema
// como el thread de exploración del teclado del PC
int ConfiguraInicializaSistema (TipoSistema *p_sistema) {
	int result = 0;
	// A completar por el alumno...
	// Hecho

	// Lanzamos thread para exploracion del teclado convencional del PC
	/*result = piThreadCreate (thread_explora_teclado_PC);

	if (result != 0) {
		printf ("Thread didn't start!!!\n");
		return -1;
	}*/

	// Inicializamos la librería wiringPi
	if (wiringPiSetupGpio() < 0)
		printf("Unable to setup wiringPi\n");

	InicializaTeclado(&teclado);

	return result;
}

//------------------------------------------------------
// FUNCIONES LIGADAS A THREADS ADICIONALES
//------------------------------------------------------

PI_THREAD (thread_explora_teclado_PC) {
	int teclaPulsada;

	while(1) {
		delay(10); // Wiring Pi function: pauses program execution for at least 10 ms

		piLock(STD_IO_BUFFER_KEY);

		if(kbhit()) {
			teclaPulsada = kbread();

			explora_teclado(teclaPulsada);
		}

		piUnlock(STD_IO_BUFFER_KEY);
	}
}

void explora_teclado(int teclaPulsada) {
	switch(teclaPulsada) {
		// A completar por el alumno...
		// Hecho
		case '4':
		case 'A':
		case 'a':
			// A completar por el alumno...
			// Hecho
			// Activamos los flags de movimiento y de boton pulsado
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_MOV_IZQUIERDA;
			flags |= FLAG_BOTON;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case 'C':
		case 'c':
			// A completar por el alumno...
			// Hecho
			// Activamos los flags de timer y de boton pulsado
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_TIMER_JUEGO;
			flags |= FLAG_BOTON;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '6':
		case 'D':
		case 'd':
			// A completar por el alumno...
			// Hecho
			// Activamos los flags de movimiento y de boton pulsado
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_MOV_DERECHA;
			flags |= FLAG_BOTON;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case 'S':
		case 's':
			// A completar por el alumno...
			// Hecho
			// Activamos los flags de movimiento y de boton pulsado
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_BOTON;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case 'F':
		case 'f':
			printf("\nGracias por jugar a arkanoPi.\n");
			// Destruimos los timers anteriormente creados para liberar la memoria
			tmr_destroy((tmr_t*) (sistema.arkanoPi.tmr_actualizacion_juego));
			tmr_destroy((tmr_t*) (teclado.tmr_duracion_columna));
			exit(0);
			break;
		default:
			// Activamos el flag de boton pulsado
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_BOTON;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
	}
}

// wait until next_activation (absolute time)
void delay_until (unsigned int next) {
	unsigned int now = millis();
	if (next > now) {
		delay (next - now);
	}
}

int main () {
	unsigned int next;

	// Maquina de estados: lista de transiciones
	// {EstadoOrigen, CondicionDeDisparo, EstadoFinal, AccionesSiTransicion }
	fsm_trans_t arkanoPi[] = {
		{ WAIT_START, CompruebaBotonPulsado, WAIT_PUSH, InicializaJuego },
		{ WAIT_PUSH, CompruebaTimeoutActualizacionJuego, WAIT_PUSH, ActualizarJuego },
		{ WAIT_PUSH, CompruebaMovimientoIzquierda, WAIT_PUSH, MuevePalaIzquierda },
		{ WAIT_PUSH, CompruebaMovimientoDerecha, WAIT_PUSH, MuevePalaDerecha },
		{ WAIT_PUSH, CompruebaFinalJuego, WAIT_END, FinalJuego },
		{ WAIT_END,  CompruebaBotonPulsado, WAIT_START, ReseteaJuego },
		{-1, NULL, -1, NULL }
	};

	// Inicializamos el temporizador de actualización de la pantalla LED
	sistema.arkanoPi.tmr_actualizacion_juego = tmr_new(tmr_actualizacion_juego_isr);

	// Creamos e iniciamos el temporizador relativo a la exploración del teclado
	teclado.tmr_duracion_columna = tmr_new(timer_duracion_columna_isr);
	tmr_startms((tmr_t*) (teclado.tmr_duracion_columna), TIMEOUT_COLUMNA_TECLADO);

	// Configuracion e incializacion del sistema
	// Hecho
	// Inicializamos el puntero a la pantalla
	sistema.arkanoPi.p_pantalla = &(led_display.pantalla);
	ConfiguraInicializaSistema(&sistema);

	fsm_t* arkanoPi_fsm = fsm_new(WAIT_START, arkanoPi, &sistema);
	// Creamos nuevas máquinas de estados para la exploración del teclado
	fsm_t* teclado_fsm = fsm_new(TECLADO_ESPERA_COLUMNA, fsm_trans_excitacion_columnas, &teclado);
	fsm_t* tecla_fsm = fsm_new(TECLADO_ESPERA_TECLA, fsm_trans_deteccion_pulsaciones, &teclado);

	// A completar por el alumno...
	// Hecho

	piLock(STD_IO_BUFFER_KEY);
	printf("¡Bienvenido a arkanoPi!\n");
	printf("Instrucciones de uso:\n");
	printf("\tCualquier tecla inicia el juego.\n");
	printf("\tLas teclas A o 4 y D o 6 mueven la pala hacia la izquierda y hacia la derecha respectivamente.\n");
	printf("\tLa tecla C actualiza la posición de la pelota en la pantalla.\n");
	printf("\tLa tecla F cierra el juego.\n");
	fflush(stdout);
	piUnlock(STD_IO_BUFFER_KEY);

	next = millis();
	while (1) {
		// Ejecutamos las comprobaciones de las máquinas de estado del teclado
		fsm_fire(teclado_fsm);
		fsm_fire(tecla_fsm);
		fsm_fire(arkanoPi_fsm);

		// A completar por el alumno...
		// Hecho

		next += CLK_MS;
		delay_until(next);
	}

	// Destruimos los timers anteriormente creados para liberar la memoria
	tmr_destroy((tmr_t*) (sistema.arkanoPi.tmr_actualizacion_juego));
	tmr_destroy((tmr_t*) (teclado.tmr_duracion_columna));
	fsm_destroy(arkanoPi_fsm);
}
