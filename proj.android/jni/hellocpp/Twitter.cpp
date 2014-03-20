#include "Common/Twitter.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>

USING_NS_CC;

void Twitter::tweet(const char *message, const char *ssFilePath) {
    JniMethodInfo jmi;
    if (!JniHelper::getStaticMethodInfo(jmi, "org.cocos2dx.cpp.Cocos2dxActivity", "tweet", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        return;
    }
    jstring messageArg = jmi.env->NewStringUTF(message);
    jstring filePathArg = jmi.env->NewStringUTF(ssFilePath);
    jmi.env->CallStaticVoidMethod(jmi.classID, jmi.methodID, messageArg, filePathArg);
    jmi.env->DeleteLocalRef(messageArg);
    jmi.env->DeleteLocalRef(filePathArg);
    jmi.env->DeleteLocalRef(jmi.classID);
}
