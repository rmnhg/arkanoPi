
#include "arkanoPi.h"

int flags[MAX_PERIFERICOS_CONECTADOS + 1];

TipoSistema sistema;

fsm_t* arkanoPi_fsm[MAX_PERIFERICOS_CONECTADOS + 1];

// Declaracion del objeto teclado
TipoTeclado teclado[MAX_PERIFERICOS_CONECTADOS + 1];

// Declaracion del objeto display
TipoLedDisplay led_display[MAX_PERIFERICOS_CONECTADOS + 1];

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

	for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
		InicializaTeclado(&teclado[partida]);
	}

	// Pintamos la pantalla inicial en la matriz de LEDs
	for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
		PintaMensajeInicialPantalla(&(led_display[partida].pantalla), &pantalla_inicial);
	}
	InicializaLedDisplay(&led_display[0]);

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

			explora_teclado(teclaPulsada, 0);
		}

		piUnlock(STD_IO_BUFFER_KEY);
	}
}

void explora_teclado(int teclaPulsada, int partida) {
	switch(teclaPulsada) {
		// A completar por el alumno...
		// Hecho
		case '1':
			// Activamos el flag del submenu de pelotas
			piLock(SYSTEM_FLAGS_KEY);
			flags[partida] |= FLAG_MENU_PELOTAS;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '2':
			// Activamos el flag del submenu paredes
			piLock(SYSTEM_FLAGS_KEY);
			flags[partida] |= FLAG_MENU_PAREDES;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '3':
			// Activamos el flag del submenu TCP
			piLock(SYSTEM_FLAGS_KEY);
			flags[partida] |= FLAG_MENU_TCP;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '5':
			// Activamos el flag del submenu TCP
			piLock(SYSTEM_FLAGS_KEY);
			flags[partida] |= FLAG_SALIR;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '7':
			// Activamos el flag de menos para los submenús
			piLock(SYSTEM_FLAGS_KEY);
			flags[partida] |= FLAG_MENOS;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '9':
			// Activamos el flag de más para los submenús
			piLock(SYSTEM_FLAGS_KEY);
			flags[partida] |= FLAG_MAS;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '4':
			// Activamos el flag del submenu de ayuda
			piLock(SYSTEM_FLAGS_KEY);
			flags[partida] |= FLAG_MENU_AYUDA;
			piUnlock(SYSTEM_FLAGS_KEY);
			if (arkanoPi_fsm[partida]->current_state != WAIT_MENU) {
				break;
			}
		case 'A':
		case 'a':
			// A completar por el alumno...
			// Hecho
			// Activamos los flags de inicio juego (para el menú), de movimiento y de boton pulsado
			piLock(SYSTEM_FLAGS_KEY);
			flags[partida] |= FLAG_INICIO_JUEGO;
			flags[partida] |= FLAG_MOV_IZQUIERDA;
			flags[partida] |= FLAG_BOTON;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case 'C':
		case 'c':
			// A completar por el alumno...
			// Hecho
			// Activamos los flags de timer y de boton pulsado
			piLock(SYSTEM_FLAGS_KEY);
			flags[partida] |= FLAG_TIMER_JUEGO;
			if (arkanoPi_fsm[partida]->current_state != WAIT_MENU) {
				flags[partida] |= FLAG_BOTON;
			}
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '6':
			// El 6 no inicia el juego mientras estamos en el menú
			if (arkanoPi_fsm[partida]->current_state != WAIT_MENU) {
				break;
			}
		case 'D':
		case 'd':
			// A completar por el alumno...
			// Hecho
			// Activamos los flags de inicio juego (para el menú), de movimiento y de boton pulsado
			piLock(SYSTEM_FLAGS_KEY);
			flags[partida] |= FLAG_INICIO_JUEGO;
			flags[partida] |= FLAG_MOV_DERECHA;
			flags[partida] |= FLAG_BOTON;
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case 'B':
		case 'b':
			// A completar por el alumno...
			// Hecho
			// Activamos los flags de movimiento y de boton pulsado
			piLock(SYSTEM_FLAGS_KEY);
			flags[partida] |= FLAG_PAUSA;
			if (arkanoPi_fsm[partida]->current_state != WAIT_MENU) {
				flags[partida] |= FLAG_BOTON;
			}
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case 'F':
		case 'f':
			// Se deshabilita la pantalla para mostrar el mensaje final
			pseudoWiringPiEnableDisplay(0);
			piLock(STD_IO_BUFFER_KEY);
			enviarConsola(partida, "\nGracias por jugar a arkanoPi.\n");
			piUnlock(STD_IO_BUFFER_KEY);
			// Destruimos los timers anteriormente creados para liberar la memoria
			tmr_destroy((tmr_t*) (sistema.arkanoPi[partida].tmr_actualizacion_juego));
			tmr_destroy((tmr_t*) (teclado[partida].tmr_duracion_columna));
			tmr_destroy((tmr_t*) (led_display[partida].tmr_refresco_display));
			// Se cierran las conexiones
			cerrarConexion();
			exit(0);
			break;
		default:
			// Activamos el flag de boton pulsado
			piLock(SYSTEM_FLAGS_KEY);
			flags[partida] |= FLAG_BOTON;
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
		{ WAIT_MENU, CompruebaBotonPulsado, WAIT_PUSH, InicializaJuego },
		// Submenu Pelotas
		{ WAIT_PELOTAS, CompruebaMenosPulsado, WAIT_PELOTAS, MostrarSubmenuPelotas },
		{ WAIT_PELOTAS, CompruebaMasPulsado, WAIT_PELOTAS, MostrarSubmenuPelotas },
		{ WAIT_PELOTAS, CompruebaSalirMenuPulsado, WAIT_MENU, ActivarMenu },
		// Submenu Paredes
		{ WAIT_PAREDES, CompruebaMenosPulsado, WAIT_PAREDES, MostrarSubmenuParedes },
		{ WAIT_PAREDES, CompruebaMasPulsado, WAIT_PAREDES, MostrarSubmenuParedes },
		{ WAIT_PAREDES, CompruebaSalirMenuPulsado, WAIT_MENU, ActivarMenu },
		// Submenu TCP
		{ WAIT_TCP, CompruebaMenosPulsado, WAIT_TCP, MostrarSubmenuTCP },
		{ WAIT_TCP, CompruebaMasPulsado, WAIT_TCP, MostrarSubmenuTCP },
		{ WAIT_TCP, CompruebaSalirMenuPulsado, WAIT_MENU, ActivarMenu },
		// Submenu Ayuda
		{ WAIT_AYUDA, CompruebaSalirMenuPulsado, WAIT_MENU, ActivarMenu },
		// Transiciones juego
		{ WAIT_START, CompruebaBotonPulsado, WAIT_PUSH, InicializaJuego },
		{ WAIT_PUSH, CompruebaTimeoutActualizacionJuego, WAIT_PUSH, ActualizarJuego },
		{ WAIT_PUSH, CompruebaMovimientoIzquierda, WAIT_PUSH, MuevePalaIzquierda },
		{ WAIT_PUSH, CompruebaMovimientoDerecha, WAIT_PUSH, MuevePalaDerecha },
		{ WAIT_PUSH, CompruebaFinalJuego, WAIT_END, FinalJuego },
		{ WAIT_PUSH, CompruebaPausaPulsada, WAIT_PAUSE, PausarJuego },
		{ WAIT_PAUSE, CompruebaPausaPulsada, WAIT_PUSH, PausarJuego },
		{ WAIT_END,  CompruebaBotonPulsado, WAIT_START, ReseteaJuego },
		{ WAIT_START, CompruebaSalirMenuPulsado, WAIT_MENU, ActivarMenu },
		{-1, NULL, -1, NULL }
	};

	// Creamos las distintas fsms que usaremos:
	fsm_t* teclado_fsm[MAX_PERIFERICOS_CONECTADOS + 1];
	fsm_t* tecla_fsm[MAX_PERIFERICOS_CONECTADOS + 1];
	fsm_t* display_fsm[MAX_PERIFERICOS_CONECTADOS + 1];

	// Establecemos que la conexión TCP del servidor está activada
	servidor.servidorHabilitado = 1;

	for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
		flags[partida] = 0;
		teclado[partida].columnas[COLUMNA_1] = GPIO_KEYBOARD_COL_1;
		teclado[partida].columnas[COLUMNA_2] = GPIO_KEYBOARD_COL_2;
		teclado[partida].columnas[COLUMNA_3] = GPIO_KEYBOARD_COL_3;
		teclado[partida].columnas[COLUMNA_4] = GPIO_KEYBOARD_COL_4;
		teclado[partida].filas[FILA_1] = GPIO_KEYBOARD_ROW_1;
		teclado[partida].filas[FILA_2] = GPIO_KEYBOARD_ROW_2;
		teclado[partida].filas[FILA_3] = GPIO_KEYBOARD_ROW_3;
		teclado[partida].filas[FILA_4] = GPIO_KEYBOARD_ROW_4;
		teclado[partida].rutinas_ISR[FILA_1] = teclado_fila_1_isr;
		teclado[partida].rutinas_ISR[FILA_2] = teclado_fila_2_isr;
		teclado[partida].rutinas_ISR[FILA_3] = teclado_fila_3_isr;
		teclado[partida].rutinas_ISR[FILA_4] = teclado_fila_4_isr;
		teclado[partida].debounceTime[FILA_1] = 0;
		teclado[partida].debounceTime[FILA_2] = 0;
		teclado[partida].debounceTime[FILA_3] = 0;
		teclado[partida].debounceTime[FILA_4] = 0;
		teclado[partida].columna_actual = COLUMNA_1;
		//teclado[partida].teclaPulsada[0] = -1;
		//teclado[partida].teclaPulsada[1] = -1;
		teclado[partida].flags = 0;
		teclado[partida].partida = partida;


		led_display[partida].pines_control_columnas[0] = GPIO_LED_DISPLAY_COL_1;
		led_display[partida].pines_control_columnas[1] = GPIO_LED_DISPLAY_COL_2;
		led_display[partida].pines_control_columnas[2] = GPIO_LED_DISPLAY_COL_3;
		led_display[partida].filas[0] = GPIO_LED_DISPLAY_ROW_1;
		led_display[partida].filas[1] = GPIO_LED_DISPLAY_ROW_2;
		led_display[partida].filas[2] = GPIO_LED_DISPLAY_ROW_3;
		led_display[partida].filas[3] = GPIO_LED_DISPLAY_ROW_4;
		led_display[partida].filas[4] = GPIO_LED_DISPLAY_ROW_5;
		led_display[partida].filas[5] = GPIO_LED_DISPLAY_ROW_6;
		led_display[partida].filas[6] = GPIO_LED_DISPLAY_ROW_7;
		led_display[partida].p_columna = 0;
		led_display[partida].flags = 0;

		// Asignamos una partida a cada arkanoPi
		sistema.arkanoPi[partida].partida = partida;

		// Inicializamos el temporizador de actualización de la pantalla LED
		sistema.arkanoPi[partida].tmr_actualizacion_juego = tmr_new(tmr_actualizacion_juego_isr);

		// Creamos e iniciamos el temporizador relativo a la exploración del teclado
		teclado[partida].tmr_duracion_columna = tmr_new(timer_duracion_columna_isr);
		tmr_startms((tmr_t*) (teclado[partida].tmr_duracion_columna), TIMEOUT_COLUMNA_TECLADO);

		// Creamos e iniciamos el temporizador relativo a la actualización de la matriz de LEDs (solo en la primera partida)
		led_display[0].tmr_refresco_display = tmr_new(timer_refresco_display_isr);
		tmr_startms((tmr_t*) (led_display[0].tmr_refresco_display), TIMEOUT_COLUMNA_DISPLAY);

		// Configuracion e incializacion del sistema
		// Hecho
		// Inicializamos el puntero a la pantalla
		sistema.arkanoPi[partida].p_pantalla = &(led_display[partida].pantalla);

		arkanoPi_fsm[partida] = fsm_new(WAIT_MENU, arkanoPi, &(sistema.arkanoPi[partida]));
		// Creamos nuevas máquinas de estados para la exploración del teclado
		teclado_fsm[partida] = fsm_new(TECLADO_ESPERA_COLUMNA, fsm_trans_excitacion_columnas, &teclado[partida]);
		tecla_fsm[partida] = fsm_new(TECLADO_ESPERA_TECLA, fsm_trans_deteccion_pulsaciones, &teclado[partida]);
		// Creamos nuevas máquinas de estados para la actualización de columnas en el display
		display_fsm[partida] = fsm_new(DISPLAY_ESPERA_COLUMNA, fsm_trans_excitacion_display, &led_display[partida]);

		// Establecemos el número de pelotas por defecto en 2
		sistema.arkanoPi[partida].numeroPelotas = 2;

		// Establecemos que no habrá paredes en un principio
		sistema.arkanoPi[partida].paredesHabilitadas = 0;
	}
	ConfiguraInicializaSistema(&sistema);
	for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
		piLock(STD_IO_BUFFER_KEY);

		// Ponemos que es la primera vez que se muestra un submenú
		sistema.arkanoPi[partida].primerAccesoSubmenu = 1;

		// Mostramos el menú de selección del juego
		MostrarMenu(partida);
		piUnlock(STD_IO_BUFFER_KEY);
	}

	// Habilitamos el display para mostrar la pantalla inicial
	pseudoWiringPiEnableDisplay(1);

	piLock(STD_IO_BUFFER_KEY);
	if (!(servidor.flags & FLAG_TCP_ERROR))
		printf("\nEscuchando conexiones de periféricos en el puerto %d.\n", servidor.puerto);
	else
		printf("Error en TCP: %s", servidor.mensaje_error);
	fflush(stdout);
	piUnlock(STD_IO_BUFFER_KEY);

	next = millis();
	while (1) {
		for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
			// Ejecutamos las comprobaciones de las máquinas de estado del teclado
			fsm_fire(teclado_fsm[partida]);
			fsm_fire(tecla_fsm[partida]);
			fsm_fire(display_fsm[partida]);
			fsm_fire(arkanoPi_fsm[partida]);
		}

		// A completar por el alumno...
		// Hecho

		next += CLK_MS;
		delay_until(next);
	}

	// Destruimos los timers anteriormente creados para liberar la memoria
	for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
		tmr_destroy((tmr_t*) (teclado[partida].tmr_duracion_columna));
		tmr_destroy((tmr_t*) (led_display[partida].tmr_refresco_display));
		tmr_destroy((tmr_t*) (sistema.arkanoPi[partida].tmr_actualizacion_juego));
		fsm_destroy(arkanoPi_fsm[partida]);
		fsm_destroy(teclado_fsm[partida]);
		fsm_destroy(tecla_fsm[partida]);
		fsm_destroy(display_fsm[partida]);
	}
}
