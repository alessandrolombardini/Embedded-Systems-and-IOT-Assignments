package com.example.dumpstermobileapp.Model;

import android.app.Application;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Debug;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;
import com.example.dumpstermobileapp.Controller.ControllerImpl;
import com.example.dumpstermobileapp.Model.btlib.BluetoothChannel;
import com.example.dumpstermobileapp.Model.btlib.BluetoothUtils;
import com.example.dumpstermobileapp.Model.btlib.ConnectToBluetoothServerTask;
import com.example.dumpstermobileapp.Model.btlib.ConnectionTask;
import com.example.dumpstermobileapp.Model.btlib.RealBluetoothChannel;
import com.example.dumpstermobileapp.Model.btlib.exceptions.BluetoothDeviceNotFound;
import com.example.dumpstermobileapp.Model.utils.C;
import com.example.dumpstermobileapp.R;
import org.json.JSONException;
import org.json.JSONObject;

import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Set;
import java.util.UUID;

public class BluetoothManager extends AppCompatActivity {

    private final ControllerImpl controller;
    private final AppCompatActivity activity;
    private BluetoothAdapter btAdapter;                            /* Entry point del bluetooth */
    private BluetoothDevice btDevice;                              /* Dispostivo con cui si comunica */
    private BluetoothChannel btChannel;                            /* Canale di comunicazione bluetooth  */
    private BroadcastReceiver mReceiver;
    private ConnectToBluetoothServerTask connect;
    private boolean connessoSN;

    public BluetoothManager(ControllerImpl controller, AppCompatActivity activity){
        this.controller = controller;
        this.activity = activity;
        this.connessoSN = false;
        mReceiver = new BroadcastReceiver() {       /* Ricevitore di notifiche della DISCOVER del bluetooth */
            public void onReceive(Context context, Intent intent) {
                final String action = intent.getAction();
                if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)) {
                    /* Inizia la ricerca */
                    controller.showBluetoothConnectionResult("Status: discovering...");
                } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
                    /* Finisce la ricerca */
                    if(btDevice != null) {
                    /* Se ho trovato un dispositivo che presenta lo stesso nome di quello ricercato,
                       cerco di instaurare una comunicazione bluetooth */
                        connectToBTServer();
                    } else {
                        controller.showBluetoothConnectionResult("Status: nothing found");
                    }
                } else if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                    /* Dispositivo bluetooth trovato */
                    final BluetoothDevice device = (BluetoothDevice) intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    if(device.getName().equals(C.bluetooth.BT_DEVICE_ACTING_AS_SERVER_NAME)){
                    /* Se il dispositivo trovato ha lo stesso nome di quello ricercato lo salvo,
                       per poi tentare una connessione al momento della conclusione della ricerca.
                       Impongo il termine della ricerca. */
                        btDevice = device;
                        btAdapter.cancelDiscovery();
                    }
                }
            }
        };
    }

    public void tryToConnectWithBluetooth(BluetoothAdapter btAdapter){
        this.btAdapter = btAdapter;
        if(btAdapter != null && !btAdapter.isEnabled()){
            this.activity.startActivityForResult(new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE), C.bluetooth.ENABLE_BT_REQUEST);
        }
        try {
            /* Questo metodo consente di ricercare fra i dispositivi precedentemente associati, quindi già conosciuti
               dallo smarphone. Se non è presente un dispositivo con il nome fornito, il metodo .getPairedDeviceByName()
               produce un' eccezione di  tipo BluetoothDeviceNotFound.
               In tal caso avvio un ricerca del dispositivo bluetooth. */
            controller.showBluetoothConnectionResult("Status: searching...");
            //btDevice = BluetoothUtils.getPairedDeviceByName(C.bluetooth.BT_DEVICE_ACTING_AS_SERVER_NAME);
            btDevice = (BluetoothDevice) BluetoothUtils.getPairedDeviceByAddress("98:D3:32:70:4F:B6");
            connectToBTServer();
            this.connessoSN = true;
        } catch (Exception bluetoothDeviceNotFound) {
            /* Prima di avviare la ricerca dei dispositivi, definisco per quali eventi devo essere avvertito
               al fine di intercettarli e gestirli correttamente */
            final IntentFilter filter = new IntentFilter();
            filter.addAction(BluetoothDevice.ACTION_FOUND);
            filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
            filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
            this.activity.registerReceiver (mReceiver, filter);
            btAdapter.startDiscovery();
        }
    }

    public void connectToBTServer() {
        final UUID uuid = BluetoothUtils.getEmbeddedDeviceDefaultUuid();
        this.connect = new ConnectToBluetoothServerTask(btDevice, uuid, new ConnectionTask.EventListener() {
            @Override
            public void onConnectionActive(final BluetoothChannel channel) {
                controller.showBluetoothConnectionResult("Status: connected to device");
                controller.showSelectionOfTypeOfTrash();
                btChannel = channel;
                btChannel.registerListener(new RealBluetoothChannel.Listener() {
                    @Override
                    public void onMessageReceived(String receivedMessage) {
                        manageMessageReceived(receivedMessage);
                    }

                    @Override
                    public void onMessageSent(String sentMessage) {

                    }
                });
            }
            @Override
            public void onConnectionCanceled() {
                controller.showBluetoothConnectionResult("Status : connection failed!");
                connessoSN = false;
            }
        });
        connect.execute();
    }

    /*
     * Viene gestita la selezione del tipo di rifiuto.
     * Viene inviato un messaggio sul canale bluetooth per segnalare il tipo di rifiuto.
     * @param trash è la tipologia di rifiuto (può essere A, B, C)
     */
    public void sendTrashSelected(String typeOfTrash) {
        try{
            final String content = new JSONObject().put("messageType", "typeOfTrash")
                    .put("value", typeOfTrash)
                    .put("token", this.controller.getTokenTimestamp())
                    .toString();
            btChannel.sendMessage(content);
        } catch(JSONException e){
            e.printStackTrace();
        }
    }

    /*
     * Viene gestita la selezione del tipo di rifiuto.
     * Viene inviato un messaggio sul canale bluetooth per segnalare il tipo di rifiuto.
     * @param trash è la tipologia di rifiuto (può essere A, B, C)
     */
    public void sendFinishDeposit() {
        try{
            final String content = new JSONObject().put("messageType", "finishDeposit")
                    .toString();
            btChannel.sendMessage(content);
        } catch(JSONException e){
            e.printStackTrace();
        }
    }

    /*
     *  Viene gestita la richiesta di più tempo.
     *  Viene inviato un messaggio sul canale bluetooth per segnalare la richiesta.
     */
    public void sendRequestOfMoreTime() {
        try{
            final String content = new JSONObject().put("messageType", "requestOfTime")
                    .toString();
            btChannel.sendMessage(content);
        } catch(JSONException e){
            e.printStackTrace();
        }
    }

    private void manageMessageReceived(String messageRecevied){
        try {
            JSONObject obj = new JSONObject(messageRecevied);
            switch(obj.getString("messageType")){
                case "typeOfTrash":
                    manageResponseMessageTypeOftrash(obj);
                    break;
                case "requestOfTime":
                    manageResponseMessageRequestOfTime(obj);
                    break;
                case "endOfConnection":
                    manageResponseMessageEndOfConnection(obj);
                    break;
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private void manageResponseMessageTypeOftrash(JSONObject obj){
        try {
            if(obj.getString("value").equals("OK")){
                final int seconds = obj.getInt("time");
                this.controller.showTimeLeftSuccess(seconds);
                this.controller.showHoldTime();
            } else if(obj.getString("value").equals("NOT OK")){
                this.controller.stopCountDown();
                this.controller.disconnectBluetooth();
                this.controller.restartInterface();
                this.controller.showInternetRequestError();
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private void manageResponseMessageRequestOfTime(JSONObject obj){
        try {
            if(obj.getString("value").equals("OK")){
                final int timeAdded = obj.getInt("time");
                this.controller.showAddTimeSuccess(timeAdded);
            } else if(obj.getString("value").equals("NOT OK")){
                this.controller.showAddTimeFailed();
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private void manageResponseMessageEndOfConnection(JSONObject obj){
        try {
            if(obj.getString("value").equals("OK")){
                final String type = obj.getString("type");
                this.controller.sendNewTrash(type);
                this.controller.showCongratulation();
            } else if(obj.getString("value").equals("TIMEOUT")){ //Questo a cosa dovrebbe servire?
                this.controller.showTimeout();
            }
            if(this.connect != null){
                btChannel.close();
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void disconnectBluetooth(){
        if(this.connessoSN){
            btChannel.close();
        }
    }


}
