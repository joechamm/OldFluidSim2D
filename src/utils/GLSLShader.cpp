#include "GLSLShader.h"

#include <GL/glew.h>
#include<fstream>
#include<iostream>

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

bool GLSLShader::LoadFromString(GLenum type, const char * source)
{
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &source, NULL);

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
		return false;
	}
	_shaders[_totalShaders++] = shader;
	return true;
}

bool GLSLShader::CreateProgram(void)
{
	_program = glCreateProgram();
	return true;
}

bool GLSLShader::LinkProgram(void)
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
		return false;
	}

	glDeleteShader(_shaders[VERTEX_SHADER]);
	glDeleteShader(_shaders[FRAGMENT_SHADER]);
	glDeleteShader(_shaders[GEOMETRY_SHADER]);
	glDeleteShader(_shaders[TESS_CONTROL_SHADER]);
	glDeleteShader(_shaders[TESS_EVAL_SHADER]);

	return true;
}

bool GLSLShader::LinkProgramWithTFF(GLenum type, GLuint numOutputs, const char** outputs)
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
		return false;
	}

	glDeleteShader(_shaders[VERTEX_SHADER]);
	glDeleteShader(_shaders[FRAGMENT_SHADER]);
	glDeleteShader(_shaders[GEOMETRY_SHADER]);
	glDeleteShader(_shaders[TESS_CONTROL_SHADER]);
	glDeleteShader(_shaders[TESS_EVAL_SHADER]);

	return false;
}

bool GLSLShader::CreateAndLinkProgram()
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
		return false;
	}

	glDeleteShader(_shaders[VERTEX_SHADER]);
	glDeleteShader(_shaders[FRAGMENT_SHADER]);
	glDeleteShader(_shaders[GEOMETRY_SHADER]);
	glDeleteShader(_shaders[TESS_CONTROL_SHADER]);
	glDeleteShader(_shaders[TESS_EVAL_SHADER]);

	return true;
}

void GLSLShader::Use()
{
	glUseProgram(_program);
}

void GLSLShader::UnUse()
{
	glUseProgram(0);
}

GLuint GLSLShader::AddAttribute(const char * attribute)
{
	GLuint idx = glGetAttribLocation(_program, attribute);
	_attributeList[attribute] = idx;
	return idx;
}

//An indexer that returns the location of the attribute
GLuint GLSLShader::operator [](const char * attribute)
{
	return _attributeList[attribute];
}

GLuint GLSLShader::AddUniform(const char * uniform)
{
	GLuint idx = glGetUniformLocation(_program, uniform);
	_uniformLocationList[uniform] = idx;
	return idx;
}

GLuint GLSLShader::AddUniformBlock(const char * uniformBlock)
{
	GLuint idx = glGetUniformBlockIndex(_program, uniformBlock);
	_uniformBlockList[uniformBlock] = idx;
	glUniformBlockBinding(_program, idx, _currentBlockIdx);
	_currentBlockIdx++;
	return idx;
}

GLuint GLSLShader::operator()(const char * uniform)
{
	return _uniformLocationList[uniform];
}

GLuint GLSLShader::GetProgram() const
{
	return _program;
}

GLuint GLSLShader::GetUniformBlockIndex(const char * uniformBlock)
{
	return _uniformBlockList[uniformBlock];
}

bool GLSLShader::LoadFromFile(GLenum whichShader, const char * filename)
{
	ifstream fp;
	fp.open(filename, ios_base::in);
	if (fp)
	{
		/*string line, buffer;
		while(getline(fp, line)) {
			buffer.append(line);
			buffer.append("\r\n");
		}		*/
		string buffer(std::istreambuf_iterator<char>(fp), (std::istreambuf_iterator<char>()));
		//copy to source
		return LoadFromString(whichShader, buffer.c_str());
	}
	else
	{
		cerr << "Error loading shader: " << filename << endl;
		return false;
	}
}

void GLSLShader::DeleteProgram()
{
	if (_program != 0) {
		glDeleteProgram(_program);
	}
	if (_shaders[VERTEX_SHADER] != 0) {
		glDeleteShader(_shaders[VERTEX_SHADER]);
		_shaders[VERTEX_SHADER] = 0;
	}
	if (_shaders[FRAGMENT_SHADER] != 0) {
		glDeleteShader(_shaders[FRAGMENT_SHADER]);
		_shaders[FRAGMENT_SHADER] = 0;
	}
	if (_shaders[GEOMETRY_SHADER] != 0) {
		glDeleteShader(_shaders[GEOMETRY_SHADER]);
		_shaders[GEOMETRY_SHADER] = 0;
	}
	if (_shaders[TESS_CONTROL_SHADER] != 0) {
		glDeleteShader(_shaders[TESS_CONTROL_SHADER]);
		_shaders[TESS_CONTROL_SHADER] = 0;
	}
	if (_shaders[TESS_EVAL_SHADER] != 0) {
		glDeleteShader(_shaders[TESS_EVAL_SHADER]);
		_shaders[TESS_EVAL_SHADER] = 0;
	}
	glDeleteProgram(_program);
	_program = 0;
}