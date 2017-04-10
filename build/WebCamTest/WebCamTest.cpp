// WebCamTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../WebCam.h"

static bool enumFunc(void *pItem, char *pDeviceName)
{
	printf("%s\n", pDeviceName);
	return true;
}

int main(int argc, char* argv[])
{
	WebCam camera;
	camera.EnumDevices(enumFunc, NULL);
	return 0;
}

