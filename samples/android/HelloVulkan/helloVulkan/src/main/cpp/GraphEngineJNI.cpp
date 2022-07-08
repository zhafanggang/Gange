#include <jni.h>
#include <string>
#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include "GGRenderBase.h"

extern "C" {

#define LOG_TAG  "native-dev"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

JNICALL
void Java_com_signalinput_hellovulkan_VulkanDrawThread_vulkanInit(JNIEnv *env,jobject thiz,
    jobject surface,
    jint width, jint height,
    jobject assetManager) {
    Gange::RenderBase* render = new Gange::RenderBase();
    jclass clazz = (jclass)env->GetObjectClass(thiz);
    jfieldID fid = (jfieldID)env->GetFieldID(clazz,"renderAddress", "J");
    env->SetLongField(thiz,fid,(jlong)render);

    ANativeWindow* androidNativeWindow = ANativeWindow_fromSurface(env, surface);
    AAssetManager* androidNativeAssetManager = AAssetManager_fromJava(env, assetManager);
    render->setTarget(androidNativeWindow,androidNativeAssetManager,width,height);
    render->initialize();
    render->prepare();
}

JNICALL
void Java_com_signalinput_hellovulkan_VulkanDrawThread_vulkanDraw(JNIEnv *env, jobject thisObj) {
    jclass clazz = (jclass)env->GetObjectClass(thisObj);
    jfieldID fid = (jfieldID)env->GetFieldID(clazz,"renderAddress", "J");
    jlong renderAddress = (jlong)env->GetLongField(thisObj,fid);
    Gange::RenderBase* renderBase = (Gange::RenderBase*)renderAddress;
    renderBase->render();
}

JNICALL
void Java_com_signalinput_hellovulkan_VulkanDrawThread_vulkanDestroy(JNIEnv *env, jobject thisObj) {
    jclass clazz = (jclass)env->GetObjectClass(thisObj);
    jfieldID fid = (jfieldID)env->GetFieldID(clazz,"renderAddress", "J");
    jlong p = (jlong)env->GetLongField(thisObj,fid);
    Gange::RenderBase* renderBase = (Gange::RenderBase*)p;
    env->SetLongField(thisObj,fid,(jlong)0);
    delete renderBase;
    renderBase = nullptr;
}

JNICALL
void Java_com_signalinput_hellovulkan_VulkanDrawThread_touchEvent(JNIEnv *env, jobject thisObj,jint msgID,
                                                                  jfloat x, jfloat y){
    jclass clazz = (jclass)env->GetObjectClass(thisObj);
    jfieldID fid = (jfieldID)env->GetFieldID(clazz,"renderAddress", "J");
    jlong renderAddress = (jlong)env->GetLongField(thisObj,fid);
    Gange::RenderBase* renderBase = (Gange::RenderBase*)renderAddress;
    renderBase->touchEvent(msgID,x,y);
}

};