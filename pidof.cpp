#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <cstring>

// /proc/stat中显示名称长度为16, 实际字符升序为15.
#define MAX_LEN 15

bool check_cmdline(std::string &path, std::string &name)
{
    std::string cmdline_name(path);

    cmdline_name.append("/cmdline");

    std::ifstream fin(cmdline_name, std::ios::in);
    if(!fin.is_open())
    {
        return false;
    }
//line first
    std::string proc_name;
    std::getline(fin, proc_name);
    fin.close();
    //std::cout<<"proc name:"<<proc_name<<std::endl;
    std::string proc_name_l1;
    std::size_t f1 = proc_name.find_first_of('\0');
    if(f1 != std::string::npos)
    {
        proc_name_l1 = proc_name.substr(0, f1);
    }
    std::size_t f2 = proc_name_l1.find_last_of('/');
    if(f1 != std::string::npos)
    {
        proc_name_l1 = proc_name_l1.substr(f2 + 1, proc_name_l1.length() - f2 - 1);
    }
    if(proc_name_l1.compare(0, name.length(), name) == 0)
    {
        return true;
    }
//line second
    //std::cout<<"first:"<<proc_name_l1<<std::endl;
    int len = f1+1;
    if(len > proc_name.length())
    {
        return false;
    }else{
        std::string proc_name_l2;
        proc_name_l2 = proc_name.substr(len, proc_name.length() - len);
        //std::cout<<"second:"<<proc_name_l2<<std::endl;
        f1 = proc_name_l2.find_first_of('\0');
        if(f1 != std::string::npos)
        {
            proc_name_l2 = proc_name_l2.substr(0, f1);
        }
        std::size_t f2 = proc_name_l2.find_last_of('/');
        if(f2 != std::string::npos)
        {
            proc_name_l2 = proc_name_l2.substr(f2 + 1, proc_name_l2.length() - f2 - 1);
        }
        if(name.compare(0, name.length(), proc_name_l2) == 0)
        {
            return true;
        }
    }

    return false;
}
bool check_stat(std::string &path, std::string &name)
{
    std::string stat_name(path);

    stat_name.append("/stat");
    std::ifstream fin(stat_name, std::ios::in);
    if (!fin.is_open())
    {
        return false;
    }
    std::string proc_name;
    std::getline(fin, proc_name);
    fin.close();
    std::size_t f1 = proc_name.find("(");
    std::size_t f2 = proc_name.find(")");
    if (f1 == std::string::npos || f2 == std::string::npos)
    {
        return false;
    }
    //如果任务名称长度小于15,匹配成功则认为已匹配到，stat仅任务名称仅显示15个字节
    proc_name = proc_name.substr(f1 + 1, (f2 - 1 - f1));
    if (proc_name == name)
    {
        if (proc_name.length() < MAX_LEN)
        {
            return true;
        }
    }
    return false;
}
bool check_exe(std::string &path, std::string &name)
{
    std::string exe_name(path);

    exe_name.append("/exe");

    std::ifstream fin(exe_name, std::ios::in);
    if(!fin.is_open())
    {
        return false;
    }
    std::string proc_name;
    std::getline(fin, proc_name);

    std::size_t f1 = proc_name.find_last_of("/");
    if(f1 != std::string::npos)
    {
        proc_name = proc_name.substr(f1+1, proc_name.length() - f1 -1);
    }

    if(proc_name.compare(name)== 0)
    {
        return true;
    }

    return false;
}

int CheckProcByName(std::string name)
{
    DIR             *dir = NULL;
    struct dirent   *d = NULL;
    pid_t           pid = 0;
    pid_t           self_pid =getpid();

    dir = opendir("/proc");
    if(!dir)
    {
        printf("cannot open /proc.\n");
        return -1;
    }

    while((d = readdir(dir)) != NULL){
        if((pid = atoi(d->d_name)) == 0)
            continue;
        //std::cout<<"pid:"<<pid<<std::endl;
        std::string base_path("/proc/");
        base_path.append(d->d_name);
        if (check_cmdline(base_path, name) || check_stat(base_path, name))
        {
            if(pid == self_pid)
                continue;
            std::cout<<pid<<" ";
            //return true;
        }
    }
    std::cout<<std::endl;
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        std::cout<<"usage: ./pidof task_name"<<std::endl;
        return 0;
    }
    CheckProcByName(argv[1]);
    return 0;
}
