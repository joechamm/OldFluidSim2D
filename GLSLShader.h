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
	void LoadFromString(GLenum whichShader, const string source);
	void LoadFromFile(GLenum whichShader, const string filename);
	void LoadFromFileWithPPD(GLenum whichShader, const string filename, const vector<string>& preProcessorDirectives);
	void CreateProgram(void);
	void LinkProgram(void);
	void LinkProgramWithTFF(GLenum type, GLuint numOutputs, const char** outputs);
	void CreateAndLinkProgram();
	void Use();
	void UnUse();
	GLuint AddAttribute(const string attribute);
	GLuint AddUniform(const string uniform);
	GLuint AddUniformBlock(const string uniformBlock);
	GLuint GetProgram() const;
	//An indexer that returns the location of the attribute/uniform
	GLuint operator[](const string attribute);
	GLuint operator()(const string uniform);
	GLuint GetUniformBlockIndex(const string uniformBlock);
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

