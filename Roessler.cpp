
#include "Roessler.h"


Roessler::Roessler( double a, double b, double c, double deltaT, ode_state initState ):
    a( a ), b( b ), c( c ), deltaT( deltaT ), initState( initState )
{
}

void Roessler::operator() ( const ode_state &x, ode_state &dxdt, const double )
{
    // Roessler-attractor (a=0.2, b=0.2, c=5.7)
    dxdt[0] = -x[1] - x[2];
    dxdt[1] = x[0] + a * x[1];
    dxdt[2] = b + x[2] * ( x[0] - c );
}

double Roessler::getDeltaT()
{
    return deltaT;
}

ode_state Roessler::getInitState()
{
    return initState;
}