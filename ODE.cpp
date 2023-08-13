
#include "ODE.h"


ODE::ODE( double a, double b, double c ) : a( a ), b( b ), c( c )
{
}

void ODE::operator() ( const ode_state & x, ode_state & dxdt, const double )
{
    // Lorenz-attractor
    dxdt[0] = a * ( x[1] - x[0] );
    dxdt[1] = x[0] * ( b - x[2] ) - x[1];
    dxdt[2] = x[0] * x[1] - c * x[2];
}