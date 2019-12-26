#include "operator.h"

UOI::UOI(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-UOI"+type)
{}

bool UOI::VisitDeclRefExpr(DeclRefExpr* I){
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    //llvm::outs()<<I->getNameInfo ()<<"\n";
    if (VarDecl *VD = dyn_cast<VarDecl>(I->getDecl())) {
        // It's a reference to a variable (a local, function parameter, global, or static data member).

        string sourcestr=I->getNameInfo ().getAsString ();
        if(I->getType().getAsString().compare("int")==0 && !(I->getType().isConstant(*astContext)) ){
            llvm::outs()<<"insert a unary before a int variable " << "\n";
            //llvm::outs()<<I->getNameInfo().getAsString ()<<"\n";
            //llvm::outs()<<I->getType().getAsString()<<"\n";

            //if(I->isRValue ()) llvm::outs()<<"right value\n";
            //if(I->isLValue ()) llvm::outs()<<"left value\n";

            //insert ++
            rewriter->ReplaceText(I->getBeginLoc (),sourcestr.length(),"++"+sourcestr );
            mutcount+=1;
            writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            //insert --
            rewriter->ReplaceText(I->getBeginLoc (),sourcestr.length()+2,"--"+sourcestr);
            mutcount+=1;
            writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
            //go back
            rewriter->ReplaceText(I->getBeginLoc (),sourcestr.length()+2,sourcestr);
        }
    }
    return true;
}
