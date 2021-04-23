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
    // message to send to the server
    private String mServerMessage;
    // sends message received notifications
    private OnMessageReceived mMessageListener = null;
    // sends information messages about the connection
    private OnExternalCommunication mCommunicationListener = null;
    // while this is true, the server will continue running
    private boolean mRun = false;
    // used to send messages
    private PrintWriter mBufferOut;
    // used to read messages from the server
    private BufferedReader mBufferIn;

    /**
     * Constructor of the class. OnMessagedReceived listens for the messages received from server
     */
    public TCPClient(OnMessageReceived listener, OnExternalCommunication mCommunicationListener, String server_ip, int server_port) {
        this.server_ip = server_ip;
        this.server_port = server_port;
        mMessageListener = listener;
        this.mCommunicationListener = mCommunicationListener;
    }

    /**
     * Sends the message entered by client to the server
     *
     * @param message text entered by client
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
     * Close the connection and release the members
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
            //here you must put your computer's IP address.
            InetAddress serverAddr = InetAddress.getByName(server_ip);

            Log.d("TCP Client", "C: Connecting...");
            mCommunicationListener.showInfoMessage("Connecting...");

            //create a socket to make the connection with the server
            Socket socket = new Socket(serverAddr, server_port);

            try {

                //sends the message to the server
                mBufferOut = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())), true);

                //receives the message which the server sends back
                mBufferIn = new BufferedReader(new InputStreamReader(socket.getInputStream()));

                Log.d("TCP Client", "C: Succesfully connected to " + server_ip + ':' + server_port + "!");
                mCommunicationListener.showInfoMessage("Succesfully connected to " + server_ip + ':' + server_port + "!");
                connected = true;

                //in this while the client listens for the messages sent by the server
                while (mRun) {

                    mServerMessage = mBufferIn.readLine();

                    if (mServerMessage != null && mMessageListener != null) {
                        //call the method messageReceived from MyActivity class
                        mMessageListener.messageReceived(mServerMessage);
                        Log.e("RESPONSE FROM SERVER", "S: Received Message: '" + mServerMessage + "'");
                        mCommunicationListener.showInfoMessage("Received Message: '" + mServerMessage + "'");
                        connected = true;
                    }

                }

                Log.d("RESPONSE FROM SERVER", "S: Received Message: '" + mServerMessage + "'");
                mCommunicationListener.showInfoMessage("Received Message: '" + mServerMessage + "'");

            } catch (Exception e) {
                Log.e("TCP", "S: Error", e);
                mCommunicationListener.showInfoMessage("Error! Closing connection!");
                connected = false;
            } finally {
                //the socket must be closed. It is not possible to reconnect to this socket
                // after it is closed, which means a new socket instance has to be created.
                socket.close();
                connected = false;
            }

        } catch (Exception e) {
            Log.e("TCP", "C: Error", e);
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
}
