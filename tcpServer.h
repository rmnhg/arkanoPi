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
#define TIMEOUT_ENVIO_PANTALLA DEBOUNCE_TIME
#define TIMEOUT_COMPRUEBA_PERIFERICO 10000

typedef struct {
	struct sockaddr_in direccion_periferico; // Estructura que almacena la dirección del servidor que usará el socket 
    int conexion_fd; // Almacena el descriptor de archivo asociado a la conexion con el periférico
	socklen_t long_periferico; // Longitud de la dirección del periférico
	char supervisado; // Almacena una 's' si se están esperando mensajes del periférico u otro carácter si no se están esperando
	int partida; //Almacena la partida que controla el periférico
	int activo;
} TipoPeriferico;

typedef struct {
	struct sockaddr_in direccion_servidor; // Estructura que almacena la dirección del servidor que usará el socket
	int puerto; // Puerto de acceso al socket
    int socket_fd; // Almacena el descriptor de archivo asociado al socket del servidor
	char mensaje_error[MAX_CARACTERES]; // En caso de error, se almacena aquí el mensaje de error
	char mensaje[MAX_CARACTERES]; // Mensaje recibido del periférico
    int flags; // Variable que almacena los flags para conocer el estado del socket
	int perifericos_conectados; // Número de periféricos conectados simultáneamente
	TipoPeriferico periferico[MAX_PERIFERICOS_CONECTADOS]; // Array que almacena las estructuras de los periféricos
	tmr_t* timer_pantalla; // Envía una pantalla nueva cada tiempo de refresco de pantalla
	tmr_t* timer_comprueba_conexiones; // Comprueba que un periférico sigue conectado cada cierti tiempo
	int servidorHabilitado; // Habilita el servidor o lo deshabilita
	pthread_t thread_acepta_perifericos; // Thread que acepta los perifericos que se pueden conectar
	int partidaMensajeActual; // Entero que identifica la partida a la que se debe enviar el mensaje actualmente guardado
	char * str_consola[MAX_PERIFERICOS_CONECTADOS + 1]; // Strings que almacenan el último texto mostrado por consola en cada partida
	int aceptandoPerifericos; // Entero que permite comprobar si el thread que acepta periféricos se ha cerrado
} TipoServidor;

#define FLAG_TCP_ERROR		0x01
#define FLAG_TCP_MENSAJE	0x02

extern TipoServidor servidor;
extern tipo_pantalla pantallas_remotas[MAX_PERIFERICOS_CONECTADOS];

PI_THREAD (thread_conexion);
void reiniciarServidor();
void enviarConsola(int partida, const char *format, ...);
void cerrarConexion();
int compruebaServidorHabilitado();
void habilitarServidor();
void enviarTexto(char * str, int partida);

void explora_teclado(int teclaPulsada, int partida);

#endif /* TCPSERVER_H_ */
