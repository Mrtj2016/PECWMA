#include "mutator.h"

MutatorVisitor::MutatorVisitor(CompilerInstance *CI,string path,vector<MutatorOp*> mut_ptr,string type):
astContext(&(CI->getASTContext())), rewriter(new Rewriter()),
mutcount(0),mutantdir(path),mutangtype(type),mut_ptr_vec(mut_ptr)
{  // initialize private members
    rewriter->setSourceMgr(astContext->getSourceManager(), astContext->getLangOpts());
    if(mutantdir[mutantdir.size()-1]!='/') {
        mutantdir=mutantdir+"/";
    }
}

bool MutatorVisitor::VisitBinaryOperator(BinaryOperator *E){
    if(!(astContext->getSourceManager().isWrittenInMainFile(E->getOperatorLoc()))){
        return true;
    }
    for(auto i =mut_ptr_vec.begin();i!=mut_ptr_vec.end();i++){
        (*i)->mutcount=mutcount;
        (*i)->mutantdir=mutantdir;
        (*i)->mutangtype=mutangtype;
        (*i)->VisitBinaryOperator(E);
        mutcount=(*i)->mutcount;
    }
    return true;
}
bool MutatorVisitor::VisitExpr(Expr* E) {
    if(!(astContext->getSourceManager().isWrittenInMainFile(E->getBeginLoc()))){
        return true;
    }
    for(auto i =mut_ptr_vec.begin();i!=mut_ptr_vec.end();i++){
        (*i)->mutcount=mutcount;
        (*i)->mutantdir=mutantdir;
        (*i)->mutangtype=mutangtype;
        (*i)->VisitExpr(E);
        mutcount=(*i)->mutcount;
    }
    return true;
}
bool MutatorVisitor::VisitUnaryOperator(UnaryOperator*E) {
    if(!(astContext->getSourceManager().isWrittenInMainFile(E->getOperatorLoc()))){
        return true;
    }
    for(auto i =mut_ptr_vec.begin();i!=mut_ptr_vec.end();i++){
        (*i)->mutcount=mutcount;
        (*i)->mutantdir=mutantdir;
        (*i)->mutangtype=mutangtype;
        (*i)->VisitUnaryOperator(E);
        mutcount=(*i)->mutcount;
    }
    return true;
}
bool MutatorVisitor::VisitStmt(Stmt *E) {
    if(!(astContext->getSourceManager().isWrittenInMainFile(E->getBeginLoc()))){
        return true;
    }
    for(auto i =mut_ptr_vec.begin();i!=mut_ptr_vec.end();i++){
        (*i)->mutcount=mutcount;
        (*i)->mutantdir=mutantdir;
        (*i)->mutangtype=mutangtype;
        mutcount=(*i)->mutcount;
        (*i)->VisitStmt(E);
    }
    return true;
}
 bool MutatorVisitor::VisitIntegerLiteral(IntegerLiteral* E)  {
    if(!(astContext->getSourceManager().isWrittenInMainFile(E->getBeginLoc()))){
        return true;
    }
    for(auto i =mut_ptr_vec.begin();i!=mut_ptr_vec.end();i++){
        (*i)->mutcount=mutcount;
        (*i)->mutantdir=mutantdir;
        (*i)->mutangtype=mutangtype;
        (*i)->VisitIntegerLiteral(E);
        mutcount=(*i)->mutcount;
    }
    return true;
}
bool MutatorVisitor::VisitFloatingLiteral(FloatingLiteral* E)  { //1.3  may go to 1.3000000000000000444089209850063.0
   if(!(astContext->getSourceManager().isWrittenInMainFile(E->getBeginLoc()))){
       return true;
   }
   for(auto i =mut_ptr_vec.begin();i!=mut_ptr_vec.end();i++){
       (*i)->mutcount=mutcount;
       (*i)->mutantdir=mutantdir;
       (*i)->mutangtype=mutangtype;
       (*i)->VisitFloatingLiteral(E);
       mutcount=(*i)->mutcount;
   }
   return true;
}
bool MutatorVisitor::VisitCallExpr(CallExpr* E){
   if(!(astContext->getSourceManager().isWrittenInMainFile(E->getBeginLoc()))) {
       return true;
   }
   for(auto i =mut_ptr_vec.begin();i!=mut_ptr_vec.end();i++){
       (*i)->mutcount=mutcount;
       (*i)->mutantdir=mutantdir;
       (*i)->mutangtype=mutangtype;
       (*i)->VisitCallExpr(E);
       mutcount=(*i)->mutcount;
   }
   return true;
}
bool MutatorVisitor::VisitFunctionDecl(FunctionDecl *E) {
   if(!(astContext->getSourceManager().isWrittenInMainFile(E->getSourceRange().getBegin()))) {
       //llvm::outs() << f->getNameAsString().c_str() << "\n";
       return true;
   }
   return true;
}
bool MutatorVisitor::VisitDeclRefExpr(DeclRefExpr* E){
   if(!(astContext->getSourceManager().isWrittenInMainFile(E->getBeginLoc()))){
       return true;
   }
   for(auto i =mut_ptr_vec.begin();i!=mut_ptr_vec.end();i++){
       (*i)->mutcount=mutcount;
       (*i)->mutantdir=mutantdir;
       (*i)->mutangtype=mutangtype;
       (*i)->VisitDeclRefExpr(E);
       mutcount=(*i)->mutcount;
   }
   return true;
}
