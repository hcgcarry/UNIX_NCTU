#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#define TARGET (0x01 << 30)
#define EXPLOSION "/tmpfs/explosion"

int main(void)
{
    int fd = open(EXPLOSION, O_CREAT | O_WRONLY, 0644);
    char tmp[10] = "hello";
    lseek(fd, TARGET - strlen(tmp), SEEK_SET);
    write(fd, tmp, strlen(tmp));
    close(fd);
}
