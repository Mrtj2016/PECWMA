#include "./ast/mutator.h"
#include "./rewriter/pecma.h"
#include "./common/json.hpp"

using json = nlohmann::json;

string sourceHeader = "";

inline void cutPath(string &path, string kind)
{
    auto pos = path.find(kind);
    path = path.substr(pos + 1, path.size() - pos - 1);
}

void process(string destProgram, string actype, string path, int begin, int end,
             vector<var_ctx> *block_var, vector<func_ctx> *block_func, vector<var_ctx> *block_decl)
{
    // CompilerInstance will hold the instance of the Clang compiler for us,
    // managing the various objects needed to run the compiler.
    CompilerInstance TheCompInst;
    TheCompInst.createDiagnostics();

    //decide to parse which kind program language
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
    headerSearchOptions.AddPath(sourceHeader, clang::frontend::Angled, false, false);

    TheCompInst.createPreprocessor(clang::TU_Module);
    TheCompInst.createASTContext();

    // Set the main file handled by the source manager to the input file.
    const FileEntry *FileIn = FileMgr.getFile(destProgram);
    SourceMgr.setMainFileID(SourceMgr.createFileID(FileIn, SourceLocation(), SrcMgr::C_User));
    TheCompInst.getDiagnosticClient().BeginSourceFile(TheCompInst.getLangOpts(), &TheCompInst.getPreprocessor());
    MutatorASTConsumer TheConsumer(&TheCompInst, actype, path, begin, end, block_var, block_func, block_decl);
    ParseAST(TheCompInst.getPreprocessor(), &TheConsumer, TheCompInst.getASTContext());
}

Pecma *divobj;

void preprocess(string configfile)
{
    divobj = new Pecma(configfile, "bubble");
}
void doMutation(string destProgram, string pt, string ops)
{
    //destProgram:path of program to Mutation
    //pt:path to store mutants
    //ops:string include operator
    vector<var_ctx> *bv = new vector<var_ctx>();
    istringstream readop(ops);
    string op;
    while (readop >> op)
    {
        var_ctx tmp;
        tmp.name = op;
        (*bv).push_back(tmp);
    }
    process(destProgram, "mutator", pt, 0, 0, bv, nullptr, nullptr);
}

void doBlock(string destProgram, string path)
{
    //destProgram:path of program to Mutation
    //path:path to store rule
    llvm::outs() << destProgram << " " << path << "\n";
    process(destProgram, "blockrule", path, 0, 0, nullptr, nullptr, nullptr);
}

void doCombine()
{
    Pecma *divobj = new Pecma("./pecma.json", "bubble");
    string rootp = divobj->programPath;
    int pos = rootp.rfind('/');
    rootp = rootp.substr(0, pos);
    system(("cp " + divobj->rulePath + " " + rootp).c_str());
    system(("cp " + divobj->blockContextPath + " " + rootp).c_str());
    system(("cp " + divobj->subBlockPath + " " + rootp).c_str());
    pos = rootp.rfind('/');
    rootp = rootp.substr(0, pos);
    system(("cp -r " + rootp + "/inputs" + " " + divobj->rootPath + "/" + divobj->programfileName).c_str());
    system(("cp " + divobj->testcasePath + "/* " + divobj->rootPath + "/" + divobj->programfileName + "/Testcase").c_str());
    divobj->division();
    divobj->cwmtCombine(1);
    divobj->cwmtWriteShell();
}

extern "C"
{
    int python_preprocess(char *configfile)
    {
        preprocess(configfile);
        return 0;
    }
    int python_doMutation(char *a, char *b, char *c)
    {
        doMutation(a, b, c);
        return 0;
    }
    int python_doBlock(char *a, char *b)
    {
        doBlock(a, b);
        return 0;
    }
    int python_doCombine()
    {
        doCombine();
        return 0;
    }
}

int control()
{
    string programName; //name of program
    string destProgram; //path of program
    string workPath;
    int mode;
    Pecma *divobj;
    while (1)
    {
        cout << "Input mode,'1' to read config file, '3' to quit\nInput: ";
        cin >> mode;
        if (mode == 1)
        {
            ifstream configFile("pecma.json", ifstream::in);
            if (configFile.fail())
            {
                cerr << "open config file error!" << endl;
                cin.ignore(numeric_limits<std::streamsize>::max());
                break;
            }
            json configJson;
            configFile >> configJson;
            cout << "Found program:\n";
            cout << "--------------------------------------------------------------------------\n";
            for (auto iter = configJson["object"].begin(); iter != configJson["object"].end(); iter++)
            {
                cout << iter.key() << " | ";
            }
            cout << "\n--------------------------------------------------------------------------\n";
            cout << "Input program name: ";
            std::cin >> programName;
            destProgram = configJson["object"][programName]["programPath"];
            workPath = configJson["object"][programName]["rootPath"];
            workPath += "Pecwma/" + programName;
            cout << "workPath:" << workPath << endl;
            auto pos = destProgram.rfind("/");
            sourceHeader = destProgram.substr(0, pos);
            std::cout << "******************************************************\n";
            divobj = new Pecma("./pecma.json", programName);
        }
        else if (mode == 3)
        {
            break;
        }
        while (1)
        {
            std::cout << "Function:\n1:generate mutant\n2:create block rule\n3:CMT\n4:WMT\n5:CWMT\n6:back\nInput: ";
            int flag;
            std::cin >> flag;
            if (flag == 1)
            {
                cout << "Input mutator op, end with --\nInput: ";
                vector<var_ctx> *bv = new vector<var_ctx>();
                string op;
                while (1)
                {
                    cin >> op;
                    if (op == "--")
                    {
                        break;
                    }
                    var_ctx tmp;
                    tmp.name = op;
                    (*bv).push_back(tmp);
                }
                process(destProgram, "mutator", workPath + "/Mutant/", 0, 0, bv, nullptr, nullptr);
            }
            else if (flag == 2)
            {
                process(destProgram, "blockrule", "", 0, 0, nullptr, nullptr, nullptr);
            }
            else if (flag == 3)
            {
                process(destProgram, "blockrule", workPath + "/Rule/", 0, 0, nullptr, nullptr, nullptr);
                divobj->division();
                divobj->cwmtWriteShell();
                divobj->cmtCombine();
            }
            else if (flag == 4)
            {
                process(destProgram, "blockrule", workPath + "/Rule/", 0, 0, nullptr, nullptr, nullptr);
                divobj->division();
                divobj->wmtWriteShell();
                divobj->wmtCombine();
            }
            else if (flag == 5)
            {
                cout << workPath + "/Rule/" << endl;
                process(destProgram, "blockrule", workPath + "/Rule/", 0, 0, nullptr, nullptr, nullptr);
                cout << "divison" << endl;
                divobj->division();
                cout << "cwmtWriteShell" << endl;
                divobj->cwmtWriteShell();
                cout << "cwmtCombine" << endl;
                divobj->cwmtCombine(1);
            }
            else if (flag == 6)
            {
                cin.ignore(numeric_limits<std::streamsize>::max());
                break;
            }
            else if (flag == 10) //not used
            {
                string path = "./";
                int begin, end;
                cin.clear();
                cin.ignore(INT_MAX, '\n');
                cout << "Input begin_loc,end_loc, back to go back: ";
                cin >> begin >> end;
                vector<var_ctx> *block_var = new vector<var_ctx>();
                vector<func_ctx> *block_func = new vector<func_ctx>();
                vector<var_ctx> *block_decl = new vector<var_ctx>();
                process(destProgram, "ese", path, begin, end, block_var, block_func, block_decl);
            }
        }
    }
    return 0;
}

int writeJson()
{
    json j2 = {{"object program", {
                                      {"bubble", {
                                                     {"rootPath", "/home/tang/tools/test/CPMS/src/mutator/"},
                                                     {"programPath", "/home/tang/tools/test/CPMS/src/mutator/source/shuju/bubble/Source/bubble.c"},
                                                     {"mutantsPath", "/home/tang/tools/test/CPMS/src/mutator/source/shuju/bubble/MutationT/"},
                                                 }},
                                  }}};
    // std::cout<<j2.dump()<<std::endl;
    std::fstream file;
    file.open("pecma.json", ios::out);
    file << j2.dump();
    file.close();
    return 0;
}

int main(int argc, const char **argv)
{
    control();
}
