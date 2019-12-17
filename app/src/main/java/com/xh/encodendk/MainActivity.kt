package com.xh.encodendk

import android.os.Bundle
import android.os.Process
import android.widget.TextView
import androidx.annotation.Keep
import androidx.appcompat.app.AppCompatActivity
import com.blankj.utilcode.util.LogUtils
import com.blankj.utilcode.util.Utils
import com.xh.encodendk.util.EncodeUtil


class MainActivity : AppCompatActivity() {
    internal var hour = 0
    internal var minute = 0
    internal var second = 0
    internal var tickView: TextView? = null




    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Utils.init(this)
        setContentView(R.layout.activity_main)
        tickView = findViewById(R.id.tickView) as TextView
        tickView?.setOnClickListener {
            LogUtils.e("aaa")
        }
        LogUtils.e(Process.myPid())

    }

    public override fun onResume() {
        super.onResume()
        second = 0
        minute = second
        hour = minute
        (findViewById(R.id.hellojniMsg) as TextView).text = EncodeUtil.stringFromJNI()

    }

    public override fun onPause() {
        super.onPause()

    }

    /*
     * A function calling from JNI to update current timer
     */
    @Keep
    private fun updateTimer() {
        ++second
        if (second >= 60) {
            ++minute
            second -= 60
            if (minute >= 60) {
                ++hour
                minute -= 60
            }
        }
        runOnUiThread {
            val ticks = "" + this@MainActivity.hour + ":" +
                    this@MainActivity.minute + ":" +
                    this@MainActivity.second
            this@MainActivity.tickView?.text = ticks
        }
    }

    companion object{
        init {
            System.loadLibrary("two")
            System.loadLibrary("myencode")

        }
    }

}
