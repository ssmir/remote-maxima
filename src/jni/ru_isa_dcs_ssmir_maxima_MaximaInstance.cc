/*  Maxima JNI API

Copyright (c) 2009, Sergey Smirnov <sasmir (at) gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * The names of contributors may not be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

