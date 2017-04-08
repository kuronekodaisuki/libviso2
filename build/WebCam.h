#pragma once

#include <opencv2/videoio.hpp>

#ifdef _MSC_VER
#include <windows.h>
#include <dshow.h>
#define HAVE_MSMF
#define HAVE_VFW
#pragma warning(disable: 4996)
#endif

using namespace std;

typedef bool(*PENUMDEVICE)(void *pItem, char *pDeviceName);

class WebCam : public cv::VideoCapture
{
public:
	// pCallbackがNULLの場合、カメラの台数だけを返す
	// pCallbackが設定されていたらコールバックでカメラ名を返す
	// pItemはコールバック関数で渡される
	static int EnumDevices(PENUMDEVICE pCallback, void *pItem);

	/*
	Usage:

	// enum callback function
	static int enumCamera(void* pItem, char *pName)
	{
	printf("%s\n", pName);
	return 0;
	}
	int main()
	{
	int cameraCount;
	cameraCount = Camera::EnumDevices((PENUMDEVICE)&enumCamera, NULL);
	}
	*/

	//bool open(const string& filename, int mode);

public:
	WebCam();
	WebCam(const string& filename);
	void SetName(const char *name) { strncpy(this->name, name, 80); }
	char *GetName() { return name; }

protected:
	char name[80];
};
