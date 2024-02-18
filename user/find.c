#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// get the filename from the whole path
char* get_filename(char *path){
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path);p>=path&&*p!='/';p--)
        ;
    p++;
    return p;
}

void find(char *path,char *target){

    char buf[512],*p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd,&st)<0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
        case T_DEVICE:
        case T_FILE:
            // 检查是否匹配目标文件名称
            char *f_name= get_filename(path);
            // printf("filename:%s,target:%s\n",f_name,target);
            if (strlen(f_name) != 0 && strcmp(f_name,target) == 0){
                printf("%s\n",path);
            }
            close(fd);
            break;
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            while(read(fd,&de,sizeof(de)) == sizeof(de)){
                 if(de.inum==0||strcmp(de.name,".")==0||strcmp(de.name,"..")==0){
                     continue;
                 }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0){
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                // printf("p:%s,buf:%s\n",p,buf);
                find(buf, target);
            }
            close(fd);
            break;
    }
}



int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(2,"find usage: find [dir] [filename]\n");
        exit(1);
    }
    find(argv[1],argv[2]);
    exit(0);
}
