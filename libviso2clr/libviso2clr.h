// libviso2clr.h

#pragma once
#include "viso_stereo.h"
#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"

using namespace System;

namespace libviso2clr {

	public class Pose
	{
		float matrix[4][4];
	};

	public ref class Stereo
	{
	public:
		Stereo();
		~Stereo() { this->!Stereo(); }
		!Stereo();

		/**
		 * @brief 
		 * @param base_line ステレオカメラの距離
		 * @param cu センター位置
		 * @param cv センター位置
		 * @param focus 焦点距離
		 * @return 
		 */
		bool init(double base_line, double cu, double cv, double focus);

		/**
		 * @brief 左右の画像ファイルパスを指定して処理
		 * @param left 左画像のファイルパス 
		 * @param right 右画像のファイルパス
		 * @return 処理に成功したらtrueを返す
		 */
		bool process(String ^left, String ^right);

		Pose getPose();

	private:
		VisualOdometryStereo *viso;
		Matrix *pose;		
	};
}
