
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
	printf("Se mete a InicializaLedDisplay");

	piLock(MATRIX_KEY);
	// Ponemos los pines GPIO de las filas como entradas
	for (int i = 0; i < NUM_FILAS_DISPLAY; i++) {
		pinMode(led_display->filas[i], OUTPUT);
	}
	// Ponemos los pines GPIO de las columnas como entradas
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
	printf("Se mete a ApagaFilas");

	piLock(MATRIX_KEY);
	// Ponemos los pines GPIO de las filas como entradas y les asignamos sus rutinas de interrupción
	for (int i = 0; i < NUM_FILAS_DISPLAY; i++) {
		pinMode(led_display->filas[i], OUTPUT);
	}
	piUnlock(MATRIX_KEY);
}

void ExcitaColumnas(int columna) {
	printf("Se mete a ExcitaColumnas");

	/*switch(columna) {
		// A completar por el alumno...
		// ...
	}*/
	piLock(MATRIX_KEY);
	for (int i = 0; i < NUM_PINES_CONTROL_COLUMNAS_DISPLAY; i++) {
		// Ponemos la columna pasado como parámetro a HIGH y el resto a LOW
		if (i == columna)
			digitalWrite(led_display.pines_control_columnas[i], HIGH);
		else
			digitalWrite(led_display.pines_control_columnas[i], LOW);
	}
	piUnlock(MATRIX_KEY);
}

void ActualizaLedDisplay (TipoLedDisplay *led_display) {
	printf("Se mete a ActualizaLedDisplay");
	// A completar por el alumno...
	// ...
	ExcitaColumnas(led_display->p_columna);
	for (int i = 0; i < NUM_FILAS_DISPLAY; i++) {
		if ((led_display->pantalla.matriz[i][led_display->p_columna] == 1 ||
				led_display->pantalla.matriz[i][led_display->p_columna] == 8)) {
			digitalWrite(led_display->filas[i], HIGH);
		}
	}
}

//------------------------------------------------------
// FUNCIONES DE ENTRADA O DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaTimeoutColumnaDisplay (fsm_t* this) {
	printf("Se mete a CompruebaTimeoutColumnaDisplay");
	int result = 0;
	TipoLedDisplay *p_ledDisplay;
	p_ledDisplay = (TipoLedDisplay*)(this->user_data);

	// A completar por el alumno...
	// ...
	piLock(MATRIX_KEY);
	result = p_ledDisplay->flags & FLAG_TIMEOUT_COLUMNA_DISPLAY;
	piUnlock(MATRIX_KEY);

	return result;
}

//------------------------------------------------------
// FUNCIONES DE SALIDA O DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

void ActualizaExcitacionDisplay (fsm_t* this) {
	printf("Se mete a ActualizaExcitacionDisplay");
	TipoLedDisplay *p_ledDisplay;
	p_ledDisplay = (TipoLedDisplay*)(this->user_data);

	// A completar por el alumno...
	// ...

	if (p_ledDisplay->p_columna == NUM_COLUMNAS_DISPLAY - 1) {
		p_ledDisplay->p_columna = 0;
	} else {
		p_ledDisplay->p_columna++;
	}

	// Actualizamos el display en función de la matriz
	ActualizaLedDisplay(p_ledDisplay);

	// Reseteamos el temporizador
	tmr_startms((tmr_t*) (led_display.tmr_refresco_display), TIMEOUT_COLUMNA_DISPLAY);

}

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------

void timer_refresco_display_isr (union sigval value) {
	printf("Se mete a timer_refresco_display_isr");
	// A completar por el alumno...
	// ...
	piLock(MATRIX_KEY);
	led_display.flags |= FLAG_TIMEOUT_COLUMNA_DISPLAY;
	piUnlock(MATRIX_KEY);
}
