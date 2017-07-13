// demo_USB_camera.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
// libviso2
#include <viso_stereo.h>�@

// �摜�t�@�C���̃A�N�Z�X��OpenCV���g���܂�
#include <opencv2\core.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2/highgui.hpp>

#define IMAGE_WIDTH 1280
#define IMAGE_HEIGHT 720

int main()
{
	Matrix pose = Matrix::eye(4); // �J�������摜�̈ړ��s��

								  // libviso2���C�u�����ɗ^����p�����[�^
	VisualOdometryStereo::parameters param;

	// �L�����u���[�V�����ݒ�
	param.calib.f = 640; // focal length in pixels
	param.calib.cu = IMAGE_WIDTH / 2; // principal point (u-coordinate) in pixels
	param.calib.cv = IMAGE_HEIGHT / 2; // principal point (v-coordinate) in pixels
	param.base = 0.38; // �摜�Ԃ̎B�e����
					   // visual odometry������
	VisualOdometryStereo viso(param);
	// ���E�̃J�������`
	cv::VideoCapture camLeft, camRight;
	// �𑜓x��ݒ�
	camLeft.set(cv::CAP_PROP_FRAME_WIDTH, IMAGE_WIDTH);
	camLeft.set(cv::CAP_PROP_FRAME_HEIGHT, IMAGE_HEIGHT);
	camRight.set(cv::CAP_PROP_FRAME_WIDTH, IMAGE_WIDTH);
	camRight.set(cv::CAP_PROP_FRAME_HEIGHT, IMAGE_HEIGHT);
	camLeft.open(0);  // �����ɐڑ�
	camRight.open(1);

	cv::Mat l, r;
	cv::Mat left, right;

	for (bool loop = true; loop; )
	{
		// �J��������L���v�`���[
		camLeft.grab(); // ���E�̃J�������\�t�g�I�ɓ���������
		camRight.grab();
		camLeft.retrieve(l);
		camRight.retrieve(r);
		// �O���[�X�P�[���ɕϊ�
		cv::cvtColor(l, left, cv::COLOR_BGR2GRAY);
		cv::cvtColor(r, right, cv::COLOR_BGR2GRAY);

		int32_t dims[] = { IMAGE_WIDTH, IMAGE_HEIGHT, left.step };
		if (viso.process(left.data, right.data, dims))
		{
			// on success, update current pose
			pose = pose * Matrix::inv(viso.getMotion()); // �ړ��s����X�V
														 // pose�̏���\��

														 // output some statistics
			double num_matches = viso.getNumberOfMatches();
			double num_inliers = viso.getNumberOfInliers();
		}

		if (cv::waitKey(30) == 'q')
			loop = false;    // �I��
	}
	return 0;
}

