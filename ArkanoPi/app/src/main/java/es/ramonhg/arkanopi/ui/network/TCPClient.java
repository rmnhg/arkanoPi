package es.ramonhg.arkanopi.ui.network;

import android.util.Log;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;

public class TCPClient {

    public static final String TAG = TCPClient.class.getSimpleName();
    public String server_ip;
    public int server_port;
    public boolean connected;
    private final boolean debug = false;
    //Mensaje que se manda al servidor
    private String mServerMessage;
    //Envía notificaciones cuando se recibe un mensaje
    private OnMessageReceived mMessageListener = null;
    //Envía los comandos necesarios para establecer los parámetros adecuados en el servidor
    private OnInitialSetup mSetupListener = null;
    //Realiza las acciones necesarias cuando ocurre un error
    private OnError mErrorListener = null;
    //Envía mensajes de información sobre la conexión
    private OnExternalCommunication mCommunicationListener = null;
    //Mantiene la conexión activa
    private boolean mRun = false;
    //Buffer para mandar mensajes
    private PrintWriter mBufferOut;
    //Buffer que recibe mensajes del servidor
    private BufferedReader mBufferIn;

    /**
     * Constructor de la clase. OnMessagedReceived escucha los mensajes recibidos del servidor
     */
    public TCPClient(OnMessageReceived mMessageListener, OnInitialSetup mSetupListener, OnError mErrorListener,OnExternalCommunication mCommunicationListener, String server_ip, int server_port) {
        this.server_ip = server_ip;
        this.server_port = server_port;
        this.mMessageListener = mMessageListener;
        this.mErrorListener = mErrorListener;
        this.mSetupListener = mSetupListener;
        this.mCommunicationListener = mCommunicationListener;
    }

    /**
     * Envía un mensaje específico al servidor.
     *
     * @param message mensaje que se envía al servidor
     */
    public void sendMessage(final String message) {
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                if (mBufferOut != null) {
                    Log.d(TAG, "Sending: " + message);
                    mBufferOut.println(message);
                    mBufferOut.flush();
                }
            }
        };
        Thread thread = new Thread(runnable);
        thread.start();
    }

    /**
     * Cierra la conexión y libera los recursos
     */
    public void stopClient() {

        mRun = false;

        if (mBufferOut != null) {
            mBufferOut.flush();
            mBufferOut.close();
        }

        mMessageListener = null;
        mBufferIn = null;
        mBufferOut = null;
        mServerMessage = null;
    }

    public void run() {
        mRun = true;

        try {
            //Dirección IP del servidor
            InetAddress serverAddr = InetAddress.getByName(server_ip);

            Log.d("TCP Client", "C: Conectando...");
            mCommunicationListener.showInfoMessage("Conectando...");

            //Se crea un socket para establecer la conexión con el servidor
            Socket socket = new Socket(serverAddr, server_port);

            try {
                //Envía los mensajes al servidor
                mBufferOut = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())), true);

                //Recibe los mensajes del servidor
                mBufferIn = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                Log.d("TCP Client", "C: ¡Se ha conectado con éxito a " + server_ip + ':' + server_port + "!");
                mCommunicationListener.showInfoMessage("¡Se ha conectado con éxito a " + server_ip + ':' + server_port + "!");
                connected = true;

                //Se mandan los comandos necesarios para configurar el servidor adecuadamente
                mSetupListener.sendSetupCommands();

                //En este bucle el cliente recibe los mensajes del servidor
                while (mRun) {
                    //Se guarda el mensaje recibido en el buffer
                    mServerMessage = mBufferIn.readLine();

                    if (mServerMessage != null && mMessageListener != null) {
                        //Se llama al método messageReceived de la clase MainActivity
                        mMessageListener.messageReceived(mServerMessage);
                        Log.e("RESPUESTA DEL SERVIDOR", "S: Mensaje recibido: '" + mServerMessage + "'");
                        if (debug) // Ralentiza mucho la pantalla
                            mCommunicationListener.showInfoMessage("Mensaje recibido: '" + mServerMessage + "'");
                        connected = true;
                    }

                }

                Log.d("RESPUESTA DEL SERVIDOR", "S: Mensaje recibido: '" + mServerMessage + "'");
                if (debug) // Ralentiza mucho la pantalla
                    mCommunicationListener.showInfoMessage("Mensaje recibido: '" + mServerMessage + "'");

            } catch (Exception e) {
                Log.e("TCP", "S: Error", e);
                mErrorListener.errorInConnection();
                connected = false;
            } finally {
                //the socket must be closed. It is not possible to reconnect to this socket
                // after it is closed, which means a new socket instance has to be created.
                socket.close();
                connected = false;
            }

        } catch (Exception e) {
            Log.e("TCP", "C: Error", e);
            mErrorListener.errorInConnection();
            connected = false;
        }

    }

    // Interfaces de salida de datos del servidor TCP.
    public interface OnMessageReceived {
        public void messageReceived(String message);
    }

    public interface OnExternalCommunication {
        public void showInfoMessage(String message);
    }

    public interface OnInitialSetup {
        public void sendSetupCommands();
    }

    public interface OnError {
        public void errorInConnection();
    }
}
