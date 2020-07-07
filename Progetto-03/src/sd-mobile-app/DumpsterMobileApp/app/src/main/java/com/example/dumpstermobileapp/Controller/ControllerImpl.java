package com.example.dumpstermobileapp.Controller;

import android.bluetooth.BluetoothAdapter;
import android.content.DialogInterface;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import com.example.dumpstermobileapp.Model.ModelImpl;
import com.example.dumpstermobileapp.View.MainActivity;

public class ControllerImpl {

    private final AppCompatActivity activity;
    private final ModelImpl model;

    public ControllerImpl(AppCompatActivity activity){
        this.activity = activity;
        this.model = new ModelImpl(this, activity);
    }

    /* VIEW -> MODEL */

    public void getToken(){
        this.restartInterface();
        this.model.getToken();
    }

    public void sendNewTrash(String type){
        this.model.sendNewTrash(type);
    }

    public void tryToConnectWithBluetooth(BluetoothAdapter btAdapter){
        this.model.tryToConnectWithBluetooth(btAdapter);
    }

    public void sendTrashSelected(String typeOfTrash){
        this.model.sendTrashSelected(typeOfTrash);
    }

    public void sendRequestOfMoreTime(){
        this.model.sendRequestOfMoreTime();
    }

    public void sendFinishDeposit(){
        this.model.sendFinishDeposit();
    }

    public void disconnectBluetooth(){
        this.model.disconnectBluetooth();
    }

    public void stopCountDown(){
        this.model.stopCountDown();
    }

    /* VIEW <- MODEL */

    public void showTimeIsOver(){
        ((MainActivity)this.activity).showTimeIsOver();
    }

    public void showGetTokenWentGood(){
        ((MainActivity)this.activity).showGetTokenWentGood();
    }

    public void showGetTokenWentBad(){
        ((MainActivity)this.activity).showGetTokenWentBad();
    }

    public void showBluetoothConnectionResult(String text){
        ((MainActivity)this.activity).showBluetoothConnectionResult(text);
    }

    public void showSelectionOfTypeOfTrash(){
        ((MainActivity)this.activity).showSelectionOfTypeOfTrash();
    }

    public void showHoldTime(){
        ((MainActivity)this.activity).showHoldTime();
    }

    public void showTimeLeftSuccess(int seconds){
        ((MainActivity)this.activity).showTimeLeftSuccess(seconds);
    }

    public void showTimeLeftFailed(){
        ((MainActivity)this.activity).showTimeLeftFailed();
    }

    public void showAddTimeSuccess(int seconds){
        ((MainActivity)this.activity).showAddTimeSuccess(seconds);
    }

    public void showAddTimeFailed(){
        ((MainActivity)this.activity).showAddTimeFailed();
    }

    public void showTimeout(){
        ((MainActivity)this.activity).showTimeout();
    }

    public void showCongratulation(){
        ((MainActivity)this.activity).showCongratulation();
    }

    public void showInternetRequestError() {
        ((MainActivity)this.activity).showInternetRequestError();
    }

    public void restartInterface(){
        ((MainActivity)this.activity).restartInterface();
    }

    /* MODEL -> MODEL */

    public long getTokenTimestamp(){
        return this.model.getTokenTimestamp();
    }

    public void setTokenTimestamp(long token){
        this.model.setTokenTimestamp(token);
    }
}
