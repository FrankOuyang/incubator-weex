/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

#include <jni.h>
#include <IPC/IPCArguments.h>
#include <IPC/Serializing/IPCSerializer.h>
#include <IPC/IPCString.h>
#include <IPC/IPCByteArray.h>
#include <malloc.h>
#include "scoped_jstring.h"
#include <iostream>
#include <sstream>
#include <string>

namespace WeexCore {

static inline std::string jString2Str(JNIEnv *env, const jstring &jstr) {
  char *rtn = NULL;
  jclass clsstring = env->FindClass("java/lang/String");
  jstring strencode = env->NewStringUTF("GB2312");
  jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
  jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
  jsize alen = env->GetArrayLength(barr);
  jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
  if (alen > 0) {
    rtn = (char *) malloc(alen + 1);
    memcpy(rtn, ba, alen);
    rtn[alen] = 0;
  }
  env->ReleaseByteArrayElements(barr, ba, 0);

  env->DeleteLocalRef(clsstring);
  env->DeleteLocalRef(strencode);
  env->DeleteLocalRef(barr);

  std::string stemp(rtn);
  free(rtn);
  return stemp;
}

static inline std::string jString2StrFast(JNIEnv *env, const jstring &jstr){
  const char *nativeString = env->GetStringUTFChars(jstr, JNI_FALSE);
  return std::string(nativeString);
}

static std::string jByteArray2Str(JNIEnv *env, jbyteArray barr) {
  char *rtn = NULL;
  jsize alen = env->GetArrayLength(barr);
  jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
  if (alen > 0) {
    rtn = (char *) malloc(alen + 1);
    memcpy(rtn, ba, alen);
    rtn[alen] = 0;
  } else {
    return "";
  }
  env->ReleaseByteArrayElements(barr, ba, 0);

  std::string stemp(rtn);
  free(rtn);
  return stemp;
}

static inline jbyteArray newJByteArray(JNIEnv *env, const char* pat) {
  jbyteArray jarray = nullptr;
  if (pat == nullptr)
    return jarray;
  int byteSize = strlen(pat);
  jbyte *jb =  (jbyte*) pat;
  jarray = env->NewByteArray(byteSize);
  env->SetByteArrayRegion(jarray, 0, byteSize, jb);
  return jarray;
}

static inline jstring newJString(JNIEnv* env, const char* pat) {
  jstring jstr = nullptr;
  if (pat == nullptr)
    return jstr;
  return env->NewStringUTF(pat);
}

static inline char* getArumentAsCStr(IPCArguments *arguments, int argument) {
    char* ret = nullptr;

    if (argument >= arguments->getCount())
      return nullptr;
    if (arguments->getType(argument) == IPCType::BYTEARRAY) {
      const IPCByteArray *ipcBA = arguments->getByteArray(argument);
      int strLen = ipcBA->length;
      ret = new char[strLen+1];
      for (int i = 0; i < strLen; ++i) {
        ret[i] = ipcBA->content[i];
      }
      ret[strLen] = '\0';
    }

    return ret;
}

static inline jstring getArgumentAsJString(JNIEnv *env, IPCArguments *arguments, int argument) {
  jstring ret = nullptr;
  if (arguments->getType(argument) == IPCType::STRING) {
    const IPCString *s = arguments->getString(argument);
    ret = env->NewString(s->content, s->length);
  }
  return ret;
}

static inline int getArgumentAsInt32(JNIEnv* env, IPCArguments* arguments, int argument) {
  int ret = 0;
  if (arguments->getType(argument) == IPCType::INT32) {
    const int32_t type = arguments->get<int32_t>(argument);
    ret = type;
  }
  return ret;
}

static inline void addString(JNIEnv *env, IPCSerializer *serializer, jstring str) {
  ScopedJString scopedString(env, str);
  const uint16_t *chars = scopedString.getChars();
  size_t charsLength = scopedString.getCharsLength();
  serializer->add(chars, charsLength);
}

static inline void addJSONString(JNIEnv *env, IPCSerializer *serializer, jstring str) {
  ScopedJString scopedString(env, str);
  const uint16_t *chars = scopedString.getChars();
  size_t charsLength = scopedString.getCharsLength();
  serializer->addJSON(chars, charsLength);
}
}
#endif //_STRING_UTILS_H_
