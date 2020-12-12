#pragma once

#include <map>
#include <string>
#include <vector>

using namespace std;

typedef unsigned int GLenum;
typedef unsigned int GLuint;

class GLSLShader
{
public:
	GLSLShader(void);
	~GLSLShader(void);
	bool LoadFromString(GLenum whichShader, const char * source);
	bool LoadFromFile(GLenum whichShader, const char * filename);	
	bool CreateProgram(void);
	bool LinkProgram(void);
	bool LinkProgramWithTFF(GLenum type, GLuint numOutputs, const char** outputs);
	bool CreateAndLinkProgram();
	void Use();
	void UnUse();
	GLuint AddAttribute(const char * attribute);
	GLuint AddUniform(const char * uniform);
	GLuint AddUniformBlock(const char * uniformBlock);
	GLuint GetProgram() const;
	//An indexer that returns the location of the attribute/uniform
	GLuint operator[](const char * attribute);
	GLuint operator()(const char * uniform);
	GLuint GetUniformBlockIndex(const char * uniformBlock);
	//Program deletion
	void DeleteProgram();
private:
	enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER, TESS_CONTROL_SHADER, TESS_EVAL_SHADER };
	GLuint	_program;
	GLuint  _currentBlockIdx;
	int _totalShaders;
	GLuint _shaders[5];//0->vertexshader, 1->fragmentshader, 2->geometryshader, 3->controlshader, 4->evaluationshader
	map<string, GLuint> _attributeList;
	map<string, GLuint> _uniformLocationList;
	map<string, GLuint> _uniformBlockList;
};

