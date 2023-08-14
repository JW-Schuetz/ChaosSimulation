
#include "Calculator.h"


Calculator::Calculator()
{
	mode		= LORENZ;
	modeChanged	= {};
	xODE		= {};
	maxCube		= -FLT_MAX;

	lorenz		= new Lorenz( 10.0, 28.0, 8.0 / 3.0, 0.001, { -20.0, 25.0, 0.0 } );
	roessler	= new Roessler( 0.2, 0.2, 5.7, 0.01, { 0.1, 0.1, 0.1 } );

	switch( mode )
	{
		case LORENZ:
			xODE = lorenz->getInitState();		// set initial values
			break;

		case ROESSLER:
			xODE = roessler->getInitState();	// set initial values
			break;
	}

	bool cubeChanged;
	calcMaxCube( cubeChanged );		// recalculate maxCube
}

void Calculator::setMode( SYSTEM_MODE mode, bool &cubeChanged )
{
	this->mode = mode;		// change mode

	switch( mode )
	{
		case LORENZ:
			xODE	= lorenz->getInitState();	// set initial values
			maxCube	= -FLT_MAX;					// reset display scaling
			break;

		case ROESSLER:
			xODE	= roessler->getInitState();	// set initial values
			maxCube	= -FLT_MAX;					// reset display scaling
			break;
	}
	calcMaxCube( cubeChanged );		// recalculate maxCube

	modeChanged = true;		// set changed marker
}

ode_state Calculator::step( bool &cubeChanged )
{
	switch( mode )
	{
		case LORENZ:
			stepper.do_step( *lorenz, xODE, 0.0, lorenz->getDeltaT() );
			break;

		case ROESSLER:
			stepper.do_step( *roessler, xODE, 0.0, roessler->getDeltaT() );
			break;
	}
	calcMaxCube( cubeChanged );		// recalculate maxCube

	return xODE;
}

void Calculator::setInitValues( ode_state xODE, bool &cubeChanged )
{
	this->xODE = xODE;

	calcMaxCube( cubeChanged );		// recalculate maxCube
}

double Calculator::calcMaxCube( bool &cubeChanged )
{
	cubeChanged = {};

	if( abs( xODE[0] ) > maxCube ) { maxCube = abs( xODE[0] ); cubeChanged = true; }
	if( abs( xODE[1] ) > maxCube ) { maxCube = abs( xODE[1] ); cubeChanged = true; }
	if( abs( xODE[2] ) > maxCube ) { maxCube = abs( xODE[2] ); cubeChanged = true; }

	return maxCube;
}

ode_state Calculator::getValue()
{
	return xODE;
}

double Calculator::getMaxCube()
{
	return maxCube;
}