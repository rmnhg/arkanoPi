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
    private final boolean debug = false;

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
                ScreenFragment screenFragment = ((ScreenFragment) getSupportFragmentManager().findFragmentByTag("ScreenFragment"));
                if (screenFragment != null)
                    //screenFragment.updateScreen(processTCPMessage(message));
                    mViewModel.updateScreen(processTCPMessage(message), screenFragment.getConsoleTextView(), screenFragment.getMatrizDeLeds(), screenFragment.getPrimeraPantallaEscrita(), "ScreenFragment");
                MixedFragment mixedFragment = ((MixedFragment) getSupportFragmentManager().findFragmentByTag("MixedFragment"));
                if (mixedFragment != null)
                    //mixedFragment.updateScreen(processTCPMessage(message));
                    mViewModel.updateScreen(processTCPMessage(message), mixedFragment.getConsoleTextView(), mixedFragment.getMatrizDeLeds(), mixedFragment.getPrimeraPantallaEscrita(), "MixedFragment");
            }
        }, new TCPClient.OnExternalCommunication() {
            @Override
            public void showInfoMessage(String message) {
                runOnUiThread(new Runnable() {
                    public void run() {
                        if (debug) // Ralentiza mucho la pantalla
                            Toast.makeText(getBaseContext(), message, Toast.LENGTH_SHORT).show();
                    }
                });
            }
        }, server_address, server_port);
        new ConnectTask(mTCPClient).start();
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