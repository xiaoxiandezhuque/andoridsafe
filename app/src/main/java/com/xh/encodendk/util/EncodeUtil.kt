package com.xh.encodendk.util

object EncodeUtil {

    init {
        System.loadLibrary("myencode")

    }

    external fun stringFromJNI(): String

    external fun startTicks()

    external fun StopTicks()

}