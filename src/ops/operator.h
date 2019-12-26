#ifndef OPERATOR_H
#define OPERATOR_H

#include "../common/common.h"
#include "../ast/mutator.h"

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

class MutatorOp{
public:
    ASTContext *astContext; // used for getting additional AST info
    Rewriter* rewriter;
    int mutcount;
    std::string mutantdir;
    std::string mutangtype;

    MutatorOp(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);

    bool writemut(std::string filename, std::string filecontent);

    virtual bool VisitBinaryOperator(BinaryOperator *E);
    virtual bool VisitExpr(Expr* I);
    virtual bool VisitUnaryOperator(UnaryOperator*U);
    virtual bool VisitStmt(Stmt *st);
    virtual bool VisitIntegerLiteral(IntegerLiteral* I);
    virtual bool VisitFloatingLiteral(FloatingLiteral* I);
    virtual bool VisitCallExpr(CallExpr* I);
    virtual bool VisitFunctionDecl(FunctionDecl *f);
    virtual bool VisitDeclRefExpr(DeclRefExpr* I);
};

class ABV : public MutatorOp{
public:
    ABV(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitExpr(Expr* I) override;
};

class AOR : public MutatorOp{
public:
    AOR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitBinaryOperator(BinaryOperator *E) override;
};

class BOR : public MutatorOp{
public:
    BOR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitBinaryOperator(BinaryOperator *E) override;
    virtual bool VisitUnaryOperator(UnaryOperator*U) override;
};

class CSR : public MutatorOp{
public:
    CSR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitStmt(Stmt *E) override;
};

class LOR : public MutatorOp{
public:
    LOR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitBinaryOperator(BinaryOperator *E) override;
    virtual bool VisitUnaryOperator(UnaryOperator*U) override;
};

class LVR : public MutatorOp{
public:
    LVR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitIntegerLiteral(IntegerLiteral* I) override;
    virtual bool VisitFloatingLiteral(FloatingLiteral* I) override;
};

class MCD : public MutatorOp{
public:
    MCD(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitCallExpr(CallExpr* I) override;
};

class ROR : public MutatorOp{
public:
    ROR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitBinaryOperator(BinaryOperator *E) override;
};

class ROV : public MutatorOp{
public:
    ROV(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitStmt(Stmt *E) override;
};
class SOR : public MutatorOp{
public:
    SOR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitBinaryOperator(BinaryOperator *E) override;
};
class STDC : public MutatorOp{
public:
    STDC(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitStmt(Stmt *E) override;
};
class UOI : public MutatorOp{
public:
    UOI(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitDeclRefExpr(DeclRefExpr* I) override;
};
class UOR : public MutatorOp{
public:
    UOR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type);
    virtual bool VisitUnaryOperator(UnaryOperator*U) override;
};

#endif
