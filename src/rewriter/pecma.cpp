#include "../ast/mutator.h"
#include "pecma.h"
#include "../common/json.hpp"
void trim(string &s)
{
    /*
    if( !s.empty() )
    {
        s.erase(0,s.find_first_not_of(" "));
        s.erase(s.find_last_not_of(" ") + 1);
    }
    */
    int index = 0;
    if (!s.empty())
    {
        while ((index = s.find(' ', index)) != string::npos)
        {
            s.erase(index, 1);
        }
    }
}

Pecma::Pecma(string configPath, string programName)
{
    ifstream configFile(configPath, ifstream::in);
    if (configFile.fail())
    {
        cerr << "open config file error!" << endl;
    }
    nlohmann::json configJson;
    configFile >> configJson;
    string line;
    rootPath = configJson["object"][programName]["rootPath"];
    if (rootPath[rootPath.size() - 1] == '/')
    {
        rootPath += "Pecwma";
    }
    else
    {
        rootPath += "/Pecwma";
    }
    //get programPath,program,programfileName
    programPath = configJson["object"][programName]["programPath"];
    auto pos = programPath.rfind("/");
    program = programPath.substr(pos + 1, programPath.size() - pos - 1);
    pos = program.rfind(".");
    programfileName = program.substr(0, pos);
    //get mutantsPath not end with '/'
    mutantsPath = configJson["object"][programName]["mutantsPath"];
    if (mutantsPath[mutantsPath.size() - 1] == '/')
    {
        mutantsPath = mutantsPath.substr(0, mutantsPath.size() - 1);
    }
    system(("export C_INCLUDE_PATH=$C_INCLUDE_PATH:" + programPath.substr(0, pos)).c_str());
    //get rulePath
    rulePath = rootPath + "/" + programName + "/Rule/blockRule.txt";
    // copyFile(rulePath,rootPath+"/"+programfileName+"/Source");
    //get block context path
    blockContextPath = rootPath + "/" + programName + "/Rule/blockContext.txt";
    // copyFile(blockContextPath,rootPath+"/"+programfileName+"/Source");
    //get subblock
    subBlockPath = rootPath + "/" + programName + "/Rule/subBlock.txt";
    // copyFile(blockForContextPath,rootPath+"/"+programfileName+"/Source");
    //get testcasePath not end with '/'
    testcasePath = configJson["object"][programName]["testcasePath"];
    if (testcasePath[testcasePath.size() - 1] == '/')
    {
        testcasePath = testcasePath.substr(0, testcasePath.size() - 1);
    }
#ifdef _DEBUG
    cout << rootPath << endl
         << programPath << endl
         << mutantsPath << endl
         << rulePath << endl
         << testcasePath << endl;
#endif
    //make workspace
    createWorkspace();
    string inputPath = programPath;
    int position = inputPath.rfind('/');
    inputPath = inputPath.substr(0, position);
    pos = inputPath.rfind('/');
    inputPath = inputPath.substr(0, position);
    system(("if [ -e " + inputPath + "/inputs" + " ];then cp -r " + inputPath + "/inputs" + " " + rootPath + "/" + programfileName + "\nfi").c_str());
    system(("cp " + testcasePath + "/* " + rootPath + "/" + programfileName + "/Testcase").c_str());
}

void Pecma::createWorkspace()
{
    //make workspace
    createMkdir(rootPath);
    createMkdir(rootPath + "/" + programfileName + "/Source/");
    createMkdir(rootPath + "/" + programfileName + "/Rule/");
    createMkdir(rootPath + "/" + programfileName + "/Mutation/");
    createMkdir(rootPath + "/" + programfileName + "/Mutant/");
    createMkdir(rootPath + "/" + programfileName + "/Testcase/");

    createMkdir(rootPath + "/" + programfileName + "/CMT/");
    createMkdir(rootPath + "/" + programfileName + "/CMT/Block/");
    createMkdir(rootPath + "/" + programfileName + "/CMT/Scripts/");
    createMkdir(rootPath + "/" + programfileName + "/CMT/Result/temp");

    createMkdir(rootPath + "/" + programfileName + "/WMT/");
    createMkdir(rootPath + "/" + programfileName + "/WMT/Block/");
    createMkdir(rootPath + "/" + programfileName + "/WMT/Scripts/");
    createMkdir(rootPath + "/" + programfileName + "/WMT/Result/temp/");

    createMkdir(rootPath + "/" + programfileName + "/CWMT/");
    createMkdir(rootPath + "/" + programfileName + "/CWMT/Block/");
    createMkdir(rootPath + "/" + programfileName + "/CWMT/Scripts/");
    createMkdir(rootPath + "/" + programfileName + "/CWMT/Result/temp/");
}

//create the Mkdir
void Pecma::createMkdir(std::string destDirName)
{
    system(("mkdir -p " + destDirName).c_str());
}

int Pecma::extractexp(int a, int b, vector<var_ctx> *block_var, vector<func_ctx> *block_func, vector<var_ctx> *block_decl, string programname)
{
#ifdef _DEBUG
    cout << programname << endl;
#endif
    block_var->clear();
    block_func->clear();
    block_decl->clear();
    // CompilerInstance will hold the instance of the Clang compiler for us,
    // managing the various objects needed to run the compiler.
    CompilerInstance TheCompInst;
    TheCompInst.createDiagnostics();

    LangOptions &lo = TheCompInst.getLangOpts();
    // lo.CPlusPlus = 1;
    // lo.GNUMode = 1;
    // lo.CXXExceptions = 1;
    // lo.RTTI = 1;
    // lo.Bool = 1;

    // Initialize target info with the default triple for our platform.
    auto TO = std::make_shared<clang::TargetOptions>();
    TO->Triple = llvm::sys::getDefaultTargetTriple();
    TargetInfo *TI = TargetInfo::CreateTargetInfo(TheCompInst.getDiagnostics(), TO);
    TheCompInst.setTarget(TI);

    TheCompInst.createFileManager();
    FileManager &FileMgr = TheCompInst.getFileManager();
    TheCompInst.createSourceManager(FileMgr);
    SourceManager &SourceMgr = TheCompInst.getSourceManager();

    HeaderSearchOptions &headerSearchOptions = TheCompInst.getHeaderSearchOpts();
    headerSearchOptions.AddPath("/usr/local/lib/clang/8.0.0/include", clang::frontend::Angled, false, false);
    headerSearchOptions.AddPath("/usr/include", clang::frontend::Angled, false, false);
    headerSearchOptions.AddPath("/usr/local/include", clang::frontend::Angled, false, false);
    headerSearchOptions.AddPath("/usr/include/x86_64-linux-gnu", clang::frontend::Angled, false, false);
    headerSearchOptions.AddPath("/usr/include/c++/5.4.0", clang::frontend::Angled, false, false);
    headerSearchOptions.AddPath("/usr/include/c++/5.4.0/backward", clang::frontend::Angled, false, false);
    auto pos = programPath.rfind("/");
    auto sourceHeader = programPath.substr(0, pos);
    headerSearchOptions.AddPath(sourceHeader, clang::frontend::Angled, false, false);

    TheCompInst.createPreprocessor(clang::TU_Module);
    TheCompInst.createASTContext();

    // Set the main file handled by the source manager to the input file.
    const FileEntry *FileIn = FileMgr.getFile(programname);
    SourceMgr.setMainFileID(SourceMgr.createFileID(FileIn, SourceLocation(), SrcMgr::C_User));
    TheCompInst.getDiagnosticClient().BeginSourceFile(TheCompInst.getLangOpts(), &TheCompInst.getPreprocessor());

    // Create an AST consumer instance which is going to get called by
    // ParseAST.
    MutatorASTConsumer TheConsumer(&TheCompInst, "ese", "", a, b, block_var, block_func, block_decl);

    // Parse the file to AST, registering our consumer as the AST consumer.
    ParseAST(TheCompInst.getPreprocessor(), &TheConsumer, TheCompInst.getASTContext());

    return 0;
}

//divide mutant into different groups
void Pecma::division()
{
#ifdef _DEBUG
    cout << "division begin!" << endl;
#endif
    readRule(rulePath, ruleList);
#ifdef _DEBUG
    cout << "division begin!2" << endl;
#endif
    string fpath = rootPath + "/" + programfileName + "/Mutation";
    fileTrav(mutantsPath, mutantsDir);
#ifdef _DEBUG
    cout << "division begin!3" << endl;
#endif
    readSub();
    if (mutantsDir.size() != 0)
    {
        for (int i = 0; i < mutantsDir.size(); i++)
        {
            readMutation(mutantsDir[i]);
            compareFile(mutantsDir[i]);
        }
    }
#ifdef _DEBUG
    cout << "division over!" << endl;
#endif
}

//read the mutant directory
void Pecma::fileTrav(std::string path, vector<string> &Dir)
{
#ifdef _DEBUG
    cout << path << " filetrav" << endl;
#endif
    Dir.clear();
    //get all file paths in the directory
    auto dirp = opendir(path.c_str());
    struct dirent *dp;
    while ((dp = readdir(dirp)) != NULL)
    {
        if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, ".."))
        {
            Dir.push_back(dp->d_name);
        }
    }
    (void)closedir(dirp);
    // #ifdef _DEBUG
    //     for(auto it=Dir.begin();it!=Dir.end();it++) {
    //         cout<<*it<<endl;
    //     }
    // #endif
}

//read source file
void Pecma::readSub()
{
#ifdef _DEBUG
    cout << programPath << " read source file!" << endl;
    cout << "cp " + programPath + " " + rootPath + "/" + programfileName + "/Source/" + program << endl;
#endif
    copyFile(programPath, rootPath + "/" + programfileName + "/Source/" + program);
    sourceList.clear();
    ifstream sourcefile(programPath, ifstream::in);
    if (sourcefile.fail())
    {
        cout << "source file open error\n";
        return;
    }
    string tmp;
    while (getline(sourcefile, tmp))
    {
        sourceList.push_back(tmp);
    }
    sourcefile.close();
}

//read the rulefile
void Pecma::readRule(string path, vector<pair<int, int>> &destList)
{
#ifdef _DEBUG
    cout << " read rule file: " << path << endl;
#endif
    std::ifstream rulefile(path, ifstream::in);
    if (rulefile.fail())
    {
        cout << "rule file open error\n";
        return;
    }
    string rule;
    int count = 0;
    while (rulefile >> rule)
    {
        count++;
        string delim = "~";              //fen ge fu
        auto pos = rule.find(delim);     //pos为分隔符第一次出现的位置，从i到pos之前的字符串是分隔出来的字符串
        string s1 = rule.substr(0, pos); //*****从i开始长度为pos-i的子字符串
        string s2 = rule.substr(pos + 1, rule.size() - 1 - pos);
        int kai = atoi(s1.c_str()), jie = atoi(s2.c_str());
        destList.push_back(make_pair(kai, jie)); //两个连续空格之间切割出的字符串为空字符串，这里没有判断s是否为空，所以最后的结果中有空字符的输出，
    }
    rulefile.close();
#ifdef _DEBUG
    // for(auto it=ruleList.begin();it!=ruleList.end();it++) {
    //     cout<<(*it).first<<" "<<(*it).second<<endl;
    // }
#endif
}

//copy the mutants
void Pecma::copyMutatnts(std::string path)
{
#ifdef _DEBUG
    cout << path << " copymutants" << endl;
    cout << fileNameForCopy << " fileNameForCopy" << endl;
#endif
    ifstream myfile(path);
    if (myfile.fail())
    {
        cout << "copy file open error\n";
        return;
    }
    ofstream myname(fileNameForCopy, ofstream::out);
    if (myname.fail())
    {
        cout << "fileNameForCopy file open error\n";
        return;
    }
    string line;
    while (getline(myfile, line))
    {
        myname << line << endl;
    }
    myfile.close();
    myname.close();
}

//read the mutants file
void Pecma::readMutation(string path)
{
    path = mutantsPath + "/" + path;
    mutantsList.clear();
    // #ifdef _DEBUG
    //     cout<<path<<" readmutation"<<endl;
    // #endif
    ifstream myfile(path);
    if (myfile.fail())
    {
        cout << "read mutation file open error\n";
        return;
    }
    string line;
    while (getline(myfile, line))
    {
        mutantsList.push_back(line);
    }
    myfile.close();
}

//compare mutant and source file to get which line is different
void Pecma::compareFile(std::string mutantName)
{
    int counts = 0, countb = 0;
    // createMkdir(rootPath+"/"+programfileName+"/Block");
    while (counts < sourceList.size())
    {
        string str1 = sourceList.at(counts);
        //std::replace_if(str1.begin(),str1.end(),[](char ch){return std::isspace(ch)||ch=='\n';}, '\0');
        //auto it1=remove_if(str1.begin(),str1.end(),[](char a){return a==' '||a=='\n';});
        //str1.erase(it1,str1.end());
        string str2 = mutantsList.at(counts);
        //std::replace_if(str2.begin(), str2.end(),[](char ch){return std::isspace(ch)||ch=='\n';}, '\0');
        //auto it2=remove_if(str2.begin(),str2.end(),[](char a){return a==' '||a=='\n';});
        //str2.erase(it2,str2.end());
        trim(str1);
        trim(str2);
        if (str1.compare(str2) != 0)
        {
#ifdef _DEBUG
            cout << counts << endl;
            cout << str1 << endl;
            cout << str2 << endl;
#endif
            countb = block(counts + 1); //countb maybe 0, this is rulefile error
            createMkdir(rootPath + "/" + programfileName + "/Mutation/Block" + to_string(countb));
            //createMkdir("MyWorkSpace/"+programfileName+"/Block/Block"+to_string(countb));
            //fileNameForCopy=rootPath+"/"+programfileName+"/Mutation/Block"+to_string(countb)+"/"+mutantName;
            copyFile(mutantsPath + "/" + mutantName, rootPath + "/" + programfileName + "/Mutation/Block" + to_string(countb));
            break;
        }
        counts++;
        if (counts == sourceList.size())
        { //source file is equal to mutant file
            createMkdir(rootPath + "/" + programfileName + "/Mutation/Block" + to_string(0));
            //createMkdir("MyWorkSpace/"+programfileName+"/Block/Block"+to_string(0));
            //fileNameForCopy=rootPath+"/"+programfileName+"/Mutation/Block"+to_string(0)+"/"+mutantName;
            copyFile(mutantsPath + "/" + mutantName, rootPath + "/" + programfileName + "/Mutation/Block" + to_string(0));
        }
    }
}

//find the block that the mutant belongs to
int Pecma::block(int row)
{
    int block = 0;
    while (block < ruleList.size())
    {
        auto rule = ruleList.at(block);
        if (row >= rule.first && row <= rule.second)
        {
            return block + 1;
        }
        block++;
    }
    cout << "block " << row << "not in rulefile" << endl;
    return 0;
}

void Pecma::copyFile(string filename, string destName)
{
    system(("cp -rf " + filename + " " + destName).c_str());
}

void Pecma::writeConcurrent(ofstream &blockfile,
                            int blockStartLine, int blockEndLine,
                            string filename,
                            int mode,
                            string mutantLabelName)
{
    vector<var_ctx> *block_var = new vector<var_ctx>();
    vector<func_ctx> *block_func = new vector<func_ctx>();
    vector<var_ctx> *block_decl = new vector<var_ctx>();
    //write the status of mutant program
    extractexp(blockStartLine, blockEndLine, block_var, block_func, block_decl, filename);
    //variables name of declared between block beginline and endline
    set<string> block_decl_mp;
    set<string> block_var_mp;
    set<string> block_func_mp;
    set<string> block_func_para_mp;
    for (int i = 0; i < (*block_decl).size(); i++)
    {
        block_decl_mp.insert((*block_decl)[i].name);
    }

    if (mode == 0)
    {
        blockfile << "PECWMA_OUTPUT_Source:" << endl;
    }
    else if (mode == 1)
    {
        blockfile << mutantLabelName + ":" << endl;
    }

    blockfile << "{" << endl;
    blockfile << "if(try_to_be_unique_flag==0){" << endl;
    blockfile << "fseek(output_ptr,0,SEEK_SET);" << endl;
    blockfile << "try_to_be_unique_flag=1;" << endl;
    blockfile << "}" << endl;
    // blockfile<<"fwrite(&(try_to_be_unique_before),sizeof(try_to_be_unique_before),1,output_ptr);"<<endl;
    //write assignment variables
    blockfile << "char* try_to_be_unique_varname;" << endl;
    for (auto i = block_var->begin(); i != block_var->end(); i++)
    {
        if (block_var_mp.find(i->name) != block_var_mp.end())
        {
            continue;
        }
        block_var_mp.insert(i->name);
        if (block_decl_mp.find(i->name) != block_decl_mp.end())
        {
            continue;
        }
        // string tmp=i->name;
        // if(tmp.find("\"")!=string::npos)
        // {
        //     tmp=tmp.replace(tmp.find("\""),1,"\\\"");
        //     // tmp=tmp.replace(tmp.rfind("\""),1,"\\\"");
        // }
        // blockfile<<"try_to_be_unique_varname=\""<<tmp<<"\";"<<endl;
        // blockfile<<"fwrite(&(try_to_be_unique_varname),sizeof(try_to_be_unique_varname),1,output_ptr);"<<endl;
        // if(i->name.find("\"")==string::npos)
        // {
        //     blockfile<<"try_to_be_unique_varname=\""<<i->name<<"\";"<<endl;
        //     blockfile<<"fwrite(&(try_to_be_unique_varname),sizeof(try_to_be_unique_varname),1,output_ptr);"<<endl;
        // }
        if (atoi((i->name).c_str()) == 0 &&
            (i->name) != "0" &&
            (i->name).find('+') == string::npos &&
            (i->name).find('-') == string::npos &&
            (i->name).find('*') == string::npos &&
            (i->name).find('/') == string::npos &&
            (i->name).find('=') == string::npos &&
            (i->name).find(',') == string::npos &&
            (i->name).find('%') == string::npos &&
            (i->name).find('&') == string::npos &&
            (i->name).find('|') == string::npos &&
            (i->name).find('\'') == string::npos &&
            (i->name).find('"') == string::npos &&
            (i->name).find('^') == string::npos)
            ;
        if ((i->name)[0] != '%')
        {
            blockfile << "fwrite(&(" << i->name << "),sizeof(" << i->name << "),1,output_ptr);" << endl;
        }
        else
        {
            replace_if(i->name.begin(), i->name.end(), [](char ch) { return ch == '"'; }, ' ');
            blockfile << "try_to_be_unique_varname=\"" << i->name << "\";" << endl;
            blockfile << "fwrite(&(try_to_be_unique_varname),sizeof(try_to_be_unique_varname),1,output_ptr);" << endl;
        }
    }
    //write called function and it's parameters
    blockfile << "char* try_to_be_unique_funcname;" << endl;
    blockfile << "char* try_to_be_unique_paraname;" << endl;
    for (auto i = block_func->begin(); i != block_func->end(); i++)
    {
        if (block_func_mp.find(i->name) != block_func_mp.end())
        {
            continue;
        }
        block_func_mp.insert(i->name);
        blockfile << "try_to_be_unique_funcname=\"" << i->name << "\";" << endl;
        blockfile << "fwrite(&(try_to_be_unique_funcname),sizeof(try_to_be_unique_funcname),1,output_ptr);" << endl;
        for (auto j = i->para.begin(); j != i->para.end(); j++)
        {
            if (block_func_para_mp.find(j->first) != block_func_para_mp.end())
            {
                continue;
            }
            block_func_para_mp.insert(j->first);
            replace_if(j->first.begin(), j->first.end(), [](char ch) { return ch == '"'; }, ' ');
            blockfile << "try_to_be_unique_paraname=\"" << j->first << "\";" << endl;
            blockfile << "fwrite(&(try_to_be_unique_paraname),sizeof(try_to_be_unique_paraname),1,output_ptr);" << endl;
            // if(j->first.find("\"")==string::npos)
            // {
            //     blockfile<<"try_to_be_unique_paraname=\""<<j->first<<"\";"<<endl;
            //     blockfile<<"fwrite(&(try_to_be_unique_paraname),sizeof(try_to_be_unique_paraname),1,output_ptr);"<<endl;
            // }
        }
    }
    //should not write too much data, because of writing disk is very slow, maybe should use shared memory
    blockfile << "int output_ptr_loc=ftell(output_ptr);" << endl;
    blockfile << "if(output_ptr_loc>100){" << endl;
    blockfile << "fclose(output_ptr);" << endl;
    blockfile << "exit(0);}" << endl;
    blockfile << "}" << endl;
}

//rewrite the program in cwmt
void Pecma::cwmtCombine(int diff)
{
    vector<var_ctx> *block_var = new vector<var_ctx>();
    vector<func_ctx> *block_func = new vector<func_ctx>();
    vector<var_ctx> *block_decl = new vector<var_ctx>();

    //get function beginline and endline, so we can change the return statement
    readRule(blockContextPath, blockContextList);
    //get subblock beginline and endline
    readRule(subBlockPath, subBlockList);
    //get Block directory
    fileTrav(rootPath + "/" + programfileName + "/Mutation", blockDir);
    //do the same thing to every block
    for (auto i = 0; i != blockDir.size(); i++)
    {
        //identifier number of the block
        string rowl = blockDir[i].substr(5, blockDir[i].size() - 5);
        int irowl = atoi(rowl.c_str());
        int blockStartLine = ruleList[irowl - 1].first;
        int blockEndLine = ruleList[irowl - 1].second;
        //block kind, 0:normal block, 1:conditon block, 2 :loop block
        int blockKind = 0;
        int subBlockStartLine = subBlockList[irowl - 1].first, subBlockEndLine = subBlockList[irowl - 1].second;
        if (subBlockStartLine > 0) //condition block
        {
            blockKind = 1;
        }
        else if (subBlockStartLine < 0) //loop block
        {
            blockKind = 2;
            subBlockStartLine = 0 - subBlockStartLine;
            subBlockEndLine = 0 - subBlockEndLine;
        }

#ifdef _DEBUG
        cout << rootPath + "/" + programfileName + "/CWMT/Block/" + blockDir[i] + ".c" << endl;
        cout << blockContextList[irowl - 1].second - 1 << endl;
#endif
        //get the mutants name in the block
        fileTrav(rootPath + "/" + programfileName + "/Mutation/" + blockDir[i], blockSub);
        ofstream blockfile(rootPath + "/" + programfileName + "/CWMT/Block/" + blockDir[i] + ".c", ofstream::out);
        if (blockfile.fail())
        {
            cout << "blockfile open error" << endl;
            return;
        }
        blockfile << "#include<unistd.h>" << endl;
        blockfile << "#include<sys/types.h>" << endl;
        blockfile << "#include<sys/wait.h>" << endl;
        blockfile << "#include<stdio.h>" << endl;
        blockfile << "int imyself_fpid= -1;" << endl;
        blockfile << "int imyself_count= -1;" << endl;
        blockfile << "int imyself_currpos;" << endl;
        //write source code before block range
        for (int j = 0; j < blockStartLine - 1; j++)
        {
            blockfile << sourceList[j] << endl;
        }
        blockfile << "{" << endl;
        blockfile << "imyself_currpos=lseek(STDIN_FILENO,0,SEEK_CUR);" << endl;
        blockfile << "while(imyself_fpid!=0&&imyself_count<" << blockSub.size() - 1 << ")" << endl;
        blockfile << "{" << endl;
        blockfile << "imyself_count=imyself_count+1;" << endl;
        blockfile << "imyself_fpid=fork();" << endl;
        blockfile << "if(imyself_fpid==0)" << endl;
        blockfile << "{" << endl;
        blockfile << "break;" << endl;
        blockfile << "}" << endl;
        blockfile << "wait(NULL);" << endl;
        blockfile << "}" << endl;
        blockfile << "if(imyself_fpid==0)" << endl;
        blockfile << "{" << endl;
        blockfile << "switch(imyself_count)" << endl;
        blockfile << "{" << endl;
        //the mutant in block
        for (int r = 0; r < blockSub.size(); r++)
        {
            blockfile << "case " << r << ":" << endl;
            blockfile << "if(access(\"" << blockDir[i] << "/" << blockSub[r] << "\",F_OK)==0)" << endl;
            blockfile << "{" << endl;
            //blockfile<<"if(freopen(\"../Result/temp/"<<blockSub[r]<<".txt\",\"a+\",stdout)==NULL)"<<endl;
            //blockfile<<"fprintf(stderr,\""<<blockDir[i]<<"/"<<blockSub[r]<<"\\n\");"<<endl;
            //blockfile<<"fflush(stdout);";
            blockfile << "FILE*output_ptr=fopen(\"../Result/temp/" << blockSub[r] << ".txt\",\"a+\");" << endl;
            blockfile << "if(output_ptr==NULL)" << endl;
            blockfile << "{" << endl;
            blockfile << "fprintf(stderr,\"output_ptr open failed!\\n\");" << endl;
            blockfile << "exit(0);" << endl;
            blockfile << "}" << endl;
            blockfile << "char* try_to_be_unique_before=\"m\";" << endl;
            blockfile << "int try_to_be_unique_flag=0;" << endl;
            blockfile << "fwrite(&(try_to_be_unique_before),sizeof(try_to_be_unique_before),1,output_ptr);" << endl;
            //blockfile<<"fwrite(&(output_ptr_loc),sizeof(output_ptr_loc),1,output_ptr);"<<endl;
            blockfile << "lseek(STDIN_FILENO,imyself_currpos,SEEK_SET);" << endl;
            readMutation(blockSub[r]);
            int pointPos = blockSub[r].find('.');
            string mutantLabelName = "PECWMA_OUTPUT_" + blockSub[r].substr(0, pointPos);
            if (blockKind == 0) //normal block
            {
                for (int k = blockStartLine - 1; k <= blockEndLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos || mutantsList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
            }
            else if (blockKind == 1) //it is condintion block
            {
                for (int k = blockStartLine - 1; k < subBlockStartLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos || mutantsList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
                blockfile << "{" << endl;
                for (int k = subBlockStartLine - 1; k <= subBlockEndLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos || mutantsList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
            }
            else // it is loop block
            {
                //write the code before subblock, bound '}' is not writed
                for (int k = blockStartLine - 1; k < subBlockStartLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos || mutantsList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
                blockfile << "{" << endl;
                for (int k = subBlockStartLine - 1; k <= subBlockEndLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos || mutantsList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
            }

#ifdef _DEBUG
            cout << "----------" << rootPath + "/" + programfileName + "/Mutation/" + blockDir[i] + "/" + blockSub[r] << endl;
#endif

            //write the status of mutant program
            if (subBlockStartLine == 0)
            {
                writeConcurrent(blockfile, blockStartLine, blockEndLine,
                                rootPath + "/" + programfileName + "/Mutation/" + blockDir[i] + "/" + blockSub[r], 1, mutantLabelName);
            }
            else
            {
                writeConcurrent(blockfile, subBlockStartLine, subBlockEndLine,
                                rootPath + "/" + programfileName + "/Mutation/" + blockDir[i] + "/" + blockSub[r], 1, mutantLabelName);
            }

            //normal block is no needed
            if (blockKind == 1)
            {
                blockfile << "}" << endl;
                //the following code is just write the last '}' of block
                for (int k = subBlockEndLine; k <= blockEndLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos)
                    {
                        blockfile << "{exit(0);" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
                writeConcurrent(blockfile, subBlockEndLine, blockEndLine,
                                rootPath + "/" + programfileName + "/Mutation/" + blockDir[i] + "/" + blockSub[r], 2, mutantLabelName);
            }
            else if (blockKind == 2)
            {
                blockfile << "}" << endl;
                //the following code is just write the last '}' of block
                for (int k = subBlockEndLine; k <= blockEndLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos)
                    {
                        blockfile << "{exit(0);" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
            }

            blockfile << "fclose(output_ptr);" << endl;
            //kill the running process
            blockfile << "exit(0);" << endl;
            blockfile << "}" << endl;
            blockfile << "else{" << endl;
            //blockfile<<"fprintf(stderr,\""<<blockDir[i]<<"/"<<blockSub[r]<<"has been killed!"<<"\\n\");"<<endl;
            blockfile << "exit(0);" << endl;
            blockfile << "}" << endl;
            blockfile << "break;" << endl;
        }
        blockfile << "default:" << endl;
        blockfile << "exit(0);" << endl;
        blockfile << "}" << endl;
        blockfile << "}" << endl;
        blockfile << "else{" << endl;
        //deal the source program code, the same procedure
        //blockfile<<"if(freopen(\"../Result/temp/Source.txt\",\"a+\",stdout)==NULL){"<<endl;
        blockfile << "FILE*output_ptr=fopen(\"../Result/temp/Source.txt\",\"a+\");" << endl;
        blockfile << "if(output_ptr==NULL){" << endl;
        blockfile << "fprintf(stderr,\"source open error\\n\");" << endl;
        blockfile << "}" << endl;
        blockfile << "char* try_to_be_unique_before=\"s\";" << endl;
        blockfile << "int try_to_be_unique_flag=0;" << endl;
        blockfile << "fwrite(&(try_to_be_unique_before),sizeof(try_to_be_unique_before),1,output_ptr);" << endl;
        blockfile << "lseek(STDIN_FILENO,imyself_currpos,SEEK_SET);" << endl;
        if (blockKind == 0) //normal block
        {
            for (int r = blockStartLine - 1; r <= blockEndLine - 1; r++)
            {
                if (sourceList[r].find("return") != string::npos || sourceList[r].find("exit(0)") != string::npos)
                {
                    blockfile << "{goto PECWMA_OUTPUT_Source;" + sourceList[r] + "}" << endl;
                }
                else
                {
                    blockfile << sourceList[r] << endl;
                }
            }
        }
        if (blockKind == 1) //condition block
        {
            for (int r = blockStartLine - 1; r < subBlockStartLine - 1; r++)
            {
                if (sourceList[r].find("return") != string::npos || sourceList[r].find("exit(0)") != string::npos)
                {
                    blockfile << "{goto PECWMA_OUTPUT_Source;" + sourceList[r] + "}" << endl;
                }
                else
                {
                    blockfile << sourceList[r] << endl;
                }
            }
            blockfile << "{" << endl;
            for (int r = subBlockStartLine - 1; r <= subBlockEndLine - 1; r++)
            {
                if (sourceList[r].find("return") != string::npos || sourceList[r].find("exit(0)") != string::npos)
                {
                    blockfile << "{goto PECWMA_OUTPUT_Source;" + sourceList[r] + "}" << endl;
                }
                else
                {
                    blockfile << sourceList[r] << endl;
                }
            }
        }
        else if (blockKind == 2) //loop block
        {
            for (int r = blockStartLine - 1; r < subBlockStartLine - 1; r++)
            {
                if (sourceList[r].find("return") != string::npos || sourceList[r].find("exit(0)") != string::npos)
                {
                    blockfile << "{goto PECWMA_OUTPUT_Source;" + sourceList[r] + "}" << endl;
                }
                else
                {
                    blockfile << sourceList[r] << endl;
                }
            }
            blockfile << "{" << endl;
            for (int r = subBlockStartLine - 1; r <= subBlockEndLine - 1; r++)
            {
                if (sourceList[r].find("return") != string::npos || sourceList[r].find("exit(0)") != string::npos)
                {
                    blockfile << "{goto PECWMA_OUTPUT_Source;" + sourceList[r] + "}" << endl;
                }
                else
                {
                    blockfile << sourceList[r] << endl;
                }
            }
        }

        //write the status of source program
        if (blockStartLine != 0) //doees not have subblock
        {
            writeConcurrent(blockfile, blockStartLine, blockEndLine, programPath, 0);
        }
        else //has subblock
        {
            writeConcurrent(blockfile, subBlockStartLine, subBlockEndLine, programPath, 0);
        }

        if (blockKind == 1)
        {
            blockfile << "}" << endl;
            for (int r = subBlockEndLine; r <= blockEndLine - 1; r++)
            {
                if (sourceList[r].find("return") != string::npos)
                {
                    blockfile << "{exit(0);" + sourceList[r] + "}" << endl;
                }
                else
                {
                    blockfile << sourceList[r] << endl;
                }
            }
            writeConcurrent(blockfile, subBlockEndLine, blockEndLine, programPath, 2);
        }
        else if (blockKind == 2)
        {
            blockfile << "}" << endl;
            for (int r = subBlockEndLine; r <= blockEndLine - 1; r++)
            {
                if (sourceList[r].find("return") != string::npos)
                {
                    blockfile << "{exit(0);" + sourceList[r] + "}" << endl;
                }
                else
                {
                    blockfile << sourceList[r] << endl;
                }
            }
        }
        blockfile << "fclose(output_ptr);" << endl;
        blockfile << "exit(0);" << endl;
        blockfile << "}" << endl;
        blockfile << "}" << endl;
        //write the rest of program file
        for (int r = blockEndLine; r < blockContextList[irowl - 1].second - 1; r++)
        {
            blockfile << sourceList[r] << endl;
        }
        //write exit(0) before the function end;
        blockfile << "{" << endl;
        blockfile << "exit(0);" << endl;
        blockfile << "}" << endl;
        for (int r = blockContextList[irowl - 1].second - 1; r < sourceList.size(); r++)
        {
            blockfile << sourceList[r] << endl;
        }
        blockfile.close();
        krCodeStyle(rootPath + "/" + programfileName + "/CWMT/Block/" + blockDir[i] + ".c");
    }
}

//rewrite the weak mutation program
void Pecma::wmtCombine()
{
    vector<var_ctx> *block_var = new vector<var_ctx>();
    vector<func_ctx> *block_func = new vector<func_ctx>();
    vector<var_ctx> *block_decl = new vector<var_ctx>();

    //get function beginline and endline, so we can change the return statement
    readRule(blockContextPath, blockContextList);
    //get subblock beginline and endline
    readRule(subBlockPath, subBlockList);
    //get Block directory
    fileTrav(rootPath + "/" + programfileName + "/Mutation", blockDir);
    //do the same thing to every block
    for (auto i = 0; i != blockDir.size(); i++)
    {
        //identifier number of the block
        string rowl = blockDir[i].substr(5, blockDir[i].size() - 5);
        int irowl = atoi(rowl.c_str());
        int blockStartLine = ruleList[irowl - 1].first;
        int blockEndLine = ruleList[irowl - 1].second;
        //block kind, 0:normal block, 1:conditon block, 2 :loop block
        int blockKind = 0;
        int subBlockStartLine = subBlockList[irowl - 1].first, subBlockEndLine = subBlockList[irowl - 1].second;
        if (subBlockStartLine > 0) //condition block
        {
            blockKind = 1;
        }
        else if (subBlockStartLine < 0) //loop block
        {
            blockKind = 2;
            subBlockStartLine = 0 - subBlockStartLine;
            subBlockEndLine = 0 - subBlockEndLine;
        }

#ifdef _DEBUG
        cout << rootPath + "/" + programfileName + "/WMT/Block/" + blockDir[i] + ".c" << endl;
        cout << blockContextList[irowl - 1].second - 1 << endl;
#endif
        //get the mutants name in the block
        fileTrav(rootPath + "/" + programfileName + "/Mutation/" + blockDir[i], blockSub);
        system(("mkdir " + rootPath + "/" + programfileName + "/WMT/Block/" + blockDir[i] + "/").c_str());
        //the mutant in block
        for (int r = 0; r < blockSub.size(); r++)
        {
            int pointPos = blockSub[r].find('.');
            string mutantLabelName = blockSub[r].substr(0, pointPos);
            readMutation(blockSub[r]);
            ofstream blockfile(rootPath + "/" + programfileName + "/WMT/Block/" + blockDir[i] + "/" + mutantLabelName + ".c", ofstream::out);
            if (blockfile.fail())
            {
                cout << "blockfile open error" << endl;
                return;
            }
            //write source code before block range
            for (int j = 0; j < blockStartLine - 1; j++)
            {
                blockfile << mutantsList[j] << endl;
            }
            blockfile << "{" << endl;
            blockfile << "FILE*output_ptr=fopen(\"../Result/temp/" << mutantLabelName << ".txt\",\"a+\");" << endl;
            blockfile << "if(output_ptr==NULL)" << endl;
            blockfile << "{" << endl;
            blockfile << "fprintf(stderr,\"output_ptr open failed!\\n\");" << endl;
            blockfile << "exit(0);" << endl;
            blockfile << "}" << endl;
            blockfile << "char* try_to_be_unique_before=\"m\";" << endl;
            blockfile << "int try_to_be_unique_flag=0;" << endl;
            blockfile << "fwrite(&(try_to_be_unique_before),sizeof(try_to_be_unique_before),1,output_ptr);" << endl;
            //blockfile<<"fwrite(&(output_ptr_loc),sizeof(output_ptr_loc),1,output_ptr);"<<endl;

            if (blockKind == 0) //normal block
            {
                for (int k = blockStartLine - 1; k <= blockEndLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos || mutantsList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
            }
            else if (blockKind == 1) //it is condintion block
            {
                for (int k = blockStartLine - 1; k < subBlockStartLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos || mutantsList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
                blockfile << "{" << endl;
                for (int k = subBlockStartLine - 1; k <= subBlockEndLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos || mutantsList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
            }
            else // it is loop block
            {
                //write the code before subblock, bound '}' is not writed
                for (int k = blockStartLine - 1; k < subBlockStartLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos || mutantsList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
                blockfile << "{" << endl;
                for (int k = subBlockStartLine - 1; k <= subBlockEndLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos || mutantsList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
            }

#ifdef _DEBUG
            cout << "----------" << rootPath + "/" + programfileName + "/Mutation/" + blockDir[i] + "/" + blockSub[r] << endl;
#endif

            //write the status of mutant program
            if (subBlockStartLine == 0)
            {
                writeConcurrent(blockfile, blockStartLine, blockEndLine,
                                rootPath + "/" + programfileName + "/Mutation/" + blockDir[i] + "/" + blockSub[r], 1, mutantLabelName);
            }
            else
            {
                writeConcurrent(blockfile, subBlockStartLine, subBlockEndLine,
                                rootPath + "/" + programfileName + "/Mutation/" + blockDir[i] + "/" + blockSub[r], 1, mutantLabelName);
            }

            //normal block is no needed
            if (blockKind == 1)
            {
                blockfile << "}" << endl;
                //the following code is just write the last '}' of block
                for (int k = subBlockEndLine; k <= blockEndLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos)
                    {
                        blockfile << "{exit(0);" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
                writeConcurrent(blockfile, subBlockEndLine, blockEndLine,
                                rootPath + "/" + programfileName + "/Mutation/" + blockDir[i] + "/" + blockSub[r], 2, mutantLabelName);
            }
            else if (blockKind == 2)
            {
                blockfile << "}" << endl;
                //the following code is just write the last '}' of block
                for (int k = subBlockEndLine; k <= blockEndLine - 1; k++)
                {
                    if (mutantsList[k].find("return") != string::npos)
                    {
                        blockfile << "{exit(0);" + mutantsList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << mutantsList[k] << endl;
                    }
                }
            }

            blockfile << "fclose(output_ptr);" << endl;
            //kill the running process
            blockfile << "exit(0);" << endl;
            blockfile << "}" << endl;
            for (int r = blockEndLine; r < blockContextList[irowl - 1].second - 1; r++)
            {
                blockfile << mutantsList[r] << endl;
            }
            //write the rest of program file
            //write exit(0) before the function end;
            blockfile << "{" << endl;
            blockfile << "exit(0);" << endl;
            blockfile << "}" << endl;
            for (int r = blockContextList[irowl - 1].second - 1; r < sourceList.size(); r++)
            {
                blockfile << mutantsList[r] << endl;
            }
            blockfile.close();
            krCodeStyle(rootPath + "/" + programfileName + "/WMT/Block/" + blockDir[i] + "/" + mutantLabelName + ".c");
        }
        ////repeat
        /////////////////////////////////////////////////////
        {
            string mutantLabelName = blockDir[i] + "source";
            ofstream blockfile(rootPath + "/" + programfileName + "/WMT/Block/" + blockDir[i] + "/" + mutantLabelName + ".c", ofstream::out);
            if (blockfile.fail())
            {
                cout << "blockfile open error" << endl;
                return;
            }
            //write source code before block range
            for (int j = 0; j < blockStartLine - 1; j++)
            {
                blockfile << sourceList[j] << endl;
            }
            blockfile << "{" << endl;
            blockfile << "FILE*output_ptr=fopen(\"../Result/temp/"
                      << "Source.txt\",\"a+\");" << endl;
            blockfile << "if(output_ptr==NULL)" << endl;
            blockfile << "{" << endl;
            blockfile << "fprintf(stderr,\"output_ptr open failed!\\n\");" << endl;
            blockfile << "exit(0);" << endl;
            blockfile << "}" << endl;
            blockfile << "char* try_to_be_unique_before=\"m\";" << endl;
            blockfile << "int try_to_be_unique_flag=0;" << endl;
            blockfile << "fwrite(&(try_to_be_unique_before),sizeof(try_to_be_unique_before),1,output_ptr);" << endl;
            //blockfile<<"fwrite(&(output_ptr_loc),sizeof(output_ptr_loc),1,output_ptr);"<<endl;

            if (blockKind == 0) //normal block
            {
                for (int k = blockStartLine - 1; k <= blockEndLine - 1; k++)
                {
                    if (sourceList[k].find("return") != string::npos || sourceList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + sourceList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << sourceList[k] << endl;
                    }
                }
            }
            else if (blockKind == 1) //it is condintion block
            {
                for (int k = blockStartLine - 1; k < subBlockStartLine - 1; k++)
                {
                    if (sourceList[k].find("return") != string::npos || sourceList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + sourceList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << sourceList[k] << endl;
                    }
                }
                blockfile << "{" << endl;
                for (int k = subBlockStartLine - 1; k <= subBlockEndLine - 1; k++)
                {
                    if (sourceList[k].find("return") != string::npos || sourceList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + sourceList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << sourceList[k] << endl;
                    }
                }
            }
            else // it is loop block
            {
                //write the code before subblock, bound '}' is not writed
                for (int k = blockStartLine - 1; k < subBlockStartLine - 1; k++)
                {
                    if (sourceList[k].find("return") != string::npos || sourceList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + sourceList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << sourceList[k] << endl;
                    }
                }
                blockfile << "{" << endl;
                for (int k = subBlockStartLine - 1; k <= subBlockEndLine - 1; k++)
                {
                    if (sourceList[k].find("return") != string::npos || sourceList[k].find("exit(0)") != string::npos)
                    {
                        blockfile << "{goto " + mutantLabelName + ";" + sourceList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << sourceList[k] << endl;
                    }
                }
            }

            //write the status of mutant program
            if (subBlockStartLine == 0)
            {
                writeConcurrent(blockfile, blockStartLine, blockEndLine,
                                rootPath + "/" + programfileName + "/Source/" + programfileName + ".c", 1, mutantLabelName);
            }
            else
            {
                writeConcurrent(blockfile, subBlockStartLine, subBlockEndLine,
                                rootPath + "/" + programfileName + "/Source/" + programfileName + ".c", 1, mutantLabelName);
            }

            //normal block is no needed
            if (blockKind == 1)
            {
                blockfile << "}" << endl;
                //the following code is just write the last '}' of block
                for (int k = subBlockEndLine; k <= blockEndLine - 1; k++)
                {
                    if (sourceList[k].find("return") != string::npos)
                    {
                        blockfile << "{exit(0);" + sourceList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << sourceList[k] << endl;
                    }
                }
                writeConcurrent(blockfile, subBlockEndLine, blockEndLine,
                                rootPath + "/" + programfileName + "/Source/" + programfileName + ".c", 2, mutantLabelName);
            }
            else if (blockKind == 2)
            {
                blockfile << "}" << endl;
                //the following code is just write the last '}' of block
                for (int k = subBlockEndLine; k <= blockEndLine - 1; k++)
                {
                    if (sourceList[k].find("return") != string::npos)
                    {
                        blockfile << "{exit(0);" + sourceList[k] + "}" << endl;
                    }
                    else
                    {
                        blockfile << sourceList[k] << endl;
                    }
                }
            }

            blockfile << "fclose(output_ptr);" << endl;
            //kill the running process
            blockfile << "exit(0);" << endl;
            blockfile << "}" << endl;
            for (int r = blockEndLine; r < blockContextList[irowl - 1].second - 1; r++)
            {
                blockfile << sourceList[r] << endl;
            }
            //write the rest of program file
            //write exit(0) before the function end;
            blockfile << "{" << endl;
            blockfile << "exit(0);" << endl;
            blockfile << "}" << endl;
            for (int r = blockContextList[irowl - 1].second - 1; r < sourceList.size(); r++)
            {
                blockfile << sourceList[r] << endl;
            }
            blockfile.close();
            krCodeStyle(rootPath + "/" + programfileName + "/WMT/Block/" + blockDir[i] + "/" + mutantLabelName + ".c");
        }
    }
}

//jiajingting
void Pecma::cmtCombine()
{
    fileTrav(rootPath + "/" + programfileName + "/Mutation", blockDir);
    for (int i = 0; i < blockDir.size(); i++)
    {
        string rowl = blockDir[i].substr(5, blockDir[i].size() - 5);
        int irowl = atoi(rowl.c_str());
        int a = ruleList[irowl - 1].first, b = ruleList[irowl - 1].second;
#ifdef _DEBUG
        cout << a << " " << b << endl;
        cout << rootPath + "/" + programfileName + "/CMT/Block/" + blockDir[i] + ".c" << endl;
#endif
        fileTrav(rootPath + "/" + programfileName + "/Mutation/" + blockDir[i], blockSub);
        ofstream blockfile(rootPath + "/" + programfileName + "/CMT/Block/" + blockDir[i] + ".c", ofstream::out);
        if (blockfile.fail())
        {
            cout << "blockfile open error" << endl;
        }
        blockfile << "#include<unistd.h>" << endl;
        blockfile << "#include<sys/types.h>" << endl;
        blockfile << "#include<sys/wait.h>" << endl;
        blockfile << "#include<stdio.h>" << endl;
        blockfile << "int imyself_fpid= -1;" << endl;
        blockfile << "int imyself_count= -1;" << endl;
        blockfile << "int imyself_currpos;" << endl;
        for (int j = 0; j < a - 1; j++)
        {
            blockfile << sourceList[j] << endl;
        }
        blockfile << "{" << endl;
        blockfile << "imyself_currpos=lseek(STDIN_FILENO,0,SEEK_CUR);" << endl;
        blockfile << "while(imyself_fpid!=0&&imyself_count<" << blockSub.size() - 1 << "){" << endl;
        blockfile << "imyself_count=imyself_count+1;" << endl;
        blockfile << "imyself_fpid=fork();" << endl;
        blockfile << "if(imyself_fpid==0){" << endl;
        blockfile << "break;" << endl;
        blockfile << "}" << endl;
        blockfile << "wait(NULL);" << endl;
        blockfile << "}" << endl;
        blockfile << "if(imyself_fpid==0){" << endl;
        blockfile << "switch(imyself_count){" << endl;
        for (int r = 0; r < blockSub.size(); r++)
        {
            blockfile << "case " << r << ":" << endl;
            blockfile << "if(access(\"" << blockDir[i] << "/" << blockSub[r] << "\",F_OK)==0){" << endl;
            blockfile << "if(freopen(\"../Result/temp/" << blockSub[r] << ".txt\",\"a+\",stdout)==NULL)" << endl;
            blockfile << "fprintf(stderr,\"" << blockDir[i] << "/" << blockSub[r] << "\\n\");" << endl;
            blockfile << "fflush(stdout);";
            blockfile << "lseek(STDIN_FILENO,imyself_currpos,SEEK_SET);" << endl;
            readMutation(blockSub[r]);
            for (int k = a - 1; k < b; k++)
            {
                blockfile << mutantsList[k] << endl;
            }
            blockfile << "}" << endl;
            blockfile << "else{" << endl;
            blockfile << "fprintf(stderr,\"" << blockDir[i] << "/" << blockSub[r] << "has been killed!"
                      << "\\n\");" << endl;
            blockfile << "exit(0);" << endl;
            blockfile << "}" << endl;
            blockfile << "break;" << endl;
        }
        blockfile << "default:" << endl;
        blockfile << "exit(0);" << endl;
        blockfile << "}" << endl;
        blockfile << "}" << endl;
        blockfile << "else{" << endl;
        blockfile << "if(freopen(\"../Result/temp/Source.txt\",\"a+\",stdout)==NULL){" << endl;
        blockfile << "fprintf(stderr,\"source open error\\n\");" << endl;
        blockfile << "}" << endl;
        blockfile << "lseek(STDIN_FILENO,imyself_currpos,SEEK_SET);" << endl;
        for (int r = a - 1; r < b; r++)
        {
            blockfile << sourceList[r] << endl;
        }
        blockfile << "}" << endl;
        blockfile << "}" << endl;
        for (int r = b; r < sourceList.size(); r++)
        {
            blockfile << sourceList[r] << endl;
        }
        blockfile.close();
        krCodeStyle(rootPath + "/" + programfileName + "/CMT/Block/" + blockDir[i] + ".c");
    }
}

//indent -kr -i8
void Pecma::krCodeStyle(string name)
{
    string cmd = "indent -kr -i8 -nbap -sob -l360 -nce -bl " + name;
#ifdef _DEBUG
    cout << cmd << endl;
#endif
    system(cmd.c_str());
}

//write shell scripts
void Pecma::cwmtWriteShell()
{
#ifdef _DEBUG
    cout << rootPath + "/" + programfileName + "/CWMT/Scripts/RunAsforBlock.sh" << endl;
#endif
    ofstream runshell(rootPath + "/" + programfileName + "/CWMT/Scripts/RunAsforBlock.sh", ofstream::out);
    if (runshell.fail())
    {
        cerr << "shell file open failed!" << endl;
    }
    runshell << "function rand(){" << endl;
    runshell << "min=$1" << endl;
    runshell << "max=$(($2-$min+1))" << endl;
    runshell << "num=$(date +%s%N)" << endl;
    runshell << "echo $(($num%$max+$min))" << endl;
    runshell << "}" << endl;

    runshell << "cd " + rootPath + "/" + programfileName + "/CWMT/Scripts/" << endl;
    runshell << "pwd" << endl;
    runshell << "date" << endl;
    runshell << "if [ ! -e ../../Testcases ];then" << endl;
    runshell << "    mkdir ../../Testcases" << endl;
    runshell << "fi" << endl;
    runshell << "echo \"The start of this testing:\"" << endl;
    runshell << "folder_A=\"" << testcasePath << "\"" << endl;
    runshell << "folder_B=\"../Block\"" << endl;
    runshell << "folder_C=\"../Result/temp\"" << endl;
    runshell << "for file_b in ${folder_B}/*.c;do" << endl;
    runshell << "    date" << endl;
    runshell << "    temp_file_b=` basename ${file_b} .c `" << endl;
    runshell << "    echo \"****************************\"" << endl;
    runshell << "    echo ${temp_file_b}\" is being executiving.\"" << endl;
    runshell << "    cnt=`ls ${folder_A} | wc -w`" << endl;
    runshell << "    for file_a in ${folder_A}/*;do" << endl;
    runshell << "        rnd=$(rand 1 ${cnt})" << endl;
    //runshell<<"        temp_file_a=` basename ${file_a} `"<<endl;
    runshell << "        temp_file_a=${rnd}" << endl;
    runshell << "        echo \"========================\"" << endl;
    runshell << "        echo \"Executing test case \"${temp_file_a}" << endl;
    runshell << "        testdir=\"../Result/${temp_file_a}\"" << endl;
    runshell << "        if [ ! -d ${testdir} ]; then" << endl;
    runshell << "            mkdir ../Result/${temp_file_a}" << endl;
    runshell << "        fi" << endl;
    runshell << "        count=$( ls ${temp_file_b} | wc -l)" << endl;
    runshell << "        if [ ${count} != 0 ]; then" << endl;
    runshell << "            while read line" << endl;
    runshell << "            do" << endl;
    runshell << "                has_input=$(echo ${line} | grep \"input\")" << endl;
    runshell << "                if [[ \"${has_input}\" != \"\" ]]" << endl;
    runshell << "                then" << endl;
    runshell << "                    i1=`echo $line | cut -d \"<\" -f 1`" << endl;
    runshell << "                    i2=`echo $line | cut -d \"<\" -f 2`" << endl;
    runshell << "                    echo ${i1}"
                "${i2}"
             << endl;
    runshell << "                    ../Block/${temp_file_b} ${i1} < ${i2}" << endl;
    runshell << "                else" << endl;
    runshell << "                    ../Block/${temp_file_b} ${line}" << endl;
    runshell << "                fi" << endl;
    runshell << "            done < " << testcasePath << "/${temp_file_a}" << endl;
    runshell << "        else" << endl;
    runshell << "            echo \"${temp_file_b} in this block have been killed.\"" << endl;
    runshell << "            break" << endl;
    runshell << "        fi" << endl;
    runshell << "        if [ ! -e \"../Result/temp/Source.txt\" ];then" << endl;
    runshell << "            echo \"Test case ${temp_file_a} dose not cover these branches relate with ${temp_file_b}.\"" << endl;
    runshell << "            continue" << endl;
    runshell << "        fi" << endl;
    runshell << "        for file_c in ${folder_C}/*;do" << endl;
    runshell << "            temp_file_c=` basename ${file_c} `" << endl;
    runshell << "            temp_file_c=${temp_file_c%.*}" << endl;
    runshell << "            if ( ! diff -q ${file_c} ../Result/temp/Source.txt );then" << endl;
    runshell << "                rm -f ${temp_file_b}/${temp_file_c}" << endl;
    runshell << "                cp ${folder_A}/${temp_file_a} ../Testcases" << endl;
    runshell << "            fi" << endl;
    runshell << "        done" << endl;
    runshell << "        mv -f ${folder_C}/* ${testdir}" << endl;
    runshell << "    done" << endl;
    runshell << "done" << endl;
    runshell << "echo \"The end of this testing.\"" << endl;
    runshell.close();

#ifdef _DEBUG
    cout << rootPath + "/" + programfileName + "/CWMT/Scripts/CompileAsforBlock.sh" << endl;
#endif
    ofstream compileshell(rootPath + "/" + programfileName + "/CWMT/Scripts/CompileAsforBlock.sh", ofstream::out);
    if (compileshell.fail())
    {
        cerr << "compile shell file open failed!" << endl;
    }
    compileshell << "cd " + rootPath + "/" + programfileName + "/CWMT/Scripts/" << endl;
    int pos = programPath.rfind('/');
    string include_path = programPath.substr(0, pos);
    compileshell << "export C_INCLUDE_PATH=$C_INCLUDE_PATH:" + include_path << endl;
    compileshell << "folder_B=\"../Block\"" << endl;
    compileshell << "folder_C=\"../Result/temp\"" << endl;
    compileshell << "for file_b in ${folder_B}/*.c;do" << endl;
    compileshell << "    date" << endl;
    compileshell << "    temp_file_b=` basename ${file_b} .c `" << endl;
    compileshell << "    echo \"*************************\"" << endl;
    compileshell << "    echo ${temp_file_b}\" is being compiled.\"" << endl;
    compileshell << "    gcc -w ../Block/${file_b} -lm -o ../Block/${temp_file_b}" << endl;
    compileshell << "    if  [ $? -eq 0 ]; then" << endl;
    compileshell << "        echo \"gcc success\"" << endl;
    compileshell << "    else" << endl;
    compileshell << "        echo \"gcc fail\"" << endl;
    compileshell << "        continue" << endl;
    compileshell << "    fi" << endl;
    compileshell << "done" << endl;
    compileshell.close();

#ifdef _DEBUG
    cout << rootPath + "/" + programfileName + "/CWMT/Scripts/TimetheRun.sh" << endl;
#endif
    ofstream ctrlshell(rootPath + "/" + programfileName + "/CWMT/Scripts/TimetheRun.sh", ofstream::out);
    if (ctrlshell.fail())
    {
        cerr << "shell file open failed!" << endl;
    }
    runshell << "mkdir ../Testcases" << endl;
    ctrlshell << "cd " + rootPath + "/" + programfileName + "/CWMT/Scripts/" << endl;
    ctrlshell << "pwd" << endl;
    ctrlshell << "echo \"time the Run\"" << endl;
    ctrlshell << "date" << endl;
    ctrlshell << "echo \">>>>>>>>>>>>>>>>>>>>>>>>compile BEGIN<<<<<<<<<<<<<<<<<\"" << endl;
    ctrlshell << "chmod u+x ./CompileAsforBlock.sh" << endl;
    ctrlshell << "time ( ./CompileAsforBlock.sh >> output.txt )" << endl;
    ctrlshell << "sleep 3s" << endl;
    ctrlshell << "rm -r ../Result/*" << endl;
    ctrlshell << "mkdir ../Result/temp" << endl;
    ctrlshell << "if [ -e output.txt ];then" << endl;
    ctrlshell << "    rm output.txt" << endl;
    ctrlshell << "fi" << endl;
    ctrlshell << "chmod u+x ./RunAsforBlock.sh" << endl;
    ctrlshell << "cp -r ../../Mutation/* ./" << endl;
    ctrlshell << "echo \">>>>>>>>>>>>>>>>>>>>>>>>run BEGIN<<<<<<<<<<<<<<<<<\"" << endl;
    ctrlshell << "#cp ../../../run_shell/" << programfileName << "/Scripts/RunAsforBlock.sh ./" << endl;
    ctrlshell << "(time ( ./RunAsforBlock.sh >> output.txt )) |& tee -a output2.txt" << endl;
    ctrlshell.close();
    string cmd = "chmod u+x " + rootPath + "/" + programfileName + "/CWMT/Scripts/TimetheRun.sh";
    system(cmd.c_str());
}

//write shell scripts of weak mutation
void Pecma::wmtWriteShell()
{
#ifdef _DEBUG
    cout << rootPath + "/" + programfileName + "/WMT/Scripts/RunAsforBlock.sh" << endl;
#endif
    ofstream runshell(rootPath + "/" + programfileName + "/WMT/Scripts/RunAsforBlock.sh", ofstream::out);
    if (runshell.fail())
    {
        cerr << "shell file open failed!" << endl;
    }
    runshell << "cd " + rootPath + "/" + programfileName + "/WMT/Scripts/" << endl;
    runshell << "pwd" << endl;
    runshell << "date" << endl;
    runshell << "echo \"*********************************************\"" << endl;
    runshell << "echo \"The start of this testing:\"" << endl;
    runshell << "folder_A=\"../../Testcase\"" << endl;
    runshell << "folder_B=\"../Exe\"" << endl;
    runshell << "folder_C=\"../Result/temp\"" << endl;
    runshell << "for dir_b in ${folder_B}/B*;do" << endl;
    runshell << "    temp_dir_b=` basename ${dir_b} `" << endl;
    runshell << "        echo ${temp_dir_b}\" is being tested.\"" << endl;
    runshell << "    for file_a in ${folder_A}/t*;do" << endl;
    runshell << "        temp_file_a=` basename ${file_a} `" << endl;
    runshell << "        echo ${temp_file_a}\" is being used.\"" << endl;
    runshell << "        count=$( ls ${dir_b} | wc -l)" << endl;
    runshell << "        if [ ${count} -ne 1 ]; then" << endl;
    runshell << "            echo \"-----run the left program------\"" << endl;
    runshell << "            for file_b in ${dir_b}/*;do" << endl;
    runshell << "                temp_file_b=` basename ${file_b} `" << endl;
    runshell << "                while read line" << endl;
    runshell << "                do" << endl;
    runshell << "                    has_input=$(echo ${line} | grep \"input\")" << endl;
    runshell << "                    if [[ \"${has_input}\" != \"\" ]]" << endl;
    runshell << "                    then" << endl;
    runshell << "                        i1=`echo $line | cut -d \"<\" -f 1`" << endl;
    runshell << "                        i2=`echo $line | cut -d \"<\" -f 2`" << endl;
    runshell << "                        ${dir_b}/${temp_file_b} ${i1} < ${i2}" << endl;
    runshell << "                    else" << endl;
    runshell << "                        ${dir_b}/${temp_file_b} ${line}" << endl;
    runshell << "                    fi" << endl;
    runshell << "                done < ${file_a}" << endl;
    runshell << "            done" << endl;
    runshell << "        else" << endl;
    runshell << "            echo \"${dir_b} have been killed.\"" << endl;
    runshell << "            break" << endl;
    runshell << "        fi" << endl;
    runshell << "        if [ ! -e \"../Result/temp/Source.txt\" ];then" << endl;
    runshell << "            echo \"Test case ${temp_file_a} dose not cover these branches relate with ${dir_b}.\"" << endl;
    runshell << "            continue" << endl;
    runshell << "        fi" << endl;
    runshell << "        for file_c in ${folder_C}/*;do" << endl;
    runshell << "            temp_file_c=` basename ${file_c} .txt`" << endl;
    runshell << "            if ( ! diff -q ${file_c} ../Result/temp/Source.txt );then" << endl;
    runshell << "                rm  ${dir_b}/${temp_file_c}" << endl;
    runshell << "            fi" << endl;
    runshell << "        done" << endl;
    runshell << "        testdir=\"../Result/${temp_file_a}\"" << endl;
    runshell << "        if [ ! -e ${testdir} ]; then" << endl;
    runshell << "            mkdir ${testdir}" << endl;
    runshell << "        fi" << endl;
    runshell << "        mv  ${folder_C}/* ${testdir}" << endl;
    runshell << "    done" << endl;
    runshell << "done" << endl;
    runshell << "echo \"The end of this testing.\"" << endl;
    runshell.close();

#ifdef _DEBUG
    cout << rootPath + "/" + programfileName + "/WMT/Scripts/CompileAsforBlock.sh" << endl;
#endif
    ofstream compileshell(rootPath + "/" + programfileName + "/WMT/Scripts/CompileAsforBlock.sh", ofstream::out);
    if (compileshell.fail())
    {
        cerr << "compile shell file open failed!" << endl;
    }
    compileshell << "cd " + rootPath + "/" + programfileName + "/WMT/Scripts/" << endl;
    int pos = programPath.rfind('/');
    string include_path = programPath.substr(0, pos);
    compileshell << "export C_INCLUDE_PATH=$C_INCLUDE_PATH:" + include_path << endl;
    compileshell << "folder_A=\"../Exe\"" << endl;
    compileshell << "if [ ! -e ${folder_A} ];then" << endl;
    compileshell << "    mkdir ${folder_A}" << endl;
    compileshell << "fi" << endl;
    compileshell << "folder_B=\"../Block\"" << endl;
    compileshell << "folder_C=\"../Result/temp\"" << endl;
    compileshell << "for dir_b in ${folder_B}/B*;do" << endl;
    compileshell << "    temp_dir_b=` basename ${dir_b} `" << endl;
    compileshell << "    if [ ! -e ${folder_A}/${temp_dir_b} ];then" << endl;
    compileshell << "        mkdir  ${folder_A}/${temp_dir_b}" << endl;
    compileshell << "    fi" << endl;
    compileshell << "    for file_b in ${dir_b}/*.c;do" << endl;
    compileshell << "        date" << endl;
    compileshell << "        temp_file_b=` basename ${file_b} .c `" << endl;
    compileshell << "        echo \"*************************\"" << endl;
    compileshell << "        echo ${temp_file_b}\" is being compiled.\"" << endl;
    compileshell << "        gcc -w ../Block/${file_b} -lm -o ${folder_A}/${temp_dir_b}/${temp_file_b}" << endl;
    compileshell << "        if  [ $? -eq 0 ]; then" << endl;
    compileshell << "            echo \"gcc success\"" << endl;
    compileshell << "        else" << endl;
    compileshell << "            echo \"gcc fail\"" << endl;
    compileshell << "            continue" << endl;
    compileshell << "        fi" << endl;
    compileshell << "    done" << endl;
    compileshell << "done" << endl;
    compileshell.close();

#ifdef _DEBUG
    cout << rootPath + "/" + programfileName + "/WMT/Scripts/TimetheRun.sh" << endl;
#endif
    ofstream ctrlshell(rootPath + "/" + programfileName + "/WMT/Scripts/TimetheRun.sh", ofstream::out);
    if (ctrlshell.fail())
    {
        cerr << "shell file open failed!" << endl;
    }
    runshell << "mkdir ../Testcases" << endl;
    ctrlshell << "cd " + rootPath + "/" + programfileName + "/WMT/Scripts/" << endl;
    ctrlshell << "pwd" << endl;
    ctrlshell << "echo \"time the Run\"" << endl;
    ctrlshell << "date" << endl;
    ctrlshell << "echo \">>>>>>>>>>>>>>>>>>>>>>>>compile BEGIN<<<<<<<<<<<<<<<<<\"" << endl;
    ctrlshell << "chmod u+x ./CompileAsforBlock.sh" << endl;
    ctrlshell << "time ( ./CompileAsforBlock.sh >> output.txt )" << endl;
    ctrlshell << "sleep 3s" << endl;
    ctrlshell << "rm -r ../Result/*" << endl;
    ctrlshell << "mkdir ../Result/temp" << endl;
    ctrlshell << "if [ -e output.txt ];then" << endl;
    ctrlshell << "    rm output.txt" << endl;
    ctrlshell << "fi" << endl;
    ctrlshell << "if [ -e output2.txt ];then" << endl;
    ctrlshell << "    rm output2.txt" << endl;
    ctrlshell << "fi" << endl;
    ctrlshell << "chmod u+x ./RunAsforBlock.sh" << endl;
    ctrlshell << "echo \">>>>>>>>>>>>>>>>>>>>>>>>run BEGIN<<<<<<<<<<<<<<<<<\"" << endl;
    ctrlshell << "#cp ../../../run_shell/" << programfileName << "/WMT/Scripts/RunAsforBlock.sh ./" << endl;
    ctrlshell << "(time ( ./RunAsforBlock.sh >> output.txt )) |& tee -a output2.txt" << endl;
    ctrlshell << "cd " + rootPath + "/" + programfileName + "/Scripts/" << endl;
    ctrlshell << "echo \">>>>>>>>>>>>>>>>>>>>>>>>Block Cant be Killed<<<<<<<<<<<<<<<<<\"" << endl;
    ctrlshell << "for infolder in ../Exe/B*;do" << endl;
    ctrlshell << "    cnt=$(ls ${infolder} | wc -l)" << endl;
    ctrlshell << "    if [ $cnt -ne 1 ];then" << endl;
    ctrlshell << "        echo ${infolder}" << endl;
    ctrlshell << "    fi" << endl;
    ctrlshell << "done" << endl;
    ctrlshell.close();
    string cmd = "chmod u+x " + rootPath + "/" + programfileName + "/WMT/Scripts/TimetheRun.sh";
    system(cmd.c_str());
}

//write shell scripts of concurrent mutation
void Pecma::cmtWriteShell()
{
#ifdef _DEBUG
    cout << rootPath + "/" + programfileName + "/CMT/Scripts/RunAsforBlock.sh" << endl;
#endif
    ofstream runshell(rootPath + "/" + programfileName + "/CMT/Scripts/RunAsforBlock.sh", ofstream::out);
    if (runshell.fail())
    {
        cerr << "shell file open failed!" << endl;
    }
    runshell << "cd " + rootPath + "/" + programfileName + "/CMT/Scripts/" << endl;
    runshell << "pwd" << endl;
    runshell << "date" << endl;
    runshell << "echo \"*********************************************\"" << endl;
    runshell << "echo \"The start of this testing:\"" << endl;
    runshell << "folder_A=\"../../Testcase\"" << endl;
    runshell << "folder_B=\"../Exe\"" << endl;
    runshell << "folder_C=\"../Result/temp\"" << endl;
    runshell << "for dir_b in ${folder_B}/B*;do" << endl;
    runshell << "    temp_dir_b=` basename ${dir_b} `" << endl;
    runshell << "    echo ${temp_dir_b}\" is being tested.\"" << endl;
    runshell << "    for file_a in ${folder_A}/t*;do" << endl;
    runshell << "        temp_file_a=` basename ${file_a} `" << endl;
    runshell << "        echo ${temp_file_a}\" is being used.\"" << endl;
    runshell << "        count=$( ls ${dir_b} | wc -l)" << endl;
    runshell << "        if [ ${count} -ne 1 ]; then" << endl;
    runshell << "            echo \"-----run the left program------\"" << endl;
    runshell << "            for file_b in ${dir_b}/*;do" << endl;
    runshell << "                temp_file_b=` basename ${file_b} `" << endl;
    runshell << "                while read line" << endl;
    runshell << "                do" << endl;
    runshell << "                    has_input=$(echo ${line} | grep \"input\")" << endl;
    runshell << "                    if [[ \"${has_input}\" != \"\" ]]" << endl;
    runshell << "                    then" << endl;
    runshell << "                        i1=`echo $line | cut -d \"<\" -f 1`" << endl;
    runshell << "                        i2=`echo $line | cut -d \"<\" -f 2`" << endl;
    runshell << "                        ${dir_b}/${temp_file_b} ${i1} < ${i2}" << endl;
    runshell << "                    else" << endl;
    runshell << "                        ${dir_b}/${temp_file_b} ${line}" << endl;
    runshell << "                    fi" << endl;
    runshell << "                done < ${file_a}" << endl;
    runshell << "            done" << endl;
    runshell << "        else" << endl;
    runshell << "            echo \"${dir_b} have been killed.\"" << endl;
    runshell << "            break" << endl;
    runshell << "        fi" << endl;
    runshell << "        if [ ! -e \"../Result/temp/Source.txt\" ];then" << endl;
    runshell << "            echo \"Test case ${temp_file_a} dose not cover these branches relate with ${dir_b}.\"" << endl;
    runshell << "            continue" << endl;
    runshell << "        fi" << endl;
    runshell << "        for file_c in ${folder_C}/*;do" << endl;
    runshell << "            temp_file_c=` basename ${file_c} .txt`" << endl;
    runshell << "            if ( ! diff -q ${file_c} ../Result/temp/Source.txt );then" << endl;
    runshell << "                rm  ${dir_b}/${temp_file_c}" << endl;
    runshell << "            fi" << endl;
    runshell << "        done" << endl;
    runshell << "        testdir=\"../Result/${temp_file_a}\"" << endl;
    runshell << "        if [ ! -e ${testdir} ]; then" << endl;
    runshell << "            mkdir ${testdir}" << endl;
    runshell << "        fi" << endl;
    runshell << "        mv  ${folder_C}/* ${testdir}" << endl;
    runshell << "    done" << endl;
    runshell << "done" << endl;
    runshell << "echo \"The end of this testing.\"" << endl;
    runshell.close();

#ifdef _DEBUG
    cout << rootPath + "/" + programfileName + "/CMT/Scripts/CompileAsforBlock.sh" << endl;
#endif
    ofstream compileshell(rootPath + "/" + programfileName + "/CMT/Scripts/CompileAsforBlock.sh", ofstream::out);
    if (compileshell.fail())
    {
        cerr << "compile shell file open failed!" << endl;
    }
    compileshell << "cd " + rootPath + "/" + programfileName + "/CMT/Scripts/" << endl;
    int pos = programPath.rfind('/');
    string include_path = programPath.substr(0, pos);
    compileshell << "export C_INCLUDE_PATH=$C_INCLUDE_PATH:" + include_path << endl;
    compileshell << "folder_A=\"../Exe\"" << endl;
    compileshell << "if [ ! -e ${folder_A} ];then" << endl;
    compileshell << "    mkdir ${folder_A}" << endl;
    compileshell << "fi" << endl;
    compileshell << "folder_B=\"../Block\"" << endl;
    compileshell << "folder_C=\"../Result/temp\"" << endl;
    compileshell << "for dir_b in ${folder_B}/B*;do" << endl;
    compileshell << "    temp_dir_b=` basename ${dir_b} `" << endl;
    compileshell << "    if [ ! -e ${folder_A}/${temp_dir_b} ];then" << endl;
    compileshell << "        mkdir  ${folder_A}/${temp_dir_b}" << endl;
    compileshell << "    fi" << endl;
    compileshell << "    for file_b in ${dir_b}/*.c;do" << endl;
    compileshell << "        date" << endl;
    compileshell << "        temp_file_b=` basename ${file_b} .c `" << endl;
    compileshell << "        echo \"*************************\"" << endl;
    compileshell << "        echo ${temp_file_b}\" is being compiled.\"" << endl;
    compileshell << "        gcc -w ../Block/${file_b} -lm -o ${folder_A}/${temp_dir_b}/${temp_file_b}" << endl;
    compileshell << "        if  [ $? -eq 0 ]; then" << endl;
    compileshell << "            echo \"gcc success\"" << endl;
    compileshell << "        else" << endl;
    compileshell << "            echo \"gcc fail\"" << endl;
    compileshell << "            continue" << endl;
    compileshell << "        fi" << endl;
    compileshell << "    done" << endl;
    compileshell << "done" << endl;
    compileshell.close();

#ifdef _DEBUG
    cout << rootPath + "/" + programfileName + "/CMT/Scripts/TimetheRun.sh" << endl;
#endif
    ofstream ctrlshell(rootPath + "/" + programfileName + "/CMT/Scripts/TimetheRun.sh", ofstream::out);
    if (ctrlshell.fail())
    {
        cerr << "shell file open failed!" << endl;
    }
    runshell << "mkdir ../Testcases" << endl;
    ctrlshell << "cd " + rootPath + "/" + programfileName + "/CMT/Scripts/" << endl;
    ctrlshell << "pwd" << endl;
    ctrlshell << "echo \"time the Run\"" << endl;
    ctrlshell << "date" << endl;
    ctrlshell << "echo \">>>>>>>>>>>>>>>>>>>>>>>>compile BEGIN<<<<<<<<<<<<<<<<<\"" << endl;
    ctrlshell << "chmod u+x ./CompileAsforBlock.sh" << endl;
    ctrlshell << "time ( ./CompileAsforBlock.sh >> output.txt )" << endl;
    ctrlshell << "sleep 3s" << endl;
    ctrlshell << "rm -r ../Result/*" << endl;
    ctrlshell << "mkdir ../Result/temp" << endl;
    ctrlshell << "if [ -e output.txt ];then" << endl;
    ctrlshell << "    rm output.txt" << endl;
    ctrlshell << "fi" << endl;
    ctrlshell << "if [ -e output2.txt ];then" << endl;
    ctrlshell << "    rm output2.txt" << endl;
    ctrlshell << "fi" << endl;
    ctrlshell << "chmod u+x ./RunAsforBlock.sh" << endl;
    ctrlshell << "echo \">>>>>>>>>>>>>>>>>>>>>>>>run BEGIN<<<<<<<<<<<<<<<<<\"" << endl;
    ctrlshell << "#cp ../../../run_shell/" << programfileName << "/WMT/Scripts/RunAsforBlock.sh ./" << endl;
    ctrlshell << "(time ( ./RunAsforBlock.sh >> output.txt )) |& tee -a output2.txt" << endl;
    ctrlshell << "cd " + rootPath + "/" + programfileName + "/Scripts/" << endl;
    ctrlshell << "echo \">>>>>>>>>>>>>>>>>>>>>>>>Block Cant be Killed<<<<<<<<<<<<<<<<<\"" << endl;
    ctrlshell << "for infolder in ../Exe/B*;do" << endl;
    ctrlshell << "    cnt=$(ls ${infolder} | wc -l)" << endl;
    ctrlshell << "    if [ $cnt -ne 1 ];then" << endl;
    ctrlshell << "        echo ${infolder}" << endl;
    ctrlshell << "    fi" << endl;
    ctrlshell << "done" << endl;
    ctrlshell.close();
    string cmd = "chmod u+x " + rootPath + "/" + programfileName + "/CMT/Scripts/TimetheRun.sh";
    system(cmd.c_str());
}
