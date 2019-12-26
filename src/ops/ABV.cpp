#include "operator.h"

ABV::ABV(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-ABV"+type)
{}

bool ABV::VisitExpr(Expr* I){
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    std::string wfilename=mutantdir;
    if(I->getValueKind()==0 && (I->getType().getAsString().compare("int")==0 || I->getType().getAsString().compare("long")==0 ||
    I->getType().getAsString().compare("long long")==0)){
        llvm::outs()<<"take the absolute value of a int/long/long long variable " << "\n";
        const string text = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(I->getSourceRange()),
        astContext->getSourceManager(), astContext->getLangOpts());

        rewriter->ReplaceText(I->getBeginLoc (),text.length(),"std::abs("+text+")");
        mutcount+=1;
        writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        //rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID()).write(outs());
        rewriter->ReplaceText(I->getBeginLoc (),text.length()+10,text);
    }
    else if(I->getValueKind()==0 && (I->getType().getAsString().compare("float")==0 || I->getType().getAsString().compare("double")==0)){
        llvm::outs()<<"take the absolute value of a floating variable " << "\n";
        const string text = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(I->getSourceRange()), astContext->getSourceManager(),
        astContext->getLangOpts());

        rewriter->ReplaceText(I->getBeginLoc (),text.length(),"std::fabs("+text+")");
        mutcount+=1;
        writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        rewriter->ReplaceText(I->getBeginLoc (),text.length()+11,text);
    }
    return true;
}
