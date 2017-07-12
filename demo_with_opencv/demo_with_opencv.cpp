// demo_with_opencv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

#include <viso_stereo.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

//#pragma warning(disable:4996)

using namespace std;

int main(int argc, char* argv[])
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
	for (int32_t i = 0; i<373; i++) {

		// input file names
		char base_name[256]; sprintf(base_name, "%06d.png", i);
		string left_img_file_name = dir + "/I1_" + base_name;
		string right_img_file_name = dir + "/I2_" + base_name;

		// catch image read/write errors here
		try {

			// load left and right input image
			cv::Mat left = cv::imread(left_img_file_name, cv::IMREAD_GRAYSCALE);
			cv::Mat right = cv::imread(right_img_file_name, cv::IMREAD_GRAYSCALE);

			int32_t width = left.cols;
			int32_t height = left.rows;
			int32_t bytePerLine = left.step;

			uint8_t *left_img_data = left.data;
			uint8_t *right_img_data = right.data;

			// status
			cout << i;

			// compute visual odometry
			int32_t dims[] = { width, height, bytePerLine };
			if (viso.process(left_img_data, right_img_data, dims)) {

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

			// catch image read errors here
		}
		catch (...) {
			cerr << "ERROR: Couldn't read input files!" << endl;
			return 1;
		}
	}

	// output
	cout << "Demo complete! Exiting ..." << endl;

	// exit
	return 0;

}

