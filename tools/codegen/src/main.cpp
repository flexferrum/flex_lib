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
    bool isScoped;
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
    
    // Implement conversion via sorted array and lower_bound search algorithm
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

class EnumHandler : public clang::ast_matchers::MatchFinder::MatchCallback
{
public:
   void run(const clang::ast_matchers::MatchFinder::MatchResult& result) override
   {
       if (const clang::EnumDecl* decl = result.Nodes.getNodeAs<clang::EnumDecl>("enum"))
       {
           ProcessEnum(decl);
       }
   }
   
   auto& GetFoundEnums() const {return m_foundEnums;}
  
private:
   std::vector<EnumDescriptor> m_foundEnums;
   
   void ProcessEnum(const clang::EnumDecl* decl)
   {
       EnumDescriptor descriptor;
       descriptor.enumName = decl->getName();
       descriptor.isScoped = decl->isScoped();
       
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
    
    CommonOptionsParser optionsParser(argc, argv, CodeGenCategory);
    ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());
    
    DeclarationMatcher enumMatcher = 
            enumDecl(isExpansionInMainFile()).bind("enum");
    
    EnumHandler handler;
    MatchFinder finder;
    finder.addMatcher(enumMatcher, &handler);
    
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
    
    // Write conversion functions to output file
    WritePreamble(*targetOs, optionsParser.getSourcePathList(), fileHash);
    for (auto& descr : handler.GetFoundEnums())
    {
        WriteEnumToStringConversion(*targetOs, descr);
        WriteEnumFromStringConversion(*targetOs, descr);
    }
    WritePostamble(*targetOs, fileHash);
}
