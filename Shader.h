#pragma once

#include <string>

#include "GL.h"
#include "GLM.h"


#define LOGBUFFSIZE 512


using namespace std;


class ShaderBase
{
public:
	void unUseShaderProgram() { glUseProgram( 0 ); }
};

class Shader: public ShaderBase
{
public:
	Shader( int, int );
	~Shader();

	void setVertexBody( string & );
	void setFragmentBody( string & );
	void createShaderProgram();
	void useShaderProgram();
	unsigned int getID();

	void setBool( const string &, bool );
	void setInt( const string &, int );
	void setFloat( const string &, GLfloat );
	void setVec2( const string &, glm::vec2 & );
	void setVec2( const string &, GLfloat, GLfloat );
	void setVec3( const string &, glm::vec3 & );
	void setVec3( const string &, GLfloat, GLfloat, GLfloat );
	void setVec4( const string &, glm::vec4 & );
	void setVec4( const string &, GLfloat, GLfloat, GLfloat, GLfloat );
	void setMat2( const string &, glm::mat2 & );
	void setMat3( const string &, glm::mat3 & );
	void setMat4( const string &, glm::mat4 & );

private:
	string firstLine;
	string vertexBody;
	string fragmentBody;
	unsigned int programID;

	unsigned int createShader( string &, int );
	string processExceptionString( string & );
	string processExceptionString( const char *, char * );
};