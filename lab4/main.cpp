
#include "comm.h"
#include <capstone/capstone.h>
#include "ptools.h"
#include"disasm.h"
#include<elf.h>
#include <iomanip>

class elfClass{
    public:
    long long entryPoint=-1;
    long long textSectionAddress=-1;
    long long textSectionSize=-1;
    string filePath;
    void init(string filePath){
        this->filePath = filePath;
    }
    long long getTextSectionAddress(){
        if(textSectionAddress == -1){
            readSection();
        }
        return textSectionAddress;
    }
    long long getTextSectionSize(){
        if(textSectionSize == -1){
            readSection();
        }
        return textSectionSize;
    }

    long long readEntryPoint(){
        ifstream in(filePath,ios::in | ios::binary);
        Elf64_Ehdr ehdr;
        in.read((char*)&ehdr,sizeof(Elf64_Ehdr));
        in.close();
        this->entryPoint = ehdr.e_entry;
        return ehdr.e_entry;
    }
    


    void readSection()
    {
            ifstream in(filePath,ios::in | ios::binary);
            Elf64_Ehdr ehdr;

            in.read((char*)&ehdr,sizeof(Elf64_Ehdr));

           
            int shnum, x;
            /// read shdr
            Elf64_Shdr *shdr = (Elf64_Shdr*)malloc(sizeof(Elf64_Shdr) * ehdr.e_shnum);
            in.seekg(ehdr.e_shoff,ios::beg); 
            in.read((char*)shdr, sizeof(Elf64_Shdr) * ehdr.e_shnum);
            /// read shstrtab
            in.seekg(shdr[ehdr.e_shstrndx].sh_offset,ios::beg);
            char shstrtab[shdr[ehdr.e_shstrndx].sh_size];
            char *names = shstrtab;
            in.read(shstrtab, shdr[ehdr.e_shstrndx].sh_size);

            for(shnum = 0; shnum < ehdr.e_shnum; shnum++)
            {
                    names=shstrtab+shdr[shnum].sh_name;
                    if(!strcmp(names,".text")){
                        textSectionAddress = shdr[shnum].sh_addr;
                        textSectionSize= shdr[shnum].sh_size;
                    }
                    //printf("%x\t%lx\t%lx\t%lx\t%s \n",shdr[shnum].sh_type,shdr[shnum].sh_addr,shdr[shnum].sh_offset,shdr[shnum].sh_size,names);
            }

    }
    bool checkIfAddressInTextRegion(long long address){
        readSection();
        printf("** textSectionAddress %llx,textsectionSize %llx\n", this->textSectionAddress,this->textSectionSize);
        if(address < this->textSectionAddress || address >= this->textSectionAddress + this->textSectionSize){
            return false;
        }
        return true;
    }
};

class disasmClass{
    public:
    csh cshandle = 0;
    map<long long, instruction1> instructions;
    //// diasm
    void init (pid_t childPid,long long textSectionStartAddress,long long textSectionSize){
        if (cs_open(CS_ARCH_X86, CS_MODE_64, &cshandle) != CS_ERR_OK){

        }
        for(long long addr = textSectionStartAddress;addr < textSectionStartAddress + textSectionSize;){
              int curInstrSize = disassemble(childPid,addr);
                addr+=curInstrSize;
        }
        cs_close(&cshandle);
    }
    void print_instruction(long long addr, instruction1 *in)
    {
        int i;
        char bytes[128] = "";
        if (in == NULL)
        {
            printf("%llx: \t<cannot disassemble>\n", addr);
        }
        else
        {
            for (i = 0; i < in->size; i++)
            {
                snprintf(&bytes[i * 3], 4, "%2.2x ", in->bytes[i]);
            }
            printf( "%llx: %-32s\t%-10s%s\n", addr, bytes, in->opr.c_str(), in->opnd.c_str());
        }
    }

    
    int getDisasInstr(unsigned long long rip){
        unsigned long long ptr = rip;
        map<long long, instruction1>::iterator mi; // from memory addr to instruction
        if ((mi = instructions.find(rip)) != instructions.end())
        {
            print_instruction(rip, &mi->second);
            return instructions[rip].size;
        }
        return -1;

    }
    int disassemble(pid_t proc, unsigned long long rip)
    {
        int count;
        char buf[64] = {0};
        unsigned long long ptr = rip;
        cs_insn *insn;
        map<long long, instruction1>::iterator mi; // from memory addr to instruction



        for (ptr = rip; ptr < rip + sizeof(buf); ptr += PEEKSIZE)
        {
            long long peek;
            errno = 0;
            peek = ptrace(PTRACE_PEEKTEXT, proc, ptr, NULL);
            if (errno != 0){
                perror("peektext");
                break;
            }
            memcpy(&buf[ptr - rip], &peek, PEEKSIZE);
        }

        /*
        if (ptr == rip)
        {
            print_instruction(rip, NULL);
            return -1;
        }
        */

        if ((count = cs_disasm(cshandle, (uint8_t *)buf, rip - ptr, rip, 0, &insn)) > 0)
        {
            int i;
            for (i = 0; i < count; i++)
            {
                instruction1 in;
                in.size = insn[i].size;
                in.opr = insn[i].mnemonic;
                in.opnd = insn[i].op_str;
                memcpy(in.bytes, insn[i].bytes, insn[i].size);
                instructions[insn[i].address] = in;
            }
            cs_free(insn, count);
        }


        return instructions[rip].size;
    }
    bool disasm(unsigned long long address,pid_t childPid,int numOfInstr,elfClass &elfClassObj)
    {

        if(!elfClassObj.checkIfAddressInTextRegion(address)){
            return false;
        }
        int wait_status;
        ////////// only to show which library current is in
        map<range_t, map_entry_t> m;
        map<range_t, map_entry_t>::iterator mi;


        //load_maps(childPid, m);
        unsigned long test_begin_address,test_end_address;
        for(int i=0;i<numOfInstr ;i++)
        {
            //fprintf(stderr, "0x%llx\n", regs.rip);
            //printf("loop i:%d  address :%llu\n",i,address);
            range_t r = {address, address};
            mi = m.find(r);
            if (mi == m.end())
            {
                m.clear();
                load_maps(childPid, m);
                mi = m.find(r);
            }
            int curInstrSize = getDisasInstr(address);
            address+=curInstrSize;
        }

        return true;
    }
};

class argsClass
{

public:
    bool arg_s = false;
    string arg_s_value;
    char **ChildCmdArgs=NULL;
    int argc;
    char **argv;
    argsClass() = delete;
    argsClass(int argc, char **argv) : argc(argc), argv(argv)
    {
        getArgs();
    }
    string getScriptName()
    {
        if (!arg_s)
            return "no scriptName provided";
        return arg_s_value;
    }
    char **getChildCmdArgs()
    {
        return ChildCmdArgs;
    }

    void getArgs()
    {

        int o;
        const char *optstr = "s:";
        while ((o = getopt(argc, argv, optstr)) != -1)
        {
            switch (o)
            {
            case 's':
                arg_s = true;
                arg_s_value = string(optarg);
                break;
            default:
                printf("usage: ./hw4 [-s script] binary file\n");
                printf("    -s: set up script\n ");
                exit(0);
            }
        }
        ChildCmdArgs = argv + optind;
    }
};

enum gdbState
{
    notLoaded,
    loaded,
    running
};
 void dump_code(unsigned long long addr, long long code) {
	fprintf(stderr, "**  %llx: code = %02x %02x %02x %02x %02x %02x %02x %02x\n",
		addr,
		((unsigned char *) (&code))[0],
		((unsigned char *) (&code))[1],
		((unsigned char *) (&code))[2],
		((unsigned char *) (&code))[3],
		((unsigned char *) (&code))[4],
		((unsigned char *) (&code))[5],
		((unsigned char *) (&code))[6],
		((unsigned char *) (&code))[7]);
}


struct breakPointInfo{
    unsigned long long address;
    unsigned long long code;
    breakPointInfo(){}
    breakPointInfo(unsigned long address , unsigned long code){
        this->address  = address;
        this->code = code;
    }
};
class breakPointClass
{
    public:
    vector<struct breakPointInfo> breakPointInfoList;
    bool lastStepIsBreakPoint = false;
    long long lastStepBreakPointAddr  ;
    
    void init(){
        breakPointInfoList.clear();
        lastStepIsBreakPoint = false;
    }
    
    void setBreakPoint(unsigned long long address,pid_t childPid){
        printf("** set break point address = 0x%llx\n", address);
		int status;
		unsigned long long code;

		code = ptrace(PTRACE_PEEKTEXT, childPid, address, 0);
		//dump_code(address , code);
		/* set break point */
		if(ptrace(PTRACE_POKETEXT, childPid, address, (code & 0xffffffffffffff00) | 0xcc) != 0)
			perror("ptrace(POKETEXT)");
        
        if(find_if(breakPointInfoList.begin(),breakPointInfoList.end(),[address](struct breakPointInfo breakPointObj){
            return breakPointObj.address == address;
        }) == breakPointInfoList.end()){
            
            breakPointInfoList.push_back(breakPointInfo(address,code));
        }

	}
    void contHandleBreakPoint(disasmClass& disasmClassObj,user_regs_struct &cur_user_regs_struct,breakPointInfo& breakPointInfoObj,pid_t childPid){
        restoreBreakPoint2OriginCode(childPid,breakPointInfoObj.address);
        printf("** breakpoint ");
        /* restore original context*/

        lastStepBreakPointAddr = breakPointInfoObj.address;

        disasmClassObj.getDisasInstr(breakPointInfoObj.address);
        /* restore rip*/
        cur_user_regs_struct.rip = cur_user_regs_struct.rip-1;
        if(ptrace(PTRACE_SETREGS, childPid, 0, &cur_user_regs_struct) != 0) perror("ptrace(SETREGS)");
    }
    void contCheckIFEncountBreakPoint(disasmClass& disasmClassObj,int status,pid_t childPid) {
        lastStepIsBreakPoint = false;
        if(!WIFSTOPPED(status)) return;
        //printf("** check if encountBreakpoint %d \n",childPid);
            
        struct user_regs_struct regs;
         if(ptrace(PTRACE_GETREGS, childPid, 0, &regs) !=0){
             perror("** check if encountbreakpoint ptrace_getregs\n");
         }
    //     long long rip;
    //     rip = ptrace(PTRACE_PEEKUSER,childPid,(&regs.rip-&regs.r15)*sizeof(long long),0);
    //         perror("** check if encountbreakpoint ptrace_getregs\n");

    // printf("rip %llx\n",rip);
        for(auto& breakPoint:breakPointInfoList){
            //printf("rip %llx breakpoint address  %llx\n",regs.rip , breakPoint.address);
            if(regs.rip -1 == breakPoint.address){
                lastStepIsBreakPoint = true;
                contHandleBreakPoint(disasmClassObj ,regs,breakPoint,childPid);
                break;
            }
        }
      
    }
    bool checkLastIsBreakPoint(){
        return lastStepIsBreakPoint;
    }
    bool checkAddressIsBreakPoint(long long address,pid_t childPid){
        for(auto& breakPoint:breakPointInfoList){
            if(address == breakPoint.address){
                return true;
            }
        }
        return false;
    }
    void contIFLastIsBreakPoint(pid_t childPid){
        if(lastStepIsBreakPoint && checkAddressIsBreakPoint(lastStepBreakPointAddr,childPid)){

            printf("** contlastStepIsBreakPoint, addr %llx\n",lastStepBreakPointAddr);
            setBreakPoint(lastStepBreakPointAddr,childPid);
        }
    }
    void singleStepIFNextStepISBreakPoint(disasmClass &disasmClassObj,long long address,pid_t childPid){
        lastStepIsBreakPoint = false;
        if(!checkAddressIsBreakPoint(address,childPid)){
            return;
        }
        lastStepBreakPointAddr = address;
        lastStepIsBreakPoint = true;

        breakPointInfo breakPointInfoObj;
        for(auto& breakPoint:breakPointInfoList){
            if(address == breakPoint.address){
                breakPointInfoObj = breakPoint;
                break;
            }
        }
        printf("** singleStepIFNextStepIsBrakPoint address %llx,lastStepBreakPointAddr %llx\n",address,lastStepBreakPointAddr);
        printf("** breakpoint ");
        disasmClassObj.getDisasInstr(breakPointInfoObj.address);

        /* restore original context*/
        restoreBreakPoint2OriginCode(childPid,address);
    }
    void singleStepIFLastStepIsBreakPoint(pid_t childPid){
        if(lastStepIsBreakPoint && checkAddressIsBreakPoint(lastStepBreakPointAddr,childPid)){
            printf("** singlelastStepIsBreakPoint, addr %llx\n",lastStepBreakPointAddr);
            setBreakPoint(lastStepBreakPointAddr,childPid);
        }
    }
    void list(){
        for(int i=0;i<breakPointInfoList.size();i++){
            printf(" %d: %llx\n",i,breakPointInfoList[i].address);
        }
    }
    void restoreBreakPoint2OriginCode(pid_t childPid,long long address ){
        breakPointInfo breakPointInfoObj;
        for(auto& breakPoint:breakPointInfoList){
            if(address == breakPoint.address){
                breakPointInfoObj = breakPoint;
                break;
            }
        }
        long long code;
        code = ptrace(PTRACE_PEEKTEXT, childPid,breakPointInfoObj.address, 0);
        printf("** restoreBreakPoint2OriginCode, current code %llx ,address %llx\n",code,address);

		/* set break point */
		if(ptrace(PTRACE_POKETEXT, childPid,breakPointInfoObj.address, (code & 0xffffffffffffff00) | (breakPointInfoObj.code & 0x00000000000000ff)) != 0)
			perror("ptrace(POKETEXT)");

    }
    void deleteBreakPoint(int index,pid_t childPid){
        auto breakPointIter= breakPointInfoList.begin()+index;
        restoreBreakPoint2OriginCode(childPid,breakPointIter->address);
        breakPointInfoList.erase(breakPointIter);
        printf("** breakpoint %d deleted.\n",index);
    }
};

class gdb
{
    public:
    breakPointClass breakPointClassObj;
    gdbState gdbstateObj ;
    disasmClass disasmClassObj;
    pid_t childPid;
    elfClass elfClassObj;
    string childName;
    char **childArgsPtr;
    string scriptName;
    gdb() = delete;
    gdb(argsClass &argsclassObj)
    {
        if(argsclassObj.arg_s){
            freopen(argsclassObj.getScriptName().c_str(),"r",stdin);
        }
        gdbstateObj = gdbState::notLoaded;
        char **childNamePtr;
        if ((childNamePtr = argsclassObj.getChildCmdArgs()) != NULL && *childNamePtr != NULL)
        {
            childName = string(*childNamePtr);
            load(childName);
        }
        scriptName = argsclassObj.getScriptName();
    }
   void iterReadInput(argsClass &argsclassObj)
    {
        char cmd[1000];
        if(!argsclassObj.arg_s) printf("sdb> ");
        while ( fgets(cmd, sizeof(cmd), stdin) !=NULL)
        {
            printf("** ---------------------------\n");
            string curCommand(cmd);
            parseCommand(curCommand);
            if(!argsclassObj.arg_s) printf("sdb> ");
        }
        exitGdb();
    }
    
    void dump(long long address){
            if(!elfClassObj.checkIfAddressInTextRegion(address)){
                return;
            }
            long ret;
            unsigned char *ptr = (unsigned char*)&ret;
            char asciiContent[16];
            for(int i=0;i<5;i++){
                ret = ptrace(PTRACE_PEEKTEXT, childPid, address+i*16, 0);
                printf("%llx: %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x", \ 
                address+i*16 , ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7]);
                for(int j=0;j<8;j++){
                    if(isprint(ptr[j])){
                        asciiContent[j] = ptr[j];
                    }
                    else asciiContent[j] = '.';
                }

                ret = ptrace(PTRACE_PEEKTEXT, childPid, address+8+i*16, 0);
                printf(" %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x ", \ 
                ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7]);
                for(int j=0;j<8;j++){
                    if(isprint(ptr[j])){
                        asciiContent[j+8] = ptr[j];
                    }
                    else asciiContent[j+8] = '.';
                }
                printf("|");
                for(int j=0;j<16;j++){
                    printf("%c",asciiContent[j]);
                }
                printf("|\n");

            }

    }
    bool checkIFChildExit(int status){
        if (WIFEXITED(status)) {
            printf("** child process %d terminiated normally (code %d)\n",childPid,status);
            gdbstateObj = gdbState::loaded;
            return true;
        }
        return false;

    }
    int vmmap(){
        char fn[128];
        char buf[256];
        FILE *fp;
        snprintf(fn, sizeof(fn), "/proc/%u/maps", childPid);
        if((fp = fopen(fn, "rt")) == NULL) return -1;
        while(fgets(buf, sizeof(buf), fp) != NULL) {
            int nargs = 0;
            char *token, *saveptr, *args[8], *ptr = buf;
            map_entry_t m;
            while(nargs < 8 && (token = strtok_r(ptr, " \t\n\r", &saveptr)) != NULL) {
                args[nargs++] = token;
                ptr = NULL;
            }
            if(nargs < 6) continue;
            if((ptr = strchr(args[0], '-')) != NULL) {
                *ptr = '\0';
                m.range.begin = strtol(args[0], NULL, 16);
                m.range.end = strtol(ptr+1, NULL, 16);
            }
            m.name = args[5];
            args[1][3] = '\0';
            string permission = string(args[1]);
            long inode= strtol(args[4], NULL, 16);
            printf("%016lx-%016lx %s %lx %s\n", m.range.begin, m.range.end, permission.c_str(), inode,m.name.c_str());
        }
    }
    void parseCommand(string cmd)
    {
        string cmdName;
        string args1,args2;
        stringstream ss(cmd);
        ss >> cmdName >> args1 >> args2;
        printf("** cmdName:%s,args1:%s\n",cmdName.c_str(),args1.c_str());
        if(cmdName == "b" || cmdName == "break"){
            if(gdbstateObj != gdbState::running){
                printf("** gdb state need to be running\n");
                return;
            }
            breakPointClassObj.setBreakPoint(stoi(args1,NULL,16),childPid);
        }
        else if(cmdName == "c" || cmdName == "cont"){
            if(gdbstateObj != gdbState::running){
                printf("** gdb state need to be running\n");
                return;
            }
            cont();

        }
        else if(cmdName == "delete"){
            if(gdbstateObj != gdbState::running){
                printf("** gdb state need to be running\n");
                return;
            }
            breakPointClassObj.deleteBreakPoint(stoi(args1),childPid);
        }
        else if(cmdName == "disasm" || cmdName == "d"){
            if(gdbstateObj != gdbState::running){
                printf("** gdb state need to be running\n");
                return;
            }
            if(args1 == "") {
                printf("** no addr given.\n");
                return;
            }
            long long address = stoll(args1,NULL,16);
            disasmClassObj.disasm(address,childPid,10,elfClassObj);
        }
        else if(cmdName == "dump" || cmdName == "x"){
            if(gdbstateObj != gdbState::running){
                printf("** gdb state need to be running\n");
                return;
            }
            if(args1 == "") {
                printf("** no addr given.\n");
                return;
            }
            long long address = stoll(args1,NULL,16);
            dump(address);
        }
        else if(cmdName == "exit" || cmdName == "q"){
            exitGdb();
        }
        else if(cmdName == "get" || cmdName == "g"){
            if(gdbstateObj != gdbState::running){
                printf("** gdb state need to be running\n");
                return;
            }
            getReg(args1);
        }
        else if(cmdName == "getregs"){
            if(gdbstateObj != gdbState::running){
                printf("** gdb state need to be running\n");
                return;
            }
            getregs();
        }
        else if(cmdName == "help" || cmdName == "h"){
            help();
        }
        else if(cmdName == "l" || cmdName == "list" || cmdName == "L"){
            breakPointClassObj.list();
        }
        else if (cmdName == "load" || cmdName == "Load")
        {
            if(gdbstateObj != gdbState::notLoaded){
                printf("** gdb state need to be notLoaded\n");
                return;
            }
            load(args1);
        }
        else if(cmdName == "run" || cmdName == "r"){
            if(gdbstateObj == gdbState::notLoaded){
                printf("** gdb state need to be loaded or running\n");
                return;
            }
            run();
        }
        else if(cmdName == "vmmap" || cmdName == "m"){
            if(gdbstateObj != gdbState::running){
                printf("** gdb state need to be running\n");
                return;
            }
            vmmap();
        }
        else if (cmdName == "set" || cmdName == "s"){
            if(gdbstateObj != gdbState::running){
                printf("** gdb state need to be running\n");
                return;
            }
            setReg(args1,stoll(args2,NULL,16));
        }
        else if (cmdName == "si")
        {
            if(gdbstateObj != gdbState::running){
                printf("** gdb state need to be running\n");
                return;
            }
            singleStep();
        }
        else if(cmdName == "start"){
            if(gdbstateObj == gdbState::notLoaded){
                printf("** gdb state need to be loaded\n");
                return;
            }
            start();
        }
    }

    bool singleStep()
    {
        if (ptrace(PTRACE_SINGLESTEP, this->childPid, 0, 0) < 0)
            perror("ptrace@parent");

        int status;
        waitpid(childPid, &status, 0);
        if(checkIFChildExit(status)){
            return true;
        }
        
        struct user_regs_struct regs;
        if(ptrace(PTRACE_GETREGS, childPid, 0, &regs) !=0){
            perror("** ptrace_getregs\n");
        }
        breakPointClassObj.singleStepIFLastStepIsBreakPoint(childPid);

    

        breakPointClassObj.singleStepIFNextStepISBreakPoint(disasmClassObj,regs.rip,childPid);
        return true;
        
    }
    void cont() {
        struct user_regs_struct regs;
        if(ptrace(PTRACE_GETREGS, childPid, 0, &regs) !=0){
            perror("** ptrace_getregs\n");
        }
        if(breakPointClassObj.checkLastIsBreakPoint()){
            
            if (ptrace(PTRACE_SINGLESTEP, this->childPid, 0, 0) < 0)
                perror("ptrace@parent");
            
            int status;
            waitpid(childPid, &status, 0);
            breakPointClassObj.contIFLastIsBreakPoint(childPid);
    
        }
        ptrace(PTRACE_CONT, childPid, NULL, NULL);
        int status;
        waitpid(childPid, &status, 0);
        if(checkIFChildExit(status)){
            return ;
        }
        breakPointClassObj.contCheckIFEncountBreakPoint(disasmClassObj,status,childPid);
        return ;
    }
    // don't care:              -1
    // hit breakpoint before:   0
    // hit breakpoint now:      1
    void run(){
        if(gdbstateObj == gdbState::running){
            printf("** program %s is already running.pid %d\n",childName.c_str(),childPid);
            cont();
        }
        else{
            gdbstateObj = gdbState::running;
            start();
            cont();
        }

    }

    bool load(string childName){
        gdbstateObj = gdbState::loaded;
        this->childName = childName;
        printf("** program '%s' is loaded.", childName.c_str());
        elfClassObj.init(childName);
        long long entryPoint = elfClassObj.readEntryPoint();
        printf(" entry point 0x%llx\n",entryPoint);
        return true;
    }
    void exitGdb(){
        printf("Bye.\n");
        exit(0);
    }
    void getReg(string registerName){
        unsigned long long reg;
        
        printf("** regName:%s , offset:%d\n",registerName.c_str(),registerName2Offset(registerName));
        reg = ptrace(PTRACE_PEEKUSER,childPid,registerName2Offset(registerName),0);
            printf("%s = %llu (0x%llx)\n",registerName.c_str(),reg,reg);
       
    }
    int registerName2Offset(string registerName) {
        struct user_regs_struct regs;
        map<string,int> result;
        result["rax"] = ((unsigned char*)&regs.rax) - ((unsigned char*)&regs.r15);
        result["rbx"] = ((unsigned char*)&regs.rbx) - ((unsigned char*)&regs.r15);
        result["rcx"] = ((unsigned char*)&regs.rcx) - ((unsigned char*)&regs.r15);
        result["rdx"] = ((unsigned char*)&regs.rdx) - ((unsigned char*)&regs.r15);
        result["rsp"] = ((unsigned char*)&regs.rsp) - ((unsigned char*)&regs.r15);
        result["rbp"] = ((unsigned char*)&regs.rbp) - ((unsigned char*)&regs.r15);
        result["rsi"] = ((unsigned char*)&regs.rsi) - ((unsigned char*)&regs.r15);
        result["rdi"] = ((unsigned char*)&regs.rdi) - ((unsigned char*)&regs.r15);
        result["rip"] = ((unsigned char*)&regs.rip) - ((unsigned char*)&regs.r15);
        result["r8"] = ((unsigned char*)&regs.r8) - ((unsigned char*)&regs.r15);
        result["r9"] = ((unsigned char*)&regs.r9) - ((unsigned char*)&regs.r15);
        result["r10"] = ((unsigned char*)&regs.r10) - ((unsigned char*)&regs.r15);
        result["r11"] = ((unsigned char*)&regs.r11) - ((unsigned char*)&regs.r15);
        result["r12"] =((unsigned char*)&regs.r12) - ((unsigned char*)&regs.r15);
        result["r13"] =  ((unsigned char*)&regs.r13) - ((unsigned char*)&regs.r15);
        result["r14"] =  ((unsigned char*)&regs.r14) - ((unsigned char*)&regs.r15);
        result["r15"] =  ((unsigned char*)&regs.r15) - ((unsigned char*)&regs.r15);
        result["flags"] =  ((unsigned char*)&regs.eflags) - ((unsigned char*)&regs.r15);
        return result[registerName];
       
    }
    bool start()
    {

        gdbstateObj = gdbState::running;
        breakPointClassObj.init();

        pid_t child;
        if ((child = fork()) < 0)
            perror("fork");
        char *argv[2];
        argv[0] = strdup(childName.c_str());
        argv[1] = NULL;
        if (child == 0)
        {
            if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0)
                perror("ptrace@child");
            execvp(argv[0], argv);
            perror("execvp");
        }
        else
        {
            int wait_status;
            if (waitpid(child, &wait_status, 0) < 0)
                perror("waitpid");
            ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);
            printf("** binary file:%s is start\n", argv[0]);
            printf("** pid %d\n",child);
            this->childPid = child;
            disasmClassObj.init(childPid,elfClassObj.getTextSectionAddress(),elfClassObj.getTextSectionSize());
            return WIFSTOPPED(wait_status);
        }
    }

    bool getregs(){
        struct user_regs_struct regs;
        if(ptrace(PTRACE_GETREGS, childPid, 0, &regs) == 0) {
            print_user_regs_struct(regs);
        }
        else{
            perror("PTRACE_getregs");
        }
        return true;
    }
    void print_user_regs_struct(struct user_regs_struct &regs){
        printf("RAX %llx                 RBX %llx                 RCX %llx                 RDX %llx\n\           
R8  %llx                 R9  %llx                 R10 %llx                 R11 %llx\n\
R12 %llx                 R13 %llx                 R14 %llx                 R15 %llx\n\
RDI %llx                 RSI %llx                 RBP %llx                 RSP %llx\n\
RIP %llx          FLAGS %016llx \n",\
                regs.rax,regs.rbx,regs.rcx,regs.rdx\
                ,regs.r8,regs.r9,regs.r10,regs.r11\
                ,regs.r12,regs.r13,regs.r14,regs.r15\
                ,regs.rdi,regs.rsi,regs.rbp,regs.rsp\
                ,regs.rip,regs.eflags);
    }
    void setReg(string regName,unsigned long long value){
        printf("** regName:%s int value:%llx , offset:%d\n",regName.c_str(),value,registerName2Offset(regName));
        ptrace(PTRACE_POKEUSER,childPid,registerName2Offset(regName),value);
        //printf("%s = %llu (0x%llx)\n",regName.c_str(),value,value);
    }
    void help(){
        printf("- break {instruction-address}: add a break point\n");
        printf("- cont: continue execution\n");
        printf("- delete {break-point-id}: remove a break point\n");
        printf("- disasm addr: disassemble instructions in a file or a memory region\n");
        printf("- dump addr [length]: dump memory content\n");
        printf("- exit: terminate the debugger\n");
        printf("- get reg: get a single value from a register\n");
        printf("- getregs: show registers\n");
        printf("- help: show this message\n");
        printf("- list: list break points\n");
        printf("- load {path/to/a/program}: load a program\n");
        printf("- run: run the program\n");
        printf("- vmmap: show memory layout\n");
        printf("- set reg val: get a single value to a register\n");
        printf("- si: step into instruction\n");
        printf("- start: start the program and stop at the first instruction\n");
    }

};

int main(int argc,char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);

    setvbuf(stderr, NULL, _IONBF, 0);
    argsClass argsClassObj(argc,argv);
    gdb gdbObj(argsClassObj);
    gdbObj.iterReadInput(argsClassObj);
}