%{
  /* C declarations */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "src/graphics/matrix.h"
#include "misc_headers.h"

  SYMTAB *s;
  struct light *l;
  struct constants *c;
  struct command op[MAX_COMMANDS];
  Matrix_t *m;
  int lastop=0;
  int lineno=0;

#define YYERROR_VERBOSE 1

  %}


/* Bison Declarations */

%union{
  double val;
  char string[255];

}

%token COMMENT
%token <val> DOUBLE
%token <string> LIGHT AMBIENT
%token <string> CONSTANTS SAVE_COORDS CAMERA 
%token <string> SPHERE TORUS BOX LINE CS MESH TEXTURE
%token <string> STRING
%token <string> SET MOVE SCALE ROTATE BASENAME SAVE_KNOBS TWEEN FRAMES VARY 
%token <string> PUSH POP SAVE GENERATE_RAYFILES
%token <string> SHADING SHADING_TYPE SETKNOBS FOCAL DISPLAY WEB
%token <string> CO
%%
/* Grammar rules */

input:
| input command
;

command: 
COMMENT {}|

LIGHT STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  l = (struct light *)malloc(sizeof(struct light));
  l->l[0]= $3;
  l->l[1]= $4;
  l->l[2]= $5;
  l->l[3]= 0;
  l->c[0]= $6;
  l->c[1]= $7;
  l->c[2]= $8;
  op[lastop].opcode=LIGHT;
  op[lastop].op.light.c[0] = $6; 
  op[lastop].op.light.c[1] = $7;
  op[lastop].op.light.c[2] = $8;
  op[lastop].op.light.c[3] = 0;
  op[lastop].op.light.p = add_symbol($2,SYM_LIGHT,l);
  lastop++;
}|

MOVE DOUBLE DOUBLE DOUBLE STRING
{ 
  lineno++;
  op[lastop].opcode = MOVE;
  op[lastop].op.move.d[0] = $2;
  op[lastop].op.move.d[1] = $3;
  op[lastop].op.move.d[2] = $4;
  op[lastop].op.move.d[3] = 0;
  op[lastop].op.move.p = add_symbol($5,SYM_VALUE,0);
  lastop++;
}
|
MOVE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = MOVE;
  op[lastop].op.move.d[0] = $2;
  op[lastop].op.move.d[1] = $3;
  op[lastop].op.move.d[2] = $4;
  op[lastop].op.move.d[3] = 0;
  op[lastop].op.move.p = NULL;
  lastop++;
}|

CONSTANTS STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  c = (struct constants *)malloc(sizeof(struct constants));
  c->r[0]=$3;
  c->r[1]=$4;
  c->r[2]=$5;
  c->r[3]=0;

  c->g[0]=$6;
  c->g[1]=$7;
  c->g[2]=$8;
  c->g[3]=0;

  c->b[0]=$9;
  c->b[1]=$10;
  c->b[2]=$11;
  c->b[3]=0;

  c->red = 0;
  c->green = 0;
  c->blue = 0;

  op[lastop].op.constants.p =  add_symbol($2,SYM_CONSTANTS,c);
  op[lastop].opcode=CONSTANTS;
  lastop++;
}|

CONSTANTS STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  c = (struct constants *)malloc(sizeof(struct constants));
  c->r[0]=$3;
  c->r[1]=$4;
  c->r[2]=$5;
  c->r[3]=0;

  c->g[0]=$6;
  c->g[1]=$7;
  c->g[2]=$8;
  c->g[3]=0;

  c->b[0]=$9;
  c->b[1]=$10;
  c->b[2]=$11;
  c->b[3]=0;

  c->red = $12;
  c->green = $13;
  c->blue = $14;
  op[lastop].op.constants.p =  add_symbol($2,SYM_CONSTANTS,c);
  op[lastop].opcode=CONSTANTS;
  lastop++;
}|

SAVE_COORDS STRING
{
  lineno++;
  op[lastop].opcode = SAVE_COORDS;
  m = (Matrix_t *)createMatrix();
  op[lastop].op.save_coordinate_system.p = add_symbol($2,SYM_MATRIX,m);
  lastop++;
}|

CAMERA DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = CAMERA;
  op[lastop].op.camera.eye[0] = $2;
  op[lastop].op.camera.eye[1] = $3;
  op[lastop].op.camera.eye[2] = $4;
  op[lastop].op.camera.eye[3] = 0;
  op[lastop].op.camera.aim[0] = $5;
  op[lastop].op.camera.aim[1] = $6;
  op[lastop].op.camera.aim[2] = $7;
  op[lastop].op.camera.aim[3] = 0;
  lastop++;
}|
TEXTURE STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = TEXTURE;
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.texture.d0[0] = $3;
  op[lastop].op.texture.d0[1] = $4;
  op[lastop].op.texture.d0[2] = $5;
  op[lastop].op.texture.d1[0] = $6;
  op[lastop].op.texture.d1[1] = $7;
  op[lastop].op.texture.d1[2] = $8;
  op[lastop].op.texture.d2[0] = $9;
  op[lastop].op.texture.d2[1] = $10;
  op[lastop].op.texture.d2[2] = $11;
  op[lastop].op.texture.d3[0] = $12;
  op[lastop].op.texture.d3[1] = $13;
  op[lastop].op.texture.d3[2] = $14;
  op[lastop].op.texture.cs = NULL;
  op[lastop].op.texture.constants =  add_symbol("",SYM_CONSTANTS,c);
  op[lastop].op.texture.p = add_symbol($2,SYM_FILE,0);
  lastop++;
}|
SPHERE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = SPHERE;
  op[lastop].op.sphere.d[0] = $2;
  op[lastop].op.sphere.d[1] = $3;
  op[lastop].op.sphere.d[2] = $4;
  op[lastop].op.sphere.d[3] = 0;
  op[lastop].op.sphere.r = $5;
  op[lastop].op.sphere.constants = NULL;
  op[lastop].op.sphere.cs = NULL;
  lastop++;
}|
SPHERE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = SPHERE;
  op[lastop].op.sphere.d[0] = $2;
  op[lastop].op.sphere.d[1] = $3;
  op[lastop].op.sphere.d[2] = $4;
  op[lastop].op.sphere.d[3] = 0;
  op[lastop].op.sphere.r = $5;
  op[lastop].op.sphere.constants = NULL;
  m = (Matrix_t *)createMatrix();
  op[lastop].op.sphere.cs = add_symbol($6,SYM_MATRIX,m);
  lastop++;
}|
SPHERE STRING DOUBLE DOUBLE DOUBLE DOUBLE 
{
  lineno++;
  op[lastop].opcode = SPHERE;
  op[lastop].op.sphere.d[0] = $3;
  op[lastop].op.sphere.d[1] = $4;
  op[lastop].op.sphere.d[2] = $5;
  op[lastop].op.sphere.d[3] = 0;
  op[lastop].op.sphere.r = $6;
  op[lastop].op.sphere.cs = NULL;
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.sphere.constants = add_symbol($2,SYM_CONSTANTS,c);
  lastop++;
}|
SPHERE STRING DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = SPHERE;
  op[lastop].op.sphere.d[0] = $3;
  op[lastop].op.sphere.d[1] = $4;
  op[lastop].op.sphere.d[2] = $5;
  op[lastop].op.sphere.d[3] = 0;
  op[lastop].op.sphere.r = $6;
  op[lastop].op.sphere.constants = NULL;
  m = (Matrix_t *)createMatrix();
  op[lastop].op.sphere.cs = add_symbol($7,SYM_MATRIX,m);
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.sphere.constants = add_symbol($2,SYM_CONSTANTS,c);
  lastop++;
}| 

TORUS DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = TORUS;
  op[lastop].op.torus.d[0] = $2;
  op[lastop].op.torus.d[1] = $3;
  op[lastop].op.torus.d[2] = $4;
  op[lastop].op.torus.d[3] = 0;
  op[lastop].op.torus.r0 = $5;
  op[lastop].op.torus.r1 = $6;
  op[lastop].op.torus.constants = NULL;
  op[lastop].op.torus.cs = NULL;

  lastop++;
}|
TORUS DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = TORUS;
  op[lastop].op.torus.d[0] = $2;
  op[lastop].op.torus.d[1] = $3;
  op[lastop].op.torus.d[2] = $4;
  op[lastop].op.torus.d[3] = 0;
  op[lastop].op.torus.r0 = $5;
  op[lastop].op.torus.r1 = $6;
  op[lastop].op.torus.constants = NULL;
  m = (Matrix_t *)createMatrix();
  op[lastop].op.torus.cs = add_symbol($7,SYM_MATRIX,m);
  lastop++;
}|
TORUS STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = TORUS;
  op[lastop].op.torus.d[0] = $3;
  op[lastop].op.torus.d[1] = $4;
  op[lastop].op.torus.d[2] = $5;
  op[lastop].op.torus.d[3] = 0;
  op[lastop].op.torus.r0 = $6;
  op[lastop].op.torus.r1 = $7;
  op[lastop].op.torus.cs = NULL;
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.torus.constants = add_symbol($2,SYM_CONSTANTS,c);

  lastop++;
}|
TORUS STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = TORUS;
  op[lastop].op.torus.d[0] = $3;
  op[lastop].op.torus.d[1] = $4;
  op[lastop].op.torus.d[2] = $5;
  op[lastop].op.torus.d[3] = 0;
  op[lastop].op.torus.r0 = $6;
  op[lastop].op.torus.r1 = $7;
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.torus.constants = add_symbol($2,SYM_CONSTANTS,c);
  m = (Matrix_t *)createMatrix();
  op[lastop].op.torus.cs = add_symbol($8,SYM_MATRIX,m);

  lastop++;
}| 
BOX DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = BOX;
  op[lastop].op.box.d0[0] = $2;
  op[lastop].op.box.d0[1] = $3;
  op[lastop].op.box.d0[2] = $4;
  op[lastop].op.box.d0[3] = 0;
  op[lastop].op.box.d1[0] = $5;
  op[lastop].op.box.d1[1] = $6;
  op[lastop].op.box.d1[2] = $7;
  op[lastop].op.box.d1[3] = 0;

  op[lastop].op.box.constants = NULL;
  op[lastop].op.box.cs = NULL;
  lastop++;
}|
BOX DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = BOX;
  op[lastop].op.box.d0[0] = $2;
  op[lastop].op.box.d0[1] = $3;
  op[lastop].op.box.d0[2] = $4;
  op[lastop].op.box.d0[3] = 0;
  op[lastop].op.box.d1[0] = $5;
  op[lastop].op.box.d1[1] = $6;
  op[lastop].op.box.d1[2] = $7;
  op[lastop].op.box.d1[3] = 0;

  op[lastop].op.box.constants = NULL;
  m = (Matrix_t *)createMatrix();
  op[lastop].op.box.cs = add_symbol($8,SYM_MATRIX,m);
  lastop++;
}|
BOX STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = BOX;
  op[lastop].op.box.d0[0] = $3;
  op[lastop].op.box.d0[1] = $4;
  op[lastop].op.box.d0[2] = $5;
  op[lastop].op.box.d0[3] = 0;
  op[lastop].op.box.d1[0] = $6;
  op[lastop].op.box.d1[1] = $7;
  op[lastop].op.box.d1[2] = $8;
  op[lastop].op.box.d1[3] = 0;
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.box.constants = add_symbol($2,SYM_CONSTANTS,c);
  op[lastop].op.box.cs = NULL;
  lastop++;
}|
BOX STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = BOX;
  op[lastop].op.box.d0[0] = $3;
  op[lastop].op.box.d0[1] = $4;
  op[lastop].op.box.d0[2] = $5;
  op[lastop].op.box.d0[3] = 0;
  op[lastop].op.box.d1[0] = $6;
  op[lastop].op.box.d1[1] = $7;
  op[lastop].op.box.d1[2] = $8;
  op[lastop].op.box.d1[3] = 0;
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.box.constants = add_symbol($2,SYM_CONSTANTS,c);
  m = (Matrix_t *)createMatrix();
  op[lastop].op.box.cs = add_symbol($9,SYM_MATRIX,m);

  lastop++;
}|


LINE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = LINE;
  op[lastop].op.line.p0[0] = $2;
  op[lastop].op.line.p0[1] = $3;
  op[lastop].op.line.p0[2] = $4;
  op[lastop].op.line.p0[3] = 0;
  op[lastop].op.line.p1[0] = $5;
  op[lastop].op.line.p1[1] = $6;
  op[lastop].op.line.p1[2] = $7;
  op[lastop].op.line.p1[3] = 0;
  op[lastop].op.line.constants = NULL;
  op[lastop].op.line.cs0 = NULL;
  op[lastop].op.line.cs1 = NULL;
  lastop++;
}|
/* first do cs0, then cs1, then both - BUT NO CONSTANTS */
LINE DOUBLE DOUBLE DOUBLE STRING DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = LINE;
  op[lastop].op.line.p0[0] = $2;
  op[lastop].op.line.p0[1] = $3;
  op[lastop].op.line.p0[2] = $4;
  op[lastop].op.line.p0[3] = 0;
  op[lastop].op.line.p1[0] = $6;
  op[lastop].op.line.p1[1] = $7;
  op[lastop].op.line.p1[2] = $8;
  op[lastop].op.line.p1[3] = 0;
  op[lastop].op.line.constants = NULL;
  m = (Matrix_t *)createMatrix();
  op[lastop].op.line.cs0 = add_symbol($5,SYM_MATRIX,m);
  op[lastop].op.line.cs1 = NULL;
  lastop++;
}|
LINE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = LINE;
  op[lastop].op.line.p0[0] = $2;
  op[lastop].op.line.p0[1] = $3;
  op[lastop].op.line.p0[2] = $4;
  op[lastop].op.line.p0[3] = 0;
  op[lastop].op.line.p1[0] = $5;
  op[lastop].op.line.p1[1] = $6;
  op[lastop].op.line.p1[2] = $7;
  op[lastop].op.line.p1[3] = 0;
  op[lastop].op.line.constants = NULL;
  op[lastop].op.line.cs0 = NULL;
  m = (Matrix_t *)createMatrix();
  op[lastop].op.line.cs1 = add_symbol($8,SYM_MATRIX,m);
  lastop++;
}|
LINE DOUBLE DOUBLE DOUBLE STRING DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = LINE;
  op[lastop].op.line.p0[0] = $2;
  op[lastop].op.line.p0[1] = $3;
  op[lastop].op.line.p0[2] = $4;
  op[lastop].op.line.p0[3] = 0;
  op[lastop].op.line.p1[0] = $6;
  op[lastop].op.line.p1[1] = $7;
  op[lastop].op.line.p1[2] = $8;
  op[lastop].op.line.p1[3] = 0;
  op[lastop].op.line.constants = NULL;
  m = (Matrix_t *)createMatrix();
  op[lastop].op.line.cs0 = add_symbol($5,SYM_MATRIX,m);
  m = (Matrix_t *)createMatrix();
  op[lastop].op.line.cs1 = add_symbol($9,SYM_MATRIX,m);
  lastop++;
}|
/* now do constants, and constants with the cs stuff */
LINE STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = LINE;
  op[lastop].op.line.p0[0] = $3;
  op[lastop].op.line.p0[1] = $4;
  op[lastop].op.line.p0[2] = $5;
  op[lastop].op.line.p0[3] = 0;
  op[lastop].op.line.p1[0] = $6;
  op[lastop].op.line.p1[1] = $7;
  op[lastop].op.line.p1[2] = $8;
  op[lastop].op.line.p1[3] = 0;
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.line.constants = add_symbol($2,SYM_CONSTANTS,c);
  op[lastop].op.line.cs0 = NULL;
  op[lastop].op.line.cs1 = NULL;
  lastop++;
}|
LINE STRING DOUBLE DOUBLE DOUBLE STRING DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = LINE;
  op[lastop].op.line.p0[0] = $3;
  op[lastop].op.line.p0[1] = $4;
  op[lastop].op.line.p0[2] = $5;
  op[lastop].op.line.p0[3] = 0;
  op[lastop].op.line.p1[0] = $7;
  op[lastop].op.line.p1[1] = $8;
  op[lastop].op.line.p1[2] = $9;
  op[lastop].op.line.p1[3] = 0;
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.line.constants = add_symbol($2,SYM_CONSTANTS,c);
  m = (Matrix_t *)createMatrix();
  op[lastop].op.line.cs0 = add_symbol($6,SYM_MATRIX,m);
  op[lastop].op.line.cs1 = NULL;
  lastop++;
}|
LINE STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = LINE;
  op[lastop].op.line.p0[0] = $3;
  op[lastop].op.line.p0[1] = $4;
  op[lastop].op.line.p0[2] = $5;
  op[lastop].op.line.p0[3] = 0;
  op[lastop].op.line.p1[0] = $6;
  op[lastop].op.line.p1[1] = $7;
  op[lastop].op.line.p1[2] = $8;
  op[lastop].op.line.p1[3] = 0;
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.line.constants = add_symbol($2,SYM_CONSTANTS,c);
  op[lastop].op.line.cs0 = NULL;
  m = (Matrix_t *)createMatrix();
  op[lastop].op.line.cs1 = add_symbol($9,SYM_MATRIX,m);
  op[lastop].op.line.cs0 = NULL;
  lastop++;
}|
LINE STRING DOUBLE DOUBLE DOUBLE STRING DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = LINE;
  op[lastop].op.line.p0[0] = $3;
  op[lastop].op.line.p0[1] = $4;
  op[lastop].op.line.p0[2] = $5;
  op[lastop].op.line.p0[3] = 0;
  op[lastop].op.line.p1[0] = $7;
  op[lastop].op.line.p1[1] = $8;
  op[lastop].op.line.p1[2] = $9;
  op[lastop].op.line.p1[3] = 0;
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.line.constants = add_symbol($2,SYM_CONSTANTS,c);
  m = (Matrix_t *)createMatrix();
  op[lastop].op.line.cs0 = add_symbol($6,SYM_MATRIX,m);
  m = (Matrix_t *)createMatrix();
  op[lastop].op.line.cs1 = add_symbol($10,SYM_MATRIX,m);
  lastop++;
}|
MESH CO STRING
{
  lineno++;
  op[lastop].opcode = MESH;
  strcpy(op[lastop].op.mesh.name,$3);
  op[lastop].op.mesh.constants = NULL;
  op[lastop].op.mesh.cs = NULL;
  lastop++;
}|
MESH STRING CO STRING
{ /* name and constants */
  lineno++;
  op[lastop].opcode = MESH;
  strcpy(op[lastop].op.mesh.name,$4);
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.mesh.constants = add_symbol($2,SYM_CONSTANTS,c);
  op[lastop].op.mesh.cs = NULL;
  lastop++;
} |
MESH STRING CO STRING STRING
{
  lineno++;
  op[lastop].opcode = MESH;
  strcpy(op[lastop].op.mesh.name,$4);
  c = (struct constants *)malloc(sizeof(struct constants));
  op[lastop].op.mesh.constants = add_symbol($2,SYM_CONSTANTS,c);
  m = (Matrix_t *)createMatrix();
  op[lastop].op.mesh.cs = add_symbol($5,SYM_MATRIX,m);
  lastop++;
} |
SET STRING DOUBLE
{
  lineno++;
  op[lastop].opcode = SET;
  op[lastop].op.set.p = add_symbol($2,SYM_VALUE,0);
  set_value(op[lastop].op.set.p,$3);
  op[lastop].op.set.val = $3;
  lastop++;
}|
SCALE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = SCALE;
  op[lastop].op.scale.d[0] = $2;
  op[lastop].op.scale.d[1] = $3;
  op[lastop].op.scale.d[2] = $4;
  op[lastop].op.scale.d[3] = 0;
  op[lastop].op.scale.p = add_symbol($5,SYM_VALUE,0);
  lastop++;
}|
SCALE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = SCALE;
  op[lastop].op.scale.d[0] = $2;
  op[lastop].op.scale.d[1] = $3;
  op[lastop].op.scale.d[2] = $4;
  op[lastop].op.scale.d[3] = 0;
  op[lastop].op.scale.p = NULL;
  lastop++;
}|
ROTATE STRING DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = ROTATE;
  switch (*$2)
    {
    case 'x':
    case 'X': 
      op[lastop].op.rotate.axis = 0;
      break;
    case 'y':
    case 'Y': 
      op[lastop].op.rotate.axis = 1;
      break;
    case 'z':
    case 'Z': 
      op[lastop].op.rotate.axis = 2;
      break;
    }

  op[lastop].op.rotate.degrees = $3;
  op[lastop].op.rotate.p = add_symbol($4,SYM_VALUE,0);
  
  lastop++;
}|
ROTATE STRING DOUBLE
{
  lineno++;
  op[lastop].opcode = ROTATE;
  switch (*$2)
    {
    case 'x':
    case 'X': 
      op[lastop].op.rotate.axis = 0;
      break;
    case 'y':
    case 'Y': 
      op[lastop].op.rotate.axis = 1;
      break;
    case 'z':
    case 'Z': 
      op[lastop].op.rotate.axis = 2;
      break;
    }
  op[lastop].op.rotate.degrees = $3;
  op[lastop].op.rotate.p = NULL;
  lastop++;
}|
BASENAME STRING
{
  lineno++;
  op[lastop].opcode = BASENAME;
  op[lastop].op.basename.p = add_symbol($2,SYM_STRING,0);
  lastop++;
}|
SAVE_KNOBS STRING
{
  lineno++;
  op[lastop].opcode = SAVE_KNOBS;
  op[lastop].op.save_knobs.p = add_symbol($2,SYM_STRING,0);
  lastop++;
}|
TWEEN DOUBLE DOUBLE STRING STRING
{
  lineno++;
  op[lastop].opcode = TWEEN;
  op[lastop].op.tween.start_frame = $2;
  op[lastop].op.tween.end_frame = $3;
  op[lastop].op.tween.knob_list0 = add_symbol($4,SYM_STRING,0);
  op[lastop].op.tween.knob_list1 = add_symbol($5,SYM_STRING,0);
  lastop++;
}|
FRAMES DOUBLE
{
  lineno++;
  op[lastop].opcode = FRAMES;
  op[lastop].op.frames.num_frames = $2;
  lastop++;
}|
VARY STRING DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = VARY;
  op[lastop].op.vary.p = add_symbol($2,SYM_STRING,0);
  op[lastop].op.vary.start_frame = $3;
  op[lastop].op.vary.end_frame = $4;
  op[lastop].op.vary.start_val = $5;
  op[lastop].op.vary.end_val = $6;
  lastop++;
}|
PUSH
{
  lineno++;
  op[lastop].opcode = PUSH;
  lastop++;
}|
GENERATE_RAYFILES
{
  lineno++;
  op[lastop].opcode = GENERATE_RAYFILES;
  lastop++;
}|
POP
{
  lineno++;
  op[lastop].opcode = POP;
  lastop++;
}|
SAVE STRING
{
  lineno++;
  op[lastop].opcode = SAVE;
  op[lastop].op.save.p = add_symbol($2,SYM_FILE,0);
  lastop++;
}|
SHADING SHADING_TYPE
{
  lineno++;
  op[lastop].opcode = SHADING;
  op[lastop].op.shading.p = add_symbol($2,SYM_STRING,0);
  lastop++;
}|
SETKNOBS DOUBLE
{
  lineno++;
  op[lastop].opcode = SETKNOBS;
  op[lastop].op.setknobs.value = $2;
  lastop++;
}|
FOCAL DOUBLE
{
  lineno++;
  op[lastop].opcode = FOCAL;
  op[lastop].op.focal.value = $2;
  lastop++;
}|
DISPLAY
{
  lineno++;
  op[lastop].opcode = DISPLAY;
  lastop++;
}|
WEB
{
  lineno++;
  op[lastop].opcode = WEB;
  lastop++;
}|
AMBIENT DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = AMBIENT;
  op[lastop].op.ambient.c[0] = $2;
  op[lastop].op.ambient.c[1] = $3;
  op[lastop].op.ambient.c[2] = $4;
  lastop++;
};





%%

/* Other C stuff */



int yyerror(char *s)
{
  printf("Error in line %d:%s\n",lineno,s);
  return 0;
}

int yywrap()
{
  return 1;
}


extern FILE *yyin;


int main(int argc, char **argv)
{
  int i;
  i = 1;

  if (argc > 1)
    {
      yyin = fopen(argv[1],"r");
      if ( argc == 3 && strncmp(argv[2], "-l", 2) == 0) {
	printf("lines");
	i = 0;
      }
    }
  yyparse();
  //COMMENT OUT PRINT_PCODE AND UNCOMMENT
  //MY_MAIN IN ORDER TO RUN YOUR CODE
  print_pcode();
  engineDriver();
  //my_main();

  return 0;    
}
