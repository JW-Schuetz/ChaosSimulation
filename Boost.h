#pragma once

#include <array>
#include <boost/numeric/odeint.hpp>


using namespace std;
using namespace boost::numeric::odeint;


typedef array<double,3> ode_state;

typedef enum
{
	LORENZ = 0,
	ROESSLER,
} SYSTEM_MODE;