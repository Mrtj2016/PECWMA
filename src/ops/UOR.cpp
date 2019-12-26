#include "operator.h"

UOR::UOR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-UOR"+type)
{}

bool UOR::VisitUnaryOperator (UnaryOperator *E){
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    if(E->isIncrementOp()){
        llvm::outs()<<"Increment op "<<E->getOpcodeStr(E->getOpcode ())<<"\n";
        rewriter->ReplaceText(E->getOperatorLoc(),2,"--");
        mutcount+=1;
        writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        rewriter->ReplaceText(E->getOperatorLoc(),2,"++");

        rewriter->ReplaceText(E->getOperatorLoc(),2,"");
        mutcount+=1;
        writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        rewriter->ReplaceText(E->getOperatorLoc(),0,"++");
    }
    else if(E->isDecrementOp()){
        llvm::outs()<<"Increment op "<<E->getOpcodeStr(E->getOpcode ())<<"\n";
        rewriter->ReplaceText(E->getOperatorLoc(),2,"++");
        mutcount+=1;
        writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        rewriter->ReplaceText(E->getOperatorLoc(),2,"--");

        rewriter->ReplaceText(E->getOperatorLoc(),2,"");
        mutcount+=1;
        writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        rewriter->ReplaceText(E->getOperatorLoc(),0,"--");
    }
    else if(E->isArithmeticOp ()){//other arithmetic Operator, such as b=-a.
        //errs()<<"Increment op "<<E->getOpcodeStr(E->getOpcode ())<<"   111111111\n";
    }
    else{//other
        //llvm::outs()<<"Increment op "<<E->getOpcodeStr(E->getOpcode ())<<"   222222222\n";
    }
    return true;
}
