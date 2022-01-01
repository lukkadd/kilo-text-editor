kilo: kilo.c
	$(CC) kilo.c -o kilo -Wall -Wextra -pedantic -std=c99

# -Wall	display all warnings
# -Wextra and -pedantic activates more types of warnings
# -std=99 use C 99 standard