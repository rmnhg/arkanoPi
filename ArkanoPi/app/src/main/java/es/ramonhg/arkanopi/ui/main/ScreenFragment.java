package es.ramonhg.arkanopi.ui.main;

import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import android.os.Build;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import es.ramonhg.arkanopi.R;
import es.ramonhg.arkanopi.ui.model.MainViewModel;

public class ScreenFragment extends Fragment {
    public static final String TAG = "ScreenFragment";
    private String mTag = TAG;
    private View mView;

    TextView consoleTextView;
    Button settings;
    Button led[][] = new Button[7][8];
    private MainViewModel mViewModel;
    private boolean primeraPantallaEscrita = true;

    public static ScreenFragment newInstance() {
        return new ScreenFragment();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        mView = inflater.inflate(R.layout.screen_fragment, container, false);
        return mView;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        // Obtenemos el view model para comunicarnos con otras clases y obtener los datos
        mViewModel = new ViewModelProvider(getActivity()).get(MainViewModel.class);
        // Guardamos este fragment en el view model
        mViewModel.setScreenFragment(this);

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

        // Se dibuja la pantalla guardada
        mViewModel.updateScreen(mViewModel.getScreenContent(), consoleTextView, led, true, TAG);
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    /**
     * Función que establece si es la primera vez que se pinta una pantalla o no
     * @param primeraPantallaEscrita valor del boolean que controla este parámetro
     */
    public void setPrimeraPantallaEscrita(boolean primeraPantallaEscrita) {
        this.primeraPantallaEscrita = primeraPantallaEscrita;
    }

    /**
     * Método que devuelve si es la primera vez que se pinta la pantalla
     * @return boolean que indica si es la primera vez que se pinta la pantalla
     */
    public boolean getPrimeraPantallaEscrita() {
        return primeraPantallaEscrita;
    }

    /**
     * Método que devuelve la matriz de LEDs (Buttons) del fragment
     * @return matriz de LEDs (Buttons) del fragment
     */
    public Button[][] getMatrizDeLeds() {
        return led;
    }

    /**
     * Método que devuelve el TextView donde se muestran los mensajes de consola
     * @return TextView donde se muestran los mensajes de consola
     */
    public TextView getConsoleTextView() {
        return consoleTextView;
    }
}