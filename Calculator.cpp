
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
}

void Calculator::changeMode( SYSTEM_MODE mode )
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

	modeChanged = true;							// set marker
}

ode_state Calculator::step()
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

	return xODE;
}

void Calculator::setInitValues( ode_state xODE )
{
	this->xODE = xODE;
}

double Calculator::calcMaxCube( bool &cubeChanged )
{
	cubeChanged = false;

	if( abs( xODE[0] ) > maxCube ) { maxCube = abs( xODE[0] ); cubeChanged = true; }
	if( abs( xODE[1] ) > maxCube ) { maxCube = abs( xODE[1] ); cubeChanged = true; }
	if( abs( xODE[2] ) > maxCube ) { maxCube = abs( xODE[2] ); cubeChanged = true; }

	return maxCube;
}