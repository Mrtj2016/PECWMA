#include "operator.h"

SOR::SOR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-SOR"+type)
{}

bool SOR::VisitBinaryOperator (BinaryOperator *E) {
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    if(E->isShiftOp()){
        //outs()<<E->getOpcodeStr()<<"\n";
        if(E->getOpcodeStr().compare(">>")==0){
            rewriter->ReplaceText(E->getOperatorLoc(),"<<");
            llvm::outs()<<"Logical_right_shift op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),">>");
        }
        else if(E->getOpcodeStr().compare("<<")==0){
            rewriter->ReplaceText(E->getOperatorLoc(),">>");
            llvm::outs()<<"Logical_left_shift op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),">>");
        }
        else{
            outs()<<"Opcode:"<<E->getOpcodeStr()<<"\n";
        }
    }
    return true;
}
