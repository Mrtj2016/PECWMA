#include "operator.h"

ROR::ROR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-ROR"+type)
{}

bool ROR::VisitBinaryOperator (BinaryOperator *E) {
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    if(E->isEqualityOp()){
        //llvm::outs()<<E->getOpcodeStr()<<E->getOpcodeStr().compare("==")<<"aaaa\n";
        if(E->getOpcodeStr().compare("==")==0){
            rewriter->ReplaceText(E->getOperatorLoc(),"!=");
            llvm::outs()<<"Equality op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),2,"==");
        }
        else if(E->getOpcodeStr().compare("!=")==0){
            rewriter->ReplaceText(E->getOperatorLoc(),"==");
            llvm::outs()<<"Not Equality op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),2,"!=");
        }
    }
    else if(E->isComparisonOp ()){//no complicated replacement
        //llvm::outs()<<E->getOpcodeStr()<<E->getOpcodeStr().compare(">")<<"bbbb\n";
        if(E->getOpcodeStr().compare(">")==0){
            rewriter->ReplaceText(E->getOperatorLoc(),"<");
            llvm::outs()<<"greater op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),1,">");
        }
        else if(E->getOpcodeStr().compare(">=")==0){//there is also can be replace by < > == !=,but it is not needed.
            rewriter->ReplaceText(E->getOperatorLoc(),"<=");
            llvm::outs()<<"greater_or_equal  op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            /*
            rewriter->ReplaceText(E->getOperatorLoc(),2,">");
            llvm::outs()<<"greater_or_equal  op "<<E->getOpcodeStr()<<"\n";
            */
            rewriter->ReplaceText(E->getOperatorLoc(),2,">=");
        }
        else if(E->getOpcodeStr().compare("<")==0){
            rewriter->ReplaceText(E->getOperatorLoc(),">");
            llvm::outs()<<"less op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),1,"<");
        }
        else if(E->getOpcodeStr().compare("<=")==0){
            rewriter->ReplaceText(E->getOperatorLoc(),">=");
            llvm::outs()<<"less_or_equal op "<<E->getOpcodeStr()<<"\n";
            mutcount+=1;
            writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            rewriter->ReplaceText(E->getOperatorLoc(),2,"<=");
        }
        else{
            llvm::outs()<<"Opcode:"<<E->getOpcodeStr()<<"\n";
        }
    }
    return true;
}
