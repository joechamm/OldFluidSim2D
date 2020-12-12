#include "GLSLShader.h"

//A simple class for handling GLSL shader compilation
//Author: Movania Muhammad Mobeen
//Last Modified: February 2, 2011

//#include<gl/glew.h>
#include <GL/glew.h>
#include<fstream>
#include<iostream>

//#pragma comment(lib,"glew32.lib")

GLSLShader::GLSLShader(void) : _program(0),
	_totalShaders(0),
	_currentBlockIdx(0),
	_attributeList(),
	_uniformBlockList(),
	_uniformLocationList()
{
	_shaders[VERTEX_SHADER] = 0;
	_shaders[FRAGMENT_SHADER] = 0;
	_shaders[GEOMETRY_SHADER] = 0;
	_shaders[TESS_CONTROL_SHADER] = 0;
	_shaders[TESS_EVAL_SHADER] = 0;
}

GLSLShader::~GLSLShader(void)
{
	_attributeList.clear();
	_uniformLocationList.clear();
	_uniformBlockList.clear();
}

void GLSLShader::LoadFromString(GLenum type, const string source)
{
	GLuint shader = glCreateShader(type);

	const char* ptmp = source.c_str();
	glShaderSource(shader, 1, &ptmp, NULL);

	//check whether the shader loads fine
	GLint status;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* infoLog = new GLchar[infoLogLength];
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
		cerr << "Compile log: " << infoLog << endl;
		delete[] infoLog;
	}
	_shaders[_totalShaders++] = shader;
}

void GLSLShader::CreateProgram(void)
{
	_program = glCreateProgram();
}

void GLSLShader::LinkProgram(void)
{
	if (_shaders[VERTEX_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[VERTEX_SHADER]);
	}
	if (_shaders[FRAGMENT_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[FRAGMENT_SHADER]);
	}
	if (_shaders[GEOMETRY_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[GEOMETRY_SHADER]);
	}
	if (_shaders[TESS_CONTROL_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[TESS_CONTROL_SHADER]);
	}
	if (_shaders[TESS_EVAL_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[TESS_EVAL_SHADER]);
	}

	//link and check whether the program links fine
	GLint status;
	glLinkProgram(_program);
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;

		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(_program, infoLogLength, NULL, infoLog);
		cerr << "Link log: " << infoLog << endl;
		delete[] infoLog;
	}

	glDeleteShader(_shaders[VERTEX_SHADER]);
	glDeleteShader(_shaders[FRAGMENT_SHADER]);
	glDeleteShader(_shaders[GEOMETRY_SHADER]);
	glDeleteShader(_shaders[TESS_CONTROL_SHADER]);
	glDeleteShader(_shaders[TESS_EVAL_SHADER]);
}

void GLSLShader::LinkProgramWithTFF(GLenum type, GLuint numOutputs, const char** outputs)
{
	if (_shaders[VERTEX_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[VERTEX_SHADER]);
	}
	if (_shaders[FRAGMENT_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[FRAGMENT_SHADER]);
	}
	if (_shaders[GEOMETRY_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[GEOMETRY_SHADER]);
	}
	if (_shaders[TESS_CONTROL_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[TESS_CONTROL_SHADER]);
	}
	if (_shaders[TESS_EVAL_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[TESS_EVAL_SHADER]);
	}

	glTransformFeedbackVaryings(_program, numOutputs, outputs, type);

	//link and check whether the program links fine
	GLint status;
	glLinkProgram(_program);
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;

		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(_program, infoLogLength, NULL, infoLog);
		cerr << "Link log: " << infoLog << endl;
		delete[] infoLog;
	}

	glDeleteShader(_shaders[VERTEX_SHADER]);
	glDeleteShader(_shaders[FRAGMENT_SHADER]);
	glDeleteShader(_shaders[GEOMETRY_SHADER]);
	glDeleteShader(_shaders[TESS_CONTROL_SHADER]);
	glDeleteShader(_shaders[TESS_EVAL_SHADER]);
}

void GLSLShader::CreateAndLinkProgram()
{
	_program = glCreateProgram();
	if (_shaders[VERTEX_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[VERTEX_SHADER]);
	}
	if (_shaders[FRAGMENT_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[FRAGMENT_SHADER]);
	}
	if (_shaders[GEOMETRY_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[GEOMETRY_SHADER]);
	}
	if (_shaders[TESS_CONTROL_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[TESS_CONTROL_SHADER]);
	}
	if (_shaders[TESS_EVAL_SHADER] != 0)
	{
		glAttachShader(_program, _shaders[TESS_EVAL_SHADER]);
	}

	//link and check whether the program links fine
	GLint status;
	glLinkProgram(_program);
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;

		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(_program, infoLogLength, NULL, infoLog);
		cerr << "Link log: " << infoLog << endl;
		delete[] infoLog;
	}

	glDeleteShader(_shaders[VERTEX_SHADER]);
	glDeleteShader(_shaders[FRAGMENT_SHADER]);
	glDeleteShader(_shaders[GEOMETRY_SHADER]);
	glDeleteShader(_shaders[TESS_CONTROL_SHADER]);
	glDeleteShader(_shaders[TESS_EVAL_SHADER]);
}

void GLSLShader::Use()
{
	glUseProgram(_program);
}

void GLSLShader::UnUse()
{
	glUseProgram(0);
}

GLuint GLSLShader::AddAttribute(const string attribute)
{
	GLuint idx = glGetAttribLocation(_program, attribute.c_str());
	_attributeList[attribute] = idx;
	return idx;
}

//An indexer that returns the location of the attribute
GLuint GLSLShader::operator [](const string attribute)
{
	return _attributeList[attribute];
}

GLuint GLSLShader::AddUniform(const string uniform)
{
	GLuint idx = glGetUniformLocation(_program, uniform.c_str());
	_uniformLocationList[uniform] = idx;
	return idx;
}

GLuint GLSLShader::AddUniformBlock(const string uniformBlock)
{
	GLuint idx = glGetUniformBlockIndex(_program, uniformBlock.c_str());
	_uniformBlockList[uniformBlock] = idx;
	glUniformBlockBinding(_program, idx, _currentBlockIdx);
	_currentBlockIdx++;
	return idx;
}

GLuint GLSLShader::operator()(const string uniform)
{
	return _uniformLocationList[uniform];
}

GLuint GLSLShader::GetProgram() const
{
	return _program;
}

GLuint GLSLShader::GetUniformBlockIndex(const string uniformBlock)
{
	return _uniformBlockList[uniformBlock];
}

void GLSLShader::LoadFromFile(GLenum whichShader, const string filename)
{
	ifstream fp;
	fp.open(filename.c_str(), ios_base::in);
	if (fp)
	{
		/*string line, buffer;
		while(getline(fp, line)) {
			buffer.append(line);
			buffer.append("\r\n");
		}		*/
		string buffer(std::istreambuf_iterator<char>(fp), (std::istreambuf_iterator<char>()));
		//copy to source
		LoadFromString(whichShader, buffer);
	}
	else
	{
		cerr << "Error loading shader: " << filename << endl;
	}
}

void GLSLShader::LoadFromFileWithPPD(GLenum whichShader, const string filename, const vector<string>& preProcessorDirectives)
{
	ifstream fp;
	fp.open(filename.c_str(), ios_base::in);
	if (fp)
	{
		string line, buffer;
		vector<string>::const_iterator it;
		for (it = preProcessorDirectives.begin(); it != preProcessorDirectives.end(); ++it)
		{
			buffer.append(it->c_str());
			buffer.append("\r\n");
		}

		while (getline(fp, line))
		{
			buffer.append(line);
			buffer.append("\r\n");
		}
		LoadFromString(whichShader, buffer);
	}
	else
	{
		cerr << "Error loading shader: " << filename << endl;
	}
}

void GLSLShader::DeleteProgram()
{
	glDeleteProgram(_program);
	_program = -1;
}