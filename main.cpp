#include<stdio.h>
#include<iostream>
#include<vector>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include "GLSLShader.h"
#include<glm/ext.hpp>

using namespace std;
using glm::value_ptr;

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"freeglut.lib")
#pragma comment(lib,"glutils.lib")

#define WINDOW_TITLE_PREFIX "FluidTest09"

static int CurrentWidth = 512;
static int CurrentHeight = 512;

const float PI = 3.14159265;

unsigned FrameCount = 0;

time_t current, initial;

GLSLShader prog_render, prog_setup;

GLsizeiptr vertPosSize, vertTexSize;

GLenum
setup_drawBuffs[2],
window_drawBuffs[2];

GLuint
vbo_handle,
fbo_handles[2],
vel_tex[2],
temp_tex[2],
setup_tex[2],
setup_tbo[2];

// GLUI Variables

int main_window;

int mouseX, mouseY;

bool mouseDown;

int
current_subroutine;

GLuint
tempSubIdx,
velSubIdx;

GLuint
fboWidth,
fboHeight;

void InitWindow(int, char**);
void InitFBO(void);
void InitShaders(void);
void InitBuffers(void);
void InitTexture(void);
void Keyboard(unsigned char, int, int);
void Mouse(int, int, int, int);
void MouseMove(int, int);
void Reshape(int, int);
void Idle(void);
void Render(void);
void Timer(int);

void InitGlobals(void);

void SetupFBO(void);

void CheckFramebuffers(void);
void PrintFBO(GLuint);
void CheckTextures(void);
void PrintTexture(GLuint);
void PrintTextureValues(GLuint, GLuint);

void Update(void);
void ClearFBO(GLuint buffIdx, GLuint fboIdx, GLfloat val);
void DrawSquare(void);
void ResetState(void);
void Reset(void);

void PrintProgInfo(GLuint);

static float* LoadFloatData(const char* szFile, int* count);

GLfloat Uniform(GLfloat a, GLfloat b);

int main(int argc, char** argv)
{
	srand(time(NULL));
	InitGlobals();
	InitWindow(argc, argv);
	InitShaders();
	InitBuffers();
	InitFBO();
	InitTexture();

	SetupFBO();

	CheckFramebuffers();
	CheckTextures();

	glutMainLoop();
	return 0;
}

void Render(void)
{
	FrameCount++;

	DrawSquare();

	glutSwapBuffers();
	glutPostRedisplay();
}

void InitWindow(int argc, char** argv)
{
	GLenum GlewInitResult;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(CurrentWidth, CurrentHeight);

	GLint majVersion, minVersion;
	glGetIntegerv(GL_MAJOR_VERSION, &majVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minVersion);

	if (majVersion && minVersion)
	{
		glutInitContextVersion(majVersion, minVersion);
	}
	else
	{
		glutInitContextVersion(2, 0);
	}

	main_window = glutCreateWindow(WINDOW_TITLE_PREFIX);
	glewExperimental = GL_TRUE;
	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "INFO: OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, "INFO: OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
	fprintf(stdout, "INFO: OpenGL Shading Language Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	GLint maxDrawBuffs, maxTexSize, maxColorAttachments;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffs);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);

	cout << "MAX DRAW BUFFERS SUPPORTED: " << maxDrawBuffs << endl;
	cout << "MAX TEXTURE SIZE SUPPORTED: " << maxTexSize << endl;
	cout << "MAX COLOR ATTACHMENTS SUPPORTED: " << maxColorAttachments << endl;

	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutIdleFunc(Idle);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMove);
	glutTimerFunc(0, Timer, 0);

	glViewport(0, 0, 512, 512);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void InitTexture(void)
{
	GLuint xDivSize = (512 / 7);
	GLuint yDivSize = (512 / 7);
	GLuint idx, i, j;
	GLfloat val, xvel, yvel, maxTemp, minTemp;
	maxTemp = 1.0f;
	minTemp = -1.0f;

	GLfloat* velPixels = new GLfloat[(512 * 512 * 2)];
	GLfloat* tempPixels = new GLfloat[(512 * 512)];

	for (i = 0; i < 512; i++)
	{
		for (j = 0; j < 512; j++)
		{
			idx = (i * 512 + j);

			val = 0.0f;
			xvel = ((GLfloat)i - 256.0f) / 256.0f;
			yvel = ((GLfloat)j - 256.0f) / 256.0f;

			GLuint minY = (yDivSize * 3);
			GLuint maxY = (yDivSize * 5);

			GLuint hotMinX = (xDivSize);
			GLuint hotMaxX = (3 * xDivSize);

			GLuint coldMinX = (4 * xDivSize);
			GLuint coldMaxX = (6 * xDivSize);

			if ((j > hotMinX) && (j < hotMaxX))
			{
				if ((i > minY) && (i < maxY))
				{
					val = maxTemp;
				}
			}
			else if ((j > coldMinX) && (j < coldMaxX))
			{
				if ((i > minY) && (i < maxY))
				{
					val = minTemp;
				}
			}

			tempPixels[idx] = val;
			velPixels[(2 * idx)] = xvel;
			velPixels[(2 * idx) + 1] = yvel;
		}
	}

	if (glIsTexture(setup_tex[0]))
	{
		glDeleteTextures(2, setup_tex);
	}

	glGenTextures(2, setup_tex);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, setup_tex[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 512, 512, 0, GL_RED, GL_FLOAT, tempPixels);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, setup_tex[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, 512, 512, 0, GL_RG, GL_FLOAT, velPixels);

	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] tempPixels;
	delete[] velPixels;
}

void InitFBO(void)
{
	GLenum err;

	glGenFramebuffers(2, fbo_handles);

	glGenTextures(2, temp_tex);
	glGenTextures(2, vel_tex);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, temp_tex[0]);

	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R32F, fboWidth, fboHeight, 0, GL_RED, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	err = glGetError();

	if (err != GL_NO_ERROR)
	{
		cout << "Error after attempting to bind temp_tex[0]. Code is: " << glGetString(err) << endl;
	}
	else
	{
		cout << "No Errors." << endl;
	}

	glBindTexture(GL_TEXTURE_RECTANGLE, temp_tex[1]);

	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R32F, fboWidth, fboHeight, 0, GL_RED, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	err = glGetError();

	if (err != GL_NO_ERROR)
	{
		cout << "Error after attempting to bind temp_tex[1]. Code is: " << glGetString(err) << endl;
	}
	else
	{
		cout << "No Errors." << endl;
	}

	glBindTexture(GL_TEXTURE_RECTANGLE, vel_tex[0]);

	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RG32F, fboWidth, fboHeight, 0, GL_RG, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	err = glGetError();

	if (err != GL_NO_ERROR)
	{
		cout << "Error after attempting to bind vel_tex[0]. Code is: " << glGetString(err) << endl;
	}
	else
	{
		cout << "No Errors." << endl;
	}

	glBindTexture(GL_TEXTURE_RECTANGLE, vel_tex[1]);

	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RG32F, fboWidth, fboHeight, 0, GL_RG, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	err = glGetError();

	if (err != GL_NO_ERROR)
	{
		cout << "Error after attempting to bind vel_tex[1]. Code is: " << glGetString(err) << endl;
	}
	else
	{
		cout << "No Errors." << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_handles[0]);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, vel_tex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, temp_tex[0], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	err = glGetError();

	if (err != GL_NO_ERROR)
	{
		cout << "Error after attempting to bind setup_fbo[0]. Code is: " << glGetString(err) << endl;
	}
	else
	{
		cout << "No Errors." << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_handles[1]);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, vel_tex[1], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, temp_tex[1], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	err = glGetError();

	if (err != GL_NO_ERROR)
	{
		cout << "Error after attempting to bind setup_fbo[1]. Code is: " << glGetString(err) << endl;
	}
	else
	{
		cout << "No Errors." << endl;
	}
}

void PrintProgInfo(GLuint prog_id)
{
	GLint info_log_length;
	glGetProgramiv(prog_id, GL_INFO_LOG_LENGTH, &info_log_length);

	GLsizei log_len = (GLsizei)info_log_length;
	GLchar infoLog[1024];

	glGetProgramInfoLog(prog_id, 1024, &log_len, infoLog);

	cout << "InfoLog: " << infoLog << endl;
}

void InitShaders(void)
{
	GLuint prog_handle = 0;
	GLuint uobj[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	vector<string> prePD;
	prePD.push_back("#version 420 compatibility");

	cout << "Attempting to load show shader" << endl;

	prog_render.LoadFromFileWithPPD(GL_VERTEX_SHADER, "..\\shader.vs.glsl", prePD);
	prog_render.LoadFromFileWithPPD(GL_FRAGMENT_SHADER, "..\\render.fs.glsl", prePD);
	prog_render.CreateAndLinkProgram();
	prog_render.AddUniform("VelocityTexture");
	prog_render.AddUniform("TemperatureTexture");
	prog_render.AddUniform("GridWidth");
	prog_render.AddUniform("GridHeight");
	prog_handle = prog_render.GetProgram();
	velSubIdx = glGetSubroutineIndex(prog_handle, GL_FRAGMENT_SHADER, "RenderVel");
	tempSubIdx = glGetSubroutineIndex(prog_handle, GL_FRAGMENT_SHADER, "RenderTemp");

	cout << "Attempting to load setup shader" << endl;

	prog_setup.LoadFromFileWithPPD(GL_VERTEX_SHADER, "..\\shader.vs.glsl", prePD);
	prog_setup.LoadFromFileWithPPD(GL_FRAGMENT_SHADER, "..\\setup.fs.glsl", prePD);
	prog_setup.CreateAndLinkProgram();
	prog_setup.AddUniform("VelocityTexture");
	prog_setup.AddUniform("TemperatureTexture");
	prog_setup.AddUniform("GridWidth");
	prog_setup.AddUniform("GridHeight");
}

void SetupFBO(void)
{
	GLfloat invWidth = 1.0f / (GLfloat)fboWidth;
	GLfloat invHeight = 1.0f / (GLfloat)fboHeight;

	glViewport(0, 0, fboWidth, fboHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_handles[0]);
	glDrawBuffers(2, setup_drawBuffs);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, setup_tex[1]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, setup_tex[0]);

	prog_setup.Use();
	glUniform1i(prog_setup("VelocityTexture"), 0);
	glUniform1i(prog_setup("TemperatureTexture"), 1);
	glUniform1i(prog_setup("GridWidth"), fboWidth);
	glUniform1i(prog_setup("GridHeight"), fboHeight);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glFlush();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_handles[1]);
	glDrawBuffers(2, setup_drawBuffs);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glFlush();

	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);

	ResetState();
}

void InitBuffers(void)
{
	GLfloat vertices[] = {
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f,

		 1.0f,  1.0f,
		-1.0f,  1.0f,
		-1.0f, -1.0f,
	};

	vertPosSize = 6 * 2 * sizeof(GLfloat);
	GLsizei buff_size = vertPosSize;

	glGenBuffers(1, &vbo_handle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
	glBufferData(GL_ARRAY_BUFFER, buff_size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertPosSize, vertices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Reshape(int w, int h)
{
	CurrentWidth = w;
	CurrentHeight = h;
}

void Idle(void)
{
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT)
	{
		if (state == GLUT_DOWN)
		{
			mouseDown = true;

			mouseX = x * 2;
			mouseY = fboHeight - y * 2;
		}
		else
		{
			cout << "mouseUp" << endl;
			mouseDown = false;
		}
	}
}

void MouseMove(int x, int y)
{
	if (mouseDown)
	{
		mouseX = x * 2;
		mouseY = fboHeight - y * 2;
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'l':
		CheckFramebuffers();
		break;
	case 'o':
		CheckTextures();
		break;
	case 'z':
		ClearFBO(5, 0, 0.0f);
		ClearFBO(5, 1, 0.0f);
		break;
	case 'r':
		Reset();
		break;
	case 's':
		current_subroutine = (current_subroutine + 1) % 2;
		cout << "current_subroutine: " << current_subroutine << endl;
		break;
	}
}

void InitGlobals(void)
{
	// globabl ints

	fboWidth = 512;
	fboHeight = 512;

	initial = clock();

	mouseDown = false;

	setup_drawBuffs[0] = GL_COLOR_ATTACHMENT0;
	setup_drawBuffs[1] = GL_COLOR_ATTACHMENT1;

	window_drawBuffs[0] = GL_BACK;

	current_subroutine = 0;
}

void ResetState(void)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ClearFBO(GLuint buffIdx, GLuint fboIdx, GLfloat clear_val)
{
	// buffIdx corresponds to: 0 - setup, 1 - advec, 2 - divergence, 3 - force, 4 - gradient, 5 - pressure
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_handles[fboIdx]);
	glDrawBuffers(2, setup_drawBuffs);

	glClearColor(clear_val, clear_val, clear_val, clear_val);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DrawSquare(void)
{
	GLfloat invWidth = 1.0f / (GLfloat)CurrentWidth;
	GLfloat invHeight = 1.0f / (GLfloat)CurrentHeight;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffers(2, window_drawBuffs);
	glViewport(0, 0, CurrentWidth, CurrentHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, vel_tex[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE, temp_tex[0]);

	prog_render.Use();

	glUniform1i(prog_render("GridWidth"), 512);
	glUniform1i(prog_render("GridHeight"), 512);
	glUniform1i(prog_render("VelocityTexture"), 0);
	glUniform1i(prog_render("TemperatureTexture"), 1);
	if (current_subroutine == 0)
	{
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &velSubIdx);
	}
	else if (current_subroutine == 1)
	{
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &tempSubIdx);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnableVertexAttribArray(0);

	glUseProgram(0);

	glBindTexture(GL_TEXTURE_RECTANGLE, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, 0);
}

void Reset(void)
{
	cout << "Resetting" << endl;
	InitTexture();
	SetupFBO();
}

void Timer(int Value)
{
	if (0 != Value)
	{
		char* TempString = (char*)malloc(512 + strlen(WINDOW_TITLE_PREFIX));

		sprintf(TempString, "%s: %d Frames Per Second @ %d x %d", WINDOW_TITLE_PREFIX, FrameCount * 4, CurrentWidth, CurrentHeight);

		glutSetWindowTitle(TempString);
		free(TempString);
	}
	FrameCount = 0;
	glutTimerFunc(250, Timer, 1);
}

static float* LoadFloatData(const char* szFile, int* count)
{
	GLint lineCount = 0;
	FILE* fp;
	float* data = 0;

	fp = fopen(szFile, "r");
	if (fp != NULL)
	{
		char szFloat[1024];
		while (fgets(szFloat, sizeof(szFloat), fp) != NULL)
		{
			lineCount++;
		}

		rewind(fp);

		data = (float*)malloc((lineCount * sizeof(float)));
		if (data != NULL)
		{
			int index = 0;
			while (fgets(szFloat, sizeof(szFloat), fp) != NULL)
			{
				data[index] = (float)atof(szFloat);
				index++;
			}
			count[0] = index;
		}

		fclose(fp);
	}
	else
	{
		return 0;
	}

	return data;
}

void PrintFBO(GLuint fbo_handle)
{
	if (glIsFramebuffer(fbo_handle))
	{
		bool statusFound = false;

		cout << "IS framebuffer." << endl;

		glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);

		GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		cout << "Framebuffer status: ";

		if (fbStatus == GL_FRAMEBUFFER_COMPLETE)
		{
			cout << "Framebuffer Complete." << endl;
			statusFound = true;
		}
		if (fbStatus == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
		{
			cout << "Framebuffer incomplete attachment." << endl;
			statusFound = true;
		}
		if (fbStatus == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
		{
			cout << "Framebuffer incomplete missing attachment." << endl;
			statusFound = true;
		}
		if (fbStatus == GL_FRAMEBUFFER_UNSUPPORTED)
		{
			cout << "Framebuffer unsupported." << endl;
			statusFound = true;
		}
		if (fbStatus == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
		{
			cout << "Framebuffer incomplete draw buffer." << endl;
			statusFound = true;

			GLint fbParams;

			glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &fbParams);

			if (fbParams == GL_TEXTURE)
			{
				cout << "fb attachment is of type GL_TEXTURE" << endl;
			}
			else
			{
				cout << "cant determine attachment type." << endl;
			}

			glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE, &fbParams);

			cout << "fb component type is ";

			if (fbParams == GL_FLOAT)
			{
				cout << "GL_FLOAT" << endl;
			}
			else if (fbParams == GL_INT)
			{
				cout << "GL_INT" << endl;
			}
			else if (fbParams == GL_UNSIGNED_INT)
			{
				cout << "GL_UNSIGNED_INT" << endl;
			}
			else if (fbParams == GL_SIGNED_NORMALIZED)
			{
				cout << "GL_SIGNED_NORMALIZED" << endl;
			}
			else if (fbParams == GL_UNSIGNED_NORMALIZED)
			{
				cout << "GL_UNSIGNED_NORMALIZED" << endl;
			}
			else
			{
				cout << "UNKOWN" << endl;
			}
		}
		if (fbStatus == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
		{
			cout << "Framebuffer incomplete read buffer." << endl;
			statusFound = true;
		}
		if (fbStatus == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
		{
			cout << "Framebuffer incomplete layer targets." << endl;
			statusFound = true;
		}
		if (fbStatus == GL_FRAMEBUFFER_UNDEFINED)
		{
			cout << "Framebuffer UNDEFINED." << endl;
			statusFound = true;
		}
		if (!(statusFound))
		{
			cout << "Framebuffer error not found." << endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		cout << "is NOT framebuffer." << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PrintTexture(GLuint tex_handle)
{
	GLboolean isTex = glIsTexture(tex_handle);

	if (isTex)
	{
		cout << "IS texture." << endl;
	}
	else
	{
		cout << "is NOT texture." << endl;
	}

	if (isTex)
	{
		GLint texWidth;
		GLint texHeight;
		GLint texIntFormat;
		GLint texRedSize;
		GLint texGreenSize;
		GLint texBlueSize;

		glBindTexture(GL_TEXTURE_2D, tex_handle);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &texIntFormat);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &texRedSize);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &texGreenSize);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, &texBlueSize);
		glBindTexture(GL_TEXTURE_2D, 0);

		string tiFormat;

		if (texIntFormat == GL_R32F)
		{
			tiFormat = "GL_R32F";
		}
		else if (texIntFormat == GL_RG32F)
		{
			tiFormat = "GL_RG32F";
		}
		else if (texIntFormat == GL_RGB32F)
		{
			tiFormat = "GL_RGB32F";
		}
		else if (texIntFormat == GL_RGBA32F)
		{
			tiFormat = "GL_RGBA32F";
		}
		else if (texIntFormat == GL_R16F)
		{
			tiFormat = "GL_R16F";
		}
		else if (texIntFormat == GL_RG16F)
		{
			tiFormat = "GL_RG16F";
		}
		else if (texIntFormat == GL_RGB16F)
		{
			tiFormat = "GL_RGB16F";
		}
		else if (texIntFormat == GL_RGBA16F)
		{
			tiFormat = "GL_RGBA16F";
		}
		else
		{
			tiFormat = "Unknown format";
		}

		cout << "tex width: " << texWidth << "  height: " << texHeight << "  intFormat: " << tiFormat << endl;
		cout << "redSize: " << texRedSize << "  greenSize: " << texGreenSize << "  blueSize: " << texBlueSize << endl;
	}
}

void PrintTextureValues(GLuint tex_handle, GLuint nComponents)
{
	GLfloat* imgPtr = NULL;

	GLint texWidth;
	GLint texHeight;
	GLint texIntFormat;
	GLenum pixFormat;
	if (nComponents == 1)
	{
		pixFormat = GL_RED;
	}
	else if (nComponents == 2)
	{
		pixFormat = GL_RG;
	}
	else if (nComponents == 3)
	{
		pixFormat = GL_RGB;
	}
	else
	{
		pixFormat = GL_RGBA;
	}

	glBindTexture(GL_TEXTURE_2D, tex_handle);

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &texIntFormat);

	GLsizeiptr texSize = (4 * texWidth * texHeight * sizeof(GLfloat));

	imgPtr = (GLfloat*)malloc(texSize);

	glGetTexImage(GL_TEXTURE_2D, 0, pixFormat, GL_FLOAT, imgPtr);

	if (imgPtr)
	{
		cout << "displaying non-zero values of tex img of width: " << texWidth << "  height: " << texHeight << "  nComponents: " << nComponents << endl;

		GLuint i, j, k;
		for (i = 0; i < texHeight; i++)
		{
			for (j = 0; j < texWidth; j++)
			{
				for (k = 0; k < 4; k++)
				{
					GLfloat cVal = *imgPtr;
					imgPtr++;
					if (cVal != 0.0f)
					{
						cout << "(" << i << "," << j << "," << k << ")=" << cVal << endl;
					}
				}
			}
		}

		cout << "end." << endl;
	}
	else
	{
		cout << "could not get image values." << endl;
	}

	free(imgPtr);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void CheckTextures(void)
{
	PrintTexture(setup_tex[0]);
	cout << "checking setup_tex1" << endl;
	PrintTexture(setup_tex[1]);
	cout << "checking temp_tex0" << endl;
	PrintTexture(temp_tex[0]);
	cout << "checking temp_tex1" << endl;
	PrintTexture(temp_tex[1]);
	cout << "checking vel_tex0" << endl;
	PrintTexture(vel_tex[0]);
	cout << "checking vel_tex1" << endl;
	PrintTexture(vel_tex[1]);
}

void CheckFramebuffers(void)
{
	cout << "checking fbo0" << endl;
	PrintFBO(fbo_handles[0]);
	cout << "checking fbo1" << endl;
	PrintFBO(fbo_handles[1]);
}