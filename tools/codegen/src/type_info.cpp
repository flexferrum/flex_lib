#include "type_info.h"
#include "ast_utils.h"
#include "utils.h"

#include <clang/AST/TypeVisitor.h>
#include <clang/AST/DeclTemplate.h>

#include <iostream>

namespace reflection 
{

class TypeUnwrapper : public clang::TypeVisitor<TypeUnwrapper, bool>
{
public:
    TypeUnwrapper(TypeInfo* targetType, const clang::ASTContext* astContext)
        : m_targetType(targetType)
        , m_astContext(astContext)
    {
    }
    
    bool VisitBuiltinType(const clang::BuiltinType* tp)
    {
        BuiltinType result;
        switch (tp->getKind())
        {
        case clang::BuiltinType::Void:
            result.type = BuiltinType::Void;
            break;
        case clang::BuiltinType::Bool:
            result.type = BuiltinType::Bool;
            result.bits = 1;
            break;
        case clang::BuiltinType::Char_U:
        case clang::BuiltinType::Char_S:
            result.type = BuiltinType::Char;
            result.bits = 8;
            break;
        case clang::BuiltinType::WChar_U:
        case clang::BuiltinType::WChar_S:
            result.type = BuiltinType::Char;
            result.bits = 32;
            break;
        case clang::BuiltinType::SChar:
            result.type = BuiltinType::Char;
            result.bits = 8;
            result.isSigned = BuiltinType::Signed;
            break;
        case clang::BuiltinType::UChar:
            result.type = BuiltinType::Char;
            result.bits = 8;
            result.isSigned = BuiltinType::Unsigned;
            break;
        case clang::BuiltinType::Char16:
            result.type = BuiltinType::Char;
            result.bits = 16;
            break;
        case clang::BuiltinType::Char32:
            result.type = BuiltinType::Char;
            result.bits = 32;
            break;
        case clang::BuiltinType::UShort:
            result.type = BuiltinType::Short;
            result.bits = 16;
            result.isSigned = BuiltinType::Unsigned;
            break;
        case clang::BuiltinType::Short:
            result.type = BuiltinType::Short;
            result.bits = 16;
            result.isSigned = BuiltinType::Signed;
            break;
        case clang::BuiltinType::UInt:
            result.type = BuiltinType::Int;
            result.bits = 32;
            result.isSigned = BuiltinType::Unsigned;
            break;
        case clang::BuiltinType::Int:
            result.type = BuiltinType::Int;
            result.bits = 32;
            result.isSigned = BuiltinType::Signed;
            break;
        case clang::BuiltinType::ULong:
            result.type = BuiltinType::Long;
            result.bits = 64;
            result.isSigned = BuiltinType::Unsigned;
            break;
        case clang::BuiltinType::Long:
            result.type = BuiltinType::Long;
            result.bits = 64;
            result.isSigned = BuiltinType::Signed;
            break;
        case clang::BuiltinType::ULongLong:
            result.type = BuiltinType::LongLong;
            result.bits = 64;
            result.isSigned = BuiltinType::Unsigned;
            break;
        case clang::BuiltinType::LongLong:
            result.type = BuiltinType::LongLong;
            result.bits = 64;
            result.isSigned = BuiltinType::Signed;
            break;
        case clang::BuiltinType::Float:
            result.type = BuiltinType::Float;
            result.bits = 32;
            break;
        case clang::BuiltinType::Double:
            result.type = BuiltinType::Double;
            result.bits = 64;
            break;
        case clang::BuiltinType::LongDouble:
            result.type = BuiltinType::LongDouble;
            result.bits = 80;
            break;
        case clang::BuiltinType::NullPtr:
            result.type = BuiltinType::Nullptr;
            break;
        default:
            result.type = BuiltinType::Extended;
            break;
        }
        result.typeInfo = tp;
        result.kind = tp->getKind();
        m_targetType->m_type = result;
        
        return true;
    }
    
    bool VisitElaboratedType(const clang::ElaboratedType* tp)
    {
        return VisitQualType(tp->isSugared() ? tp->desugar() : tp->getNamedType());
    }
    
    bool VisitReferenceType(const clang::ReferenceType* tp)
    {
        if (tp->isSpelledAsLValue())
            m_targetType->m_isReference = true;
        else
            m_targetType->m_isRVReference = true;
        
        return VisitQualType(tp->getPointeeType());
    }
    
    bool VisitDecayedType(const clang::DecayedType* tp)
    {
        return VisitQualType(tp->getOriginalType());
    }
    
    bool VisitPointerType(const clang::PointerType* tp)
    {
        m_targetType->m_pointingLevels ++;
        return VisitQualType(tp->getPointeeType());
    }
    
    bool VisitTypedefType(const clang::TypedefType* tp)
    {
        return VisitQualType(tp->getDecl()->getUnderlyingType());
    }
    
    bool VisitRecordType(const clang::RecordType* tp)
    {
        RecordType result;
        result.decl = tp->getDecl();
        FillDeclDependentFields(result.decl);
        m_targetType->m_type = result;
        return true;
    }
    
    bool VisitTemplateSpecializationType(const clang::TemplateSpecializationType* tp)
    {
        WellKnownType result;
        
        clang::TemplateName tplName = tp->getTemplateName();
        if (tplName.getKind() != clang::TemplateName::Template)
            return false;
        const clang::TemplateDecl* tplDecl = tplName.getAsTemplateDecl();
        result.decl = tplDecl;
        FillDeclDependentFields(tplDecl);
        
        if (tp->isTypeAlias())
            result.aliasedType = TypeInfo::Create(tp->getAliasedType(), m_astContext);
        
        for (const clang::TemplateArgument& tplArg : tp->template_arguments())
        {
            TemplateType::TplArg argInfo;
            switch (tplArg.getKind())
            {
            case clang::TemplateArgument::Null:
                argInfo = TemplateType::GenericArg{TemplateType::NullTplArg};
                break;
            case clang::TemplateArgument::Type:
                argInfo = TypeInfo::Create(tplArg.getAsType(), m_astContext);
                break;
            case clang::TemplateArgument::Declaration:
                argInfo = tplArg.getAsDecl();
                break;
            case clang::TemplateArgument::NullPtr:
                argInfo = TemplateType::GenericArg{TemplateType::NullPtrTplArg};
                break;
            case clang::TemplateArgument::Integral:
                argInfo = ConvertAPSInt(tplArg.getAsIntegral()).AsSigned();
                break;
            case clang::TemplateArgument::Template:
                argInfo = TemplateType::GenericArg{TemplateType::TemplateTplArg};
                break;
            case clang::TemplateArgument::TemplateExpansion:
                argInfo = TemplateType::GenericArg{TemplateType::TemplateExpansionTplArg};
                break;
            case clang::TemplateArgument::Expression:
            {
                llvm::APSInt value;
                if (tplArg.getAsExpr()->EvaluateAsInt(value, *m_astContext))
                    argInfo = ConvertAPSInt(value).AsSigned();
                else
                    argInfo = TemplateType::GenericArg{TemplateType::UnknownTplArg};
                
                break;
            }
            case clang::TemplateArgument::Pack:
                argInfo = TemplateType::GenericArg{TemplateType::PackTplArg};
                break;                
            }
            result.arguments.push_back(argInfo);
        }
        
        m_targetType->m_type = static_cast<TemplateType&&>(std::move(result));
        return true;
    }
    
    bool VisitEnumType(const clang::EnumType* tp)
    {
        EnumType result;
        result.decl = tp->getDecl();
        FillDeclDependentFields(result.decl);
        m_targetType->m_type = result;
        return true;        
    }
    
    bool VisitConstantArrayType(const clang::ConstantArrayType* tp)
    {
        ArrayType result;
        result.dims.push_back(ConvertAPInt(tp->getSize()).AsUnsigned());
        TypeInfoPtr itemType = TypeInfo::Create(tp->getElementType(), m_astContext);
        if (itemType->getAsArrayType() != nullptr)
        {
            const ArrayType* subArray = itemType->getAsArrayType();
            result.dims.insert(result.dims.end(), subArray->dims.begin(), subArray->dims.end());
            itemType = subArray->itemType;
        }
        result.itemType = itemType;
        m_targetType->m_type = result;
        return true;
    }
    
    bool VisitQualType(const clang::QualType& qt)
    {
        m_targetType->m_isConst = qt.isConstQualified();
        m_targetType->m_isVolatile = qt.isVolatileQualified();

        return Visit(qt.getTypePtr());        
    }
    
private:
    void FillDeclDependentFields(const clang::NamedDecl* decl)
    {
        ;
    }
    
private:
    TypeInfo* m_targetType;
    const clang::ASTContext* m_astContext;
};

TypeInfo::TypeInfo()
{
    
}

TypeInfoPtr TypeInfo::Create(const clang::QualType& qt, const clang::ASTContext* astContext)
{
    TypeInfoPtr result = std::make_shared<TypeInfo>();
    
    result->m_printedName = EntityToString(&qt, astContext);
    result->m_typeDecl= qt.getTypePtr();
    
    TypeUnwrapper visitor(result.get(), astContext);
    bool unwrapped = visitor.VisitQualType(qt);
    
    if (!unwrapped) // result->m_type.empty())
    {
        result->m_typeDecl->dump();
        std::clog << std::endl;
    }
    std::clog << "### Unwrapped type: " << result << std::endl;
    
    return result;
}

} // reflection
