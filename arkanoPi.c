
#include "arkanoPi.h"

int flags = 0;

TipoSistema sistema;

fsm_t* arkanoPi_fsm;

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
	.p_columna = 0,
	.flags = 0
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

	// Lanzamos un thread para gestionar las conexiones TCP de los periféricos externos
	result = piThreadCreate(thread_conexion);
	if (result != 0) {
		printf ("No se pudo crear el thread thread_conexion.\n");
		return -1;
	}

	// Inicializamos la librería wiringPi
	if (wiringPiSetupGpio() < 0)
		printf("Unable to setup wiringPi\n");

	InicializaTeclado(&teclado);

	// Pintamos la pantalla inicial en la matriz de LEDs
	PintaMensajeInicialPantalla(&(led_display.pantalla), &pantalla_inicial);
	InicializaLedDisplay(&led_display);

	// Nos falta escribir las instrucciones del juego, deshabilitamos la pantalla
	pseudoWiringPiEnableDisplay(0);

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
		case '1':
			// Activamos el flag del 1
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_NUM_1;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '2':
			// Activamos el flag del 2
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_NUM_2;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '3':
			// Activamos el flag del 3
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_NUM_3;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '5':
			// Activamos el flag del 5
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_NUM_5;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '7':
			// Activamos el flag del 7
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_NUM_7;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '8':
			// Activamos el flag del 8
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_NUM_8;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '9':
			// Activamos el flag del 9
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_NUM_9;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '4':
			// Activamos el flag del 4
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_NUM_4;
			piUnlock(SYSTEM_FLAGS_KEY);
			if (arkanoPi_fsm->current_state != WAIT_MENU) {
				break;
			}
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
			// Activamos el flag del 6
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_NUM_6;
			piUnlock(SYSTEM_FLAGS_KEY);
			if (arkanoPi_fsm->current_state != WAIT_MENU) {
				break;
			}
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
		case 'B':
		case 'b':
			// A completar por el alumno...
			// Hecho
			// Activamos los flags de movimiento y de boton pulsado
			piLock(SYSTEM_FLAGS_KEY);
			flags |= FLAG_PAUSA;
			flags |= FLAG_BOTON;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case 'F':
		case 'f':
			// Se deshabilita la pantalla para mostrar el mensaje final
			pseudoWiringPiEnableDisplay(0);
			piLock(STD_IO_BUFFER_KEY);
			enviarConsola("\nGracias por jugar a arkanoPi.\n");
			piUnlock(STD_IO_BUFFER_KEY);
			// Destruimos los timers anteriormente creados para liberar la memoria
			tmr_destroy((tmr_t*) (sistema.arkanoPi.tmr_actualizacion_juego));
			tmr_destroy((tmr_t*) (teclado.tmr_duracion_columna));
			tmr_destroy((tmr_t*) (led_display.tmr_refresco_display));
			// Se cierran las conexiones
			close(servidor.socket_fd);
			close(servidor.periferico[0].conexion_fd);
			close(servidor.periferico[1].conexion_fd);
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
		// Transiciones menú
		{ WAIT_MENU, CompruebaNumeroPelotas, WAIT_PELOTAS, MostrarSubmenuPelotas},
		{ WAIT_MENU, CompruebaParedes, WAIT_PAREDES, MostrarSubmenuParedes},
		{ WAIT_MENU, CompruebaTCP, WAIT_TCP, MostrarSubmenuTCP},
		{ WAIT_MENU, CompruebaAyuda, WAIT_AYUDA, MostrarSubmenuAyuda},
		// Submenu Pelotas
		{ WAIT_PELOTAS, CompruebaNumerosPulsados, WAIT_MENU, MostrarMenu},
		// Transiciones juego
		{ WAIT_START, CompruebaBotonPulsado, WAIT_PUSH, InicializaJuego },
		{ WAIT_PUSH, CompruebaTimeoutActualizacionJuego, WAIT_PUSH, ActualizarJuego },
		{ WAIT_PUSH, CompruebaMovimientoIzquierda, WAIT_PUSH, MuevePalaIzquierda },
		{ WAIT_PUSH, CompruebaMovimientoDerecha, WAIT_PUSH, MuevePalaDerecha },
		{ WAIT_PUSH, CompruebaFinalJuego, WAIT_END, FinalJuego },
		{ WAIT_PUSH, CompruebaPausaPulsada, WAIT_PAUSE, PausarJuego },
		{ WAIT_PAUSE, CompruebaPausaPulsada, WAIT_PUSH, PausarJuego },
		{ WAIT_END,  CompruebaBotonPulsado, WAIT_START, ReseteaJuego },
		{-1, NULL, -1, NULL }
	};

	// Inicializamos el temporizador de actualización de la pantalla LED
	sistema.arkanoPi.tmr_actualizacion_juego = tmr_new(tmr_actualizacion_juego_isr);

	// Creamos e iniciamos el temporizador relativo a la exploración del teclado
	teclado.tmr_duracion_columna = tmr_new(timer_duracion_columna_isr);
	tmr_startms((tmr_t*) (teclado.tmr_duracion_columna), TIMEOUT_COLUMNA_TECLADO);

	// Creamos e iniciamos el temporizador relativo a la actualización de la matriz de LEDs
	led_display.tmr_refresco_display = tmr_new(timer_refresco_display_isr);
	tmr_startms((tmr_t*) (led_display.tmr_refresco_display), TIMEOUT_COLUMNA_DISPLAY);

	// Configuracion e incializacion del sistema
	// Hecho
	// Inicializamos el puntero a la pantalla
	sistema.arkanoPi.p_pantalla = &(led_display.pantalla);
	ConfiguraInicializaSistema(&sistema);

	arkanoPi_fsm = fsm_new(WAIT_MENU, arkanoPi, &sistema);
	// Creamos nuevas máquinas de estados para la exploración del teclado
	fsm_t* teclado_fsm = fsm_new(TECLADO_ESPERA_COLUMNA, fsm_trans_excitacion_columnas, &teclado);
	fsm_t* tecla_fsm = fsm_new(TECLADO_ESPERA_TECLA, fsm_trans_deteccion_pulsaciones, &teclado);
	// Creamos nuevas máquinas de estados para la actualización de columnas en el display
	fsm_t* display_fsm = fsm_new(DISPLAY_ESPERA_COLUMNA, fsm_trans_excitacion_display, &led_display);

	// A completar por el alumno...
	// Hecho

	piLock(STD_IO_BUFFER_KEY);
	// Mostramos el menú de selección del juego
	MostrarMenu();
	if (!(servidor.flags & FLAG_TCP_ERROR))
		printf("\nEscuchando conexiones de periféricos en el puerto %d.\n", servidor.puerto);
	else
		printf("Error en TCP: %s", servidor.mensaje_error);
	fflush(stdout);
	piUnlock(STD_IO_BUFFER_KEY);

	// Habilitamos el display para mostrar la pantalla inicial
	pseudoWiringPiEnableDisplay(1);

	next = millis();
	while (1) {
		// Ejecutamos las comprobaciones de las máquinas de estado del teclado
		fsm_fire(teclado_fsm);
		fsm_fire(tecla_fsm);
		fsm_fire(display_fsm);
		fsm_fire(arkanoPi_fsm);

		// A completar por el alumno...
		// Hecho

		next += CLK_MS;
		delay_until(next);
	}

	// Destruimos los timers anteriormente creados para liberar la memoria
	tmr_destroy((tmr_t*) (sistema.arkanoPi.tmr_actualizacion_juego));
	tmr_destroy((tmr_t*) (teclado.tmr_duracion_columna));
	tmr_destroy((tmr_t*) (led_display.tmr_refresco_display));
	fsm_destroy(arkanoPi_fsm);
	fsm_destroy(teclado_fsm);
	fsm_destroy(tecla_fsm);
	fsm_destroy(display_fsm);
}
