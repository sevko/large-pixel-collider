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

// A representation of a variable's value over a number of frames.
typedef struct {
	char * name; // The name of the variable.
	double * gradient; // The values of the variable per frame.
} VariableGradient_t;

// The MDL script's variable values per frame.
static VariableGradient_t ** g_variableGradients;
static int g_frames; // The number of frames used by the MDL script.

/*
 * @brief Create a ::VariableGradient_t for a given `vary` command.
 *
 * @param startFrame The number of the frame during which varying begins.
 * @param endFrame The number of the frame during which varying ends.
 * @param startVal The starting value of the variable.
 * @param endVal The ending value of the variable.
 * @param varName The name of the variable.
 *
 * @return A pointer to the ::VariableGradient_t generated for the given
 *      variable.
 */
static VariableGradient_t * createGradient(int startFrame, int endFrame,
		double startVal, double endVal, char * varName);

int initializeVariables(){
	int cmdNum;
	for(cmdNum = 0; cmdNum < lastop; cmdNum++){
		Command_t * cmd = &op[cmdNum];
		int opCode = cmd->opcode;

		if(opCode == FRAMES){
			g_frames = cmd->op.frames.num_frames;
			break;
		}

		else if(opCode == VARY){
			ERROR("Command %d: `vary` command called but no `frames`"
					"specified.", cmdNum);
			return 0;
		}
	}

	int numVariables = 0;
	for(cmdNum = 0; cmdNum < lastop; cmdNum++){
		Command_t * cmd = &op[cmdNum];
		int opCode = cmd->opcode;

		if(opCode == VARY){
			struct symVary * var = &(cmd->op.vary);
			g_variableGradients = realloc(g_variableGradients,
					(numVariables + 1) * sizeof(VariableGradient_t *));
			g_variableGradients[numVariables++] = createGradient(
					var->start_frame, var->end_frame, var->start_val,
					var->end_val, var->p->name);
		}
	}

	int variable;
	for(variable = 0; variable < numVariables; variable++){
		printf("%s:", g_variableGradients[variable]->name);
		int frame;
		for(frame = 0; frame < g_frames; frame++)
			printf(" %f", g_variableGradients[variable]->gradient[frame]);
		puts("");
	}

	return 1;
}

void evaluateMDLScript(Matrix_t ** points, Stack_t * coordStack){
	int cmdNum;
	for(cmdNum = 0; cmdNum < lastop; cmdNum++){
		Command_t * cmd = &op[cmdNum];
		int opCode = cmd->opcode;

		if(opCode == BOX){
			struct symBox * box = &(cmd->op.box);
			addRectangularPrism(*points, box->d0[0], box->d0[1], box->d0[2],
				box->d1[0], box->d1[1], box->d1[2]);
			multiplyMatrix(peek(coordStack), *points);
			drawMatrix(*points);
			CLEAR(*points);
		}

		else if(opCode == DISPLAY)
			renderScreen();

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
	}
}

static VariableGradient_t * createGradient(int startFrame, int endFrame,
		double startVal, double endVal, char * varName){
	VariableGradient_t * gradient = malloc(sizeof(VariableGradient_t));
	gradient->name = varName;
	gradient->gradient = malloc(g_frames * sizeof(double));

	double deltaVarPerFrame = (endVal - startVal) / (endFrame - startFrame);

	int frame;
	for(frame = 0; frame <= startFrame; frame++)
		gradient->gradient[frame] = startVal;

	for(; frame <= endFrame; frame++){
		startVal += deltaVarPerFrame;
		gradient->gradient[frame] = startVal;
	}

	for(; frame < g_frames; frame++)
		gradient->gradient[frame] = endVal;

	return gradient;
}
