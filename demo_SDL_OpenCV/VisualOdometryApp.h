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

private:
	struct cam {
		float p[10][3];
		bool  keyframe;
	};
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

