// demo_USB_camera.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
// libviso2
#include <viso_stereo.h>　

// 画像ファイルのアクセスにOpenCVを使います
#include <opencv2\core.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2/highgui.hpp>

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

int main()
{
	// カメラ＝画像の移動行列
	Matrix pose = Matrix::eye(4); 

	// libviso2ライブラリに与えるパラメータ
	VisualOdometryStereo::parameters param;

	// キャリブレーション設定
	param.calib.f = 640; // focal length in pixels
	param.calib.cu = IMAGE_WIDTH / 2; // principal point (u-coordinate) in pixels
	param.calib.cv = IMAGE_HEIGHT / 2; // principal point (v-coordinate) in pixels
	param.base = 0.38; // 画像間の撮影距離
	
	// visual odometry初期化
	VisualOdometryStereo viso(param);
	// 左右のカメラを定義
	cv::VideoCapture camLeft, camRight;
	// 解像度を設定
	camLeft.set(cv::CAP_PROP_FRAME_WIDTH, IMAGE_WIDTH);
	camLeft.set(cv::CAP_PROP_FRAME_HEIGHT, IMAGE_HEIGHT);
	camRight.set(cv::CAP_PROP_FRAME_WIDTH, IMAGE_WIDTH);
	camRight.set(cv::CAP_PROP_FRAME_HEIGHT, IMAGE_HEIGHT);
	camLeft.open(0);  // 左を先に接続
	camRight.open(1);

	cv::Mat l, r;
	cv::Mat left, right;
	left.create(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
	right.create(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC1);
	int bypePerLine = left.step;

	for (bool loop = true; loop; )
	{
		// カメラからキャプチャー
		camLeft.grab(); // 左右のカメラをソフト的に同期させる
		camRight.grab();
		camLeft.retrieve(l);
		camRight.retrieve(r);

		// グレースケールに変換
		cv::cvtColor(l, left, cv::COLOR_BGR2GRAY);
		cv::cvtColor(r, right, cv::COLOR_BGR2GRAY);
		// 表示
		cv::imshow("Left", left);
		cv::imshow("Right", right);

		int32_t dims[] = { IMAGE_WIDTH, IMAGE_HEIGHT, bypePerLine };
		if (viso.process(left.data, right.data, dims))
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

		if (cv::waitKey(30) == 'q')
			loop = false;    // 終了
	}
	return 0;
}


