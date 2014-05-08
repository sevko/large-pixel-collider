#pragma once

#include "symtab.h"
#include "src/graphics/matrix.h"

#define MAX_COMMANDS 512

extern int lastop;

#define Ka 0
#define Kd 1
#define Ks 2

struct command
{
  int opcode;
  union
  {
    struct symLight {
      SYMTAB *p;
      double c[4];
    } light;
    struct symAmbient {
      double c[4];
    } ambient;
    struct symConstants {
      SYMTAB *p;
      /* each triple holds ka kd and ks for red green and blue
	 respectively */
    } constants;
    struct symSave_coordinate_system {
      SYMTAB *p;
    } save_coordinate_system;
    struct symCamera {
      double eye[4],aim[4];
    } camera;
    struct  symSphere {
      SYMTAB *constants;
      double d[4];
      double r;
      SYMTAB *cs;
    } sphere;
    struct symTexture {
      SYMTAB *constants;
      double d0[3];
      double d1[3];
      double d2[3];
      double d3[3];
      SYMTAB *p;
      SYMTAB *cs;
    } texture;
    struct symTorus {
      SYMTAB *constants;
      double d[4];
      double r0,r1;
      SYMTAB *cs;
    } torus;
    struct symBox {
      SYMTAB *constants;
      double d0[4],d1[4];
      SYMTAB *cs;
    } box;
    struct symLine {
      SYMTAB *constants;
      double p0[4],p1[4];
      SYMTAB *cs0,*cs1;
    } line;
    struct symMesh {
      SYMTAB *constants;
      char name[255];
      SYMTAB *cs; 
    } mesh;
    struct symSet {
      SYMTAB *p;
      double val;
    } set;
    struct symMove {
      double d[4];
      SYMTAB *p;
    } move;
    struct symScale {
      double d[4];
      SYMTAB *p;
    } scale;
    struct symRotate {
      double axis;
      double degrees;
      SYMTAB *p;
    } rotate;
    struct symBasename {
      SYMTAB *p;
    } basename;
    struct symSave_knobs {
      SYMTAB *p;
    } save_knobs;
    struct symTween {
      double start_frame, end_frame;
      SYMTAB *knob_list0;
      SYMTAB *knob_list1;
    } tween;
    struct symFrames {
      double num_frames;
    }frames;
    struct symVary {
      SYMTAB *p;
      double start_frame, end_frame, start_val, end_val;
    } vary;
    struct symSave {
      SYMTAB *p;
    } save;
    struct symShading {
      SYMTAB *p;
    } shading;
    struct symSetknobs {
      double value;
    } setknobs;
    struct symFocal {
      double value;
    } focal;
  } op;
};

extern struct command op[MAX_COMMANDS];
typedef struct command Command_t;

void print_pcode();
