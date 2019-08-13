// demo_monocular.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "pch.h"
#include <iostream>


#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

// libviso2
#include <viso_mono.h>

// 画像ファイルのアクセスにOpenCVを使います
#pragma warning(push)
#pragma warning(disable: 4819)
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#pragma warning(pop)

#define FOCAL_LENGTH	20 
#define IMAGE_WIDTH		1920
#define IMAGE_HEIGHT	1080

#include <Windows.h>

using namespace VISO2;

int main(int argc, char *argv[])
{
	// カメラ＝画像の移動行列
	Matrix pose = Matrix::eye(4);

	// libviso2ライブラリに与えるパラメータ
	VisualOdometryMono::parameters param;

	// キャリブレーション設定
	param.calib.f = FOCAL_LENGTH; // focal length in pixels
	param.calib.cu = IMAGE_WIDTH / 2; // principal point (u-coordinate) in pixels
	param.calib.cv = IMAGE_HEIGHT / 2; // principal point (v-coordinate) in pixels

	// visual odometry初期化
	VisualOdometryMono viso(param);

	cv::VideoCapture movie;

	if (2 <= argc && movie.open(argv[1]))
	{
		cv::Mat image, gray;

		while (movie.grab())
		{
			movie.retrieve(image);
			cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
			cv::imshow("image", image);

			int width = gray.cols;
			int height = gray.rows;
			int bypePerLine = gray.step;

			int32_t dims[] = { width, height, bypePerLine };
			if (viso.process(gray.data, dims))
			{
				// 移動行列を更新
				pose = pose * Matrix::inv(viso.getMotion());

				// output some statistics
				double num_matches = viso.getNumberOfMatches();
				double num_inliers = viso.getNumberOfInliers();

				// poseの情報を表示
				std::cout << ", Matches: " << num_matches;
				std::cout << ", Inliers: " << 100.0*num_inliers / num_matches << " %" << ", Current pose: " << std::endl;
				std::cout << pose << std::endl;
			}
		}
		movie.release();
	}
	else
	{
		DWORD error = GetLastError();
		fprintf(stderr, "%d", error);
	}
}

