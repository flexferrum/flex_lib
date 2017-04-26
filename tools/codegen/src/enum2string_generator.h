#ifndef ENUM2STRING_GENERATOR_H
#define ENUM2STRING_GENERATOR_H

#include "basic_generator.h"

namespace codegen 
{
class Enum2StringGenerator : public BasicGenerator
{
public:
    Enum2StringGenerator(const Options& opts);
    
    // GeneratorBase interface
    void SetupMatcher(clang::ast_matchers::MatchFinder &finder, clang::ast_matchers::MatchFinder::MatchCallback *defaultCallback) override;
    void HandleMatch(const clang::ast_matchers::MatchFinder::MatchResult &matchResult) override;
    
    // BasicGenerator interface
protected:
    void WriteHeaderPreamble(CppSourceStream &hdrOs) override;
    void WriteHeaderContent(CppSourceStream &hdrOs) override;
    void WriteHeaderPostamble(CppSourceStream &hdrOs) override;
    
private:
    struct EnumDescriptor
    {
        // Enumeration name
        std::string enumName;
        // Is enum item needs scope specifier or not
        bool isScoped = false;
        // Collection of enum items
        std::vector<std::string> enumItems;
    };

    void WriteEnumToStringConversion(CppSourceStream &hdrOs, const EnumDescriptor &enumDescr);
    void WriteEnumFromStringConversion(CppSourceStream &hdrOs, const EnumDescriptor &enumDescr);

private:
    std::vector<EnumDescriptor> m_foundEnums;        
};
} // codegen

#endif // ENUM2STRING_GENERATOR_H
