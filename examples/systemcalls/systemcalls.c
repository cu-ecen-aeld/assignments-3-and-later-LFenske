#include "systemcalls.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * DONE  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    int retval;
    retval = system(cmd);

    return retval == 0 ? true : false;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    //command[count] = command[count];

/*
 * DONE:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

    // mostly duplicate with do_exec_redirect()

    va_end(args);

    bool retval = true;

    fflush(stdout);
    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork");
        retval = false;
        goto exit;
    }

    if (child_pid) {

        // parent

        int wstatus;

        pid_t dead_pid = waitpid(child_pid, &wstatus, 0);
        if (dead_pid != child_pid) {
            retval = false;
            goto exit;
        }

        if (WIFEXITED(wstatus)) {
            // Child terminated normally.
            if (WEXITSTATUS(wstatus) != 0) {
                // Child returned an error (non-zero exit code)/
                retval = false;
                goto exit;
            }
        } else {
            // Child did not terminate normally.
            retval = false;
            goto exit;
        }

    } else {

        // child

        execv(command[0], command);
        // Only gets here if execv failed.
        perror("execv");
        // Return error code to parent.
        exit(-1);
    }

exit:

    return retval;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    //command[count] = command[count];


/*
 * DONE
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/

    // mostly duplicate with do_exec()

    va_end(args);

    bool retval = true;

    // Open a file to redirect stdout.
    int fd = open(outputfile, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd < 0) {
        retval = false;
        goto exit;
    }

    fflush(stdout);
    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork");
        retval = false;
        goto exit;
    }

    if (child_pid) {

        // parent

        int wstatus;

        close(fd);

        pid_t dead_pid = waitpid(child_pid, &wstatus, 0);
        if (dead_pid != child_pid) {
            retval = false;
            goto exit;
        }

        if (WIFEXITED(wstatus)) {
            // Child terminated normally.
            if (WEXITSTATUS(wstatus) != 0) {
                // Child returned an error (non-zero exit code)/
                retval = false;
                goto exit;
            }
        } else {
            // Child did not terminate normally.
            retval = false;
            goto exit;
        }

    } else {

        // child

        if (dup2(fd, 1) < 0) {  // Copy it into the stdout file descriptor.
            perror("dup2");
            retval = false;
            goto exit;
        }
        close(fd);  // The original isn't needed anymore: it's now as stdout.

        execv(command[0], command);
        // Only gets here if execv failed.
        perror("execv");
        // Return error code to parent.
        exit(-1);
    }

exit:

    return retval;
}



////// for emacs
// Local Variables:
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
////// for vi and vim
// vi: set ts=4 sw=4 et:
