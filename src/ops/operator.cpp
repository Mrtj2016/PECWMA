#include "operator.h"

MutatorOp::MutatorOp(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):
astContext(ac),rewriter(rt),mutcount(count),mutantdir(dir),mutangtype(type)
{
    rewriter->setSourceMgr(astContext->getSourceManager(), astContext->getLangOpts());
}

bool MutatorOp::writemut(std::string filename, std::string filecontent){
    std::ofstream mutfile(filename);
    if(!mutfile){
        return false;
    }
    mutfile<<filecontent;
    return true;
}

bool MutatorOp::VisitBinaryOperator(BinaryOperator *E){
    return true;
}
bool MutatorOp::VisitExpr(Expr* I) {
    return true;
}
bool MutatorOp::VisitUnaryOperator(UnaryOperator*U) {
    return true;
}
bool MutatorOp::VisitStmt(Stmt *st) {
    return true;
}
bool MutatorOp::VisitIntegerLiteral(IntegerLiteral* I)  {
    return true;
}
bool MutatorOp::VisitFloatingLiteral(FloatingLiteral* I){
    return true;
}
bool MutatorOp::VisitCallExpr(CallExpr* I){
    return true;
}
bool MutatorOp::VisitFunctionDecl(FunctionDecl *f) {
    return true;
}
bool MutatorOp::VisitDeclRefExpr(DeclRefExpr* I){
    return true;
}
