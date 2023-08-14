#include <limits>
#include <algorithm>
#include <stdio.h>

#include "windows.h"
#include "Common.h"
#include "Viewer.h"
#include "ShaderSources.h"


#define LOGBUFFSIZE 512


using namespace std;

Calculator *Viewer::calculator;
Camera *Viewer::camera;
GLsizei Viewer::edge;
GLsizei Viewer::viewPortWidth;
GLsizei Viewer::viewPortHeight;
double Viewer::aspectRatio;

int Viewer::windowWidth;
int Viewer::windowHeight;
bool Viewer::winMinimized;
ofstream Viewer::logger;
VIEW_MODE Viewer::viewModus;
ROTATION_MODE Viewer::rotationModus;
GLfloat Viewer::scale;
bool Viewer::mouseButton1Event;
bool Viewer::mouseButton2Event;
double Viewer::mouseRawX;
double Viewer::mouseRawY;
double Viewer::mouseViewPortX;
double Viewer::mouseViewPortY;
double Viewer::mousePhysX;
double Viewer::mousePhysY;

Viewer::Viewer( string logFile )
{
	logger = ofstream { logFile, ios_base::app };

	calculator = new Calculator();

	windowWidth  = 2000;
	windowHeight = 2000;
	edge		 = 50;

	viewModus = ZAXIS;
	scale	  = 1.0f;

	rotationModus = RIGHT_ROTATION;

	mouseButton1Event	= {};
	mouseButton2Event	= {};

	// Background
	backColor = { 0.0f, 0.0f, 0.0f };	// background color

	// Axes
	axesWidth = 5.0f;					// axes width
	axesColor = { 1.0f, 0.5f, 0.2f };	// axes color

	// Point
	pointSize  = 10.0f;					// point size
	pointColor = { 1.0f, 1.0f, 1.0f };	// point color

	// Trace
	tracePointSize       = 2.0f;					// trace point size
	tracePointStartColor = { 1.0f, 0.0f, 0.0f };	// initial trace point color
	tracePointEndColor   = { 0.0f, 0.0f, 1.0f };	// end trace point color

	// Glyphs
	glyphsColor = { 0.0f, 1.0f, 0.0f };		// glyphs color

	// Cuboid
	cuboidColor = { 1.0f, 1.0f, 1.0f };		// cuboid color

	// Axes
	VBOAxes		 = {};
	VAOAxes		 = {};
	verticesAxes = {};

	// Point
	VBOPoint	  = {};
	VAOPoint	  = {};
	verticesPoint = {};

	// Point trace
	VBOPointTrace = {};
	VAOPointTrace = {};
	verticesTrace = new( array<GLTraceVertex,traceLength> );

	// Glyphs
	VBOGlyphs = {};
	VAOGlyphs = {};

	// Cuboid
	verticesCuboid = {};

	// initialize windowing system
	window = initGL();
	initFont();

	camera = new Camera( glm::vec3( 0.0f, 0.0f, 3.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ),
		-90.0f, 0.0f );

	// initialize shader programs
	axes = new Shader( MAJOR_VERSION, MINOR_VERSION );
	axes->setVertexBody( axesVertexBody );
	axes->setFragmentBody( axesFragmentBody );
	axes->createShaderProgram();

	point = new Shader( MAJOR_VERSION, MINOR_VERSION );
	point->setVertexBody( pointVertexBody );
	point->setFragmentBody( pointFragmentBody );
	point->createShaderProgram();

	pointTrace = new Shader( MAJOR_VERSION, MINOR_VERSION );
	pointTrace->setVertexBody( pointTraceVertexBody );
	pointTrace->setFragmentBody( pointTraceFragmentBody );
	pointTrace->createShaderProgram();

	glyphs = new Shader( MAJOR_VERSION, MINOR_VERSION );
	glyphs->setVertexBody( glyphsVertexBody );
	glyphs->setFragmentBody( glyphsFragmentBody );
	glyphs->createShaderProgram();

	cuboid = new Shader( MAJOR_VERSION, MINOR_VERSION );
	cuboid->setVertexBody( cuboidVertexBody );
	cuboid->setFragmentBody( cuboidFragmentBody );
	cuboid->createShaderProgram();

	fillVertices();			// fill data in vertex array
	prepareDrawing();		// do OpenGL stuff

	render();				// OpenGL render loop
}

GLFWwindow* Viewer::initGL()
{
	// init glfw
	glfwInit();

	//glfwWindowHint( GLFW_SAMPLES, 1 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, MAJOR_VERSION );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, MINOR_VERSION );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#ifdef DEBUG
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, true );
#endif

	const GLFWvidmode * videoMode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
	glfwWindowHint( GLFW_RED_BITS, videoMode->redBits );
	glfwWindowHint( GLFW_GREEN_BITS, videoMode->greenBits );
	glfwWindowHint( GLFW_BLUE_BITS, videoMode->blueBits );
	glfwWindowHint( GLFW_REFRESH_RATE, videoMode->refreshRate );

	// create window
	window = glfwCreateWindow( windowWidth, windowHeight, "Lorenz-Attractor", NULL, NULL );
	if( window == NULL )
		throw string{ "Failed to create GLFW window" };

	glfwMakeContextCurrent( window );

	// GFWL: set cursor shape
	createCursor();
	glfwSetCursor( window, cursor );

	// GFWL: register callback functions
	glfwSetFramebufferSizeCallback( window, framebufferSizeCallback );
	glfwSetMouseButtonCallback( window, mouseButtonCallback );
	glfwSetCursorPosCallback( window, cursorPositionCallback );
	glfwSetScrollCallback( window, scrollCallback );

	// init glad
	if( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
		throw string{ "Failed to initialize GLAD" };

#ifdef DEBUG
	// init glDebugging
	int flags;
	glGetIntegerv( GL_CONTEXT_FLAGS, &flags );
	if( flags & GL_CONTEXT_FLAG_DEBUG_BIT )
	{
		glEnable( GL_DEBUG_OUTPUT );
		glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );	// makes sure errors are displayed synchronously
		glDebugMessageCallback( glDebugOutput, 0 );	// register callback function
		glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE );
	}
#endif

	return window;
}

void Viewer::initFont()
{
	glEnable( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// init freetype
	FT_Library ft;
	if( FT_Init_FreeType( &ft ) )
		throw string{ "Could not init FreeType Library" };

	filesystem::path p = filesystem::path( "resources/fonts/Antonio-Bold.ttf" );
	string s = p.string();

	FT_Face face;
	if( FT_New_Face( ft, s.c_str(), 0, &face ) )
		throw string{ "Failed to load font" };

	FT_Set_Pixel_Sizes( face, 0, 48 );	// set size to load glyphs as

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // no byte-alignment restriction

	for( unsigned char c = 0; c < 128; c++ )
	{
		// load character glyph
		if( FT_Load_Char( face, c, FT_LOAD_RENDER ) ) throw string{ "Failed to load Glyph" };

		// generate texture
		unsigned int texture;
		glGenTextures( 1, &texture );
		glBindTexture( GL_TEXTURE_2D, texture );

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		// set texture options
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		// now store character for later use
		Character character =
		{
			texture,
			glm::ivec2( face->glyph->bitmap.width, face->glyph->bitmap.rows ),
			glm::ivec2( face->glyph->bitmap_left, face->glyph->bitmap_top ),
			face->glyph->advance.x
		};

		characters.insert( std::pair<char,Character>( c, character ) );
	}

	glBindTexture( GL_TEXTURE_2D, 0 );

	// tidy up
	FT_Done_Face( face );
	FT_Done_FreeType( ft );
}

void Viewer::fillVertices()
{
	// Axes
	verticesAxes[0] = {  1.0f,  1.0f, 0.0f };	// right top
	verticesAxes[1] = {  1.0f, -1.0f, 0.0f };	// right bottom
	verticesAxes[2] = { -1.0f, -1.0f, 0.0f };	// left bottom
	verticesAxes[3] = { -1.0f,  1.0f, 0.0f };	// left top

	// Points
	verticesPoint.fill( { 0.0f, 0.0f, 0.0f } );

	// Trace
	verticesTrace->fill( { 0.0f, 0.0f, 0.0f } );

	// Cuboid
	verticesCuboid[0] = { -1.0f, -1.0f,  1.0f };
	verticesCuboid[1] = {  1.0f, -1.0f,  1.0f };
	verticesCuboid[2] = {  1.0f,  1.0f,  1.0f };
	verticesCuboid[3] = { -1.0f,  1.0f,  1.0f };
	verticesCuboid[4] = { -1.0f, -1.0f, -1.0f };
	verticesCuboid[5] = {  1.0f, -1.0f, -1.0f };
	verticesCuboid[6] = {  1.0f,  1.0f, -1.0f };
	verticesCuboid[7] = { -1.0f,  1.0f, -1.0f };

	indicesCuboid = {
		0, 1, 1, 2, 2, 3, 3, 0,		// front lines
		4, 5, 5, 6, 6, 7, 7, 4,		// back lines
		0, 4, 1, 5, 2, 6, 3, 7		// side lines
	};
}

void Viewer::prepareDrawing()
{
	setViewport();	// set viewport

	// Axes
	glGenVertexArrays( 1, &VAOAxes );
	glBindVertexArray( VAOAxes );

	glGenBuffers( 1, &VBOAxes );
	glBindBuffer( GL_ARRAY_BUFFER, VBOAxes );
	glBufferData( GL_ARRAY_BUFFER, sizeof( verticesAxes ), verticesAxes.data(), GL_STATIC_DRAW );

	glVertexAttribPointer( ATTRIB0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (void*)0 );
	glEnableVertexAttribArray( ATTRIB0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	// Point
	glGenVertexArrays( 1, &VAOPoint );
	glBindVertexArray( VAOPoint );

	glGenBuffers( 1, &VBOPoint );
	glBindBuffer( GL_ARRAY_BUFFER, VBOPoint );
	glBufferData( GL_ARRAY_BUFFER, sizeof( verticesPoint ), NULL, GL_DYNAMIC_DRAW );

	glVertexAttribPointer( ATTRIB0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (void*)0 );
	glEnableVertexAttribArray( ATTRIB0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	// Trace
	glGenVertexArrays( 1, &VAOPointTrace );
	glBindVertexArray( VAOPointTrace );

	glGenBuffers( 1, &VBOPointTrace );
	glBindBuffer( GL_ARRAY_BUFFER, VBOPointTrace );
	glBufferData( GL_ARRAY_BUFFER, sizeof( *verticesTrace ), NULL, GL_DYNAMIC_DRAW );

	glVertexAttribPointer( ATTRIB0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( GLfloat ), (void *)0 );
	glVertexAttribPointer( ATTRIB1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( GLfloat ), (void *)( 3 * sizeof( float ) ) );
	glEnableVertexAttribArray( ATTRIB0 );
	glEnableVertexAttribArray( ATTRIB1 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	// Glyphs
	glGenVertexArrays( 1, &VAOGlyphs );
	glBindVertexArray( VAOGlyphs );

	glGenBuffers( 1, &VBOGlyphs );
	glBindBuffer( GL_ARRAY_BUFFER, VBOGlyphs );
	glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * 6 * 4, NULL, GL_DYNAMIC_DRAW );

	glEnableVertexAttribArray( ATTRIB0 );
	glVertexAttribPointer( ATTRIB0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), 0 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	// Cuboid
	glGenVertexArrays( 1, &VAOCuboid );
	glBindVertexArray( VAOCuboid );

	glGenBuffers( 1, &VBOCuboid );
	glBindBuffer( GL_ARRAY_BUFFER, VBOCuboid );
	glBufferData( GL_ARRAY_BUFFER, sizeof( verticesCuboid ), verticesCuboid.data(), GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &VBOCuboidElement );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, VBOCuboidElement );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indicesCuboid ), indicesCuboid.data(), GL_DYNAMIC_DRAW );

	glVertexAttribPointer( ATTRIB0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (void *)0 );
	glEnableVertexAttribArray( ATTRIB0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	// general settings
	glClearColor( backColor[0], backColor[1], backColor[2], 1.0f );
}

void Viewer::render()
{
	// set uniforms
	axes->useShaderProgram();
	axes->setVec3( "axesColor", axesColor[0], axesColor[1], axesColor[2] );
	axes->unUseShaderProgram();

	point->useShaderProgram();
	point->setVec3( "pointColor", pointColor[0], pointColor[1], pointColor[2] );
	point->unUseShaderProgram();

	glyphs->useShaderProgram();
	glm::mat4 glyphsProjection = glm::ortho( 0.0f, (float)windowWidth, 0.0f, (float)windowHeight );
	glyphs->setVec3( "glyphsColor", glyphsColor[0], glyphsColor[1], glyphsColor[2] );
	glyphs->setMat4( "projection", glyphsProjection );
	glyphs->unUseShaderProgram();

	cuboid->useShaderProgram();
	cuboid->setVec3( "cuboidColor", cuboidColor[0], cuboidColor[1], cuboidColor[2] );
	cuboid->unUseShaderProgram();

	// initialize function call parameters
	float time			    = {};
	float lastTime		    = {};
	int index               = traceLength - 1;	// set index to last element
	bool newInitialValue    = {};
	Double2d initValues     = {};
	ode_state xODE			= calculator->getValue();

	while( !glfwWindowShouldClose( window ) )
	{
		if( !winMinimized )		// do no calculation and no graphics if window is minimized
			renderLoop( xODE, index, time, lastTime, newInitialValue, initValues );

		glfwPollEvents();		// poll GLFW events
	}
}

void Viewer::renderLoop( ode_state &xODE, int &index, float &time, float &lastTime, 
	bool &newInitialValue, Double2d &initValues )
{
	float t = (float)glfwGetTime();	// time for rotation animation

	handleRotationMode( t, time, lastTime );
	handleMouseEvents( index, newInitialValue, initValues );

	if( newInitialValue )	// new setting of initial values
	{
		xODE = calculator->getValue();

		if( index < traceLength - cursorRedrawTraceCount )
		{
			double mouseViewPortX, mouseViewPortY, mouseRawX, mouseRawY;	// local variables

			double maxCube = calculator->getMaxCube();
			physToNormalizedViewportCoordinates( maxCube, initValues[0], initValues[1], 
				mouseViewPortX, mouseViewPortY );
			normalizedViewportToMouseRawCoordinates( mouseViewPortX, mouseViewPortY, mouseRawX, mouseRawY );
			glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
			glfwSetCursorPos( window, mouseRawX, mouseRawY );

			newInitialValue = false;
		}
	}

	glClear( GL_COLOR_BUFFER_BIT );	// clear screen with backColor

	glm::mat4 projection = glm::mat4( 1.0f );
	glm::mat4 view = glm::mat4( 1.0f );
	glm::mat4 model = glm::mat4( 1.0f );

	calcMatrices( time, projection, view, model );
	drawTracePoints( index, xODE, projection, view, model );
	drawSolutionPoint( xODE, projection, view, model );
	drawGlyphs();
	drawCube( projection, view, model );
	drawAxes();

	// solve ODE system -> calc next solution point
	bool cubeChanged;
	xODE = calculator->step( cubeChanged );
	if( cubeChanged ) mouseRawToPhysCoordinates();	// reflect possible change of maxCube

	// tidy up
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
	glFlush();

	--index;									// next index
	if( index == -1 ) index = traceLength - 1;	// index limitation, set index to last element

	glfwSwapBuffers( window );
	processInput( index );			// read keyboard
}

void Viewer::handleMouseEvents( int &index, bool &newInitialValue, Double2d &initValues )
{
	if( mouseButton1Event )
	{
		switch( viewModus )
		{
			case ZAXIS:							// transform. to physical coordinates in mouseButtonCallback()
				bool cubeChanged;
				calculator->setInitValues( { mousePhysX, mousePhysY, 0.0 }, cubeChanged );

				clearTraceDisplay();

				index = traceLength - 1;		// reset index to last element again

				initValues[0] = mousePhysX;
				initValues[1] = mousePhysY;
				newInitialValue = true;

				glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
				break;
			case ROTATE_XAXIS:
			case ROTATE_YAXIS:
				break;
			case MOUSECONTROL:
				break;
		}
		mouseButton1Event = {};	// reset marker
	}

	if( mouseButton2Event )
	{
		switch( viewModus )
		{
			case ZAXIS:
				viewModus = ROTATE_XAXIS;
				break;
			case ROTATE_XAXIS:
				viewModus = ROTATE_YAXIS;
				break;
			case ROTATE_YAXIS:
				viewModus = MOUSECONTROL;
				break;
			case MOUSECONTROL:
				viewModus = ZAXIS;
				break;
		}
		mouseButton2Event = {};	// reset marker
	}
}

void Viewer::handleRotationMode( float t, float &time, float &lastTime )
{
	switch( rotationModus )
	{
		case NO_ROTATION:
			break;
		case LEFT_ROTATION:
			time = time + ( t - lastTime );
			break;
		case RIGHT_ROTATION:
			time = time - ( t - lastTime );
			break;
	}

	lastTime = t;
}

void Viewer::drawAxes()
{
	glLineWidth( axesWidth );
	axes->useShaderProgram();
	glBindVertexArray( VAOAxes );
	glDrawArrays( GL_LINE_LOOP, 0, (int)verticesAxes.size() );
	axes->unUseShaderProgram();
}

void Viewer::drawGlyphs()
{
	const size_t buffSize = 50;
	char *buff = new char[buffSize];

	glyphs->useShaderProgram();
	glBindVertexArray( VAOGlyphs );
	glActiveTexture( GL_TEXTURE0 );

	switch( viewModus )
	{
		case ZAXIS:
			snprintf( buff, buffSize, "x = % 2.4f,  y = % 2.4f,  s = % 1.2f", 
				mousePhysX, mousePhysY, scale );
			break;
		case ROTATE_XAXIS:
			snprintf( buff, buffSize, "x-axis,  s = % 1.2f", scale );
			break;
		case ROTATE_YAXIS:
			snprintf( buff, buffSize, "y-axis,  s = % 1.2f", scale );
			break;
		case MOUSECONTROL:
			snprintf( buff, buffSize, "mouse,  s = % 1.2f", scale );
			break;
	}

	renderText( buff, 10.0f, 10.0f, 0.5f );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glyphs->unUseShaderProgram();

	delete[] buff;
}

void Viewer::drawSolutionPoint(Double3d &solution, glm::mat4 &projection, glm::mat4 &view, glm::mat4 &model )
{
	point->useShaderProgram();
	point->setMat4( "projection", projection );
	point->setMat4( "view", view );
	point->setMat4( "model", model );
	glBindVertexArray( VAOPoint );
	glBindBuffer( GL_ARRAY_BUFFER, VBOPoint );
	GLCoor3d *buffPoint = (GLCoor3d *)glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
	*buffPoint = { (GLfloat)solution[0], (GLfloat)solution[1], (GLfloat)solution[2] };	// copy solution point into buffer
	glUnmapBuffer( GL_ARRAY_BUFFER );
	glPointSize( pointSize );
	glDrawArrays( GL_POINTS, 0, (int)verticesPoint.size() );
	point->unUseShaderProgram();
}

void Viewer::drawTracePoints( int index, Double3d &solution, glm::mat4 &projection, glm::mat4 &view, glm::mat4 &model )
{
	pointTrace->useShaderProgram();
	pointTrace->setMat4( "projection", projection );
	pointTrace->setMat4( "view", view );
	pointTrace->setMat4( "model", model );
	glBindVertexArray( VAOPointTrace );
	glPointSize( tracePointSize );
	glBindBuffer( GL_ARRAY_BUFFER, VBOPointTrace );
	// every vertex consists of 2 attributes: coordinate (3 floats) and color (3 floats)
	GLTraceVertex *buffTrace = (GLTraceVertex *)glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
	GLTraceVertex *p = buffTrace + index;	// writepointer to vertex attribute 0 (coordinate)
	p->position = { (GLfloat)solution[0], (GLfloat)solution[1], (GLfloat)solution[2] };	// copy coordinate of new solution point into buffer
	GLfloat delta = (GLfloat)index / ( traceLength - 1 );
	GLfloat r = tracePointStartColor[0] * delta + tracePointEndColor[0] * ( 1.0f - delta );
	GLfloat g = tracePointStartColor[1] * delta + tracePointEndColor[1] * ( 1.0f - delta );
	GLfloat b = tracePointStartColor[2] * delta + tracePointEndColor[2] * ( 1.0f - delta );
	p->color = { r, g, b };		// copy color of new point into buffer
	glUnmapBuffer( GL_ARRAY_BUFFER );
	glDrawArrays( GL_POINTS, 0, traceLength );
	pointTrace->unUseShaderProgram();
}

void Viewer::drawCube( glm::mat4 &projection, glm::mat4 &view, glm::mat4 &model )
{
	glLineWidth( 1 );
	cuboid->useShaderProgram();
	cuboid->setMat4( "projection", projection );
	cuboid->setMat4( "view", view );
	cuboid->setMat4( "model", model );
	glBindVertexArray( VAOCuboid );
	glDrawElements( GL_LINES, 24, GL_UNSIGNED_INT, 0 );
	cuboid->unUseShaderProgram();
}

void Viewer::calcMatrices( float &time, glm::mat4 &projection, glm::mat4 &view, glm::mat4 &model )
{
	double maxCube = calculator->getMaxCube();

	float X = (float)( maxCube * aspectRatio );

	switch( viewModus )
	{
		case ZAXIS:
			projection = glm::ortho( -X, X, -(float)maxCube, (float)maxCube, -FLT_MAX, FLT_MAX );
			view = glm::scale( view, glm::vec3( scale ) );
			break;
		case ROTATE_XAXIS:
			projection = glm::ortho( -X, X, -(float)maxCube, (float)maxCube, -FLT_MAX, FLT_MAX );
			view = glm::rotate( view, time, glm::vec3( 1.0f, 0.0f, 0.0f ) );
			view = glm::scale( view, glm::vec3( scale ) );
			break;
		case ROTATE_YAXIS:
			projection = glm::ortho( -X, X, -(float)maxCube, (float)maxCube, -FLT_MAX, FLT_MAX );
			view = glm::rotate( view, time, glm::vec3( 0.0f, 1.0f, 0.0f ) );
			view = glm::scale( view, glm::vec3( scale ) );
			break;
		case MOUSECONTROL:
			projection = glm::ortho( -X, X, -(float)maxCube, (float)maxCube, -FLT_MAX, FLT_MAX );
			view = camera->GetViewMatrix();
			view = glm::scale( view, glm::vec3( scale ) );
			break;
	}
}

void Viewer::clearTraceDisplay()
{
	// get pointer to first element of buffer
	glBindBuffer( GL_ARRAY_BUFFER, VBOPointTrace );
	GLTraceVertex *buffTrace = (GLTraceVertex *)glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );

	// normalize all buffer elements
	for( int i = 0; i <= traceLength - 1; ++i ) *( buffTrace + i ) = {};

	// unmap buffer pointer
	glUnmapBuffer( GL_ARRAY_BUFFER );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

Viewer::~Viewer()
{
	delete verticesTrace;

	// Axes
	glDeleteBuffers(1, &VBOAxes);
	glDeleteVertexArrays(1, &VAOAxes);
	delete axes;

	// Point
	glDeleteBuffers( 1, &VBOPoint );
	glDeleteVertexArrays( 1, &VAOPoint );
	delete point;

	// Trace
	glDeleteBuffers( 1, &VBOPointTrace );
	glDeleteVertexArrays( 1, &VAOPointTrace );
	delete pointTrace;

	// Glyphs
	glDeleteBuffers( 1, &VBOGlyphs );
	glDeleteVertexArrays( 1, &VAOGlyphs );
	delete glyphs;

	// Cuboid
	glDeleteBuffers( 1, &VBOCuboid );
	glDeleteVertexArrays( 1, &VAOCuboid );
	delete cuboid;

	delete camera;
	delete calculator;

	// GLFW
	glfwDestroyCursor( cursor );
	glfwTerminate();

	logger.flush();
}

void Viewer::setViewport()
{
	// don't set vieport with negative width or height
	if( windowWidth == 0 || windowHeight == 0 ) winMinimized = true;
	else
	{
		winMinimized = false;

		viewPortWidth  = windowWidth - 2 * edge;
		viewPortHeight = windowHeight - 2 * edge;
		aspectRatio    = (float)windowWidth / (float)windowHeight;

		glViewport( edge, edge, viewPortWidth, viewPortHeight );
	}
}

void Viewer::renderText( const string &text, float x, float y, float txtScale )
{
	string::const_iterator c;		// iterate through all characters

	for( c = text.begin(); c != text.end(); ++c )
	{
		Character ch = characters[*c];

		float xpos = x + ch.bearing.x * txtScale;
		float ypos = y - ( ch.size.y - ch.bearing.y ) * txtScale;

		float w = ch.size.x * txtScale;
		float h = ch.size.y * txtScale;

		// update VBO for each character
		float vertices[6][4] = {
			{xpos, ypos + h, 0.0f, 0.0f},
			{xpos, ypos, 0.0f, 1.0f},
			{xpos + w, ypos, 1.0f, 1.0f},
			{xpos, ypos + h, 0.0f, 0.0f},
			{xpos + w, ypos, 1.0f, 1.0f},
			{xpos + w, ypos + h, 1.0f, 0.0f}
		};

		// render glyph texture over quad
		glBindTexture( GL_TEXTURE_2D, ch.textureID );

		// update content of VBO memory
		glBindBuffer( GL_ARRAY_BUFFER, VBOGlyphs );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		// render quad
		glDrawArrays( GL_TRIANGLES, 0, 6 );

		// advance cursors for next glyph (advance is 1/64 pixels)
		x += ( ch.advance >> 6 ) * txtScale; // bitshift by 6 (2^6 = 64)
	}
}

void Viewer::processInput( int &index )
{
	if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )	// escape program
		glfwSetWindowShouldClose( window, true );
	if( glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS )		// no rotation
		rotationModus = NO_ROTATION;
	if( glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS )		// no rotation
		rotationModus = NO_ROTATION;
	if( glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS )		// left rotation
		rotationModus = LEFT_ROTATION;
	if( glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS )	// right rotation
		rotationModus = RIGHT_ROTATION;
	if( glfwGetKey( window, GLFW_KEY_L ) == GLFW_PRESS )		// solve Lorenz system
	{
		bool cubeChanged;
		calculator->setMode( LORENZ, cubeChanged );
		if( cubeChanged ) mouseRawToPhysCoordinates();			// reflect possible change of maxCube

		clearTraceDisplay();

		index = traceLength - 1;								// set index to last element
	}
	if( glfwGetKey( window, GLFW_KEY_R ) == GLFW_PRESS )		// solve Roessler system
	{
		bool cubeChanged;
		calculator->setMode( ROESSLER, cubeChanged );
		if( cubeChanged ) mouseRawToPhysCoordinates();			// reflect possible change of maxCube

		clearTraceDisplay();

		index = traceLength - 1;								// set index to last element
	}
}

void Viewer::mouseRawToNormalizedViewportCoordinates()
{
	mouseViewPortX =  ( 2.0 * mouseRawX - viewPortWidth  - 2.0 * edge ) / viewPortWidth;
	mouseViewPortY = -( 2.0 * mouseRawY - viewPortHeight - 2.0 * edge  ) / viewPortHeight;
}

void Viewer::normalizedViewportToMouseRawCoordinates( double viewX, double viewY,
	double &rawX, double &rawY )
{
	rawX = viewPortWidth  * ( 1.0 + viewX ) / 2.0 + edge;
	rawY = viewPortHeight * ( 1.0 - viewY ) / 2.0 + edge;
}

void Viewer::normalizedViewportToPhysCoordinates()
{
	double maxCube = calculator->getMaxCube();

	double a   = maxCube / scale;
	mousePhysX = a * mouseViewPortX * aspectRatio;
	mousePhysY = a * mouseViewPortY;
}

void Viewer::physToNormalizedViewportCoordinates( double maxCube, double physX, double physY, 
	double &viewX, double &viewY )
{
	double a = maxCube / scale;
	viewX    = physX / ( a * aspectRatio );
	viewY    = physY / a;
}

void Viewer::mouseRawToPhysCoordinates()
{
	// mousePosXY coordinates counts from left upper window corner (0, 0) to
	// right lower window corner (width + 2*top, height + 2*left)
	mouseRawToNormalizedViewportCoordinates();
	normalizedViewportToPhysCoordinates();
}

bool Viewer::mouseInViewportInteriour()
{
	return( abs( mouseViewPortX ) <= 1.0 && abs( mouseViewPortY ) <= 1.0 );
}

void Viewer::framebufferSizeCallback( GLFWwindow *, int w, int h )
{
	windowWidth  = w;
	windowHeight = h;

	setViewport();	// set viewport
}

void Viewer::cursorPositionCallback( GLFWwindow *, double x, double y )
{
	mouseRawX = x;
	mouseRawY = y;

	mouseRawToPhysCoordinates();

	if( mouseInViewportInteriour() )
		camera->ProcessMouseMovement( x, y );	// handle camera
}

void Viewer::scrollCallback( GLFWwindow *window, double, double yoffset )
{
	switch( viewModus )
	{
		case ZAXIS:
		case ROTATE_XAXIS:
		case ROTATE_YAXIS:
		case MOUSECONTROL:
			scale += (GLfloat)( scrollIncrement * yoffset );	// cumulated scroll offsets

			if( scale < minScale ) scale = minScale;
			if( scale > maxScale ) scale = maxScale;

			mouseRawToPhysCoordinates();						// display right coordinates after scrolling
			break;
	}
}

void Viewer::mouseButtonCallback( GLFWwindow *window, int button, int action, int )
{
	glfwGetCursorPos( window, &mouseRawX, &mouseRawY );

	mouseRawToPhysCoordinates();

	if( mouseInViewportInteriour() )
		switch( button )
		{
			case GLFW_MOUSE_BUTTON_1:
				if( action == GLFW_PRESS ) mouseButton1Event = true;
				break;
			case GLFW_MOUSE_BUTTON_2:
				if( action == GLFW_PRESS ) mouseButton2Event = true;
				break;
		}
}

void Viewer::createCursor()
{
	int i = 0, x, y;
	unsigned char buffer[32 * 32 * 4] = {};
	const GLFWimage image = { 32, 32, buffer };

	for( y = 0; y < image.width; ++y )
	{
		for( x = 0; x < image.height; ++x )
		{
			if( x == 16 || y == 16 )
			{
				buffer[i++] = 255;	// red
				buffer[i++] = 255;	// green
				buffer[i++] = 255;	// blue
				buffer[i++] = 255;	// alpha
			}
			else
			{
				buffer[i++] = 0;
				buffer[i++] = 0;
				buffer[i++] = 0;
				buffer[i++] = 0;
			}
		}
	}

	cursor = glfwCreateCursor( &image, image.width / 2, image.height / 2 );
}

void APIENTRY Viewer::glDebugOutput( GLenum source, GLenum type, unsigned int id,
	GLenum severity, GLsizei length, const char *message, const void *userParam )
{
	if( id == 131169 || id == 131185 || id == 131218 || id == 131204 ) return; // ignore these non-significant error codes

	// ignore message code resulted from deprecated behaviour of glLineWidth( x ) with x > 1: "Wide lines
	// have been deprecated."
	if( source == GL_DEBUG_SOURCE_API && type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR &&
		severity == GL_DEBUG_SEVERITY_MEDIUM && id == 7 ) return;

	logger << "---------------" << endl;
	logger << "Debug message (" << id << "): " << message << endl;

	switch( source )
	{
		case GL_DEBUG_SOURCE_API:             logger << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   logger << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: logger << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     logger << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     logger << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           logger << "Source: Other"; break;
	} logger << endl;

	switch( type )
	{
		case GL_DEBUG_TYPE_ERROR:               logger << "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: logger << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  logger << "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         logger << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         logger << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              logger << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          logger << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           logger << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               logger << "Type: Other"; break;
	} logger << endl;

	switch( severity )
	{
		case GL_DEBUG_SEVERITY_HIGH:         logger << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       logger << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          logger << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: logger << "Severity: notification"; break;
	} logger << endl;

	logger << endl;
}