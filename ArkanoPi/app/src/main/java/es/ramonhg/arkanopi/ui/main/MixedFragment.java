package es.ramonhg.arkanopi.ui.main;

import android.os.Build;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.google.android.material.snackbar.Snackbar;

import es.ramonhg.arkanopi.R;
import es.ramonhg.arkanopi.ui.model.MainViewModel;

public class MixedFragment extends Fragment {
    public static final String TAG = "MixedFragment";
    private String mTag = TAG;
    private View mView;

    TextView consoleTextView;
    Button settings;
    Button led[][] = new Button[7][8];
    Button tecla[][] = new Button[4][4];
    private MainViewModel mViewModel;
    private boolean primeraPantallaEscrita = true;

    public static MixedFragment newInstance() {
        return new MixedFragment();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        mView = inflater.inflate(R.layout.mixed_fragment, container, false);
        return mView;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mViewModel = new ViewModelProvider(getActivity()).get(MainViewModel.class);
        mViewModel.setMixedFragment(this);

        settings = mView.findViewById(R.id.settingsButton);
        settings.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ((MainActivity)getActivity()).showSettings();
            }
        });

        // Preparación de los mensajes de la consola
        consoleTextView = mView.findViewById(R.id.consoleTextView);
        // Se muestra el mensaje de la consola actualizado almacenado si no está escrito ya
        if (consoleTextView.getText() != null && !(consoleTextView.getText().equals(mViewModel.getConsoleContent()))) {
            consoleTextView.setText(mViewModel.getConsoleContent());
        }

        // Preparación matriz de lEDs
        led[0][0] = mView.findViewById(R.id.led00);
        led[0][1] = mView.findViewById(R.id.led01);
        led[0][2] = mView.findViewById(R.id.led02);
        led[0][3] = mView.findViewById(R.id.led03);
        led[0][4] = mView.findViewById(R.id.led04);
        led[0][5] = mView.findViewById(R.id.led05);
        led[0][6] = mView.findViewById(R.id.led06);
        led[0][7] = mView.findViewById(R.id.led07);
        led[1][0] = mView.findViewById(R.id.led10);
        led[1][1] = mView.findViewById(R.id.led11);
        led[1][2] = mView.findViewById(R.id.led12);
        led[1][3] = mView.findViewById(R.id.led13);
        led[1][4] = mView.findViewById(R.id.led14);
        led[1][5] = mView.findViewById(R.id.led15);
        led[1][6] = mView.findViewById(R.id.led16);
        led[1][7] = mView.findViewById(R.id.led17);
        led[2][0] = mView.findViewById(R.id.led20);
        led[2][1] = mView.findViewById(R.id.led21);
        led[2][2] = mView.findViewById(R.id.led22);
        led[2][3] = mView.findViewById(R.id.led23);
        led[2][4] = mView.findViewById(R.id.led24);
        led[2][5] = mView.findViewById(R.id.led25);
        led[2][6] = mView.findViewById(R.id.led26);
        led[2][7] = mView.findViewById(R.id.led27);
        led[3][0] = mView.findViewById(R.id.led30);
        led[3][1] = mView.findViewById(R.id.led31);
        led[3][2] = mView.findViewById(R.id.led32);
        led[3][3] = mView.findViewById(R.id.led33);
        led[3][4] = mView.findViewById(R.id.led34);
        led[3][5] = mView.findViewById(R.id.led35);
        led[3][6] = mView.findViewById(R.id.led36);
        led[3][7] = mView.findViewById(R.id.led37);
        led[4][0] = mView.findViewById(R.id.led40);
        led[4][1] = mView.findViewById(R.id.led41);
        led[4][2] = mView.findViewById(R.id.led42);
        led[4][3] = mView.findViewById(R.id.led43);
        led[4][4] = mView.findViewById(R.id.led44);
        led[4][5] = mView.findViewById(R.id.led45);
        led[4][6] = mView.findViewById(R.id.led46);
        led[4][7] = mView.findViewById(R.id.led47);
        led[5][0] = mView.findViewById(R.id.led50);
        led[5][1] = mView.findViewById(R.id.led51);
        led[5][2] = mView.findViewById(R.id.led52);
        led[5][3] = mView.findViewById(R.id.led53);
        led[5][4] = mView.findViewById(R.id.led54);
        led[5][5] = mView.findViewById(R.id.led55);
        led[5][6] = mView.findViewById(R.id.led56);
        led[5][7] = mView.findViewById(R.id.led57);
        led[6][0] = mView.findViewById(R.id.led60);
        led[6][1] = mView.findViewById(R.id.led61);
        led[6][2] = mView.findViewById(R.id.led62);
        led[6][3] = mView.findViewById(R.id.led63);
        led[6][4] = mView.findViewById(R.id.led64);
        led[6][5] = mView.findViewById(R.id.led65);
        led[6][6] = mView.findViewById(R.id.led66);
        led[6][7] = mView.findViewById(R.id.led67);

        // Preparación matriz de teclas
        tecla[0][0] = mView.findViewById(R.id.tecla00);
        tecla[0][1] = mView.findViewById(R.id.tecla01);
        tecla[0][2] = mView.findViewById(R.id.tecla02);
        tecla[0][3] = mView.findViewById(R.id.tecla03);
        tecla[1][0] = mView.findViewById(R.id.tecla10);
        tecla[1][1] = mView.findViewById(R.id.tecla11);
        tecla[1][2] = mView.findViewById(R.id.tecla12);
        tecla[1][3] = mView.findViewById(R.id.tecla13);
        tecla[2][0] = mView.findViewById(R.id.tecla20);
        tecla[2][1] = mView.findViewById(R.id.tecla21);
        tecla[2][2] = mView.findViewById(R.id.tecla22);
        tecla[2][3] = mView.findViewById(R.id.tecla23);
        tecla[3][0] = mView.findViewById(R.id.tecla30);
        tecla[3][1] = mView.findViewById(R.id.tecla31);
        tecla[3][2] = mView.findViewById(R.id.tecla32);
        tecla[3][3] = mView.findViewById(R.id.tecla33);

        // Envío de pulsado de teclas
        for (int fila = 0; fila < 4; fila++) {
            for (int columna = 0; columna < 4; columna++) {
                if (!(fila == 3 && columna == 3)) {
                    int f = fila;
                    int c = columna;
                    tecla[fila][columna].setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            if (mViewModel.getTcpClient() != null)
                                mViewModel.getTcpClient().sendMessage(""+f+c);
                            else
                                Toast.makeText(getContext(), "No está conectado al servidor", Toast.LENGTH_SHORT).show();
                        }
                    });
                }
            }
        }
        // Un toque simple en la F solo desconecta el móvil del servidor.
        tecla[3][3].setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mViewModel.getTcpClient() != null) {
                    Snackbar.make(v, getResources().getText(R.string.snackbar_desconectar_cliente), Snackbar.LENGTH_LONG)
                            .setAction(R.string.snackbar_ok, new View.OnClickListener() {
                                @Override
                                public void onClick(View v) {
                                    mViewModel.getTcpClient().sendMessage("$Desconectar_cliente");
                                    mViewModel.setTcpClient(null);
                                }
                            })
                            .show();
                } else
                    Toast.makeText(getContext(), "No está conectado al servidor", Toast.LENGTH_SHORT).show();
            }
        });
        // Mantener pulsada la F detiene el servidor.
        tecla[3][3].setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                if (mViewModel.getTcpClient() != null) {
                    Snackbar.make(v, getResources().getText(R.string.snackbar_detener_servidor), Snackbar.LENGTH_LONG)
                            .setAction(R.string.snackbar_ok, new View.OnClickListener() {
                                @Override
                                public void onClick(View v) {
                                    mViewModel.getTcpClient().sendMessage("33");
                                    mViewModel.setTcpClient(null);
                                }
                            })
                            .show();
                } else
                    Toast.makeText(getContext(), "No está conectado al servidor", Toast.LENGTH_SHORT).show();
                return true;
            }
        });
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    public void setPrimeraPantallaEscrita(boolean primeraPantallaEscrita) {
        this.primeraPantallaEscrita = primeraPantallaEscrita;
    }

    public boolean getPrimeraPantallaEscrita() {
        return primeraPantallaEscrita;
    }

    public Button[][] getMatrizDeLeds() {
        return led;
    }

    public TextView getConsoleTextView() {
        return consoleTextView;
    }
}