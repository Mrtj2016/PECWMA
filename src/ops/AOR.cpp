#include "operator.h"

AOR::AOR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-AOR"+type)
{}

bool AOR::VisitBinaryOperator (BinaryOperator *E) {
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    std::string wfilename=mutantdir;
    if(E->isAdditiveOp()){
        //outs()<<E->getOpcodeStr()<<E->getOpcodeStr().compare("+")<<"\n";
        if(E->getOpcodeStr().compare("+")==0){
            //replace + with -
            rewriter->ReplaceText(E->getOperatorLoc(),"-");
            llvm::outs()<<"Additive op "<<E->getOpcodeStr()<<"\n";
            //rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID()).write(outs());
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"+");

            //replace + with *
            rewriter->ReplaceText(E->getOperatorLoc(),"*");
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"+");

            //replace + with /
            rewriter->ReplaceText(E->getOperatorLoc(),"/");
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"+");
        }
        else if(E->getOpcodeStr().compare("-")==0){
            //replace - with +
            rewriter->ReplaceText(E->getOperatorLoc(),"+");
            llvm::outs()<<"Additive op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"-");

            //replace - with *
            rewriter->ReplaceText(E->getOperatorLoc(),"*");
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"-");

            //replace - with /
            rewriter->ReplaceText(E->getOperatorLoc(),"/");
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"-");
        }
    }
    else if(E->isMultiplicativeOp()){
        if(E->getOpcodeStr().compare("*")==0){
            //replace * with +
            rewriter->ReplaceText(E->getOperatorLoc(),"+");
            llvm::outs()<<"Additive op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"*");

            //replace * with -
            rewriter->ReplaceText(E->getOperatorLoc(),"-");
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"*");

            //replace * with /
            rewriter->ReplaceText(E->getOperatorLoc(),"/");
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"*");
        }
        else if(E->getOpcodeStr().compare("/")==0){
            //replace / with +
            rewriter->ReplaceText(E->getOperatorLoc(),"+");
            llvm::outs()<<"Additive op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"/");

            //replace / with -
            rewriter->ReplaceText(E->getOperatorLoc(),"-");
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"/");

            //replace '/' with '*'
            rewriter->ReplaceText(E->getOperatorLoc(),"*");
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"/");
        }
    }
    return true;
}
