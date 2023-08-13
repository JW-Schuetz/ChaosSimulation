
#include "Shader.h"


Shader::Shader( int openGLMajorVersion, int openGLMinorVersion )
{
	firstLine = "#version " + to_string( openGLMajorVersion ) + to_string( openGLMinorVersion ) + "0 core\n";				// 1::
}

Shader::~Shader()
{
	glDeleteProgram( programID );	// shaders are still deleted after linking to program
}

void Shader::setVertexBody( string & body )
{
	vertexBody = body;
}

void Shader::setFragmentBody( string & body )
{
	fragmentBody = body;
}

unsigned int Shader::createShader( string & body, int mode )
{
	string source = firstLine + body;
	auto s = source.c_str();

	unsigned int shader = glCreateShader( mode );
	glShaderSource( shader, 1, &s, NULL );
	glCompileShader( shader );

	// compilation successful
	int success;
	glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
	if( !success )
	{
		char infoLog[LOGBUFFSIZE];
		glGetShaderInfoLog( shader, LOGBUFFSIZE, NULL, infoLog );
		throw processExceptionString( "Compilation fragment shader failed", infoLog );
	}

	return shader;
}

void Shader::createShaderProgram()
{
	unsigned int vertexShader = createShader( vertexBody, GL_VERTEX_SHADER );
	unsigned int fragmentShader = createShader( fragmentBody, GL_FRAGMENT_SHADER );

	programID = glCreateProgram();

	glAttachShader( programID, vertexShader );
	glAttachShader( programID, fragmentShader );
	glLinkProgram( programID );

	// delete shaders after link
	glDeleteShader( vertexShader );
	glDeleteShader( fragmentShader );

	int success;
	glGetProgramiv( programID, GL_LINK_STATUS, &success );
	if( !success )
	{
		char infoLog[LOGBUFFSIZE];
		glGetProgramInfoLog( programID, LOGBUFFSIZE, NULL, infoLog );
		throw processExceptionString( "Linking shader program failed", infoLog );
	}
}

void Shader::useShaderProgram()
{
	glUseProgram( programID );
}

unsigned int Shader::getID()
{
	return programID;
}

// utility uniform functions
void Shader::setBool( const string & name, bool value )
{
	GLint location = glGetUniformLocation( programID, name.c_str() );
	if( location == -1 )
	{
		string s = string( "Cannot get uniform location of " + name );
		throw processExceptionString( s );
	}
    glUniform1i( location, (int)value );
}

void Shader::setInt( const string & name, int value )
{
	GLint location = glGetUniformLocation( programID, name.c_str() );
	glUniform1i( location, value );
}

void Shader::setFloat( const string & name, GLfloat value )
{
	GLint location = glGetUniformLocation( programID, name.c_str() );
	glUniform1f( location, value );
}

void Shader::setVec2( const string & name, glm::vec2 & value )
{
	GLint location = glGetUniformLocation( programID, name.c_str() );
	glUniform2fv( location, 1, &value[0] );
}

void Shader::setVec2( const string & name, GLfloat x, GLfloat y )
{
	GLint location = glGetUniformLocation( programID, name.c_str() );
	glUniform2f( location, x, y );
}

void Shader::setVec3( const string & name, glm::vec3 & value )
{
	GLint location = glGetUniformLocation( programID, name.c_str() );
	glUniform3fv( location, 1, &value[0] );
}

void Shader::setVec3( const string & name, GLfloat x, GLfloat y, GLfloat z )
{
	GLint location = glGetUniformLocation( programID, name.c_str() );
	glUniform3f( location, x, y, z );
}

void Shader::setVec4( const string & name, glm::vec4 & value )
{
	GLint location = glGetUniformLocation( programID, name.c_str() );
	glUniform4fv( location, 1, &value[0] );
}

void Shader::setVec4( const string & name, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
	GLint location = glGetUniformLocation( programID, name.c_str() );
	glUniform4f( location, x, y, z, w );
}

void Shader::setMat2( const string & name, glm::mat2 & mat )
{
	GLint location = glGetUniformLocation( programID, name.c_str() );
	glUniformMatrix2fv( location, 1, GL_FALSE, &mat[0][0] );
}

void Shader::setMat3( const string & name, glm::mat3 & mat )
{
	GLint location = glGetUniformLocation( programID, name.c_str() );
	glUniformMatrix3fv( location, 1, GL_FALSE, &mat[0][0] );
}

void Shader::setMat4( const string & name, glm::mat4 & mat )
{
	GLint location = glGetUniformLocation( programID, name.c_str() );
	glUniformMatrix4fv( location, 1, GL_FALSE, &mat[0][0] );
}

string Shader::processExceptionString( string & pretxt )
{
	return( pretxt + '\n' );
}

string Shader::processExceptionString( const char *pretxt, char *info )
{
	// remove ending newline characters
	string log { info };
	size_t h = log.find_last_of( '\n', log.length() );

	return string( pretxt ) + '\n' + log.substr( 0, h - 1 );
}