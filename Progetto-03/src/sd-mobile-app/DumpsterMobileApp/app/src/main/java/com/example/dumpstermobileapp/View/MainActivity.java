package com.example.dumpstermobileapp.View;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import com.example.dumpstermobileapp.Controller.ControllerImpl;
import com.example.dumpstermobileapp.Model.utils.C;
import com.example.dumpstermobileapp.Model.btlib.BluetoothChannel;
import com.example.dumpstermobileapp.Model.btlib.BluetoothUtils;
import com.example.dumpstermobileapp.Model.btlib.ConnectToBluetoothServerTask;
import com.example.dumpstermobileapp.Model.btlib.ConnectionTask;
import com.example.dumpstermobileapp.Model.btlib.RealBluetoothChannel;
import com.example.dumpstermobileapp.Model.btlib.exceptions.BluetoothDeviceNotFound;
import com.example.dumpstermobileapp.Model.netutils.Http;
import com.example.dumpstermobileapp.R;
import org.json.JSONException;
import org.json.JSONObject;
import java.net.HttpURLConnection;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.Locale;
import java.util.Objects;
import java.util.UUID;

import static android.view.View.INVISIBLE;
import static android.view.View.VISIBLE;

public class MainActivity extends AppCompatActivity {
    
    /* Project Group: Baiardi Martina, 
                  Giachin Marco, 
                  Lombardini Alessandro */

    /*
        E' necessario modificare:
        - HOST (HTTPManager): ovvero indirizzo dell'Dumpster Service (ngrok)
        - BT_DEVICE_ACTING_AS_SERVER_NAME (Model.utils.C): ovvero il nome del dispositivo server (Arduino)
     */
    private ControllerImpl controller;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_NOSENSOR);
        this.controller = new ControllerImpl(this);
        Toast.makeText(this, "onCreate()", Toast.LENGTH_SHORT).show();
        initUI();
    }

    @Override
    protected  void onDestroy() {
        super.onDestroy();
        this.controller.disconnectBluetooth();
    }

    private void initUI() {
        findViewById(R.id.buttonGetToken).setOnClickListener(v -> this.controller.getToken());
    }

    public void showGetTokenWentBad(){
        ((TextView)findViewById(R.id.textResult)).setText("Token non disponibile");
    }

    public void showGetTokenWentGood(){
        ((TextView)findViewById(R.id.textResult)).setText("Token ottenuto");
        showConnectToDumpster();
    }

    private void showConnectToDumpster(){
        ((Button)findViewById(R.id.buttonDumpster)).setVisibility(VISIBLE);
        ((TextView)findViewById(R.id.textDumpster)).setVisibility(VISIBLE);
        ((Button)findViewById(R.id.buttonGetToken)).setEnabled(false);
        /* Alla pressione del pulsante il dispositivo tenta di connettersi mediante bluetooth al dispositivo menzionato in C.java */
        findViewById(R.id.buttonDumpster).setOnClickListener(v ->  {
            /* Se il bluetooth non è attivo, ne viene richiesta l'attivazione */
            BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
            this.controller.tryToConnectWithBluetooth(btAdapter);
        });
    }

    public void showTimeIsOver(){
        this.showAlert("Il token non è più valido", "Richiedi un nuovo token per interagire con il dumpster");
        restartInterface();
    }


    public void showBluetoothConnectionResult(String text){
        ((TextView)findViewById(R.id.textDumpster)).setText(text);
    }

    public void showSelectionOfTypeOfTrash(){
        ((TextView)findViewById(R.id.textTipoDiRifiuto)).setVisibility(VISIBLE);
        ((Button)findViewById(R.id.buttonA)).setVisibility(VISIBLE);
        ((Button)findViewById(R.id.buttonB)).setVisibility(VISIBLE);
        ((Button)findViewById(R.id.buttonC)).setVisibility(VISIBLE);
        findViewById(R.id.buttonA).setOnClickListener(v -> {
            this.controller.stopCountDown();
            this.controller.sendTrashSelected("A");
            disableSelectionOfTrash();
        });
        findViewById(R.id.buttonB).setOnClickListener(v -> {
            this.controller.stopCountDown();
            this.controller.sendTrashSelected("B");
            disableSelectionOfTrash();
        });
        findViewById(R.id.buttonC).setOnClickListener(v -> {
            this.controller.stopCountDown();
            this.controller.sendTrashSelected("C");
            disableSelectionOfTrash();
        });
        this.showDisconnectButton();
    }

    private void showDisconnectButton(){
        ((Button)findViewById(R.id.buttonDumpster)).setText("DISCONNETTI DAL DUMPSTER");
        findViewById(R.id.buttonDumpster).setOnClickListener(v -> {
            ((Button)findViewById(R.id.buttonDumpster)).setText("CONNETTI A DUMPSTER");
            this.controller.stopCountDown();
            this.controller.disconnectBluetooth();
            this.restartInterface();
            findViewById(R.id.buttonDumpster).setOnClickListener(c ->  {
                /* Se il bluetooth non è attivo, ne viene richiesta l'attivazione */
                BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
                this.controller.tryToConnectWithBluetooth(btAdapter);
            });
        });
    }

    private void disableSelectionOfTrash(){
        ((Button)findViewById(R.id.buttonA)).setEnabled(false);
        ((Button)findViewById(R.id.buttonB)).setEnabled(false);
        ((Button)findViewById(R.id.buttonC)).setEnabled(false);
        ((Button)findViewById(R.id.buttonDumpster)).setEnabled(false);
    }

    public void showHoldTime(){
        this.period = 90000;
        this.activeTimer();
        Calendar calendar = Calendar.getInstance();
        calendar.add(Calendar.MINUTE, 1);
        calendar.add(Calendar.SECOND, 30);
        ((TextView)findViewById(R.id.textTempo)).setText("Il cassonetto si chiuderà alle " + calendar.get(Calendar.HOUR_OF_DAY) + ":" + calendar.get(Calendar.MINUTE) + ":" + calendar.get(Calendar.SECOND));
        ((TextView)findViewById(R.id.textTempo)).setVisibility(VISIBLE);
        ((Button)findViewById(R.id.buttonTime)).setVisibility(VISIBLE);
        ((Button)findViewById(R.id.buttonFinish)).setVisibility(VISIBLE);
        ((TextView)findViewById(R.id.textCounter)).setVisibility(VISIBLE);
        findViewById(R.id.buttonFinish).setOnClickListener(e -> this.controller.sendFinishDeposit());
        findViewById(R.id.buttonTime).setOnClickListener(v -> this.controller.sendRequestOfMoreTime());
    }

    public void showInternetRequestError(){
        ((TextView)findViewById(R.id.textResult)).setText("Network is unabled");
    }

    public void showTimeLeftSuccess(int seconds){
        this.showAlert("Procedere con il deposito", "Hai a disposizione " + seconds + " secondi per depositare i rifiuti");
    }

    public void showTimeLeftFailed(){
        this.showAlert("Non è possibile procedere con il deposito", "La tipologia di rifiuto inserita non è valida");
    }

    public void showAddTimeSuccess(int seconds){
        /* Aggiorno il cronometro */
        this.countDownTimer.cancel();
        this.period = Integer.parseInt(((TextView)findViewById(R.id.textCounter)).getText().toString().split(" ")[1]) * 1000;
        this.period = this.period + seconds * 1000;
        this.activeTimer();
        /* Aggiorno l'orario */
        try {
            SimpleDateFormat formatter = new SimpleDateFormat("HH:mm:ss");
            String data = ((TextView)findViewById(R.id.textTempo)).getText().toString().substring(31);
            Date date = formatter.parse(data);
            Calendar calendar = Calendar.getInstance();
            calendar.setTime(date);
            calendar.add(Calendar.SECOND, seconds);
            ((TextView)findViewById(R.id.textTempo)).setText("Il cassonetto si chiuderà alle " + calendar.get(Calendar.HOUR_OF_DAY) + ":" + calendar.get(Calendar.MINUTE) + ":" + calendar.get(Calendar.SECOND));
        } catch (ParseException e) {
            e.printStackTrace();
        }
        this.showAlert("Tempo prolungato", "Hai a disposizione " + seconds + " secondi in più per depositare i rifiuti");
    }

    private CountDownTimer countDownTimer;
    private int period = 0;
    private void activeTimer(){
        countDownTimer = new CountDownTimer(period, 1000) {
            public void onTick(long millisUntilFinished) {
                ((TextView)findViewById(R.id.textCounter)).setText(String.format(Locale.getDefault(), "Mancano %d secondi", millisUntilFinished / 1000L));
            }

            public void onFinish() {

            }
        }.start();
    }

    public void showAddTimeFailed(){
        this.showAlert("Tempo non prolungato", "Ci dispiace, non è stato possibile prolungare il tempo concesso.");
    }

    public void showTimeout(){
        this.showAlert("Tempo esaurito", "Hai terminato il tempo, richiedi un nuovo token.");
        this.restartInterface();
    }

    public void showCongratulation(){
        this.showAlert("Deposito terminato con successo", "Congratulazioni, hai terminato con successo il deposito.");
        this.restartInterface();
    }

    public void restartInterface(){
        ((Button)findViewById(R.id.buttonDumpster)).setEnabled(true);
        ((Button)findViewById(R.id.buttonGetToken)).setEnabled(true);
        ((TextView)findViewById(R.id.textResult)).setText("");
        ((TextView)findViewById(R.id.textDumpster)).setText("");
        ((Button)findViewById(R.id.buttonDumpster)).setText("CONNETTI A DUMPSTER");
        ((Button)findViewById(R.id.buttonA)).setEnabled(true);
        ((Button)findViewById(R.id.buttonB)).setEnabled(true);
        ((Button)findViewById(R.id.buttonC)).setEnabled(true);
        ((Button)findViewById(R.id.buttonDumpster)).setVisibility(INVISIBLE);
        ((TextView)findViewById(R.id.textDumpster)).setVisibility(INVISIBLE);
        ((TextView)findViewById(R.id.textTipoDiRifiuto)).setVisibility(INVISIBLE);
        ((Button)findViewById(R.id.buttonA)).setVisibility(INVISIBLE);
        ((Button)findViewById(R.id.buttonB)).setVisibility(INVISIBLE);
        ((Button)findViewById(R.id.buttonC)).setVisibility(INVISIBLE);
        ((Button)findViewById(R.id.buttonTime)).setVisibility(INVISIBLE);
        ((Button)findViewById(R.id.buttonFinish)).setVisibility(INVISIBLE);
        ((TextView)findViewById(R.id.textTempo)).setVisibility(INVISIBLE);
        ((TextView)findViewById(R.id.textCounter)).setVisibility(INVISIBLE);
        if (countDownTimer != null){
            this.countDownTimer.cancel();
        }
    }

    private void showAlert(final String title, final String message) {
        final AlertDialog dialog =   new AlertDialog.Builder(this)
                .setTitle(title)
                .setMessage(message)
                .setCancelable(false)
                .setNeutralButton("OK", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.dismiss();
                    }
                })
                .create();
        dialog.show();
    }

}
