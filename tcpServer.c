#include "tcpServer.h"
//#define DEBUG // Da más información sobre los resultados de algunas funciones de TCP
//#define MOSTRAR_MENSAJES

TipoServidor servidor = {
	.puerto = PUERTO,
    .socket_fd = 0,
    .flags = 0,
	.perifericos_conectados = 0,
	.servidorHabilitado = 1
};

/**
 * Establece un mensaje de error y cierra las conexiones por completo.
 */
void error(char * mensajeError) {
	printf("%s", mensajeError);
	bzero(servidor.mensaje_error, MAX_CARACTERES);
	strncpy(servidor.mensaje_error, mensajeError, strlen(mensajeError));
	servidor.flags |= FLAG_TCP_ERROR;
	// Se detiene sel servidor
	cerrarConexion();
	// Se reanuda el servidor
	servidor.servidorHabilitado = 1;
	// Lanzamos un thread para gestionar las conexiones TCP de los periféricos externos
	if (piThreadCreate(thread_conexion) != 0) {
		printf("No se pudo crear el thread thread_conexion.\n");
		return;
	}
}

/**
 * Pone al servidor en modo escucha.
 */
int escucha() {
    int err = 0;

	// Se pone el socket en modo escucha de conexiones entrantes
	err = listen(servidor.socket_fd, 0);
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
	struct linger sl;

	// Para establecer que los sockets de los clientes se cierren instantáneamente usando la opción SO_LINGER
	sl.l_onoff = 1;
	sl.l_linger = 0;

	while (servidor.servidorHabilitado) {
		if (servidor.socket_fd == -1 && servidor.perifericos_conectados < MAX_PERIFERICOS_CONECTADOS) {
			inicializaSocketTCP();
			servidor.aceptandoPerifericos = 1;
		}
		for (int idPeriferico = 0; idPeriferico < MAX_PERIFERICOS_CONECTADOS; idPeriferico++) {
			if (servidor.socket_fd != -1 && servidor.periferico[idPeriferico].conexion_fd == -1) {
				p_periferico = &(servidor.periferico[idPeriferico]);

				p_periferico->long_periferico = sizeof(p_periferico->direccion_periferico);

				err = (p_periferico->conexion_fd = accept(servidor.socket_fd, (struct sockaddr*)&(p_periferico->direccion_periferico), &(p_periferico->long_periferico)));
				if (err == -1) {
					#ifdef DEBUG
					perror("accept");
					#endif
					perror("accept");
					sprintf(buffer_error, "Hubo un fallo al aceptar al periferico con id %d.\n", idPeriferico);
					//error(buffer_error);
				} else {
					#ifdef MOSTRAR_MENSAJES
					piLock(STD_IO_BUFFER_KEY);
					printf("El periferico con id %d se ha conectado.\n", i);
					piUnlock(STD_IO_BUFFER_KEY);
					#endif
					// Definimos que no se espere a que se transmitan datos al cerrar el socket
					setsockopt(servidor.periferico[idPeriferico].conexion_fd, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl));
					servidor.perifericos_conectados++;
					p_periferico->activo = 1;
					// A cada periférico se le asigna su propia partida en un principio
					p_periferico->partida = idPeriferico + 1;
					// Se comienza a obtener sus mensajes
					if (pthread_create(&(servidor.thread_supervisa_perifericos[idPeriferico]), NULL, thread_obtener_mensajes, NULL) != 0) {
						error("No se pudo crear el thread de thread_obtener_mensajes.\n");
						return NULL;
					}
					// Se envía la pantalla
					enviar_pantalla(p_periferico->partida);
					// Se envía la consola almacenada por si no la recibió anteriormente
					enviarTexto(servidor.str_consola[p_periferico->partida], idPeriferico);
					tmr_startms((tmr_t*) (servidor.timer_comprueba_conexiones), TIMEOUT_COMPRUEBA_PERIFERICO);
				}
			}
		}
	}
	// Se deja de aceptar periféricos
	close(servidor.socket_fd);
	servidor.socket_fd = -1;
	servidor.aceptandoPerifericos = 0;

	return NULL;
}

/**
 * Envía el texto de la string str al periférico pasado como parámetro.
 */
void enviarTexto(char * str, int idPeriferico) {
	if (strlen(str) < 2)
		return;

	// Sustituimos los saltos de línea por '#' para evitar mandar más de un mensaje por string.
	// No llegamos al salto del final para que el búffer sepa que ahí acaba el mensaje que se debe enviar.
	for (int i = 0; i < strlen(str) - 2; i++) {
		if (*(str + i) == '\n')
			*(str + i) = '#';
	}
	if (servidor.perifericos_conectados && (servidor.servidorHabilitado || (strstr(str, "$Servidor_cerrado") != NULL))) {
		if (servidor.periferico[idPeriferico].conexion_fd != -1) {
			if (send(servidor.periferico[idPeriferico].conexion_fd, str, strlen(str), MSG_NOSIGNAL) < 0) {
				#ifdef MOSTRAR_MENSAJES
				piLock(STD_IO_BUFFER_KEY);
				#ifdef DEBUG
				perror("send");
				#endif
				printf("El periférico con id %d se ha desconectado.\nEscuchando de nuevo conexión con periférico en el puerto %d.\n", periferico, servidor.puerto);
				piUnlock(STD_IO_BUFFER_KEY);
				#endif
				desconectarPeriferico(idPeriferico);
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
	vsprintf(str_consola, format, arg);
	if (partida == 0) {
		// Solo mostramos el texto de la consola en la consola local si la partida es la del host (la 0)
		fprintf(stdout, "%s", str_consola);
	}
	// Almacenamos la consola que se envía por si hubiera que enviarla de nuevo
	bzero(servidor.str_consola[partida], MAX_CARACTERES);
	sprintf(servidor.str_consola[partida], "%s", str_consola);
	va_end(arg);
	// Enviamos la consola al periférico que tenga como partida la partida pasada como parámetro
	for (int idPeriferico = 0; idPeriferico < MAX_PERIFERICOS_CONECTADOS; idPeriferico++) {
		if (servidor.periferico[idPeriferico].partida == partida) {
			enviarTexto(str_consola, idPeriferico);
		}
	}
}

/**
 * Función de interrupción del timer que recorre todas las pantallas y
 * las transforma en una string de dígitos consecutivos para su envío.
 */
void enviar_pantalla(int partida) {
	char str_pantalla[NUM_FILAS_DISPLAY * NUM_COLUMNAS_DISPLAY + 2];

	if (servidor.servidorHabilitado) {
		for (int idPeriferico = 0; idPeriferico < MAX_PERIFERICOS_CONECTADOS; idPeriferico++) {
			if (partida == servidor.periferico[idPeriferico].partida) {
				// Se transmite la pantalla
				for (int i = 0; i < NUM_FILAS_DISPLAY; i++) {
					for (int j = 0; j < NUM_COLUMNAS_DISPLAY; j++) {
						if (partida == 0) { // Pantalla del host con led_display
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
						} else { // Pantallas remotas
							switch(pantallas_remotas[partida - 1].matriz[i][j]) {
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
				enviarTexto(str_pantalla, idPeriferico);
			}
		}
	}
}

/**
 * Función que desconecta el periférico pasado como parámetro
 */
void desconectarPeriferico(int idPeriferico) {
	if (servidor.periferico[idPeriferico].conexion_fd != -1) {
		// Se cancelan los threads que tengan algo que ver con este socket
		servidor.aceptandoPerifericos = 0;
		pthread_cancel(servidor.thread_supervisa_perifericos[idPeriferico]);
		// Cerramos la conexión con el periférico
		shutdown(servidor.periferico[idPeriferico].conexion_fd, 2);
		close(servidor.periferico[idPeriferico].conexion_fd);
		// Se marca su descriptor de archivo como no usado
		servidor.periferico[idPeriferico].conexion_fd = -1;
		// Se pone como no supervisado el periferico
		servidor.periferico[idPeriferico].supervisado = 'n';
		// Se establece que no tiene ningún mensaje pendiente de procesar
		servidor.periferico[idPeriferico].mensajeSinProcesar = 0;
		// Ponemos el periferico como inactivo
		servidor.periferico[idPeriferico].activo = 0;
		// Se actualiza el número de perifericos_conectados
		servidor.perifericos_conectados--;
	}
}

/**
 * Función de interrupción del timer que comprueba si un periférico sigue ativo.
 */
void timer_comprueba_perifericos_isr(union sigval value) {
	for (int idPeriferico = 0; idPeriferico < MAX_PERIFERICOS_CONECTADOS; idPeriferico++) {
		if (servidor.periferico[idPeriferico].activo) {
			servidor.periferico[idPeriferico].activo = 0;
		} else {
			enviarTexto("$Desconectado_por_inactividad", idPeriferico);
			desconectarPeriferico(idPeriferico);
		}
	}
	tmr_startms((tmr_t*) (servidor.timer_comprueba_conexiones), TIMEOUT_COMPRUEBA_PERIFERICO);
}

/**
 * Thread para leer los mensajes recibidos de los perifericos.
 * Solo los lee si hay periféricos conectados y el socket que va a leer está inicializado.
 */
PI_THREAD(thread_obtener_mensajes) {
	int idPeriferico = 0;
	char partidaRecibida[2];
	for (int i = 0; i < MAX_PERIFERICOS_CONECTADOS; i++) {
		if (servidor.periferico[i].supervisado == 'n') {
			servidor.periferico[i].supervisado = 's';
			idPeriferico = i;
			break;
		}
	}
	while (servidor.servidorHabilitado) {
		if (servidor.flags & FLAG_TCP_ERROR)
			break;
		if (servidor.perifericos_conectados && !(servidor.flags & FLAG_TCP_MENSAJE)) {
			if (servidor.periferico[idPeriferico].conexion_fd != -1) {
				bzero(servidor.periferico[idPeriferico].mensaje, sizeof(servidor.periferico[idPeriferico].mensaje));
				if(read(servidor.periferico[idPeriferico].conexion_fd, servidor.periferico[idPeriferico].mensaje, sizeof(servidor.periferico[idPeriferico].mensaje)) <= 0) {
					#ifdef MOSTRAR_MENSAJES
					piLock(STD_IO_BUFFER_KEY);
					#ifdef DEBUG
					perror("read");
					#endif
					printf("El periférico con id %d se ha desconectado.\nEscuchando de nuevo conexión con periférico en el puerto %d.\n", idPeriferico, servidor.puerto);
					piUnlock(STD_IO_BUFFER_KEY);
					#endif
					desconectarPeriferico(idPeriferico);
					if (!servidor.aceptandoPerifericos) {
						// Se comienza a escuchar a perifericos externos
						if (pthread_create(&(servidor.thread_acepta_perifericos), NULL, thread_aceptar_periferico, NULL) != 0) {
							error("No se pudo crear el thread de thread_aceptar_periferico.\n");
						}
					}
				} else {
					servidor.periferico[idPeriferico].activo = 1;
					if (strstr(servidor.periferico[idPeriferico].mensaje, "$Desconectar_cliente") != NULL) {
						desconectarPeriferico(idPeriferico);
					} else if (strstr(servidor.periferico[idPeriferico].mensaje, "$Cambiar_a_partida_") != NULL) {
						// Se coge el última carácter como partida
						sprintf(partidaRecibida, "%c", *(servidor.periferico[idPeriferico].mensaje + 19));
						if (atoi(partidaRecibida) >= 0 && atoi(partidaRecibida) <= MAX_PERIFERICOS_CONECTADOS) {
							servidor.periferico[idPeriferico].partida = atoi(partidaRecibida);
							// Se envía la pantalla
							enviar_pantalla(servidor.periferico[idPeriferico].partida);
							// Se envía la consola almacenada en la nueva partida
							enviarTexto(servidor.str_consola[servidor.periferico[idPeriferico].partida], idPeriferico);
						}
					} else if (strstr(servidor.periferico[idPeriferico].mensaje, "$Sigo_conectado") == NULL) {
						servidor.flags |= FLAG_TCP_MENSAJE;
						servidor.periferico[idPeriferico].mensajeSinProcesar = 1;
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
	servidor.flags = 0;
	servidor.socket_fd = -1;

	// Se reserva memoria para las strings de la consola
	for (int partida = 0; partida < MAX_PERIFERICOS_CONECTADOS + 1; partida++) {
		servidor.str_consola[partida] = (char *) malloc(MAX_CARACTERES * sizeof(char));
	}

	// Se inicializa el socket TCP
 	inicializaSocketTCP();

	// Se crea el temporizador que comprueba la actividad de los periféricos
	servidor.timer_comprueba_conexiones = tmr_new(timer_comprueba_perifericos_isr);

	// Se inicializan los atributos de los periféricos
	for (int idPeriferico = 0; idPeriferico < MAX_PERIFERICOS_CONECTADOS; idPeriferico++) {
		// Se ponen como no supervisados los perifericos
		servidor.periferico[idPeriferico].supervisado = 'n';
		// Se establece que no tiene ningún mensaje pendiente de procesar
		servidor.periferico[idPeriferico].mensajeSinProcesar = 0;
		// Se marca su descriptor de archivo como no usado
		servidor.periferico[idPeriferico].conexion_fd = -1;
	}

	servidor.aceptandoPerifericos = 1;
	// Se comienza a escuchar a perifericos externos
	err = pthread_create(&(servidor.thread_acepta_perifericos), NULL, thread_aceptar_periferico, NULL);
	if (err != 0) {
		error("No se pudo crear el thread de thread_aceptar_periferico.\n");
		return;
	}
}

/**
 * Inicializa el socket del servidor en el puerto especificado en sistema.puerto
 * y comienza a aceptar perifericos.
 */
void inicializaSocketTCP() {
    int err = 0;
	char buffer_error[MAX_CARACTERES];

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
}

/**
 * Thread que inicia y gestiona la conexión TCP de forma general.
 */
PI_THREAD (thread_conexion) {
	int partidaActual = 0, posicionTecla = 0, fila = 0, columna = 0;
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
				for (int idPeriferico = 0; idPeriferico < MAX_PERIFERICOS_CONECTADOS; idPeriferico++) {
					if (servidor.periferico[idPeriferico].mensajeSinProcesar) {
						partidaActual = servidor.periferico[idPeriferico].partida;
						posicionTecla = atoi((const char *) servidor.periferico[idPeriferico].mensaje);
						fila = posicionTecla / 10;
						columna = posicionTecla % 10;
						// Llegará XY siendo X la fila e Y la columna de la tecla pulsada
						if (partidaActual >= 0 && partidaActual <= MAX_PERIFERICOS_CONECTADOS) {
							if (partidaActual != 0) {
								explora_teclado(tecladoTL04[fila][columna], partidaActual);
							} else {
								teclado.teclaPulsada.row = fila;
								teclado.teclaPulsada.col = columna;
								piLock(SYSTEM_FLAGS_KEY);
								teclado.flags |= FLAG_TECLA_PULSADA;
								piUnlock(SYSTEM_FLAGS_KEY);
							}
						}	
						// Se establece que no tiene ningún mensaje pendiente de procesar
						servidor.periferico[idPeriferico].mensajeSinProcesar = 0;
					}
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
	// Se deja de aceptar perifericos
	close(servidor.socket_fd);
	servidor.socket_fd = -1;
	// Para desconectar a todos los periféricos del servidor.
	// Se notifica a los periféricos y se cierran las conexiones
	for (int idPeriferico = 0; idPeriferico < MAX_PERIFERICOS_CONECTADOS; idPeriferico++) {
		enviarTexto("$Servidor_cerrado", idPeriferico);
	}
	servidor.servidorHabilitado = 0;
	delay(TIMEOUT_ENVIO_PANTALLA);
	pthread_cancel(servidor.thread_acepta_perifericos);
	//tmr_destroy((tmr_t*) (servidor.timer_pantalla));
	// Se eliminan las referencias a los clientes
	for (int idPeriferico = 0; idPeriferico < MAX_PERIFERICOS_CONECTADOS; idPeriferico++) {
		if (servidor.periferico[idPeriferico].conexion_fd != -1) {
			desconectarPeriferico(idPeriferico);
		}
	}
	servidor.perifericos_conectados = 0;
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
