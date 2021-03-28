#include "tcpServer.h"
//#define DEBUG // Da más información sobre los resultados de algunas funciones de TCP
//#define MOSTRAR_MENSAJES

TipoServidor servidor = {
	.puerto = PUERTO,
    .socket_fd = 0,
    .flags = 0,
	.perifericos_conectados = 0
};

/**
 * Establece un mensaje de error y cierra las conexiones por completo.
 */
void error(char * mensajeError) {
	bzero(servidor.mensaje_error, MAX_CARACTERES);
	strncpy(servidor.mensaje_error, mensajeError, strlen(mensajeError));
	servidor.flags |= FLAG_TCP_ERROR;
	// Se cierran todos los sockets abiertos
	for(int i = 0; i < MAX_PERIFERICOS_CONECTADOS; i++)
		if (servidor.periferico[i].conexion_fd != -1)
			close(servidor.periferico[i].conexion_fd);
	close(servidor.socket_fd);
}

/**
 * Pone al servidor en modo escucha.
 */
int escucha() {
    int err = 0;

	// Se pone el socket en modo escucha de conexiones entrantes
	err = listen(servidor.socket_fd, MAX_PERIFERICOS_CONECTADOS);
	if (err == -1) {
		#ifdef DEBUG
		perror("listen");
		#endif

		return err;
	}
	return 0;
}
/**
 * Thread que está pendiente de aceptar y almacenar hasta MAX_PERIFERICOS_CONECTADOS perifericos.
 * Si hay más perifericos, los acepta y los almacena donde estaban los anteriores perifericos
 * (se supone que esos perifericos se han desconectado).
 */
PI_THREAD(thread_aceptar_periferico) {
	int err = 0;
	TipoPeriferico * p_periferico;
	char buffer_error[MAX_CARACTERES];

	for (int i = 0; i < MAX_PERIFERICOS_CONECTADOS; i++) {
		if (servidor.periferico[i].conexion_fd == -1) {
			p_periferico = &(servidor.periferico[i]);

			p_periferico->long_periferico = sizeof(p_periferico->direccion_periferico);

			err = (p_periferico->conexion_fd = accept(servidor.socket_fd, (struct sockaddr*)&(p_periferico->direccion_periferico), &(p_periferico->long_periferico)));
			if (err == -1) {
				#ifdef DEBUG
				perror("accept");
				#endif
				sprintf(buffer_error, "Hubo un fallo al aceptar al periferico con id %d.\n", i);
				error(buffer_error);
			} else {
				#ifdef MOSTRAR_MENSAJES
				piLock(STD_IO_BUFFER_KEY);
				printf("El periferico con id %d se ha conectado.\n", i);
				piUnlock(STD_IO_BUFFER_KEY);
				#endif
				servidor.perifericos_conectados++;
				tmr_startms((tmr_t*) (servidor.timer_pantalla), TIMEOUT_ENVIO_PANTALLA);
			}
		}
	}
	return NULL;
}

/**
 * Envía la pantalla del juego a los perifericos a partir de la string str_pantalla.
 */
void enviarPantalla(char * str_pantalla) {
	if (servidor.perifericos_conectados) {
		for (int i = 0; i < MAX_PERIFERICOS_CONECTADOS; i++) {
			if (servidor.periferico[i].conexion_fd != -1) {
				if (send(servidor.periferico[i].conexion_fd, str_pantalla, strlen(str_pantalla), MSG_NOSIGNAL) < 0) {
					#ifdef MOSTRAR_MENSAJES
					piLock(STD_IO_BUFFER_KEY);
					#ifdef DEBUG
					perror("send");
					#endif
					printf("El periférico con id %d se ha desconectado.\nEscuchando de nuevo conexión con periférico en el puerto %d.\n", i, servidor.puerto);
					piUnlock(STD_IO_BUFFER_KEY);
					#endif
					servidor.perifericos_conectados--;
					close(servidor.periferico[i].conexion_fd);
					servidor.periferico[i].conexion_fd = -1;
					servidor.periferico[i].supervisado = 'n';
					// Se comienza a escuchar a perifericos externos
					if (piThreadCreate(thread_aceptar_periferico) != 0) {
						error("No se pudo crear el thread de thread_aceptar_periferico.\n");
					}
				}
			}
		}
	}
}

void timer_envio_pantalla_isr(union sigval value) {
	char str_pantalla[NUM_FILAS_DISPLAY * NUM_COLUMNAS_DISPLAY + 2];

	// Se transmite la pantalla
	for (int i = 0; i < NUM_FILAS_DISPLAY; i++) {
		for (int j = 0; j < NUM_COLUMNAS_DISPLAY; j++) {
			switch(led_display.pantalla.matriz[i][j]) {
				case 1:
					str_pantalla[i*NUM_COLUMNAS_DISPLAY + j] = '1';
					break;
				case 8:
					str_pantalla[i*NUM_COLUMNAS_DISPLAY + j] = '8';
					break;
				default:
					str_pantalla[i*NUM_COLUMNAS_DISPLAY + j] = '0';
			}
		}
	}

	str_pantalla[56] = '\n';
	str_pantalla[57] = '\0';
	enviarPantalla(str_pantalla);

	tmr_startms((tmr_t*) (servidor.timer_pantalla), TIMEOUT_ENVIO_PANTALLA);
}

/**
 * Thread para leer los mensajes recibidos de los perifericos.
 * Solo los lee si hay periféricos conectados y el socket que va a leer está inicializado.
 */
PI_THREAD(thread_obtener_mensajes) {
	int idPeriferico = 0;
	for (int i = 0; i < MAX_PERIFERICOS_CONECTADOS; i++) {
		if (servidor.periferico[i].supervisado == 'n') {
			servidor.periferico[i].supervisado = 'y';
			idPeriferico = i;
			break;
		}
	}
	while (1) {
		if (servidor.flags & FLAG_TCP_ERROR)
			break;
		if (servidor.perifericos_conectados && !(servidor.flags & FLAG_TCP_MENSAJE)) {
			if (servidor.periferico[idPeriferico].conexion_fd != -1) {
				bzero(servidor.mensaje, sizeof(servidor.mensaje)); 
				if(read(servidor.periferico[idPeriferico].conexion_fd, servidor.mensaje, sizeof(servidor.mensaje)) <= 0) {
					#ifdef MOSTRAR_MENSAJES
					piLock(STD_IO_BUFFER_KEY);
					#ifdef DEBUG
					perror("read");
					#endif
					printf("El periférico con id %d se ha desconectado.\nEscuchando de nuevo conexión con periférico en el puerto %d.\n", idPeriferico, servidor.puerto);
					piUnlock(STD_IO_BUFFER_KEY);
					#endif
					servidor.perifericos_conectados--;
					close(servidor.periferico[idPeriferico].conexion_fd);
					servidor.periferico[idPeriferico].conexion_fd = -1;
					servidor.periferico[idPeriferico].supervisado = 'n';
					// Se comienza a escuchar a perifericos externos
					if (piThreadCreate(thread_aceptar_periferico) != 0) {
						error("No se pudo crear el thread de thread_aceptar_periferico.\n");
					}
				} else {
					servidor.flags |= FLAG_TCP_MENSAJE;
				}
			}
		}
	}
	return NULL;
}

/**
 * Inicia el socket del servidor en el puerto especificado en sistema.puerto
 * y comienza a aceptar perifericos.
 */
void iniciarServidor() {
    int err = 0;
	char buffer_error[MAX_CARACTERES];
	servidor.flags = 0;
	servidor.socket_fd = 0;

	// Se crea el socket
	err = (servidor.socket_fd = socket(AF_INET, SOCK_STREAM, 0));
	if (err == -1) {
		#ifdef DEBUG
		perror("socket");
		#endif
		error("Hubo un fallo al crear el socket.\n");
		return;
	}
	// Permitimos que el socket se libere para reutilizarlo al finalizar el programa
	err = 1;
	setsockopt(servidor.socket_fd, SOL_SOCKET, SO_REUSEADDR, &err, sizeof(int));
	err = 0;

	// Se establece la dirección IP y el puerto del servidor
	servidor.direccion_servidor.sin_family = AF_INET; // IPv4
	servidor.direccion_servidor.sin_addr.s_addr = htonl(INADDR_ANY);
	servidor.direccion_servidor.sin_port = htons(servidor.puerto);

	// Se asigna dicha dirección y puerto al socket
	err = bind(servidor.socket_fd,
	           (struct sockaddr*)&(servidor.direccion_servidor),
	           sizeof(servidor.direccion_servidor));
	if (err == -1) {
		#ifdef DEBUG
		perror("bind");
		#endif
		error("Hubo un fallo al asociar el socket a la dirección y puerto establecidos.\n");
		return ;
	}

	// Se pone el socket en modo escucha
	err = escucha();
	if (err) {
		bzero(buffer_error, MAX_CARACTERES);
		sprintf(buffer_error, "Hubo un fallo al poner en escucha el socket en el puerto %d.\n", servidor.puerto);
		error(buffer_error);
		return;
	}

	// Se crea el temporizador de envío de pantalla
	servidor.timer_pantalla = tmr_new(timer_envio_pantalla_isr);

	// Se inicializan los atributos de los periféricos
	for (int i = 0; i < MAX_PERIFERICOS_CONECTADOS; i++) {
		// Se ponen como no supervisados los perifericos
		servidor.periferico[i].supervisado = 'n';
		// Se marca su descriptor de archivo como no usado
		servidor.periferico[i].conexion_fd = -1;
	}

	// Se comienza a escuchar a perifericos externos
	err = piThreadCreate(thread_aceptar_periferico);
	if (err != 0) {
		error("No se pudo crear el thread de thread_aceptar_periferico.\n");
		return;
	}
	// Se comienza a obtener sus mensajes
	for (int i = 0; i < MAX_PERIFERICOS_CONECTADOS; i++) {
		err = piThreadCreate(thread_obtener_mensajes);
		if (err != 0) {
			error("No se pudo crear el thread de thread_obtener_mensajes.\n");
			return;
		}
	}
}

/**
 * Thread que inicia y gestiona la conexión TCP de forma general.
 */
PI_THREAD (thread_conexion) {
	iniciarServidor();

	while(1) {
		if (servidor.perifericos_conectados) {
			if (servidor.flags & FLAG_TCP_ERROR) {
				servidor.flags &= ~FLAG_TCP_ERROR;
				#ifdef MOSTRAR_MENSAJES
				piLock(STD_IO_BUFFER_KEY);
				printf("Error en TCP: %s", servidor.mensaje_error);
				piUnlock(STD_IO_BUFFER_KEY);
				#endif
				break;
			} else if (servidor.flags & FLAG_TCP_MENSAJE)  {
				servidor.flags &= ~FLAG_TCP_MENSAJE;
				// Llegará XY siendo X la fila e Y la columna de la tecla pulsada
				for (int i = 0; i < 2; i++) {
					switch(*(servidor.mensaje + i)) {
						case '0':
							if (i == 0)
								teclado.teclaPulsada.row = FILA_1;
							else
								teclado.teclaPulsada.col = COLUMNA_1;
							break;
						case '1':
							if (i == 0)
								teclado.teclaPulsada.row = FILA_2;
							else
								teclado.teclaPulsada.col = COLUMNA_2;
							break;
						case '2':
							if (i == 0)
								teclado.teclaPulsada.row = FILA_3;
							else
								teclado.teclaPulsada.col = COLUMNA_3;
							break;
						case '3':
							if (i == 0)
								teclado.teclaPulsada.row = FILA_4;
							else
								teclado.teclaPulsada.col = COLUMNA_4;
							break;
					}
				}
				piLock(SYSTEM_FLAGS_KEY);
				teclado.flags |= FLAG_TECLA_PULSADA;
				piUnlock(SYSTEM_FLAGS_KEY);
			}
		}
	}
	return NULL;
}
