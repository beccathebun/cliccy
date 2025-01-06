// src: https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
#ifndef _COLOUR_H
#define  _COLOUR_H
#define T_SEG(V) V ";"
#define T_RESET "\x1b[0m"

// ------ Cursor controls -------

/*
moves cursor to home position (0, 0)
*/
#define CUR_NIL "\x1b[H"
/*
moves cursor to (l,c)
*/
#define CUR_MOV(nlines,ncols) "\x1b[" #nlines ";" #ncols "H"
#define CUR_UP(nlines)        "\x1b[" #nlines "A"
#define CUR_DOWN(nlines)      "\x1b[" #nlines "B"
#define CUR_RIGHT(nlines)     "\x1b[" #nlines "C"
#define CUR_LEFT(nlines)      "\x1b[" #nlines "D"

// ------ Erase -------

// erase from cursor until end of screen
#define ERS_END       "\x1b[0J"
#define ERS_BEG       "\x1b[1J"
#define ERS_ALL       "\x1b[2J"
#define ERS_SAVED     "\x1b[3J"

// erase from cursor to end of line
#define ERS_EOL       "\x1b[0K"
#define ERS_BOL       "\x1b[1K"
#define ERS_LINE      "\x1b[2K"

// ------ Colours -------

#define FG_BLK        "\x1b[30m"
#define FG_RED        "\x1b[31m"
#define FG_GRN        "\x1b[32m"
#define FG_YLW        "\x1b[33m"
#define FG_BLU        "\x1b[34m"
#define FG_MAG        "\x1b[35m"
#define FG_CYN        "\x1b[36m"
#define FG_WHT        "\x1b[37m"
#define FG_DEF        "\x1b[39m"
#define FG_BBLK       "\x1b[90m"
#define FG_BRED       "\x1b[91m"
#define FG_BGRN       "\x1b[92m"
#define FG_BYLW       "\x1b[93m"
#define FG_BBLU       "\x1b[94m"
#define FG_BMAG       "\x1b[95m"
#define FG_BCYN       "\x1b[96m"
#define FG_BWHT       "\x1b[97m"
#define FG_256(id)    "\x1b[38;5;" #id "m"
#define FG_RGB(r,g,b) "\x1b[38;2;" #r ";" #g ";" #b "m"

#define BG_BLK        "\x1b[40m"
#define BG_RED        "\x1b[41m"
#define BG_GRN        "\x1b[42m"
#define BG_YLW        "\x1b[43m"
#define BG_BLU        "\x1b[44m"
#define BG_MAG        "\x1b[45m"
#define BG_CYN        "\x1b[46m"
#define BG_WHT        "\x1b[47m"
#define BG_DEF        "\x1b[49m"
#define BG_BBLK       "\x1b[100m"
#define BG_BRED       "\x1b[101m"
#define BG_BGRN       "\x1b[102m"
#define BG_BYLW       "\x1b[103m"
#define BG_BBLU       "\x1b[104m"
#define BG_BMAG       "\x1b[105m"
#define BG_BCYN       "\x1b[106m"
#define BG_BWHT       "\x1b[107m"
#define BG_256(id)    "\x1b[48;5;" #id "m"
#define BG_RGB(r,g,b) "\x1b[48;2;" #r ";" #g ";" #b "m"


#endif //_COLOUR_H