/**
**/

#include "mutator.h"

EseVisitor::EseVisitor(CompilerInstance *CI, int begin_loc, int end_loc,vector<var_ctx>*bv,vector<func_ctx>*bf,vector<var_ctx>*bd)
:   astContext(&(CI->getASTContext())),
    rewriter(Rewriter()),
    block_var(bv),
    block_func(bf),
    block_decl(bd),
    blockBeginLine(begin_loc),
    blockEndLine(end_loc)
{  // initialize private members
    rewriter.setSourceMgr(astContext->getSourceManager(), astContext->getLangOpts());
}

void EseVisitor::write_var(string name, string type, unsigned line, unsigned flag)
{
    if(flag==0)
    {
        var_ctx new_var;
        new_var.name=name;
        new_var.type=type;
        new_var.line=line;
        new_var.kind="modified";
        new_var.in=stmtkind[line];
        (*block_var).push_back(new_var);
    }
    // else if(flag==1) {
    //     var_ctx new_var;
    //     new_var.name=name;
    //     new_var.type=type;
    //     new_var.line=line;
    //     new_var.kind="declaration";
    //     new_var.in=stmtkind[line];
    //     (*block_decl).push_back(new_var);
    // }
    else if(flag==2)
    {
        var_ctx new_var;
        new_var.name=name;
        new_var.type=type;
        new_var.line=line;
        new_var.kind="return";
        new_var.in=stmtkind[line];
        (*block_var).push_back(new_var);
    }
}


bool EseVisitor::VisitBinaryOperator(BinaryOperator *E)
{
    //not in main file
    if(!(astContext->getSourceManager().isWrittenInMainFile(E->getOperatorLoc())))
    {
        return true;
    }
    //not in block range
    if(astContext->getSourceManager().getPresumedLoc(E->getExprLoc ()).getLine()<blockBeginLine||
    astContext->getSourceManager().getPresumedLoc(E->getExprLoc ()).getLine()>blockEndLine)
    {
        return true;
    }
    //if it is assignment operator
    if(E->isAssignmentOp())
    {
#ifdef _INFO
        llvm::outs()<<"assignment operator found===================="<<"\n";
#endif
        if(E->isCompoundAssignmentOp())  //like '+='
        {
            if(E->getLHS ()->isLValue ())
            {
                auto locStart = astContext->getSourceManager().getPresumedLoc(E->getExprLoc ());
                //llvm::outs()<<E->getExprLoc ().printToString (astContext->getSourceManager())<<"\n";
                //llvm::outs()<<E->getExprLoc ().getRawEncoding ()<<"\n";
                //const string text = Lexer::getSourceText(E->getCharRange(), astContext->getSourceManager(), astContext->getLangOpts());
                string text = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(E->getLHS ()->getSourceRange()),
                                                            astContext->getSourceManager(), astContext->getLangOpts());
                //try to judge if it is a array type, like 'buff[pos]'
                string type;
                auto pos=text.find("[");
                //meaning it is a array
                if(pos!=string::npos && text.find("\"")==string::npos)
                {
                    //text=text.substr(0,pos);
                    type=E->getLHS()->getType () .getAsString () + " Array";
                }
                else{
                    type=E->getLHS()->getType () .getAsString ();
                }
#ifdef _INFO
                llvm::outs()<<locStart.getLine()<<"\t";
                llvm::outs()<<text<<"\t";
                llvm::outs()<<type<<"\n";
#endif
                write_var(text,type,locStart.getLine());
            }
        }
        else
        {  // is '=', do the same
            if(E->getLHS ()->isLValue ())
            {
                //clang::SourceRange loc = Variable16->getSourceRange();
                //locEnd = astContext->getSourceManager().getPresumedLoc(loc.getEnd());
                //std::cout << locStart.getLine()<< ":" << locEnd.getLine() << std::endl;
                //std::cout << locStart.getColumn() <<":" << locEnd.getColumn() << std::endl;
                auto locStart = astContext->getSourceManager().getPresumedLoc(E->getExprLoc ());
                //llvm::outs()<<E->getExprLoc ().printToString (astContext->getSourceManager())<<"\n";
                string text = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(E->getLHS ()->getSourceRange()),
                                                            astContext->getSourceManager(), astContext->getLangOpts());

                string type;
                auto pos=text.find("[");
                //meaning this is a array
                if(pos!=string::npos && text.find("\"")==string::npos)
                {
                    //text=text.substr(0,pos);
                    type=E->getLHS()->getType () .getAsString () + " Array";
                }
                else
                {
                    type=E->getLHS()->getType () .getAsString ();
                }
#ifdef _INFO
                llvm::outs()<<locStart.getLine()<<"\t";
                llvm::outs()<<text<<"\t";
                llvm::outs()<<type<<"\n";
#endif
                write_var(text,type,locStart.getLine());
            }
        }
    }
    else if(E->isComparisonOp())
    {
        auto locStartL = astContext->getSourceManager().getPresumedLoc(E->getExprLoc ());
        string textL = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(E->getLHS ()->getSourceRange()),
                                                    astContext->getSourceManager(), astContext->getLangOpts());
        string typeL=E->getLHS()->getType () .getAsString () ;
        auto pos=textL.find("[");
        //meaning this is a array
        if(pos!=string::npos && textL.find("\"")==string::npos)
        {
            //text=text.substr(0,pos);
            typeL += " Array";
        }
        auto locStartR = astContext->getSourceManager().getPresumedLoc(E->getExprLoc ());
        string textR = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(E->getRHS ()->getSourceRange()),
                                                    astContext->getSourceManager(), astContext->getLangOpts());
        string typeR=E->getRHS()->getType () .getAsString ();
        pos=textR.find("[");
        //is a array
        if(pos!=string::npos && textR.find("\"")==string::npos)
        {
            //text=text.substr(0,pos);
            typeR+= " Array";
        }
#ifdef _INFO
        llvm::outs()<<locStartL.getLine()<<"\t";
        llvm::outs()<<textL<<"\t";
        llvm::outs()<<typeL<<"\n";
        llvm::outs()<<locStartR.getLine()<<"\t";
        llvm::outs()<<textR<<"\t";
        llvm::outs()<<typeR<<"\n";
#endif
        if(!E->getLHS()->isLValue())
        {
            textL='%'+textL;
        }
        if(!E->getRHS()->isLValue())
        {
            textR='%'+textR;
        }
        write_var(textL,typeL,locStartL.getLine());
        write_var(textR,typeR,locStartR.getLine());
    }
    else if(E->isEqualityOp())
    {
        auto locStartL = astContext->getSourceManager().getPresumedLoc(E->getExprLoc ());
        string textL = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(E->getLHS ()->getSourceRange()),
                                                    astContext->getSourceManager(), astContext->getLangOpts());
        string typeL=E->getLHS()->getType().getAsString ();
        auto pos=textL.find("[");
        //meaning this is a array
        if(pos!=string::npos && textL.find("\"")==string::npos)
        {
            //text=text.substr(0,pos);
            typeL+=" Array";
        }
        auto locStartR = astContext->getSourceManager().getPresumedLoc(E->getExprLoc ());
        string textR = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(E->getRHS ()->getSourceRange()),
                                                    astContext->getSourceManager(), astContext->getLangOpts());
        string typeR=E->getRHS()->getType().getAsString ();
        pos=textR.find("[");
        //is a array
        if(pos!=string::npos && textR.find("\"")==string::npos)
        {
            //text=text.substr(0,pos);
            typeR+=" Array";
        }
#ifdef _INFO
        llvm::outs()<<locStartL.getLine()<<"\t";
        llvm::outs()<<textL<<"\t";
        llvm::outs()<<typeL<<"\n";
        llvm::outs()<<locStartR.getLine()<<"\t";
        llvm::outs()<<textR<<"\t";
        llvm::outs()<<typeR<<"\n";
#endif
        if(!E->getLHS()->isLValue())
        {
            textL='%'+textL;
        }
        if(!E->getRHS()->isLValue())
        {
            textR='%'+textR;
        }
        write_var(textL,typeL,locStartL.getLine());
        write_var(textR,typeR,locStartR.getLine());
    }
    return true;
}

bool EseVisitor::VisitUnaryOperator(UnaryOperator *uop)
{
    if(!(astContext->getSourceManager().isWrittenInMainFile(uop->getExprLoc())))
    {
        return true;
    }
    if(astContext->getSourceManager().getPresumedLoc(uop->getExprLoc()).getLine()<blockBeginLine||
    astContext->getSourceManager().getPresumedLoc(uop->getExprLoc()).getLine()>blockEndLine)
    {
        return true;
    }
    //it is isIncrement or Decrement Operator, like 'a++' or '--b'
    if(uop->isIncrementDecrementOp()){
#ifdef _INFO
        llvm::outs()<<"unary operator found-------------------------"<<"\n";
#endif
        if(uop->getSubExpr ()->isLValue ())
        {
            auto locStart = astContext->getSourceManager().getPresumedLoc(uop->getExprLoc ());

            string text = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(uop->getSubExpr()->getSourceRange()),
                                                        astContext->getSourceManager(), astContext->getLangOpts());

            //try to judge if it is a array type, like 'buff[pos]'
            string type;
            auto pos=text.find("[");
            //meaning it is a array
            if(pos!=string::npos && text.find("\"")==string::npos)
            {
                //text=text.substr(0,pos);
                type=uop->getSubExpr()->getType ().getAsString () + " Array";
            }
            else{
                type=uop->getSubExpr()->getType ().getAsString ();
            }
#ifdef _INFO
            llvm::outs()<<locStart.getLine()<<"\t";
            llvm::outs()<<text<<"\t";
            llvm::outs()<<type<<"\n";
#endif
            write_var(text,type,locStart.getLine());
        }
    }
    return true;
}

//not used
bool EseVisitor::VisitVarDecl(VarDecl *variDecl)
{
    if(!(astContext->getSourceManager().isWrittenInMainFile(variDecl->getBeginLoc())))
    {
        return true;
    }
    if(astContext->getSourceManager().getPresumedLoc(variDecl->getBeginLoc()).getLine()<blockBeginLine||
        astContext->getSourceManager().getPresumedLoc(variDecl->getEndLoc()).getLine()>blockEndLine)
    {
        return true;
    }
#ifdef _INFO
    llvm::outs()<<">>>>>>>variable declaration statement found<<<<<<<<<"<<"\n";
#endif
    string varName = variDecl->getQualifiedNameAsString();
    string varType = variDecl->getType().getAsString();
    auto locStart = astContext->getSourceManager().getPresumedLoc(variDecl->getSourceRange().getBegin());
#ifdef _INFO
    llvm::outs()<<locStart.getLine()<<"\t";
    llvm::outs()<<varName<<"\t"<<varType<<"\n";
#endif
    const clang::Type *type = variDecl->getType().getTypePtr();
    if(type->isConstantArrayType() || type->isArrayType ())
    {
        const clang::ArrayType *Array = type->castAsArrayTypeUnsafe();
#ifdef _INFO
        llvm::outs() << "Array type: "  << Array->getElementType().getAsString() << "\n";
#endif
    }
    write_var(varName,varType,locStart.getLine(),1);
    return true;
}

bool EseVisitor::VisitCallExpr(CallExpr *CExpr) {
    if(!(astContext->getSourceManager().isWrittenInMainFile(CExpr->getBeginLoc())))
    {
        return true;
    }
    if(astContext->getSourceManager().getPresumedLoc(CExpr->getBeginLoc()).getLine()<blockBeginLine||
        astContext->getSourceManager().getPresumedLoc(CExpr->getEndLoc()).getLine()>blockEndLine)
    {
        return true;
    }
    //llvm::outs()<<"callee function name: "<<callexpr->getDirectCallee ()->getNameInfo().getAsString()<<"\n";
    //if(callexpr->getArg(0)->getType()==callexpr->getArg(0)->getTpye()){}
#ifdef _INFO
    llvm::outs()<<"call expression found>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<"\n";
#endif
    auto locStart = astContext->getSourceManager().getPresumedLoc(CExpr->getBeginLoc());
    //argument number of the function call
    unsigned argnum=CExpr->getNumArgs ();
    //function name
    const string calleeName = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(
        CExpr->getCallee()->getSourceRange()),astContext->getSourceManager(), astContext->getLangOpts());
    func_ctx new_func;
    new_func.name=calleeName;
    new_func.line=locStart.getLine();
    new_func.in=stmtkind[locStart.getLine()];
#ifdef _INFO
    llvm::outs()<<locStart.getLine()<<"\t";
    llvm::outs()<<calleeName<<"\n";
#endif
    for(int i=0;i<argnum;i++)
    {
        string text = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(CExpr->getArg(i)->getSourceRange()),
                                                    astContext->getSourceManager(), astContext->getLangOpts());

        //try to judge if it is a array type, like 'buff[pos]'
        string type;
        auto pos=text.find("[");
        //meaning it is a array
        if(pos!=string::npos && text.find("\"")==string::npos)
        {
            //text=text.substr(0,pos);
            type=CExpr->getArg(i)->getType().getAsString () + " Array";
        }
        else{
            type=CExpr->getArg(i)->getType().getAsString ();
        }
#ifdef _INFO
        llvm::outs()<<"\t\t";
        llvm::outs()<<text<<"\t";
        llvm::outs()<<type<<"\n";
#endif
        new_func.para.push_back(make_pair(text,type));
    }
    (*block_func).push_back(new_func);
    return true;
}

bool EseVisitor::VisitStmt(Stmt *s)
{
    if(!(astContext->getSourceManager().isWrittenInMainFile(s->getBeginLoc())))
    {
        return true;
    }
    if(astContext->getSourceManager().getPresumedLoc(s->getBeginLoc()).getLine()<blockBeginLine||
        astContext->getSourceManager().getPresumedLoc(s->getEndLoc()).getLine()>blockEndLine)
    {
        return true;
    }
    if(isa<IfStmt>(s))
    {
        auto line=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc()).getLine();
        stmtkind[line]="if";
    }
    else if(isa<WhileStmt>(s))
    {
        auto line=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc()).getLine();
        stmtkind[line]="while";
    }
    else if(isa<ForStmt>(s)){
        auto line=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc()).getLine();
        stmtkind[line]="for";
    }
    else if(isa<SwitchStmt>(s))
    {
        auto line=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc()).getLine();
        stmtkind[line]="switch";
    }
    else if(isa<BreakStmt>(s))
    {
        auto line=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc()).getLine();
        stmtkind[line]="break";
    }
    else if(isa<CaseStmt>(s))
    {
        auto line=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc()).getLine();
        stmtkind[line]="case";
    }
    else if(isa<DefaultStmt>(s))
    {
        auto line=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc()).getLine();
        stmtkind[line]="default";
    }
    else if(isa<ContinueStmt>(s))
    {
        auto line=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc()).getLine();
        stmtkind[line]="continue";
    }
    else if(isa<ReturnStmt>(s))
    {
        auto line=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc()).getLine();
        stmtkind[line]="return";
        auto rets=cast<ReturnStmt>(s);
    #ifdef _INFO
        llvm::outs()<<"**********return statement found*************"<<"\n";
    #endif
        auto expr=rets->getRetValue();
        if(!expr)
        {
            return true;
        }
        string varName = Lexer::getSourceText(clang::CharSourceRange::getTokenRange(expr->getSourceRange()),
                                                astContext->getSourceManager(), astContext->getLangOpts());
        string varType = expr->getType().getAsString();
        auto locStart = astContext->getSourceManager().getPresumedLoc(s->getSourceRange().getBegin());
    #ifdef _INFO
        llvm::outs()<<locStart.getLine()<<"\t";
        llvm::outs()<<varName<<"\t"<<varType<<"\n";
    #endif
        if(!expr->isLValue())
        {
            varName='%'+varName;
        }
        write_var(varName,varType,locStart.getLine(),2);
    }
    else
    {
        auto line=astContext->getSourceManager().getPresumedLoc(s->getBeginLoc()).getLine();
        if(stmtkind.find(line)==stmtkind.end())
            stmtkind[line]="normal";
    }
    return true;
}


void EseVisitor::show()
{
#ifdef _INFO
    llvm::outs()<<"---------------------------------------\n";
    llvm::outs()<<"block_var size: "<<block_var->size()<<"\n";
    for (int i=0;i<(*block_var).size();i++)
    {
        llvm::outs()<<(*block_var)[i].name<<"\t"<<(*block_var)[i].type<<"\t"<<(*block_var)[i].line<<"\t"<<(*block_var)[i].kind<<"\n";
    }
    llvm::outs()<<"block_func size: "<<block_func->size()<<"\n";
    for (int i=0;i<(*block_func).size();i++)
    {
        llvm::outs()<<(*block_func)[i].name<<"\t"<<(*block_func)[i].line<<"\n";
        for (int j=0;j<(*block_func)[i].para.size();j++)
        {
            llvm::outs()<<"\t\t\t"<<(*block_func)[i].para[j].first<<"\t"<<(*block_func)[i].para[j].second<<"\n";
        }
    }
    llvm::outs()<<"block_decl size: "<<block_decl->size()<<"\n";
    for (int i=0;i<(*block_decl).size();i++)
    {
        llvm::outs()<<(*block_decl)[i].name<<"\t"<<(*block_decl)[i].type<<"\t"<<(*block_decl)[i].line<<"\t"<<(*block_decl)[i].kind<<"\n";
    }
    for (auto i=stmtkind.begin();i!=stmtkind.end();i++)
    {
        llvm::outs()<<i->first<<"\t"<<i->second<<"\n";
    }
#endif
}
