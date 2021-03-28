
#include "ledDisplay.h"

tipo_pantalla pantalla_inicial = {
	.matriz = {
	{0,0,0,0,0,0,0,0},
	{0,1,1,0,0,1,1,0},
	{0,1,1,0,0,1,1,0},
	{0,0,0,0,0,0,0,0},
	{0,1,0,0,0,0,1,0},
	{0,0,1,1,1,1,0,0},
	{0,0,0,0,0,0,0,0},
	}
};

tipo_pantalla pantalla_final = {
	.matriz = {
	{0,0,0,0,0,0,0,0},
	{0,1,1,0,0,1,1,0},
	{0,1,1,0,0,1,1,0},
	{0,0,0,0,0,0,0,0},
	{0,0,1,1,1,1,0,0},
	{0,1,0,0,0,0,1,0},
	{0,0,0,0,0,0,0,0},
	}
};

// Maquina de estados: lista de transiciones
// {EstadoOrigen, CondicionDeDisparo, EstadoFinal, AccionesSiTransicion }
fsm_trans_t fsm_trans_excitacion_display[] = {
	{ DISPLAY_ESPERA_COLUMNA, CompruebaTimeoutColumnaDisplay, DISPLAY_ESPERA_COLUMNA, ActualizaExcitacionDisplay },
	{-1, NULL, -1, NULL },
};

//------------------------------------------------------
// PROCEDIMIENTOS DE INICIALIZACION DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------

void InicializaLedDisplay (TipoLedDisplay *led_display) {
	// A completar por el alumno...
	// ...

	piLock(MATRIX_KEY);
	// Ponemos los pines GPIO de las filas como entradas
	for (int i = 0; i < NUM_FILAS_DISPLAY; i++) {
		pinMode(led_display->filas[i], OUTPUT);
	}
	// Ponemos los pines GPIO del descodificador de las columnas como entradas
	for (int i = 0; i < NUM_PINES_CONTROL_COLUMNAS_DISPLAY; i++) {
		pinMode(led_display->pines_control_columnas[i], OUTPUT);
	}
	piUnlock(MATRIX_KEY);
}

//------------------------------------------------------
// OTROS PROCEDIMIENTOS PROPIOS DE LA LIBRERIA
//------------------------------------------------------

void ApagaFilas (TipoLedDisplay *led_display){
	// A completar por el alumno...
	// ...

	// Ponemos los pines GPIO de las filas en HIGH (apagamos las filas a nivel alto)
	for (int i = 0; i < NUM_FILAS_DISPLAY; i++) {
		digitalWrite(led_display->filas[i], HIGH);
	}
}

void ExcitaColumnas(int columna) {
	// Matriz que extrae la configuración de los pines del descodificador en función de la columna activa
	int columnaBinaria[NUM_COLUMNAS_DISPLAY][NUM_PINES_CONTROL_COLUMNAS_DISPLAY] = {
		{0, 0, 0},
		{1, 0, 0},
		{0, 1, 0},
		{1, 1, 0},
		{0, 0, 1},
		{1, 0, 1},
		{0, 1, 1},
		{1, 1, 1},
	};

	/*switch(columna) {
		// A completar por el alumno...
		// ...
	}*/
	for (int i = 0; i < NUM_PINES_CONTROL_COLUMNAS_DISPLAY; i++) {
		// Ponemos los pines del descodificador de la columna a HIGH o a LOW en función de la columna que seleccionamos
		if (columnaBinaria[columna][i] == 1)
			digitalWrite(led_display.pines_control_columnas[i], HIGH);
		else
			digitalWrite(led_display.pines_control_columnas[i], LOW);
	}
}

void ActualizaLedDisplay (TipoLedDisplay *led_display) {
	// A completar por el alumno...
	// ...

	piLock(MATRIX_KEY);
	ExcitaColumnas(led_display->p_columna);
	//ApagaFilas(led_display); // No usamos ApagaFilas() porque tenemos un display emulado y nos pondría todas las filas a 0 siempre
	for (int i = 0; i < NUM_FILAS_DISPLAY; i++) {
		if ((led_display->pantalla.matriz[i][led_display->p_columna] != 0)) {
			digitalWrite(led_display->filas[i], LOW);
		} else {
			digitalWrite(led_display->filas[i], HIGH);
		}
	}
	piUnlock(MATRIX_KEY);
}

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaTimeoutColumnaDisplay (fsm_t* this) {
	int result = 0;
	TipoLedDisplay *p_ledDisplay;
	p_ledDisplay = (TipoLedDisplay*)(this->user_data);

	// A completar por el alumno...
	// ...
	piLock(SYSTEM_FLAGS_KEY);
	result = p_ledDisplay->flags & FLAG_TIMEOUT_COLUMNA_DISPLAY;
	piUnlock(SYSTEM_FLAGS_KEY);

	return result;
}

//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

void ActualizaExcitacionDisplay (fsm_t* this) {
	TipoLedDisplay *p_ledDisplay;
	p_ledDisplay = (TipoLedDisplay*)(this->user_data);

	// A completar por el alumno...
	// ...

	// Desactivamos el flag que atendemos
	piLock(SYSTEM_FLAGS_KEY);
	p_ledDisplay->flags &= ~FLAG_TIMEOUT_COLUMNA_DISPLAY;
	piUnlock(SYSTEM_FLAGS_KEY);

	// Actualizamos el display en función de la matriz
	ActualizaLedDisplay(p_ledDisplay);

	// Cambiamos a la columna siguiente
	piLock(MATRIX_KEY);
	if (p_ledDisplay->p_columna < NUM_COLUMNAS_DISPLAY - 1) {
		p_ledDisplay->p_columna++;
	} else {
		p_ledDisplay->p_columna = 0;
	}
	piUnlock(MATRIX_KEY);

	// Reseteamos el temporizador
	tmr_startms((tmr_t*) (led_display.tmr_refresco_display), TIMEOUT_COLUMNA_DISPLAY);

}

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------

void timer_refresco_display_isr (union sigval value) {
	// A completar por el alumno...
	// ...
	piLock(SYSTEM_FLAGS_KEY);
	led_display.flags |= FLAG_TIMEOUT_COLUMNA_DISPLAY;
	piUnlock(SYSTEM_FLAGS_KEY);
}
