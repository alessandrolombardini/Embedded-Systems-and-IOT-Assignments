package com.example.dumpstermobileapp.Model;

import android.bluetooth.BluetoothAdapter;

import androidx.appcompat.app.AppCompatActivity;

import com.example.dumpstermobileapp.Controller.ControllerImpl;

import java.sql.Timestamp;

public class ModelImpl {

    private final ControllerImpl controller;
    private final AppCompatActivity activity;
    private final HTTPManager httpManager;
    private final BluetoothManager bluetoothManager;
    
    public ModelImpl(ControllerImpl controller, AppCompatActivity activity){
        this.controller = controller;
        this.activity = activity;
        this.httpManager = new HTTPManager(this.controller, activity);
        this.bluetoothManager = new BluetoothManager(this.controller, activity);
    }

    public void getToken() {
        this.httpManager.getToken();
    }

    public long getTokenTimestamp(){
        return this.httpManager.getTokenTimestamp();
    }


    public void setTokenTimestamp(long token){
        this.httpManager.setTokenTimestamp(token);
    }

    public void sendNewTrash(String type){
        this.httpManager.sendNewTrash(type);
    }

    public void tryToConnectWithBluetooth(BluetoothAdapter btAdapter){
        this.bluetoothManager.tryToConnectWithBluetooth(btAdapter);
    }

    public void sendTrashSelected(String typeOfTrash){
        this.bluetoothManager.sendTrashSelected(typeOfTrash);
    }

    public void sendRequestOfMoreTime(){
        this.bluetoothManager.sendRequestOfMoreTime();
    }

    public void sendFinishDeposit() {this.bluetoothManager.sendFinishDeposit();}

    public void disconnectBluetooth(){
        this.bluetoothManager.disconnectBluetooth();
    }

    public boolean checkValidToken() {
        return this.httpManager.checkValidToken();
    }

    public void stopCountDown(){
        this.httpManager.stopCountDown();
    }
}
