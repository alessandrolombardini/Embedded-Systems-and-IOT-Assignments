package com.example.dumpstermobileapp.Model.btlib;

public interface ExtendedRunnable extends Runnable {
    void write(byte[] bytes);
    void cancel();
}
