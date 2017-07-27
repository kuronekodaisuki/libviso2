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

	bg_wall_flag = 1;
	bg_wall_pos = 1;
	show_cam_flag = 1;
	show_grid_flag = 1;
	show_white_flag = 1;
}

VisualOdometryApp::~VisualOdometryApp()
{
	cams.clear();
	for (int32_t i = 0; i<(int32_t)gl_list.size(); i++) 
		glDeleteLists(gl_list[i], 1);
	gl_list.clear();
}

void VisualOdometryApp::addCamera(Matrix H_total, float s, bool keyframe) {
	// create list with points for this camera
	Matrix C(4, 10);
	C.val[0][0] = -0.5*s; C.val[1][0] = -0.5*s; C.val[2][0] = +1.0*s;
	C.val[0][1] = +0.5*s; C.val[1][1] = -0.5*s; C.val[2][1] = +1.0*s;
	C.val[0][2] = +0.5*s; C.val[1][2] = +0.5*s; C.val[2][2] = +1.0*s;
	C.val[0][3] = -0.5*s; C.val[1][3] = +0.5*s; C.val[2][3] = +1.0*s;
	C.val[0][4] = -0.5*s; C.val[1][4] = -0.5*s; C.val[2][4] = +1.0*s;
	C.val[0][5] = 0; C.val[1][5] = 0; C.val[2][5] = 0;
	C.val[0][6] = +0.5*s; C.val[1][6] = -0.5*s; C.val[2][6] = +1.0*s;
	C.val[0][7] = +0.5*s; C.val[1][7] = +0.5*s; C.val[2][7] = +1.0*s;
	C.val[0][8] = 0; C.val[1][8] = 0; C.val[2][8] = 0;
	C.val[0][9] = -0.5*s; C.val[1][9] = +0.5*s; C.val[2][9] = +1.0*s;
	for (int32_t i = 0; i<10; i++)
		C.val[3][i] = 1;

	// transfer camera to reference coordinate system
	Matrix C_ref = H_total*C;

	// add camera to list of cameras
	cam ccam;
	ccam.keyframe = keyframe;
	for (int32_t i = 0; i<10; i++)
		for (int32_t j = 0; j<3; j++)
			ccam.p[i][j] = C_ref.val[j][i];
	cams.push_back(ccam);

	// update 3d view
	update();
}

void VisualOdometryApp::addPoints(vector< vector<point_3d> > p) {

	preDraw();

	// if 2 point clouds given, delete last display list
	if (p.size()>1 && gl_list.size()>0) {
		glDeleteLists(gl_list.back(), 1);
		gl_list.pop_back();
	}

	// process last two elements of p
	for (int32_t i = max((int32_t)p.size() - 2, 0); i<(int32_t)p.size(); i++) {

		if (0) {

			// allocate bins
			vector< vector<point_3d> > points;
			for (int32_t j = 0; j<256; j++)
				points.push_back(vector<point_3d>());

			// put points into bins
			for (vector<point_3d>::iterator it = p[i].begin(); it != p[i].end(); it++)
				points[(int32_t)(it->val*255.0)].push_back(*it);

			// create display list
			int32_t gl_idx = glGenLists(1);
			gl_list.push_back(gl_idx);
			glNewList(gl_idx, GL_COMPILE);

			glPointSize(2);
			glBegin(GL_POINTS);
			for (int32_t j = 0; j<256; j++) {
				if (points[j].size()>0) {
					float c = (float)j / 255.0;
					glColor3f(c, c, c);
					for (vector<point_3d>::iterator it = points[j].begin(); it != points[j].end(); it++)
						glVertex3f(it->x, it->y, it->z);
				}
			}

			// finish display list
			glEnd();
			glEnd();
			glEndList();

		}
		else {

			// create display list
			int32_t gl_idx = glGenLists(1);
			gl_list.push_back(gl_idx);
			glNewList(gl_idx, GL_COMPILE);

			glPointSize(2);
			glBegin(GL_POINTS);
			for (vector<point_3d>::iterator it = p[i].begin(); it != p[i].end(); it++) {
				glColor3f(it->val, it->val, it->val);
				glVertex3f(it->x, it->y, it->z);
			}

			// finish display list
			glEnd();
			glEnd();
			glEndList();
		}
	}

	postDraw();

	// update view
	update();
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

bool VisualOdometryApp::mouseButton(int32_t x, int32_t y, MOUSE_BUTTON button)
{
	return true;
}

bool VisualOdometryApp::mouseWheel(int32_t x, int32_t y)
{
	return true;
}

void VisualOdometryApp::resize(uint32_t width, uint32_t height)
{
	cout << "GL Viewport size: " << width << " x " << height << endl;

	int side = max(width, height);
	glViewport((width - side) / 2, (height - side) / 2, side, side);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 0.1, 10000);
	glMatrixMode(GL_MODELVIEW);

	update();
}