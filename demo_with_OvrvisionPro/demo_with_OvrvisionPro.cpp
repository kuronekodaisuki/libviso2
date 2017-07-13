// demo_with_OvrvisionPro.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

/*
Copyright 2012. All rights reserved.
Institute of Measurement and Control Systems
Karlsruhe Institute of Technology, Germany

This file is part of libviso2.
Authors: Andreas Geiger

libviso2 is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or any later version.

libviso2 is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
libviso2; if not, write to the Free Software Foundation, Inc., 51 Franklin
Street, Fifth Floor, Boston, MA 02110-1301, USA
*/

/*
Modified by Horoshi Matsuoka using OpenCV instead of libpng
*/

/*
Documented C++ sample code of stereo visual odometry (modify to your needs)
To run this demonstration, download the Karlsruhe dataset sequence
'2010_03_09_drive_0019' from: www.cvlibs.net!
Usage: ./viso2 path/to/sequence/2010_03_09_drive_0019
*/

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

#include <viso_stereo.h>

#pragma warning(push)
#pragma warning (disable: 4819)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#pragma warning(pop)

#include <ovrvision_pro.h>

using namespace std;

int main(int argc, char** argv)
{
	OVR::OvrvisionPro ovrvision;	// OvrvisionPro camera
	if (ovrvision.CheckGPU())
	{
		// Open with OpenGL sharing mode
		if (ovrvision.Open(0, OVR::OV_CAMHD_FULL, 0, NULL, true) == 0)
			puts("Can't open OvrvisionPro");
		printf_s("Focal point: %f\n", ovrvision.GetCamFocalPoint());

		ovrvision.SetCameraExposure(2000);

		int height = ovrvision.GetCamHeight();
		int width = ovrvision.GetCamWidth();
		int bytePerLine;
		OVR::ROI roi = { 0, 0, width, height };
		cv::Mat lRGBA, rRGBA;
		lRGBA.create(height, width, CV_8UC4);
		rRGBA.create(height, width, CV_8UC4);

		height /= 2;
		width /= 2;
		cv::Mat left, right;
		left.create(height, width, CV_8UC1);
		right.create(height, width, CV_8UC1);
		bytePerLine = left.step;

		// set most important visual odometry parameters
		// for a full parameter list, look at: viso_stereo.h
		VisualOdometryStereo::parameters param;

		// calibration parameters for sequence 2010_03_09_drive_0019 
		param.calib.f = ovrvision.GetCamFocalPoint();	// focal length in pixels
		param.calib.cu = width / 2;						// principal point (u-coordinate) in pixels
		param.calib.cv = height / 2;					// principal point (v-coordinate) in pixels
		param.base = 0.06;								// baseline in meters

		VisualOdometryStereo viso(param);				// init visual odometry

		// current pose (this matrix transforms a point from the current
		// frame's camera coordinates to the first frame's camera coordinates)
		Matrix pose = Matrix::eye(4);
		bool stereo = false;
		int i = 0;
		bool write = false;
		bool rgba = false;
		char buffer[30];
		int interval = 30;

		for (bool loop = true; loop; i++)
		{
			ovrvision.Capture(OVR::Camqt::OV_CAMQT_DMSRMP);

			//ovrvision.GetStereoImageBGRA(left.data, right.data, roi);
			ovrvision.GrayscaleHalf(left.data, right.data);
			
			cv::imshow("Left", left);
			cv::imshow("Right", right);

			char left_name[MAX_PATH], right_name[MAX_PATH];
			sprintf(left_name, "I1_%06d.jpg", i);
			sprintf(right_name, "I2_%06d.jpg", i);

			int value; // for gain / exposure
			int code = cv::waitKey(interval);
			switch (code)
			{
			case 'q':
				loop = false;
				break;

			case 's':
				stereo = !stereo;
				break;

			case 'c':
				write = !write;
				break;

			case 'C':
				ovrvision.GetStereoImageBGRA(lRGBA.data, rRGBA.data, roi);
				cv::imshow("L", lRGBA);
				cv::imshow("R", rRGBA);
				break;

			case 'l':
				ovrvision.GetCamImageBGRA(lRGBA.data, OVR::Cameye::OV_CAMEYE_LEFT);
				cv::imshow("RGBA", lRGBA);
				break;

			case 'r':
				ovrvision.GetCamImageBGRA(rRGBA.data, OVR::Cameye::OV_CAMEYE_RIGHT);
				cv::imshow("RGBA", rRGBA);
				break;

			case 'R':
				rgba = !rgba;
				break;

			case 0x250000:
				OutputDebugStringA("←\n");
				break;

			case 0x260000:
				OutputDebugStringA("↑\n");
				interval += 5;
				break;

			case 0x270000:
				OutputDebugStringA("→\n");
				break;

			case 0x280000:
				OutputDebugStringA("↓\n");
				if (10 < interval)
					interval -= 5;
				break;

			case 0x210000:
				OutputDebugStringA("PageUp\n");
				value = ovrvision.GetCameraExposure();
				ovrvision.SetCameraExposure(value + 100);
				printf_s("Exposure:%d\n", value);
				break;

			case 0x220000:
				OutputDebugStringA("PageDown\n");
				value = ovrvision.GetCameraExposure();
				ovrvision.SetCameraExposure(value - 100);
				printf_s("Exposure:%d\n", value);
				break;

			case -1:
				break;

			default:
				sprintf_s(buffer, "%X %d\n", code, code);
				OutputDebugStringA(buffer);
			}
			if (rgba)
			{
				ovrvision.GetStereoImageBGRA(lRGBA.data, rRGBA.data, roi);
				cv::imshow("L", lRGBA);
				cv::imshow("R", rRGBA);
			}

			if (write)
			{
				ovrvision.GetStereoImageBGRA(lRGBA.data, rRGBA.data, roi);

				cv::imwrite(left_name, lRGBA);
				cv::imwrite(right_name, rRGBA);
			}

			if (stereo)
			{
				// status
				cout << "Processing: Frame: " << i;

				// compute visual odometry
				int32_t dims[] = { width,height,bytePerLine };
				if (viso.process(left.data, right.data, dims)) {

					// on success, update current pose
					pose = pose * Matrix::inv(viso.getMotion());

					// output some statistics
					double num_matches = viso.getNumberOfMatches();
					double num_inliers = viso.getNumberOfInliers();
					cout << ", Matches: " << num_matches;
					cout << ", Inliers: " << 100.0*num_inliers / num_matches << " %" << ", Current pose: " << endl;
					cout << pose << endl;
				}
				else {
					cout << " ... failed!" << endl;
				}
			}
		}
	}
    return 0;
}

