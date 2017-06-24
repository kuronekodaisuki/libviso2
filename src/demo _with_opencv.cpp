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

#include <opencv2\core.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

int main (int argc, char** argv) {

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
  param.calib.f  = 640; // focal length in pixels
  param.calib.cu = 640; // principal point (u-coordinate) in pixels
  param.calib.cv = 360; // principal point (v-coordinate) in pixels
  param.base     = 0.38; // baseline in meters
  
  // init visual odometry
  VisualOdometryStereo viso(param);
  
  // current pose (this matrix transforms a point from the current
  // frame's camera coordinates to the first frame's camera coordinates)
  Matrix pose = Matrix::eye(4);
    
  // loop through all frames i=0:372
  for (int32_t i = 1; i < 120; i++) {

    // input file names
    char base_name[256]; 
	sprintf(base_name,"%06d.jpg",i);
    string left_img_file_name  = dir + "/I1_" + base_name;
    string right_img_file_name = dir + "/I2_" + base_name;
    
    // catch image read/write errors here
    try {

      // load left and right input image
		cv::Mat l = cv::imread(left_img_file_name);
		cv::Mat r = cv::imread(right_img_file_name);
		cv::Mat L, R, left, right;
		cv::cvtColor(l, L, cv::COLOR_BGR2HSV);
		cv::cvtColor(r, R, cv::COLOR_BGR2HSV);

		vector<cv::Mat> hsv;
		cv::split(L, hsv);
		left = hsv[1];
		cv::split(R, hsv);
		right = hsv[1];

		//cv::imshow("LEFT", left);
		//cv::imshow("RIGHT", right);
		cv::imwrite(left_img_file_name + ".png", left);
		cv::imwrite(right_img_file_name + ".png", right);

		int32_t width = left.cols;
		int32_t height = left.rows;
		int32_t bytePerLine = left.step;

		uint8_t *left_img_data = left.data;
		uint8_t *right_img_data = right.data;

      // status
      cout << "Processing: Frame: " << i;
      
      // compute visual odometry
      int32_t dims[] = {width,height,bytePerLine};
      if (viso.process(left_img_data,right_img_data,dims)) {
      
        // on success, update current pose
        pose = pose * Matrix::inv(viso.getMotion());
      
        // output some statistics
        double num_matches = viso.getNumberOfMatches();
        double num_inliers = viso.getNumberOfInliers();
        cout << ", Matches: " << num_matches;
        cout << ", Inliers: " << 100.0*num_inliers/num_matches << " %" << ", Current pose: " << endl;
        cout << pose << endl;

      } else {
        cout << " ... failed!" << endl;
      }

    // catch image read errors here
    } catch (...) {
      cerr << "ERROR: Couldn't read input files!" << endl;
      return 1;
    }
  }
  
  // output
  cout << "Demo complete! Exiting ..." << endl;

  // exit
  return 0;
}

