//
//
//

#include "VisualOdometryApp.h"

using namespace std;

VisualOdometryApp::VisualOdometryApp()
{
	pose_curr.zoom = -1.5;
	pose_curr.rotx = 180;
	pose_curr.roty = 0;
	pose_curr.tx = 0;
	pose_curr.ty = 0;
	pose_curr.tz = -1.5;

	bg_wall_flag = 0;
	bg_wall_pos = 1;
	show_cam_flag = 1;
	show_grid_flag = 1;
	show_white_flag = 1;
}


VisualOdometryApp::~VisualOdometryApp()
{
}

void VisualOdometryApp::draw()
{
	preDraw();

	// clear screen & set matrices
	if (show_white_flag) glClearColor(1, 1, 1, 1);
	else                 glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// apply transformation
	glTranslatef(0.0, 0.0, pose_curr.zoom);
	glRotatef(pose_curr.rotx, 1.0, 0.0, 0.0);
	glRotatef(pose_curr.roty, 0.0, 1.0, 0.0);
	glTranslatef(pose_curr.tx, pose_curr.ty, pose_curr.tz);

	if (show_grid_flag) {

		// paint ground grid
		float r = 200;
		float h = 2;
		glColor3f(0.5, 0.5, 0.5);
		glLineWidth(1);
		glBegin(GL_LINES);
		for (float x = -r; x <= r + 0.001; x += 5) {
			glVertex3f(x, h, -r); glVertex3f(x, h, +r);
			glVertex3f(-r, h, x); glVertex3f(+r, h, x);
		}
		glEnd();
	}

	// draw 3d points
	for (vector<GLuint>::iterator it = gl_list.begin(); it != gl_list.end(); it++)
		glCallList(*it);

	// draw background wall
	if (bg_wall_flag) {
		if (show_white_flag) glColor3f(1, 1, 1);
		else                 glColor3f(0, 0, 0);
		glBegin(GL_QUADS);
		glVertex3f(+20, -20, bg_wall_pos);
		glVertex3f(-20, -20, bg_wall_pos);
		glVertex3f(-20, +20, bg_wall_pos);
		glVertex3f(+20, +20, bg_wall_pos);
		glEnd();
	}

	if (show_cam_flag) {

		// draw cameras
		glDisable(GL_DEPTH_TEST);
		glLineWidth(1);
		for (vector<cam>::iterator it = cams.begin(); it != cams.end(); it++) {
			if (it->keyframe) glColor3f(1.0, 0.0, 0.0);
			else              glColor3f(1.0, 1.0, 0.0);
			glBegin(GL_LINE_STRIP);
			for (int32_t i = 0; i<10; i++)
				glVertex3f(it->p[i][0], it->p[i][1], it->p[i][2]);
			glEnd();
		}

		// draw connection line
		glBegin(GL_LINE_STRIP);
		for (vector<cam>::iterator it = cams.begin(); it != cams.end(); it++)
			glVertex3f(it->p[5][0], it->p[5][1], it->p[5][2]);
		glEnd();
		glEnable(GL_DEPTH_TEST);


		// draw coordinate system
		float s = 0.3;
		glDisable(GL_DEPTH_TEST);
		glLineWidth(3);
		glBegin(GL_LINES);
		glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(s, 0, 0);
		glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, s, 0);
		glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, s);
		glEnd();
		glEnable(GL_DEPTH_TEST);


		// draw rotation anchor
		glPointSize(3);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_POINTS);
		glVertex3f(-pose_curr.tx, -pose_curr.ty, -pose_curr.tz);
		glEnd();
	}

	postDraw();
}

bool VisualOdometryApp::mouseMove(int32_t x, int32_t y, int32_t dx, int32_t dy, MOUSE_BUTTON button)
{
	switch (button)
	{
	case MIDDLE:	// both buttons => zoom
		if (dy>0) pose_curr.zoom *= (1 + 0.01 * (dy));
		else      pose_curr.zoom /= (1 + 0.01 * (dy));
		update();
		break;

	case LEFT:		// left button => rotation
		pose_curr.rotx += dy;
		if (pose_curr.rotx<90)  pose_curr.rotx = 90;
		if (pose_curr.rotx>270) pose_curr.rotx = 270;
		pose_curr.roty += dx;
		if (pose_curr.roty<-180) pose_curr.roty = -180;
		if (pose_curr.roty>+180) pose_curr.roty = +180;
		break;

	case RIGHT:		// right button => translation
		float rotx2 = pose_curr.rotx;
		if (rotx2<170) rotx2 = 90;
		if (rotx2>190) rotx2 = 270;
		float rx = rotx2*M_PI / 180.0;
		float ry = pose_curr.roty*M_PI / 180.0;

		Matrix R = Matrix::rotMatY(-ry)*Matrix::rotMatX(-rx);

		Matrix v(3, 1);
		v.val[0][0] = dx;
		v.val[1][0] = dy;

		v = R*v;
		pose_curr.tx += 0.0025*pose_curr.zoom*v.val[0][0];
		pose_curr.ty += 0.0025*pose_curr.zoom*v.val[1][0];
		pose_curr.tz += 0.0025*pose_curr.zoom*v.val[2][0];
		break;
	}

	update();

	return true;
}