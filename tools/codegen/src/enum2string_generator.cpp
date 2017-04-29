#include "enum2string_generator.h"
#include "options.h"
#include "ast_reflector.h"

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
        reflection::AstReflector reflector;
        
        reflector.ReflectEnum(decl, m_namespaces, matchResult.Context);
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
    hdrOs << "#include <flex_lib/stringized_enum.h>\n";
    hdrOs << "#include <algorithm>\n";
    hdrOs << "#include <utility>\n\n";
}

void Enum2StringGenerator::WriteHeaderPostamble(CppSourceStream &hdrOs)
{
    hdrOs << out::scope_exit;
}

template<typename Fn>
void WriteNamespaceContents(CppSourceStream &hdrOs, reflection::NamespaceInfoPtr ns, Fn&& fn)
{
    out::BracedStreamScope nsScope("namespace " + ns->name, "", 0);
    if (!ns->isRootNamespace)
        hdrOs << out::new_line << nsScope;
    
    fn(hdrOs, ns);
    for (auto& inner : ns->innerNamespaces)
        WriteNamespaceContents(hdrOs, inner, std::forward<Fn>(fn));
}

void Enum2StringGenerator::WriteHeaderContent(CppSourceStream &hdrOs)
{
    WriteNamespaceContents(hdrOs, m_namespaces.GetRootNamespace(), [this](CppSourceStream &os, reflection::NamespaceInfoPtr ns) {
        for (auto& enumInfo : ns->enums)
        {
            WriteEnumToStringConversion(os, enumInfo);
            WriteEnumFromStringConversion(os, enumInfo);
        }      
    });
}

// Enum item to string conversion writer
void Enum2StringGenerator::WriteEnumToStringConversion(CppSourceStream &hdrOs, const reflection::EnumInfoPtr &enumDescr)
{
    std::string scopeName = enumDescr->scopeSpecifier;
    std::string fullName =  scopeName + (scopeName.empty() ? "" : "::") + enumDescr->name;
    out::ScopedParams params(hdrOs, {
        {"enumName", enumDescr->name},
        {"enumFullName", fullName},
        {"prefix", enumDescr->isScoped ? fullName + "::" : scopeName + (scopeName.empty() ? "" : "::")}
    });
    
    out::BracedStreamScope fnScope("inline const char* $enumName$ToString($enumFullName$ e)", "\n");
    hdrOs << out::new_line << fnScope;
    {
        out::BracedStreamScope switchScope("switch (e)", "\n");
        hdrOs << out::new_line << switchScope;
        out::OutParams innerParams;
        for (auto& i : enumDescr->items)
        {
            innerParams["itemName"] = i.itemName;
            hdrOs << out::with_params(innerParams)
                  << out::new_line(-1) << "case $prefix$$itemName$:"
                  << out::new_line << "return \"$itemName$\";";
        }
    }
    hdrOs << out::new_line << "return \"Unknown Item\";";
}

// String to enum conversion writer
void Enum2StringGenerator::WriteEnumFromStringConversion(CppSourceStream &hdrOs, const reflection::EnumInfoPtr &enumDescr)
{
    std::string scopeName = enumDescr->scopeSpecifier;
    std::string fullName =  scopeName + (scopeName.empty() ? "" : "::") + enumDescr->name;
    out::ScopedParams params(hdrOs, {
        {"enumName", enumDescr->name},
        {"enumFullName", fullName},
        {"prefix", enumDescr->isScoped ? fullName + "::" : scopeName + (scopeName.empty() ? "" : "::")}
    });
    
    out::BracedStreamScope fnScope("inline $enumFullName$ StringTo$enumName$(const char* itemName)", "\n");
    hdrOs << out::new_line << fnScope;
    {
        out::BracedStreamScope itemsScope("static std::pair<const char*, $enumFullName$> items[] = ", ";\n");
        hdrOs << out::new_line << itemsScope;

        out::OutParams& innerParams = params.GetParams();
        auto items = enumDescr->items;
        std::sort(begin(items), end(items), [](auto& i1, auto& i2) {return i1.itemName < i2.itemName;});
        for (auto& i : items)
        {
            innerParams["itemName"] = i.itemName;
            hdrOs << out::with_params(innerParams) << out::new_line << "{\"$itemName$\", $prefix$$itemName$},";
        }    
    }

    hdrOs << out::with_params(params.GetParams()) << R"(
     $enumFullName$ result;
     if (!flex_lib::detail::String2Enum(itemName, items, result))
         flex_lib::bad_enum_name::Throw(itemName, "$enumName$");
 
     return result;)";
}
} // codegen

codegen::GeneratorPtr CreateEnum2StringGen(const codegen::Options& opts)
{
    return std::make_unique<codegen::Enum2StringGenerator>(opts);
}
