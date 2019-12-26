#include "operator.h"

ROV::ROV(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-ROV"+type)
{}

bool ROV::VisitStmt (Stmt *st) {
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    if (CallExpr *callexpr = dyn_cast<CallExpr>(st)) {
        //llvm::outs()<<"callee function name: "<<callexpr->getDirectCallee ()->getNameInfo().getAsString()<<"\n";
        string ct="operator<<";//ping bi xi qu han shu.
        if(ct.compare(callexpr->getDirectCallee ()->getNameInfo().getAsString())==0){
            return true;
        }
        if(callexpr->getNumCommas ()>=1){
            llvm::outs() << "** Exchange the function argument order **\n";
            //call->setArg(1,call->getArg(0));
            //if(callexpr->getArg(0)->getType()==callexpr->getArg(0)->getTpye()){}
            unsigned argnum=callexpr->getNumArgs ();
            for(int i=0;i<argnum;i++){
                for(int j=i+1;j<argnum;j++){
                    if(callexpr->getArg(i)->getType().getAsString().compare(callexpr->getArg(j)->getType().getAsString())==0){
                        string arg0=rewriter->getRewrittenText(callexpr->getArg(i)->getSourceRange());
                        string arg1=rewriter->getRewrittenText(callexpr->getArg(j)->getSourceRange());
                        //llvm::outs()<<arg0<<arg1<<"\n";
                        rewriter->ReplaceText(callexpr->getArg(i)->getExprLoc(),arg0.length(),arg1);
                        rewriter->ReplaceText(callexpr->getArg(j)->getExprLoc(),arg1.length(),arg0);
                        mutcount+=1;
                        writemut(mutantdir+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
                        //recover
                        rewriter->ReplaceText(callexpr->getArg(i)->getExprLoc(),arg1.length(),arg0);
                        rewriter->ReplaceText(callexpr->getArg(j)->getExprLoc(),arg0.length(),arg1);
                    }
                }
            }
        }
        //rewriter->ReplaceText(call->getBeginLoc(), "");
        //rewriter->ReplaceText(call->getEndLoc(), "");
    }
    return true;
}
