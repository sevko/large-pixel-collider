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
    struct sym_light {
      SYMTAB *p;
      double c[4];
    } light;
    struct sym_ambient {
      double c[4];
    } ambient;
    struct sym_constants {
      SYMTAB *p;
      /* each triple holds ka kd and ks for red green and blue
	 respectively */
    } constants;
    struct sym_save_coordinate_system {
      SYMTAB *p;
    } save_coordinate_system;
    struct sym_camera {
      double eye[4],aim[4];
    } camera;
    struct  sym_sphere {
      SYMTAB *constants;
      double d[4];
      double r;
      SYMTAB *cs;
    } sphere;
    struct sym_texture {
      SYMTAB *constants;
      double d0[3];
      double d1[3];
      double d2[3];
      double d3[3];
      SYMTAB *p;
      SYMTAB *cs;
    } texture;
    struct sym_torus {
      SYMTAB *constants;
      double d[4];
      double r0,r1;
      SYMTAB *cs;
    } torus;
    struct sym_box {
      SYMTAB *constants;
      double d0[4],d1[4];
      SYMTAB *cs;
    } box;
    struct sym_line {
      SYMTAB *constants;
      double p0[4],p1[4];
      SYMTAB *cs0,*cs1;
    } line;
    struct sym_mesh {
      SYMTAB *constants;
      char name[255];
      SYMTAB *cs; 
    } mesh;
    struct sym_set {
      SYMTAB *p;
      double val;
    } set;
    struct sym_move {
      double d[4];
      SYMTAB *p;
    } move;
    struct sym_scale {
      double d[4];
      SYMTAB *p;
    } scale;
    struct sym_rotate {
      double axis;
      double degrees;
      SYMTAB *p;
    } rotate;
    struct sym_basename {
      SYMTAB *p;
    } basename;
    struct sym_save_knobs {
      SYMTAB *p;
    } save_knobs;
    struct sym_tween {
      double start_frame, end_frame;
      SYMTAB *knob_list0;
      SYMTAB *knob_list1;
    } tween;
    struct sym_frames {
      double num_frames;
    }frames;
    struct sym_vary {
      SYMTAB *p;
      double start_frame, end_frame, start_val, end_val;
    } vary;
    struct sym_save {
      SYMTAB *p;
    } save;
    struct sym_shading {
      SYMTAB *p;
    } shading;
    struct sym_setknobs {
      double value;
    } setknobs;
    struct sym_focal {
      double value;
    } focal;
  } op;
};

extern struct command op[MAX_COMMANDS];
typedef struct command Command_t;

void print_pcode();
