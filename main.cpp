
#include <iostream>
#include <fstream>

#include "Common.h"
#include "DateTime.h"
#include "Simulation.h"


using namespace std;


ofstream logger{};
DateTime datetime = DateTime();


void logMessage( string e )
{
	datetime.setTime();
	logger << '\n' << datetime.getDateTime() << " - Runtime exception: " << e << '\n';
	logger.flush();
}

// Without console
// linker options: Subsystem=windows, EntryPoint=mainCRTstartup
int main( int argc, char** argv )
{
	if( argc < 2 )
	{
		// no logging file -> output to stderr
		datetime.setTime();
		cerr << datetime.getDateTime() << " - Insufficient argument count" << '\n';
		cerr.flush();
		return -1;
	}

	// create logging file
	logger = ofstream { argv[1], ios_base::app };
	if( !logger )
	{
		datetime.setTime();
		cerr << datetime.getDateTime() << " - Cannot open logfile" << '\n';
		cerr.flush();
		return -1;
	}

	try
	{
		Simulation *s = new Simulation( argv[1] );
		s->render();
		delete s;
		return 0;
	}
	catch( string& e )
	{
		logMessage( e );
	}
	return -1;
}