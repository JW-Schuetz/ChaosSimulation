#pragma once

#include "GL.h"


class Axes
{
protected:
	static GLsizei width;
	static GLsizei height;

	Axes( GLsizei w, GLsizei h );
	~Axes();
};