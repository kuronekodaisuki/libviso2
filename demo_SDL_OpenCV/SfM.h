#pragma once

#include <memory>
#include <array>
#include <viso_mono.h>
#include <reconstruction.h>

struct Point3d {
	float x, y, z;
	Point3d() {}
	Point3d(float x, float y, float z) : x(x), y(y), z(z) {}
};

class SfM
{
public:
	SfM();
	~SfM();

	std::unique_ptr<VisualOdometryMono> viso;
	Reconstruction reconstruction;

	bool replace = false;
	bool is_first_frame = true;
	std::array<uint32_t, 3> dims;

	// this matrix transforms a point from the first
	// frame's camera coordinates to the current frame's camera coordinates
	Matrix Tr_total = Matrix::eye(4);

	//std::unique_ptr<OpenCL::Container> cl_container;

public:

	SfM(VisualOdometryMono::parameters params, const std::array<uint32_t, 3> dims, const bool use_opencl);

	void update(uint8_t* img_data);
	const std::vector<Point3d> &getPoints();
};

