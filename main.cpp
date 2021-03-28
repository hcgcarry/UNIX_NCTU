#include <iostream>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include<string.h>
#include <pwd.h>
#include <sys/types.h>
#include <fstream>
#include<set>
#include <vector>
#include<sstream>

using namespace std;

class argsClass
{

public:
    bool arg_c = false;
    string arg_c_value;
    bool arg_t = false;
    string arg_t_value;
    bool arg_f = false;
    string arg_f_value;

    argsClass() {}
    void getArgs(int argc, char *argv[])
    {

        int o;
        const char *optstr = "ctf";
        while ((o = getopt(argc, argv, optstr)) != -1)
        {
            switch (o)
            {
            case 'c':
                arg_c = true;
                arg_c_value = string(optarg);
                break;
            case 't':
                arg_t = true;
                arg_t_value = string(optarg);
                break;
            case '?':
                printf("error args\n");
                break;
            }
        }
    }
};
struct fileInfo {
    string COMMAND="noCmd";
    int PID=-1;
    string USER="noUser";
    string FD = "noFD";
    string TYPE = "noType";
    int NODE = -1;
    string NAME = "noName";
    friend ostream & operator<<(ostream &os,struct fileInfo fileInfoObj)
    {
        cout << "------ fileInfo------------" << endl;
        cout << fileInfoObj.COMMAND << " " << fileInfoObj.PID << " " << fileInfoObj.USER << " " << fileInfoObj.FD << " " << fileInfoObj.TYPE \
        << " " << fileInfoObj.NODE<< " " << fileInfoObj.NAME<< endl;
        cout << "------ fileInfoend------------" << endl;
    }
};
class parseProcess
{
public:
    int pid;
    parseProcess(){};
    parseProcess(int pid)
    {
        this->pid = pid;
    }
    void parse(vector<struct fileInfo> &fileInfoList)
    {
        string path = "/proc/" + to_string(pid);
        parseLink(path + "/cwd", "cwd",fileInfoList);
        parseLink(path + "/root", "root",fileInfoList);
        parseLink(path + "/exe", "exe",fileInfoList);
        parseMem(fileInfoList);
        parseFD(fileInfoList);
    }
    void printResult(){
        /*
        cout << "fileInfoList size: " <<fileInfoList.size() <<endl;
        for(auto item:fileInfoList){
            cout << item << endl;
        }
            for (int i=0;i< fileInfoList.size();i++)
            {
                cout << "i: " << i << endl;
                struct fileInfo tmp(fileInfoList[i]);
                cout << tmp.NAME << endl;
            }
        */
    }

    string findCmd()
    {
        string path = "/proc/" + to_string(pid) + "/cmdline";
        ifstream file(path.c_str());
        string cmd;
        file >> cmd;
        cout << "cmd: "<< cmd << endl;
        return cmd;
    }
    string findUserByUID(int uid)
    {
        struct passwd *user;
        user = getpwuid(uid);
        return string(user->pw_name);
    }
    int getUID()
    {
        string path = "/proc/" + to_string(pid);
        struct stat statBuf;
        if(stat(path.c_str(), &statBuf) <0 ){
            perror("getuid error");
        }
        return statBuf.st_uid;
    }
    void parseLink(string path, string fileName,vector<struct fileInfo> &fileInfoList)
    {
        cout <<"parselink-------------------------------"<<endl;
        //if(access(path.c_str(),F_OK)<0)return;

        struct stat statBuf;
        if (stat(path.c_str(), &statBuf) < 0)
        {
            cout << "path: " << path << endl;
            perror("parseLink,stat error");
        }

        char linkActualPath[100];
        if(readlink(path.c_str(),linkActualPath,sizeof(linkActualPath))<0){
            cout << "path: " << path << endl;
            perror("parseLink,read link error");
        }
        cout << getFileInfoObj(fileName,findFileType(statBuf),statBuf.st_ino,string(linkActualPath)) << endl;
    }
    void parseMem(vector<struct fileInfo>&fileInfoList)
    {

        string path = "/proc/" + to_string(pid) + "/maps";
        ifstream mapsFile(path);
        string line;
        set<int> inodeSet;
        while (getline(mapsFile, line))
        {
            stringstream ss(line);
            string region,permission, offset, dev,pathName;
            int inode;
            ss >> region >> permission >> offset >> dev >> inode >> pathName;
            if (inode != 0 && inodeSet.find(inode) == inodeSet.end())
            {
                inodeSet.insert(inode);
                struct stat statBuf;
                if (stat(pathName.c_str(), &statBuf) < 0)
                {
                    cout << "pathName" << pathName << endl;
                    perror("parseMem stat error");
                }
                fileInfoList.push_back(getFileInfoObj("mem", findFileType(statBuf), inode, pathName));
            }
        }
    }

    void parseFD(vector<struct fileInfo>&fileInfoList)
    {
        string path = "/proc/" + to_string(pid) + "/fd/";
        DIR *dp;
        struct dirent *dirp;
        if ((dp = opendir(path.c_str())) < 0)
        {
            cout << "path" << path << endl;
            perror("open dir faild");
        }
        while ((dirp = readdir(dp)) != NULL)
        {
            if(!strcmp(dirp->d_name,".") ||!strcmp(dirp->d_name, ".."))continue;
            struct stat statBuf;
            string curPath = path+ dirp ->d_name;
            if (stat(curPath.c_str(), &statBuf) < 0)
            {
                cout << "curPath: " << curPath << endl;
                perror("parseFD,stat error");
            }
            string fileNamePostFix;
            if ((S_IRUSR & statBuf.st_mode) && (S_IWUSR &statBuf.st_mode))
            {
                fileNamePostFix = "u";
            }
            if (S_IRUSR & statBuf.st_mode)
                fileNamePostFix = "r";
            else
                fileNamePostFix = "w";

            parseLink(curPath, dirp->d_name + fileNamePostFix,fileInfoList);
        }
        closedir(dp);
    }
    struct fileInfo getFileInfoObj(string FD, string TYPE, int NODE, string NAME)
    {
        struct fileInfo fileInfoObj;
        fileInfoObj.COMMAND = findCmd();
        fileInfoObj.PID = pid;
        fileInfoObj.USER = findUserByUID(getUID());
        fileInfoObj.FD = FD;
        fileInfoObj.TYPE = TYPE;
        fileInfoObj.NODE = NODE;
        fileInfoObj.NAME = NAME;
        cout << fileInfoObj << endl;
        //fileInfoList.push_back(fileInfoObj);
        /*
        cout << "filename" << fileName << endl;
        cout << "type" << findFileType(statBuf)<<endl;
        cout << "inode" << statBuf.st_ino;
        cout << "linkActualPath:" << linkActualPath << endl;
        */
    }
    string findFileType(struct stat& statBuf)
    {
        if (S_ISREG(statBuf.st_mode))
            return "REG";
        else if (S_ISDIR(statBuf.st_mode))
            return "DIR";
        else if (S_ISCHR(statBuf.st_mode))
            return "CHR";
        //if(S_ISBLK(statBuf->st_mode))  block device?
        else if (S_ISFIFO(statBuf.st_mode))
            return "FIFO";
        //if(S_ISLNK(statBuf->st_mode))  symbolic link?  (Not in POSIX.1-1996.)
        else if (S_ISSOCK(statBuf.st_mode))
            return "SOCK";
        
        else return "unknown type";
    }
};
class runClass
{
public:
    runClass(){
        getAllProcessFileInfo();
    };
    vector<struct fileInfo> fileInfoList;
    void printPrefix()
    {
        printf("COMMAND             	PID      USER      FD       TYPE      NODE      NAM\n");
    }
    void getAllProcessFileInfo()
    {
        DIR *dir;
        if ((dir = opendir("/proc")) == NULL)
        {
            
            perror("opendir error");
        }
        struct dirent *dirp;
        while ((dirp = readdir(dir)) != NULL)
        {
            if (checkStrIsNum(dirp->d_name))
            {
                parseProcess parseProcessObj(atoi(dirp->d_name));
               if(atoi(dirp->d_name) == 20301){
                    cout << "!!!!!!!!!!!!!!!!!!!!enter bash " << endl;
                parseProcessObj.parse(this->fileInfoList);
               }
               
            }
        }
    }
    bool checkStrIsNum(char *str)
    {
        for (int i = 0; str[i]; i++)
        {
            if (!isdigit(str[i]))
            {
                return false;
            }
        }
        return true;
    }
    void printAllResult()
    {
        struct fileInfo tmp;
        fileInfoList.push_back(tmp);
        printPrefix();
        for (auto &fileInfoObj: fileInfoList)
        {
            cout << "-------------------" << endl;
            cout << fileInfoObj.COMMAND << " " << fileInfoObj.PID << " " << fileInfoObj.USER << " " << fileInfoObj.FD << " " << fileInfoObj.TYPE \
            << " " << fileInfoObj.NODE<< " " << fileInfoObj.NAME<< endl;
        }
    }
};
int main(int argc, char *argv[])
{
    string processName = "bash";
    argsClass argsClassObj;
    runClass runClassObj;
    runClassObj;
    runClassObj.printAllResult();
}