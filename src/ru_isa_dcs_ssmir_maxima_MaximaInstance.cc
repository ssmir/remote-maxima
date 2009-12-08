/*  Maxima JNI API
    Copyright (C) 2007-2009 Sergey Smirnov <sasmir (at) gmail.com>
    
    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file BOOST_LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#include "ru_isa_dcs_ssmir_maxima_MaximaInstance.h"
#include <MaximaAPI/MaximaInstance.h>

MaximaAPI::MaximaInstance *getMaximaInstance(JNIEnv *env, jobject obj) {
    jfieldID id = env->GetFieldID(env->GetObjectClass(obj), "_ptr", "J");
    return (id == NULL) ? NULL : ((MaximaAPI::MaximaInstance *)env->GetLongField(obj, id));
}

std::string jstringToString(JNIEnv *env, jstring str) {
    const char *strUTF = env->GetStringUTFChars(str, 0);
    if (strUTF == NULL)
        return "";
    std::string result(strUTF);
    env->ReleaseStringUTFChars(str, strUTF);
    return result;
}

void setException(JNIEnv *env, const char *msg) {
    if (env->ThrowNew(env->FindClass("ru/isa/dcs/ssmir/maxima/MaximaException"), msg)) {
        printf("MaximaInstanceJNI failed to throw exception with message: %s\n", msg);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_ru_isa_dcs_ssmir_maxima_MaximaInstance_initialize(JNIEnv *env, jobject obj, jstring maximaPath, jstring utilsPath) {
    try {
        MaximaAPI::MaximaInstance *instance = new MaximaAPI::MaximaInstance(
            jstringToString(env, maximaPath), ".", jstringToString(env, utilsPath));
        env->SetLongField(obj, env->GetFieldID(env->GetObjectClass(obj), "_ptr", "J"),
            (jlong)instance);
    } catch (const std::exception &e) {
        setException(env, e.what());
        return;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_ru_isa_dcs_ssmir_maxima_MaximaInstance_destroy(JNIEnv *env, jobject obj) {
    try {
        delete getMaximaInstance(env, obj);
    } catch (const std::exception &e) {
        setException(env, e.what());
        return;
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_ru_isa_dcs_ssmir_maxima_MaximaInstance_executeCommand(JNIEnv *env, jobject obj, jstring command) {
    try {
        return env->NewStringUTF(
            getMaximaInstance(env, obj)->executeCommand(jstringToString(env, command)).c_str());
    } catch (const std::exception &e) {
        setException(env, e.what());
        return env->NewStringUTF("");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_ru_isa_dcs_ssmir_maxima_MaximaInstance_interruptMaxima(JNIEnv *env, jobject obj) {
    try {
        getMaximaInstance(env, obj)->interruptMaxima();
    } catch (const std::exception &e) {
        setException(env, e.what());
        return;
    }
}

