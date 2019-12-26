#include "operator.h"

BOR::BOR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-BOR"+type)
{}
    // Override Binary Operator expressions
bool BOR::VisitBinaryOperator (BinaryOperator *E) {
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    std::string wfilename=mutantdir;
    if(E->isBitwiseOp()){
        //llvm::outs()<<E->getOpcodeStr()<<"\n";
        if(E->getOpcodeStr().compare("&")==0){
            rewriter->ReplaceText(E->getOperatorLoc(),"|");
            llvm::outs()<<"Bitwise_and op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"&");
        }
        else if(E->getOpcodeStr().compare("|")==0){
            rewriter->ReplaceText(E->getOperatorLoc(),"&");
            llvm::outs()<<"Bitwise_or op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),"|");
        }
        else if(E->getOpcodeStr().compare("^")==0){
            llvm::outs()<<"Opcode ^:"<<E->getOpcodeStr()<<"\n";
        }
        else{
            llvm::outs()<<"Opcode:"<<E->getOpcodeStr()<<"\n";
        }
    }
    return true;
}
bool BOR::VisitUnaryOperator (UnaryOperator *E) {
    //llvm::outs()<<E->getOpcodeStr(E->getOpcode ())<<"\n";
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    std::string wfilename=mutantdir;
    if(E->isArithmeticOp ()){
        if(E->getOpcodeStr(E->getOpcode ()).compare("~")==0){
            rewriter->ReplaceText(E->getOperatorLoc(),1,"");
            llvm::outs()<<"Logical_not op "<<E->getOpcodeStr(E->getOpcode ())<<"\n";
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),0,"!");
        }
    }
    return true;
}
