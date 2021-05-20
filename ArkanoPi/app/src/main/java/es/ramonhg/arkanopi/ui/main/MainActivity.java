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
        // Obtenemos el view model para comunicarnos con otras clases y obtener los datos
        mViewModel = new ViewModelProvider(this).get(MainViewModel.class);
        // Guardamos esta activity en el view model
        mViewModel.setMainActivity(this);
        if (savedInstanceState == null) {
            getSupportFragmentManager().beginTransaction()
                    .replace(R.id.container, MixedFragment.newInstance())
                    .commitNow();
        }
    }

    /**
     * Método que muestra el fragment de ajustes.
     */
    public void showSettings() {
        getSupportFragmentManager().beginTransaction()
            .replace(R.id.container, SettingsFragment.newInstance(), SettingsFragment.TAG)
            .commitNow();
    }

    /**
     * Método que muestra el fragment de pantalla.
     */
    public void showScreen() {
        getSupportFragmentManager().beginTransaction()
            .replace(R.id.container, ScreenFragment.newInstance(), ScreenFragment.TAG)
            .commitNow();
    }

    /**
     * Método que muestra el fragment del teclado.
     */
    public void showKeyboard() {
        getSupportFragmentManager().beginTransaction()
            .replace(R.id.container, KeyboardFragment.newInstance(), KeyboardFragment.TAG)
            .commitNow();
    }

    /**
     * Método que muestra el fragment mixto.
     */
    public void showMixed() {
        getSupportFragmentManager().beginTransaction()
            .replace(R.id.container, MixedFragment.newInstance(), MixedFragment.TAG)
            .commitNow();
    }

    public void createConnection(String server_address, int server_port) {
        TCPClient mTCPClient = new TCPClient(new TCPClient.OnMessageReceived() {
            @Override
            public void messageReceived(String message) {
                // Si el mensaje no contiene un '$' sabemos que no es un mensaje de control. Se lo pasamos al método updateScreen del view model que se encarga de actualizar la pantalla o la consola adecuada
                if (message != null && processTCPMessage(message).length() > 0 && processTCPMessage(message).toCharArray()[0] != '$') { // No es un mensaje de control
                    ScreenFragment screenFragment = ((ScreenFragment) getSupportFragmentManager().findFragmentByTag("ScreenFragment"));
                    if (screenFragment != null)
                        mViewModel.updateScreen(processTCPMessage(message), screenFragment.getConsoleTextView(), screenFragment.getMatrizDeLeds(), screenFragment.getPrimeraPantallaEscrita(), "ScreenFragment");
                    MixedFragment mixedFragment = ((MixedFragment) getSupportFragmentManager().findFragmentByTag("MixedFragment"));
                    if (mixedFragment != null)
                        mViewModel.updateScreen(processTCPMessage(message), mixedFragment.getConsoleTextView(), mixedFragment.getMatrizDeLeds(), mixedFragment.getPrimeraPantallaEscrita(), "MixedFragment");

                    // Si no tenemos ningún fragment de pantalla abierto, guardamos los datos para cuando cambiemos de fragment
                    if (screenFragment == null && mixedFragment == null) {
                        if (processTCPMessage(message).contains("0")) { //Guardamos la pantalla para usarla después
                            mViewModel.setScreenContent(processTCPMessage(message));
                        } else { //Guardamos la consola para usarla después
                            // Sustituimos los # por \n por la transformación realizada en el servidor
                            mViewModel.setConsoleContent(processTCPMessage(message).replace('#', '\n'));
                        }
                    }
                } else {  // Es un mensaje de control
                    if (message != null && processTCPMessage(message).length() > 0 && (processTCPMessage(message).contains("$Servidor_cerrado") || processTCPMessage(message).contains("$Desconectado_por_inactividad"))) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                if (processTCPMessage(message).contains("$Servidor_cerrado")) {
                                    Toast.makeText(getBaseContext(), "El servidor se ha cerrado. Se ha detenido la conexión", Toast.LENGTH_SHORT).show();
                                } else {
                                    Toast.makeText(getBaseContext(), "El servidor le ha desconectado por inactividad", Toast.LENGTH_SHORT).show();
                                }
                            }
                        });
                        // Se detiene el cliente TCP
                        if (mViewModel.getTcpClient() != null) {
                            mViewModel.getTcpClient().stopClient();
                            mViewModel.setTcpClient(null);
                        }
                        // Se habilitan los elementos del fragment de ajustes para conectarse nuevamente
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
                        // Si hay un error en la conexión, se elimina el cliente TCP, se permiten nuevas conexiones y se informa al usuario a través de un Toast
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
                // Para fines de desarrollo, se pueden enviar Toasts informativos desde el cliente TCP
                runOnUiThread(new Runnable() {
                    public void run() {
                        Toast.makeText(getBaseContext(), message, Toast.LENGTH_SHORT).show();
                    }
                });
            }
        }, server_address, server_port);
        // Antes de conectarnos nos aseguramos de que no quede ninguna conexión pendiente de detenerse
        if (mViewModel.getConnectTask() != null && mViewModel.getConnectTask().isAlive()) {
            try {
                if (mViewModel.getTcpClient() != null) {
                    mViewModel.getTcpClient().stopClient();
                }
                mViewModel.getConnectTask().interrupt();
            } catch (Exception e) {}
            mViewModel.setConnectTask(null);
        } else if (mViewModel.getTcpClient() != null) {
            mViewModel.getTcpClient().stopClient();
        }
        // Guardamos todos los parámetros de la conexión en el view model
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