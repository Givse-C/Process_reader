/* Standard Logic & I/O */
#include <stdio.h>      // For reading files in /proc (fopen, fgets, fscanf)
#include <stdlib.h>     // For memory management and system() calls
#include <stdbool.h>    // For the infinite monitoring loop (while(true))
#include <string.h>     // For parsing strings and searching for "VmRSS" or "cpu"
#include <time.h>       // For controlling the polling interval (sleep)

/* System & Process Handling */
#include <unistd.h>     // For fork(), getpid(), and access to POSIX system calls
#include <dirent.h>     // For directory navigation (opening and reading /proc)
#include <sys/types.h>  // For system data types like pid_t
#include <signal.h>     // For handling interrupts (Ctrl+C) to shut down cleanly

/* Utility & Parsing */
#include <ctype.h>      // For isdigit() to verify if a folder name is a PID
#include <fcntl.h>      // Optional: For low-level file control

int main(){

   while(1){
        //let's open the direcotry
        DIR* dir_ptr = opendir("/proc");

        if(dir_ptr == NULL){
            perror("Something happened");
            exit(-1);
        }
        struct dirent* entry;

        entry = readdir(dir_ptr);
    
        while(entry != NULL){
            //FIND A PID IN THE LIST
            if(isdigit(entry->d_name[0])){
                //we have a process
                //printf("PID [%s]\n", entry->d_name);

                //build path
                char path[512];
                snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);

                //open and read inside
                FILE* f = fopen(path, "r");

                //manages if a process closes between when we see it and when we open it
                if(f != NULL){
                    char line[256];
                    char proc_name[256] = "unknown";
                    bool found = false;

                    while(!found && fgets(line,sizeof(line), f) != NULL){
                        if(strncmp(line, "Name: ", 5) == 0)
                            sscanf(line, "Name:\t%s", proc_name);

                        if(strncmp(line, "VmRSS:", 6) == 0){
                            int mem_value = 0;

                            //extracting from string
                            sscanf(line, "VmRSS: %d", &mem_value);

                            if(mem_value > 100000){
                                printf("ALERT: process with PID [ %s ] and name [ %s ] is using %d kb\n", entry->d_name, proc_name ,mem_value);
                            }
                            found = true;
                        }               
                    }
                    fclose(f);
                }   
            }
            entry = readdir(dir_ptr);
        }
        closedir(dir_ptr);
        sleep(5);
    }
    return 0;
}