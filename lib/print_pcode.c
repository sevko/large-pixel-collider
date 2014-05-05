#include <stdio.h>

#include "parser.h"
#include "symtab.h"
#include "y.tab.h"



void print_pcode()
{
  int i;
  for (i=0;i<lastop;i++)
    {
      printf("%d: ",i);
      switch (op[i].opcode)
	{
	case LIGHT:
	  printf("Light: %s at: %6.2f %6.2f %6.2f",
		 op[i].op.light.p->name,
		 op[i].op.light.c[0], op[i].op.light.c[1],
		 op[i].op.light.c[2]);
	  break;
	case AMBIENT:
	  printf("Ambient: %6.2f %6.2f %6.2f",
		 op[i].op.ambient.c[0],
		 op[i].op.ambient.c[1],
		 op[i].op.ambient.c[2]);
	  break;

	case CONSTANTS:
	  printf("Constants: %s",op[i].op.constants.p->name);
	  break;
	case SAVE_COORDS:
	  printf("Save Coords: %s",op[i].op.save_coordinate_system.p->name);
	  break;
	case CAMERA:
	  printf("Camera: eye: %6.2f %6.2f %6.2f\taim: %6.2f %6.2f %6.2f",
		 op[i].op.camera.eye[0], op[i].op.camera.eye[1],
		 op[i].op.camera.eye[2],
		 op[i].op.camera.aim[0], op[i].op.camera.aim[1],
		 op[i].op.camera.aim[2]);

	  break;
	case SPHERE:
	  printf("Sphere: %6.2f %6.2f %6.2f r=%6.2f",
		 op[i].op.sphere.d[0],op[i].op.sphere.d[1],
		 op[i].op.sphere.d[2],
		 op[i].op.sphere.r);
	  if (op[i].op.sphere.constants != NULL)
	    {
	      printf("\tconstants: %s",op[i].op.sphere.constants->name);
	    }
	  if (op[i].op.sphere.cs != NULL)
	    {
	      printf("\tcs: %s",op[i].op.sphere.cs->name);
	    }

	  break;
	case TORUS:
	  printf("Torus: %6.2f %6.2f %6.2f r0=%6.2f r1=%6.2f",
		 op[i].op.torus.d[0],op[i].op.torus.d[1],
		 op[i].op.torus.d[2],
		 op[i].op.torus.r0,op[i].op.torus.r1);
	  if (op[i].op.torus.constants != NULL)
	    {
	      printf("\tconstants: %s",op[i].op.torus.constants->name);
	    }
	  if (op[i].op.torus.cs != NULL)
	    {
	      printf("\tcs: %s",op[i].op.torus.cs->name);
	    }

	  break;
	case BOX:
	  printf("Box: d0: %6.2f %6.2f %6.2f d1: %6.2f %6.2f %6.2f",
		 op[i].op.box.d0[0],op[i].op.box.d0[1],
		 op[i].op.box.d0[2],
		 op[i].op.box.d1[0],op[i].op.box.d1[1],
		 op[i].op.box.d1[2]);
	  if (op[i].op.box.constants != NULL)
	    {
	      printf("\tconstants: %s",op[i].op.box.constants->name);
	    }
	  if (op[i].op.box.cs != NULL)
	    {
	      printf("\tcs: %s",op[i].op.box.cs->name);
	    }

	  break;
	case LINE:
	  printf("Line: from: %6.2f %6.2f %6.2f to: %6.2f %6.2f %6.2f",
		 op[i].op.line.p0[0],op[i].op.line.p0[1],
		 op[i].op.line.p0[1],
		 op[i].op.line.p1[0],op[i].op.line.p1[1],
		 op[i].op.line.p1[1]);
	  if (op[i].op.line.constants != NULL)
	    {
	      printf("\n\tConstants: %s",op[i].op.line.constants->name);
	    }
	  if (op[i].op.line.cs0 != NULL)
	    {
	      printf("\n\tCS0: %s",op[i].op.line.cs0->name);
	    }
	  if (op[i].op.line.cs1 != NULL)
	    {
	      printf("\n\tCS1: %s",op[i].op.line.cs1->name);
	    }
	  break;
	case MESH:
	  printf("Mesh: filename: %s",op[i].op.mesh.name);
	  if (op[i].op.mesh.constants != NULL)
	    {
	      printf("\tconstants: %s",op[i].op.mesh.constants->name);
	    }
	  break;
	case SET:
	  printf("Set: %s %6.2f",
		 op[i].op.set.p->name,
		 op[i].op.set.p->s.value);
	  break;
	case MOVE:
	  printf("Move: %6.2f %6.2f %6.2f",
		 op[i].op.move.d[0],op[i].op.move.d[1],
		 op[i].op.move.d[2]);
	  if (op[i].op.move.p != NULL)
	    {
	      printf("\tknob: %s",op[i].op.move.p->name);
	    }
	  break;
	case SCALE:
	  printf("Scale: %6.2f %6.2f %6.2f",
		 op[i].op.scale.d[0],op[i].op.scale.d[1],
		 op[i].op.scale.d[2]);
	  if (op[i].op.scale.p != NULL)
	    {
	      printf("\tknob: %s",op[i].op.scale.p->name);
	    }
	  break;
	case ROTATE:
	  printf("Rotate: axis: %6.2f degrees: %6.2f",
		 op[i].op.rotate.axis,
		 op[i].op.rotate.degrees);
	  if (op[i].op.rotate.p != NULL)
	    {
	      printf("\tknob: %s",op[i].op.rotate.p->name);
	    }
	  break;
	case BASENAME:
	  printf("Basename: %s",op[i].op.basename.p->name);
	  break;
	case SAVE_KNOBS:
	  printf("Save knobs: %s",op[i].op.save_knobs.p->name);
	  break;
	case TWEEN:
	  printf("Tween: %4.0f %4.0f, %s %s",
		 op[i].op.tween.start_frame,
		 op[i].op.tween.end_frame,
		 op[i].op.tween.knob_list0->name,
		 op[i].op.tween.knob_list1->name);
	  break;
	case FRAMES:
	  printf("Num frames: %4.0f",op[i].op.frames.num_frames);
	  break;
	case VARY:
	  printf("Vary: %4.0f %4.0f, %4.0f %4.0f",
		 op[i].op.vary.start_frame,
		 op[i].op.vary.end_frame,
		 op[i].op.vary.start_val,
		 op[i].op.vary.end_val);
	  break;
	case PUSH:
	  printf("Push");
	  break;
	case POP:
	  printf("Pop");
	  break;
	case GENERATE_RAYFILES:
	  printf("Generate Ray Files");
	  break;
	case SAVE:
	  printf("Save: %s",op[i].op.save.p->name);
	  break;
	case SHADING:
	  printf("Shading: %s",op[i].op.shading.p->name);
	  break;
	case SETKNOBS:
	  printf("Setknobs: %f",op[i].op.setknobs.value);
	  break;
	case FOCAL:
	  printf("Focal: %f",op[i].op.focal.value);
	  break;
	case DISPLAY:
	  printf("Display");
	  break;
    }
      printf("\n");
    }
}


