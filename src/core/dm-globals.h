//-----------------------------------------------------------------------------
// name: dm-audio.h
// desc: audio stuff
//
// author: Trijeet Mukhopadhyay (trijeetm@stanford.edu)
//   date: 2014
//-----------------------------------------------------------------------------
#ifndef __DM_GLOBALS_H__
#define __DM_GLOBALS_H__

#include <string>
#include <map>
#include <vector>
#include <utility>

#include "x-audio.h"
#include "y-entity.h"
#include "bk-sim.h"
// #include "RtAudio.h"

// defines
#define DM_SRATE        44100
#define DM_FRAMESIZE    512
#define DM_NUMCHANNELS  2
#define DM_MAX_TEXTURES 32
#define DM_N_GRID_ELEMENTS 16
#define DM_POLYPHONY    8
#define DM_DRUMS        0
#define DM_PIANOS       1
#define DM_SYNTH        2

struct note {
    bool on;
    int channel;
    int patch;
    float pitch;
    float duration;
    float velocity;

    note() {
        on = 0;
        channel = 0;
        patch = 0;
        pitch = 0;
        duration = 0;
        velocity = 0;
    }

    note(bool state, int c, int pat, float p, float d, float v) {
        on = state;
        channel = c;
        patch = pat;
        pitch = p;
        duration = d;
        velocity = v;
    }
};

struct noteStack {
    note notes[DM_POLYPHONY];
    int totalPatches;

    noteStack() {
        totalPatches = 0;
        for (int i = 0; i < DM_POLYPHONY; i++) {
            notes[i].on = false;
            notes[i].channel = 1;
            notes[i].patch = i;
            notes[i].pitch = 0;
            notes[i].duration = 0;
            notes[i].velocity = 0;
        }
    }
};

//-----------------------------------------------------------------------------
// name: class Globals
// desc: the global class
//-----------------------------------------------------------------------------
class Globals
{
public:
    // top level root simulation
    static BKSim * sim;
    
    // // path
    // static std::string path;
    // // path to datapath
    // static std::string relpath;
    // // datapath
    // static std::string datapath;
    // // version
    // static std::string version;

    // last audio buffer
    static SAMPLE * lastAudioBuffer;
    static SAMPLE * lastAudioBufferMono;
    static SAMPLE * audioBufferWindow;
    static unsigned int lastAudioBufferFrames;
    static unsigned int lastAudioBufferChannels;

    static int test;

    static SAMPLE now;
    static SAMPLE accumulator;
    static SAMPLE tempoPeriod;
    static int bpm;

    static noteStack drumGrid[DM_N_GRID_ELEMENTS];
    static noteStack pianoGrid[DM_N_GRID_ELEMENTS];
    static noteStack synthGrid[DM_N_GRID_ELEMENTS];
    static int activeSynth;

    static int playhead;

    static int currentPatch;
    static int currentNote;

    static int currentPianoPatch;
    static int currentPianoNote;

    static int currentSynthPatch;
    static int currentSynthNote;

    // RtAudio
    // static RtAudio * dm_adac;

    // // waveform
    // static YWaveform * waveform;

    // // width and height of the window
    // static GLsizei windowWidth;
    // static GLsizei windowHeight;
    // static GLsizei lastWindowWidth;
    // static GLsizei lastWindowHeight;

    // // graphics fullscreen
    // static GLboolean fullscreen;
    // // render waveform
    // static GLboolean renderWaveform;
    // // blend pane instead of clearing screen
    // static GLboolean blendScreen;
    // // blend screen parameters
    // static Vector3D blendAlpha;
    // static GLfloat blendRed;
    // // fill mode
    // static GLenum fillmode;
    // // background color
    // static iSlew3D bgColor;
    // // view stuff
    // static Vector3D viewRadius;
    // static Vector3D viewEyeY;
    // static Vector3D fov;
    
    // // textures
    // static GLuint textures[];
    
    // // light 0 position
    // static GLfloat light0_pos[4];
    // // light 1 parameters
    // static GLfloat light1_ambient[4];
    // static GLfloat light1_diffuse[4];
    // static GLfloat light1_specular[4];
    // static GLfloat light1_pos[4];
    // // line width
    // static GLfloat linewidth;
    // // do
    // static GLboolean fog;
    // static GLuint fog_mode[4];   // storage for three/four types of fog
    // static GLuint fog_filter;    // which fog to use
    // static GLfloat fog_density;  // fog density
    
    // // colors
    // static Vector3D ourWhite;
    // static Vector3D ourRed;
    // static Vector3D ourBlue;
    // static Vector3D ourOrange;
    // static Vector3D ourGreen;
    // static Vector3D ourGray;
    // static Vector3D ourYellow;
    // static Vector3D ourSoftYellow;
    // static Vector3D ourPurple;
};


#endif
