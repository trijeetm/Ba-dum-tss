//---------------------------------------------------------------------------
// name: main.cpp
// desc: audio stuff
//
// author: Trijeet Mukhopadhyay (trijeetm@stanford.edu)
//   date: 2014
//---------------------------------------------------------------------------

#include <iostream>

#include "dm-globals.h"
#include "dm-audio.h"
#include "dm-gfx.h"

using namespace std;

int main( int argc, char ** argv ) {
    // invoke graphics setup and loop
    if( !dm_gfx_init( argc, argv ) )
    {
        // error message
        cerr << "[drum-machine]: cannot initialize graphics/data system..." << endl;
        return -1;
    }

	// start real-time audio
    if( !dm_audio_init( DM_SRATE, DM_FRAMESIZE, DM_NUMCHANNELS ) )
    {
        // error message
        cerr << "[drum-machine]: cannot initialize real-time audio I/O..." << endl;
        return -1;
    }
    
    // start audio
    if( !dm_audio_start() )
    {
        // error message
        cerr << "[drum-machine]: cannot start real-time audio I/O..." << endl;
        return -1;
    }

    // dm_test();

    dm_gfx_loop();
    
	cerr << "I DIDNT DIE!" << endl;
    return 0;
}