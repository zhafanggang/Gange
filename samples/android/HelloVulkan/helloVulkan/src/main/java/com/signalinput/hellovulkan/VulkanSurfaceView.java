package com.signalinput.hellovulkan;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.MotionEvent;

public class VulkanSurfaceView extends SurfaceView implements SurfaceHolder.Callback {
    public static final String TAG = "VulkanSurfaceView";
    float x=0,y=0;
    private boolean isCanTouch = false;
    public static final float SCALE_MAX = 1.1f; //最大的缩放比例
    private static final float SCALE_MIN = 0.9f;

    private double oldDist = 0;
    private double moveDist = 0;
    private float downX1 = 0;
    private float downX2 = 0;
    private float downY1 = 0;
    private float downY2 = 0;

    private VulkanDrawThread vulkanDrawThread = null;
    private AssetManager assetManager = null;
    public VulkanSurfaceView(Context context, AssetManager inAssetManager) {
        super(context);
        assetManager = inAssetManager;
        getHolder().addCallback(this);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        vulkanDrawThread = new VulkanDrawThread(getHolder(), assetManager);
        vulkanDrawThread.setRunning(true);
        vulkanDrawThread.start();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        boolean retry = true;
        vulkanDrawThread.setRunning(false);
        while (retry) {
            try {
                if (vulkanDrawThread.isAlive()){
                    vulkanDrawThread.join();
                }
                retry = false;
            } catch (InterruptedException e) {

            }
        }
    }
    private double spacing(MotionEvent event) {
        if (event.getPointerCount() == 2) {
            float x = event.getX(0) - event.getX(1);
            float y = event.getY(0) - event.getY(1);
            return Math.sqrt(x * x + y * y);
        } else {
            return 0;
        }
    }

    public boolean onTouchEvent(MotionEvent event) {
        Log.d(TAG, "onTouch isCanTouch= " + isCanTouch);
        int pointerCount = event.getPointerCount();
        switch (event.getAction() & MotionEvent.ACTION_MASK) {
            case MotionEvent.ACTION_UP:
            {
                if (pointerCount == 1)
                {
                    vulkanDrawThread.touchEvent(2,x,y);
                }
                else if (pointerCount == 2)
                {
                    downX1 = 0;
                    downY1 = 0;
                    downX2 = 0;
                    downY2 = 0;
                    vulkanDrawThread.touchEvent(6,0,0);
                }
                break;
            }
            case MotionEvent.ACTION_MOVE:
            {
                if (pointerCount == 2) {
                    float x1 = event.getX(0);
                    float x2 = event.getX(1);
                    float y1 = event.getY(0);
                    float y2 = event.getY(1);

                    double changeX1 = x1 - downX1;
                    double changeX2 = x2 - downX2;
                    double changeY1 = y1 - downY1;
                    double changeY2 = y2 - downY2;

                    if (getScaleX() > 1) { //滑动
                        float lessX = (float) ((changeX1) / 2 + (changeX2) / 2);
                        float lessY = (float) ((changeY1) / 2 + (changeY2) / 2);
                        //setSelfPivot(-lessX, -lessY);
                        // Log.d(TAG, "此时为滑动");
                    }
                    //缩放处理
                    moveDist = spacing(event);
                    double space = moveDist - oldDist;
                    float scale = (float) (getScaleX() + space / getWidth());
                    if (scale < SCALE_MIN) {
                        vulkanDrawThread.touchEvent(4,SCALE_MIN,0);
                    } else if (scale > SCALE_MAX) {
                        vulkanDrawThread.touchEvent(4,SCALE_MAX,0);
                    } else {
                        vulkanDrawThread.touchEvent(4,scale,0);
                    }
                }
                else
                {
                    x = event.getX();
                    y = event.getY();

                    vulkanDrawThread.touchEvent(1,x,y);
                }
                break;
            }

            case MotionEvent.ACTION_DOWN:
            {
                if (pointerCount == 1)
                {
                    x=event.getX();
                    y=event.getY();
                    vulkanDrawThread.touchEvent(0,x,y);
                }
                else if (pointerCount == 2)
                {
                    float x1 = event.getX(0);
                    float x2 = event.getX(1);
                    float y1 = event.getY(0);
                    float y2 = event.getY(1);

                    x = (x1+x2)/2;
                    y = (y1+y2)/2;

                    vulkanDrawThread.touchEvent(5,x,y);
                }
            }

            case MotionEvent.ACTION_POINTER_DOWN:
            {
                if (pointerCount == 2) {
                    downX1 = event.getX(0);
                    downX2 = event.getX(1);
                    downY1 = event.getY(0);
                    downY2 = event.getY(1);
                    Log.d(TAG, "ACTION_POINTER_DOWN 双指按下 downX1=" + downX1 + " downX2="
                            + downX2 + "  downY1=" + downY1 + " downY2=" + downY2);
                    oldDist = spacing(event); //两点按下时的距离
                }
                break;
            }

            case MotionEvent.ACTION_POINTER_UP:
                Log.d(TAG, "ACTION_POINTER_UP");
                break;
            default:
                break;
        }
        return true;
    }
}
