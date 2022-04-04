#ifndef _GG_WINDOW_H_
#define _GG_WINDOW_H_

#include <Windows.h>
#include <iostream>
#include "GGRenderBase.h"

namespace demovulkan {

	Gange::GGRenderBase *mRender = NULL;
	class DBWindow
	{
	protected:
		int         mWidth;
		int         mHeight;
		HINSTANCE   mHInstance;
		HWND		mWindow;
		 

	public:
		explicit DBWindow(int width, int height)
		{
			mWidth = width;
			mHeight = height;
			mHInstance = GetModuleHandle(0);
			WNDCLASSEX wndClass;
			wndClass.cbSize = sizeof(WNDCLASSEX);
			wndClass.style = CS_HREDRAW | CS_VREDRAW;
			wndClass.lpfnWndProc = wndProc;
			wndClass.cbClsExtra = 0;
			wndClass.cbWndExtra = 0;
			wndClass.hInstance = mHInstance;
			wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
			wndClass.lpszMenuName = NULL;
			wndClass.lpszClassName = "Gange";
			wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

			RegisterClassEx(&wndClass);

			DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			RECT windowRect;
			windowRect.left = 0L;
			windowRect.top = 0L;
			windowRect.right = (long)mWidth;
			windowRect.bottom = (long)mHeight;

			lastTimestamp = std::chrono::high_resolution_clock::now();

			AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

			std::string windowTitle = "Drakensberg8";
			mWindow = CreateWindowEx(0, "Gange", windowTitle.c_str(), dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0,
				0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL,
				mHInstance, NULL);

			ShowWindow(mWindow, SW_SHOW);
			SetForegroundWindow(mWindow);
			SetFocus(mWindow);

			mRender = new Gange::GGRenderBase();

			mRender->initialize();
			mRender->setTarget(mHInstance, mWindow);
			mRender->prepare();
			renderLoop();
		}

		~DBWindow()
		{
			delete (mRender);
		}

		void renderLoop()
		{
			lastTimestamp = std::chrono::high_resolution_clock::now();
#if defined(_WIN32)
			MSG msg;
			bool quitMessageReceived = false;
			while (!quitMessageReceived) {
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					if (msg.message  == WM_QUIT ) {
						quitMessageReceived = true;
						break;
					}
				}
				if (!IsIconic(mWindow)) {
					nextFrame();
				}
			}
#endif
		}

		void nextFrame()
		{
			auto tStart = std::chrono::high_resolution_clock::now();
			mRender->render();
			frameCounter++;
			auto tEnd = std::chrono::high_resolution_clock::now();
			auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
			float fpsTimer = (float)(std::chrono::duration<double, std::milli>(tEnd - lastTimestamp).count());

			if (fpsTimer > 1000.0f)
			{
				uint32_t lastFPS = static_cast<uint32_t>((float)frameCounter * (1000.0f / fpsTimer));
				std::cout << "FPS:" << lastFPS << std::endl;
				frameCounter = 0;
				lastTimestamp = tEnd;
			}
		}

		static LRESULT CALLBACK  wndProc(HWND hWnd, UINT msgId, WPARAM wParam, LPARAM lParam)
		{
			if (msgId == WM_CLOSE)
			{
				DestroyWindow(hWnd);
				PostQuitMessage(0);
			}
			else if (mRender != NULL) {
				mRender->handleMessages(hWnd, msgId, wParam, lParam);
			}
			return (DefWindowProc(hWnd, msgId, wParam, lParam));
			
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp;
		size_t frameCounter = 0;
	};

}  // namespace demovulkan

#endif  //
