#ifndef _ARKANOPI_H_
#define _ARKANOPI_H_

#include "systemLib.h"
#include "kbhit.h" // para poder detectar teclas pulsadas sin bloqueo y leer las teclas pulsadas
#include "fsm.h"
#include "tmr.h"
#include "teclado_TL04.h"
#include "arkanoPiLib.h"
#include "ledDisplay.h"
#include "tcpServer.h" // para conectar hardware externo simulado en móviles Android

#define MAX_PERIFERICOS_CONECTADOS 2

typedef struct {
	tipo_arkanoPi arkanoPi[MAX_PERIFERICOS_CONECTADOS + 1];
} TipoSistema;

//------------------------------------------------------
// FUNCIONES DE TRANSICION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
int CompruebaBotonPulsado (fsm_t* this);
int CompruebaMovimientoIzquierda(fsm_t* this);
int CompruebaMovimientoDerecha(fsm_t* this);
int CompruebaTimeoutActualizacionJuego (fsm_t* this);
int CompruebaFinalJuego(fsm_t* this);

//------------------------------------------------------
// FUNCIONES DE ACCION DE LA MAQUINA DE ESTADOS
//------------------------------------------------------
void InicializaJuego (fsm_t* this);
void MuevePalaIzquierda (fsm_t* this);
void MuevePalaDerecha (fsm_t* this);
void ActualizarJuego (fsm_t* this);
void FinalJuego (fsm_t* this);
void ReseteaJuego (fsm_t* this);

//------------------------------------------------------
// FUNCIONES DE CONFIGURACION/INICIALIZACION
//------------------------------------------------------
int ConfiguraInicializaSistema (TipoSistema *p_sistema);

//------------------------------------------------------
// SUBRUTINAS DE ATENCION A LAS INTERRUPCIONES
//------------------------------------------------------
void tmr_actualizacion_juego_isr(union sigval value);

//------------------------------------------------------
// FUNCIONES LIGADAS A THREADS ADICIONALES
//------------------------------------------------------
PI_THREAD(thread_explora_teclado_PC);
void explora_teclado(int teclaPulsada, int partida);

#endif /* ARKANOPI_H_ */
