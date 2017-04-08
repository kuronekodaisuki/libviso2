//#include "stdafx.h"
#include "SfM.h"


SfM::SfM()
{
}


SfM::~SfM()
{
}

SfM::SfM(VisualOdometryMono::parameters params, const std::array<uint32_t, 3> dims, const bool use_opencl)
	:
	dims(dims)
{
	reconstruction.setCalibration(params.calib.f, params.calib.cu, params.calib.cv);

	if (use_opencl)
	{
#ifdef __arm__
		cl_container.reset(new OpenCL::FPGAContainer);
#else
		//cl_container.reset(new OpenCL::GPUContainer);
#endif
		//cl_container->init(KERNEL_SRCS);
		//viso.reset(new VisualOdometryMono_CL(params, *cl_container));
	}
	else
	{
		viso.reset(new VisualOdometryMono(params));
	}
}

void SfM::update(uint8_t* img_data)
{
	bool viso_success = viso->process(img_data, &dims[0], replace);

	if (is_first_frame)
	{
		is_first_frame = false;
		std::cout << std::endl;
	}
	else if (viso_success)
	{
		Matrix motion = Matrix::inv(viso->getMotion());
		Tr_total = Tr_total * motion;

		// print stats
		double num_matches = viso->getNumberOfMatches();
		double num_inliers = viso->getNumberOfInliers();
		std::cout << "Matches: " << num_matches;
		std::cout << ", Inliers: " << 100.0*num_inliers / num_matches << '%' << ", Current pose: " << std::endl;
		std::cout << Tr_total << std::endl << std::endl;

		//            reconstruction.update(viso.getMatches(),viso.getMotion(), 2, 2, 30, 3);
		reconstruction.update(viso->getMatches(), viso->getMotion(), 0, 2, 30, 3);

		replace = false;
	}
	else
	{
		std::cout << "No motion" << std::endl;
		replace = true;
	}
}

const std::vector<Point3d> SfM::&getPoints()
{
	return reconstruction.getPoints();
}

