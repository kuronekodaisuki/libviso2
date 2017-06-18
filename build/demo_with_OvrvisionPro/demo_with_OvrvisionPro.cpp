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
		if (ovrvision.Open(0, OVR::OV_CAMHD_FULL, 0) == 0)
			puts("Can't open OvrvisionPro");
		printf_s("Focal point: %f\n", ovrvision.GetCamFocalPoint());
		int height = ovrvision.GetCamHeight();
		int width = ovrvision.GetCamWidth();
		int bytePerLine;
		OVR::ROI roi = { 0, 0, width, height };
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
		for (bool loop = true; loop; )
		{
			ovrvision.Capture(OVR::Camqt::OV_CAMQT_DMS);

			//ovrvision.GetStereoImageBGRA(left.data, right.data, roi);
			ovrvision.Grayscale(left.data, right.data);
			
			cv::imshow("Left", left);
			cv::imshow("Right", right);

			switch (cv::waitKey(10))
			{
			case 'q':
				loop = false;
				break;

			case 's':
				stereo = !stereo;
				break;
			}

			if (stereo)
			{
				// compute visual odometry
				int32_t dims[] = { width,height,bytePerLine };
				if (viso.process(left.data, right.data, dims)) {

					// on success, update current pose
					pose = pose * Matrix::inv(viso.getMotion());

					// output some statistics
					double num_matches = viso.getNumberOfMatches();
					double num_inliers = viso.getNumberOfInliers();
					cout << ", " << num_matches;
					cout << ", " << 100.0*num_inliers / num_matches << ", ";
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

