
//#define WITH_MEM_LEAK_TESTING

#ifdef WITH_MEM_LEAK_TESTING
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include <fstream>

#include "Common.h"
#include "DateTime.h"
#include "Viewer.h"


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
#ifdef WITH_MEM_LEAK_TESTING
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 180 );
#endif
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
#ifdef WITH_MEM_LEAK_TESTING
		_CrtMemState s1;
		_CrtMemCheckpoint( &s1 );
#endif
		Viewer *v = new Viewer( argv[1] );
		v->render();
		delete v;
#ifdef WITH_MEM_LEAK_TESTING
		_CrtMemDumpAllObjectsSince( &s1 );
		//bool leak = _CrtDumpMemoryLeaks();
#endif

		return 0;
	}
	catch( string& e )
	{
		logMessage( e );
	}

	return -1;
}