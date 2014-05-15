#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/globals.h"
#include "src/graphics/screen.h"
#include "src/graphics/matrix.h"
#include "src/interpreter/interpreter.h"
#include "src/interpreter/stack/point.h"
#include "src/interpreter/stack/stack.h"

#include "lib/parser.h"
#include "bin/y.tab.h"

extern int lastop;
extern struct command op[MAX_COMMANDS];

void evaluateMDLScript(Matrix_t ** points, Stack_t * coordStack){
	int cmdNum;
	for(cmdNum = 0; cmdNum < lastop; cmdNum++){
		Command_t * cmd = &op[cmdNum];
		int opCode = cmd->opcode;

		if(opCode == BASENAME)
			printf("Basename: %s\n", op[cmdNum].op.basename.p->name);

		else if(opCode == BOX){
			struct symBox * box = &(cmd->op.box);
			addRectangularPrism(*points, box->d0[0], box->d0[1], box->d0[2],
				box->d1[0], box->d1[1], box->d1[2]);
			multiplyMatrix(peek(coordStack), *points);
			drawMatrix(*points);
			CLEAR(*points);
		}

		else if(opCode == DISPLAY)
			renderScreen();

		else if(opCode == FRAMES)
			printf("Num frames: %4.0f\n",op[cmdNum].op.frames.num_frames);

		else if(opCode == LINE){
			struct symLine * line = &(cmd->op.line);
			addEdge(*points, line->p0[0], line->p0[1], line->p0[2], line->p1[0],
				line->p1[1], line->p1[2]);
			multiplyMatrix(peek(coordStack), *points);
			drawMatrix(*points);
			CLEAR(*points);
		}

		else if(opCode == MOVE){
			struct symMove * move = &(cmd->op.move);
			Matrix_t * translation = createTranslation(move->d[0], move->d[1],
				move->d[2]);
			multiplyMatrix(peek(coordStack), translation);
			freeMatrix(pop(coordStack));
			push(coordStack, translation);
		}

		else if(opCode == POP)
			freeMatrix(pop(coordStack));

		else if(opCode == PUSH)
			push(coordStack, copyMatrix(peek(coordStack)));

		else if(opCode == ROTATE){
			struct symRotate * symRotation = &(cmd->op.rotate);
			Matrix_t * rotation = createRotation((int)symRotation->axis,
				symRotation->degrees);
			multiplyMatrix(peek(coordStack), rotation);
			freeMatrix(pop(coordStack));
			push(coordStack, rotation);
		}

		else if(opCode == SAVE)
			writeScreen(cmd->op.save.p->name);

		else if(opCode == SCALE){
			struct symScale * symScale = &(cmd->op.scale);
			Matrix_t * scale = createScale(symScale->d[0],
				symScale->d[1], symScale->d[2]);
			multiplyMatrix(peek(coordStack), scale);
			freeMatrix(pop(coordStack));
			push(coordStack, scale);
		}

		else if(opCode == SPHERE){
			struct symSphere * sphere = &(cmd->op.sphere);
			addSphere(*points, sphere->d[0], sphere->d[1], sphere->r);
			multiplyMatrix(peek(coordStack), *points);
			drawMatrix(*points);
			CLEAR(*points);
		}

		else if(opCode == TORUS){
			struct symTorus * torus = &(cmd->op.torus);
			addTorus(*points, torus->d[0], torus->d[1], torus->r0, torus->r1);
			multiplyMatrix(peek(coordStack), *points);
			drawMatrix(*points);
			CLEAR(*points);
		}

		else if(opCode == VARY)
			printf("Vary: %4.0f %4.0f, %4.0f %4.0f\n",
				op[cmdNum].op.vary.start_frame,
				op[cmdNum].op.vary.end_frame,
				op[cmdNum].op.vary.start_val,
				op[cmdNum].op.vary.end_val);
	}
}
