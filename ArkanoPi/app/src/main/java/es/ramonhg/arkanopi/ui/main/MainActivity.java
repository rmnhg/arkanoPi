package es.ramonhg.arkanopi.ui.main;

import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.ViewModelProvider;

import android.os.Bundle;
import android.widget.Toast;

import es.ramonhg.arkanopi.ui.network.ConnectTask;
import es.ramonhg.arkanopi.R;
import es.ramonhg.arkanopi.ui.network.TCPClient;
import es.ramonhg.arkanopi.ui.model.MainViewModel;

public class MainActivity extends AppCompatActivity {
    private MainViewModel mViewModel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout._activity);
        mViewModel = new ViewModelProvider(this).get(MainViewModel.class);
        mViewModel.setMainActivity(this);
        if (savedInstanceState == null) {
            getSupportFragmentManager().beginTransaction()
                    .replace(R.id.container, ScreenFragment.newInstance())
                    .commitNow();
        }
    }

    public void showSettings() {
        getSupportFragmentManager().beginTransaction()
            .replace(R.id.container, SettingsFragment.newInstance(), SettingsFragment.TAG)
            .commitNow();
    }

    public void showScreen() {
        getSupportFragmentManager().beginTransaction()
            .replace(R.id.container, ScreenFragment.newInstance(), ScreenFragment.TAG)
            .commitNow();
    }

    public void showKeyboard() {
        getSupportFragmentManager().beginTransaction()
            .replace(R.id.container, KeyboardFragment.newInstance(), KeyboardFragment.TAG)
            .commitNow();
    }

    public void showMixed() {
        getSupportFragmentManager().beginTransaction()
            .replace(R.id.container, MixedFragment.newInstance(), MixedFragment.TAG)
            .commitNow();
    }

    public void createConnection(String server_address, int server_port) {
        TCPClient mTCPClient = new TCPClient(new TCPClient.OnMessageReceived() {
            @Override
            //here the messageReceived method is implemented
            public void messageReceived(String message) {
                //this method calls the onProgressUpdate
                if (message != null && processTCPMessage(message).length() > 0 && processTCPMessage(message).toCharArray()[0] != '$') { // No es un mensaje de control
                    ScreenFragment screenFragment = ((ScreenFragment) getSupportFragmentManager().findFragmentByTag("ScreenFragment"));
                    if (screenFragment != null)
                        //screenFragment.updateScreen(processTCPMessage(message));
                        mViewModel.updateScreen(processTCPMessage(message), screenFragment.getConsoleTextView(), screenFragment.getMatrizDeLeds(), screenFragment.getPrimeraPantallaEscrita(), "ScreenFragment");
                    MixedFragment mixedFragment = ((MixedFragment) getSupportFragmentManager().findFragmentByTag("MixedFragment"));
                    if (mixedFragment != null)
                        //mixedFragment.updateScreen(processTCPMessage(message));
                        mViewModel.updateScreen(processTCPMessage(message), mixedFragment.getConsoleTextView(), mixedFragment.getMatrizDeLeds(), mixedFragment.getPrimeraPantallaEscrita(), "MixedFragment");

                    if (screenFragment == null && mixedFragment == null) {
                        if (processTCPMessage(message).contains("0")) { //Guardamos la pantalla para usarla después
                            mViewModel.setScreenContent(processTCPMessage(message));
                        } else { //Guardamos la consola para usarla después
                            mViewModel.setConsoleContent(processTCPMessage(message).replace('#', '\n'));
                        }
                    }
                } else {  // Es un mensaje de control
                    if (message != null && processTCPMessage(message).length() > 0 && processTCPMessage(message).contains("$Servidor_cerrado")) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(getBaseContext(), "Se ha desconectado del servidor", Toast.LENGTH_SHORT).show();
                            }
                        });
                        if (mViewModel.getTcpClient() != null) {
                            mViewModel.getTcpClient().stopClient();
                            mViewModel.setTcpClient(null);
                        }
                        SettingsFragment settingsFragment = ((SettingsFragment) getSupportFragmentManager().findFragmentByTag("SettingsFragment"));
                        if (settingsFragment != null)
                            settingsFragment.enableNewConnection();
                    }
                }
            }
        }, new TCPClient.OnInitialSetup() {
            @Override
            public void sendSetupCommands() {
                mViewModel.sendPartidaActual();
            }
        }, new TCPClient.OnError() {
            @Override
            public void errorInConnection() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mViewModel.setTcpClient(null);
                        SettingsFragment settingsFragment = ((SettingsFragment) getSupportFragmentManager().findFragmentByTag("SettingsFragment"));
                        if (settingsFragment != null)
                            settingsFragment.enableNewConnection();
                        Toast.makeText(getBaseContext(), "¡Ha habido un error! ¡Se detiene la conexión!", Toast.LENGTH_SHORT).show();
                    }
                });
            }
        }, new TCPClient.OnExternalCommunication() {
            @Override
            public void showInfoMessage(String message) {
                runOnUiThread(new Runnable() {
                    public void run() {
                        Toast.makeText(getBaseContext(), message, Toast.LENGTH_SHORT).show();
                    }
                });
            }
        }, server_address, server_port);
        if (mViewModel.getConnectTask() != null && mViewModel.getConnectTask().isAlive()) {
            try {
                mViewModel.getConnectTask().interrupt();
            } catch (Exception e) {}
            mViewModel.setConnectTask(null);
        }
        mViewModel.setConnectTask(new ConnectTask(mTCPClient));
        mViewModel.getConnectTask().start();
        mViewModel.setTcpClient(mTCPClient);
        mViewModel.setServerAddress(server_address);
        mViewModel.setServerPort(server_port);
    }

    /**
     * Método que elimina caracteres 0 de la string recbida por TCP.
     * @param TCPMessage el mensaje en bruto recibido por TCP.
     * @return el mensaje recibido procesado.
     */
    public String processTCPMessage(String TCPMessage) {
        String processedMessage = "";

        for (char s: TCPMessage.toCharArray()) {
            if (s != '\u0000') {
                processedMessage += s;
            }
        }
        return processedMessage;
    }
}