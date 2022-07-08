package com.signalinput.hellovulkan;

import android.content.res.AssetManager;
import android.view.Surface;
import android.view.SurfaceHolder;

public class VulkanDrawThread extends Thread{
    static {
        System.loadLibrary("hellovulkan");
    }
    private long renderAddress;
    private boolean runFlag = false;
    private SurfaceHolder surfaceHolder = null;
    private AssetManager assetManager = null;

    VulkanDrawThread(SurfaceHolder surfaceHolder, AssetManager inAssetManager){
        this.surfaceHolder = surfaceHolder;
        this.assetManager = inAssetManager;
        setPriority(MAX_PRIORITY);
    }

    public void setRunning(boolean run) {
        synchronized (this) {
            runFlag = run;
        }
    }

    @Override
    public void run() {
        // TODO: 最简单的实现，当游戏被最小化时，上下文崩溃，当它开始时，一切都被重新创建
        Surface surface = this.surfaceHolder.getSurface();
        int width = this.surfaceHolder.getSurfaceFrame().width();
        int height = this.surfaceHolder.getSurfaceFrame().height();
        vulkanInit(surface, width, height, this.assetManager);

        while (runFlag) {
            vulkanDraw();
        }

        vulkanDestroy();
    }

    public native void vulkanInit(Object surface, int width, int height, Object assetManager);
    public native void vulkanDraw();
    public native void vulkanDestroy();
    public native void touchEvent(int msg,float x,float y);
}