package com.xh.encodendk.util;

public class EncodeUtil {
//    static  {
//        System.loadLibrary("myencode");
//    }

    public static native String stringFromJNI();
    public static native void startTicks();
    public static native void StopTicks();

}
