
#include "arkanoPi.h"

int flags[MAX_PERIFERICOS_CONECTADOS + 1];

TipoSistema sistema;

// Declaramos las pantallas remotas que usaremos en los arkanoPi diferentes de 0 en p_pantalla
tipo_pantalla pantallas_remotas[MAX_PERIFERICOS_CONECTADOS];

// Definimos la máquina de estados de arkanoPi como variable global para acceder a él desde explora_teclado()
fsm_t* arkanoPi_fsm[MAX_PERIFERICOS_CONECTADOS + 1];

// Creamos el timer único de actualizacion del juego
tmr_t* timer_juego;

// Declaracion del objeto teclado usado en la partida principal
TipoTeclado teclado = {
	.columnas = {
			GPIO_KEYBOARD_COL_1,
			GPIO_KEYBOARD_COL_2,
			GPIO_KEYBOARD_COL_3,
			GPIO_KEYBOARD_COL_4
	},
	.filas = {
			GPIO_KEYBOARD_ROW_1,
			GPIO_KEYBOARD_ROW_2,
			GPIO_KEYBOARD_ROW_3,
			GPIO_KEYBOARD_ROW_4
	},
	.rutinas_ISR = {
		teclado_fila_1_isr,
		teclado_fila_2_isr,
		teclado_fila_3_isr,
		teclado_fila_4_isr
	},
	.debounceTime = {0, 0, 0, 0},
	.columna_actual = COLUMNA_1,
	.teclaPulsada = {-1, -1},
	.flags = 0
};

// Declaracion del objeto display usado en la partida principal
TipoLedDisplay led_display = {
	.pines_control_columnas = {
			GPIO_LED_DISPLAY_COL_1,
			GPIO_LED_DISPLAY_COL_2,
			GPIO_LED_DISPLAY_COL_3
	},
	.filas = {
			GPIO_LED_DISPLAY_ROW_1,
			GPIO_LED_DISPLAY_ROW_2,
			GPIO_LED_DISPLAY_ROW_3,
			GPIO_LED_DISPLAY_ROW_4,
			GPIO_LED_DISPLAY_ROW_5,
			GPIO_LED_DISPLAY_ROW_6,
			GPIO_LED_DISPLAY_ROW_7
	},
	.p_columna = 0,
	.flags = 0
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

	// Lanzamos un thread para gestionar las conexiones TCP de los periféricos externos
	result = piThreadCreate(thread_conexion);
	if (result != 0) {
		printf ("No se pudo crear el thread thread_conexion.\n");
		return -1;
	}

	// Inicializamos la librería wiringPi para poder inicializar los pines del teclado y el display
	if (wiringPiSetupGpio() < 0)
		printf("Unable to setup wiringPi\n");

	InicializaTeclado(&teclado);

	// Pintamos la pantalla inicial en la matriz de LEDs de cada partida
	for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
		PintaMensajeInicialPantalla(sistema.arkanoPi[partida].p_pantalla, &pantalla_inicial);
	}
	InicializaLedDisplay(&led_display);

	// Nos falta escribir las instrucciones del juego, por lo que deshabilitamos la pantalla
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

/**
 * Función que evalúa la tecla pulsada pasada como parámetro y en función de ella y
 * del estado actual de la partida pasada como parámetro, se palican los flags que
 * correspondan en dicha partida. 
 */
void explora_teclado(int teclaPulsada, int partida) {
	switch(teclaPulsada) {
		case '1':
			// Activamos el flag del submenú de pelotas y del botón
			piLock(SYSTEM_FLAGS_KEY);
			switch (arkanoPi_fsm[partida]->current_state) {
				case WAIT_START:
				case WAIT_END:
					flags[partida] |= FLAG_BOTON;
					break;
				case WAIT_MENU:
					flags[partida] |= FLAG_MENU_PELOTAS;
					break;
			}
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '2':
			// Activamos el flag del submenú paredes y del botón
			piLock(SYSTEM_FLAGS_KEY);
			switch (arkanoPi_fsm[partida]->current_state) {
				case WAIT_START:
				case WAIT_END:
					flags[partida] |= FLAG_BOTON;
					break;
				case WAIT_MENU:
					flags[partida] |= FLAG_MENU_PAREDES;
					break;
			}
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '3':
			// Activamos el flag del submenú TCP y del botón
			piLock(SYSTEM_FLAGS_KEY);
			switch (arkanoPi_fsm[partida]->current_state) {
				case WAIT_START:
				case WAIT_END:
					flags[partida] |= FLAG_BOTON;
					break;
				case WAIT_MENU:
					flags[partida] |= FLAG_MENU_TCP;
					break;
			}
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '5':
			// Activamos el flag para ir o volver al menú y del botón
			piLock(SYSTEM_FLAGS_KEY);
			switch (arkanoPi_fsm[partida]->current_state) {
				case WAIT_END:
					flags[partida] |= FLAG_BOTON;
					break;
				case WAIT_START:
				case WAIT_PELOTAS:
				case WAIT_PAREDES:
				case WAIT_TCP:
				case WAIT_AYUDA:
					flags[partida] |= FLAG_MENU;
			}
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '7':
			// Activamos el flag de menos para los submenús y el flag de botón
			piLock(SYSTEM_FLAGS_KEY);
			switch (arkanoPi_fsm[partida]->current_state) {
				case WAIT_START:
				case WAIT_END:
					flags[partida] |= FLAG_BOTON;
					break;
				case WAIT_PELOTAS:
				case WAIT_PAREDES:
				case WAIT_TCP:
				case WAIT_AYUDA:
					flags[partida] |= FLAG_MENOS;
					break;
			}
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '9':
			// Activamos el flag de más para los submenús y el flag de botón
			piLock(SYSTEM_FLAGS_KEY);
			switch (arkanoPi_fsm[partida]->current_state) {
				case WAIT_START:
				case WAIT_END:
					flags[partida] |= FLAG_BOTON;
					break;
				case WAIT_PELOTAS:
				case WAIT_PAREDES:
				case WAIT_TCP:
				case WAIT_AYUDA:
					flags[partida] |= FLAG_MAS;
					break;
			}
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '4':
			// Activamos el flag del submenú de ayuda y el flag de botón
			switch (arkanoPi_fsm[partida]->current_state) {
				case WAIT_START:
				case WAIT_END:
					flags[partida] |= FLAG_BOTON;
					break;
				case WAIT_MENU:
					flags[partida] |= FLAG_MENU_AYUDA;
					break;
			}
		case 'A':
		case 'a':
			// Activamos los flags de inicio juego (para el menú), de movimiento y de botón pulsado
			piLock(SYSTEM_FLAGS_KEY);
			switch (arkanoPi_fsm[partida]->current_state) {
				case WAIT_START:
				case WAIT_END:
				case WAIT_MENU:
					flags[partida] |= FLAG_BOTON;
					break;
				case WAIT_PUSH:
					flags[partida] |= FLAG_MOV_IZQUIERDA;
					break;
			}
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case 'C':
		case 'c':
			// Activamos los flags de timer y de botón pulsado
			piLock(SYSTEM_FLAGS_KEY);
			switch (arkanoPi_fsm[partida]->current_state) {
				case WAIT_START:
				case WAIT_END:
					flags[partida] |= FLAG_BOTON;
					break;
				case WAIT_PUSH:
					flags[partida] |= FLAG_TIMER_JUEGO;
					break;
			}
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case '6':
			// El 6 no inicia el juego mientras estamos en el menú
			if (arkanoPi_fsm[partida]->current_state == WAIT_MENU) {
				break;
			}
		case 'D':
		case 'd':
			// Activamos los flags de movimiento y de botón pulsado
			piLock(SYSTEM_FLAGS_KEY);
			switch (arkanoPi_fsm[partida]->current_state) {
				case WAIT_START:
				case WAIT_END:
				case WAIT_MENU:
					flags[partida] |= FLAG_BOTON;
					break;
				case WAIT_PUSH:
					flags[partida] |= FLAG_MOV_DERECHA;
					break;
			}
			piUnlock(SYSTEM_FLAGS_KEY);
			break;
		case 'B':
		case 'b':
			// Activamos los flags de movimiento y de botón pulsado
			piLock(SYSTEM_FLAGS_KEY);
			switch (arkanoPi_fsm[partida]->current_state) {
				case WAIT_START:
				case WAIT_END:
					flags[partida] |= FLAG_BOTON;
					break;
				case WAIT_PUSH:
				case WAIT_PAUSE:
					flags[partida] |= FLAG_PAUSA;
					break;
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
			tmr_destroy((tmr_t*) (teclado.tmr_duracion_columna));
			tmr_destroy((tmr_t*) (led_display.tmr_refresco_display));
			// Se cierran las conexiones
			cerrarConexion();
			exit(0);
			break;
		default:
			// Activamos el flag de botón pulsado
			piLock(SYSTEM_FLAGS_KEY);
			switch (arkanoPi_fsm[partida]->current_state) {
				case WAIT_START:
				case WAIT_END:
					flags[partida] |= FLAG_BOTON;
					break;
			}
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
		{ WAIT_PELOTAS, CompruebaMenuPulsado, WAIT_MENU, ActivarMenu },
		// Submenu Paredes
		{ WAIT_PAREDES, CompruebaMenosPulsado, WAIT_PAREDES, MostrarSubmenuParedes },
		{ WAIT_PAREDES, CompruebaMasPulsado, WAIT_PAREDES, MostrarSubmenuParedes },
		{ WAIT_PAREDES, CompruebaMenuPulsado, WAIT_MENU, ActivarMenu },
		// Submenu TCP
		{ WAIT_TCP, CompruebaMenosPulsado, WAIT_TCP, MostrarSubmenuTCP },
		{ WAIT_TCP, CompruebaMasPulsado, WAIT_TCP, MostrarSubmenuTCP },
		{ WAIT_TCP, CompruebaMenuPulsado, WAIT_MENU, ActivarMenu },
		// Submenu Ayuda
		{ WAIT_AYUDA, CompruebaMenuPulsado, WAIT_MENU, ActivarMenu },
		// Transiciones juego
		{ WAIT_START, CompruebaBotonPulsado, WAIT_PUSH, InicializaJuego },
		{ WAIT_PUSH, CompruebaTimeoutActualizacionJuego, WAIT_PUSH, ActualizarJuego },
		{ WAIT_PUSH, CompruebaMovimientoIzquierda, WAIT_PUSH, MuevePalaIzquierda },
		{ WAIT_PUSH, CompruebaMovimientoDerecha, WAIT_PUSH, MuevePalaDerecha },
		{ WAIT_PUSH, CompruebaFinalJuego, WAIT_END, FinalJuego },
		{ WAIT_PUSH, CompruebaPausaPulsada, WAIT_PAUSE, PausarJuego },
		{ WAIT_PAUSE, CompruebaPausaPulsada, WAIT_PUSH, PausarJuego },
		{ WAIT_END,  CompruebaBotonPulsado, WAIT_START, ReseteaJuego },
		{ WAIT_START, CompruebaMenuPulsado, WAIT_MENU, ActivarMenu },
		{-1, NULL, -1, NULL }
	};

	// Creamos las distintas fsms que usaremos:
	// Creamos nuevas máquinas de estados para la exploración del teclado
	fsm_t* teclado_fsm = fsm_new(TECLADO_ESPERA_COLUMNA, fsm_trans_excitacion_columnas, &teclado);
	fsm_t* tecla_fsm = fsm_new(TECLADO_ESPERA_TECLA, fsm_trans_deteccion_pulsaciones, &teclado);
	// Creamos nuevas máquinas de estados para la actualización de columnas en el display
	fsm_t* display_fsm = fsm_new(DISPLAY_ESPERA_COLUMNA, fsm_trans_excitacion_display, &led_display);

	// Creamos e iniciamos el temporizador relativo a la exploración del teclado
	teclado.tmr_duracion_columna = tmr_new(timer_duracion_columna_isr);
	tmr_startms((tmr_t*) (teclado.tmr_duracion_columna), TIMEOUT_COLUMNA_TECLADO);

	// Creamos e iniciamos el temporizador relativo a la actualización de la matriz de LEDs (solo en la primera partida)
	led_display.tmr_refresco_display = tmr_new(timer_refresco_display_isr);
	tmr_startms((tmr_t*) (led_display.tmr_refresco_display), TIMEOUT_COLUMNA_DISPLAY);

	// Inicializamos el timer de actualización de las partidas
	timer_juego = tmr_new(tmr_actualizacion_juego_isr);

	// Configuracion e incializacion del sistema
	// Inicializamos los puntero a las pantallas de cada partida
	sistema.arkanoPi[0].p_pantalla = &(led_display.pantalla); // Partida del host
	for (int partida = 1; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
		sistema.arkanoPi[partida].p_pantalla = &(pantallas_remotas[partida - 1]); // Partidas remotas
	}

	for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
		flags[partida] = 0;

		// Asignamos una partida a cada arkanoPi
		sistema.arkanoPi[partida].partida = partida;

		// Inicializamos el temporizador de actualización de la pantalla del juego
		sistema.arkanoPi[partida].tmr_actualizacion_juego = timer_juego;

		// Inicializamos las máquinas de estados de arkanoPi introduciendo su propia estructura tipo_arkanopi
		arkanoPi_fsm[partida] = fsm_new(WAIT_MENU, arkanoPi, &(sistema.arkanoPi[partida]));

		// Establecemos el número de pelotas por defecto en 2
		sistema.arkanoPi[partida].numeroPelotas = 2;

		// Establecemos que no habrá paredes en un principio
		sistema.arkanoPi[partida].paredesHabilitadas = 0;
	}
	ConfiguraInicializaSistema(&sistema);
	// Mostramos el menú principal al comenzar cada partida
	for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
		piLock(STD_IO_BUFFER_KEY);

		// Ponemos que es la primera vez que se muestra un submenú
		sistema.arkanoPi[partida].primerAccesoSubmenu = 1;

		// Mostramos el menú de selección del juego
		MostrarMenu(partida);
		piUnlock(STD_IO_BUFFER_KEY);
	}

	// Mostramos si el servidor TCP se ha iniciado correctamente al principio del juego
	piLock(STD_IO_BUFFER_KEY);
	if (!(servidor.flags & FLAG_TCP_ERROR))
		printf("\nEscuchando conexiones de periféricos en el puerto %d.\n", servidor.puerto);
	else
		printf("Error en TCP: %s", servidor.mensaje_error);
	fflush(stdout);
	piUnlock(STD_IO_BUFFER_KEY);

	// Habilitamos el display para mostrar la pantalla inicial después de mostrar el menú
	pseudoWiringPiEnableDisplay(1);

	next = millis();
	while (1) {
		// Ejecutamos las comprobaciones de las máquinas de estado del teclado
		fsm_fire(teclado_fsm);
		fsm_fire(tecla_fsm);
		fsm_fire(display_fsm);
		// Comprobamos la máquina de estados de cada partida
		for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
			fsm_fire(arkanoPi_fsm[partida]);
		}

		next += CLK_MS;
		delay_until(next);
	}

	// Destruimos los timers y máquinas de estados anteriormente creados para liberar la memoria
	tmr_destroy((tmr_t*) (teclado.tmr_duracion_columna));
	tmr_destroy((tmr_t*) (led_display.tmr_refresco_display));
	fsm_destroy(teclado_fsm);
	fsm_destroy(tecla_fsm);
	fsm_destroy(display_fsm);
	for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
		tmr_destroy((tmr_t*) (sistema.arkanoPi[partida].tmr_actualizacion_juego));
		fsm_destroy(arkanoPi_fsm[partida]);
	}
}
