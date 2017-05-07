#ifndef AST_UTILS_H
#define AST_UTILS_H

#include "decls_reflection.h"

#include <clang/Basic/SourceManager.h>
#include <clang/AST/PrettyPrinter.h>
#include <clang/AST/ASTContext.h>

namespace reflection 
{

inline auto GetLocation(const clang::SourceLocation& loc, const clang::ASTContext* context)
{
    SourceLocation result;
    clang::PresumedLoc ploc = context->getSourceManager().getPresumedLoc(loc, false);
    result.fileName = ploc.getFilename();
    result.line = ploc.getLine();
    result.column = ploc.getColumn();
    
    return result;
}

template<typename Entity>
auto GetLocation(const Entity* decl, const clang::ASTContext* context)
{
    return GetLocation(decl->getLocation(), context); 
}

template<typename Entity>
std::string EntityToString(const Entity* decl, const clang::ASTContext* context)
{
    clang::PrintingPolicy policy(context->getLangOpts());
    
    policy.Bool = true;
    policy.AnonymousTagLocations = false;
    policy.SuppressUnwrittenScope = true;
    policy.Indentation = 4;
    policy.UseVoidForZeroParams = false;
    
    std::string result;
    {
        llvm::raw_string_ostream os(result);
    
        decl->print(os, policy);
    }
    return result;
}

} // reflection

#endif // AST_UTILS_H
