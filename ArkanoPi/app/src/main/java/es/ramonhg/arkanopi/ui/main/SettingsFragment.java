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
        mViewModel = new ViewModelProvider(getActivity()).get(MainViewModel.class);
        // TODO: Use the ViewModel
        tipoConexion = mView.findViewById(R.id.tipo_conexion_spinner);
        tipoConexion.setOnItemSelectedListener(this);

        //Creating the ArrayAdapter instance having the country list
        ArrayAdapter aa = new ArrayAdapter(this.getContext(), android.R.layout.simple_spinner_item, periferico);
        aa.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        //Setting the ArrayAdapter data on the Spinner
        tipoConexion.setAdapter(aa);
        for (int i = 0; i < periferico.length; i++) {
            if (periferico[i].equals(mViewModel.getTipoPeriferico())) {
                tipoConexion.setSelection(i);
            }
        }
        selectorPartida = mView.findViewById(R.id.partida_spinner);
        selectorPartida.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                mViewModel.setPartidaActual(partida[position]);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        //Creating the ArrayAdapter instance having the country list
        selectorPartida.setSelection(1);
        ArrayAdapter aa2 = new ArrayAdapter(this.getContext(), android.R.layout.simple_spinner_item, partida);
        aa2.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        selectorPartida.setAdapter(aa2);
        // Ponemos la partida guardada anteriormente
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
        connect = mView.findViewById(R.id.connectButton);
        connect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mView.findViewById(R.id.editTextIP).setFocusable(false);
                String server_address = ((EditText) mView.findViewById(R.id.editTextIP)).getText().toString();
                String server_port_str = ((EditText) mView.findViewById(R.id.editTextPuerto)).getText().toString();
                int server_port = Integer.parseInt(server_port_str);
                MainActivity main = ((MainActivity)getActivity());
                main.createConnection(server_address, server_port);
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });
        ((EditText) mView.findViewById(R.id.editTextIP)).setText(mViewModel.getServerAddress());
        if (mViewModel.getServerPort() != -1)
            ((EditText) mView.findViewById(R.id.editTextPuerto)).setText(""+mViewModel.getServerPort());
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
    }

    public void enableNewConnection() {
        //EditText.setFocusable(false)
        mView.findViewById(R.id.editTextIP).setFocusableInTouchMode(true);
        mView.findViewById(R.id.editTextPuerto).setFocusableInTouchMode(true);
        mView.findViewById(R.id.tipo_conexion_spinner).setFocusableInTouchMode(true);
    }


    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        mViewModel.setTipoPeriferico(periferico[position]);
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }
}