#pragma once
#include "includes.h"
#include <vector>
#include <fstream>


class Shader {

public:
	Shader(); //init without compiling
	Shader(const std::string& vsf, const std::string& fsf); //compiles on declaration
	Shader(const std::string& vsf, const std::string& fsf, const std::vector<std::string>& flags); //flags
	~Shader();
	void release(); //deletes programs
	//loading
	bool load(); //needs setting filenames first
	bool load(const std::string& vsf, const std::string& fsf);
	bool load(const std::string& vsf, const std::string& fsf, const std::vector<std::string>& flags); //not const as params are modified
	void setFilenames(const std::string& vsf, const std::string& fsf); //set but not compile
	
	GLuint program;
	bool compiled;

private:
	//reading and compiling
	bool readFile_(const std::string& filename, std::string& content);
	bool compileFromStrings_(const std::string& vsm, const std::string& fsm);
	//creating objects
	bool createVertexShaderObject_(const std::string& shader);
	bool createFragmentShaderObject_(const std::string& shader);
	bool createShaderObject_(unsigned int type, GLuint& handle, const std::string& shader);
	//logging
	void saveShaderInfoLog_(GLuint obj);
	void saveProgramInfoLog_(GLuint obj);

	std::string vs_filename_;
	std::string fs_filename_;
	GLuint vs_;
	GLuint fs_;
	std::vector<std::string> preprocessor_flags_;
	std::string log_;
};

