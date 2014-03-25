LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/Scene/Title/TitleScene.cpp \
                   hellocpp/LBSocial.cpp \
                   hellocpp/LBFileUtils.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static


include $(BUILD_SHARED_LIBRARY)

$(call import-module,2d)
$(call import-module,audio/android)
$(call import-module,Box2D)
$(call import-module,editor-support/cocosbuilder)
