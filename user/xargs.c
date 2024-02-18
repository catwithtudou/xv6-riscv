#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"


#define buf_size 512


int main(int argc, char *argv[]) {
    char buf[buf_size+1] = {0};
    char *xargv[MAXARG] = {0};
    int used_size = 0;
    int stdin_end = 0;

    for (int i = 1; i < argc; i++) {
        xargv[i - 1] = argv[i];
    }

    while (!stdin_end || used_size != 0) {
        if (!stdin_end) {
            int read_bytes = read(0, buf + used_size, buf_size - used_size);
            if (read_bytes < 0) {
                fprintf(2, "xargs: read returns -1 error\n");
                exit(1);
            }

            if (read_bytes == 0) {
                close(0);
                stdin_end = 1;
            }
            used_size += read_bytes;
        }

        char *line_end = strchr(buf, '\n');

        while (line_end) {
            char xbuf[buf_size + 1] = {0};
            memcpy(xbuf, buf, line_end - buf);
            xargv[argc - 1] = xbuf;

            int pid = fork();
            if (pid == 0) { // child
                if (!stdin_end) {
                    close(0);
                }
                if (exec(argv[1], xargv) < 0) {
                    fprintf(2, "xargs: exec fails with -1\n");
                    exit(1);
                }
            } else { // parent
                int remain_line = line_end - buf;
                memmove(buf, line_end + 1, used_size - remain_line - 1);
                used_size -= remain_line + 1;
                memset(buf + used_size, 0, buf_size - used_size);
                wait(0);
            }

            line_end = strchr(buf, '\n');
        }
    }
    exit(0);
}
