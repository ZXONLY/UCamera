package com.zxx.camera.Monitor;

import java.util.List;

public abstract class MonitorBase<T> {

    protected String mMonitorName;
    public abstract T sample();

    //public static List<MonitorBase> initBenchmarkMonitor(){return null;}
}
