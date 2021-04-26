#include <iostream>
#include <string>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <fstream>
#include <set>
#include <vector>
#include <sstream>
#include <bitset>
#include<regex>
#include<errno.h>
using namespace std;
struct fileInfo
{
    string COMMAND = "noCmd";
    int PID = -1;
    string USER = "noUser";
    string FD = "noFD";
    string TYPE = "noType";
    string NODE = "-1";
    string NAME = "noName";
    friend ostream &operator<<(ostream &os, struct fileInfo fileInfoObj)
    {
        cout << fileInfoObj.COMMAND << " " << fileInfoObj.PID << " " << fileInfoObj.USER << " " << fileInfoObj.FD << " " << fileInfoObj.TYPE
             << " " << fileInfoObj.NODE << " " << fileInfoObj.NAME ;
    }
};

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
        const char *optstr = "c:t:f:";
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
            case 'f':
                arg_f = true;
                arg_f_value = string(optarg);
                break;
            case '?':
                printf("error args\n");
                break;
            default: 
                break;
            }
        }
    }
    void typeFilter(vector<struct fileInfo>& fileInfoList){
        if(arg_t == false)return;
        set<string> validType = {"REG","CHR", "DIR", "FIFO", "SOCK", "unknown"};
        if(validType.find(string(arg_t_value)) == validType.end()){
            cout << "Invalid TYPE option."<< endl;
            exit(0);
        } 
        for(auto iter = fileInfoList.begin();iter!=fileInfoList.end();){
            if(iter->TYPE != arg_t_value) fileInfoList.erase(iter);
            else iter++;
        }
    }
    void regexFilterCommand(vector<struct fileInfo>& fileInfoList){
        if(arg_c == false)return;
        string str(arg_c_value);
        regex re(str);
        for(auto iter = fileInfoList.begin();iter!=fileInfoList.end();){
            smatch sm;
            if(!regex_search(iter->COMMAND,sm,re)){
                fileInfoList.erase(iter);
            }
            else iter++;
        }
    }
    void regexFilterFileName(vector<struct fileInfo>& fileInfoList){
        if(arg_f == false)return;
        string str(arg_f_value);
        regex re(str);
        for(auto iter = fileInfoList.begin();iter!=fileInfoList.end();){
            smatch sm;
            stringstream ss(iter->NAME);
            string nameWithoutErrorMess ;
            ss >> nameWithoutErrorMess;
            if(!regex_search(nameWithoutErrorMess,sm,re)){
                fileInfoList.erase(iter);
            }
            else iter++;
        }
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
        parseLink(path + "/cwd", "cwd", fileInfoList);
        parseLink(path + "/root", "root", fileInfoList);
        parseLink(path + "/exe", "exe", fileInfoList);
        parseMem(fileInfoList);
        parseFD(fileInfoList);
    }

    string findCmd()
    {
        string path = "/proc/" + to_string(pid) + "/comm";
        ifstream file(path.c_str());
        string cmd;
        file >> cmd;
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
        if (stat(path.c_str(), &statBuf) < 0)
        {
            #ifdef debug
            cout << "path: " << path << endl;
            perror("getuid error");
            #endif
        }
        return statBuf.st_uid;
    }
    void parseLink(string path, string fileName, vector<struct fileInfo> &fileInfoList)
    {
        //cout << "parselink-------------------------------" << endl;

        struct stat statBuf;
        if (stat(path.c_str(), &statBuf) < 0)
        {
            // file not exist
            //如果這個path的根本就沒存取權限 那他只會permission denied,不會ENOENT
            if(errno == ENOENT)
            return;
        }

        char linkActualPath[100];
        int readlinkNameNum=0;
        if ((readlinkNameNum = readlink(path.c_str(), linkActualPath, sizeof(linkActualPath))) < 0)
        {
            #ifdef debug
            cout << "path: " << path << endl;
            perror("parseLink,read link error");
            #endif
        }
        linkActualPath[readlinkNameNum] = '\0';
        struct fileInfo item ;
        string linkActualPathTmp = string(linkActualPath);
        if(linkActualPathTmp.find("(deleted)") !=linkActualPathTmp.npos){
                fileInfoList.push_back( getFileInfoObj(fileName, "unknown", to_string(statBuf.st_ino),linkActualPathTmp ));
                return;
        }
        if(access(path.c_str(),R_OK)<0){
                fileInfoList.push_back( getFileInfoObj(fileName, "unknown", "", path +" (readlink: Permission denied)"));
                return;
        }

        fileInfoList.push_back( getFileInfoObj(fileName, findFileType(statBuf), to_string(statBuf.st_ino), string(linkActualPath)));
    }
    void parseMem(vector<struct fileInfo> &fileInfoList)
    {

        //cout << "parseMem-----------------------------" << endl;
        string path = "/proc/" + to_string(pid) + "/maps";
        if(access(path.c_str(), R_OK)<0 ){
            return;
        }

        ifstream mapsFile(path);
        string line;
        set<int> inodeSet;
        while (getline(mapsFile, line))
        {
            stringstream ss(line);
            string region, permission, offset, dev, pathName,errorMessage;
            int inode;
            ss >> region >> permission >> offset >> dev >> inode >> pathName>>errorMessage;
            if (inode != 0 && inodeSet.find(inode) == inodeSet.end())
            {
                inodeSet.insert(inode);
                struct stat statBuf;
                if (stat(pathName.c_str(), &statBuf) < 0)
                {
                    #ifdef debug
                    cout << "pathName" << pathName << endl;
                    perror("parseMem stat error");
                    #endif
                }
                if(errorMessage.find("deleted") != pathName.npos){
                    fileInfoList.push_back(getFileInfoObj("del", "unknown", to_string(inode), pathName+" (deleted)"));
                }
                else
                    fileInfoList.push_back(getFileInfoObj("mem", findFileType(statBuf), to_string(inode), pathName));
            }
        }
    }

    void parseFD(vector<struct fileInfo> &fileInfoList)
    {
        //cout << "parseFD-----------------------" << endl;
        string path = "/proc/" + to_string(pid) + "/fd";
        if(access(path.c_str(), R_OK)<0 ){
            fileInfoList.push_back(getFileInfoObj("NOFD", "", "" , path+ " (opendir: Permission denied)"));
            return;
        }

        DIR *dp;
        struct dirent *dirp;
        if ((dp = opendir(path.c_str())) < 0)
        {
            #ifdef debug
            cout << "path" << path << endl;
            perror("open dir faild");
            #endif
        }
        while ((dirp = readdir(dp)) != NULL)
        {
            if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
                continue;
            struct stat statBuf;
            string curPath = path +"/"+ dirp->d_name;
            if (stat(curPath.c_str(), &statBuf) < 0)
            {
                #ifdef debug
                cout << "curPath: " << curPath << endl;
                perror("parseFD,stat error");
                #endif
            }
            string fileNamePostFix;
            ifstream fdinfo("/proc/"+to_string(pid)+"/fdinfo/"+dirp->d_name);
            string readWriteMode;
            fdinfo >> readWriteMode;
            fdinfo >> readWriteMode;
            fdinfo >> readWriteMode;
            fdinfo >> readWriteMode;
            int flag = stoi(readWriteMode,0,8);
            if (flag%4 == 0 ) 
            {
                fileNamePostFix = "r";
            }
            else if (flag & O_WRONLY)
                fileNamePostFix = "w";
            else
                fileNamePostFix = "u";

            parseLink(curPath, dirp->d_name + fileNamePostFix, fileInfoList);
        }
        closedir(dp);
    }
    fileInfo getFileInfoObj(string FD, string TYPE, string NODE, string NAME)
    {
        struct fileInfo fileInfoObj;
        fileInfoObj.COMMAND = findCmd();
        fileInfoObj.PID = pid;
        fileInfoObj.USER = findUserByUID(getUID());
        fileInfoObj.FD = FD;
        fileInfoObj.TYPE = TYPE;
        fileInfoObj.NODE = NODE == "-1" ?"":NODE;
        fileInfoObj.NAME = NAME;
        return fileInfoObj;
    }
    string findFileType(struct stat &statBuf)
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

        else
            return "unknown";
    }
};
class runClass
{
public:
    runClass()
    {
        getAllProcessFileInfo();
    };
    vector<struct fileInfo> fileInfoList;
    void printPrefix()
    {
        printf("COMMAND             	PID      USER      FD       TYPE      NODE      NAME\n");
    }
    void getAllProcessFileInfo()
    {
        DIR *dir;
        if ((dir = opendir("/proc")) == NULL)
        {

            #ifdef debug
            perror("opendir error");
            #endif
        }
        struct dirent *dirp;
        // get pid list
        vector<int> PIDList;
        while ((dirp = readdir(dir)) != NULL)
        {
            if (checkStrIsNum(dirp->d_name))
            {
                PIDList.push_back(atoi(dirp->d_name));
            }
        }
        for(auto PID:PIDList){
                parseProcess parseProcessObj(PID);
                #ifdef debug
                if (atoi(dirp->d_name) == 10319)
                {
                #endif 
                    parseProcessObj.parse(this->fileInfoList);
                #ifdef debug
                }
                #endif
        }
        /*
        sort(fileInfoList.begin(),fileInfoList.end(),[](struct fileInfo& fileInfo1,struct fileInfo& fileInfo2){
            return fileInfo1.PID < fileInfo2.PID;
        });
        */
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
    void printResult()
    {
        printPrefix();
        for (auto &fileInfoObj : fileInfoList)
        {
            cout << fileInfoObj << endl;
        }
    }
    void printResultWithFlag(int argc,char* argv[]){
        argsClass argsClassObj;
        argsClassObj.getArgs(argc,argv);
        argsClassObj.typeFilter(fileInfoList);
        argsClassObj.regexFilterCommand(fileInfoList);
        argsClassObj.regexFilterFileName(fileInfoList);
        printResult();
    }
};
int main(int argc, char *argv[])
{
    string processName = "bash";
    runClass runClassObj;
    runClassObj.printResultWithFlag(argc,argv);
    //runClassObj.printAllResult();
}
