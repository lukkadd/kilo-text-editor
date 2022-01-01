/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>


/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s) {
	/** error handling**/
	perror(s);
	exit(1);
}

void disableRawMode() {
	/** set the terminal attributes back to it's original state **/
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    	die("tcsetattr");
}

void enableRawMode() {
	/** read current attr to orig_termios **/
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
	/** 'at the program exit' run disableRawMode**/
	atexit(disableRawMode);

	struct termios raw = orig_termios;

	/** ~CS8~ - setting character size to 8 bytes*/
	raw.c_cflag |= (CS8);

	/** turn off the output flags:
	 * OPOST - output post processing (transforming '\n' into '\r\n') **/
	raw.c_oflag &= ~(OPOST);

	/** turn off the input flags:
	 * IXON - disable software flow control signals like Ctrl+S or Ctrl+Q
	 * ICRNL - forces Ctrl+M (carriage return '\r') from being read as newline '\n' 
	 * ~BRKINT~ - set break conditions to generate SIGINT's to the program
	 * ~INPCK~ - enable parity checking 
	 * ~ISTRIP~ - stop the program from stripping the eigth bit of each input byte **/
	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP );

	/** turn off the local flags:
	 * ECHO - stop the terminal from showing (echoing) the user input as he types 
	 * ICANON - read input byte by byte instead of line by line 
	 * ISIG - disable signals like Ctrl+C or Ctrl+Z
	 * IEXTEN - Stops Ctrl+V from waiting for an additional character **/
	raw.c_lflag &= ~(ECHO | ICANON | ISIG);

	/** set control characters:
	 * VMIN - minimum number of bytes before a read() is triggered
	 * VTIME - maximum ammount of time before read() returns (in tenths of seconds) **/ 
	raw.c_cc[VMIN] = 0;
  	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*** init ***/

int main() {
	enableRawMode();

	/** read user input **/
	while (1) {
	    char c = '\0';
	    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");

		if(iscntrl(c)){ //check if c is a control character
			printf("%d\r\n", c);
		}else {
			printf("%d ('%c')\r\n", c, c);
		}
		if (c == 'q') break;
	}

	/* code */
	return 0;
}