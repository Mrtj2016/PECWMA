#include "operator.h"

STDC::STDC(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-STDC"+type)
{}

bool STDC::VisitStmt (Stmt* I){
    //
    // const string text = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(I->getSourceRange()), astContext->getSourceManager(), astContext->getLangOpts());
    //
    // //llvm::outs()<<I->getCallReturnType(*astContext).getAsString()<<"\n";
    // clang::SourceLocation SemiLoc = clang::arcmt::trans::findSemiAfterLocation(I->getEndLoc(), *astContext);
    //
    // if(SemiLoc.isValid()){
    //     if(astContext->getSourceManager().getPresumedLineNumber(I->getBeginLoc())==lineflag){
    //         return ;
    //     }
    //     llvm::outs()<<"*****************************" << "\n";
    //     llvm::outs()<<text<<" "<<text.length()<<"\n";
    //     llvm::outs()<<astContext->getSourceManager().getPresumedLineNumber(I->getBeginLoc())<<"  line number\n";
    //     llvm::outs()<<astContext->getSourceManager().getPresumedColumnNumber(I->getBeginLoc())<<"  begin number\n";
    //     llvm::outs()<<astContext->getSourceManager().getPresumedColumnNumber(I->getEndLoc())<<"end number\n";
    //     llvm::outs()<<astContext->getSourceManager().getPresumedColumnNumber(SemiLoc)<<"---------\n";
    //     auto lineflag=astContext->getSourceManager().getPresumedLineNumber(I->getBeginLoc());
    // }
    return true;
}
