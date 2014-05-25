#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
static int g_numVariables;
static int g_numFrames; // The number of frames used by the MDL script.

/*
 * @brief Find a ::VariableGradient_t with a given name.
 *
 * @param name The name of the variable to search for.
 *
 * @return A pointer to the ::VariableGradient_t struct with a name identical to
 *      @p name.
 */
static VariableGradient_t * findVariable(char * name);

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

/*
 * @brief Deallocate all memory belonging to a ::VariableGradient_t.
 *
 * @param gradient A pointer to the ::VariableGradient_t to deallocate.
*/
static void freeGradient(VariableGradient_t * gradient);

int initializeVariables(){
	int cmdNum;
	for(cmdNum = 0; cmdNum < lastop; cmdNum++){
		Command_t * cmd = &op[cmdNum];
		int opCode = cmd->opcode;

		if(opCode == FRAMES){
			g_numFrames = cmd->op.frames.num_frames;
			break;
		}

		else if(opCode == VARY){
			ERROR("Command %d: `vary` command called but no `frames`"
					"specified.", cmdNum);
			return 0;
		}
	}

	g_numVariables = 0;
	for(cmdNum = 0; cmdNum < lastop; cmdNum++){
		Command_t * cmd = &op[cmdNum];
		int opCode = cmd->opcode;

		if(opCode == VARY){
			struct symVary * var = &(cmd->op.vary);
			g_variableGradients = realloc(g_variableGradients,
					(g_numVariables + 1) * sizeof(VariableGradient_t *));
			g_variableGradients[g_numVariables++] = createGradient(
					var->start_frame, var->end_frame, var->start_val,
					var->end_val, var->p->name);
		}
	}

	return 1;
}

void evaluateMDLScript(){
	int frame;
	for(frame = 0; frame < g_numFrames; frame++){
		Matrix_t * points = createMatrix();
		Stack_t * coordStack = createStack();

		int cmdNum;
		for(cmdNum = 0; cmdNum < lastop; cmdNum++){
			Command_t * cmd = &op[cmdNum];
			int opCode = cmd->opcode;

			if(opCode == BOX){
				struct symBox * box = &(cmd->op.box);
				addRectangularPrism(points,
					POINT(box->d0[0], box->d0[1], box->d0[2]),
					POINT(box->d1[0], box->d1[1], box->d1[2]));
				multiplyMatrix(peek(coordStack), points);
				drawMatrix(points);
				CLEAR(points);
			}

			else if(opCode == DISPLAY)
				renderScreen();

			else if(opCode == LINE){
				struct symLine * line = &(cmd->op.line);
				addEdge(points,
					POINT(line->p0[0], line->p0[1], line->p0[2]),
					POINT(line->p1[0], line->p1[1], line->p1[2]));
				multiplyMatrix(peek(coordStack), points);
				drawMatrix(points);
				CLEAR(points);
			}

			else if(opCode == MOVE){
				struct symMove * move = &(cmd->op.move);

				double dx = move->d[0], dy = move->d[1], dz = move->d[2];
				if(move->p){
					double scale = findVariable(move->p->name)->gradient[frame];
					dx *= scale;
					dy *= scale;
					dz *= scale;
				}

				Matrix_t * translation = createTranslation(POINT(dx, dy, dz));
				multiplyMatrix(peek(coordStack), translation);
				freeMatrix(pop(coordStack));
				push(coordStack, translation);
			}

			else if(opCode == POP)
				freeMatrix(pop(coordStack));

			else if(opCode == PUSH)
				push(coordStack, copyMatrix(peek(coordStack)));

			else if(opCode == ROTATE){
				struct symRotate * symRot = &(cmd->op.rotate);

				double angle = symRot->degrees;
				if(symRot->p)
					angle *= findVariable(symRot->p->name)->
							gradient[frame];

				Matrix_t * rotation = createRotation((int)symRot->axis, angle);
				multiplyMatrix(peek(coordStack), rotation);
				freeMatrix(pop(coordStack));
				push(coordStack, rotation);
			}

			else if(opCode == SAVE)
				writeScreen(cmd->op.save.p->name);

			else if(opCode == SCALE){
				struct symScale * symScale = &(cmd->op.scale);

				double dx = symScale->d[0], dy = symScale->d[1],
						dz = symScale->d[2];
				if(symScale->p){
					double scale = findVariable(symScale->p->name)->
							gradient[frame];
					dx *= scale;
					dy *= scale;
					dz *= scale;
				}

				Matrix_t * scale = createScale(POINT(dx, dy, dz));
				multiplyMatrix(peek(coordStack), scale);
				freeMatrix(pop(coordStack));
				push(coordStack, scale);
			}

			else if(opCode == SPHERE){
				struct symSphere * sphere = &(cmd->op.sphere);
				addSphere(points, POINT(sphere->d[0], sphere->d[1]), sphere->r);
				multiplyMatrix(peek(coordStack), points);
				drawMatrix(points);
				CLEAR(points);
			}

			else if(opCode == TORUS){
				struct symTorus * torus = &(cmd->op.torus);
				addTorus(points, POINT(torus->d[0], torus->d[1]), torus->r0,
						torus->r1);
				multiplyMatrix(peek(coordStack), points);
				drawMatrix(points);
				CLEAR(points);
			}
		}

		usleep(1e6 / 20);
		clearScreen();
		freeMatrix(points);
		freeStack(coordStack, &freeMatrixFromVoid);
	}

	int gradient;
	for(gradient = 0; gradient < g_numVariables; gradient++)
		freeGradient(g_variableGradients[gradient]);
	free(g_variableGradients);
}

static VariableGradient_t * findVariable(char * name){
	int var;
	for(var = 0; var < g_numVariables; var++)
		if(strcmp(name, g_variableGradients[var]->name) == 0)
			return g_variableGradients[var];
	return NULL;
}

static VariableGradient_t * createGradient(int startFrame, int endFrame,
		double startVal, double endVal, char * varName){
	if(startFrame < 0)
		FATAL("The starting frame cannot be negative.");
	else if(g_numFrames <= endFrame)
		FATAL("The ending frame must be less than the number of frames.");
	else if(endFrame < startFrame)
		FATAL("The ending frame must be greater than or equal to the starting"
				"frame.");

	VariableGradient_t * gradient = malloc(sizeof(VariableGradient_t));
	gradient->name = varName;
	gradient->gradient = malloc(g_numFrames * sizeof(double));

	double deltaVarPerFrame = (endVal - startVal) / (endFrame - startFrame);

	int frame;
	for(frame = 0; frame <= startFrame; frame++)
		gradient->gradient[frame] = startVal;

	for(; frame <= endFrame; frame++){
		startVal += deltaVarPerFrame;
		gradient->gradient[frame] = startVal;
	}

	for(; frame < g_numFrames; frame++)
		gradient->gradient[frame] = endVal;

	return gradient;
}

static void freeGradient(VariableGradient_t * gradient){
	free(gradient->name);
	free(gradient->gradient);
	free(gradient);
}
