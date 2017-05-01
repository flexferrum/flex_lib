#ifndef TYPE_INFO_H
#define TYPE_INFO_H

#include <clang/AST/DeclCXX.h>

#include <boost/variant.hpp>
#include <memory>

namespace reflection 
{
class TypeInfo;
using TypeInfoPtr = std::shared_ptr<TypeInfo>;

struct BuiltinType
{
    
};

struct RecordType
{
    
};

struct TemplateType
{
    
};

struct WellKnownType
{
    
};

struct ArrayType
{
    
};

struct EnumType
{
    
};

class TypeInfo
{
public:
    using Type = boost::variant<BuiltinType, RecordType, TemplateType, WellKnownType, ArrayType, EnumType>;
    
    TypeInfo();
    
    const auto& GetType() {return m_type;}
    auto getAsBuiltin() const
    {
        return boost::get<const BuiltinType>(&m_type);
    }
    auto getAsRecord() const
    {
        return boost::get<const RecordType>(&m_type);
    }
    auto getAsTemplate() const
    {
        return boost::get<const TemplateType>(&m_type);
    }
    auto getAsWellKnownType() const
    {
        return boost::get<const WellKnownType>(&m_type);
    }
    auto getAsArrayType() const
    {
        return boost::get<const ArrayType>(&m_type);
    }
    auto getAsEnumType() const
    {
        return boost::get<const EnumType>(&m_type);
    }
    
    bool getIsConst() const
    {
        return m_isConst;
    }
    bool getIsVolatile() const
    {
        return m_isVolatile;
    }
    bool getIsReference() const
    {
        return m_isReference;
    }
    bool getIsRVReference() const
    {
        return m_isRVReference;
    }
    int getPointingLevels() const
    {
        return m_pointingLevels;
    }
    std::string getDeclaredName() const
    {
        return m_declaredName;
    }
    std::string getScopedName() const
    {
        return m_scopedName;
    }
    std::string getFullQualifiedName() const
    {
        return m_fullQualifiedName;
    }
    
    static TypeInfoPtr Create(const clang::QualType& qt, const clang::ASTContext* astContext);
    
private:
    bool m_isConst = false;
    bool m_isVolatile = false;
    bool m_isReference = false;
    bool m_isRVReference = false;
    int m_pointingLevels = 0;
    std::string m_declaredName;
    std::string m_scopedName;
    std::string m_fullQualifiedName;
    Type m_type;
    
    friend class TypeInfoVisitor;
};

} // reflection

#endif // TYPE_INFO_H
