//---------------------------------------------------------------------------
// name: dm-audio.cpp
// desc: audio stuff
//
// author: Trijeet Mukhopadhyay (trijeetm@stanford.edu)
//   date: 2014
//---------------------------------------------------------------------------
#include "dm-globals.h"

BKSim * Globals::sim = NULL;

SAMPLE * Globals::lastAudioBuffer = NULL;
SAMPLE * Globals::lastAudioBufferMono = NULL;
SAMPLE * Globals::audioBufferWindow = NULL;
unsigned int Globals::lastAudioBufferFrames = 0;
unsigned int Globals::lastAudioBufferChannels = 0;

int Globals::test = 0;

int Globals::bpm = 120;
SAMPLE Globals::now = 0;
SAMPLE Globals::accumulator = 0;
SAMPLE Globals::tempoPeriod = (1.0 / (Globals::bpm * 4 / 60.0)) * DM_SRATE;

noteStack Globals::drumGrid[DM_N_GRID_ELEMENTS]; 
noteStack Globals::pianoGrid[DM_N_GRID_ELEMENTS];
noteStack Globals::synthGrid[DM_N_GRID_ELEMENTS];
int Globals::activeSynth = DM_DRUMS;

int Globals::playhead = 0;

int Globals::currentPatch = 0;
int Globals::currentNote = 35;

int Globals::currentPianoPatch = 0;
int Globals::currentPianoNote = 24;

int Globals::currentSynthPatch = 0;
int Globals::currentSynthNote = 48;

// RtAudio * dm_adac = NULL;