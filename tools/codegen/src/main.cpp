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

using namespace llvm;

namespace
{
// Define code generation tool option category
cl::OptionCategory CodeGenCategory("Code generator options");
// Define option for output file name
cl::opt<std::string> OutputFilename("o", cl::desc("Specify output filename"), cl::value_desc("filename"), cl::cat(CodeGenCategory));

// Define common help message printer
cl::extrahelp CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
// Define specific help message printer
cl::extrahelp MoreHelp("\nCode generation tool help text...");
}

// Enum declaration description
struct EnumDescriptor
{
    // Enumeration name
    std::string enumName;
    // Is enum item needs scope specifier or not
    bool isScoped = false;
    // Collection of enum items
    std::vector<std::string> enumItems;
};

// File preamble writer
void WritePreamble(std::ostream& os, const std::vector<std::string>& inputFiles, uint32_t outFileCrc)
{
    // Write header guard
    os << "#ifndef ENUMGEN_" << outFileCrc << "\n";
    os << "#define ENUMGEN_" << outFileCrc << "\n";
    os << "\n";
    
    // Include input files (directly, by path)
    for (auto& f : inputFiles)
    {
        auto fileName = f;
        std::replace(fileName.begin(), fileName.end(), '\\', '/');
        os << "#include \"" << fileName << "\"\n";
    }
    
    // Necessary library files
    os << "#include <algorithm>\n";
    os << "#include <utility>\n\n";
}

void WritePostamble(std::ostream& os, uint32_t outFileCrc)
{
    os << "#endif // ENUMGEN_" << outFileCrc << "\n";
}

// Enum item to string conversion writer
void WriteEnumToStringConversion(std::ostream& os, const EnumDescriptor& enumDescr)
{
    auto& enumName = enumDescr.enumName;
    
    // Implement conversion via switch/case statement
    os << "inline const char* " << enumName << "ToString(" << enumName << " e)\n";
    os << "{\n";
    os << "    switch (e)\n";
    os << "    {\n";
    auto scopePrefix = enumDescr.isScoped ? enumName + "::" : std::string();
    for (auto& i : enumDescr.enumItems)
    {
        os << "    case " << scopePrefix << i << ":\n";
        os << "        return \"" << i << "\";\n";
    }
      
    os << "    }\n";
    os << "    return \"Unknown Item\";\n";
    os << "}\n\n";    
}

// String to enum conversion writer
void WriteEnumFromStringConversion(std::ostream& os, const EnumDescriptor& enumDescr)
{
    auto& enumName = enumDescr.enumName;
    auto scopePrefix = enumDescr.isScoped ? enumName + "::" : std::string();
    
    // Implement conversion via sorted array of paris 'string -> enum item'
    // and std::lower_bound search algorithm
    os << "inline " << enumName << " StringTo" << enumName << "(const char* itemName)\n";
    os << "{\n";
    os << "    static std::pair<const char*, " << enumName << "> items[] = {\n";

    for (auto& i : enumDescr.enumItems)
        os << "        {\"" << i << "\", " << scopePrefix << i << "},\n";

    os <<R"(    };
    
    auto p = std::lower_bound(begin(items), end(items), itemName,
                      [](auto&& i, auto&& v) {return strcmp(i.first, v) < 0;});
    
    if (p == end(items) || strcmp(p->first, itemName) != 0)
        throw std::bad_cast();
    
    return p->second;
}

)";
}

// Handler of the results of MatchFinder
class EnumHandler : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
    // Method is invoked when new enum declaration found in the input file
    void run(const clang::ast_matchers::MatchFinder::MatchResult& result) override
    {
        if (const clang::EnumDecl* decl = result.Nodes.getNodeAs<clang::EnumDecl>("enum"))
        {
            ProcessEnum(decl);
        }
    }
    
    auto& GetFoundEnums() const {return m_foundEnums;}
    
private:
    // Collection of the found enum declaration
    std::vector<EnumDescriptor> m_foundEnums;
    
    void ProcessEnum(const clang::EnumDecl* decl)
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
};

int main(int argc, const char** argv)
{
    using namespace clang::tooling;
    using namespace clang::ast_matchers;
    
    // Parse command line options and setup ClangTool
    CommonOptionsParser optionsParser(argc, argv, CodeGenCategory);
    ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());
    
    // Setup declaration matcher and match finder
    DeclarationMatcher enumMatcher = 
            enumDecl(isExpansionInMainFile()).bind("enum");
    
    EnumHandler handler;
    MatchFinder finder;
    finder.addMatcher(enumMatcher, &handler);
    
    // Run tool for the specified input files
    auto result = tool.run(newFrontendActionFactory(&finder).get());
    if (result != 0)
        return result;
    
    // Open output file
    std::ostream* targetOs = &std::cout;
    std::ofstream outFile(OutputFilename.c_str());
    uint32_t fileHash = 0;
    if (!outFile.good())
    {
        std::cerr << "Can't open output file for writing: " << OutputFilename << std::endl;
        std::cerr << "Use stdout instead" << std::endl;
    }
    else
    {
        targetOs = &outFile;
        llvm::JamCRC crcCalc;
        crcCalc.update(llvm::ArrayRef<char>(OutputFilename.getValue().data(), OutputFilename.getValue().size()));
        fileHash = crcCalc.getCRC();
    }
    
    // Write output file preamble
    WritePreamble(*targetOs, optionsParser.getSourcePathList(), fileHash);
    // Write conversion functions to output file
    for (auto& descr : handler.GetFoundEnums())
    {
        WriteEnumToStringConversion(*targetOs, descr);
        WriteEnumFromStringConversion(*targetOs, descr);
    }
    // Write output file postamble
    WritePostamble(*targetOs, fileHash);
    
    return 0;
}
