
#include <errno.h>
#include <unistd.h>

#include "io.h"

/* Reads next char from stdin. If no more characters, it returns EOF */
int
read_char() {
  char c;
  int result = read(0, &c, 1);
  if (result == 0 || result == -1){
    return EOF;
  } else
  return c;
}

/* Writes c to stdout.  If no errors occur, it returns 0, otherwise EOF */
int
write_char(char c) {
  int result = write(1, &c, 1);
  if (result == -1){
    return EOF;
  } else return 0;
}

/* Writes a null-terminated string to stdout.  If no errors occur, it returns 0, otherwise EOF */
int
write_string(char* s) {
  while (*s) {
      if (write_char(*s) == EOF) {
        return EOF;
      }
      s++;
  }

  return 0;
}

/* Writes n to stdout (without any formatting).   
 * If no errors occur, it returns 0, otherwise EOF
 */
int
write_int(int n) {
    char buffer[12];  // Buffer to hold the string representation of the integer
                      // The maximum length of an integer in decimal form (with sign) is 11 characters, plus a null terminator.
    int i = sizeof(buffer) - 1;
    int is_negative = (n < 0);

    buffer[i] = '\0';  // Null-terminate the string

    // Handle special case for 0
    if (n == 0) {
        buffer[--i] = '0';
    } else if (is_negative) {
      return EOF;
    } else {
        while (n > 0) {
            buffer[--i] = (n % 10) + '0';  // Get the last digit and convert to character
            n /= 10;
        }
    }

    // Write the string to stdout
    if (write(1, &buffer[i], sizeof(buffer) - i - 1) == -1) {
        return EOF;
    }

    return 0;
}
