#include "GLWidget3D.h"
#include "MainWindow.h"
#include "OBJLoader.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

GLWidget3D::GLWidget3D(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers)) {
	// 光源位置をセット
	// ShadowMappingは平行光源を使っている。この位置から原点方向を平行光源の方向とする。
	light_dir = glm::normalize(glm::vec3(-4, -5, -8));

	// シャドウマップ用のmodel/view/projection行列を作成
	/*glm::mat4 light_pMatrix = glm::ortho<float>(-50, 50, -50, 50, 0.1, 200);
	glm::mat4 light_mvMatrix = glm::lookAt(-light_dir * 50.0f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	light_mvpMatrix = light_pMatrix * light_mvMatrix;*/
}

/**
 * This event handler is called when the mouse press events occur.
 */
void GLWidget3D::mousePressEvent(QMouseEvent *e) {
	camera.mousePress(e->x(), e->y());
}

/**
 * This event handler is called when the mouse release events occur.
 */
void GLWidget3D::mouseReleaseEvent(QMouseEvent *e) {
}

/**
 * This event handler is called when the mouse move events occur.
 */
void GLWidget3D::mouseMoveEvent(QMouseEvent *e) {
	if (e->buttons() & Qt::LeftButton) { // Rotate
		camera.rotate(e->x(), e->y());
	} else if (e->buttons() & Qt::MidButton) { // Move
		camera.move(e->x(), e->y());
	} else if (e->buttons() & Qt::RightButton) { // Zoom
		camera.zoom(e->x(), e->y());
	}

	updateGL();
}

/**
 * This function is called once before the first call to paintGL() or resizeGL().
 */
void GLWidget3D::initializeGL() {
	renderManager.init("../shaders/vertex.glsl", "../shaders/geometry.glsl", "../shaders/fragment.glsl");

	// set the clear color for the screen
	//qglClearColor(QColor(113, 112, 117));

	rb.init(renderManager.program, width(), height());
}

/**
 * This function is called whenever the widget has been resized.
 */
void GLWidget3D::resizeGL(int width, int height) {
	height = height ? height : 1;
	glViewport(0, 0, width, height);
	camera.updatePMatrix(width, height);

	rb.update(width, height);
}

/**
 * This function is called whenever the widget needs to be painted.
 */
void GLWidget3D::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	// Model view projection行列をシェーダに渡す
	glUniformMatrix4fv(glGetUniformLocation(renderManager.program, "mvpMatrix"),  1, GL_FALSE, &camera.mvpMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(renderManager.program, "mvMatrix"),  1, GL_FALSE, &camera.mvMatrix[0][0]);

	// pass the light direction to the shader
	//glUniform1fv(glGetUniformLocation(renderManager.program, "lightDir"), 3, &light_dir[0]);
	glUniform3f(glGetUniformLocation(renderManager.program, "lightDir"), light_dir.x, light_dir.y, light_dir.z);
	

	//// normalバッファ、depthバッファを更新
	glBindFramebuffer(GL_FRAMEBUFFER, rb.fboNormal);
	glEnable(GL_TEXTURE_2D);

	glUniform1i(glGetUniformLocation(renderManager.program, "pass"), 1);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.1f, 4.0f);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawScene(0);


	glUniform1i(glGetUniformLocation(renderManager.program, "pass"), 2);
	glUniform1i(glGetUniformLocation(renderManager.program, "screenWidth"), width());
	glUniform1i(glGetUniformLocation(renderManager.program, "screenHeight"), height());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.443, 0.439, 0.458, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawScene(0);
	
}

/**
 * Draw the scene.
 */
void GLWidget3D::drawScene(int drawMode) {	
	renderManager.removeObjects();

	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(glm::vec3(-10, 0, 0), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1)));
	vertices.push_back(Vertex(glm::vec3(10, 0, 0), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1)));
	vertices.push_back(Vertex(glm::vec3(0, 10, 0), glm::vec3(0, 0, 1), glm::vec3(1, 1, 1)));

	vertices.push_back(Vertex(glm::vec3(-30, 0, -30), glm::vec3(0, 1, 0), glm::vec3(1, 0.8, 0.8)));
	vertices.push_back(Vertex(glm::vec3(-30, 0, 30), glm::vec3(0, 1, 0), glm::vec3(1, 0.8, 0.8)));
	vertices.push_back(Vertex(glm::vec3(30, 0, 30), glm::vec3(0, 1, 0), glm::vec3(1, 0.8, 0.8)));

	vertices.push_back(Vertex(glm::vec3(-30, 0, -30), glm::vec3(0, 1, 0), glm::vec3(1, 0.8, 0.8)));
	vertices.push_back(Vertex(glm::vec3(30, 0, 30), glm::vec3(0, 1, 0), glm::vec3(1, 0.8, 0.8)));
	vertices.push_back(Vertex(glm::vec3(30, 0, -30), glm::vec3(0, 1, 0), glm::vec3(1, 0.8, 0.8)));
	renderManager.addObject("object", "", vertices);

	renderManager.renderAll();
}

