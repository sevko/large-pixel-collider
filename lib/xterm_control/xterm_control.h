// xterm control sequences

// 0 parameter controls:
#define XT_CLEAR_SCREEN			"\033[2J"
#define XT_ERASE_LINE			"\033[2K"

// 1 parameter controls:
#define XT_SET_COL_POS			"\033[%d`"
#define XT_DELETE_LINES			"\033[%dM"
#define XT_INSERT_LINES			"\033[%dL"

// 2  parameter controls
#define XT_SET_ROW_COL_POS		"\033[%d;%dH"

// character attribute and color changes
#define XT_CH_NORMAL		"\033[0m"
#define XT_CH_BOLD			"\033[1m"
#define XT_CH_UNDERLINE		"\033[4m"
#define XT_CH_INVERSE		"\033[7m"
#define XT_CH_BLACK			"\033[30m"
#define XT_CH_RED			"\033[31m"
#define XT_CH_GREEN			"\033[32m"
#define XT_CH_YELLOW		"\033[33m"
#define XT_CH_BLUE			"\033[34m"
#define XT_CH_MAGENTA		"\033[35m"
#define XT_CH_CYAN			"\033[36m"
#define XT_CH_WHITE			"\033[37m"
#define XT_CH_DEFAULT		"\033[39m"

// background color changes
#define XT_BG_BLACK			"\033[40m"
#define XT_BG_RED			"\033[41m"
#define XT_BG_GREEN			"\033[42m"
#define XT_BG_YELLOW		"\033[43m"
#define XT_BG_BLUE			"\033[44m"
#define XT_BG_MAGENTA		"\033[45m"
#define XT_BG_CYAN			"\033[46m"
#define XT_BG_WHITE			"\033[47m"
#define XT_BG_DEFAULT		"\033[49m"


void xt_par0(char *control_sequence);
void xt_par1(char *control_sequence, int parameter1);
void xt_par2(char *control_sequence, int parameter1, int parameter2);
