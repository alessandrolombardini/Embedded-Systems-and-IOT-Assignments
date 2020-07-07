package com.example.dumpstermobileapp.Model;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.example.dumpstermobileapp.Controller.ControllerImpl;
import com.example.dumpstermobileapp.Model.netutils.Http;
import android.os.CountDownTimer;
import org.json.JSONException;
import org.json.JSONObject;
import java.net.HttpURLConnection;
import java.sql.Timestamp;
import java.util.Objects;


public class HTTPManager {

    private static final String HOST = "http://bb0acec9.ngrok.io";        /* SERVER con API REST (da modificare in funzione di ngrok) */
    private final ControllerImpl controller;
    private final AppCompatActivity activity;
    private long token;
    private long timestamp;
    private int timeAllowed;
    private CountDownTimer countdown;

    public HTTPManager(ControllerImpl controller, AppCompatActivity activity) {
        this.controller = controller;
        this.activity = activity;
    }

    public void getToken() {
        //final String url = "http://" + HOST + "/assignments-sistemi-embedded-e-iot/Assignment%203/Dumpster%20Service/call_to_get_token.php";
        final String url = HOST + "/api/getToken/";
        Http.post(url, url.getBytes(),response -> {
            try {
                if(response.code() == HttpURLConnection.HTTP_OK) {
                    final String content = response.contentAsString();
                    final JSONObject obj = new JSONObject(content);
                    if (obj.getBoolean("available")) {
                        token = obj.getLong("token");
                        timeAllowed = obj.getInt("time");
                        timestamp = new Timestamp(System.currentTimeMillis()).getTime();
                        this.controller.showGetTokenWentGood();

                        countdown = new CountDownTimer(timeAllowed * 1000, 1000){

                            @Override
                            public void onTick(long millisUntilFinished) {

                            }

                            @Override
                            public void onFinish() {
                                controller.disconnectBluetooth();
                                controller.showTimeIsOver();
                                this.cancel();
                            }
                        };
                        countdown.start();

                    } else {
                        this.controller.showGetTokenWentBad();
                    }
                }
            } catch (Exception e) {
                this.controller.showInternetRequestError();
            }
        });
    }

    public void sendNewTrash(String type) {
        //final String url = "http://" + HOST + "/assignments-sistemi-embedded-e-iot/Assignment%203/Dumpster%20Service/call_to_add_trash.php";
        final String url = HOST + "/api/finishDeposit";
        try {
            String content = new JSONObject().put("type", type)
                                             .toString();
            Log.d("HTTP", content);
            Http.post(url, content.getBytes(), response -> {  });
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public boolean checkValidToken() {
        final long timestampNow = new Timestamp(System.currentTimeMillis()).getTime();
        if(timestampNow - this.timestamp > this.timeAllowed * 1000){
            return false;
        }
        return true;
    }

    public long getTokenTimestamp(){
        return this.token;
    }
    public void setTokenTimestamp(long token){
        this.token = token;
    }

    public void stopCountDown(){
        this.countdown.cancel();
    }

}
