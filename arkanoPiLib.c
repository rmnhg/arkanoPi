#include "arkanoPiLib.h"

int activarTimer[MAX_PERIFERICOS_CONECTADOS + 1] = {0, 0, 0};

int ladrillos_basico[NUM_FILAS_DISPLAY][NUM_COLUMNAS_DISPLAY] = {
	{1,1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1,1},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
};

//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------

void PintaMensajeInicialPantalla (tipo_pantalla *p_pantalla, tipo_pantalla *p_pantalla_inicial) {
	int i, j = 0;

	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		for(j=0;j<NUM_COLUMNAS_DISPLAY;j++) {
			p_pantalla->matriz[i][j] = p_pantalla_inicial->matriz[i][j];
		}
	}

	return;
}

void PintaPantallaPorTerminal (tipo_pantalla *p_pantalla) {
#ifdef __SIN_PSEUDOWIRINGPI__
	int i=0, j=0;

	printf("\n[PANTALLA]\n");
	fflush(stdout);
	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		for(j=0;j<NUM_COLUMNAS_DISPLAY;j++) {
			printf("%d", p_pantalla->matriz[i][j]);
			fflush(stdout);
		}
		printf("\n");
		fflush(stdout);
	}
	fflush(stdout);
#endif
}

void ReseteaPantalla (tipo_pantalla *p_pantalla) {
	int i=0, j=0;

	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		for(j=0;j<NUM_COLUMNAS_DISPLAY;j++) {
			p_pantalla->matriz[i][j] = 0;
		}
	}
}


//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------

void InicializaLadrillos(tipo_pantalla *p_ladrillos) {
	int i=0, j=0;

	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		for(j=0;j<NUM_COLUMNAS_DISPLAY;j++) {
			p_ladrillos->matriz[i][j] = ladrillos_basico[i][j];
		}
	}
}

void InicializaPelota(tipo_pelota *p_pelota) {
	// Aleatorizamos la posicion inicial de la pelota
	p_pelota->x = rand() % NUM_COLUMNAS_DISPLAY;
	p_pelota->y = 2 + rand() % (NUM_FILAS_DISPLAY-3); // 2 evita que aparezca encima de ladrillos y para que no empiece demasiado pegada al suelo de la pantalla

	// Pelota inicialmente en el centro de la pantalla
	//p_pelota->x = NUM_COLUMNAS_DISPLAY/2 - 1;
	//p_pelota->y = NUM_FILAS_DISPLAY/2 -1 ;

	InicializaPosiblesTrayectorias(p_pelota);

	// Trayectoria inicial
	//p_pelota->trayectoria.xv = 0;
	//p_pelota->trayectoria.yv = 1;
	CambiarDireccionPelota(p_pelota, rand() % p_pelota->num_posibles_trayectorias);
}

void InicializaPala(tipo_pala *p_pala) {
	// Pala inicialmente en el centro de la pantalla
	p_pala->x = NUM_COLUMNAS_DISPLAY/2 - p_pala->ancho/2;
	p_pala->y = NUM_FILAS_DISPLAY - 1;
	p_pala->ancho = NUM_COLUMNAS_PALA;
	p_pala->alto = NUM_FILAS_PALA;
}

void InicializaPosiblesTrayectorias(tipo_pelota *p_pelota) {
	p_pelota->num_posibles_trayectorias = 0;
	p_pelota->posibles_trayectorias[ARRIBA_IZQUIERDA].xv = -1;
	p_pelota->posibles_trayectorias[ARRIBA_IZQUIERDA].yv = -1;
	p_pelota->num_posibles_trayectorias++;
	p_pelota->posibles_trayectorias[ARRIBA].xv = 0;
	p_pelota->posibles_trayectorias[ARRIBA].yv = -1;
	p_pelota->num_posibles_trayectorias++;
	p_pelota->posibles_trayectorias[ARRIBA_DERECHA].xv = 1;
	p_pelota->posibles_trayectorias[ARRIBA_DERECHA].yv = -1;
	p_pelota->num_posibles_trayectorias++;

	p_pelota->posibles_trayectorias[ABAJO_DERECHA].xv = 1;
	p_pelota->posibles_trayectorias[ABAJO_DERECHA].yv = 1;
	p_pelota->num_posibles_trayectorias++;
	p_pelota->posibles_trayectorias[ABAJO].xv = 0;
	p_pelota->posibles_trayectorias[ABAJO].yv = 1;
	p_pelota->num_posibles_trayectorias++;
	p_pelota->posibles_trayectorias[ABAJO_IZQUIERDA].xv = -1;
	p_pelota->posibles_trayectorias[ABAJO_IZQUIERDA].yv = 1;
	p_pelota->num_posibles_trayectorias++;

	//p_pelota->posibles_trayectorias[IZQUIERDA].xv = -1;
	//p_pelota->posibles_trayectorias[IZQUIERDA].yv = 0;
	//p_pelota->num_posibles_trayectorias++;
	//p_pelota->posibles_trayectorias[DERECHA].xv = 1;
	//p_pelota->posibles_trayectorias[DERECHA].yv = 0;
	//p_pelota->num_posibles_trayectorias++;
}

void PintaLadrillos(tipo_pantalla *p_ladrillos, tipo_pantalla *p_pantalla) {
	int i=0, j=0;

	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		for(j=0;j<NUM_COLUMNAS_DISPLAY;j++) {
			p_pantalla->matriz[i][j] = p_ladrillos->matriz[i][j];
		}
    }
}

void PintaPala(tipo_pala *p_pala, tipo_pantalla *p_pantalla) {
	int i=0, j=0;

	for(i=0;i<NUM_FILAS_PALA;i++) {
		for(j=0;j<NUM_COLUMNAS_PALA;j++) {
			if (( (p_pala->y+i >= 0) && (p_pala->y+i < NUM_FILAS_DISPLAY) ) &&
				( (p_pala->x+j >= 0) && (p_pala->x+j < NUM_COLUMNAS_DISPLAY) ))
				p_pantalla->matriz[p_pala->y+i][p_pala->x+j] = 1;
		}
	}
}

void PintaPelota(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla) {
	if( (p_pelota->x >= 0) && (p_pelota->x < NUM_COLUMNAS_DISPLAY) ) {
		if( (p_pelota->y >= 0) && (p_pelota->y < NUM_FILAS_DISPLAY) ) {
			p_pantalla->matriz[p_pelota->y][p_pelota->x] = 8;
		}
		else {
			printf("\n\nPROBLEMAS!!!! posicion y=%d de la pelota INVALIDA!!!\n\n", p_pelota->y);
			fflush(stdout);
		}
	}
	else {
		printf("\n\nPROBLEMAS!!!! posicion x=%d de la pelota INVALIDA!!!\n\n", p_pelota->x);
		fflush(stdout);
	}
}

void ActualizaPantalla(tipo_arkanoPi* p_arkanoPi) {

    // Borro toda la pantalla
	ReseteaPantalla((tipo_pantalla*)(p_arkanoPi->p_pantalla));

    // Pinta los ladrillos
	PintaLadrillos(
		(tipo_pantalla*)(&(p_arkanoPi->ladrillos)),
		(tipo_pantalla*)(p_arkanoPi->p_pantalla));

    // Pinta la pala
	PintaPala(
		(tipo_pala*)(&(p_arkanoPi->pala)),
		(tipo_pantalla*)(p_arkanoPi->p_pantalla));

	// Pinta las pelotas
	for (int i = 0; i < p_arkanoPi->numeroPelotas; i++) {
		if (p_arkanoPi->pelota[i].y >= 0) {
			PintaPelota(
				(tipo_pelota*)(&(p_arkanoPi->pelota[i])),
				(tipo_pantalla*)(p_arkanoPi->p_pantalla));
		}
	}
}

void InicializaArkanoPi(tipo_arkanoPi *p_arkanoPi) {
	ResetArkanoPi(p_arkanoPi);
	ActualizaPantalla(p_arkanoPi);
}

void ResetArkanoPi(tipo_arkanoPi *p_arkanoPi) {
	int posiciones_pelotas[p_arkanoPi->numeroPelotas][3]; // En la segunda dimensión, se guarda la x, la y y la paridad de la x
	int pelotas_unicas = FALSE;
	int paridad_igual = TRUE; // Significa que todas las pelotas tienen paridad en la x igual
	ReseteaPantalla((tipo_pantalla*)(p_arkanoPi->p_pantalla));
	InicializaLadrillos((tipo_pantalla*)(&(p_arkanoPi->ladrillos)));
	// Inicializamos las pelotas
	for (int i = 0; i < p_arkanoPi->numeroPelotas; i++) {
		InicializaPelota((tipo_pelota*)(&(p_arkanoPi->pelota[i])));
		posiciones_pelotas[i][0] = p_arkanoPi->pelota[i].x;
		posiciones_pelotas[i][1] = p_arkanoPi->pelota[i].y;
		posiciones_pelotas[i][2] = p_arkanoPi->pelota[i].x % 2;
	}

	// Comprobamos que ninguna pelota está en el mismo sitio que las demás
	if (p_arkanoPi->numeroPelotas > 1) {
		while (!pelotas_unicas || paridad_igual) {	// Si las pelotas no son unicas o la paridad es igual
			pelotas_unicas = TRUE; // Caso ideal
			paridad_igual = TRUE; // Caso peor
			for (int i = 0; i < p_arkanoPi->numeroPelotas; i++) {
				for (int j = 0; j < p_arkanoPi->numeroPelotas - 1; j++) {
					// Si una pelota tiene distinta paridad a otra, se guarda y no se vuelve a comprobar
					if ((i != j) && paridad_igual && (posiciones_pelotas[i][2] != posiciones_pelotas[j][2])) {
						paridad_igual = FALSE;
					}
					// Si llegamos a la última pelota y todas tienen la misma paridad, se cambia
					if ((i == p_arkanoPi->numeroPelotas-1) && (j == p_arkanoPi->numeroPelotas-2) && paridad_igual) {
						paridad_igual = FALSE;
						if (posiciones_pelotas[j][0] < NUM_COLUMNAS_DISPLAY - 1) {
							posiciones_pelotas[j][0]++;
						} else {
							posiciones_pelotas[j][0]--;
						}
					}
					// Si una pelota tiene la misma posición que otra, se cambia su posición
					if ((i != j) && (posiciones_pelotas[i][0] == posiciones_pelotas[j][0]) && (posiciones_pelotas[i][1] == posiciones_pelotas[j][1])) {
						InicializaPelota((tipo_pelota*)(&(p_arkanoPi->pelota[i])));
						//Actualizamos su posición en el array de posiciones
						posiciones_pelotas[i][0] = p_arkanoPi->pelota[i].x;
						posiciones_pelotas[i][1] = p_arkanoPi->pelota[i].y;
						posiciones_pelotas[i][2] = p_arkanoPi->pelota[i].x % 2;
						// Como hemos cambiado la posición de la pelota, habrá que comprobar e nuevo las condiciones que queremos
						pelotas_unicas = FALSE;
						paridad_igual = TRUE;
					}
				}
			}
		}
	}

	InicializaPala((tipo_pala*)(&(p_arkanoPi->pala)));
}

void CambiarDireccionPelota(tipo_pelota *p_pelota, enum t_direccion direccion) {
	if((direccion < 0)||(direccion > p_pelota->num_posibles_trayectorias)) {
		printf("[ERROR!!!!][direccion NO VALIDA!!!!][%d]", direccion);
		return;
	}
	else {
		p_pelota->trayectoria.xv = p_pelota->posibles_trayectorias[direccion].xv;
		p_pelota->trayectoria.yv = p_pelota->posibles_trayectorias[direccion].yv;
	}
}

void ActualizaPosicionPala(tipo_pala *p_pala, enum t_direccion direccion) {
	switch (direccion) {
		case DERECHA:
			// Dejamos que la pala rebase parcialmente el límite del area de juego
			if( p_pala->x + 1 + p_pala->ancho <= NUM_COLUMNAS_DISPLAY + 2 )
				p_pala->x = p_pala->x + 1;
			break;
		case IZQUIERDA:
			// Dejamos que la pala rebase parcialmente el límite del area de juego
			if( p_pala->x - 1 >= -2)
					p_pala->x = p_pala->x - 1;
			break;
		default:
			printf("[ERROR!!!!][direccion NO VALIDA!!!!][%d]", direccion);
			break;
	}
}

void ActualizaPosicionPelota (tipo_pelota *p_pelota, int paredesHabilitadas) {
	p_pelota->x += p_pelota->trayectoria.xv;
	if (!paredesHabilitadas && p_pelota->x == NUM_COLUMNAS_DISPLAY) {
		p_pelota->x = 0;
	} else if (!paredesHabilitadas && p_pelota->x == -1) {
		p_pelota->x = NUM_COLUMNAS_DISPLAY - 1;
	}
	p_pelota->y += p_pelota->trayectoria.yv;
}

int CompruebaReboteLadrillo (tipo_arkanoPi *p_arkanoPi, int pelota) {
	int p_posible_ladrillo_x = 0;
	int p_posible_ladrillo_y = 0;

	p_posible_ladrillo_x = p_arkanoPi->pelota[pelota].x + p_arkanoPi->pelota[pelota].trayectoria.xv;
	if (!(p_arkanoPi->paredesHabilitadas) && p_posible_ladrillo_x == NUM_COLUMNAS_DISPLAY) {
		p_posible_ladrillo_x = 0;
	} else if (!(p_arkanoPi->paredesHabilitadas) && p_posible_ladrillo_x == -1) {
		p_posible_ladrillo_x = NUM_COLUMNAS_DISPLAY - 1;
	}

	if ( ( p_posible_ladrillo_x < 0) || ( p_posible_ladrillo_x >= NUM_COLUMNAS_DISPLAY ) ) {
		printf("\n\nERROR GRAVE!!! p_posible_ladrillo_x = %d!!!\n\n", p_posible_ladrillo_x);
		fflush(stdout);
		exit(-1);
	}

	p_posible_ladrillo_y = p_arkanoPi->pelota[pelota].y + p_arkanoPi->pelota[pelota].trayectoria.yv;

	if ( ( p_posible_ladrillo_y < 0) || ( p_posible_ladrillo_y >= NUM_FILAS_DISPLAY ) ) {
		printf("\n\nERROR GRAVE!!! p_posible_ladrillo_y = %d!!!\n\n", p_posible_ladrillo_y);
		fflush(stdout);
	}

	if(p_arkanoPi->ladrillos.matriz[p_posible_ladrillo_y][p_posible_ladrillo_x] > 0 ) {
		// La pelota ha entrado en el area de ladrillos
		// y descontamos el numero de golpes que resta para destruir el ladrillo
		p_arkanoPi->ladrillos.matriz[p_posible_ladrillo_y][p_posible_ladrillo_x] = p_arkanoPi->ladrillos.matriz[p_posible_ladrillo_y][p_posible_ladrillo_x] - 1;

		return 1;
	}
	return 0;
}

int CompruebaReboteParedesVerticales (tipo_arkanoPi arkanoPi, int pelota) {
	// Comprobamos si la nueva posicion de la pelota excede los limites de la pantalla
	if((arkanoPi.pelota[pelota].x + arkanoPi.pelota[pelota].trayectoria.xv >= NUM_COLUMNAS_DISPLAY) ||
		(arkanoPi.pelota[pelota].x + arkanoPi.pelota[pelota].trayectoria.xv < 0) ) {
		// La pelota rebota contra la pared derecha o izquierda
		return 1;
	}
	return 0;
}

int CompruebaReboteTecho (tipo_arkanoPi arkanoPi, int pelota) {
	// Comprobamos si la nueva posicion de la pelota excede los limites de la pantalla
	if(arkanoPi.pelota[pelota].y + arkanoPi.pelota[pelota].trayectoria.yv < 0) {
		// La pelota rebota contra la pared derecha o izquierda
		return 1;
	}
	return 0;
}

int CompruebaRebotePala (tipo_arkanoPi arkanoPi, int pelota) {
	if(arkanoPi.pelota[pelota].trayectoria.yv > 0) { // Esta condicion solo tiene sentido si la pelota va hacia abajo en la pantalla
		if ((arkanoPi.pelota[pelota].x + arkanoPi.pelota[pelota].trayectoria.xv >= arkanoPi.pala.x ) &&
			(arkanoPi.pelota[pelota].x + arkanoPi.pelota[pelota].trayectoria.xv < arkanoPi.pala.x + NUM_COLUMNAS_PALA)) {
				if ((arkanoPi.pelota[pelota].y + arkanoPi.pelota[pelota].trayectoria.yv >= arkanoPi.pala.y) &&
					(arkanoPi.pelota[pelota].y + arkanoPi.pelota[pelota].trayectoria.yv < arkanoPi.pala.y + NUM_FILAS_PALA)) {
					return 1;
				}
		}
	}
	return 0;
}

int CompruebaFallo (tipo_arkanoPi arkanoPi, int pelota) {
	// Comprobamos si no hemos conseguido devolver la pelota
	if(arkanoPi.pelota[pelota].y + arkanoPi.pelota[pelota].trayectoria.yv >= NUM_FILAS_DISPLAY) {
		// Hemos fallado
		return 1;
	}
	return 0;
}

int CalculaLadrillosRestantes(tipo_pantalla *p_ladrillos) {
	int i=0, j=0;
	int numLadrillosRestantes;

	numLadrillosRestantes = 0;

	for(i=0;i<NUM_FILAS_DISPLAY;i++) {
		for(j=0;j<NUM_COLUMNAS_DISPLAY;j++) {
			if(p_ladrillos->matriz[i][j] != 0) {
				numLadrillosRestantes++;
			}
		}
	}

	return numLadrillosRestantes;
}

//------------------------------------------------------
// FUNCIONES DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

int CompruebaBotonPulsado (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag del botón ha sido activado
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_BOTON);
	piUnlock(SYSTEM_FLAGS_KEY);
	// A completar por el alumno
	// Hecho

	return result;
}

int CompruebaPausaPulsada (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag del botón de pausa ha sido activado
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_PAUSA);
	piUnlock(SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaMovimientoIzquierda(fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag del movimiento a la izquierda ha sido activado
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_MOV_IZQUIERDA);
	piUnlock(SYSTEM_FLAGS_KEY);
	// A completar por el alumno
	// Hecho

	return result;
}

int CompruebaMovimientoDerecha(fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag del movimiento a la derecha ha sido activado
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_MOV_DERECHA);
	piUnlock(SYSTEM_FLAGS_KEY);
	// A completar por el alumno
	// Hecho

	return result;
}

int CompruebaTimeoutActualizacionJuego (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag del timer del juego ha sido activado
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_TIMER_JUEGO);
	piUnlock(SYSTEM_FLAGS_KEY);
	// A completar por el alumno
	// Hecho

	return result;
}

int CompruebaFinalJuego(fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag del fin del juego ha sido activado
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_FIN_JUEGO);
	piUnlock(SYSTEM_FLAGS_KEY);
	// A completar por el alumno
	// Hecho

	return result;
}

int CompruebaNumeroPelotas(fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag del submenu de número de pelotas está activo
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_MENU_PELOTAS);
	piUnlock(SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaParedes(fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag del submenu de paredes está activo
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_MENU_PAREDES);
	piUnlock(SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaTCP(fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag del submenu de TCP está activo
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_MENU_TCP);
	piUnlock(SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaAyuda(fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag del submenu de ayuda está activo
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_MENU_AYUDA);
	piUnlock(SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaMenosPulsado(fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag del menos está activo
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_MENOS);
	piUnlock(SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaMasPulsado(fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag del menos está activo
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_MAS);
	piUnlock(SYSTEM_FLAGS_KEY);

	return result;
}

int CompruebaSalirMenuPulsado(fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);
	int result = 0;

	// Comprobamos si el flag de salir del submenú está activo
	piLock(SYSTEM_FLAGS_KEY);
	result = (flags[p_arkanoPi->partida] & FLAG_SALIR);
	piUnlock(SYSTEM_FLAGS_KEY);

	return result;
}

//------------------------------------------------------
// FUNCIONES DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------

// void InicializaJuego (void): funcion encargada de llevar a cabo
// la oportuna inicialización de toda variable o estructura de datos
// que resulte necesaria para el desarrollo del juego.

void InicializaJuego(fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// Cancelamos los posibles flags generados por las teclas de control además del flag del botón
	piLock(SYSTEM_FLAGS_KEY);
	flags[p_arkanoPi->partida] &= ~FLAG_BOTON;
	flags[p_arkanoPi->partida] &= ~FLAG_MOV_DERECHA;
	flags[p_arkanoPi->partida] &= ~FLAG_MOV_IZQUIERDA;
	flags[p_arkanoPi->partida] &= ~FLAG_TIMER_JUEGO;
	piUnlock(SYSTEM_FLAGS_KEY);

	// Inicializamos las variables del juego
	InicializaArkanoPi(p_arkanoPi);

	// Finalmente pintamos la pantalla por terminal
	piLock(STD_IO_BUFFER_KEY);
	PintaPantallaPorTerminal(p_arkanoPi->p_pantalla);
	piUnlock(STD_IO_BUFFER_KEY);

	// Habilitamos la pantalla emulada
	if (p_arkanoPi->partida == 0)
		pseudoWiringPiEnableDisplay(1);

	// Inicializamos el primer timer
	tmr_startms((tmr_t*)p_arkanoPi->tmr_actualizacion_juego, TIMEOUT_ACTUALIZA_JUEGO);
	piLock(SYSTEM_FLAGS_KEY);
	activarTimer[p_arkanoPi->partida] = 1;
	piUnlock(SYSTEM_FLAGS_KEY);
}

// void MuevePalaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la pala.
// Debe garantizar la viabilidad del mismo mediante la comprobación
// de que la nueva posición correspondiente a la pala no suponga
// que ésta rebase o exceda los límites definidos para el área de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).

void MuevePalaIzquierda (fsm_t* this) {
	tipo_arkanoPi* p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	piLock(SYSTEM_FLAGS_KEY);
	flags[p_arkanoPi->partida] &= ~FLAG_MOV_IZQUIERDA;
	piUnlock(SYSTEM_FLAGS_KEY);
	ActualizaPosicionPala(&p_arkanoPi->pala, IZQUIERDA);
	piLock(MATRIX_KEY);
	ActualizaPantalla(p_arkanoPi);
	piUnlock(MATRIX_KEY);
	piLock(STD_IO_BUFFER_KEY);
	PintaPantallaPorTerminal(p_arkanoPi->p_pantalla);
	piUnlock(STD_IO_BUFFER_KEY);
	// A completar por el alumno
	// Hecho
}

// void MuevePalaDerecha (void): función similar a la anterior
// encargada del movimiento hacia la derecha.

void MuevePalaDerecha (fsm_t* this) {
	tipo_arkanoPi* p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	piLock(SYSTEM_FLAGS_KEY);
	flags[p_arkanoPi->partida] &= ~FLAG_MOV_DERECHA;
	piUnlock(SYSTEM_FLAGS_KEY);
	ActualizaPosicionPala(&p_arkanoPi->pala, DERECHA);
	piLock(MATRIX_KEY);
	ActualizaPantalla(p_arkanoPi);
	piUnlock(MATRIX_KEY);
	piLock(STD_IO_BUFFER_KEY);
	PintaPantallaPorTerminal(p_arkanoPi->p_pantalla);
	piUnlock(STD_IO_BUFFER_KEY);
	// A completar por el alumno
	// Hech
}

// void ActualizarJuego (void): función encargada de actualizar la
// posición de la pelota conforme a la trayectoria definida para ésta.
// Para ello deberá identificar los posibles rebotes de la pelota para,
// en ese caso, modificar su correspondiente trayectoria (los rebotes
// detectados contra alguno de los ladrillos implicarán adicionalmente
// la eliminación del ladrillo). Del mismo modo, deberá también
// identificar las situaciones en las que se dé por finalizada la partida:
// bien porque el jugador no consiga devolver la pelota, y por tanto ésta
// rebase el límite inferior del área de juego, bien porque se agoten
// los ladrillos visibles en el área de juego.

void ActualizarJuego (fsm_t* this) {
	tipo_arkanoPi* p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	int pelotasEnJuego = p_arkanoPi->numeroPelotas;
	int pelotasCaidas = 0;

	/*
	  Bola 0 caida, ¿bola 1 caida? -> Si (fin juego)
	  -> No, se sigue juego
	  Bola 1 caida, ¿bola 0 caida? -> Si (fin juego)
	  -> No, se sigue jugando
	 */

	// Eliminamos los flags que ya hemos cubierto
	piLock(SYSTEM_FLAGS_KEY);
	flags[p_arkanoPi->partida] &= (~FLAG_TIMER_JUEGO);
	flags[p_arkanoPi->partida] &= (~FLAG_BOTON);
	piUnlock(SYSTEM_FLAGS_KEY);

	for (int i = 0; i < p_arkanoPi->numeroPelotas; i++) {
		if (p_arkanoPi->pelota[i].y >= 0) {
			if (p_arkanoPi->paredesHabilitadas && CompruebaReboteParedesVerticales(*(p_arkanoPi), i)){
				p_arkanoPi->pelota[i].trayectoria.xv *=-1;
			}
			if (CompruebaReboteTecho(*(p_arkanoPi), i)){
				p_arkanoPi->pelota[i].trayectoria.yv *=-1;
			}
			if (CompruebaFallo(*(p_arkanoPi), i)){
				// Sacamos la pelota actual fuera de la pantalla
				p_arkanoPi->pelota[i].y = -1;
				pelotasEnJuego = p_arkanoPi->numeroPelotas;
				pelotasEnJuego--;
				// Comprobamos si la otra pelota también ha fallado
				for (int j = 0; j < p_arkanoPi->numeroPelotas; j++) {
					if ((j != i) && (p_arkanoPi->pelota[j].y < 0)) {
						pelotasEnJuego--;
					}
				}
				// Si no quedan pelotas en juego se termina la partida
				if (pelotasEnJuego == 0) {
					piLock(SYSTEM_FLAGS_KEY);
					flags[p_arkanoPi->partida] |= FLAG_FIN_JUEGO;
					piUnlock(SYSTEM_FLAGS_KEY);

					// Pintamos la pantalla final y habilitamos la pantalla para que se muestre
					piLock(MATRIX_KEY);
					PintaMensajeInicialPantalla(p_arkanoPi->p_pantalla, &pantalla_final);
					piUnlock(MATRIX_KEY);
					return;
				}
				// Si quedan pelotas en juego pero se ha caido esta, se comprueban las demás
				continue;
			} else if (CompruebaRebotePala(*(p_arkanoPi), i)){
				switch (p_arkanoPi->pelota[i].x + p_arkanoPi->pelota[i].trayectoria.xv - p_arkanoPi->pala.x){
					case 0:
						CambiarDireccionPelota(&(p_arkanoPi->pelota[i]),ARRIBA_IZQUIERDA);
						break;
					case 1:
						CambiarDireccionPelota(&(p_arkanoPi->pelota[i]),ARRIBA);
						break;
					case 2:
						CambiarDireccionPelota(&(p_arkanoPi->pelota[i]),ARRIBA_DERECHA);
						break;
				}
				if (p_arkanoPi->paredesHabilitadas && CompruebaReboteParedesVerticales(*(p_arkanoPi), i)){
					p_arkanoPi->pelota[i].trayectoria.xv *=-1;
				}
			}
			if (CompruebaReboteLadrillo(p_arkanoPi, i)){
				p_arkanoPi->pelota[i].trayectoria.yv *= -1;

				if (CalculaLadrillosRestantes(&(p_arkanoPi->ladrillos)) <= 0){
					piLock(SYSTEM_FLAGS_KEY);
					flags[p_arkanoPi->partida] |= FLAG_FIN_JUEGO;
					piUnlock(SYSTEM_FLAGS_KEY);

					// Pintamos la pantalla final y habilitamos la pantalla para que se muestre
					piLock(MATRIX_KEY);
					PintaMensajeInicialPantalla(p_arkanoPi->p_pantalla, &pantalla_final);
					piUnlock(MATRIX_KEY);
					return;
				}
			}
			ActualizaPosicionPelota(&(p_arkanoPi->pelota[i]), p_arkanoPi->paredesHabilitadas);
		} else {
			pelotasCaidas++;
		}
	}

	// Si no quedan pelotas en juego se termina la partida
	if (pelotasCaidas == p_arkanoPi->numeroPelotas) {
		piLock(SYSTEM_FLAGS_KEY);
		flags[p_arkanoPi->partida] |= FLAG_FIN_JUEGO;
		piUnlock(SYSTEM_FLAGS_KEY);

		// Pintamos la pantalla final y habilitamos la pantalla para que se muestre
		piLock(MATRIX_KEY);
		PintaMensajeInicialPantalla(p_arkanoPi->p_pantalla, &pantalla_final);
		piUnlock(MATRIX_KEY);
		return;
	}

	piLock(MATRIX_KEY);
	ActualizaPantalla(p_arkanoPi);
	piUnlock(MATRIX_KEY);
	piLock(STD_IO_BUFFER_KEY);
	PintaPantallaPorTerminal(p_arkanoPi->p_pantalla);
	piUnlock(STD_IO_BUFFER_KEY);

	// Inicializamos el timer
	tmr_startms((tmr_t*)p_arkanoPi->tmr_actualizacion_juego, TIMEOUT_ACTUALIZA_JUEGO);
	piLock(SYSTEM_FLAGS_KEY);
	activarTimer[p_arkanoPi->partida] = 1;
	piUnlock(SYSTEM_FLAGS_KEY);
	// A completar por el alumno
	// Hecho
}

// void FinalJuego (void): función encargada de mostrar en la ventana de
// terminal los mensajes necesarios para informar acerca del resultado del juego.

void FinalJuego (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// Eliminamos los flags que estamos atendiendo
	piLock(SYSTEM_FLAGS_KEY);
	flags[p_arkanoPi->partida] &= ~FLAG_FIN_JUEGO;
	flags[p_arkanoPi->partida] &= ~FLAG_BOTON;
	piUnlock(SYSTEM_FLAGS_KEY);

	// Imprimimos por consola los resultados de la partida
	piLock(STD_IO_BUFFER_KEY);
	enviarConsola(p_arkanoPi->partida, "\nHas destruido %d ladrillos. ¡Enhorabuena!\n"
				  "Pulsa cualquier tecla para jugar de nuevo.\n"
				  "Si quieres salir pulsa la tecla F.\n", NUM_COLUMNAS_DISPLAY * 2 - CalculaLadrillosRestantes(&(p_arkanoPi->ladrillos)));
	fflush(stdout);
	piUnlock(STD_IO_BUFFER_KEY);

	if (p_arkanoPi->partida == 0)
		pseudoWiringPiEnableDisplay(1);

	// A completar por el alumno
	// Hecho
}

//void ReseteaJuego (void): función encargada de llevar a cabo la
// reinicialización de cuantas variables o estructuras resulten
// necesarias para dar comienzo a una nueva partida.

void ReseteaJuego (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// Cancelamos los posibles flags generados por las teclas de control además del flag del botón
	piLock(SYSTEM_FLAGS_KEY);
	flags[p_arkanoPi->partida] &= ~FLAG_BOTON;
	flags[p_arkanoPi->partida] &= ~FLAG_MOV_DERECHA;
	flags[p_arkanoPi->partida] &= ~FLAG_MOV_IZQUIERDA;
	flags[p_arkanoPi->partida] &= ~FLAG_TIMER_JUEGO;
	piUnlock(SYSTEM_FLAGS_KEY);

	// Inicializamos el juego
	ResetArkanoPi(p_arkanoPi);

	// Deshabilitamos la pantalla para escribir por consola
	if (p_arkanoPi->partida == 0) {
		pseudoWiringPiEnableDisplay(0);
	}

	// Imprimimos el saludo y las instrucciones del juego
	piLock(STD_IO_BUFFER_KEY);
	mostrarInstruccionesJuego(p_arkanoPi->partida, 0);
	piUnlock(STD_IO_BUFFER_KEY);

	// Pintamos la pantalla inicial
	piLock(MATRIX_KEY);
	PintaMensajeInicialPantalla(p_arkanoPi->p_pantalla, &pantalla_inicial);
	piUnlock(MATRIX_KEY);

	// Volvemos a habilitar la pantalla
	if (p_arkanoPi->partida == 0) {
		pseudoWiringPiEnableDisplay(1);
	}

	// A completar por el alumno
	// Hecho
}

/**
  * Función que pausa o reactiva la partida.
  */
void PausarJuego (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// Cancelamos el flag de pausa
	piLock(SYSTEM_FLAGS_KEY);
	flags[p_arkanoPi->partida] &= ~FLAG_PAUSA;
	piUnlock(SYSTEM_FLAGS_KEY);
}

void ActivarMenu (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// Cancelamos los flags de salir del submenú y de botón por si estaba activado, para no empezar el juego directamente
	piLock(SYSTEM_FLAGS_KEY);
	flags[p_arkanoPi->partida] &= ~FLAG_SALIR;
	flags[p_arkanoPi->partida] &= ~FLAG_BOTON;
	piUnlock(SYSTEM_FLAGS_KEY);

	// Desactivamos la pantalla para mostrar el menú
	if (p_arkanoPi->partida == 0) {
		pseudoWiringPiEnableDisplay(0);
	}
	// Ponemos que es la primera vez que se muestra un submenú
	p_arkanoPi->primerAccesoSubmenu = 1;
	MostrarMenu(p_arkanoPi->partida);
}

void MostrarSubmenuPelotas (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// Cancelamos el flag del submenu del número de pelotas
	piLock(SYSTEM_FLAGS_KEY);
	flags[p_arkanoPi->partida] &= ~FLAG_MENU_PELOTAS;
	if (flags[p_arkanoPi->partida] & FLAG_MAS) {
		if (p_arkanoPi->numeroPelotas < MAX_PELOTAS) {
			p_arkanoPi->numeroPelotas++;
		}
		flags[p_arkanoPi->partida] &= ~FLAG_MAS;
	} else if (flags[p_arkanoPi->partida] & FLAG_MENOS) {
		if (p_arkanoPi->numeroPelotas > 1) {
			p_arkanoPi->numeroPelotas--;
		}
		flags[p_arkanoPi->partida] &= ~FLAG_MENOS;
	}
	piUnlock(SYSTEM_FLAGS_KEY);

	if (p_arkanoPi->partida == 0) {
		pseudoWiringPiEnableDisplay(0);
	}
	piLock(STD_IO_BUFFER_KEY);
	if (p_arkanoPi->primerAccesoSubmenu) {
		p_arkanoPi->primerAccesoSubmenu = 0;
	} else {
		if (p_arkanoPi->partida == 0) {
			printf("\033[A\033[2K\033[A\033[2K\033[A\033[2K\033[A");
		}
	}
	enviarConsola(p_arkanoPi->partida, "\nPulse el número 7 para disminuir el número de pelotas o 9 para aumentarlo.\nActualmente hay %d pelotas.\nPara volver al menú pulse 5.\n", p_arkanoPi->numeroPelotas);
	fflush(stdout);
	piUnlock(STD_IO_BUFFER_KEY);
}

void MostrarSubmenuParedes (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// Cancelamos el flag del submenu de paredes
	piLock(SYSTEM_FLAGS_KEY);
	flags[p_arkanoPi->partida] &= ~FLAG_MENU_PAREDES;
	if (flags[p_arkanoPi->partida] & FLAG_MAS) {
		p_arkanoPi->paredesHabilitadas = 1;
		flags[p_arkanoPi->partida] &= ~FLAG_MAS;
	} else if (flags[p_arkanoPi->partida] & FLAG_MENOS) {
		p_arkanoPi->paredesHabilitadas = 0;
		flags[p_arkanoPi->partida] &= ~FLAG_MENOS;
	}
	piUnlock(SYSTEM_FLAGS_KEY);

	if (p_arkanoPi->partida == 0) {
		pseudoWiringPiEnableDisplay(0);
	}
	piLock(STD_IO_BUFFER_KEY);
	if (p_arkanoPi->primerAccesoSubmenu) {
		p_arkanoPi->primerAccesoSubmenu = 0;
	} else {
		if (p_arkanoPi->partida == 0) {
			printf("\033[A\033[2K\033[A\033[2K\033[A\033[2K\033[A");
		}
	}
	if (p_arkanoPi->paredesHabilitadas)
		enviarConsola(p_arkanoPi->partida, "\nPulse el número 7 para deshabilitar las paredes o 9 para habilitarlo.\nActualmente están habilitadas.\nPara volver al menú pulse 5.\n");
	else
		enviarConsola(p_arkanoPi->partida, "\nPulse el número 7 para deshabilitar las paredes o 9 para habilitarlo.\nActualmente están deshabilitadas.\nPara volver al menú pulse 5.\n");
	fflush(stdout);
	piUnlock(STD_IO_BUFFER_KEY);
}

void MostrarSubmenuTCP (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// Cancelamos el flag del submenu de TCP
	piLock(SYSTEM_FLAGS_KEY);
	flags[p_arkanoPi->partida] &= ~FLAG_MENU_TCP;
	if (flags[p_arkanoPi->partida] & FLAG_MAS) {
		if (!compruebaServidorHabilitado()) {
			habilitarServidor();
			// Lanzamos un thread para gestionar las conexiones TCP de los periféricos externos
			int result = piThreadCreate(thread_conexion);
			if (result != 0) {
				printf("No se pudo crear el thread thread_conexion.\n");
				return;
			}
		}
		flags[p_arkanoPi->partida] &= ~FLAG_MAS;
	} else if (flags[p_arkanoPi->partida] & FLAG_MENOS) {
		if (compruebaServidorHabilitado()) {
			cerrarConexion();
		}
		flags[p_arkanoPi->partida] &= ~FLAG_MENOS;
	}
	piUnlock(SYSTEM_FLAGS_KEY);

	if (p_arkanoPi->partida == 0) {
		pseudoWiringPiEnableDisplay(0);
	}
	piLock(STD_IO_BUFFER_KEY);
	if (p_arkanoPi->primerAccesoSubmenu) {
		p_arkanoPi->primerAccesoSubmenu = 0;
	} else {
		if (p_arkanoPi->partida == 0) {
			printf("\033[A\033[2K\033[A\033[2K\033[A\033[2K\033[A");
		}
	}
	if (compruebaServidorHabilitado())
		enviarConsola(p_arkanoPi->partida, "\nPulse el número 7 para deshabilitar la conexión TCP o 9 para habilitarlo.\nActualmente está habilitado.\nPara volver al menú pulse 5.\n");
	else
		enviarConsola(p_arkanoPi->partida, "\nPulse el número 7 para deshabilitar la conexión TCP o 9 para habilitarlo.\nActualmente está deshabilitado.\nPara volver al menú pulse 5.\n");
	//printf("\033[A\033[A"); // Para reescribir sobre las líneas anteriores.
	// \u001b[1G is a "Cursor Horizontal Absolute" code. The 1 means it tries to move the cursor to the first character of the line.
	// \u001b[2K is a "Erase in Line" code. The 2 makes it mean "Erase the entire line".
	// \033[A\033[2K\033[A\033[2K
	// printf("\033[2J\033[1;1H");
	fflush(stdout);
	piUnlock(STD_IO_BUFFER_KEY);
}

void MostrarSubmenuAyuda (fsm_t* this) {
	tipo_arkanoPi *p_arkanoPi;
	p_arkanoPi = (tipo_arkanoPi*)(this->user_data);

	// Cancelamos el flag del submenu de ayuda
	piLock(SYSTEM_FLAGS_KEY);
	flags[p_arkanoPi->partida] &= ~FLAG_MENU_AYUDA;
	piUnlock(SYSTEM_FLAGS_KEY);

	if (p_arkanoPi->partida == 0) {
		pseudoWiringPiEnableDisplay(0);
	}
	piLock(STD_IO_BUFFER_KEY);
	mostrarInstruccionesJuego(p_arkanoPi->partida, 1);
	fflush(stdout);
	piUnlock(STD_IO_BUFFER_KEY);
}

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------

void tmr_actualizacion_juego_isr(union sigval value) {
	// A completar por el alumno
	// Hecho
	piLock(SYSTEM_FLAGS_KEY);
	for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
		if (activarTimer[partida]) {
			activarTimer[partida] = 0;
			flags[partida] |= FLAG_TIMER_JUEGO;
		}
	}
	piUnlock(SYSTEM_FLAGS_KEY);
}

