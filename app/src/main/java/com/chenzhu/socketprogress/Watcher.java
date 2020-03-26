package com.chenzhu.socketprogress;

public class Watcher {

    static {
        System.loadLibrary("native-lib");
    }


    public native void createWatcher(String user_Id);

    public native void connectMonitor();

}
