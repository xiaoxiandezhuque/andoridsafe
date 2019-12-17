#include <jni.h>
#include <string>
#include <android/log.h>
#include <fstream>
#include <thread>
#include <unistd.h>


// Android log function wrappers
static const char *kTAG = "jni-log";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

using namespace std;

struct TickContext {
    JavaVM *javaVm;
    jclass jniHelperClz;
    jobject jniHelperObj;
    jclass mainActivityClz;
    jobject mainActivityObj;
    pthread_mutex_t lock;
    int done;
} g_ctx;


void queryTracerPid(int myPid) {
//    int myPid = (int)*args;
    const char *s = "/proc/";
    const char *s2 = "/status";
    const char *bufPid = new char[strlen(s) + sizeof(myPid) + strlen(s2) + 1];
    sprintf(const_cast<char *>(bufPid), "%s%d%s", s, myPid, s2);

    while (true) {
        sleep(1);

        LOGE("%s", bufPid);
        ifstream pidFile(bufPid);
        string mycontext;
        while (getline(pidFile, mycontext)) {
            if (mycontext.compare(0, 9, "TracerPid", 0, 9) == 0) {
                mycontext = mycontext.substr(10, mycontext.length());
                break;
            }
        }
        int tracerPid = stoi(mycontext);
        LOGE("find TracerPid  is = %d", tracerPid);
        pidFile.close();
        if (tracerPid > 0) {
            LOGE("begin  dbug");
            exit(1);
        }
    }

}

jstring getStringFromJNI(JNIEnv *env, jclass) {

    return env->NewStringUTF("hello  from  jni   is ");
}

static JNINativeMethod method[] = {
        {"stringFromJNI", "()Ljava/lang/String;", (void *) getStringFromJNI}
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {


    LOGE("进入jni_onload");
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOGE("cuowu?");
        return JNI_ERR;
    }

    LOGE("----begin-----");
//    jclass clz = env->FindClass("com/xh/encodendk/util/JniHandler");
//    LOGE("find  clz");
//    g_ctx.jniHelperClz = static_cast<jclass>(env->NewGlobalRef(clz));
//
//    jmethodID jniHelper = env->GetMethodID(g_ctx.jniHelperClz, "<init>", "()V");
//
//    jobject handler = env->NewObject(g_ctx.jniHelperClz, jniHelper);
//
//    g_ctx.jniHelperObj = env->NewGlobalRef(handler);

//    jmethodID versionFunc = env->GetStaticMethodID(
//            clz,
//            "getBuildVersion", "()Ljava/lang/String;");
//    LOGE("method");
//    jstring buildVersion = static_cast<jstring>(env->CallStaticObjectMethod(clz,
//                                                                            versionFunc));
//
//    LOGE("call method");
//    const char *version = env->GetStringUTFChars(buildVersion, NULL);
//    LOGE("app version %s", version);



    jclass appClz = env->FindClass("com/xh/encodendk/App");
    jfieldID contextField = env->GetStaticFieldID(appClz, "instance", "Lcom/xh/encodendk/App;");

    jobject context = env->GetStaticObjectField(appClz, contextField);

    jclass contextWarpperClz = env->FindClass("android/content/ContextWrapper");
    jmethodID getPagNameMethod = env->GetMethodID(contextWarpperClz, "getPackageName",
                                                  "()Ljava/lang/String;");

    jstring pagName = static_cast<jstring>(env->CallObjectMethod(context, getPagNameMethod));
    const char *name = env->GetStringUTFChars(pagName, NULL);
    LOGE("pagname = %s", name);

    if (strcmp(name, "com.xh.encodendk") != 0) {
        LOGE("pagName  check  error");
        return JNI_ERR;
    }

    jclass processClz = env->FindClass("android/os/Process");
    jmethodID myPidMethod = env->GetStaticMethodID(processClz, "myPid", "()I");
    jint myPid = env->CallStaticIntMethod(processClz, myPidMethod);

//    char str[20];
//    sprintf(str,"%d",myPid);
//    printf("%d",myPid);

//获取包名
    const char *s = "/proc/";
    const char *s1 = "/cmdline";
    const char *buf = new char[strlen(s) + sizeof(myPid) + strlen(s1) + 1];
    sprintf(const_cast<char *>(buf), "%s%d%s", s, myPid, s1);

    LOGE("%s", buf);

    ifstream myFile(buf);
//
    if (!myFile) {
        LOGE("file open error ");
        return JNI_ERR;
    }
    string temp;
    while (getline(myFile, temp)) {
//        LOGE("-----read-----");
//        LOGE("%s%s", temp.c_str(), "----");
    }
    myFile.close();
    if (strcmp(temp.c_str(), "com.xh.encodendk") != 0) {
        LOGE("pag  error");
        return JNI_ERR;
    }

//    开线程去获取TracerPid
//    pthread_t pthread;
//    pthread_create(&pthread, NULL, , &myPid);
//    thread task1(&queryTracerPid, myPid);
////    task1.detach();
    LOGE("-----end-------");

    jclass encodeUtilClz = env->FindClass("com/xh/encodendk/util/EncodeUtil");
    if (env->RegisterNatives(encodeUtilClz, method, sizeof(method) / sizeof(method[0])) < 0) {
        return JNI_ERR;
    }


    return JNI_VERSION_1_6;
}





//extern "C" JNIEXPORT jstring JNICALL
//Java_com_xh_encodendk_util_EncodeUtil_stringFromJNI(
//        JNIEnv *env,
//        jobject /* this */) {
//
//    return env->NewStringUTF("hello from jni");
//}


//extern "C" JNIEXPORT void JNICALL
//Java_com_xh_encodendk_util_EncodeUtil_startTicks(
//        JNIEnv *env,
//        jclass* clazz) {
//
//}
//
//extern "C" JNIEXPORT void JNICALL
//Java_com_xh_encodendk_util_EncodeUtil_StopTicks(
//        JNIEnv *env,
//        jclass clazz) {
//
//}