//--------------------------------------------------------------------------
// name: dm-gfx.h
// desc: gfx stuff
//
// author: Trijeet Mukhopadhyay (trijeetm@stanford.edu)
//   date: 2014
//--------------------------------------------------------------------------
#ifndef __DM_GFX_H__
#define __DM_GFX_H__

#include "RtAudio.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <thread>
#include "x-def.h"
#include "x-gfx.h"
#include <string>


// entry point for graphics
bool dm_gfx_init( int argc, char ** argv );
void dm_gfx_loop();

#endif
