#pragma once

#include "Boost.h"
#include "Lorenz.h"
#include "Roessler.h"


class Calculator
{
	ode_state xODE;						// system state (3-dim phase state)
	double maxCube;						// physical min/max. value, solution-cube

	Lorenz *lorenz;						// example: Lorenz attractor
	Roessler *roessler;					// example: Roessler attractor
	runge_kutta4<ode_state> stepper;	// stepper Runge-Kutta 4. order

	SYSTEM_MODE mode;					// which ODE should be solved
	bool modeChanged;					// request: new mode

	double calcMaxCube( bool & );		// calculate maximum cube size

public:
	Calculator();
	~Calculator();

	ode_state step( bool & );
	void setMode( SYSTEM_MODE );
	void setInitValues( ode_state, bool & );
	ode_state getValue();
	double getMaxCube();
};