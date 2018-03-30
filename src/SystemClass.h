#pragma once

#define GLEW_STATIC

#pragma warning (disable : 4099)

#pragma comment(lib, "OPENGL32.lib")
#pragma comment(lib, "libfbxsdk-md.lib")

#pragma comment(lib, "winmm.lib")//時間計測用

#pragma comment(lib, "common.lib")
#pragma comment(lib, "OpenAL32.lib")

#pragma comment ( lib, "libogg_static.lib" )
#pragma comment ( lib, "libvorbis_static.lib" )
#pragma comment ( lib, "libvorbisfile_static.lib" )

#pragma comment(lib, "Effekseer.lib" )
#pragma comment(lib, "EffekseerRendererGL.lib" )
#pragma comment(lib, "EffekseerSoundAL.lib" )

#pragma comment(lib, "freetype.lib" )

#if defined(_DEBUG)
#pragma comment(lib, "glew32d.lib")
#pragma comment(lib, "glfw3d.lib")
#pragma comment(lib, "BulletDynamics_Debug.lib")
#pragma comment(lib, "BulletCollision_Debug.lib")
#pragma comment(lib, "LinearMath_Debug.lib")
#else
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "BulletDynamics.lib")
#pragma comment(lib, "BulletCollision.lib")
#pragma comment(lib, "LinearMath.lib")
#endif

#include<Windows.h>
#include<GLEW/glew.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include"MyApplication.h"


//アプリケーションの動作を管理
class SystemClass {
public:
	SystemClass();
	~SystemClass();
	bool Initialize();
	void Run();

private:
	bool CreateAppricationWindow(const char* windowName, int width, int height, bool fullScreen);
	bool InitializeOpenGL();
	bool InitializeApplication();
	
private:
	int screenWidth, screenHeight;
	bool isFullScreen;
	GLFWwindow* windowHandle;
	MyApplication* application;

	//FPS計測
	unsigned long cullentTime;
	unsigned long startTime;
	int framecount;
};