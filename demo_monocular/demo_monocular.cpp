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

	cv::VideoCapture camera;

	// 動画ファイルの場合
	if (2 <= argc && camera.open(argv[1]))
	{
		cv::Mat image, gray;
		cv::VideoWriter writer;
		bool write = false;
		if (3 <= argc)
		{
			write = writer.open(argv[2], cv::VideoWriter::fourcc('M', 'P', '4', 'S'), 30, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT));
		}

		while (camera.grab())
		{
			camera.retrieve(image);
			cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

			int width = gray.cols;
			int height = gray.rows;
			int bypePerLine = gray.step;

			int32_t dims[] = { width, height, bypePerLine };
			if (viso.process(gray.data, dims))
			{
				// 移動行列を更新
				pose = pose * Matrix::inv(viso.getMotion());

				if (write)
				{
					std::vector<Matcher::p_match> matched = viso.getMatches();
					std::vector<int> indices = viso.getInlierIndices();
					for (size_t idx = 0; idx < indices.size(); idx++)
					{
						int i = indices[idx];
						cv::line(image, cv::Point(matched[i].u1c, matched[i].v1c), cv::Point(matched[i].u1p, matched[i].v1p), cv::Scalar(0, 0, 255), 2);
					}
					writer.write(image);
				}
				// output some statistics
				double num_matches = viso.getNumberOfMatches();
				double num_inliers = viso.getNumberOfInliers();
				VISO2::FLOAT values[16];

				// poseの情報を表示
				std::cout << "Matches: " << num_matches;
				std::cout << ", Inliers: " << 100.0*num_inliers / num_matches << "%, ";
				//std::cout << pose << std::endl;
				pose.getData(values);
				std::cout << values[3] << ", " << values[7] << ", " << values[11] << std::endl;
			}
		}
		if (write)
			writer.release();
		camera.release();
	}
	else if (camera.open(0))
	{
		cv::Mat image, gray;
		cv::VideoWriter writer;
		bool write = false;

		write = writer.open("output.mp4", cv::VideoWriter::fourcc('M', 'P', '4', 'S'), 30, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT));

		for (bool loop = true; loop && camera.grab(); )
		{
			camera.retrieve(image);
			cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

			int width = gray.cols;
			int height = gray.rows;
			int bypePerLine = gray.step;

			int32_t dims[] = { width, height, bypePerLine };
			if (viso.process(gray.data, dims))
			{
				// 移動行列を更新
				pose = pose * Matrix::inv(viso.getMotion());

				if (write)
				{
					std::vector<Matcher::p_match> matched = viso.getMatches();
					std::vector<int> indices = viso.getInlierIndices();
					for (size_t idx = 0; idx < indices.size(); idx++)
					{
						int i = indices[idx];
						cv::line(image, cv::Point(matched[i].u1c, matched[i].v1c), cv::Point(matched[i].u1p, matched[i].v1p), cv::Scalar(0, 0, 255), 2);
					}
					writer.write(image);
				}
				// output some statistics
				double num_matches = viso.getNumberOfMatches();
				double num_inliers = viso.getNumberOfInliers();
				VISO2::FLOAT values[16];

				// poseの情報を表示
				//std::cout << "Matches: " << num_matches;
				//std::cout << ", Inliers: " << 100.0 * num_inliers / num_matches << "%, ";
				//std::cout << pose << std::endl;
				pose.getData(values);
				//std::cout << values[3] << ", " << values[7] << ", " << values[11] << std::endl;

				if (cv::waitKey(10) == 'q')
					loop = false;
			}
		}

		writer.release();
		camera.release();
	}
}

