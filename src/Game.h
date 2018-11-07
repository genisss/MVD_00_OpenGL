//
//  Game.h
//  03-Drawing
//
//  Copyright © 2018 Alun Evans. All rights reserved.
//
#pragma once
#include "includes.h"
#include "Shader.h"

struct TGAInfo //stores info about TGA file
{
	GLuint width;
	GLuint height;
	GLuint bpp; //bits per pixel
	GLubyte* data; //bytes with the pixel information
};

class Game
{
public:
	~Game();
	void init();
	void update();

private:
	GLuint vao_;
	Shader* shader_;
	GLuint texture_id_;
	GLuint loadTexture(std::string);
	TGAInfo* loadTGA(std::string);
};
