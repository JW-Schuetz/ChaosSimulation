
#include "Lorenz.h"


Lorenz::Lorenz( double a, double b, double c, double deltaT, ode_state initState ):
    a( a ), b( b ), c( c ), deltaT( deltaT ), initState( initState )
{
}

void Lorenz::operator() ( const ode_state &x, ode_state &dxdt, const double )
{    // Lorenz-attractor (a=10, b=28, c=8/3)
    dxdt[0] = a * ( x[1] - x[0] );
    dxdt[1] = x[0] * ( b - x[2] ) - x[1];
    dxdt[2] = x[0] * x[1] - c * x[2];
}

double Lorenz::getDeltaT()
{
    return deltaT;
}

ode_state Lorenz::getInitState()
{
    return initState;
}