#include "teclado_TL04.h"

char tecladoTL04[4][4] = {
	{'1', '2', '3', 'C'},
	{'4', '5', '6', 'D'},
	{'7', '8', '9', 'E'},
	{'A', '0', 'B', 'F'}
};

// Maquina de estados: lista de transiciones
// {EstadoOrigen, CondicionDeDisparo, EstadoFinal, AccionesSiTransicion }
fsm_trans_t fsm_trans_excitacion_columnas[] = {
	{ TECLADO_ESPERA_COLUMNA, CompruebaTimeoutColumna, TECLADO_ESPERA_COLUMNA, TecladoExcitaColumna },
	{-1, NULL, -1, NULL },
};

fsm_trans_t fsm_trans_deteccion_pulsaciones[] = {
	{ TECLADO_ESPERA_TECLA, CompruebaTeclaPulsada, TECLADO_ESPERA_TECLA, ProcesaTeclaPulsada},
	{-1, NULL, -1, NULL },
};

//------------------------------------------------------
// PROCEDIMIENTOS DE INICIALIZACION DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------

void InicializaTeclado(TipoTeclado *p_teclado) {
	piLock(KEYBOARD_KEY);
	// Ponemos los pines GPIO de las filas como entradas y les asignamos sus rutinas de interrupción
	for (int i = 0; i < NUM_FILAS_TECLADO; i++) {
		pinMode(p_teclado->filas[i], INPUT);
		pullUpDnControl(p_teclado->filas[i], PUD_DOWN);
		wiringPiISR(p_teclado->filas[i], INT_EDGE_RISING, p_teclado->rutinas_ISR[i]);
	}

	// Ponemos los pines GPIO de las columnas como salidas
	for (int i = 0; i < NUM_COLUMNAS_TECLADO; i++)
		pinMode(p_teclado->columnas[i], OUTPUT);

	piUnlock(KEYBOARD_KEY);

	// Excitamos por primera vez la columna actual
	ActualizaExcitacionTecladoGPIO(p_teclado->columna_actual);
}

//------------------------------------------------------
// OTROS PROCEDIMIENTOS PROPIOS DE LA LIBRERIA
//------------------------------------------------------

void ActualizaExcitacionTecladoGPIO (int columna) {
	for (int i = 0; i < NUM_COLUMNAS_TECLADO; i++) {
		// Ponemos la columna pasado como parámetro a HIGH y el resto a LOW
		if (i == columna)
			digitalWrite(teclado.columnas[i], HIGH);
		else
			digitalWrite(teclado.columnas[i], LOW);
	}
	piUnlock(KEYBOARD_KEY);
}

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaTimeoutColumna (fsm_t* this) {
	int result = 0;
	TipoTeclado *p_teclado;
	p_teclado = (TipoTeclado*)(this->user_data);

	piLock(SYSTEM_FLAGS_KEY);
	result = (p_teclado->flags & FLAG_TIMEOUT_COLUMNA_TECLADO);
	piUnlock(SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaTeclaPulsada (fsm_t* this) {
	int result = 0;
	TipoTeclado *p_teclado;
	p_teclado = (TipoTeclado*)(this->user_data);

	piLock(SYSTEM_FLAGS_KEY);
	result = (p_teclado->flags & FLAG_TECLA_PULSADA);
	piUnlock(SYSTEM_FLAGS_KEY);

	return result;
}

//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LAS MAQUINAS DE ESTADOS
//------------------------------------------------------

void TecladoExcitaColumna (fsm_t* this) {
	TipoTeclado *p_teclado;
	p_teclado = (TipoTeclado*)(this->user_data);

	// Se elimina el flag de timout columna atendido
	piLock(SYSTEM_FLAGS_KEY);
	p_teclado->flags &= ~FLAG_TIMEOUT_COLUMNA_TECLADO;
	piUnlock(SYSTEM_FLAGS_KEY);

	// Se cambia a la columna siguiente
	if (p_teclado->columna_actual == COLUMNA_4) {
		teclado.columna_actual = COLUMNA_1;
	} else {
		teclado.columna_actual++;
	}

	// Llamada a ActualizaExcitacionTecladoGPIO con columna a activar como argumento
	ActualizaExcitacionTecladoGPIO(p_teclado->columna_actual);

	// Se reinicia la cuenta del temporizador de excitación de columnas
	tmr_startms((tmr_t*) (p_teclado->tmr_duracion_columna), TIMEOUT_COLUMNA_TECLADO);
}

void ProcesaTeclaPulsada (fsm_t* this) {
	TipoTeclado *p_teclado;
	p_teclado = (TipoTeclado*)(this->user_data);

	piLock(SYSTEM_FLAGS_KEY);
	p_teclado->flags &= ~FLAG_TECLA_PULSADA;
	piUnlock(SYSTEM_FLAGS_KEY);
	// Llamamos a explora teclado pasando como parámetro la tecla pulsada y la partida 0 porque aquí solo se controla la partida del host
	explora_teclado(tecladoTL04[p_teclado->teclaPulsada.row][p_teclado->teclaPulsada.col], 0);
	p_teclado->teclaPulsada.row = -1;
	p_teclado->teclaPulsada.col = -1;
}


//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------

void teclado_fila_1_isr (void) {
	// Pin event (key / button event) debouncing procedure
	if (millis() < teclado.debounceTime[FILA_1]) {
		teclado.debounceTime[FILA_1] = millis() + DEBOUNCE_TIME;
		return;
	}

	piLock(SYSTEM_FLAGS_KEY);
	teclado.teclaPulsada.row = FILA_1;
	teclado.teclaPulsada.col = teclado.columna_actual;
	teclado.flags |= FLAG_TECLA_PULSADA;
	piUnlock(SYSTEM_FLAGS_KEY);

	teclado.debounceTime[FILA_1] = millis() + DEBOUNCE_TIME;
}

void teclado_fila_2_isr (void) {
	// Pin event (key / button event) debouncing procedure
	if (millis() < teclado.debounceTime[FILA_2]) {
		teclado.debounceTime[FILA_2] = millis() + DEBOUNCE_TIME;
		return;
	}

	piLock(SYSTEM_FLAGS_KEY);
	teclado.teclaPulsada.row = FILA_2;
	teclado.teclaPulsada.col = teclado.columna_actual;
	teclado.flags |= FLAG_TECLA_PULSADA;
	piUnlock(SYSTEM_FLAGS_KEY);

	teclado.debounceTime[FILA_2] = millis() + DEBOUNCE_TIME;
}

void teclado_fila_3_isr (void) {
	// Pin event (key / button event) debouncing procedure
	if (millis() < teclado.debounceTime[FILA_3]) {
		teclado.debounceTime[FILA_3] = millis() + DEBOUNCE_TIME;
		return;
	}

	piLock(SYSTEM_FLAGS_KEY);
	teclado.teclaPulsada.row = FILA_3;
	teclado.teclaPulsada.col = teclado.columna_actual;
	teclado.flags |= FLAG_TECLA_PULSADA;
	piUnlock(SYSTEM_FLAGS_KEY);

	teclado.debounceTime[FILA_3] = millis() + DEBOUNCE_TIME;
}

void teclado_fila_4_isr (void) {
	// Pin event (key / button event) debouncing procedure
	if (millis() < teclado.debounceTime[FILA_4]) {
		teclado.debounceTime[FILA_4] = millis() + DEBOUNCE_TIME;
		return;
	}

	piLock(SYSTEM_FLAGS_KEY);
	teclado.teclaPulsada.row = FILA_4;
	teclado.teclaPulsada.col = teclado.columna_actual;
	teclado.flags |= FLAG_TECLA_PULSADA;
	piUnlock(SYSTEM_FLAGS_KEY);

	teclado.debounceTime[FILA_4] = millis() + DEBOUNCE_TIME;
}

void timer_duracion_columna_isr (union sigval value) {
	piLock(SYSTEM_FLAGS_KEY);
	teclado.flags |= FLAG_TIMEOUT_COLUMNA_TECLADO;
	piUnlock(SYSTEM_FLAGS_KEY);
}
