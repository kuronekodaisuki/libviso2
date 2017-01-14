#pragma once
#include "App.h"

#include <vector>
#include <viso_stereo.h>

class VisualOdometryApp :
	public App
{
public:
	VisualOdometryApp();
	~VisualOdometryApp();

	// delived
	void draw();

	bool mouseMove(int32_t x, int32_t y, int32_t dx, int32_t dy, MOUSE_BUTTON button);

	struct point_3d {
		float x, y, z;
		float val;
		point_3d(float x, float y, float z, float val) : x(x), y(y), z(z), val(val) {}
	};

	struct cam {
		float p[10][3];
		bool  keyframe;
	};

	void addCamera(Matrix H_total, float s, bool keyframe);
	void addPoints(std::vector< std::vector<point_3d> > p);

private:
	std::vector<cam> cams;

	struct pose {
		float  zoom;
		float  rotx, roty;
		float  tx, ty, tz;
	};
	std::vector<pose> poses;
	pose   pose_curr;
	std::vector<GLuint> gl_list;

	bool   bg_wall_flag;
	float  bg_wall_pos;
	bool   show_cam_flag;
	bool   show_grid_flag;
	bool   show_white_flag;
};

