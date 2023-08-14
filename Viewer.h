#pragma once

#include <filesystem>
#include <map>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "GL.h"
#include "GLM.h"
#include "Shader.h"
#include "Camera.h"
#include "Boost.h"
#include "Calculator.h"


using namespace std;


struct Character
{
	unsigned int textureID;		// ID handle of the glyph texture
	glm::ivec2 size;			// Size of glyph
	glm::ivec2 bearing;			// Offset from baseline to left/top of glyph
	FT_Pos advance;				// Offset to advance to next glyph
};


typedef array<double,2>   Double2d;
typedef array<double,3>   Double3d;
typedef array<GLfloat,3>  GLCoor3d;
typedef array<GLfloat,3>  GLColor;
typedef array<GLCoor3d,8> GLCoor8d;


struct GLTraceVertex
{
	GLCoor3d position;			// attribute location 0
	GLColor  color;				// attribute location 1
};

typedef enum
{
	NO_ROTATION = 0,			// no rotation
	RIGHT_ROTATION,				// rotate in right direction
	LEFT_ROTATION				// rotate in left direction
} ROTATION_MODE;

typedef enum
{
	ZAXIS = 0,					// orthographic projection onto z-coordinate
	ROTATE_XAXIS,				// rotation around x-axis
	ROTATE_YAXIS,				// rotation around y-axis
	MOUSECONTROL				// mouse controlled
} VIEW_MODE;

typedef enum
{
	ATTRIB0 = 0,
	ATTRIB1 = 1,
} ATTRIB;


static const int	traceLength		       = 20000;		// trace length [points]
static const int	cursorRedrawTraceCount = 500;		// wait trace points for cursor redraw [points]
static const double scrollIncrement	       = 0.05f;		// scale increment of mouse scroll wheel
static const double minScale		       = 0.1f;		// min. allowed scale factor
static const double maxScale		       = 5.0f;		// max. allowed scale factor


class Viewer
{
	static Calculator *calculator;					// ODE solver

	static GLfloat scale;							// scale factor
	static Camera *camera;
	static VIEW_MODE viewModus;
	static ROTATION_MODE rotationModus;
	static double mouseRawX, mouseRawY;				// mouse raw values
	static double mouseViewPortX, mouseViewPortY;	// mouse in viewport coordinate system
	static double mousePhysX, mousePhysY;			// mouse in physical coordinate system
	static bool mouseButton1Event, mouseButton2Event;

	// OpenGL-stuff
	GLfloat axesWidth;					// axes width
	GLfloat pointSize;					// point size
	GLfloat tracePointSize;				// trace point size

	GLColor backColor;					// background color
	GLColor axesColor;					// axes color
	GLColor pointColor;					// point color
	GLColor tracePointStartColor;		// initial trace point color
	GLColor tracePointEndColor;			// end trace point color
	GLColor glyphsColor;				// glyphs color
	GLColor cuboidColor;				// cuboid color

	unsigned int VBOAxes, VBOPoint, VBOPointTrace, VBOGlyphs;
	unsigned int VAOAxes, VAOPoint, VAOPointTrace, VAOGlyphs;

	array<GLCoor3d,4> verticesAxes;
	array<GLCoor3d,1> verticesPoint;
	array<GLTraceVertex,traceLength> *verticesTrace;

	unsigned int VBOCuboid;
	unsigned int VBOCuboidElement;
	unsigned int VAOCuboid;

	array<GLCoor3d,8> verticesCuboid;
	array<GLuint,24> indicesCuboid;

	map<char, Character> characters;

	Shader *axes, *point, *pointTrace, *glyphs, *cuboid;

	void createCursor();
	void initFont();
	void processInput( int & );
	void fillVertices();
	void prepareDrawing();

	// ODE-stuff
	static double maxCube;				// physical min/max. value, solution-cube

public:
	Viewer( string );
	~Viewer();

	void render();
	void renderLoop( ode_state &, int &, float &, float &, bool &, Double2d & );
	void renderText( const string &, float, float, float );
	void handleRotationMode( float, float &, float & );
	void handleMouseEvents( int &, bool &, Double2d & );
	void clearTraceDisplay();
	void calcMatrices( float &, glm::mat4 &, glm::mat4 &, glm::mat4 & );
	void drawCube( glm::mat4 &, glm::mat4 &, glm::mat4 & );
	void drawGlyphs();
	void drawSolutionPoint( Double3d &, glm::mat4 &, glm::mat4 &, glm::mat4 & );
	void drawTracePoints( int, Double3d &, glm::mat4 &, glm::mat4 &, glm::mat4 & );
	void drawAxes();

protected:
	static ofstream logger;

private:
	static GLsizei edge;			// top/bottom/left/right offset viewport to client windows area

	static GLsizei viewPortWidth;	// viewport width  (frameBufferWidth  - 2*left)
	static GLsizei viewPortHeight;	// viewport height (frameBufferHeight - 2*top )
	static double aspectRatio;		// total aspect-ratio

	static int windowWidth;			// window width (viewPortWidth  + 2*left)
	static int windowHeight;		// window height (viewPortHeight + 2*top )
	static bool winMinimized;		// window is in minimized state

	GLFWwindow *window;
	GLFWwindow *initGL();

	GLFWcursor *cursor;

	// static functions and callback functions
	static void setViewport();

	static void mouseRawToNormalizedViewportCoordinates();
	static void normalizedViewportToMouseRawCoordinates( double, double, double &, double & );

	static void normalizedViewportToPhysCoordinates();
	static void physToNormalizedViewportCoordinates( double, double, double &, double & );

	static void mouseRawToPhysCoordinates();
	static bool mouseInViewportInteriour();

	static void framebufferSizeCallback( GLFWwindow *, int, int );
	static void mouseButtonCallback( GLFWwindow *, int, int, int );
	static void cursorPositionCallback( GLFWwindow *, double, double );
	static void scrollCallback( GLFWwindow *, double, double );
	static void APIENTRY glDebugOutput( GLenum, GLenum, unsigned int, GLenum, GLsizei,
		const char *, const void * );
};