#pragma once

#include <glew.h>
#include <QGLWidget>

class RenderingBuffer {
public:
public:
	int width;
	int height;

	int programId;

	uint fboNormal;
	uint textureNormal;
	uint fboDepth;
	uint textureDepth;

public:
	RenderingBuffer();

	void init(int programId, int width, int height);
	void update(int width, int height);
};

