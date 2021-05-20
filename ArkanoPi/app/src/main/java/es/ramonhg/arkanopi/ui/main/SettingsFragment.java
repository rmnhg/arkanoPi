package es.ramonhg.arkanopi.ui.main;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import es.ramonhg.arkanopi.R;
import es.ramonhg.arkanopi.ui.model.MainViewModel;

public class SettingsFragment extends Fragment implements AdapterView.OnItemSelectedListener {
    public static final String TAG = "SettingsFragment";
    Button back, connect;
    Spinner tipoConexion, selectorPartida;
    String[] periferico = {"Ambos", "Pantalla", "Teclado"};
    String[] partida = {"Host", "Partida 1", "Partida 2"};
    private MainViewModel mViewModel;
    private View mView;

    public SettingsFragment() {
        super(R.layout.settings_fragment);
    }

    public static SettingsFragment newInstance() {
        return new SettingsFragment();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        mView = inflater.inflate(R.layout.settings_fragment, container, false);
        return mView;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        // Obtenemos el view model para comunicarnos con otras clases y obtener los datos
        mViewModel = new ViewModelProvider(getActivity()).get(MainViewModel.class);
        // Inicializamos el spinner que define el tipo de conexión
        tipoConexion = mView.findViewById(R.id.tipo_conexion_spinner);
        tipoConexion.setOnItemSelectedListener(this);

        // Se crea el ArrayAdapter del tipo de conexión o periférico y se establece el layout de los elementos internos
        ArrayAdapter aa = new ArrayAdapter(this.getContext(), android.R.layout.simple_spinner_item, periferico);
        aa.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        // Se asigna este ArrayAdapter al spinner
        tipoConexion.setAdapter(aa);
        // Establecemos el periférico anteriormente elegido si había alguno
        for (int i = 0; i < periferico.length; i++) {
            if (periferico[i].equals(mViewModel.getTipoPeriferico())) {
                tipoConexion.setSelection(i);
            }
        }

        // Inicializamos el spinner que define la partida que se jugará
        selectorPartida = mView.findViewById(R.id.partida_spinner);
        // Definimos el comportamiento del spinner al cambiar de partida
        selectorPartida.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (!partida[position].equals(mViewModel.getPartidaActual())) {
                    // Se establece la nueva partida en el view model
                    mViewModel.setPartidaActual(partida[position]);
                    // Si cambiamos de partida, se deben borrar los datos anteriores porque no son válidos
                    mViewModel.setConsoleContent(null);
                    mViewModel.setScreenContent("00000000000000000000000000000000000000000000000000000000");
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
        // Establecemos la partida por defecto en la partida 1
        selectorPartida.setSelection(1);

        // Se crea el ArrayAdapter de las partidas y se establece el layout de los elementos internos
        ArrayAdapter aa2 = new ArrayAdapter(this.getContext(), android.R.layout.simple_spinner_item, partida);
        aa2.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        // Se asigna este ArrayAdapter al spinner
        selectorPartida.setAdapter(aa2);
        // Ponemos la partida guardada anteriormente si había alguna
        for (int i = 0; i < partida.length; i++) {
            if (partida[i].equals(mViewModel.getPartidaActual())) {
                selectorPartida.setSelection(i);
            }
        }

        // Se vuelve a la vista acorde con el tipo de periférico seleccionado
        back = mView.findViewById(R.id.backButton);
        back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mViewModel.getTipoPeriferico().equals("Ambos")) {
                    ((MainActivity)getActivity()).showMixed();
                } else if (mViewModel.getTipoPeriferico().equals("Teclado")) {
                    ((MainActivity)getActivity()).showKeyboard();
                } else {
                    ((MainActivity)getActivity()).showScreen();
                }
            }
        });
        // Inicializamos el botón de conectar
        connect = mView.findViewById(R.id.connectButton);
        connect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Bloqueamos el editor de texto IP al crear una conexión
                mView.findViewById(R.id.editTextIP).setFocusable(false);
                // Obtenemos los parámetros del servidor al que nos conectaremos
                String server_address = ((EditText) mView.findViewById(R.id.editTextIP)).getText().toString();
                String server_port_str = ((EditText) mView.findViewById(R.id.editTextPuerto)).getText().toString();
                int server_port = Integer.parseInt(server_port_str);
                // Finalmente creamos una nueva conexión con los datos recogidos
                MainActivity main = ((MainActivity)getActivity());
                main.createConnection(server_address, server_port);
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });
        // Si anteriormente nos conectamos a un servidor, establecemos los parámetros de dicho servidor en el fragment
        ((EditText) mView.findViewById(R.id.editTextIP)).setText(mViewModel.getServerAddress());
        if (mViewModel.getServerPort() != -1)
            ((EditText) mView.findViewById(R.id.editTextPuerto)).setText(""+mViewModel.getServerPort());
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
    }

    /**
     * Método que restaura la disponibilidad de los elementos relacionados con la conexión.
     */
    public void enableNewConnection() {
        mView.findViewById(R.id.editTextIP).setFocusableInTouchMode(true);
        mView.findViewById(R.id.editTextPuerto).setFocusableInTouchMode(true);
        mView.findViewById(R.id.tipo_conexion_spinner).setFocusableInTouchMode(true);
    }


    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }


    /**
     * Método que establece el tipo de periférico seleccionado en el view model
     */
    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        mViewModel.setTipoPeriferico(periferico[position]);
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }
}