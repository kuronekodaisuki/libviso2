// これは メイン DLL ファイルです。

#include "stdafx.h"

#include "libviso2clr.h"
#include "opencv2/highgui.hpp"

using namespace System::Runtime::InteropServices;

namespace libviso2clr {

	Stereo::Stereo()
	{
		viso = nullptr;
	}

	Stereo::!Stereo()
	{
		if (viso != nullptr) 
			delete viso;
	}

	bool Stereo::init(double base_line, double cu, double cv, double focus)
	{
		VisualOdometryStereo::parameters param;
		param.base = base_line;
		param.calib.f = focus;
		param.calib.cu = cu;
		param.calib.cv = cv;
		viso = new VisualOdometryStereo(param);

		pose = new Matrix();
		pose->eye(4);
		return true;
	}

	bool Stereo::process(String ^left, String ^right)
	{
		bool result;

		IntPtr pLeft = Marshal::StringToHGlobalUni(left);
		IntPtr pRight = Marshal::StringToHGlobalUni(right);

		cv::Mat l = cv::imread((const char *)pLeft.ToPointer(), cv::IMREAD_GRAYSCALE);
		cv::Mat r = cv::imread((const char *)pRight.ToPointer(), cv::IMREAD_GRAYSCALE);
		int32_t dims[] = { l.cols, l.rows, l.step };

		result = viso->process(l.data, r.data, dims);
		if (result)
		{
			*pose = *pose * Matrix::inv(viso->getMotion());
		}

		Marshal::FreeHGlobal(pLeft);
		Marshal::FreeHGlobal(pRight);
		return result;
	}
}