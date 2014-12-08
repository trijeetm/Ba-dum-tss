//-----------------------------------------------------------------------------
// name: dm-audio.cpp
//
// author: Trijeet Mukhopadhyay (trijeetm@stanford.edu)
//   date: 2014
//-----------------------------------------------------------------------------
#include "dm-audio.h"
#include "x-audio.h"
#include "y-fluidsynth.h"
#include "y-echo.h"
#include "x-fun.h"
#include <iostream>
#include <time.h>

using namespace std;

// globals
YFluidSynth * g_synth;
// mYEcho * g_echo;

void dm_playSynths() {
    bool noteplayed = false;
    int drumNotesPlayed = 0;
    // clear notes
    for (int i = 0; i < DM_POLYPHONY; i++) {
        if (Globals::pianoGrid[Globals::playhead].notes[i].on) {
            g_synth->allNotesOff(1);
        }
        if (Globals::synthGrid[Globals::playhead].notes[i].on) {
            g_synth->allNotesOff(2);
        }
    }
    for (int i = 0; i < DM_POLYPHONY; i++) {
        if (Globals::drumGrid[Globals::playhead].notes[i].on) {
            // play midi drums
            g_synth->programChange(0, 69);
            if (i % 4 == 0)
                g_synth->noteOn(0, Globals::drumGrid[Globals::playhead].notes[i].pitch, XFun::rand2i(7, 120));
            else if (i % 2 == 0)
                g_synth->noteOn(0, Globals::drumGrid[Globals::playhead].notes[i].pitch, XFun::rand2i(15, 100));
            else
                g_synth->noteOn(0, Globals::drumGrid[Globals::playhead].notes[i].pitch, XFun::rand2i(15, 75));
            noteplayed = true;
            drumNotesPlayed++;
        }
        if (Globals::pianoGrid[Globals::playhead].notes[i].on) {
            // play midi pianos
            g_synth->programChange(1, 36);
            if (i % 4 == 0) {
                g_synth->noteOn(1, Globals::pianoGrid[Globals::playhead].notes[i].pitch, XFun::rand2i(27, 100));
                g_synth->noteOn(1, Globals::pianoGrid[Globals::playhead].notes[i].pitch + 12, XFun::rand2i(27, 100));
                // g_synth->noteOn(1, Globals::pianoGrid[Globals::playhead].notes[i].pitch + 7, XFun::rand2i(27, 100));
            }
            else if (i % 2 == 0) {
                g_synth->noteOn(1, Globals::pianoGrid[Globals::playhead].notes[i].pitch, XFun::rand2i(20, 95));
                g_synth->noteOn(1, Globals::pianoGrid[Globals::playhead].notes[i].pitch + 12, XFun::rand2i(20, 95));
                // g_synth->noteOn(1, Globals::pianoGrid[Globals::playhead].notes[i].pitch + 7, XFun::rand2i(20, 95));
            }
            else {
                g_synth->noteOn(1, Globals::pianoGrid[Globals::playhead].notes[i].pitch, XFun::rand2i(25, 70));
                g_synth->noteOn(1, Globals::pianoGrid[Globals::playhead].notes[i].pitch + 12, XFun::rand2i(25, 70));
                // g_synth->noteOn(1, Globals::pianoGrid[Globals::playhead].notes[i].pitch + 7, XFun::rand2i(30, 75));
            }
            // noteplayed = true;
            // drumNotesPlayed++;
        }
        if (Globals::synthGrid[Globals::playhead].notes[i].on) {
            // play midi synth chords
            g_synth->programChange(2, 4);
            if (i % 4 == 0) {
                g_synth->noteOn(2, Globals::synthGrid[Globals::playhead].notes[i].pitch, XFun::rand2i(27, 100));
                g_synth->noteOn(2, Globals::synthGrid[Globals::playhead].notes[i].pitch + 12, XFun::rand2i(27, 100));
                g_synth->noteOn(2, Globals::synthGrid[Globals::playhead].notes[i].pitch + 7, XFun::rand2i(27, 100));
            }
            else if (i % 2 == 0) {
                g_synth->noteOn(2, Globals::synthGrid[Globals::playhead].notes[i].pitch, XFun::rand2i(20, 95));
                g_synth->noteOn(2, Globals::synthGrid[Globals::playhead].notes[i].pitch + 12, XFun::rand2i(20, 95));
                g_synth->noteOn(2, Globals::synthGrid[Globals::playhead].notes[i].pitch + 7, XFun::rand2i(20, 95));
            }
            else {
                g_synth->noteOn(2, Globals::synthGrid[Globals::playhead].notes[i].pitch, XFun::rand2i(25, 70));
                g_synth->noteOn(2, Globals::synthGrid[Globals::playhead].notes[i].pitch + 12, XFun::rand2i(25, 70));
                g_synth->noteOn(2, Globals::synthGrid[Globals::playhead].notes[i].pitch + 7, XFun::rand2i(30, 75));
            }
            // noteplayed = true;
            // drumNotesPlayed++;
        }
    }
    Globals::drumGrid[Globals::playhead].totalPatches = drumNotesPlayed;
    Globals::pianoGrid[Globals::playhead].totalPatches = drumNotesPlayed;
    Globals::synthGrid[Globals::playhead].totalPatches = drumNotesPlayed;
    // if (noteplayed)
    //     cerr << "•" << "\t";
    // else
    //     cerr << "x" << "\t";
}


//--------------------------------------------------------------------------
// name: audio_callback
// desc: audio callback
//--------------------------------------------------------------------------
static void audio_callback(SAMPLE * buffer, unsigned int numFrames, void * userData) {

    Globals::now += numFrames;

    // beat event
    Globals::tempoPeriod = (1.0 / (Globals::bpm * 4 / 60.0)) * DM_SRATE;
    if (Globals::accumulator > Globals::tempoPeriod) {
        Globals::accumulator -= Globals::tempoPeriod;
        dm_playSynths();
        Globals::playhead = (Globals::playhead + 1) % DM_N_GRID_ELEMENTS;
        // if (Globals::playhead == 0)
        //     cerr << endl;
    }
    Globals::accumulator += numFrames;
    
    // sum
    SAMPLE sum = 0;
    // // num channels
    // unsigned int channels = Globals::lastAudioBufferChannels;
    
    // // zero out
    // memset( Globals::lastAudioBuffer, 0,
    //        sizeof(SAMPLE)*Globals::lastAudioBufferFrames*channels );
    // memset( Globals::lastAudioBufferMono, 0,
    //        sizeof(SAMPLE)*Globals::lastAudioBufferFrames );
    
    // // copy to global buffer
    // memcpy( Globals::lastAudioBuffer, buffer,
    //        sizeof(SAMPLE)*numFrames*channels );
    
    // // copy to mono buffer
    // for( int i = 0; i < numFrames; i++ )
    // {
    //     // zero out
    //     sum = 0;
    //     // loop over channels
    //     for( int j = 0; j < channels; j++ )
    //     {
    //         // sum
    //         sum += buffer[i*channels + j];
    //     }
    //     // set
    //     Globals::lastAudioBufferMono[i] = sum / channels;
    // }

    // synthesize it
    g_synth->synthesize2( buffer, numFrames );
    // echo it
    // g_echo->synthesize2( buffer, numFrames );
}




//--------------------------------------------------------------------------
// name: dm_audio_init()
// desc: initialize audio system
//--------------------------------------------------------------------------
bool dm_audio_init( unsigned int srate, unsigned int frameSize, unsigned channels ) {
    // cout << "tempoPeriod = " << Globals::tempoPeriod << endl;
    // initialize
    if( !XAudioIO::init( 0, 0, srate, frameSize, channels, audio_callback, NULL ) )
    {
        // done
        return false;
    }

    XFun::srand();
    
    g_synth = new YFluidSynth();
    g_synth->init( srate, 32 );
    g_synth->load( "data/sfonts/rocking8m11e copy.sf2", "" );
    g_synth->programChange(0, 69);
    
    // // allocate echo
    // g_echo = new YEcho( srate );
    // g_echo->setDelay( 0, .25 );
    // g_echo->setDelay( 1, .5 );
    
    // allocate
    Globals::lastAudioBuffer = new SAMPLE[frameSize*channels];
    // allocate mono buffer
    Globals::lastAudioBufferMono = new SAMPLE[frameSize];
    // allocate window buffer
    Globals::audioBufferWindow = new SAMPLE[frameSize];
    // set frame size (could have changed in XAudioIO::init())
    Globals::lastAudioBufferFrames = frameSize;
    // set num channels
    Globals::lastAudioBufferChannels = channels;
    
    return true;
}




//--------------------------------------------------------------------------
// name: dm_audio_start()
// desc: start audio system
//--------------------------------------------------------------------------
bool dm_audio_start() {
    // start the audio
    if( !XAudioIO::start() ) {
        // done
        return false;
    }
    
    return true;
}
