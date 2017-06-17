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
#include <opencv2\core.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#pragma warning(pop)

#include <ovrvision_pro.h>

using namespace std;


int main(int argc, char** argv)
{
	// we need the path name to 2010_03_09_drive_0019 as input argument
	if (argc<2) {
		cerr << "Usage: ./viso2 path/to/sequence/2010_03_09_drive_0019" << endl;
		return 1;
	}

	// sequence directory
	string dir = argv[1];

	// set most important visual odometry parameters
	// for a full parameter list, look at: viso_stereo.h
	VisualOdometryStereo::parameters param;

	// calibration parameters for sequence 2010_03_09_drive_0019 
	param.calib.f = 645.24; // focal length in pixels
	param.calib.cu = 635.96; // principal point (u-coordinate) in pixels
	param.calib.cv = 194.13; // principal point (v-coordinate) in pixels
	param.base = 0.5707; // baseline in meters

						 // init visual odometry
	VisualOdometryStereo viso(param);

	// current pose (this matrix transforms a point from the current
	// frame's camera coordinates to the first frame's camera coordinates)
	Matrix pose = Matrix::eye(4);

	// loop through all frames i=0:372
	for (int32_t i = 0; i < 1400; i++) {
	}
    return 0;
}

