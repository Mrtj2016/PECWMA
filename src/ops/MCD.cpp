#include "operator.h"

MCD::MCD(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-MCD"+type)
{}

bool MCD::VisitCallExpr(CallExpr* I){
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    //llvm::outs()<<"*****************************" << "\n";
    const string text = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(I->getSourceRange()), astContext->getSourceManager(), astContext->getLangOpts());
    //llvm::outs()<<text<<" "<<text.length()<<"\n";
    //llvm::outs()<<I->getCallReturnType(*astContext).getAsString()<<"\n";
    if(I->getCallReturnType(*astContext).getAsString().compare("void")==0){
        //llvm::outs()<<astContext->getSourceManager().getPresumedLoc(I->getBeginLoc()).getLine()<<"============\n";
        llvm::outs()<<"delete void function call " << "\n";
        rewriter->ReplaceText(I->getBeginLoc (),text.length(),"");
        mutcount+=1;
        //write into file to generate mutant
        writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        //rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID()).write(outs());
        rewriter->ReplaceText(I->getBeginLoc (),0,text);
    }
    else if(I->getCallReturnType(*astContext).getAsString().compare("int")==0 || I->getCallReturnType(*astContext).getAsString().compare("long")==0 ||
        I->getCallReturnType(*astContext).getAsString().compare("long long")==0){
        llvm::outs()<<"delete int/long/longlong function call " << "\n";
        rewriter->ReplaceText(I->getBeginLoc (),text.length(),"0");
        mutcount+=1;
        writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        rewriter->ReplaceText(I->getBeginLoc (),1,text);
    }
    else if(I->getCallReturnType(*astContext).getAsString().compare("float")==0 || I->getCallReturnType(*astContext).getAsString().compare("double")==0){
        llvm::outs()<<"delete float function call " << "\n";
        rewriter->ReplaceText(I->getBeginLoc (),text.length(),"0.0");
        mutcount+=1;
        writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        rewriter->ReplaceText(I->getBeginLoc (),3,text);
    }
    else if(I->getCallReturnType(*astContext).getAsString().compare("_Bool")==0){
        llvm::outs()<<"delete bool function call " << "\n";
        rewriter->ReplaceText(I->getBeginLoc (),text.length(),"true");
        mutcount+=1;
        writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        rewriter->ReplaceText(I->getBeginLoc (),4,text);
    }
    return true;
}
