#include "operator.h"

LOR::LOR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-LOR"+type)
{}

bool LOR::VisitBinaryOperator(BinaryOperator *E){
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    std::string wfilename=mutantdir;
    if(E->isLogicalOp()){
        //llvm::outs()<<E->getOpcodeStr()<<E->getOpcodeStr().compare("&&")<<"aaaa\n";
        if(E->getOpcodeStr().compare("&&")==0){
            //replace && with ||
            rewriter->ReplaceText(E->getOperatorLoc(),"||");
            llvm::outs()<<"Logical_and op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            //go back
            rewriter->ReplaceText(E->getOperatorLoc(),"&&");
        }
        else if(E->getOpcodeStr().compare("||")==0){
            //replace || with &&
            rewriter->ReplaceText(E->getOperatorLoc(),"&&");
            llvm::outs()<<"Logical_or op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            //go back
            rewriter->ReplaceText(E->getOperatorLoc(),"||");
        }
        else{
            llvm::outs()<<"Opcode:"<<E->getOpcodeStr()<<"\n";
        }
    }
    return true;
}
bool LOR::VisitUnaryOperator (UnaryOperator *E) {
    //outs()<<E->getOpcodeStr(E->getOpcode ()).compare("&&")<<"\n";
    //outs()<<E->getOpcodeStr(E->getOpcode ())<<"\n";
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    std::string wfilename=mutantdir+"lor-";
    if(E->isArithmeticOp ()){
        if(E->getOpcodeStr(E->getOpcode ()).compare("!")==0){
            //remvoe the ! operator
            rewriter->ReplaceText(E->getOperatorLoc(),1,"");
            llvm::outs()<<"Logical_not op "<<E->getOpcodeStr(E->getOpcode ())<<"\n";
            mutcount+=1;
            writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            //go back
            rewriter->ReplaceText(E->getOperatorLoc(),0,"!");
        }
    }
    return true;
}
