#include "mutator.h"

MutatorASTConsumer::MutatorASTConsumer(CompilerInstance *CI,string actype,string path,
    int begin_loc,int end_loc,
    vector<var_ctx>*bv,vector<func_ctx>*bf,vector<var_ctx>*bd){

    visitorblock=nullptr;
    visitormutator=nullptr;
    visitorese=nullptr;
    actionType=actype;
    if(actionType=="mutator") {
        vector<MutatorOp*>op;
        static std::map<string,int>opmp;
        opmp["ABV"]=1,opmp["AOR"]=2,opmp["BOR"]=3,opmp["CSR"]=4,opmp["LOR"]=5,opmp["LVR"]=6;
        opmp["MCD"]=7,opmp["ROR"]=8,opmp["ROV"]=9,opmp["SOR"]=10,opmp["STDC"]=11,opmp["UOI"]=12,opmp["UOR"]=13;
        for(auto  i=bv->begin();i!=bv->end();i++){
            switch(opmp[(*i).name]){
                case 1:
                {
                    MutatorOp*aor=new ABV(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
                case 2:
                {
                    MutatorOp*aor=new AOR(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
                case 3:
                {
                    MutatorOp*aor=new BOR(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
                case 4:
                {
                    MutatorOp*aor=new CSR(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
                case 5:
                {
                    MutatorOp*aor=new LOR(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
                case 6:
                {
                    MutatorOp*aor=new LVR(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
                case 7:
                {
                    MutatorOp*aor=new MCD(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
                case 8:
                {
                    MutatorOp*aor=new ROR(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
                case 9:
                {
                    MutatorOp*aor=new ROV(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
                case 10:
                {
                    MutatorOp*aor=new SOR(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
                case 11:
                {
                    MutatorOp*aor=new STDC(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
                case 12:
                {
                    MutatorOp*aor=new UOI(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
                case 13:
                {
                    MutatorOp*aor=new UOR(&(CI->getASTContext()),new Rewriter(),0,"","");
                    op.push_back(aor);
                }
                break;
            }
        }
        visitormutator=new MutatorVisitor(CI,path,op,".c");
    }
    else if(actionType=="blockrule") {
        visitorblock=new BlockVisitor(CI,path);
    }
    else if(actionType=="ese") {
        visitorese=new EseVisitor(CI,begin_loc,end_loc,bv,bf,bd);
    }

}// initialize the visitor

void MutatorASTConsumer::HandleTranslationUnit(ASTContext &Context) {
    // llvm::errs() << "********* The whole TU *************\n";
    // Context.getTranslationUnitDecl()->dump();
    if(actionType=="mutator") {
        visitormutator->TraverseDecl(Context.getTranslationUnitDecl());
    }
    else if(actionType=="blockrule") {
        visitorblock->TraverseDecl(Context.getTranslationUnitDecl());
        visitorblock->pxsc();
    }
    else if(actionType=="ese") {
        visitorese->TraverseDecl(Context.getTranslationUnitDecl());
        visitorese->show();
    }
    //visitor->TraverseDecl(Context.getTranslationUnitDecl());
}
MutatorASTConsumer::~MutatorASTConsumer(){
    if(visitorblock!=nullptr) {
        delete visitorblock;
        visitorblock=nullptr;
    }
    if(visitormutator!=nullptr) {
        delete visitormutator;
        visitormutator=nullptr;
    }
    if(visitorese!=nullptr) {
        delete visitorese;
        visitorese=nullptr;
    }
}
