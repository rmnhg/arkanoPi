package es.ramonhg.arkanopi.ui.model;

import android.inputmethodservice.Keyboard;
import android.os.Build;
import android.widget.Button;
import android.widget.TextView;

import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModel;

import es.ramonhg.arkanopi.R;
import es.ramonhg.arkanopi.ui.main.KeyboardFragment;
import es.ramonhg.arkanopi.ui.main.MainActivity;
import es.ramonhg.arkanopi.ui.main.ScreenFragment;
import es.ramonhg.arkanopi.ui.main.MixedFragment;
import es.ramonhg.arkanopi.ui.network.TCPClient;

public class MainViewModel extends ViewModel {
    public MainActivity mainActivity;
    public ScreenFragment screenFragment;
    public KeyboardFragment keyboardFragment;
    public MixedFragment mixedFragment;
    private String screenContent, tipoPeriferico, server_address, consoleContent, partidaActual;
    private int server_port = -1;
    TCPClient tcpClient;

    public MainActivity getMainActivity() {
        return mainActivity;
    }

    public void setMainActivity(MainActivity mainActivity) {
        this.mainActivity = mainActivity;
    }

    public void setScreenFragment(ScreenFragment screenFragment) {
        this.screenFragment = screenFragment;
    }

    public ScreenFragment getScreenFragment() {
        return screenFragment;
    }

    public void setKeyboardFragment(KeyboardFragment keyboardFragment) {
        this.keyboardFragment = keyboardFragment;
    }

    public KeyboardFragment getKeyboardFragment() {
        return keyboardFragment;
    }

    public void setMixedFragment(MixedFragment mixedFragment) {
        this.mixedFragment = mixedFragment;
    }

    public MixedFragment getMixedFragment() {
        return mixedFragment;
    }

    public String getScreenContent() {
        if (screenContent == null)
            // Si es la primera vez que se pide el contenido de la pantalla y no se ha
            // recibido nada anteriormente, se ponen todos los LEDs apagados (en 0).
            screenContent = "00000000000000000000000000000000000000000000000000000000";
        return screenContent;
    }

    public void setScreenContent(String screenContent) {
        this.screenContent = screenContent;
    }

    public String getTipoPeriferico() {
        return tipoPeriferico;
    }

    public void setTipoPeriferico(String tipoPeriferico) {
        this.tipoPeriferico = tipoPeriferico;
    }

    public void setTcpClient(TCPClient tcpClient) {
        this.tcpClient = tcpClient;
    }

    public TCPClient getTcpClient() {
        if (tcpClient != null && !(tcpClient.connected))
            tcpClient = null;
        return tcpClient;
    }

    public String getServerAddress() {
        return server_address;
    }

    public void setServerAddress(String server_address) {
        this.server_address = server_address;
    }

    public int getServerPort() {
        return server_port;
    }

    public void setServerPort(int server_port) {
        this.server_port = server_port;
    }

    public String getConsoleContent() {
        return consoleContent;
    }

    public void setConsoleContent(String consoleContent) {
        this.consoleContent = consoleContent;
    }

    /**
     * Método que actualiza los elementos de la pantalla con cada mensaje recibido por TCP.
     * @param receivedMessage el mensaje procesado recibido por TCP.
     * @param consoleTextView el TexView que contiene el texto de la consola en el fragment
     * @param led array de Buttons que representa el array de LEDs
     * @param primeraPantallaEscrita boolean que representa si un fragment pinta la matriz de LEDs por primera vez
     * @param fragmentType nombre del fragmento que se cambia
     */
    public void updateScreen(String receivedMessage, TextView consoleTextView, Button[][] led, boolean primeraPantallaEscrita, String fragmentType) {
        boolean forzarDibujadoPantalla = primeraPantallaEscrita;
        getMainActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                boolean jugando = false;
                // Si la longitud del mensaje recibido no es la de una pantalla, es un mensaje de
                // consola.
                if (receivedMessage.length() != 7*8) {
                    String realMessage = receivedMessage.replace('#', '\n');
                    setConsoleContent(realMessage);
                    // Se muestra el mensaje de la consola recibido
                    consoleTextView.setText(getConsoleContent());
                } else if (forzarDibujadoPantalla || !receivedMessage.equals(getScreenContent())) {
                    // Si se ha cambiado de fragment o ha llegado una pantalla distinta a la
                    // almacenada, se pinta la pantalla de LEDs.
                    if (fragmentType.equals("ScreenFragment")) {
                        getScreenFragment().setPrimeraPantallaEscrita(false);
                    } else {
                        getMixedFragment().setPrimeraPantallaEscrita(false);
                    }
                    setScreenContent(receivedMessage);
                    char[] screenDigits = receivedMessage.toCharArray();

                    for (int i = 6 * 8; i < 6 * 8 + 8; i++) {
                        // Se comprueba si se está jugando o no viendo si en la última fila hay unos
                        // (la pala). Si no, estaremos mostrando una pantalla inicial o final.
                        if (screenDigits[i] != '0') {
                            jugando = true;
                            setConsoleContent(null);
                            consoleTextView.setText("");
                            break;
                        }
                    }

                    for (int i = 0; i < 7; i++) {
                        for (int j = 0; j < 8; j++) {
                            int strIndex = i * 8 + j;
                            char digit = screenDigits[strIndex];
                            if (jugando) {
                                if (digit == '8') { // Pelota
                                    led[i][j].setBackgroundColor(getMainActivity().getResources().getColor(R.color.blue));
                                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                                        led[i][j].setBackgroundTintList(getMainActivity().getResources().getColorStateList(R.color.blue));
                                    }
                                } else if (digit == '1' && i < 3) { // Ladrillos
                                    led[i][j].setBackgroundColor(getMainActivity().getResources().getColor(R.color.red));
                                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                                        led[i][j].setBackgroundTintList(getMainActivity().getResources().getColorStateList(R.color.red));
                                    }
                                } else if (digit == '1' && i == 6) { // pala
                                    led[i][j].setBackgroundColor(getMainActivity().getResources().getColor(R.color.green));
                                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                                        led[i][j].setBackgroundTintList(getMainActivity().getResources().getColorStateList(R.color.green));
                                    }
                                } else { // LED apagado
                                    led[i][j].setBackgroundColor(getMainActivity().getResources().getColor(R.color.black));
                                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                                        led[i][j].setBackgroundTintList(getMainActivity().getResources().getColorStateList(R.color.black));
                                    }
                                }
                            } else {
                                // Estamos en una pantalla final o inicial. Los LEDs encendidos
                                // se pintarán de color rojo.
                                if (digit == '1') { // LED encendido
                                    led[i][j].setBackgroundColor(getMainActivity().getResources().getColor(R.color.red));
                                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                                        led[i][j].setBackgroundTintList(getMainActivity().getResources().getColorStateList(R.color.red));
                                    }
                                } else { // LED apagado
                                    led[i][j].setBackgroundColor(getMainActivity().getResources().getColor(R.color.black));
                                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                                        led[i][j].setBackgroundTintList(getMainActivity().getResources().getColorStateList(R.color.black));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        });
    }

    public String getPartidaActual() {
        if (partidaActual == null)
            partidaActual = "Partida 1";
        return partidaActual;
    }

    public void setPartidaActual(String partidaActual) {
        this.partidaActual = partidaActual;
        sendPartidaActual();
    }

    public void sendPartidaActual() {
        if (this.getTcpClient() != null) {
            // Debemos informar al servidor de qué partida queremos controlar
            if (partidaActual.equals("Host")) {
                this.getTcpClient().sendMessage("$Cambiar_a_partida_0\n");
            } else if (partidaActual.equals("Partida 1")) {
                this.getTcpClient().sendMessage("$Cambiar_a_partida_1\n");
            } else if (partidaActual.equals("Partida 2")) {
                this.getTcpClient().sendMessage("$Cambiar_a_partida_2\n");
            }
        }
    }
}