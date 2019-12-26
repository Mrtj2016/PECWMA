#include "mutator.h"

BlockVisitor::BlockVisitor(CompilerInstance *CI,string path)
:   astContext(&(CI->getASTContext())),
    rewriter(Rewriter()),
    rule(vector<pair<unsigned,unsigned>>()),
    locflag(set<pair<unsigned,unsigned>>()),
    normalstmt(1),
    lastloc(0),
    if_flag(false),  //whether it is nesting if structure
    outfilename(path+"blockRule.txt"),
    out_block_func_filename(path+"blockContext.txt"),
    out_return_stmt_filename(path+"ReturnStmt.txt"),
    out_subblock_filename(path+"subBlock.txt")
{  // initialize private members
    if(path=="")
    {
        outfilename="";
        out_block_func_filename="";
        out_return_stmt_filename="";
        out_subblock_filename="";
    }
    rewriter.setSourceMgr(astContext->getSourceManager(), astContext->getLangOpts());
    system(("if [ -e "+outfilename+" ];then rm "+outfilename+"\nfi").c_str());
    system(("if [ -e "+out_block_func_filename+" ];then rm "+out_block_func_filename+"\nfi").c_str());
    system(("if [ -e "+out_return_stmt_filename+" ];then rm "+out_return_stmt_filename+"\nfi").c_str());
    system(("if [ -e "+out_subblock_filename+" ];then rm "+out_subblock_filename+"\nfi").c_str());
}

//algorithm to part the block
void BlockVisitor::pxsc()
{
    vector<block_struct>block_file;
    for(auto i=block_context.begin();i!=block_context.end();i++)
    {
        if (block_file.empty())
        {
                block_file.push_back(*i);
                continue;
        }
        for(auto j=block_file.end()-1;;j--)
        {
            if(j<block_file.begin() || (*i).begin>(*j).end)
            {
                block_file.insert(j+1,*i);
                break;
            }
        }
    }
#ifdef _DEBUG
    llvm::outs()<<"block_file size:\t"<<block_file.size()<<"\n";
    for (auto i=block_file.begin();i!=block_file.end();i++)
    {
        llvm::outs()<<(*i).begin<<" "<<(*i).end<<"\n";
    }
#endif
    //write the block rule to files
    ofstream outfile(outfilename,ofstream::out);
    if(!outfile){
        return;
    }
    for (auto i=block_file.begin();i!=block_file.end();i++)
    {
        outfile<<(*i).begin<<"~"<<(*i).end<<"\n";
    }
    outfile.close();
    ofstream outcontextfile(out_block_func_filename,ofstream::out);
    if(!outcontextfile){
        return;
    }
    for (auto i=block_file.begin();i!=block_file.end();i++)
    {
        outcontextfile<<(*i).func_begin<<"~"<<(*i).func_end<<"\n";
    }
    outcontextfile.close();
    ofstream subblockfile(out_subblock_filename,ofstream::out);
    if(!subblockfile){
        return;
    }
    for (auto i=block_file.begin();i!=block_file.end();i++)
    {
        subblockfile<<(*i).sub_begin<<"~"<<(*i).sub_end<<"\n";
    }
    subblockfile.close();
}

void BlockVisitor::dealStmt(clang::Stmt *s)
{
    clang::Stmt *statement=s;
    auto kai=astContext->getSourceManager().getPresumedLoc(statement->getBeginLoc()).getLine();
    auto jie=astContext->getSourceManager().getPresumedLoc(statement->getEndLoc ()).getLine();
    write_stmt_loc(kai,jie);
}

void BlockVisitor::dealStmt(clang::CompoundStmt *s)
{
    return;
    clang::CompoundStmt *statement=s;
    auto kai=astContext->getSourceManager().getPresumedLoc(statement->getBeginLoc()).getLine()+1;
    auto jie=astContext->getSourceManager().getPresumedLoc(statement->getEndLoc ()).getLine()-1;
    write_stmt_loc(kai,jie);
}
void BlockVisitor::write_stmt_loc(unsigned kai, unsigned jie)
{
#ifdef _DEBUG
    llvm::outs()<<"begin line:"<<kai<<"\n"<<"end line:"<<jie<<"\n";
#endif
    auto locstmt=make_pair(kai,jie);
    if(locflag.find(locstmt)==locflag.end()){
        block_struct tmp;
        tmp.begin=kai;
        tmp.end=jie;
        tmp.func_begin=func_context.first;
        tmp.func_end=func_context.second;
        tmp.sub_begin=subblock_context.first;
        tmp.sub_end=subblock_context.second;
        if(!(tmp.begin>=tmp.func_begin && tmp.end<=tmp.func_end))
        {
            tmp.func_begin=tmp.begin;
            tmp.func_end=tmp.end;
        }
        block_context.push_back(tmp);
        rule.push_back(locstmt);
        locflag.insert(locstmt);
        subblock_context.first=0;
        subblock_context.second=0;
    }
}

void BlockVisitor::decideStmt(clang::Stmt *s, int degree)
{
    //do not judge second time if a block is in others which has already appeared.
    if (degree==0 && astContext->getSourceManager().getPresumedLoc(s->getBeginLoc ()).getLine()<=lastloc)
    {
        return;
    }
    //this is for 1 degree, meaning not spread the block
    // if(degree>1)
    // {
    //     return;
    // }
    //expend the lastloc to the last statement of this block
    lastloc=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine();
    //judge the statement from it's type
    if (isa<CompoundStmt>(s))
    {
        //end the statement before if it exist
        if (normalstmt!=-1) {
            auto kai=normalstmt;
            auto jie=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc ()).getLine()-1;
            write_stmt_loc(kai,jie);
            normalstmt=-1;
        }
        dealStmt(cast<CompoundStmt>(s));
        CompoundStmt* compStmt = cast<CompoundStmt>(s);
        for(auto iter=compStmt->body_begin ();iter!=compStmt->body_end();iter++) {
                decideStmt(*iter,degree+1);
        }
        if (normalstmt!=-1){
            auto kai=normalstmt;
            auto jie=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine()-1;
            write_stmt_loc(kai,jie);
            normalstmt=-1;
        }
    }
    else
    {  //single statement
        if (normalstmt!=-1 && (isa<IfStmt>(s) || isa<WhileStmt>(s) || isa<SwitchStmt>(s) || isa<LabelStmt>(s)
                                || isa<ForStmt>(s) || isa<CaseStmt>(s) || isa<DefaultStmt>(s)) )
        {
            if (normalstmt!=-1){
                auto kai=normalstmt;
                auto jie=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc ()).getLine()-1;
                write_stmt_loc(kai,jie);
                normalstmt=-1;
            }
        }
        else if (normalstmt==-1 && !(isa<IfStmt>(s) || isa<WhileStmt>(s) || isa<SwitchStmt>(s) || isa<ForStmt>(s) || isa<NullStmt>(s)
                || isa<CaseStmt>(s) || isa<DefaultStmt>(s) || isa<DeclStmt>(s) || isa<ReturnStmt>(s) || isa<BreakStmt>(s)
                || isa<ContinueStmt>(s)|| isa<CallExpr>(s)))
        {
            normalstmt=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc ()).getLine();
        }
        if(isa<DeclStmt>(s))
        {
            auto kai=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc ()).getLine();
            if (normalstmt!=-1)
            {
                write_stmt_loc(normalstmt,kai-1);
            }
            auto jie=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine();
            write_stmt_loc(kai,jie);
            normalstmt=-1;
        }
        if(isa<ReturnStmt>(s))
        {
            auto kai=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc ()).getLine();
            if (normalstmt!=-1)
            {
                write_stmt_loc(normalstmt,kai-1);
            }
            auto jie=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine();
            write_stmt_loc(kai,jie);
            normalstmt=-1;
            ofstream outfile(out_return_stmt_filename,ofstream::app);
            outfile<<kai<<"~"<<jie<<"\n";
        }
        else if (isa<BreakStmt>(s))
        {
            auto kai=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc ()).getLine();
            if (normalstmt!=-1)
            {
                write_stmt_loc(normalstmt,kai-1);
            }
            auto jie=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine();
            write_stmt_loc(kai,jie);
            normalstmt=-1;
        }
        else if (isa<ContinueStmt>(s))
        {
            auto kai=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc ()).getLine();
            if (normalstmt!=-1)
            {
                write_stmt_loc(normalstmt,kai-1);
            }
            auto jie=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine();
            write_stmt_loc(kai,jie);
            normalstmt=-1;
        }
        else if (isa<NullStmt>(s))
        {
            auto kai=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc ()).getLine();
            if (normalstmt!=-1)
            {
                write_stmt_loc(normalstmt,kai);
            }
            normalstmt=-1;
        }
        else if (isa<CallExpr>(s))
        {
            auto kai=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc ()).getLine();
            if (normalstmt!=-1)
            {
                write_stmt_loc(normalstmt,kai-1);
            }
            auto jie=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine();
            write_stmt_loc(kai,jie);
            normalstmt=-1;
        }
        else if (isa<IfStmt>(s))
        {
            IfStmt *IfStatement = cast<IfStmt>(s);
            Stmt *thenStmt = IfStatement->getThen();
            Stmt *elseStmt = IfStatement->getElse();
            //get the 'true' subblock information, leave the 'false' subblock
            if(thenStmt)
            {
                subblock_context.first=astContext->getSourceManager().getPresumedLoc(thenStmt->getBeginLoc()).getLine();
                subblock_context.second=astContext->getSourceManager().getPresumedLoc(thenStmt->getEndLoc ()).getLine();
                if(isa<CompoundStmt>(thenStmt)) //minus the two braces of body begin and end
                {
                    subblock_context.first=astContext->getSourceManager().getPresumedLoc(cast<CompoundStmt>(thenStmt)->body_front()->getBeginLoc()).getLine();
                    subblock_context.second=astContext->getSourceManager().getPresumedLoc(cast<CompoundStmt>(thenStmt)->body_back()->getEndLoc()).getLine();
                }
            }
            //not nesting structure
            if (!if_flag)
            {
                dealStmt(s);
            }
            else
            {
                if_flag=false;
            }
            //then part
            decideStmt(thenStmt,degree+1);
            if (normalstmt!=-1)
            {
                auto kai=normalstmt;
                auto jie=astContext->getSourceManager().getPresumedLoc(thenStmt->getEndLoc ()).getLine();
                if (isa<CompoundStmt>(thenStmt))
                {
                    jie--;
                }
                write_stmt_loc(kai,jie);
                normalstmt=-1;
            }
            //else part
            if (elseStmt)
            {
                //is nesting if structure
                if (!isa<CompoundStmt>(s) && isa<IfStmt>(s))
                {
                    if_flag=true;
                }
                decideStmt(elseStmt,degree+1);
                if_flag=false;
            }
            // end this block
            if (normalstmt!=-1)
            {
                auto kai=normalstmt;
                auto jie=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine();
                if (elseStmt!=NULL && isa<CompoundStmt>(elseStmt))
                {
                    jie--;
                }
                else if (elseStmt==NULL && isa<CompoundStmt>(thenStmt))
                {
                    jie--;
                }
                write_stmt_loc(kai,jie);
                normalstmt=-1;
            }
        }
        else if(isa<WhileStmt>(s))
        {
            WhileStmt*whileStatement=cast<WhileStmt>(s);
            Stmt*bodyStmt=whileStatement->getBody();
            subblock_context.first=astContext->getSourceManager().getPresumedLoc(bodyStmt->getBeginLoc()).getLine();
            subblock_context.second=astContext->getSourceManager().getPresumedLoc(bodyStmt->getEndLoc ()).getLine();
            if(isa<CompoundStmt>(bodyStmt))
            {
                subblock_context.first=astContext->getSourceManager().getPresumedLoc(cast<CompoundStmt>(bodyStmt)->body_front()->getBeginLoc()).getLine();
                subblock_context.second=astContext->getSourceManager().getPresumedLoc(cast<CompoundStmt>(bodyStmt)->body_back()->getEndLoc()).getLine();
            }
            subblock_context.first=0-subblock_context.first;
            subblock_context.second=0-subblock_context.second;
            dealStmt(s);
            //body part
            decideStmt(bodyStmt,degree+1);
            if (normalstmt!=-1)
            {
                auto kai=normalstmt;
                auto jie=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine();
                if (isa<CompoundStmt>(bodyStmt))
                {
                    jie--;
                }
                write_stmt_loc(kai,jie);
                normalstmt=-1;
            }
        }
        else if(isa<DoStmt>(s))
        {
            DoStmt*doStatement=cast<DoStmt>(s);
            Stmt*bodyStmt=doStatement->getBody();
            subblock_context.first=astContext->getSourceManager().getPresumedLoc(bodyStmt->getBeginLoc()).getLine();
            subblock_context.second=astContext->getSourceManager().getPresumedLoc(bodyStmt->getEndLoc ()).getLine();
            if(isa<CompoundStmt>(bodyStmt))
            {
                subblock_context.first=astContext->getSourceManager().getPresumedLoc(cast<CompoundStmt>(bodyStmt)->body_front()->getBeginLoc()).getLine();
                subblock_context.second=astContext->getSourceManager().getPresumedLoc(cast<CompoundStmt>(bodyStmt)->body_back()->getEndLoc()).getLine();
            }
            subblock_context.first=0-subblock_context.first;
            subblock_context.second=0-subblock_context.second;
            dealStmt(s);
            //body part
            decideStmt(bodyStmt,degree+1);
            if (normalstmt!=-1)
            {
                auto kai=normalstmt;
                auto jie=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine();
                if (isa<CompoundStmt>(bodyStmt))
                {
                    jie--;
                }
                write_stmt_loc(kai,jie);
                normalstmt=-1;
            }
        }
        else if (isa<SwitchStmt>(s))
        { //switch structure is special
            SwitchStmt*switchStatement=cast<SwitchStmt>(s);
            auto bodyStmt=switchStatement->getBody ();
            subblock_context.first=astContext->getSourceManager().getPresumedLoc(bodyStmt->getBeginLoc()).getLine();
            subblock_context.second=astContext->getSourceManager().getPresumedLoc(bodyStmt->getEndLoc ()).getLine();
            if(isa<CompoundStmt>(bodyStmt))
            {
                subblock_context.first=astContext->getSourceManager().getPresumedLoc(cast<CompoundStmt>(bodyStmt)->body_front()->getBeginLoc()).getLine();
                subblock_context.second=astContext->getSourceManager().getPresumedLoc(cast<CompoundStmt>(bodyStmt)->body_back()->getEndLoc()).getLine();
            }
            dealStmt(s);
            //body part
            if(bodyStmt) {
                decideStmt(bodyStmt, degree+1);
            }
            if (normalstmt!=-1){
                auto kai=normalstmt;
                auto jie=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine();
                if (isa<CompoundStmt>(bodyStmt)) {
                    jie--;
                }
                write_stmt_loc(kai,jie);
                normalstmt=-1;
            }
        }
        else if(isa<CaseStmt>(s))
        { //the ast structure is different from what I think.
            CaseStmt*caseStmt=cast<CaseStmt>(s);
            auto loc=astContext->getSourceManager().getPresumedLoc(caseStmt->getCaseLoc()).getLine();
            write_stmt_loc(loc,loc);
            Stmt* subStmt=caseStmt->getSubStmt();
            decideStmt(subStmt,degree+1);
        }
        else if(isa<DefaultStmt>(s))
        { //the ast structure is different from what I think.
            DefaultStmt*defaultStmt=cast<DefaultStmt>(s);
            auto loc=astContext->getSourceManager().getPresumedLoc(defaultStmt->getDefaultLoc()).getLine();
            write_stmt_loc(loc,loc);
            Stmt* subStmt=defaultStmt->getSubStmt();
            decideStmt(subStmt,degree+1);
        }
        else if (isa<ForStmt>(s))
        {
            ForStmt*forStatement=cast<ForStmt>(s);
            Stmt*bodyStmt=forStatement->getBody();
            subblock_context.first=astContext->getSourceManager().getPresumedLoc(bodyStmt->getBeginLoc()).getLine();
            subblock_context.second=astContext->getSourceManager().getPresumedLoc(bodyStmt->getEndLoc ()).getLine();
            if(isa<CompoundStmt>(bodyStmt)) //minus the two braces of body begin and end
            {
                subblock_context.first=astContext->getSourceManager().getPresumedLoc(cast<CompoundStmt>(bodyStmt)->body_front()->getBeginLoc()).getLine();
                subblock_context.second=astContext->getSourceManager().getPresumedLoc(cast<CompoundStmt>(bodyStmt)->body_back()->getEndLoc()).getLine();
            }
            subblock_context.first=0-subblock_context.first;
            subblock_context.second=0-subblock_context.second;
            dealStmt(s);
            decideStmt(bodyStmt,degree+1);
            if (normalstmt!=-1)
            {
                auto kai=normalstmt;
                auto jie=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine();
                if (isa<CompoundStmt>(bodyStmt))
                {
                    jie--;
                }
                write_stmt_loc(kai,jie);
                normalstmt=-1;
            }
        }
        else if (isa<LabelStmt>(s))
        {
            LabelStmt*labelStatement=cast<LabelStmt>(s);
            Stmt*subStmt=labelStatement->getSubStmt();
            dealStmt(s);
            decideStmt(subStmt,degree+1);
            if (normalstmt!=-1)
            {
                auto kai=normalstmt;
                auto jie=astContext->getSourceManager().getPresumedLoc(s->getEndLoc ()).getLine();
                if (isa<CompoundStmt>(subStmt))
                {
                    jie--;
                }
                write_stmt_loc(kai,jie);
                normalstmt=-1;
            }
        }
    }
}

bool BlockVisitor::VisitStmt(clang::Stmt *s)
{
    if(!(astContext->getSourceManager().isWrittenInMainFile(s->getBeginLoc())))
    {
        return true;
    }
    decideStmt(s,0);
    return true;
}

bool BlockVisitor::VisitFunctionDecl(FunctionDecl*funcD)
{
    if(!(astContext->getSourceManager().isWrittenInMainFile(funcD->getSourceRange ().getBegin ())))
    {
        return true;
    }
    if (normalstmt!=-1)
    {
        auto kai=normalstmt;
        auto jie=astContext->getSourceManager().getPresumedLoc(funcD->getSourceRange ().getBegin ()).getLine()-1;
        if (kai<=jie){
            write_stmt_loc(kai,jie);
        }
        normalstmt=-1;
    }
    auto kai=astContext->getSourceManager().getPresumedLoc(funcD->getSourceRange ().getBegin ()).getLine();
    auto jie=astContext->getSourceManager().getPresumedLoc(funcD->getSourceRange ().getEnd ()).getLine();
    func_context.first=kai;
    func_context.second=jie;
    write_stmt_loc(kai,jie);
    //judge if this is declaration or define
    if (funcD->isThisDeclarationADefinition () && funcD->hasBody())
    {
        Stmt * 	st=funcD->getBody ();
        decideStmt(st,0);
    }
    lastloc=jie;
    return true;
}
