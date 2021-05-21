
#ifndef _ARKANOPILIB_H_
#define _ARKANOPILIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ledDisplay.h"
#include "tmr.h"

enum t_direccion {
	ARRIBA_IZQUIERDA,
	ARRIBA,
	ARRIBA_DERECHA,
	ABAJO_DERECHA,
	ABAJO,
	ABAJO_IZQUIERDA,
	IZQUIERDA, // NO PERMITIDA
	DERECHA, // NO PERMITIDA
};

// CONSTANTES DEL JUEGO
#define NUM_COLUMNAS_PALA 	3
#define NUM_FILAS_PALA 		1
#define MAX_NUM_TRAYECTORIAS 8
#define MAX_PELOTAS			9 // Máximo número de pelotas que puede haber en el juego

typedef struct {
  int ancho;
  int alto;
  int x;
  int y;
} tipo_pala;

typedef struct {
  int xv;
  int yv;
} tipo_trayectoria;

typedef struct {
  tipo_trayectoria posibles_trayectorias[MAX_NUM_TRAYECTORIAS];
  int num_posibles_trayectorias;
  tipo_trayectoria trayectoria;
  int x;
  int y;
} tipo_pelota;

typedef struct {
	tipo_pantalla *p_pantalla; // Esta es nuestra pantalla de juego (matriz 10x7 de labo)
	tipo_pantalla ladrillos;
	tipo_pala pala;
	tipo_pelota pelota[MAX_PELOTAS];
	int numeroPelotas; // Entero que almacena el número de pelotas que se muestran en esta partida
	int paredesHabilitadas; // Entero que almacena si hay rebotes en las paredes verticales o no en esta partida
	int primerAccesoSubmenu; // Entero que almacena si algún submenú se muestra por primera vez o no en en esta partida
	int partida; // Número de la partida que se juega con esta estructura arkanoPi
	tmr_t* tmr_actualizacion_juego;
} tipo_arkanoPi;

//------------------------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET  DE LOS OBJETOS ESPECIFICOS
//------------------------------------------------------------------------
void InicializaLadrillos(tipo_pantalla *p_ladrillos);
void InicializaPelota(tipo_pelota *p_pelota);
void InicializaPala(tipo_pala *p_pala);
void InicializaPosiblesTrayectorias(tipo_pelota *p_pelota);
void InicializaArkanoPi(tipo_arkanoPi *p_arkanoPi);
void ResetArkanoPi(tipo_arkanoPi *p_arkanoPi);
void ReseteaMatriz(tipo_pantalla *p_pantalla);

//------------------------------------------------------
// PROCEDIMIENTOS PARA LA GESTION DEL JUEGO
//------------------------------------------------------
void CambiarDireccionPelota(tipo_pelota *p_pelota, enum t_direccion direccion);
void ActualizaPosicionPala(tipo_pala *p_pala, enum t_direccion direccion);
void ActualizaPosicionPelota (tipo_pelota *p_pelota, int paredesHabilitadas);
int CompruebaReboteLadrillo (tipo_arkanoPi *p_arkanoPi, int pelota);
int CompruebaReboteParedesVerticales (tipo_arkanoPi arkanoPi, int pelota);
int CompruebaReboteTecho (tipo_arkanoPi arkanoPi, int pelota);
int CompruebaRebotePala (tipo_arkanoPi arkanoPi, int pelota);
int CompruebaFallo (tipo_arkanoPi arkanoPi, int pelota);
int CalculaLadrillosRestantes(tipo_pantalla *p_ladrillos);

//------------------------------------------------------
// PROCEDIMIENTOS PARA LA VISUALIZACION DEL JUEGO
//------------------------------------------------------
void PintaMensajeInicialPantalla (tipo_pantalla *p_pantalla, tipo_pantalla *p_pantalla_inicial);
void PintaPantallaPorTerminal (tipo_pantalla *p_pantalla);
void PintaLadrillos(tipo_pantalla *p_ladrillos, tipo_pantalla *p_pantalla);
void PintaPala(tipo_pala *p_pala, tipo_pantalla *p_pantalla);
void PintaPelota(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla);
void ActualizaPantalla(tipo_arkanoPi* p_arkanoPi);

//------------------------------------------------------
// FUNCIONES DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
int CompruebaBotonPulsado (fsm_t* this);
int CompruebaPausaPulsada (fsm_t* this);
int CompruebaMovimientoArriba (fsm_t* this);
int CompruebaMovimientoAbajo (fsm_t* this);
int CompruebaMovimientoIzquierda (fsm_t* this);
int CompruebaMovimientoDerecha (fsm_t* this);
int CompruebaTimeoutActualizacionJuego (fsm_t* this);
int CompruebaFinalJuego (fsm_t* this);
int CompruebaNumeroPelotas(fsm_t* this);
int CompruebaParedes(fsm_t* this);
int CompruebaTCP(fsm_t* this);
int CompruebaAyuda(fsm_t* this);
int CompruebaNumerosPulsados(fsm_t* this);
int CompruebaMenosPulsado(fsm_t* this);
int CompruebaMasPulsado(fsm_t* this);
int CompruebaMenuPulsado(fsm_t* this);

//------------------------------------------------------
// FUNCIONES DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
void InicializaJuego (fsm_t* this);
void MuevePalaIzquierda (fsm_t* this);
void MuevePalaDerecha (fsm_t* this);
void ActualizarJuego (fsm_t* this);
void FinalJuego (fsm_t* this);
void ReseteaJuego (fsm_t* this);
void PausarJuego (fsm_t* this);
void ActivarMenu (fsm_t* this);
void MostrarSubmenuPelotas (fsm_t* this);
void MostrarSubmenuParedes (fsm_t* this);
void MostrarSubmenuTCP (fsm_t* this);
void MostrarSubmenuAyuda (fsm_t* this);

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------
void tmr_actualizacion_juego_isr(union sigval value);

//------------------------------------------------------
// SUBRUTINAS DE CONTROL Y COMUNICACIÓN DEL SERVIDOR TCP
//------------------------------------------------------
void enviarPantalla(int partida);
void enviarConsola(int partida, const char *format, ...);
PI_THREAD (thread_conexion);
int compruebaServidorHabilitado();
void habilitarServidor();
void cerrarConexion();

//-------------------------------------------------------------------
// SUBRUTINAS COMUNES PARA MOSTRAR EL MENÚ Y LOS CONTROLES DEL JUEGO
//-------------------------------------------------------------------
inline static void mostrarInstruccionesJuego(int partida, int desdeMenu) {
	if (desdeMenu) {
		enviarConsola(partida, "\nInstrucciones de uso:\n"
							   "\tLas teclas A o 4 y D o 6 mueven la pala hacia la izquierda y hacia la derecha respectivamente.\n"
							   "\tLa tecla C actualiza la posición de la pelota en la pantalla manualmente.\n"
							   "\tLa tecla B pausa el juego.\n"
							   "\tLa tecla 5 vuelve al menú del juego.\n"
							   "\tLa tecla F cierra el juego.\n");
	} else {
		enviarConsola(partida, "\nInstrucciones de uso:\n"
							   "\tCualquier tecla inicia el juego.\n"
							   "\tLas teclas A o 4 y D o 6 mueven la pala hacia la izquierda y hacia la derecha respectivamente.\n"
							   "\tLa tecla C actualiza la posición de la pelota en la pantalla manualmente.\n"
							   "\tLa tecla B pausa el juego.\n"
							   "\tLa tecla 5 abre el menú del juego.\n"
							   "\tLa tecla F cierra el juego.\n");
	}
	fflush(stdout);
}

inline static void MostrarMenu(int partida) {
	enviarConsola(partida, "\n¡Bienvenido a arkanoPi!\n"
						   "\tPulsa 1 para cambiar el números de pelotas en juego (1 - %d).\n"
						   "\tPulsa 2 para alternar las paredes del juego.\n"
						   "\tPulsa 3 para alternar el soporte de periféricos externos.\n"
						   "\tPulsa 4 para ver la ayuda.\n"
						   "\tPulsa A o D para comenzar la partida.\n", MAX_PELOTAS);
	fflush(stdout);
}

// Externalizamos el timer de actualización de partidas para reiniciarlo desde su interrupción
tmr_t* timer_juego;

#endif /* _ARKANOPILIB_H_ */
