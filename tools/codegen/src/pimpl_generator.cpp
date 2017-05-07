#include "pimpl_generator.h"
#include "options.h"
#include "ast_reflector.h"

#include <clang/ASTMatchers/ASTMatchers.h>

#include <iostream>

using namespace clang::ast_matchers;

namespace codegen 
{
namespace  
{
DeclarationMatcher pimplMatcher = cxxRecordDecl
        (isExpansionInMainFile(), isDerivedFrom("flex_lib::pimpl")).bind("pimpl");
}


PimplGenerator::PimplGenerator(const codegen::Options &opts)
    : BasicGenerator(opts)
{
    
}

void PimplGenerator::SetupMatcher(clang::ast_matchers::MatchFinder &finder, clang::ast_matchers::MatchFinder::MatchCallback *defaultCallback)
{
    finder.addMatcher(pimplMatcher, defaultCallback);    
}

void PimplGenerator::HandleMatch(const clang::ast_matchers::MatchFinder::MatchResult &matchResult)
{
    if (const clang::CXXRecordDecl* decl = matchResult.Nodes.getNodeAs<clang::CXXRecordDecl>("pimpl"))
    {
        reflection::AstReflector reflector(matchResult.Context);
        
        auto ci = reflector.ReflectClass(decl, m_namespaces);
        
        std::cout << "### Pimpl declaration found: " << ci->GetFullQualifiedName(false) << std::endl;
        std::cout << "Methods: " << std::endl;
        
        for (auto mi : ci->methods)
        {
            std::cout << mi->fullPrototype << std::endl;
            std::cout << "Return type: " << mi->returnType->getPrintedName() << std::endl;
            std::cout << "Params: ";
            for (auto& pi : mi->params)
            {
                std::cout << pi.type->getPrintedName() << ", ";
            }
            std::cout << std::endl;
        }
        // reflection::AstReflector reflector;
        
        // reflector.ReflectEnum(decl, m_namespaces, matchResult.Context);
    }        
}

bool PimplGenerator::Validate()
{
    return true;    
}

void PimplGenerator::WriteHeaderPreamble(CppSourceStream &hdrOs)
{
    
}

void PimplGenerator::WriteHeaderContent(CppSourceStream &hdrOs)
{
    
}

void PimplGenerator::WriteHeaderPostamble(CppSourceStream &hdrOs)
{
    
}

void PimplGenerator::WriteSourcePreamble(CppSourceStream &srcOs)
{
    
}

void PimplGenerator::WriteSourceContent(CppSourceStream &srcOs)
{
    
}

void PimplGenerator::WriteSourcePostamble(CppSourceStream &srcOs)
{
    
}

} // codegen

codegen::GeneratorPtr CreatePimplGen(const codegen::Options& opts)
{
    return std::make_unique<codegen::PimplGenerator>(opts);
}
