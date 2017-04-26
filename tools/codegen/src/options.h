#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <vector>

namespace codegen
{
enum class Standard
{
    Auto,
    Cpp03,
    Cpp11,
    Cpp14,
    Cpp17
};

enum class GeneratorId
{
    Unknown = -1,
    Enum2StringGen,
    PimplGen
};

struct Options
{
    GeneratorId generatorType = GeneratorId::Unknown;
    std::string outputHeaderName;
    std::string outputSourceName;
    std::vector<std::string> inputFiles;
    std::vector<std::string> extraHeaders;
    bool debugMode = false;
    Standard targetStandard = Standard::Auto;
};

} // codegen

#endif // OPTIONS_H
