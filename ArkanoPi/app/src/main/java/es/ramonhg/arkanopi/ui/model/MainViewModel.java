package es.ramonhg.arkanopi.ui.model;

import android.inputmethodservice.Keyboard;

import androidx.lifecycle.ViewModel;

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
    private String screenContent, tipoPeriferico, server_address, consoleContent;
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
}