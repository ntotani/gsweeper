#include "Common/LBAnalytics.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>

USING_NS_CC;

void LBAnalytics::logEvent(const char* event, map<string, string> param) {
    /*
    JniMethodInfo jmi;
    if (!JniHelper::getStaticMethodInfo(jmi, "org.cocos2dx.cpp.Cocos2dxActivity", "showGamePlatform", "()V")) {
        return;
    }
    jmi.env->CallStaticVoidMethod(jmi.classID, jmi.methodID);
    jmi.env->DeleteLocalRef(jmi.classID);
    */
}

void LBAnalytics::startEvent(const char* event, map<string, string> param) {
}

void LBAnalytics::endEvent(const char* event, map<string, string> param) {
}

