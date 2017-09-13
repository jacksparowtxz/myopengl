#pragma once


#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	Shader(const char* vertexPath, const char* fragmentPath)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();
		// 2. compile shaders
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	size_t TypeSize(GLenum type)
	{
		size_t size;
#define CASE(Enum,Count,Type)\
        case Enum :size=Count*sizeof(Type);break

		switch (type)
		{
			CASE(GL_FLOAT, 1, GLfloat);
			CASE(GL_FLOAT_VEC2, 2, GLfloat);
			CASE(GL_FLOAT_VEC3, 3, GLfloat);
			CASE(GL_FLOAT_VEC4, 4, GLfloat);
			CASE(GL_INT, 1, GLint);
			CASE(GL_INT_VEC2, 2, GLint);
			CASE(GL_INT_VEC3, 3, GLint);
			CASE(GL_INT_VEC4, 4, GLint);
			CASE(GL_UNSIGNED_INT, 1, GLuint);
			CASE(GL_UNSIGNED_INT_VEC2, 2, GLuint);
			CASE(GL_UNSIGNED_INT_VEC3, 3, GLuint);
			CASE(GL_UNSIGNED_INT_VEC4, 4, GLuint);
			CASE(GL_BOOL, 1, GLboolean);
			CASE(GL_BOOL_VEC2, 2, GLboolean);
			CASE(GL_BOOL_VEC3, 3, GLboolean);
			CASE(GL_BOOL_VEC4, 4, GLboolean);
			CASE(GL_FLOAT_MAT2, 4, GLfloat);
			CASE(GL_FLOAT_MAT2x3, 6, GLfloat);
			CASE(GL_FLOAT_MAT2x4, 8, GLfloat);
			CASE(GL_FLOAT_MAT3, 9, GLfloat);
			CASE(GL_FLOAT_MAT3x2, 6, GLfloat);
			CASE(GL_FLOAT_MAT3x4, 12, GLfloat);
			CASE(GL_FLOAT_MAT4, 16, GLfloat);
			CASE(GL_FLOAT_MAT4x2, 8, GLfloat);
			CASE(GL_FLOAT_MAT4x3, 12, GLfloat);
#undef CASE
		default:
			std::cout << stderr << "UNKONW" << type;
			exit(EXIT_FAILURE);
			break;
		}
		return size;
	}
	// activate the shader
	// ------------------------------------------------------------------------
	void use()
	{
		glUseProgram(ID);
	}
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setUniformBlock()
	{
		const GLchar *name="BlobSettings";
		GLuint blockIndex = glGetUniformBlockIndex(ID,
			"BlobSettings");
		GLint blockSize;
		glGetActiveUniformBlockiv(ID,
			blockIndex,
			GL_UNIFORM_BLOCK_DATA_SIZE,
			&blockSize);
		GLubyte *blockBuffer = (GLubyte*)malloc(blockSize);
		if (blockBuffer==nullptr)
		{
			std::cout << "fail to allocate buffer";
		}
		//Query for the offsets of the each block variable  
		//layout of the data within a uniform block is implementation dependent  
		const GLchar *names[] = { "InnerColor","OuterColor",
			"RadiusInner","RadiusOuter" };
		GLuint indices[4];
		glGetUniformIndices(ID, 4, names, indices);

		GLint offset[4];
		glGetActiveUniformsiv(ID, 4, indices, GL_UNIFORM_OFFSET, offset);

		GLfloat outerColor[] = { 5.0f, 5.0f, 5.0f,5.0f };
		GLfloat innerColor[] = { 1.0f, 1.0f, 0.75f, 1.0f };
		GLfloat innerRadius = 5.25f, outerRadius = 5.45f;

		memcpy(blockBuffer + offset[0], innerColor, 4 * sizeof(GLfloat));
		memcpy(blockBuffer + offset[1], outerColor, 4 * sizeof(GLfloat));
		memcpy(blockBuffer + offset[2], &innerRadius, sizeof(GLfloat));
		memcpy(blockBuffer + offset[3], &outerRadius, sizeof(GLfloat));

		//create OpenGL buffer UBO to store the data  
		GLuint uboHandle;
		glGenBuffers(1, &uboHandle);
		glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
		glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer,
			GL_DYNAMIC_DRAW);

		//bind the UBO th the uniform block  
		glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uboHandle);

		free(blockBuffer);
	}

private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};
#endif

