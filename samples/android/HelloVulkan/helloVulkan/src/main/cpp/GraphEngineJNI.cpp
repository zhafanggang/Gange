#include <jni.h>
#include <string>
#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "GGRenderBase.h"

extern "C" {

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
    render->setTarget(androidNativeWindow,androidNativeAssetManager);
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

}