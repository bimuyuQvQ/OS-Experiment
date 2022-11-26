#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include<unistd.h>
#define OK 0
#define ERROR -1
int cpyfile(char* from, char* to)
{
    FILE *fpf = fopen(from,"r");
    FILE *fpt = fopen(to,"w");
    if(fpf == NULL) {
        perror("fp1:");
        return ERROR;
    }
    if(fpt == NULL) {
        perror("fp2:");
        return ERROR;
    }
    char buffer = fgetc(fpf);
    int cnt = 0;
    while(!feof(fpf)) {
        cnt++;
        fputc(buffer,fpt);
        buffer = fgetc(fpf);
    }
    fclose(fpt);
    fclose(fpf);
    return cnt;
}
int main(int argc , char* argv[])
{
    DIR * dir;
    struct dirent * ptr;
    int i;
    dir = opendir(argv[1]);
    if(dir==NULL){
        cpyfile(argv[1], argv[2]);
        return OK;
    }
    mkdir(argv[2], S_IRWXU);
    while((ptr = readdir(dir)) != NULL)
    {
        //printf("d_name : %s, d_type: %d\n", ptr->d_name, ptr->d_type);//dir 4;  file 8; link 10
        char toRoot[100]={0};
        strcpy(toRoot,argv[2]);
        
        if(ptr->d_type==4){
            strcat(toRoot,"/");
            strcat(toRoot,ptr->d_name);
            mkdir(toRoot, S_IRWXU);

        }
        else if(ptr->d_type==8){
            strcat(toRoot,"/");
            strcat(toRoot,ptr->d_name);
            char from[100]={0};
            strcpy(from, argv[1]);
            strcat(from,"/");
            strcat(from,ptr->d_name);
            if(cpyfile(from,toRoot)==ERROR){
                printf("ERROR: file copy error.\n");
                return ERROR;
            }
        }
        else if(ptr->d_type==10){
            char from[100]={0};
            strcpy(from, argv[1]);
            strcat(from,"/");
            strcat(from,ptr->d_name);

            char buf[200]={0};
            readlink(from, buf, 200);

            strcat(toRoot,"/");
            strcat(toRoot,ptr->d_name);

            if(symlink(buf, toRoot)==ERROR){
                printf("ERROR: link copy error.\n");
                return ERROR;
            }
        }
    }
    closedir(dir);

    return 0;

}