#include "operator.h"

LVR::LVR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-LVR"+type)
{}

bool LVR::VisitIntegerLiteral(IntegerLiteral* I){
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    //llvm::outs()<<I->getNameInfo().getAsString ()<<"\n";
    //llvm::outs()<<I->getType().getAsString()<<"\n";
    //I->dump();
    auto sourcevalue=I->getValue ();
    rewriter->ReplaceText(I->getLocation(),"0");
    //llvm::outs()<<"change int value " << "\n";
    //llvm::outs()<<I->getValue () << "\n";
    mutcount+=1;
    //rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID()).write(llvm::outs());
    //write into file to generate mutant
    writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
    rewriter->ReplaceText(I->getLocation(),1,sourcevalue.toString(10,false));
    return true;
}
bool LVR::VisitFloatingLiteral(FloatingLiteral* I){ //1.3  may go to 1.3000000000000000444089209850063.0
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    //llvm::outs()<<I->getNameInfo().getAsString ()<<"\n";
    //llvm::outs()<<I->getType().getAsString()<<"\n";
    //I->dump();
    auto sourcevalue=I->getValue ();
    rewriter->ReplaceText(I->getLocation(),"0.0");
    llvm::outs()<<"change float value " << "\n";
    mutcount+=1;
    //write into file to generate mutant
    writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
    rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID()).write(llvm::outs());
    llvm::SmallVector<char, 32> tmpstring;
    sourcevalue.toString(tmpstring,32);
    std::string outstring="";
    for(auto kai=tmpstring.begin();kai!=tmpstring.end();kai++){
        outstring+=*kai;
    }
    rewriter->ReplaceText(I->getLocation(),3,outstring);
    //sourcevalue.dump();
    rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID()).write(llvm::outs());
    return true;
}
