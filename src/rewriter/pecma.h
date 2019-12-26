#ifndef PECMA_H
#define PECMA_H

#include "../common/common.h"

using namespace std;

class Pecma {
public:
    string program; //source file name
    string mutantsPath;  //path of mutant files
    string programPath;  //path of source file
    string rulePath;  //path of rule file
    string blockContextPath;
    string subBlockPath;
    string testcasePath; //path of test cases dir
    string rootPath;  //workspace path
    string programfileName;  //program name without extention
    string fileNameForCopy;  //destination file name to copy
    vector<pair<int,int>>ruleList;  //rule pair
    vector<pair<int,int>>subBlockList; //subblock pair
    vector<pair<int,int>>blockContextList;
    vector<pair<int,int>>returnStmtList;
    vector<string>sourceList;  //context of source file
    vector<string>mutantsList;  //context of mutant file
    vector<string>mutantsDir;  //names of mutants
    vector<string>blockDir;  //every block dir,
    vector<string>blockSub;  //mutant names in every block dir
public:
    Pecma(string configPath,string programName);

    inline void cutPath(string& path, string kind){
        auto pos=path.find(kind);
        path=path.substr(pos+1,path.size()-pos-1);
    }
    void createWorkspace();
    void division();
    //read the mutantdir
    void fileTrav(std::string,vector<string>&);
    //read source file
    void readSub();
    //read the rulefile
    void readRule(string path,vector<pair<int,int>>&destList);

    //copy the mutants
    void copyMutatnts(std::string);

    //read the mutantsfile
    void readMutation(std::string);
    //traversal the String
    void compareFile(std::string);
    //find the block
    int block(int);
    //create the Mkdir
    void createMkdir(std::string);
    //copy files
    void copyFile(string, string);
    //begin to rewrite the program
    void precombine();
    //extrace the variable and function name and parameters
    int extractexp(int a,int b,vector<var_ctx>*block_var,vector<func_ctx>*block_func,vector<var_ctx>*block_decl,string programname);
    void writeConcurrent(ofstream& blockfile,
                        int blockStartLine,int blockEndLine,
                        string filename,
                        int mode,
                        string mutantLabelName="");
    //combine different mutant of one block, the third is jiajingting version.
    void cwmtCombine(int);
    void wmtCombine();
    void cmtCombine();
    //indent -kr -i8
    void krCodeStyle(string);
    //write shell
    void cwmtWriteShell();
    void wmtWriteShell();
    void cmtWriteShell();
};

#endif
