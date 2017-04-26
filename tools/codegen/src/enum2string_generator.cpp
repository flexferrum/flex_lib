#include "enum2string_generator.h"
#include "options.h"

#include <clang/ASTMatchers/ASTMatchers.h>

#include <iostream>

using namespace clang::ast_matchers;

namespace codegen 
{
namespace  
{
DeclarationMatcher enumMatcher = 
        enumDecl(isExpansionInMainFile()).bind("enum");
}

Enum2StringGenerator::Enum2StringGenerator(const Options &opts)
    : BasicGenerator(opts)
{
    
}

void Enum2StringGenerator::SetupMatcher(clang::ast_matchers::MatchFinder &finder, clang::ast_matchers::MatchFinder::MatchCallback *defaultCallback)
{
    finder.addMatcher(enumMatcher, defaultCallback);    
}

void Enum2StringGenerator::HandleMatch(const clang::ast_matchers::MatchFinder::MatchResult &matchResult)
{
    if (const clang::EnumDecl* decl = matchResult.Nodes.getNodeAs<clang::EnumDecl>("enum"))
    {
        EnumDescriptor descriptor;
        // Get name of the found enum
        descriptor.enumName = decl->getName();
        // Get 'isScoped' flag
        descriptor.isScoped = decl->isScoped();
        
        // Get enum items
        for (auto itemDecl : decl->enumerators())
            descriptor.enumItems.push_back(itemDecl->getName());
        
        std::sort(descriptor.enumItems.begin(), descriptor.enumItems.end());
        m_foundEnums.push_back(std::move(descriptor));       
    }    
}

void Enum2StringGenerator::WriteHeaderPreamble(CppSourceStream &hdrOs)
{
    hdrOs << out::header_guard(m_options.outputHeaderName) << "\n";
    
    // Include input files (directly, by path)
    for (auto fileName : m_options.inputFiles)
    {
        std::replace(fileName.begin(), fileName.end(), '\\', '/');
        hdrOs << "#include \"" << fileName << "\"\n";
    }
    
    // Necessary library files
    hdrOs << "#include <algorithm>\n";
    hdrOs << "#include <utility>\n\n";
}

void Enum2StringGenerator::WriteHeaderPostamble(CppSourceStream &hdrOs)
{
    hdrOs << out::scope_exit;
}

void Enum2StringGenerator::WriteHeaderContent(CppSourceStream &hdrOs)
{
    for (auto& enumInfo : m_foundEnums)
    {
        WriteEnumToStringConversion(hdrOs, enumInfo);
        WriteEnumFromStringConversion(hdrOs, enumInfo);
    }
}

// Enum item to string conversion writer
void Enum2StringGenerator::WriteEnumToStringConversion(CppSourceStream &hdrOs, const EnumDescriptor& enumDescr)
{
    out::ScopedParams params(hdrOs, {
        {"enumName", enumDescr.enumName},
        {"prefix", enumDescr.isScoped ? enumDescr.enumName + "::" : std::string()}
    });
    
    out::BracedStreamScope fnScope("inline const char* $enumName$ToString($enumName$ e)", "\n\n");
    hdrOs << fnScope;
    {
        out::BracedStreamScope switchScope("switch (e)", "\n");
        hdrOs << out::new_line << switchScope;
        out::OutParams innerParams;
        for (auto& i : enumDescr.enumItems)
        {
            innerParams["itemName"] = i;
            hdrOs << out::with_params(innerParams)
                  << out::new_line(-1) << "case $prefix$$itemName$:"
                  << out::new_line << "return \"$itemName$\";";
        }
    }
    hdrOs << out::new_line << "return \"Unknown Item\";";
}

// String to enum conversion writer
void Enum2StringGenerator::WriteEnumFromStringConversion(CppSourceStream &hdrOs, const EnumDescriptor& enumDescr)
{
    out::ScopedParams params(hdrOs, {
        {"enumName", enumDescr.enumName},
        {"prefix", enumDescr.isScoped ? enumDescr.enumName + "::" : std::string()}
    });
    
    out::BracedStreamScope fnScope("inline $enumName$ StringTo$enumName$(const char* itemName)", "\n\n");
    hdrOs << fnScope;
    {
        out::BracedStreamScope itemsScope("static std::pair<const char*, $enumName$> items[] = ", ";\n");
        hdrOs << out::new_line << itemsScope;

        out::OutParams& innerParams = params.GetParams();
        for (auto& i : enumDescr.enumItems)
        {
            innerParams["itemName"] = i;
            hdrOs << out::new_line << "{\"$itemName$\", $prefix$$itemName$},";
        }    
    }

    hdrOs << R"(
    auto p = std::lower_bound(begin(items), end(items), itemName,
                      [](auto&& i, auto&& v) {return strcmp(i.first, v) < 0;});
    
    if (p == end(items) || strcmp(p->first, itemName) != 0)
        throw std::bad_cast();
    
    return p->second;)";
}
} // codegen

codegen::GeneratorPtr CreateEnum2StringGen(const codegen::Options& opts)
{
    return std::make_unique<codegen::Enum2StringGenerator>(opts);
}
