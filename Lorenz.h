#pragma once

#include "Boost.h"


class Lorenz
{
    double a, b, c;                 // system parameter
    double deltaT;                  // time increment
    ode_state initState;            // initial state

public:
    Lorenz( double, double, double, double, ode_state );

    void operator() ( const ode_state &, ode_state &, const double );

    double getDeltaT();             // get time increment
    ode_state getInitState();       // get start values
};