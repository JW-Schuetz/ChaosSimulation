#pragma once

#include <string>
#include "Viewer.h"
#include "Calculator.h"


class Simulation: public Viewer, public Calculator
{
public:
	Simulation( string );
};