// demo_SDL_OpenCV.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

#include <viso_stereo.h>

#pragma warning(push)
#pragma warning(disable: 4819)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#pragma warning(pop)

#include "VisualOdometryApp.h"

using namespace std;

static const char WINDOW_CAPTION[] = "SDL with OpenGL : projection test";
static const Uint32 WINDOW_WIDTH = 1024;
static const Uint32 WINDOW_HEIGHT = 768;
static const Uint32 WINDOW_BPP = 32;
static const Uint32 FPS = 60;

#define	NUMBER_OF_FRAME	8000
static float scale = 0.05;

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char* argv[])
{
	VisualOdometryApp app;

	if (2 <= argc)
	{
		// sequence directory
		string dir = argv[1];

		// set most important visual odometry parameters
		// for a full parameter list, look at: viso_stereo.h
		VisualOdometryStereo::parameters param;

		// calibration parameters for sequence 2010_03_09_drive_0019 
		param.calib.f = 645.24; // focal length in pixels
		param.calib.cu = 320; // principal point (u-coordinate) in pixels
		param.calib.cv = 240; // principal point (v-coordinate) in pixels
		param.base = 0.3707; // baseline in meters

		// init visual odometry
		VisualOdometryStereo viso(param);

		// current pose (this matrix transforms a point from the current
		// frame's camera coordinates to the first frame's camera coordinates)
		Matrix pose = Matrix::eye(4);

		// loop through all frames i=0:372
		int32_t i = 0, n = NUMBER_OF_FRAME;
		if (3 <= argc)
		{
			i = atoi(argv[2]);
			if (4 <= argc)
				n = atoi(argv[3]);
		}

		for (; i < n; i += 10) 
		{ 
			// input file names
			char base_name[256]; sprintf(base_name, "%06d.jpg", i);
			string left_img_file_name = dir + "/I1_" + base_name;
			string right_img_file_name = dir + "/I2_" + base_name;

			// catch image read/write errors here
			try {

				// load left and right input image
				cv::Mat left = cv::imread(left_img_file_name, cv::IMREAD_GRAYSCALE);
				cv::Mat right = cv::imread(right_img_file_name, cv::IMREAD_GRAYSCALE);
				cv::Mat	LEFT(left.rows, left.cols, CV_8UC3);
				cv::Mat	RIGHT(right.rows, right.cols, CV_8UC3);

				int32_t width = left.cols;
				int32_t height = left.rows;
				int32_t bytePerLine = left.step;

				if (height == 0 || width == 0 || bytePerLine == 0)
					continue;

				//cv::imshow("LEFT", left);
				//cv::imshow("RIGHT", right);

				uint8_t *left_img_data = left.data;
				uint8_t *right_img_data = right.data;

				// status
				// status
				std::cout << "Processing: Frame: " << i;

				// compute visual odometry
				int32_t dims[] = { width, height, bytePerLine };
				if (viso.process(left_img_data, right_img_data, dims)) {

					// on success, update current pose
					pose = pose * Matrix::inv(viso.getMotion());

					// output some statistics
					double num_matches = viso.getNumberOfMatches();
					double num_inliers = viso.getNumberOfInliers();
					// pose‚Ìî•ñ‚ð•\Ž¦
					std::cout << ", Matches: " << num_matches;
					std::cout << ", Inliers: " << 100.0*num_inliers / num_matches << " %" << ", Current pose: " << std::endl;
					std::cout << pose << std::endl;

					app.addCamera(pose, scale, true);

					cv:cvtColor(left, LEFT, cv::COLOR_GRAY2BGR);
					//cv:cvtColor(right, RIGHT, cv::COLOR_GRAY2BGR);

					std::vector<Matcher::p_match> matched = viso.getMatches();
					std::vector<int> indices = viso.getInlierIndices();
					for (size_t i = 0; i < matched.size(); i++)
					{
						std::cout << matched[i].u1c << ", " << matched[i].v1c << std::endl;
						cv::line(LEFT, cv::Point(matched[i].u1c, matched[i].v1c), cv::Point(matched[i].u1p, matched[i].v1p), cv::Scalar(0, 0, 255), 2);
						cv::line(RIGHT, cv::Point(matched[i].u2c, matched[i].v2c), cv::Point(matched[i].u2p, matched[i].v2p), cv::Scalar(0, 0, 255), 2);
					}
					cv::imshow("LEFT", LEFT);
					cv::imshow("RIGHT", RIGHT);
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
	}
	// initialize
	if (!app.init(WINDOW_CAPTION, WINDOW_WIDTH, WINDOW_HEIGHT)) {
		std::cerr << "ERROR: failed to initialize SDL" << std::endl;
		exit(1);
	}
	app.setFPS(FPS);
	// mainloop
	while (true) {
		// check event
		if (!app.pollingEvent()) break;

		// update and draw
		app.update();
	}
	// finalize
	app.finalize();

	return 0;
}



