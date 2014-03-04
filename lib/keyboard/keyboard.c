/* Keyboard access for Linux
Version 0.8 -- Peter Brooks, Stuyvesant High School

	int getkey(void)
		returns:
			-2 (or KEY_NOTHING) if no key has been pressed
			the ASCII code of the key pressed
			the KEY_* code of the special key pressed (see below for KEY_* constants)
		
	Typical usage:
		while (TRUE) {
			if ((c = getkey()) != KEY_NOTHING)
				// then process the key c
		}
*/

#include "keyboard.h"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifndef	TRUE
#define TRUE	1
#define FALSE	0
#endif

static int isBeginningOf(char *, char *);
static int put_circ(int code);
static int get_circ(void);

#define CIRC_SIZE	20
static struct circular_buffer {
	int buf[CIRC_SIZE];
	int next_in;
	int next_out;
	int in_buf;
} circ;

static struct termios oldt;
static int oldf;

int getkey(void) {
	struct termios newt;
	struct code_set {
			char *code_string;
			int code_int;
	};
	
	static struct code_set code_sets[] = 
		{
						{"\033[A",   KEY_UP },
						{"\033[B",	 KEY_DOWN},
						{"\033[C",	 KEY_RIGHT},
						{"\033[D",	 KEY_LEFT},
						{"\033[H",	 KEY_HOME},
						{"\033[1~",  KEY_HOME},
						{"\033[F",	 KEY_END},
						{"\033[4~",  KEY_END},
						{"\033[5~",	 KEY_PGUP},
						{"\033[6~",	 KEY_PGDN},
						{"\033[3~",	 KEY_DELETE},
						{"\033[11~", KEY_F1},
						{"\033OP",   KEY_F1},
						{"\033[12~", KEY_F2},
						{"\033OQ",   KEY_F2},
						{"\033[13~", KEY_F3},
						{"\033OR",   KEY_F3},
						{"\033[14~", KEY_F4},
						{"\033OS",   KEY_F4},
						{"\033[15~", KEY_F5},
						{"\033[17~", KEY_F6}, // strange skipping of "\033[18~"
						{"\033[18~", KEY_F7},
						{"\033[20~", KEY_F9}, // couldn't get F8 to work
						{"\033[21~", KEY_F10}
		};

	static int ncodes = sizeof(code_sets)/sizeof(code_sets[0]);
	
	static int init = FALSE;
	
	// turn ECHOing off, NONBLOCKing on
	if (!init) {
		tcgetattr(STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
		fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
		
		circ.next_in = circ.next_out = circ.in_buf = 0;
		init = TRUE;
	}
	
	// read in all pending chars into the immediate buffer
	char immediate_buffer[20];
	int imm_bufsize=20, in_immediate_buffer=0, ch;
	while ((ch = getchar()) != EOF) {
		if (in_immediate_buffer >= imm_bufsize) break;
		immediate_buffer[in_immediate_buffer++]=ch;
	}
	immediate_buffer[in_immediate_buffer] = '\0';
	
	// empty all chars and recognized codes from immediate_buffer to circular_buffer
	char *p = immediate_buffer;
	int icode, code_len;
	while (in_immediate_buffer > 0) {
		for (icode = 0; icode < ncodes; ++icode) {
			if (isBeginningOf(p, code_sets[icode].code_string)) {
				put_circ(code_sets[icode].code_int);
				code_len = strlen(code_sets[icode].code_string);
				p += code_len;
				in_immediate_buffer -= code_len;
				break;
			}
		}
		if (icode >= ncodes) {
			put_circ(*p++);
			--in_immediate_buffer;
		}
	}
	
	// Done processing incoming chars
	// deliver the next char/code from the circular buffer
	return get_circ();
}

// you MUST call this when you're done with keyboard processing, before you terminate
void getkey_terminate(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
}
	
// Return TRUE if the string scode is at the beginning of the string s
static int isBeginningOf(char *s, char *scode) {
	while (*scode) {
		if (*s++ != *scode++)
			return FALSE;
	}
	return TRUE;
}

// Put a code/char into the circular buffer
static int put_circ(int code) {
	if (circ.in_buf >= CIRC_SIZE)
		return FALSE;
	circ.buf[circ.next_in++] = code;
	circ.next_in %= CIRC_SIZE;
	++circ.in_buf;
	return TRUE;
}

// Get the next code/char from the circular buffer
static int get_circ(void) {
	int out_code;
	if (circ.in_buf == 0)
		return KEY_NOTHING;
	out_code = circ.buf[circ.next_out++];
	circ.next_out %= CIRC_SIZE;
	--circ.in_buf;
	return out_code;
}
