#pragma once

#include <glew.h>
#include <QGLWidget>

class RenderingBuffer {
public:
public:
	int width;
	int height;

	int programId;

	uint fbo;
	uint textureNormal;
	uint textureDepth;

public:
	RenderingBuffer();

	void init(int programId, int width, int height);
	void update(int width, int height);
};

