#include <stdio.h>
#include <jni.h>

#include "inlineHook.h"
#include <android/log.h>

#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

#define LOG_TAG "xyz"

#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,fmt, ##args)


const char *getClzName(
        JNIEnv *env,
        jclass clazz) {
    jclass findClzNameUtilClz = env->FindClass("com/xh/encodendk/util/FindClzNameUtil");
    jmethodID methodId = env->GetStaticMethodID(findClzNameUtilClz, "getClzName",
                                                "(Ljava/lang/Class;)Ljava/lang/String;");

    jstring jstring1 = static_cast<jstring>(env->CallStaticObjectMethod(findClzNameUtilClz,
                                                                        methodId, clazz));
    return env->GetStringUTFChars(jstring1, NULL);
}

static unsigned long find_database_of(char *soName)//获取libcocos2dlua.so内存基址
{
    char filename[32];
    char cmdline[256];
    sprintf(filename, "/proc/%d/maps", getpid());
    LOGD("filename = %s", filename);
    FILE *fp = fopen(filename, "r");
    unsigned long revalue = 0;
    if (fp) {
        while (fgets(cmdline, 256, fp)) //逐行读取
        {
            if (strstr(cmdline, soName) && strstr(cmdline, "r-xp"))//筛选
            {
                LOGD("cmdline = %s", cmdline);
                char *str = strstr(cmdline, "-");
                if (str) {
                    *str = '\0';
                    char num[32];
                    sprintf(num, "0x%s", cmdline);
                    revalue = strtoul(num, NULL, 0);
                    LOGD("revalue = %lu", revalue);
                    return revalue;
                }
            }
            memset(cmdline, 0, 256); //清零
        }
        fclose(fp);
    }
    return 0L;
}

const struct JNINativeInterface *jniNativeInterface;

jint (*old_RegisterNatives)(JNIEnv *env, jclass clazz, const JNINativeMethod *methods,
                            jint nMethods) = NULL;

jint new_RegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods, jint nMethods) {

    LOGD("注册%p  ,%p  ,%p  ,%d  ", env, clazz, methods, nMethods);
    LOGD("class name :%s", getClzName(env, clazz));
//    LOGD("address : 0x%s",env.get);
    unsigned long base = find_database_of("libmyencode.so");
    LOGD("base = %x ", base);
    for (int i = 0; i < nMethods; i++) {
        LOGD("name:%s  ,signature:%s  ,address:%p  ", methods[i].name, methods[i].signature,
             methods[i].fnPtr);
    }

    jint jint1 = old_RegisterNatives(env, clazz, methods, nMethods);
    LOGD("register result:%d", jint1);
    return jint1;
}

int hookRegisterNatives() {
    if (registerInlineHook((uint32_t) jniNativeInterface->RegisterNatives,
                           (uint32_t) new_RegisterNatives,
                           (uint32_t **) &old_RegisterNatives) !=
        ELE7EN_OK) {
        LOGD("registerInlineHook  error");
        return -1;
    }
    if (inlineHook((uint32_t) jniNativeInterface->RegisterNatives) != ELE7EN_OK) {
        LOGD("inlineHook  error");
        return -1;
    }

    return 0;
}

unsigned long func = NULL;

jstring (*old_getStringFromJNI)(void *env, void *jobject) = NULL;


jstring new_getStringFromJNI(JNIEnv *env, void *jobject) {
    jstring ret = old_getStringFromJNI(env, jobject);
    const char *charret = env->GetStringUTFChars(ret, NULL);
    LOGD("修改前的ret = %s", charret);
    return env->NewStringUTF("i is new string");
}


int hookCalcFunc() {
    LOGD("func = %x", func);
    if (registerInlineHook((uint32_t) func, (uint32_t) new_getStringFromJNI,
                           (uint32_t **) &old_getStringFromJNI) !=
        ELE7EN_OK) {
        return -1;
    }
    if (inlineHook((uint32_t) func) != ELE7EN_OK) {
        return -1;
    }
    LOGD("hookCalcFunc-------");
    return 0;
}

int unHookCalcFunc() {
    if (inlineUnHook((uint32_t) func) != ELE7EN_OK) {
        return -1;
    }
    return 0;
}


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    jniNativeInterface = env->functions;

//    unsigned long base = find_database_of("libmyencode.so");
//    LOGD("base = %x ", base);
//
//    if (base > 0L) {
//        func = base + 0x379B4 + 1;
//        LOGD("FUNC = %x", func);
//        hookCalcFunc();
//    }
    hookRegisterNatives();
    return JNI_VERSION_1_6;
}

