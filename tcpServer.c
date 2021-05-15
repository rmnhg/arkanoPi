#include "tcpServer.h"
//#define DEBUG // Da más información sobre los resultados de algunas funciones de TCP
//#define MOSTRAR_MENSAJES

TipoServidor servidor = {
	.puerto = PUERTO,
    .socket_fd = 0,
    .flags = 0,
	.perifericos_conectados = 0,
	.servidorHabilitado = 1,
	.partidaMensajeActual = -1
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
				// A cada periférico se le asigna su propia partida en un principio
				servidor.periferico[i].partida = i + 1;
				tmr_startms((tmr_t*) (servidor.timer_pantalla), TIMEOUT_ENVIO_PANTALLA);
			}
		}
	}
	return NULL;
}

/**
 * Envía el texto de la string str a un periférico en concreto.
 */
void enviarTexto(char * str, int periferico) {
	if (servidor.perifericos_conectados && servidor.servidorHabilitado) {
		if (servidor.periferico[periferico].conexion_fd != -1) {
			if (send(servidor.periferico[periferico].conexion_fd, str, strlen(str), MSG_NOSIGNAL) < 0) {
				#ifdef MOSTRAR_MENSAJES
				piLock(STD_IO_BUFFER_KEY);
				#ifdef DEBUG
				perror("send");
				#endif
				printf("El periférico con id %d se ha desconectado.\nEscuchando de nuevo conexión con periférico en el puerto %d.\n", periferico, servidor.puerto);
				piUnlock(STD_IO_BUFFER_KEY);
				#endif
				servidor.perifericos_conectados--;
				close(servidor.periferico[periferico].conexion_fd);
				servidor.periferico[periferico].conexion_fd = -1;
				servidor.periferico[periferico].supervisado = 'n';
				// Se comienza a escuchar a perifericos externos
				if (pthread_create(&(servidor.thread_acepta_perifericos), NULL, thread_aceptar_periferico, NULL) != 0) {
					error("No se pudo crear el thread de thread_aceptar_periferico.\n");
				}
			}
		}
	}
}

/**
 * Envía el texto de la consola del juego a los perifericos a partir de la string y parámetros pasados.
 */
void enviarConsola(int partida, const char *format, ...) {
	char * str_consola = (char *) malloc(MAX_CARACTERES * sizeof(char));
	// Componemos la string final y la mostramos en la consola (stdout) y la enviamos a los periféricos conectados.
	va_list arg;
	va_start(arg, format);
	if (partida == 0) {
		vfprintf(stdout, format, arg);
	}
	vsprintf(str_consola, format, arg);
	va_end(arg);
	// Sustituimos los saltos de línea por '#' para evitar mandar más de un mensaje por string.
	// No llegamos al salto del final para que el búffer sepa que ahí acaba el mensaje que se debe enviar.
	for (int i = 0; i < strlen(str_consola) - 2; i++) {
		if (*(str_consola + i) == '\n')
			*(str_consola + i) = '#';
	}
	enviarTexto(str_consola, partida);
}

void timer_envio_pantalla_isr(union sigval value) {
	char str_pantalla[NUM_FILAS_DISPLAY * NUM_COLUMNAS_DISPLAY + 2];

	if (servidor.servidorHabilitado) {
		for (int periferico = 0; periferico < MAX_PERIFERICOS_CONECTADOS; periferico++) {
			// Se transmite la pantalla
			if (servidor.periferico[periferico].partida == 0) {
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
			} else {
				for (int i = 0; i < NUM_FILAS_DISPLAY; i++) {
					for (int j = 0; j < NUM_COLUMNAS_DISPLAY; j++) {
						switch(pantallas_remotas[servidor.periferico[periferico].partida - 1].matriz[i][j]) {
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
			}

			str_pantalla[56] = '\n';
			str_pantalla[57] = '\0';
			enviarTexto(str_pantalla, periferico);
		}

		tmr_startms((tmr_t*) (servidor.timer_pantalla), TIMEOUT_ENVIO_PANTALLA);
	}
}

void desconectarPeriferico(int idPeriferico) {
	close(servidor.periferico[idPeriferico].conexion_fd);
	servidor.periferico[idPeriferico].conexion_fd = -1;
	servidor.periferico[idPeriferico].supervisado = 'n';
	servidor.perifericos_conectados--;
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
	while (servidor.servidorHabilitado) {
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
					if (pthread_create(&(servidor.thread_acepta_perifericos), NULL, thread_aceptar_periferico, NULL) != 0) {
						error("No se pudo crear el thread de thread_aceptar_periferico.\n");
					}
				} else {
					if (strstr(servidor.mensaje, "$Desconectar_client") != NULL) {
						desconectarPeriferico(idPeriferico);
					} else if (strstr(servidor.mensaje, "$Cambiar_a_partida_0") != NULL) {
						servidor.periferico[idPeriferico].partida = 0;
					} else if (strstr(servidor.mensaje, "$Cambiar_a_partida_1") != NULL) {
						servidor.periferico[idPeriferico].partida = 1;
					} else if (strstr(servidor.mensaje, "$Cambiar_a_partida_2") != NULL) {
						servidor.periferico[idPeriferico].partida = 2;
					} else {
						servidor.flags |= FLAG_TCP_MENSAJE;
						servidor.partidaMensajeActual = servidor.periferico[idPeriferico].partida;
					}
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
	err = pthread_create(&(servidor.thread_acepta_perifericos), NULL, thread_aceptar_periferico, NULL);
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
	int partidaActual = -1, posicionTecla = -1, fila = -1, columna = -1;
	servidor.servidorHabilitado = 1;
	iniciarServidor();

	while(servidor.servidorHabilitado) {
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
				partidaActual = servidor.partidaMensajeActual;
				posicionTecla = atoi((const char *)servidor.mensaje);
				fila = posicionTecla / 10;
				columna = posicionTecla % 10;
				// Llegará XY siendo X la fila e Y la columna de la tecla pulsada
				if (partidaActual != 0) {
					explora_teclado(tecladoTL04[fila][columna], partidaActual);
				} else {
					teclado.teclaPulsada.row = fila;
					teclado.teclaPulsada.col = columna;
					piLock(SYSTEM_FLAGS_KEY);
					teclado.flags |= FLAG_TECLA_PULSADA;
					piUnlock(SYSTEM_FLAGS_KEY);
				}
				servidor.flags &= ~FLAG_TCP_MENSAJE;
			}
		}
	}
	return NULL;
}

/**
 * Función que desconecta a todos los perféricos del servidor
 */
void cerrarConexion() {
	// Para desconectar a todos los periféricos del servidor.
	// Se cierran las conexiones
	servidor.servidorHabilitado = 0;
	delay(TIMEOUT_ENVIO_PANTALLA);
	for (int i = 0; i < MAX_PERIFERICOS_CONECTADOS; i++) {
		enviarTexto("$Servidor_cerrado", i);
	}
	pthread_cancel(servidor.thread_acepta_perifericos);
	//tmr_destroy((tmr_t*) (servidor.timer_pantalla));
	// Se eliminan las referencias a los clientes
	for (int i = 0; i < MAX_PERIFERICOS_CONECTADOS; i++) {
		if (servidor.periferico[i].conexion_fd != -1) {
			close(servidor.periferico[i].conexion_fd);
			servidor.periferico[i].conexion_fd = -1;
			servidor.periferico[i].supervisado = 'n';
		}
	}
	servidor.perifericos_conectados = 0;
	close(servidor.socket_fd);
}

/**
 * Función que devuelve un 1 si el servidor está habilitado o un 0 si no lo está.
 */
int compruebaServidorHabilitado() {
	return servidor.servidorHabilitado;
}

/**
 * Función que devuelve un 1 si el servidor está habilitado o un 0 si no lo está.
 */
void habilitarServidor() {
	servidor.servidorHabilitado = 1;
}
