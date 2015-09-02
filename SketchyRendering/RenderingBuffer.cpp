#include "RenderingBuffer.h"

RenderingBuffer::RenderingBuffer() {
}

/**
 * レンダリングバッファの初期化。
 * 本関数は、GLWidget3D::initializeGL()内で呼び出すこと。
 *
 * @param programId		シェイダーのprogram id
 * @param width			シャドウマッピングの幅
 * @param height		シャドウマッピングの高さ
 */
void RenderingBuffer::init(int programId, int width, int height) {
	this->programId = programId;
	this->width = width;
	this->height = height;

	// FBO作成
	glGenFramebuffers(1, &fboNormal);
	glBindFramebuffer(GL_FRAMEBUFFER, fboNormal);
		
	// normalバッファを保存するための2Dテクスチャを作成
	glGenTextures(1, &textureNormal);

	// GL_TEXTURE5に、このバッファをbindすることで、
	// シェーダからは5番でアクセスできる
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, textureNormal);

	// テクスチャパラメータの設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // テクスチャ領域の確保(GL_DEPTH_COMPONENTを用いる)
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);

	// 生成した2Dテクスチャを、デプスバッファとしてfboに括り付ける。
	// 以後、このfboに対するレンダリングを実施すると、デプスバッファのデータは
	// この2Dテクスチャに自動的に保存される。
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureNormal, 0);


	// depthバッファを保存するための2Dテクスチャを作成
	glGenTextures(1, &textureDepth);

	// GL_TEXTURE6に、このバッファをbindすることで、
	// シェーダからは6番でアクセスできる
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, textureDepth);

	// テクスチャパラメータの設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// テクスチャの外側、つまり、光源の外側は、影ってことにする(?)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// ただ、そもそも光源の外にならないよう、projection行列を設定すべき！
		
    // テクスチャ領域の確保(GL_DEPTH_COMPONENTを用いる)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	// 生成した2Dテクスチャを、デプスバッファとしてfboに括り付ける。
	// 以後、このfboに対するレンダリングを実施すると、デプスバッファのデータは
	// この2Dテクスチャに自動的に保存される。
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepth, 0);
	
	glActiveTexture(GL_TEXTURE0);

		
	// シェーダに、normalバッファとdepthバッファの番号を伝える
	glUniform1i(glGetUniformLocation(programId, "normalMap"), 5);
	glUniform1i(glGetUniformLocation(programId, "depthMap"), 6);

	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void RenderingBuffer::update(int width, int height) {
	/*
	this->width = width;
	this->height = height;

	glDeleteTextures(1, &textureNormal);
	glDeleteFramebuffers(1, &fboNormal);

	// FBO作成
	glGenFramebuffers(1, &fboNormal);
	glBindFramebuffer(GL_FRAMEBUFFER, fboNormal);
		
	// normalバッファを保存するための2Dテクスチャを作成
	glGenTextures(1, &textureNormal);

	// GL_TEXTURE5に、このバッファをbindすることで、
	// シェーダからは5番でアクセスできる
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, textureNormal);

	// テクスチャパラメータの設定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // テクスチャ領域の確保(GL_DEPTH_COMPONENTを用いる)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);

	// 生成した2Dテクスチャを、デプスバッファとしてfboに括り付ける。
	// 以後、このfboに対するレンダリングを実施すると、デプスバッファのデータは
	// この2Dテクスチャに自動的に保存される。
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureNormal, 0);
	*/
}
