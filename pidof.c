#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define BUF_SIZE 1024

void getPidByName(char* task_name)
{
    DIR *dir;
    struct dirent *ptr;
    FILE *fp;
    char filepath[512];
    char cur_task_name[50];
    char buf[BUF_SIZE];
    dir = opendir("/proc");
    if (NULL != dir)
    {
        while ((ptr = readdir(dir)) != NULL)
        {
	    //printf("dirname: %s\n", ptr->d_name);
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) 
			continue;
            if (DT_DIR != ptr->d_type)
		continue;

            sprintf(filepath, "/proc/%s/status", ptr->d_name);
            fp = fopen(filepath, "r");
            if (NULL != fp)
            {
                if( fgets(buf, BUF_SIZE-1, fp)== NULL ){
			fclose(fp);
			continue;
		}
		sscanf(buf, "%*s %s", cur_task_name);

                if (!strcmp(task_name, cur_task_name))
			printf("PID:  %s\n", ptr->d_name);
                fclose(fp);
            }
        }
        closedir(dir);
    }
}

void getNameByPid(pid_t pid, char *task_name) {
    char proc_pid_path[BUF_SIZE];
    char buf[BUF_SIZE];

    sprintf(proc_pid_path, "/proc/%d/status", pid);
    FILE* fp = fopen(proc_pid_path, "r");
    if(NULL != fp){
        if( fgets(buf, BUF_SIZE-1, fp)== NULL ){
            fclose(fp);
        }
        fclose(fp);
        sscanf(buf, "%*s %s", task_name);
    }
}

void main(int argc, char** argv)
{
    //char task_name[50];
    //pid_t pid = getpid();

    //printf("pid of this process:%d\n", pid);
    //getNameByPid(pid, task_name);

    /*
    strcpy(task_name, argv[0]+2);
    printf("task name is %s\n", task_name);
    getPidByName(task_name);
    */
    if(argc < 2) return;
    printf("task name is %s\n", argv[1]);
    getPidByName(argv[1]);
    sleep(15);
}
