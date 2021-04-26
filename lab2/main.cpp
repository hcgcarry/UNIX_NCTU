
#include "comm.h"
class argsClass
{

public:
    bool arg_o = false;
    string arg_o_value;
    bool arg_p = false;
    string arg_p_value;
    char ** cmdArgs;
    int argc;
    char **argv;
    argsClass(int argc,char** argv):argc(argc),argv(argv){
        getArgs();
        setOutputFile();
    }
    string getExePath(){
        if(argc < 2 ){
            printf("no command given.\n");
            exit(0);
        }
        return string(*cmdArgs);
    };
    void getArgs()
    {

        int o;
        const char *optstr = "o:p:";
        while ((o = getopt(argc, argv, optstr)) != -1)
        {
            switch (o)
            {
            case 'o':
                arg_o = true;
                arg_o_value = string(optarg);
                break;
            case 'p':
                arg_p = true;
                arg_p_value = string(optarg);
                break;
            default: 
              cout <<"usage: ./logger [-o file] [-p sopath] [--] cmd [cmd args ...]" <<endl;
                cout <<"-p: set the path to logger.so, default = ./logger.so"<<endl;
                cout <<"-o: print output to file, print to \"stderr\" if no file specified"<<endl;
                cout <<"--: separate the arguments for logger and for the command "<<endl;
                exit(0);
            }
        }
        cmdArgs = argv+optind;
    }
    string getSoPath(){
        if(!arg_p) return "./logger.so";
        return arg_p_value;
    }
    void setOutputFile(){
        if(!arg_o) return;
        setenv("OUTPUT_FILE",arg_o_value.c_str(),1);
    }
    
};
class inject{
    public:
    argsClass argsClassObj;
    char **argv ;
    inject(int argc,char** argv):argsClassObj(argc,argv){
    }
    void run(){
        printInfo();
        setenv("LD_PRELOAD",argsClassObj.getSoPath().c_str(),1);
        execvp(argsClassObj.getExePath().c_str(),argsClassObj.cmdArgs);
    }
    void printInfo(){
        printf("soPath: %s\n",argsClassObj.getSoPath().c_str());
        printf("exePath: %s\n",argsClassObj.getExePath().c_str());
        printf("outputFile: %s\n",argsClassObj.arg_o_value.c_str());
    }

};
int main(int argc,char* argv[]){
    inject injectObj(argc,argv);
    injectObj.run();
}