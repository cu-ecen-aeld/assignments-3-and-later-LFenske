
/*
 * Write a given string to a file.
 * writer <writefile> <writestr>
 *   writefile: path to file to write, directory must exist
 *   writestr : string to write to writefile
 */

/* Written for CU Boulder ECEA 5305, Assignment 2 */

#include <unistd.h>  /* for open(), write() */
#include <fcntl.h>   /* for open parameter */
#include <stdio.h>   /* for printf() */
#include <string.h>  /* for strlen() */
#include <stdlib.h>  /* for exit() */
#include <errno.h>   /* for errno */
#include <syslog.h>

#define SHUTDOWN(exitval)  closelog(); exit(exitval)

int
usage(char *progname)
{
  printf("Write a given string to a file.\n");
  printf("usage: %s <writefile> <writestr>\n", progname);
}

int
main(int argc, char **argv)
{
  char *writefile;  /* name of file to write to */
  char *writestr;   /* string to write to writefile */
  int fd;           /* file descriptor of writefile */
  int length;       /* length of writestr */
  int retval;       /* return value from write */

  openlog(NULL, LOG_CONS | LOG_PERROR, LOG_USER);

  /* Check number of arguments. */
  if (argc != 3) {
    syslog(LOG_ERR, "Improper number of arguments.");
    usage(argv[0]);
    SHUTDOWN(1);
  }

  writefile = argv[1];
  writestr  = argv[2];

  /* Open the file for writing, creating it if it doesn't exist, and
   * truncating it if it does exist. */
  fd = open(writefile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (-1 == fd) {
    syslog(LOG_ERR, "open: %s", strerror(errno));
    SHUTDOWN(2);
  }

  /* Write writestr to writefile. */
  length = strlen(writestr);
  syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);
  retval = write(fd, writestr, length);
  if (retval != length) {
    if (-1 == retval) {
      /* failure, and errno has the reason */
      syslog(LOG_ERR, "write: %s", strerror(errno));
      SHUTDOWN(3);
    } else {
      /* not all bytes written */
      syslog(LOG_ERR, "only wrote %d characters out of %d", retval, length);
      SHUTDOWN(4);
    }
  }

  /* success */
  SHUTDOWN(0);
}
