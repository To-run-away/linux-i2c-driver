#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void usge_print(char *file)
{
    /*  
     * 使用说明
     */
    printf("%s  </dev/at24cxx>  <r> <addr> <num>\n", file);
    printf("%s  </dev/at24cxx>  <w> <addr> <str>\n", file);
    
}



int main(int argc,char *argv[])
{
    int fd; 
    
    unsigned char buf[50], addr;
    int  len, i;    


    if(argc != 5) {
        usge_print(argv[0]);
        exit(1);
    }   


    fd = open(argv[1], O_RDWR);
    if(fd < 0) {
        perror("open fail\n");
        exit(1);
    }

    memset(buf, 0, 50);


    if(strcmp(argv[2], "r") == 0)   {
        /*
         * 提取要写的寄存器地址和要写的数据长度
         */
        addr = strtoul(argv[3], NULL, 0);    
        len  = strtoul(argv[4], NULL, 0);
        printf("read addr =  %x , len = %d\n", addr, len);
        buf[0] = addr;

        /*
         * 读数据
         */
        if( read(fd,buf, len ) < len) {
            perror("read fail\n");
            exit(1);
        }
        for(i = 0; i < len; i ++)
            printf("read data = %03d, %02x, %c\n", buf[i], buf[i],buf[i]);

    } else if(strcmp(argv[2], "w") == 0) {
        /*
         * 提取要写的寄存器地址和要写的数据长度
         */
        addr = strtoul(argv[3], NULL, 0);
        buf[0] = addr;
        len = strlen(argv[4]);

        /*
         * 第一个地址是要写的寄存器地址,所有数据要向后延长一个字节
         */
        for(i = 0; i < len; i++) {
            buf[i + 1] = argv[4][i];
        }
        if(write(fd, buf, len + 1) < (len + 1)) {
            perror("write fail\n");
            exit(1);
        }
    }
    else {
        usge_print(argv[0]);
        exit(1);
    }
    return 0;
}
