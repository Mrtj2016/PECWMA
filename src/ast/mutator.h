#ifndef MUTANTOR_H
#define MUTANTOR_H

#include "../common/common.h"
#include "../ops/operator.h"

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

class MutatorOp;

class BlockVisitor : public RecursiveASTVisitor<BlockVisitor> {
private:
    ASTContext *astContext; // used for getting additional AST info
    Rewriter rewriter;
    vector<pair<unsigned,unsigned>>rule;  //<begin_loc, end_loc> location pair of blocks
    vector<block_struct>block_context;  //context of block, the begin and end of block, the begin and end of funciton include the block
    pair<int,int>func_context;  //the begin and end of function
    pair<int,int>subblock_context;  //the begin and end of for block, if is not for block, it would be <0,0>
    set<pair<unsigned,unsigned>>locflag; //make sure every location pair is not duplicated. I think it no use now.
    int normalstmt; //location of normal statement begin
    unsigned lastloc; //location of last block end
    bool if_flag; //if the else body statment is a if_else structure
    string outfilename;  //the name of output  filename for rule
    string out_block_func_filename;  //the name of output filename block context
    string out_return_stmt_filename;  //the name of output filename for return statement
    string out_subblock_filename;  //the name of subblock
public:
    BlockVisitor(CompilerInstance *CI,string path);

    //algorithm to part the block
    void pxsc();

    void dealStmt(clang::Stmt *s);

    void dealStmt(clang::CompoundStmt *s);
    void write_stmt_loc(unsigned kai, unsigned jie);

    void decideStmt(clang::Stmt *s, int dg=0);

    bool VisitStmt(clang::Stmt *s);

    bool VisitFunctionDecl(FunctionDecl*funcD);
};


class MutatorVisitor : public RecursiveASTVisitor<MutatorVisitor> {
private:
    ASTContext *astContext; // used for getting additional AST info
    Rewriter* rewriter;
    int mutcount;
    std::string mutantdir;
    std::string mutangtype;
    vector<MutatorOp*>mut_ptr_vec;
public:
    //explicit
    MutatorVisitor(CompilerInstance *CI,string path,vector<MutatorOp*> mut_ptr=vector<MutatorOp*>(),string type=".c");

    // Override Binary Operator expressions
    bool VisitBinaryOperator(BinaryOperator *E);
    bool VisitExpr(Expr* I);
    bool VisitUnaryOperator(UnaryOperator*U);
    bool VisitStmt(Stmt *st) ;
    bool VisitIntegerLiteral(IntegerLiteral* I);
    bool VisitFloatingLiteral(FloatingLiteral* I);
    bool VisitCallExpr(CallExpr* I);
    bool VisitFunctionDecl(FunctionDecl *f);
    bool VisitDeclRefExpr(DeclRefExpr* I);
    //bool VisitFloatingLiteral(FloatingLiteral* I);
};



class EseVisitor : public RecursiveASTVisitor<EseVisitor> {
private:
    ASTContext *astContext; // used for getting additional AST info
    Rewriter rewriter;
    vector<var_ctx>*block_var;
    vector<func_ctx>*block_func;
    vector<var_ctx>*block_decl;
    set<string>var_duplicate;
    int blockBeginLine;
    int blockEndLine;
    map<int,string>stmtkind;

public:
    EseVisitor(CompilerInstance *CI, int begin_loc, int end_loc,vector<var_ctx>*bv,vector<func_ctx>*bf,vector<var_ctx>*bd);
    // EseVisitor(CompilerInstance *CI, int begin_loc, int end_loc):
    // astContext(&(CI->getASTContext())),rewriter(Rewriter()),
    // blockBeginLine(begin_loc),blockEndLine(end_loc)
    // {  // initialize private members
    //     rewriter.setSourceMgr(astContext->getSourceManager(), astContext->getLangOpts());
    //     block_var=new vector<var_ctx>();
    //     block_func=new vector<func_ctx>();
    // }

    void write_var(string name, string type, unsigned line, unsigned flag=0);
    // Override Binary Operator expressions
    bool VisitBinaryOperator(BinaryOperator *E);

    bool VisitCallExpr(CallExpr *CExpr);

    bool VisitUnaryOperator(UnaryOperator *uop);

    bool VisitVarDecl(VarDecl *variDecl);

    bool VisitStmt(Stmt *s);

    void show();
};

class MutatorASTConsumer : public ASTConsumer {
private:
    BlockVisitor *visitorblock; // doesn't have to be private
    MutatorVisitor*visitormutator;
    EseVisitor*visitorese;
public:
    // override the constructor in order to pass CI
    MutatorASTConsumer(CompilerInstance *CI,string actype,string path,int begin_loc,int end_loc,vector<var_ctx>*bv,vector<func_ctx>*bf,vector<var_ctx>*bd);

    virtual void HandleTranslationUnit(ASTContext &Context);
    ~MutatorASTConsumer();
private:
    string actionType;
};

class MutatorFrontendAction : public ASTFrontendAction {
public:
    MutatorFrontendAction(string actype,string path,int begin,int end,vector<var_ctx>*bv,vector<func_ctx>*bf,vector<var_ctx>*bd):
    actionType(actype),mutPath(path),begin_loc(begin),end_loc(end),block_var(bv),block_func(bf),block_decl(bd)
    {}

    //Note that unique pointer is used.
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) {
        return  llvm::make_unique<MutatorASTConsumer>(&CI,actionType,mutPath,begin_loc,end_loc,block_var,block_func,block_decl); // pass CI pointer to ASTConsumer
    }
    void setActionType(string actype){
        actionType=actype;
    }
    void setPath(string pt){
        mutPath=pt;
    }
    void setLoc(int begin,int end){
        begin_loc=begin;
        end_loc=end;
    }
private:
    string actionType;
    string mutPath;
    int begin_loc;
    int end_loc;
    vector<var_ctx>*block_var;
    vector<func_ctx>*block_func;
    vector<var_ctx>*block_decl;
};

template <typename T>
class SimpleFrontendActionFactory : public FrontendActionFactory {
public:
    string actype;
    string path;
    int begin;
    int end;
    vector<var_ctx>*block_var;
    vector<func_ctx>*block_func;
    vector<var_ctx>*block_decl;
    SimpleFrontendActionFactory(string type,string pt,int bg,int ed,vector<var_ctx>*bvv,vector<func_ctx>*bff,vector<var_ctx>*bdd):
    actype(type),path(pt),begin(bg),end(ed),block_var(bvv),block_func(bff),block_decl(bdd){}

    void setLoc(int bg,int ed){
        begin=bg;
        end=ed;
    }
    void setActype(string type){
        actype=type;
    }
    void setPath(string pt){
        path=pt;
    }

    clang::FrontendAction *create() override { return new T(actype,path,begin,end,block_var,block_func,block_decl); }
};

template <typename T>
std::unique_ptr<FrontendActionFactory> newFrontendActionFactory(string actype,string path,int begin,int end,
    vector<var_ctx>*bv=new vector<var_ctx>(),vector<func_ctx>*bf=new vector<func_ctx>(),vector<var_ctx>*bd=new vector<var_ctx>()) {

  return std::unique_ptr<FrontendActionFactory>(new SimpleFrontendActionFactory<T>(actype,path,begin,end,bv,bf,bd));
}

#endif
