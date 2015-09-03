#include <iostream>
#include "GLWidget3D.h"
#include "MainWindow.h"
#include <GL/GLU.h>
#include "OBJLoader.h"

/**
 * This event handler is called when the mouse press events occur.
 */
void GLWidget3D::mousePressEvent(QMouseEvent *e) {
	lastPos = e->pos();
}

/**
 * This event handler is called when the mouse release events occur.
 */
void GLWidget3D::mouseReleaseEvent(QMouseEvent *e) {
	updateGL();
}

/**
 * This event handler is called when the mouse move events occur.
 */
void GLWidget3D::mouseMoveEvent(QMouseEvent *e) {
	float dx = (float)(e->x() - lastPos.x());
	float dy = (float)(e->y() - lastPos.y());
	lastPos = e->pos();

	if (e->buttons() & Qt::LeftButton) {
		camera.changeXRotation(dy);
		camera.changeYRotation(dx);
	} else if (e->buttons() & Qt::RightButton) {
		camera.changeXYZTranslation(0, 0, -dy * camera.dz * 0.02f);
	} else if (e->buttons() & Qt::MidButton) {
		camera.changeXYZTranslation(-dx * 0.1, dy * 0.1, 0);
	}

	updateGL();
}

/**
 * This function is called once before the first call to paintGL() or resizeGL().
 */
void GLWidget3D::initializeGL()
{
	glClearColor(0.443, 0.439, 0.458, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	static GLfloat lightPosition[4] = {0.0f, 0.0f, 100.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	loadOBJ("../models/triangle.obj");
}

/**
 * This function is called whenever the widget has been resized.
 */
void GLWidget3D::resizeGL(int width, int height) {
	height = height ? height : 1;

	glViewport(0, 0, (GLint)width, (GLint)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (GLfloat)width/(GLfloat)height, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.applyCamTransform();
}

/**
 * This function is called whenever the widget needs to be painted.
 */
void GLWidget3D::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	camera.applyCamTransform();

	// First pass: render the geometry
	glPolygonMode(GL_BACK, GL_FILL);	// we want to fill with geometry
	glCullFace(GL_BACK);				// don't draw back-facing
	drawScene();

	// Second pass: silhouette rendering
	glPolygonMode(GL_BACK, GL_LINE);   // Draw backfacing as wireframe
	glLineWidth(5);   // Set the line width
	glCullFace(GL_FRONT);   // Don't Draw Any Front-Facing Polygons
	glDepthFunc(GL_LEQUAL); // 新しいdepthが、該当ピクセルのdepthバッファ値と同じ、または小さければ、新しい色で更新する
							// これにより、outlineのline描画が上書きされる！
							// 一方、完全に内側のlineは、手前のfaceよりも完全に後ろなので、上書きされない！
	glDisable(GL_TEXTURE_2D);
	drawScene();

}

/**
 * Draw the scene.
 */
void GLWidget3D::drawScene()
{
    glColor3f(1.0f, 0.0f, 0.0f);
	for (int i = 0; i < vertices.size(); i += 3) {
		glBegin(GL_TRIANGLES);
		for (int j = 0; j < 3; ++j) {
			glColor3f(vertices[i + j].color[0], vertices[i + j].color[1], vertices[i + j].color[2]);
			glNormal3f(vertices[i + j].normal[0], vertices[i + j].normal[1], vertices[i + j].normal[2]);
			glVertex3f(vertices[i + j].position[0], vertices[i + j].position[1], vertices[i + j].position[2]);
		}
		glEnd();
	}
}

/**
 * Load an OBJ file and create the corresponding VAO.
 */
void GLWidget3D::loadOBJ(char* filename) {
	OBJLoader::load(filename, vertices);
}

