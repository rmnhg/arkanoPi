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

public class KeyboardFragment extends Fragment {
    public static final String TAG = "KeyboardFragment";
    private String mTag = TAG;
    private View mView;

    Button settings;
    Button tecla[][] = new Button[4][4];
    private MainViewModel mViewModel;

    public static KeyboardFragment newInstance() {
        return new KeyboardFragment();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        mView = inflater.inflate(R.layout.keyboard_fragment, container, false);
        return mView;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mViewModel = new ViewModelProvider(getActivity()).get(MainViewModel.class);
        mViewModel.setKeyboardFragment(this);

        settings = mView.findViewById(R.id.settingsButton);
        settings.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ((MainActivity)getActivity()).showSettings();
            }
        });

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
                    Snackbar.make(v, getResources().getText(R.string.snackbar_desconectar_cliente), Snackbar.LENGTH_LONG)
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
}