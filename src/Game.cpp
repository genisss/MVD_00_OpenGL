//
//  Game.cpp
//  03-Drawing
//
//  Copyright � 2018 Alun Evans. All rights reserved.
//

#include "Game.h"

using namespace lm;

Game::~Game() {
	delete(shader_); 
	shader_ = nullptr;
}

//Nothing here yet
void Game::init() {
	//set 'background' colour of framebuffer
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	//create buffers

	//send the data to OpenGL

	//create and bind an OpenGL Vertex Array Object (VAO) i.e. a 'container' for all the data

	//create and bind Vertex Buffer Objects (VBO) to store data on GPU

	// 'Send' our data to OpenGL.

	//tell OpenGL that this vertex array should have attribute id of '0' in shader

	//do the same for the index buffer

	//unbind buffers (optional)


    //compile shader from relative path (note project settings, output directory is same as debug working directory)
	shader_ = new Shader("data/shaders/simple.vert", "data/shaders/simple.frag");

	//load texture
	//texture_id_ = loadTexture("data/assets/test.tga");

}
//Entry point for game rendering code
void Game::update() {
	//clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//activate shader

	//create matrices

	//draw vao

	//draw our geometry

}

GLuint Game::loadTexture(std::string file)
{
	//this will be returned, id of texture on GPU
	GLuint texture_id;

	std::string str = file;
	std::string ext = str.substr(str.size() - 4, 4);

	TGAInfo* tgainfo = loadTGA(file);
	if (tgainfo == NULL) {
		std::cerr << "ERROR: Could not load TGA file" << std::endl;
		return 0;
	}

	//generate new openGL texture and bind it (tell openGL we want to do stuff with it)
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id); //we are making a regular 2D texture

											  //screen pixels will almost certainly not be same as texture pixels, so we need to
											  //set some parameters regarding the filter we use to deal with these cases
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//set the mag filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //set the min filter
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4); //use anisotropic filtering

																	  //this is function that actually loads texture data into OpenGL
	glTexImage2D(GL_TEXTURE_2D, //the target type, a 2D texture
		0, //the base level-of-detail in the mipmap
		(tgainfo->bpp == 24 ? GL_RGB : GL_RGBA), //specified the color channels for opengl
		tgainfo->width, //the width of the texture
		tgainfo->height, //the height of the texture
		0, //border - must always be 0
		(tgainfo->bpp == 24 ? GL_BGR : GL_BGRA), //the format of the incoming data
		GL_UNSIGNED_BYTE, //the type of the incoming data
		tgainfo->data); // a pointer to the incoming data

						//we want to use mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);

	//clean up memory
	delete tgainfo->data;
	delete tgainfo;
	return texture_id;

}

// this reader supports only uncompressed RGB targa files with no colour table
TGAInfo* Game::loadTGA(std::string filename)
{
	//the TGA header is 18 bytes long. The first 12 bytes are for specifying the compression
	//and various fields that are very infrequently used, and hence are usually 0.
	//for this limited file parser, we start by reading the first 12 bytes and compare
	//them against the pattern that identifies the file a simple, uncompressed RGB file.
	//more info about the TGA format cane be found at http://www.paulbourke.net/dataformats/tga/

	char TGA_uncompressed[12] = { 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	char TGA_compare[12];
	char info_header[6];
	GLuint bytes_per_pixel;
	GLuint image_size;

	//open file
	std::ifstream file(filename, std::ios::binary);

	//read first 12 bytes
	file.read(&TGA_compare[0], 12);
	std::streamsize read_header_12 = file.gcount();
	//compare to check that file in uncompressed (or not corrupted)
	int header_compare = memcmp(TGA_uncompressed, TGA_compare, sizeof(TGA_uncompressed));
	if (read_header_12 != sizeof(TGA_compare) || header_compare != 0) {
		std::cerr << "ERROR: TGA file is not in correct format or corrupted: " << filename << std::endl;
		file.close();
		return nullptr;
	}

	//read in next 6 bytes, which contain 'important' bit of header
	file.read(&info_header[0], 6);

	TGAInfo* tgainfo = new TGAInfo;

	tgainfo->width = info_header[1] * 256 + info_header[0]; //width is stored in first two bytes of info_header
	tgainfo->height = info_header[3] * 256 + info_header[2]; //height is stored in next two bytes of info_header

	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (info_header[4] != 24 && info_header[4] != 32)) {
		file.close();
		delete tgainfo;
		std::cerr << "ERROR: TGA file is not 24 or 32 bits, or has no width or height: " << filename << std::endl;
		return NULL;
	}

	//calculate bytes per pixel and then total image size in bytes
	tgainfo->bpp = info_header[4];
	bytes_per_pixel = tgainfo->bpp / 8;
	image_size = tgainfo->width * tgainfo->height * bytes_per_pixel;

	//reserve memory for the image data
	tgainfo->data = (GLubyte*)malloc(image_size);

	//read data into memory
	file.read((char*)tgainfo->data, image_size);
	std::streamsize image_read_size = file.gcount();

	//check it has been read correctly
	if (image_read_size != image_size) {
		if (tgainfo->data != NULL)
			free(tgainfo->data);
		file.close();
		std::cerr << "ERROR: Could not read tga data: " << filename << std::endl;
		delete tgainfo;
		return NULL;
	}

	file.close();

	return tgainfo;
}

