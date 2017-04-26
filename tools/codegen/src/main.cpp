#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/JamCRC.h>
#include <llvm/Support/Path.h>

#include <iostream>
#include <fstream>
#include <algorithm>

#include "options.h"
#include "generator_base.h"

using namespace llvm;

extern codegen::GeneratorPtr CreateEnum2StringGen(const codegen::Options&);

namespace
{
// Define code generation tool option category
cl::OptionCategory CodeGenCategory("Code generator options");

// Define the generation mode
cl::opt<codegen::GeneratorId> GenerationMode(cl::desc("Choose generation mode:"),
  cl::values(
        clEnumValN(codegen::GeneratorId::Enum2StringGen, "gen-enum2string" , "Enum2string conversion generation"),
        clEnumValN(codegen::GeneratorId::PimplGen, "gen-pimpl" , "Pimpl wrapper classes generation")
    ), cl::Required, cl::cat(CodeGenCategory));

// Define options for output file names
// cl::opt<std::string> OutputFilename("o", cl::desc("Specify output filename"), cl::value_desc("filename"), cl::cat(CodeGenCategory));
cl::opt<std::string> OutputHeaderName("ohdr", cl::desc("Specify output header filename"), cl::value_desc("filename"), cl::cat(CodeGenCategory));
cl::opt<std::string> OutputSourceName("osrc", cl::desc("Specify output source filename"), cl::value_desc("filename"), cl::cat(CodeGenCategory));

cl::opt<codegen::Standard> LangStandart("std", cl::desc("Choose the standard conformance for the generation results:"),
  cl::values(
        clEnumValN(codegen::Standard::Auto, "auto" , "Automatic detection (default)"),
        clEnumValN(codegen::Standard::Cpp03, "c++03" , "C++ 2003 standard"),
        clEnumValN(codegen::Standard::Cpp11, "c++11" , "C++ 2011 standard"),
        clEnumValN(codegen::Standard::Cpp03, "c++14" , "C++ 2014 standard"),
        clEnumValN(codegen::Standard::Cpp03, "c++17" , "C++ 2017 standard")
    ), cl::Optional, cl::init(codegen::Standard::Auto), cl::cat(CodeGenCategory));

// Define common help message printer
cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
// Define specific help message printer
cl::extrahelp MoreHelp("\nCode generation tool help text...");

std::vector<std::pair<codegen::GeneratorId, codegen::GeneratorFactory>> GenFactories = {
    {codegen::GeneratorId::Enum2StringGen, CreateEnum2StringGen},
    {codegen::GeneratorId::PimplGen, nullptr},    
};
}

namespace codegen 
{
class MatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    MatchHandler(const Options& options, GeneratorBase* generator)
        : m_options(options)
        , m_generator(generator)
    {
    }
    
    void onStartOfTranslationUnit()
    {
        m_generator->OnCompilationStarted();
    }
    
    void run(const clang::ast_matchers::MatchFinder::MatchResult& result)
    {
        m_generator->HandleMatch(result);
    }
    
    void onEndOfTranslationUnit()
    {
        if (!(m_hasErrors = !m_generator->Validate()))
            m_hasErrors = !m_generator->GenerateOutput();
    }
    
    bool HasErrors() const {return m_hasErrors;}
    
private:
    Options m_options;
    GeneratorBase* m_generator = nullptr;
    bool m_hasErrors = false;
};
} // codegen

int main(int argc, const char** argv)
{
    using namespace clang::tooling;
    using namespace clang::ast_matchers;
    
    // Parse command line options and setup ClangTool
    CommonOptionsParser optionsParser(argc, argv, CodeGenCategory);
    ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());
    
    codegen::Options options;
    options.generatorType = GenerationMode;
    options.inputFiles = optionsParser.getSourcePathList();
    options.outputHeaderName = OutputHeaderName;
    options.outputSourceName = OutputSourceName;
    options.targetStandard = LangStandart;
    
    auto genFactory = std::find_if(begin(GenFactories), end(GenFactories), [type = options.generatorType](auto& p) {return p.first == type && p.second != nullptr;});
    
    if(genFactory == end(GenFactories))
    {
        std::cerr << "Generator is not defined for generation mode '" << GenerationMode.ValueStr.str() << "'" << std::endl;
        return -1;
    }
    
    auto generator = genFactory->second(options);
    
    if (!generator)
    {
        std::cerr << "Failed to create generator for generation mode '" << GenerationMode.ValueStr.str() << "'" << std::endl;
        return -1;
    }
    
    codegen::MatchHandler handler(options, generator.get());
    MatchFinder finder;
    generator->SetupMatcher(finder, &handler);
    
    
    // Run tool for the specified input files
    auto result = tool.run(newFrontendActionFactory(&finder).get());
    if (result == 0 && handler.HasErrors())
        return -1;
    
    return result;
}
