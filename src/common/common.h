#ifndef COMMON_H
#define COMMON_H

#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"

#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include<dirent.h>
#include <algorithm>
#include <string.h>


#define _DEBUG

typedef struct {
    std::string name;
    std::string type;
    unsigned line;
    std::string kind;
    std::string in;
}var_ctx;

typedef struct {
    std::string name;
    unsigned line;
    std::string in;
    std::vector<std::pair<std::string,std::string>>para;  //name,type
}func_ctx;

typedef struct {
    int begin;
    int end;
    int func_begin;
    int func_end;
    int sub_begin;
    int sub_end;
}block_struct;

#endif
