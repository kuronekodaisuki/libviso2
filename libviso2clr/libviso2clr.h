// libviso2clr.h

#pragma once
#include "viso_stereo.h"
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"

using namespace System;

namespace libviso2clr {

	public ref class Stereo
	{
	public:
		Stereo();
		~Stereo() { this->!Stereo(); }
		!Stereo();

		/**
		 * @brief 
		 * @param base_line �X�e���I�J�����̋���
		 * @param cu �Z���^�[�ʒu
		 * @param cv �Z���^�[�ʒu
		 * @param focus �œ_����
		 * @return 
		 */
		bool init(double base_line, double cu, double cv, double focus);

		/**
		 * @brief ���E�̉摜�t�@�C���p�X���w�肵�ď���
		 * @param left ���摜�̃t�@�C���p�X 
		 * @param right �E�摜�̃t�@�C���p�X
		 * @return �����ɐ���������true��Ԃ�
		 */
		bool process(String ^left, String ^right);

	private:
		VisualOdometryStereo *viso;
		Matrix *pose;
		
	};
}