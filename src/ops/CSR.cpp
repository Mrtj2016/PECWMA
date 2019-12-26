#include "operator.h"

CSR::CSR(ASTContext *ac,Rewriter* rt,int count,string dir,std::string type):MutatorOp(ac,rt,count,dir,"-CSR"+type)
{}

bool CSR::VisitStmt (Stmt *st) {
    RewriteBuffer & RewriteBuf=rewriter->getEditBuffer(rewriter->getSourceMgr().getMainFileID());
    std::string wfilename=mutantdir;
    if(isa<IfStmt>(st)){
        //st->dump ();
  		IfStmt* IfStatement = cast<IfStmt>(st);
        string sourcestr=rewriter->getRewrittenText(IfStatement->getCond()->getSourceRange());
  		Stmt*IfCond = IfStatement->getCond();
        //repalce condition with  1
  		rewriter->ReplaceText(IfCond->getBeginLoc(),sourcestr.length(),"1");
        mutcount+=1;
        writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        //replace condition with 0
        rewriter->ReplaceText(IfCond->getBeginLoc(),1,"0");
        mutcount+=1;
        writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        //go back to the source condition
        rewriter->ReplaceText(IfCond->getBeginLoc(),1,sourcestr);
        //negate the condition
        rewriter->ReplaceText(IfCond->getBeginLoc(),sourcestr.length(),"!("+sourcestr+")");
        mutcount+=1;
        writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        //go back to the source condition
        rewriter->ReplaceText(IfCond->getBeginLoc(),sourcestr.length()+3,sourcestr);
  		llvm::outs()<<"** rewrite  if condition\n";
  	}
    if(isa<WhileStmt>(st)){
        //st->dump ();
        WhileStmt* WhileStatement = cast<WhileStmt>(st);
        string sourcestr=rewriter->getRewrittenText(WhileStatement->getCond()->getSourceRange());
        Stmt*WhileCond = WhileStatement->getCond();
        //repalce the condition with 1
        rewriter->ReplaceText(WhileCond->getBeginLoc(),sourcestr.length(),"1");
        mutcount+=1;
        writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        //replace the condition with 0
        rewriter->ReplaceText(WhileCond->getBeginLoc(),1,"0");
        mutcount+=1;
        writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        //go back to the source condition
        rewriter->ReplaceText(WhileCond->getBeginLoc(),1,sourcestr);
        //negate the condition
        rewriter->ReplaceText(WhileCond->getBeginLoc(),sourcestr.length(),"!("+sourcestr+")");
        mutcount+=1;
        writemut(wfilename+std::to_string(mutcount)+mutangtype,std::string(RewriteBuf.begin(), RewriteBuf.end()));
        //go back to the source condition
        rewriter->ReplaceText(WhileCond->getBeginLoc(),sourcestr.length()+3,sourcestr);
        llvm::outs()<<"** rewrite  while condition\n";
    }
    return true;
}
