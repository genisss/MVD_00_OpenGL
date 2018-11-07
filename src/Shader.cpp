//
//  Shader.cpp
//
//  Copyright © 2018 Alun Evans. All rights reserved.
//
#include "Shader.h"
#include <string>
#include <sstream>

// Default constructor, creates object with uncompiled shader
Shader::Shader()
{
	vs_filename_ = "";
	fs_filename_ = "";
	compiled = false;
}

// Constructor which accepts paths to the shaders and compiles them
// vsf: path to vertex shader
// fsf: path to fragment shader
Shader::Shader(const std::string& vsf, const std::string& fsf)
{
	vs_filename_ = vsf;
	fs_filename_ = fsf;
	compiled = false;
	if (!load(vs_filename_, fs_filename_)) {
		std::cerr << "ERROR: Failed to compile shader " << vs_filename_ << ", " << fs_filename_ << std::endl;
	}
}

// Constructor which accepts paths to the shaders and compiles them. Also receives a std::vector
// of shader preprocessor flags (e.g "#define XXX") which will be injected into the shader string before compilation.
// This permits us to use #ifdef preprocessor commands in the shader to 'activate' different sections of code
// vsf: path to vertex shader
// fsf: path to fragment shader
// flags: std::vector of string containing lines to inject at start of shader
Shader::Shader(const std::string& vsf, const std::string& fsf, const std::vector<std::string>& flags)
{
	vs_filename_ = vsf;
	fs_filename_ = fsf;
	compiled = false;
	if (!load(vs_filename_, fs_filename_, flags)) {
		std::cerr << "ERROR: Failed to compile shader " << vs_filename_ << ", " << fs_filename_ << std::endl;
	}
}

// Destructor frees memory
Shader::~Shader()
{
	release();
}

// ****************************
// Public functions
// ****************************

// Frees memory and deletes program
void Shader::release()
{
	if (vs_)
	{
		glDeleteShader(vs_);
		glCheckError();
		vs_ = 0;
	}

	if (fs_)
	{
		glDeleteShader(fs_);
		glCheckError();
		fs_ = 0;
	}

	if (program)
	{
		glDeleteProgram(program);
		glCheckError();
		program = 0;
	}

	compiled = false;
}

// Compiles shader using file paths stores in member variables. Injects preprocessor flags if present
bool Shader::load()
{
	if (vs_filename_ == "" | fs_filename_ == "") {
		std::cerr << "ERROR: Can't load shader - filenames not set." << std::endl;
		return false;
	}

	if (compiled) {
		std::cerr << "ERROR: Trying to load pre-compiled shader " << vs_filename_ << ", " << fs_filename_ << std::endl;
		return false;
	}

	std::cout << " * Shader: Vertex: " << vs_filename_ << "  Pixel: " << fs_filename_ << std::endl;
	std::string vs_content, fs_content;
	if (!readFile_(vs_filename_, vs_content) || !readFile_(fs_filename_, fs_content))
		return false;

	//inject preprocessor flags at start of each shader - should go after version directive
	size_t insert_offset = 0;
	size_t found = vs_content.find("#version"); //look for version directive
	if (found != std::string::npos)
		insert_offset = vs_content.find('\n') + 1; // if it exists, insert preprocessor directives after end of line
	else
		std::cerr << "WARNING: Could not find version directive in vertex shader\n";
	for (auto f : preprocessor_flags_) {
		vs_content.insert(insert_offset, (f + "\n"));
	}

	insert_offset = 0;
	found = fs_content.find("#version"); //look for version directive
	if (found != std::string::npos) 
		insert_offset = fs_content.find('\n') + 1; // if it exists, insert preprocessor directives after end of line
	else
		std::cerr << "WARNING: Could not find version directive in fragment shader\n";
	for (auto f : preprocessor_flags_) {
		fs_content.insert(insert_offset, (f + "\n"));
	}

	if (!compileFromStrings_(vs_content, fs_content))
		return false;

	glCheckError();

	std::cout << " * Compiled shader okay!" << std::endl;

	compiled = true;
	return true;
}

// Compiles shader with paths passed as parameters 
// vsf: path to vertex shader
// fsf: path to fragment shader
bool Shader::load(const std::string& vsf, const std::string& fsf)
{
	if (compiled) {
		std::cerr << "ERROR: Trying to load pre-compiled shader " << vsf << ", " << fsf << std::endl;
	}

	vs_filename_ = vsf;
	fs_filename_ = fsf;

	return load();
}

// Compiles shader with paths passed as parameters. Also receives a std::vector
// of shader preprocessor flags (e.g "#define XXX") which will be injected into the shader string before compilation.
// This permits us to use #ifdef preprocessor commands in the shader to 'activate' different sections of code
// vsf: path to vertex shader
// fsf: path to fragment shader
// flags: std::vector of string containing lines to inject at start of shader
bool Shader::load(const std::string& vsf, const std::string& fsf, const std::vector<std::string>& flags) {
	
	preprocessor_flags_ = flags;

	return load(vsf, fsf);
}

// Sets the member variables for the paths to the shader code files
void Shader::setFilenames(const std::string& vsf, const std::string& fsf)
{
	vs_filename_ = vsf;
	fs_filename_ = fsf;
}

// ****************************
// Private functions
// ****************************

// Reads file into a string
bool Shader::readFile_(const std::string& filename, std::string& content) {
	
	std::ifstream f(filename);
	std::stringstream buffer;
	buffer << f.rdbuf();
	content = buffer.str();

	return true;
}

//compiles shader from string
bool Shader::compileFromStrings_(const std::string& vsm, const std::string& fsm) {
	
	program = glCreateProgram();
	glCheckError();

	if (!createVertexShaderObject_(vsm))
	{
		std::cerr << "ERROR: Vertex shader compilation failed" << std::endl;
		return false;
	}

	if (!createFragmentShaderObject_(fsm))
	{
		std::cerr << "ERROR: Fragment shader compilation failed" << std::endl;
		return false;
	}

	glLinkProgram(program);
	glCheckError();

	GLint linked = 0;

	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	glCheckError();

	if (!linked)
	{
		saveProgramInfoLog_(program);
		release();
		return false;
	}

	compiled = true;

	return true;
}

// prints log if shader did not compile
void Shader::saveShaderInfoLog_(GLuint obj)
{
	int len = 0;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &len);
	glCheckError();

	if (len > 0)
	{
		char* ptr = new char[len + 1];
		GLsizei written = 0;
		glGetShaderInfoLog(obj, len, &written, ptr);
		ptr[written - 1] = '\0';
		glCheckError();
		log_.append(ptr);
		delete[] ptr;

		printf("LOG **********************************************\n%s\n", log_.c_str());
	}
}

// prints log if shader did not compile
void Shader::saveProgramInfoLog_(GLuint obj)
{
	int len = 0;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &len);
	glCheckError();

	if (len > 0)
	{
		char* ptr = new char[len + 1];
		GLsizei written = 0;
		glGetProgramInfoLog(obj, len, &written, ptr);
		ptr[written - 1] = '\0';
		glCheckError();
		log_.append(ptr);
		delete[] ptr;

		printf("LOG **********************************************\n%s\n", log_.c_str());
	}
}

// creates a vertex shader object
bool Shader::createVertexShaderObject_(const std::string& shader)
{
	return createShaderObject_(GL_VERTEX_SHADER, vs_, shader);
}


// creates a fragment shader object
bool Shader::createFragmentShaderObject_(const std::string& shader)
{
	return createShaderObject_(GL_FRAGMENT_SHADER, fs_, shader);
}

// standalone helper function that splits a string by a delimiting character
std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

// creates a global shader object
bool Shader::createShaderObject_(unsigned int type, GLuint& handle, const std::string& code)
{
	handle = glCreateShader(type);

	const char* ptr = code.c_str();
	glShaderSource(handle, 1, &ptr, NULL);

	glCompileShader(handle);

	GLint compile = 0;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &compile);

	//we want to see the compile log if we are in debug (to check warnings)
	if (!compile)
	{
		saveShaderInfoLog_(handle);
		std::cout << "Shader code:\n " << std::endl;
		std::vector<std::string> lines = split(code, '\n');
		for (size_t i = 0; i < lines.size(); ++i)
			std::cout << i << "  " << lines[i] << std::endl;

		return false;
	}

	//attach shader
	glAttachShader(program, handle);

	glCheckError();
	return true;
}