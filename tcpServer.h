#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "pseudoWiringPi.h"
#include "ledDisplay.h"
#include "teclado_TL04.h"
#include "ledDisplay.h"

#define MAX_CARACTERES 500
#define PUERTO 1607
#define MAX_PERIFERICOS_CONECTADOS 2
#define TIMEOUT_COMPRUEBA_PERIFERICO 10000

typedef struct {
	struct sockaddr_in direccion_periferico; // Estructura que almacena la dirección del periférico que usará el socket 
    int conexion_fd; // Almacena el descriptor de archivo asociado a la conexion con el periférico
	socklen_t long_periferico; // Longitud de la dirección del periférico
	char supervisado; // Almacena una 's' si se están esperando mensajes del periférico u otro carácter si no se están esperando
	int partida; // Almacena la partida que controla el periférico
	int mensajeSinProcesar; // Entero que indica si se tiene un mensaje que se deba procesar
	char mensaje[MAX_CARACTERES]; // Mensaje recibido del periférico
	int activo; // Entero que nos informa si el periférico no ha enviado nada en un tiempo de 10 segundos
} TipoPeriferico;

typedef struct {
	struct sockaddr_in direccion_servidor; // Estructura que almacena la dirección del servidor que usará el socket
	int puerto; // Puerto de acceso al socket
    int socket_fd; // Almacena el descriptor de archivo asociado al socket del servidor
	char mensaje_error[MAX_CARACTERES]; // En caso de error, se almacena aquí el mensaje de error
    int flags; // Variable que almacena los flags para conocer el estado del socket
	int perifericos_conectados; // Número de periféricos conectados simultáneamente
	TipoPeriferico periferico[MAX_PERIFERICOS_CONECTADOS]; // Array que almacena las estructuras de los periféricos
	tmr_t* timer_comprueba_conexiones; // Comprueba que un periférico sigue conectado cada cierto tiempo
	int servidorHabilitado; // Habilita el servidor o lo deshabilita
	pthread_t thread_acepta_perifericos; // Thread que acepta los perifericos que se pueden conectar
	pthread_t thread_supervisa_perifericos[MAX_PERIFERICOS_CONECTADOS]; // Thread que obtiene los mensajes que se reciban del servidor
	char * str_consola[MAX_PERIFERICOS_CONECTADOS + 1]; // Strings que almacenan el último texto mostrado por consola en cada partida
	int aceptandoPerifericos; // Entero que permite comprobar si el thread que acepta periféricos se ha cerrado
} TipoServidor;

#define FLAG_TCP_ERROR		0x01
#define FLAG_TCP_MENSAJE	0x02

extern TipoServidor servidor;
extern tipo_pantalla pantallas_remotas[MAX_PERIFERICOS_CONECTADOS];

/* Threads TCP */
PI_THREAD (thread_conexion);
PI_THREAD (thread_obtener_mensajes);
PI_THREAD(thread_aceptar_periferico);

/* Funciones internas TCP */
void error(char * mensajeError);
int escucha();
void desconectarPeriferico(int idPeriferico);
void inicializaSocketTCP();
void enviarTexto(char * str, int partida);
void timer_comprueba_perifericos_isr(union sigval value);
void iniciarServidor();

/* Funciones usadas externamente TCP */
void enviarPantalla(int partida);
void enviarConsola(int partida, const char *format, ...);
void cerrarConexion();
int compruebaServidorHabilitado();
void habilitarServidor();

/* Otras funciones externas necesarias */
void explora_teclado(int teclaPulsada, int partida);

#endif /* TCPSERVER_H_ */
