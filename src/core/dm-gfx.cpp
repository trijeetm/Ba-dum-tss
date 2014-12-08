//--------------------------------------------------------------------------
// name: dm-gfx.cpp
// desc: gfx stuff
//
// author: Trijeet Mukhopadhyay (trijeetm@stanford.edu)
//   date: 2014
//--------------------------------------------------------------------------

#include "dm-gfx.h"
#include "dm-globals.h"
#include "y-entity.h"
#include "bk-sim.h"
#include "x-fun.h"

#include <stdlib.h>
using namespace std;


//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------
bool dm_gfx_init( int argc, const char ** argv );
void dm_gfx_loop();
void help();
void initOpenGL();
void initSim();
void initGfx();
void idleFunc();
void displayFunc();
void reshapeFunc( GLsizei width, GLsizei height );
void keyboardFunc( unsigned char, int, int );
void mouseFunc( int button, int state, int x, int y );
void specialFunc( int key, int x, int y );

// width and height
long g_width = 1024;
long g_height = 720;
long g_last_width = g_width;
long g_last_height = g_height;

// global variables
GLboolean g_fullscreen = FALSE;

// the grid
// master control
YEntity *g_drumGfx;
YEntity *g_pianoGfx;
YEntity *g_synthGfx;

// drum globals
YCubeOutline *g_playhead;
YCubeOutline *g_playheadTrail1;
YCubeOutline *g_playheadTrail2;
YEntity *g_allPatchEvents;
vector<YCube *> g_patchCubes;
YEntity *g_allPatches;
YEntity *g_drumLabels;
vector<YCube *> g_patches;
vector<YCubeOutline *> g_patchSelector;
YEntity *g_tutorial;
vector<YText *> keyOffLabels;
Vector3D gridCoords[DM_N_GRID_ELEMENTS];
iSlew3D drumSca(1, 1, 1, 0.25);
iSlew3D drumLoc(0, 0, 0, 0.25);

// piano globals
YCubeOutline *g_playheadPiano;
YCubeOutline *g_playheadTrail1Piano;
YCubeOutline *g_playheadTrail2Piano;
YEntity *g_allPatchEventsPiano;
vector<YCube *> g_patchCubesPiano;
YEntity *g_allPatchesPiano;
YEntity *g_pianoLabels;
vector<YCube *> g_patchesPiano;
vector<YCubeOutline *> g_patchSelectorPiano;
YEntity *g_tutorialPiano;
vector<YText *> keyOffLabelsPiano;
Vector3D gridCoordsPiano[DM_N_GRID_ELEMENTS];
iSlew3D pianoSca(0.25, 1, 1, 0.25);
iSlew3D pianoLoc(6, 0, -30, 0.25);

// synth globals
YCubeOutline *g_playheadSynth;
YCubeOutline *g_playheadTrail1Synth;
YCubeOutline *g_playheadTrail2Synth;
YEntity *g_allPatchEventsSynth;
vector<YCube *> g_patchCubesSynth;
YEntity *g_allPatchesSynth;
YEntity *g_synthLabels;
vector<YCube *> g_patchesSynth;
vector<YCubeOutline *> g_patchSelectorSynth;
YEntity *g_tutorialSynth;
vector<YText *> keyOffLabelsSynth;
Vector3D gridCoordsSynth[DM_N_GRID_ELEMENTS];
iSlew3D synthSca(0.15, 1, 1, 0.25);
iSlew3D synthLoc(12, 0, 0, 0.25);

int g_pianoNotes[DM_POLYPHONY] = {24, 27, 29, 31, 34, 36, 39, 41};
int g_synthNotes[DM_POLYPHONY] = {24 + 24, 27 + 24, 29 + 24, 31 + 24, 34 + 24, 36 + 24, 39 + 24, 41 + 24};
int g_drumNotes[DM_POLYPHONY] = {35, 36, 38, 40, 45, 47, 42, 46};

// tempo gfx globals 
YText *g_tempo;

// color scheme
Vector3D cream(1.0, 0.964, 0.898);
Vector3D seaGray(0.243, 0.27, 0.298);
Vector3D lGray(0.878, 0.872, 0.867);
Vector3D warmRed(1.0, 0.498, 0.4);
Vector3D blue(0.494, 0.807, 0.992);
// Vector3D lBlue(0.494, 0.807, 0.992);

Vector3D ourWhite( 1, 1, 1 );
Vector3D ourRed( 1, .5, .5 );
Vector3D ourBlue( 102.0f/255, 204.0f/255, 1.0f );
Vector3D ourOrange( 1, .75, .25 );
Vector3D ourGreen( .7, 1, .45 );
Vector3D ourGray( .4, .4, .4 );
Vector3D ourYellow( 1, 1, .25 );
Vector3D ourSoftYellow( .7, .7, .1 );
Vector3D ourPurple( .6, .25, .6 );

Vector3D patchColors[DM_POLYPHONY] = { seaGray, ourGray, warmRed, ourRed, ourOrange, ourYellow, ourBlue, ourPurple };

// Vector3D test(1, 1, 1);
// Vector3D testGoal(10, 10, 10);
// iSlew3D slewTest(test, 0.1);


//-----------------------------------------------------------------------------
// name: dm_gfx_init()
// desc: GLUT Init
//-----------------------------------------------------------------------------
bool dm_gfx_init( int argc, char ** argv ) {
    // openGL init
    // initOpenGL();
    // initialize GLUT
    glutInit( &argc, argv );

    // print help
    help();
    
    // do our own initialization
    initGfx();
    // simulation
    initSim();

    // slewTest.update(testGoal);
    // test.value = 1000;
    // // do data
    // if( !initialize_data() )
    // {
    //     // done
    //     return false;
    // }
    
    return true;
}

//------------------------------------------------------------------
// initGfx()
// App gfx data init
//------------------------------------------------------------------
void initGfx() {
    // double buffer, use rgb color, enable depth buffer
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    // initialize the window size
    glutInitWindowSize( g_width, g_height );
    // set the window postion
    glutInitWindowPosition( 100, 100 );
    // create the window
    glutCreateWindow("Da-da-drum-machine");
    
    // set the idle function - called when idleFunc
    glutIdleFunc( idleFunc );
    // set the display function - called when redrawing
    glutDisplayFunc( displayFunc );
    // set the reshape function - called when client area changes
    glutReshapeFunc( reshapeFunc );
    // set the keyboard function - called on keyboard events
    glutKeyboardFunc( keyboardFunc );
    // set the mouse function - called on mouse stuff
    glutMouseFunc( mouseFunc );
    // spec key
    glutSpecialFunc(specialFunc);

    // reset time
    XGfx::resetCurrentTime();
    // set simulation speed
    XGfx::setDeltaFactor( 1.0f );
    // get the first
    XGfx::getCurrentTime( true );
    // random
    XFun::srand();
    
    // set clear color
    glClearColor( 0, 0, 0, 1 );
    // enable color material
    glEnable( GL_COLOR_MATERIAL );
    // enable depth test
    glEnable( GL_DEPTH_TEST );
    // enable blending
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);

}

// drum gfx init func
void initDrumGfx() {
    g_drumGfx = new YEntity();
    // drum gfx
    // grid outline
    float yOffset = 2.25;
    g_tutorial = new YEntity();
    for (int i = 0; i < DM_N_GRID_ELEMENTS; i++) {
        YCubeOutline *gridOutline = new YCubeOutline();
        gridOutline->sca.setAll(1.0);
        gridCoords[i].set(1.5 * ((i % (DM_N_GRID_ELEMENTS / 4)) + 0.5 - (DM_N_GRID_ELEMENTS / 8)), 
            yOffset, 
            0);
        gridOutline->loc = gridCoords[i];
        gridOutline->outlineColor = lGray;
        gridOutline->lineWidth = 1.0;
        g_drumGfx->addChild(gridOutline);
        if (((i + 1) % (DM_N_GRID_ELEMENTS / 4)) == 0)
            yOffset -= 1.5;
        YText *helper = new YText();
        helper->setCenterLocation(Vector3D(gridCoords[i].x + 0.45, gridCoords[i].y, gridCoords[i].z));
        helper->col = lGray;
        helper->setStretch(2);
        switch (i) {
            case 0:
                helper->set("1");
            break;
            case 1:
                helper->set("2");
            break;
            case 2:
                helper->set("3");
            break;
            case 3:
                helper->set("4");
            break;
            case 4:
                helper->set("q");
            break;
            case 5:
                helper->set("w");
            break;
            case 6:
                helper->set("e");
            break;
            case 7:
                helper->set("r");
            break;
            case 8:
                helper->set("a");
            break;
            case 9:
                helper->set("s");
            break;
            case 10:
                helper->set("d");
            break;
            case 11:
                helper->set("f");
            break;
            case 12:
                helper->set("z");
            break;
            case 13:
                helper->set("x");
            break;
            case 14:
                helper->set("c");
            break;
            case 15:
                helper->set("v");
            break;
        }
        g_tutorial->addChild(helper);
        keyOffLabels.push_back(helper);
    }
    g_drumGfx->addChild(g_tutorial);

    // init playhead and trials
    g_playhead = new YCubeOutline();
    g_playhead->sca.setAll(1.1);
    g_playhead->loc = gridCoords[Globals::playhead];
    g_playhead->outlineColor = blue;
    g_playhead->lineWidth = 5;
    g_drumGfx->addChild(g_playhead);  

    g_playheadTrail1 = new YCubeOutline();
    g_playheadTrail1->sca.setAll(1.05);
    g_playheadTrail1->loc = gridCoords[Globals::playhead];
    g_playheadTrail1->outlineColor = blue;
    g_playheadTrail1->lineWidth = 3;
    g_drumGfx->addChild(g_playheadTrail1);  

    // g_playheadTrail2 = new YCubeOutline();
    // g_playheadTrail2->sca.setAll(1.01);
    // g_playheadTrail2->loc = gridCoords[Globals::playhead];
    // g_playheadTrail2->outlineColor = blue;
    // g_playheadTrail2->lineWidth = 1;
    // g_drumGfx->addChild(g_playheadTrail2);   

    // init patch events
    g_allPatchEvents = new YEntity();
    for (int i = 0; i < DM_N_GRID_ELEMENTS; i++) {
        for (int j = 0; j < DM_POLYPHONY; j++) {
            YCube *cube = new YCube();
            cube->sca.set(1.0, 1.0 / DM_POLYPHONY, 1);
            cube->loc.set(gridCoords[i][0], 
                gridCoords[i][1] - 0.5 + (1.0 / (2 * DM_POLYPHONY)) + (j * (1.0 / (1 * DM_POLYPHONY))), 
                gridCoords[i][2]);
            cube->col = patchColors[Globals::drumGrid[i].notes[j].patch];
            cube->active = false;
            g_patchCubes.push_back(cube);
            g_allPatchEvents->addChild(cube);
        }
    }
    g_drumGfx->addChild(g_allPatchEvents);

    // init patches selector
    g_allPatches = new YEntity();
    g_drumLabels = new YEntity();
    for (int i = 0; i < DM_POLYPHONY; i++) {
        YCube *cube = new YCube();
        cube->sca.set(1.0, 0.3, 1);
        cube->loc.set(((i - (DM_POLYPHONY / 2)) * 1.2) + 0.6, 
            3.5, 
            0);
        cube->col = patchColors[i];
        g_patches.push_back(cube);
        g_allPatches->addChild(cube);
        // patch selector
        YCubeOutline *cubeOutline = new YCubeOutline();
        cubeOutline->active = false;
        cubeOutline->sca.set(1.05, 0.35, 1.05);
        cubeOutline->lineWidth = 2;
        cubeOutline->loc.set(((i - (DM_POLYPHONY / 2)) * 1.2) + 0.6, 
            3.5, 
            0);
        cubeOutline->outlineColor = seaGray;
        g_patchSelector.push_back(cubeOutline);
        g_allPatches->addChild(cubeOutline);
        // patch label
        YText *label = new YText();
        switch (i) {
            case 0:
                label->set("Bass 1 ( , )");
            break;
            case 1:
                label->set("Bass 2 ( . )");
            break;
            case 2:
                label->set("Snare 1 ( L )");
            break;
            case 3:
                label->set("Snare 2 ( ; )");
            break;
            case 4:
                label->set("Mid toms ( p )");
            break;
            case 5:
                label->set("High toms ( [ )");
            break;
            case 6:
                label->set("Hi Hat ( - )");
            break;
            case 7:
                label->set("Cymbal ( + )");
            break;
        }
        label->col = seaGray;
        Vector3D labelLoc(((i - (DM_POLYPHONY / 2)) * 1.2) + 0.6, 
            3.75, 
            0);
        label->setCenterLocation(labelLoc);
        g_drumLabels->addChild(label);
    }
    g_drumGfx->addChild(g_allPatches);
    g_drumGfx->addChild(g_drumLabels);

    YText *instrument = new YText();
    instrument->col = seaGray;
    instrument->set("Drums");
    instrument->setStretch(3);
    instrument->setCenterLocation(Vector3D(0, -3.25, 0));
    g_drumGfx->addChild(instrument);

    // add drum gfx to root
    Globals::sim->root().addChild(g_drumGfx);
}

// piano gfx init func
void initPianoGfx() {
    g_pianoGfx = new YEntity();
    // piano gfx
    // grid outline
    float yOffset = 2.25;
    g_tutorialPiano = new YEntity();
    for (int i = 0; i < DM_N_GRID_ELEMENTS; i++) {
        YCubeOutline *gridOutline = new YCubeOutline();
        gridOutline->sca.setAll(1.0);
        gridCoords[i].set(1.5 * ((i % (DM_N_GRID_ELEMENTS / 4)) + 0.5 - (DM_N_GRID_ELEMENTS / 8)), 
            yOffset, 
            0);
        gridOutline->loc = gridCoords[i];
        gridOutline->outlineColor = lGray;
        gridOutline->lineWidth = 1.0;
        g_pianoGfx->addChild(gridOutline);
        if (((i + 1) % (DM_N_GRID_ELEMENTS / 4)) == 0)
            yOffset -= 1.5;
        YText *helper = new YText();
        helper->setCenterLocation(Vector3D(gridCoords[i].x + 0.45, gridCoords[i].y, gridCoords[i].z));
        helper->col = lGray;
        helper->setStretch(2);
        switch (i) {
            case 0:
                helper->set("1");
            break;
            case 1:
                helper->set("2");
            break;
            case 2:
                helper->set("3");
            break;
            case 3:
                helper->set("4");
            break;
            case 4:
                helper->set("q");
            break;
            case 5:
                helper->set("w");
            break;
            case 6:
                helper->set("e");
            break;
            case 7:
                helper->set("r");
            break;
            case 8:
                helper->set("a");
            break;
            case 9:
                helper->set("s");
            break;
            case 10:
                helper->set("d");
            break;
            case 11:
                helper->set("f");
            break;
            case 12:
                helper->set("z");
            break;
            case 13:
                helper->set("x");
            break;
            case 14:
                helper->set("c");
            break;
            case 15:
                helper->set("v");
            break;
        }
        g_tutorialPiano->addChild(helper);
        keyOffLabelsPiano.push_back(helper);
    }
    g_pianoGfx->addChild(g_tutorialPiano);

    // init playhead and trials
    g_playheadPiano = new YCubeOutline();
    g_playheadPiano->sca.setAll(1.1);
    g_playheadPiano->loc = gridCoords[Globals::playhead];
    g_playheadPiano->outlineColor = blue;
    g_playheadPiano->lineWidth = 5;
    g_pianoGfx->addChild(g_playheadPiano);  

    g_playheadTrail1Piano = new YCubeOutline();
    g_playheadTrail1Piano->sca.setAll(1.05);
    g_playheadTrail1Piano->loc = gridCoords[Globals::playhead];
    g_playheadTrail1Piano->outlineColor = blue;
    g_playheadTrail1Piano->lineWidth = 3;
    g_pianoGfx->addChild(g_playheadTrail1Piano);  

    // g_playheadTrail2 = new YCubeOutline();
    // g_playheadTrail2->sca.setAll(1.01);
    // g_playheadTrail2->loc = gridCoords[Globals::playhead];
    // g_playheadTrail2->outlineColor = blue;
    // g_playheadTrail2->lineWidth = 1;
    // g_pianoGfx->addChild(g_playheadTrail2);   

    // init patch events
    g_allPatchEventsPiano = new YEntity();
    for (int i = 0; i < DM_N_GRID_ELEMENTS; i++) {
        for (int j = 0; j < DM_POLYPHONY; j++) {
            YCube *cube = new YCube();
            cube->sca.set(1.0, 1.0 / DM_POLYPHONY, 1);
            cube->loc.set(gridCoords[i][0], 
                gridCoords[i][1] - 0.5 + (1.0 / (2 * DM_POLYPHONY)) + (j * (1.0 / (1 * DM_POLYPHONY))), 
                gridCoords[i][2]);
            cube->col = patchColors[Globals::drumGrid[i].notes[j].patch];
            cube->active = false;
            g_patchCubesPiano.push_back(cube);
            g_allPatchEventsPiano->addChild(cube);
        }
    }
    g_pianoGfx->addChild(g_allPatchEventsPiano);

    // init patches selector
    g_allPatchesPiano = new YEntity();
    g_pianoLabels = new YEntity();
    for (int i = 0; i < DM_POLYPHONY; i++) {
        YCube *cube = new YCube();
        cube->sca.set(1.0, 0.3, 1);
        cube->loc.set(((i - (DM_POLYPHONY / 2)) * 1.2) + 0.6, 
            3.5, 
            0);
        cube->col = patchColors[i];
        g_patchesPiano.push_back(cube);
        g_allPatchesPiano->addChild(cube);
        // patch selector
        YCubeOutline *cubeOutline = new YCubeOutline();
        cubeOutline->active = false;
        cubeOutline->sca.set(1.05, 0.35, 1.05);
        cubeOutline->loc.set(((i - (DM_POLYPHONY / 2)) * 1.2) + 0.6, 
            3.5, 
            0);
        cubeOutline->outlineColor = seaGray;
        cubeOutline->lineWidth = 2;
        g_patchSelectorPiano.push_back(cubeOutline);
        g_allPatchesPiano->addChild(cubeOutline);
        // patch label
        YText *label = new YText();
        switch (i) {
            case 0:
                label->set("C 1 ( , )");
            break;
            case 1:
                label->set("D\x23 1 ( . )");
            break;
            case 2:
                label->set("F 1 ( L )");
            break;
            case 3:
                label->set("G 1 ( ; )");
            break;
            case 4:
                label->set("A\x23 1 ( p )");
            break;
            case 5:
                label->set("C 2 ( [ )");
            break;
            case 6:
                label->set("D\x23 2 ( - )");
            break;
            case 7:
                label->set("F 2 ( + )");
            break;
        }
        label->col = seaGray;
        Vector3D labelLoc(((i - (DM_POLYPHONY / 2)) * 1.2) + 0.6, 
            3.75, 
            0);
        label->setCenterLocation(labelLoc);
        g_pianoLabels->addChild(label);
    }
    g_pianoGfx->addChild(g_allPatchesPiano);
    g_pianoGfx->addChild(g_pianoLabels);

    YText *instrument = new YText();
    instrument->col = seaGray;
    instrument->set("Bass");
    instrument->setStretch(3);
    instrument->setCenterLocation(Vector3D(0, -3.25, 0));
    g_pianoGfx->addChild(instrument);

    // add drum gfx to root
    Globals::sim->root().addChild(g_pianoGfx);
}

// piano gfx init func
void initSynthGfx() {
    g_synthGfx = new YEntity();
    // piano gfx
    // grid outline
    float yOffset = 2.25;
    g_tutorialSynth = new YEntity();
    for (int i = 0; i < DM_N_GRID_ELEMENTS; i++) {
        YCubeOutline *gridOutline = new YCubeOutline();
        gridOutline->sca.setAll(1.0);
        gridCoords[i].set(1.5 * ((i % (DM_N_GRID_ELEMENTS / 4)) + 0.5 - (DM_N_GRID_ELEMENTS / 8)), 
            yOffset, 
            0);
        gridOutline->loc = gridCoords[i];
        gridOutline->outlineColor = lGray;
        gridOutline->lineWidth = 1.0;
        g_synthGfx->addChild(gridOutline);
        if (((i + 1) % (DM_N_GRID_ELEMENTS / 4)) == 0)
            yOffset -= 1.5;
        YText *helper = new YText();
        helper->setCenterLocation(Vector3D(gridCoords[i].x + 0.45, gridCoords[i].y, gridCoords[i].z));
        helper->col = lGray;
        helper->setStretch(2);
        switch (i) {
            case 0:
                helper->set("1");
            break;
            case 1:
                helper->set("2");
            break;
            case 2:
                helper->set("3");
            break;
            case 3:
                helper->set("4");
            break;
            case 4:
                helper->set("q");
            break;
            case 5:
                helper->set("w");
            break;
            case 6:
                helper->set("e");
            break;
            case 7:
                helper->set("r");
            break;
            case 8:
                helper->set("a");
            break;
            case 9:
                helper->set("s");
            break;
            case 10:
                helper->set("d");
            break;
            case 11:
                helper->set("f");
            break;
            case 12:
                helper->set("z");
            break;
            case 13:
                helper->set("x");
            break;
            case 14:
                helper->set("c");
            break;
            case 15:
                helper->set("v");
            break;
        }
        g_tutorialSynth->addChild(helper);
        keyOffLabelsSynth.push_back(helper);
    }
    g_synthGfx->addChild(g_tutorialSynth);

    // init playhead and trials
    g_playheadSynth = new YCubeOutline();
    g_playheadSynth->sca.setAll(1.1);
    g_playheadSynth->loc = gridCoords[Globals::playhead];
    g_playheadSynth->outlineColor = blue;
    g_playheadSynth->lineWidth = 5;
    g_synthGfx->addChild(g_playheadSynth);  

    g_playheadTrail1Synth = new YCubeOutline();
    g_playheadTrail1Synth->sca.setAll(1.05);
    g_playheadTrail1Synth->loc = gridCoords[Globals::playhead];
    g_playheadTrail1Synth->outlineColor = blue;
    g_playheadTrail1Synth->lineWidth = 3;
    g_synthGfx->addChild(g_playheadTrail1Synth);  

    // g_playheadTrail2 = new YCubeOutline();
    // g_playheadTrail2->sca.setAll(1.01);
    // g_playheadTrail2->loc = gridCoords[Globals::playhead];
    // g_playheadTrail2->outlineColor = blue;
    // g_playheadTrail2->lineWidth = 1;
    // g_synthGfx->addChild(g_playheadTrail2);   

    // init patch events
    g_allPatchEventsSynth = new YEntity();
    for (int i = 0; i < DM_N_GRID_ELEMENTS; i++) {
        for (int j = 0; j < DM_POLYPHONY; j++) {
            YCube *cube = new YCube();
            cube->sca.set(1.0, 1.0 / DM_POLYPHONY, 1);
            cube->loc.set(gridCoords[i][0], 
                gridCoords[i][1] - 0.5 + (1.0 / (2 * DM_POLYPHONY)) + (j * (1.0 / (1 * DM_POLYPHONY))), 
                gridCoords[i][2]);
            cube->col = patchColors[Globals::drumGrid[i].notes[j].patch];
            cube->active = false;
            g_patchCubesSynth.push_back(cube);
            g_allPatchEventsSynth->addChild(cube);
        }
    }
    g_synthGfx->addChild(g_allPatchEventsSynth);

    // init patches selector
    g_allPatchesSynth = new YEntity();
    g_synthLabels = new YEntity();
    for (int i = 0; i < DM_POLYPHONY; i++) {
        YCube *cube = new YCube();
        cube->sca.set(1.0, 0.3, 1);
        cube->loc.set(((i - (DM_POLYPHONY / 2)) * 1.2) + 0.6, 
            3.5, 
            0);
        cube->col = patchColors[i];
        g_patchesPiano.push_back(cube);
        g_allPatchesSynth->addChild(cube);
        // patch selector
        YCubeOutline *cubeOutline = new YCubeOutline();
        cubeOutline->active = false;
        cubeOutline->sca.set(1.05, 0.35, 1.05);
        cubeOutline->loc.set(((i - (DM_POLYPHONY / 2)) * 1.2) + 0.6, 
            3.5, 
            0);
        cubeOutline->outlineColor = seaGray;
        cubeOutline->lineWidth = 2;
        g_patchSelectorSynth.push_back(cubeOutline);
        g_allPatchesSynth->addChild(cubeOutline);
        // patch label
        YText *label = new YText();
        switch (i) {
            case 0:
                label->set("C 1 ( , )");
            break;
            case 1:
                label->set("D\x23 1 ( . )");
            break;
            case 2:
                label->set("F 1 ( L )");
            break;
            case 3:
                label->set("G 1 ( ; )");
            break;
            case 4:
                label->set("A\x23 1 ( p )");
            break;
            case 5:
                label->set("C 2 ( [ )");
            break;
            case 6:
                label->set("D\x23 2 ( - )");
            break;
            case 7:
                label->set("F 2 ( + )");
            break;
        }
        label->col = seaGray;
        Vector3D labelLoc(((i - (DM_POLYPHONY / 2)) * 1.2) + 0.6, 
            3.75, 
            0);
        label->setCenterLocation(labelLoc);
        g_synthLabels->addChild(label);
    }
    g_synthGfx->addChild(g_allPatchesSynth);
    g_synthGfx->addChild(g_synthLabels);

    YText *instrument = new YText();
    instrument->col = seaGray;
    instrument->set("Electric Piano");
    instrument->setStretch(3);
    instrument->setCenterLocation(Vector3D(0, -3.25, 0));
    g_synthGfx->addChild(instrument);

    // add synth gfx to root
    Globals::sim->root().addChild(g_synthGfx);
}

//------------------------------------------------------------------
// initSim()
// Simulation init
//------------------------------------------------------------------
void initSim() {
    // instantiate simulation
    Globals::sim = new BKSim();

    // init drum gfx
    initDrumGfx();

    // init piano gfx
    initPianoGfx();

    // init synth gfx
    initSynthGfx();

    // tempo and controls
    g_tempo = new YText();
    g_tempo->loc.set(-0.5, -3.75, 0);
    g_tempo->col = seaGray;
    g_tempo->set("Tempo = 120");
    Globals::sim->root().addChild(g_tempo);
    YText *tempoUp = new YText();
    tempoUp->loc.set(-2.1, -3.95, 0);
    tempoUp->col = seaGray;
    tempoUp->set("<arrow-down> dec. tempo by 5");
    Globals::sim->root().addChild(tempoUp);
    YText *tempoDown = new YText();
    tempoDown->loc.set(0.25, -3.95, 0);
    tempoDown->col = seaGray;
    tempoDown->set("<arrow-up> inc. tempo by 5");
    Globals::sim->root().addChild(tempoDown);
    YText *cyclePatches = new YText();
    cyclePatches->loc.set(-1.75, -4.15, 0);
    cyclePatches->col = seaGray;
    cyclePatches->set("<arrow-left> and <arrow-right> to circle b/w patches");
    Globals::sim->root().addChild(cyclePatches);
}

//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void initOpenGL()
{
    // double buffer, use rgb color, enable depth buffer
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    // initialize the window size
    glutInitWindowSize( g_width, g_height );
    // set the window postion
    glutInitWindowPosition( 100, 100 );
    // create the window
    glutCreateWindow("Da-da-drum-machine");
    
    // set the idle function - called when idleFunc
    glutIdleFunc( idleFunc );
    // set the display function - called when redrawing
    glutDisplayFunc( displayFunc );
    // set the reshape function - called when client area changes
    glutReshapeFunc( reshapeFunc );
    // set the keyboard function - called on keyboard events
    glutKeyboardFunc( keyboardFunc );
    // set the mouse function - called on mouse stuff
    glutMouseFunc( mouseFunc );
    
    // set clear color
    glClearColor( 0, 0, 0, 1 );
    // enable color material
    glEnable( GL_COLOR_MATERIAL );
    // enable depth test
    glEnable( GL_DEPTH_TEST );
    // enable blending
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
}




//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void reshapeFunc( GLsizei w, GLsizei h )
{
    // save the new window size
    g_width = w; g_height = h;
    // map the view port to the client area
    glViewport( 0, 0, w, h );
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    glLoadIdentity( );
    // create the viewing frustum
    gluPerspective( 5.0, (GLfloat) w / (GLfloat) h, 1.0, 300.0 );
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );
    // position the view point
    gluLookAt( 0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
}




//-----------------------------------------------------------------------------
// Name: help( )
// Desc: print usage
//----------------------------------------------------------------------------- 
void help()
{
    cerr << "------------------------------------------------------------------------" << endl;
    cerr << "Ba-dum-tss" << endl;
    cerr << "Trijeet Mukhopadhyay" << endl;
    cerr << "http://ccrma.stanford.edu/~trijeetm/ba-dum-tss" << endl;
    cerr << "------------------------------------------------------------------------" << endl;
    cerr << "'h' - print this help message" << endl;
    cerr << "'`' - toggle fullscreen (below esc key)" << endl;
    cerr << "'<esc>' - quit visualization" << endl;
    cerr << "'<tab>' - switch instruments" << endl;
    cerr << "left and right arrows to cycle b/w patches/notes" << endl;
    cerr << "or use the following keys to select notes/patches" << endl;
    cerr << "\t'-'\t'='\t" << endl;
    cerr << "\t'p'\t'['\t" << endl;
    cerr << "\t'l'\t';'\t" << endl;
    cerr << "\t','\t'.'\t" << endl;
    cerr << "and place them on the sequencer" << endl;
    cerr << "keys correspond to position in the grid, hitting those toggles notes in that position" << endl;
    cerr << "'<spacebar>' to clear all notes" << endl;
    cerr << "<up-arrow> and <down-arrow> to control tempo" << endl;
    cerr << "------------------------------------------------------------------------" << endl;
    cerr << "PROTIP: if program crashes, run with wi-fi/bluetooth turned off" << endl;
    cerr << "------------------------------------------------------------------------" << endl;
}




//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y )
{
    switch( key )
    {
        case 27: // quit
            exit(1);
        break; 
        case 9: // switch synth
            if (Globals::activeSynth == DM_DRUMS)
                Globals::activeSynth = DM_PIANOS;
            else if (Globals::activeSynth == DM_PIANOS)
                Globals::activeSynth = DM_SYNTH;
            else if (Globals::activeSynth == DM_SYNTH)
                Globals::activeSynth = DM_DRUMS;
        break;
        case 'h': // print help
            help();
            break;

        // case '/': // pause sim
        //     cout << "pausing" << endl;
        //     if (Globals::sim->isPaused())
        //         Globals::sim->resume();
        //     else
        //         Globals::sim->pause();
        //     break;
            
        case '`': // toggle fullscreen
        {
            // check fullscreen
            if( !g_fullscreen )
            {
                g_last_width = g_width;
                g_last_height = g_height;
                glutFullScreen();
            }
            else
                glutReshapeWindow( g_last_width, g_last_height );
            
            // toggle variable value
            g_fullscreen = !g_fullscreen;
        }
        break;
        // drum patch selector
        case ',': 
            Globals::currentPatch = 0;
            Globals::currentPianoPatch = 0;
            Globals::currentSynthPatch = 0;
            Globals::currentNote = 35;
            Globals::currentPianoNote = 24;
            Globals::currentSynthNote = 24 + 24;
        break;
        case '.': 
            Globals::currentPatch = 1;
            Globals::currentPianoPatch = 1;
            Globals::currentSynthPatch = 1;
            Globals::currentNote = 36;
            Globals::currentPianoNote = 27;
            Globals::currentSynthNote = 27 + 24;
        break;
        case 'l': 
            Globals::currentPatch = 2;
            Globals::currentPianoPatch = 2;
            Globals::currentSynthPatch = 2;
            Globals::currentNote = 38;
            Globals::currentPianoNote = 29;
            Globals::currentSynthNote = 29 + 24;
        break;
        case ';': 
            Globals::currentPatch = 3;
            Globals::currentPianoPatch = 3;
            Globals::currentSynthPatch = 3;
            Globals::currentNote = 40;
            Globals::currentPianoNote = 31;
            Globals::currentSynthNote = 31 + 24;
        break;
        case 'p': // mid tom
            Globals::currentPatch = 4;
            Globals::currentPianoPatch = 4;
            Globals::currentSynthPatch = 4;
            Globals::currentNote = 45;
            Globals::currentPianoNote = 34;
            Globals::currentSynthNote = 34 + 24;
        break;
        case '[': // high tom
            Globals::currentPatch = 5;
            Globals::currentPianoPatch = 5;
            Globals::currentSynthPatch = 5;
            Globals::currentNote = 47;
            Globals::currentPianoNote = 36;
            Globals::currentSynthNote = 36 + 24;
        break;
        case '-': 
            Globals::currentPatch = 6;
            Globals::currentPianoPatch = 6;
            Globals::currentSynthPatch = 6;
            Globals::currentNote = 42;
            Globals::currentPianoNote = 39;
            Globals::currentSynthNote = 39 + 24;
        break;
        case '=': 
            Globals::currentPatch = 7;
            Globals::currentPianoPatch = 7;
            Globals::currentSynthPatch = 7;
            Globals::currentNote = 46;
            Globals::currentPianoNote = 41;
            Globals::currentSynthNote = 41 + 24;
        break;
        // grid selector
        // grids 0 - 3
        case '1': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[0].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[0].notes[Globals::currentPatch].on = !Globals::drumGrid[0].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[0].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[0].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[0].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[0].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[0].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[0].notes[Globals::currentSynthPatch].on;
            }
        break;
        case '2': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[1].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[1].notes[Globals::currentPatch].on = !Globals::drumGrid[1].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[1].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[1].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[1].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[1].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[1].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[1].notes[Globals::currentSynthPatch].on;
            }
        break;
        case '3': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[2].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[2].notes[Globals::currentPatch].on = !Globals::drumGrid[2].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[2].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[2].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[2].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[2].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[2].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[2].notes[Globals::currentSynthPatch].on;
            }
        break;
        case '4': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[3].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[3].notes[Globals::currentPatch].on = !Globals::drumGrid[3].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[3].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[3].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[3].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[3].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[3].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[3].notes[Globals::currentSynthPatch].on;
            }
        break;
        // grids 4 - 7
        case 'q': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[4].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[4].notes[Globals::currentPatch].on = !Globals::drumGrid[4].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[4].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[4].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[4].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[4].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[4].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[4].notes[Globals::currentSynthPatch].on;
            }
        break;
        case 'w': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[5].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[5].notes[Globals::currentPatch].on = !Globals::drumGrid[5].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[5].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[5].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[5].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[5].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[5].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[5].notes[Globals::currentSynthPatch].on;
            }
        break;
        case 'e': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[6].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[6].notes[Globals::currentPatch].on = !Globals::drumGrid[6].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[6].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[6].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[6].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[6].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[6].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[6].notes[Globals::currentSynthPatch].on;
            }
        break;
        case 'r': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[7].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[7].notes[Globals::currentPatch].on = !Globals::drumGrid[7].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[7].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[7].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[7].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[7].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[7].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[7].notes[Globals::currentSynthPatch].on;
            }
        break;
        // grids 8 - 11
        case 'a': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[8].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[8].notes[Globals::currentPatch].on = !Globals::drumGrid[8].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[8].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[8].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[8].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[8].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[8].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[8].notes[Globals::currentSynthPatch].on;
            }
        break;
        case 's': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[9].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[9].notes[Globals::currentPatch].on = !Globals::drumGrid[9].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[9].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[9].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[9].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[9].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[9].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[9].notes[Globals::currentSynthPatch].on;
            }
        break;
        case 'd': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[10].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[10].notes[Globals::currentPatch].on = !Globals::drumGrid[10].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[10].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[10].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[10].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[10].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[10].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[10].notes[Globals::currentSynthPatch].on;
            }
        break;
        case 'f': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[11].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[11].notes[Globals::currentPatch].on = !Globals::drumGrid[11].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[11].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[11].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[11].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[11].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[11].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[11].notes[Globals::currentSynthPatch].on;
            }
        break;
        // grids 12 - 15
        case 'z': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[12].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[12].notes[Globals::currentPatch].on = !Globals::drumGrid[12].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[12].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[12].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[12].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[12].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[12].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[12].notes[Globals::currentSynthPatch].on;
            }
        break;
        case 'x': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[13].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[13].notes[Globals::currentPatch].on = !Globals::drumGrid[13].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[13].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[13].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[13].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[13].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[13].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[13].notes[Globals::currentSynthPatch].on;
            }
        break;
        case 'c': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[14].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[14].notes[Globals::currentPatch].on = !Globals::drumGrid[14].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[14].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[14].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[14].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[14].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[14].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[14].notes[Globals::currentSynthPatch].on;
            }
        break;
        case 'v': 
            if (Globals::activeSynth == DM_DRUMS) {
                Globals::drumGrid[15].notes[Globals::currentPatch].pitch = Globals::currentNote;
                Globals::drumGrid[15].notes[Globals::currentPatch].on = !Globals::drumGrid[15].notes[Globals::currentPatch].on;
            }
            else if (Globals::activeSynth == DM_PIANOS) {
                Globals::pianoGrid[15].notes[Globals::currentPianoPatch].pitch = Globals::currentPianoNote;
                Globals::pianoGrid[15].notes[Globals::currentPianoPatch].on = !Globals::pianoGrid[15].notes[Globals::currentPianoPatch].on;
            }
            else if (Globals::activeSynth == DM_SYNTH) {
                Globals::synthGrid[15].notes[Globals::currentSynthPatch].pitch = Globals::currentSynthNote;
                Globals::synthGrid[15].notes[Globals::currentSynthPatch].on = !Globals::synthGrid[15].notes[Globals::currentSynthPatch].on;
            }
        break;
        case ' ': // clear
            if (Globals::activeSynth == DM_DRUMS)
                for (int i = 0; i < DM_N_GRID_ELEMENTS; i++)
                    for (int j = 0; j < DM_POLYPHONY; j++)
                        Globals::drumGrid[i].notes[j].on = false;
            else if (Globals::activeSynth == DM_PIANOS)
                for (int i = 0; i < DM_N_GRID_ELEMENTS; i++)
                    for (int j = 0; j < DM_POLYPHONY; j++)
                        Globals::pianoGrid[i].notes[j].on = false;
            else if (Globals::activeSynth == DM_SYNTH)
                for (int i = 0; i < DM_N_GRID_ELEMENTS; i++)
                    for (int j = 0; j < DM_POLYPHONY; j++)
                        Globals::synthGrid[i].notes[j].on = false;
        break;
    }
    
    // trigger redraw
    glutPostRedisplay( );
}

//-----------------------------------------------------------------------------
// Name: specialFunc( )
// Desc: handles arrow stuff
//-----------------------------------------------------------------------------
void specialFunc( int key, int x, int y )
{
    // check
    bool handled = false;
    
    // if not handled
    if( !handled )
    {
        switch( key )
        {
            case GLUT_KEY_LEFT:
                Globals::currentPatch = (Globals::currentPatch + DM_POLYPHONY - 1) % DM_POLYPHONY;
                Globals::currentNote = g_drumNotes[Globals::currentPatch];
                Globals::currentPianoPatch = (Globals::currentPianoPatch + DM_POLYPHONY - 1) % DM_POLYPHONY;
                Globals::currentPianoNote = g_pianoNotes[Globals::currentPianoPatch];
                Globals::currentSynthPatch = (Globals::currentSynthPatch + DM_POLYPHONY - 1) % DM_POLYPHONY;
                Globals::currentSynthNote = g_synthNotes[Globals::currentSynthPatch];
                break;
            case GLUT_KEY_RIGHT:
                Globals::currentPatch = (Globals::currentPatch + 1) % DM_POLYPHONY;
                Globals::currentNote = g_drumNotes[Globals::currentPatch];
                Globals::currentPianoPatch = (Globals::currentPianoPatch + 1) % DM_POLYPHONY;
                Globals::currentPianoNote = g_pianoNotes[Globals::currentPianoPatch];
                Globals::currentSynthPatch = (Globals::currentSynthPatch + 1) % DM_POLYPHONY;
                Globals::currentSynthNote = g_synthNotes[Globals::currentSynthPatch];
                break;
            case GLUT_KEY_UP:
                if (Globals::bpm >= 400)
                    Globals::bpm == 400;
                else
                    Globals::bpm += 5;
                break;
            case GLUT_KEY_DOWN:
                if (Globals::bpm <= 5) 
                    Globals::bpm = 5;
                else
                    Globals::bpm += -5;
                break;
        }
    }
}


//-----------------------------------------------------------------------------
// Name: mouseFunc( )
// Desc: handles mouse stuff
//-----------------------------------------------------------------------------
void mouseFunc( int button, int state, int x, int y )
{
    if( button == GLUT_LEFT_BUTTON )
    {
        // when left mouse button is down
        if( state == GLUT_DOWN )
        {
        }
        else
        {
        }
    }
    else if ( button == GLUT_RIGHT_BUTTON )
    {
        // when right mouse button down
        if( state == GLUT_DOWN )
        {
        }
        else
        {
        }
    }
    else
    {
    }
    
    glutPostRedisplay( );
}


//-----------------------------------------------------------------------------
// Name: idleFunc( )
// Desc: callback from GLUT
//-----------------------------------------------------------------------------
void idleFunc( )
{
    // render the scene
    glutPostRedisplay( );
}


//-----------------------------------------------------------------------------
// Name: displayFunc( )
// Desc: callback function invoked to draw the client area
//-----------------------------------------------------------------------------
void displayFunc( )
{
    XGfx::getCurrentTime( true );

    // slewTest.update(testGoal);
    // // test.interp(0.1);
    // slewTest.interp(0.1);

    // // // cout << test.value << endl;
    // cout << slewTest.actual().x << endl;

    glClearColor(cream.x, cream.y, cream.z, 1.0);
    
    // clear the color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glColor3f(warmRed.x, warmRed.y, warmRed.z);

    // drum gfx update
        // reveal patchCubes
        for (int i = 0; i < DM_N_GRID_ELEMENTS; i++) {
            for (int j = 0, k = 0; j < DM_POLYPHONY; j++) {
                YCube *cube = g_patchCubes[i * DM_POLYPHONY + j];
                k++;
                // cube->sca.set(1.0, 1.0 / Globals::drumGrid[i].totalPatches, 1);
                // cube->loc.set(gridCoords[i][0], 
                //     gridCoords[i][1] + k, 
                //     gridCoords[i][2]);
                if (Globals::drumGrid[i].notes[j].on) {
                    cube->active = true;
                }
                else {
                    cube->active = false;
                }
            }
        }

        for (int i = 0; i < DM_POLYPHONY; i++) {
            if (i == Globals::currentPatch)
                g_patchSelector[i]->active = true;
            else 
                g_patchSelector[i]->active = false;
        }

        // draw playhead and trail
        // g_playhead->loc = gridCoords[Globals::playhead];
        g_playhead->loc = gridCoords[(Globals::playhead + DM_N_GRID_ELEMENTS - 1) % DM_N_GRID_ELEMENTS];
        g_playheadTrail1->loc = gridCoords[(Globals::playhead + DM_N_GRID_ELEMENTS - 2) % DM_N_GRID_ELEMENTS];

    // piano gfx update
        // reveal patchCubes
        for (int i = 0; i < DM_N_GRID_ELEMENTS; i++) {
            for (int j = 0, k = 0; j < DM_POLYPHONY; j++) {
                YCube *cube = g_patchCubesPiano[i * DM_POLYPHONY + j];
                k++;
                if (Globals::pianoGrid[i].notes[j].on) {
                    cube->active = true;
                }
                else {
                    cube->active = false;
                }
            }
        }

        for (int i = 0; i < DM_POLYPHONY; i++) {
            if (i == Globals::currentPatch)
                g_patchSelectorPiano[i]->active = true;
            else 
                g_patchSelectorPiano[i]->active = false;
        }

        // draw playhead and trail
        // g_playhead->loc = gridCoords[Globals::playhead];
        g_playheadPiano->loc = gridCoords[(Globals::playhead + DM_N_GRID_ELEMENTS - 1) % DM_N_GRID_ELEMENTS];
        g_playheadTrail1Piano->loc = gridCoords[(Globals::playhead + DM_N_GRID_ELEMENTS - 2) % DM_N_GRID_ELEMENTS];

    // synth gfx update
        // reveal patchCubes
        for (int i = 0; i < DM_N_GRID_ELEMENTS; i++) {
            for (int j = 0, k = 0; j < DM_POLYPHONY; j++) {
                YCube *cube = g_patchCubesSynth[i * DM_POLYPHONY + j];
                k++;
                if (Globals::synthGrid[i].notes[j].on) {
                    cube->active = true;
                }
                else {
                    cube->active = false;
                }
            }
        }

        for (int i = 0; i < DM_POLYPHONY; i++) {
            if (i == Globals::currentPatch)
                g_patchSelectorSynth[i]->active = true;
            else 
                g_patchSelectorSynth[i]->active = false;
        }

        // draw playhead and trail
        g_playheadSynth->loc = gridCoords[(Globals::playhead + DM_N_GRID_ELEMENTS - 1) % DM_N_GRID_ELEMENTS];
        g_playheadTrail1Synth->loc = gridCoords[(Globals::playhead + DM_N_GRID_ELEMENTS - 2) % DM_N_GRID_ELEMENTS];

    // switch piano and drums and synth
    if (Globals::activeSynth == DM_PIANOS) {
        for (int i = 0; i < DM_POLYPHONY; i++) {
            g_patchSelector[i]->active = false;
            g_patchSelectorSynth[i]->active = false;
        }
        Vector3D goal;
        goal.set(1, 1, 1);
        pianoSca.update(goal);
        goal.set(0, 0, 0);
        pianoLoc.update(goal);
        goal.set(0.25, 1, 1);
        drumSca.update(goal);
        goal.set(-5.5, 0, -30);
        drumLoc.update(goal);
        goal.set(0.25, 1, 1);
        synthSca.update(goal);
        goal.set(5.5, 0, -30);
        synthLoc.update(goal);
    }
    else if (Globals::activeSynth == DM_DRUMS) {
        for (int i = 0; i < DM_POLYPHONY; i++) {
            g_patchSelectorPiano[i]->active = false;
            g_patchSelectorSynth[i]->active = false;
        }
        Vector3D goal;
        goal.set(0.25, 1, 1);
        pianoSca.update(goal);
        goal.set(5.5, 0, -30);
        pianoLoc.update(goal);
        goal.set(1, 1, 1);
        drumSca.update(goal);
        goal.set(0, 0, 0);
        drumLoc.update(goal);
        goal.set(0.2, 1, 1);
        synthSca.update(goal);
        goal.set(9, 0, -50);
        synthLoc.update(goal);
    }
    else if (Globals::activeSynth == DM_SYNTH) {
        for (int i = 0; i < DM_POLYPHONY; i++) {
            g_patchSelectorPiano[i]->active = false;
            g_patchSelector[i]->active = false;
        }
        Vector3D goal;
        goal.set(0.25, 1, 1);
        pianoSca.update(goal);
        goal.set(-5.5, 0, -30);
        pianoLoc.update(goal);
        goal.set(0.2, 1, 1);
        drumSca.update(goal);
        goal.set(-9, 0, -50);
        drumLoc.update(goal);
        goal.set(1, 1, 1);
        synthSca.update(goal);
        goal.set(0, 0, 0);
        synthLoc.update(goal);
    }
    pianoSca.interp2(0.25);
    pianoLoc.interp2(0.25);
    drumSca.interp2(0.25);
    drumLoc.interp2(0.25);
    synthSca.interp2(0.25);
    synthLoc.interp2(0.25);
    g_pianoGfx->sca = pianoSca.actual();
    g_pianoGfx->loc = pianoLoc.actual();
    g_drumGfx->sca = drumSca.actual();
    g_drumGfx->loc = drumLoc.actual();
    g_synthGfx->sca = synthSca.actual();
    g_synthGfx->loc = synthLoc.actual();

    g_tempo->set("Tempo = " + to_string(Globals::bpm));

    // cascade simulation
    Globals::sim->systemCascade();

    // flush!
    glFlush( );
    // swap the double buffer
    glutSwapBuffers( );
}

//-----------------------------------------------------------------------------
// name: dm_gfx_loop( )
// desc: hand off to graphics loop
//-----------------------------------------------------------------------------
void dm_gfx_loop()
{
    // let GLUT handle the current thread from here
    glutMainLoop();
}