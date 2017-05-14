#include "ast_reflector.h"
#include "ast_utils.h"
#include "type_info.h"

#include <clang/AST/ASTContext.h>
#include <boost/algorithm/string/replace.hpp>

namespace reflection 
{

using namespace clang;

template<typename Cont>
auto FindExisting(const Cont& cont, const std::string& qualifiedName)
{
    auto p = std::find_if(begin(cont), end(cont), 
         [&qualifiedName](auto& item) {return item->decl->getQualifiedNameAsString() == qualifiedName;});
    
    
    return p == end(cont) ? Cont::value_type() : *p;
}

AccessType ConvertAccessType(clang::AccessSpecifier access)
{
    AccessType result = AccessType::Undefined;
    switch (access)
    {
    case clang::AS_public:
        result = AccessType::Public;
        break;
    case clang::AS_protected:
        result = AccessType::Protected;
        break;
    case clang::AS_private:
        result = AccessType::Private;
        break;
    case clang::AS_none:
        break;
        
    }
    
    return result;
}

EnumInfoPtr AstReflector::ReflectEnum(const clang::EnumDecl *decl, NamespacesTree* nsTree)
{
    const DeclContext* nsContext = decl->getEnclosingNamespaceContext();
    
    NamespaceInfoPtr ns;
    EnumInfoPtr enumInfo;
    if (nsTree != nullptr)
    {
        ns = nsTree->GetNamespace(nsContext);
        enumInfo = FindExisting(ns->enums, decl->getQualifiedNameAsString());    
    }
    
    if (enumInfo)
        return enumInfo;
    
    // const NamedDecl* parentDecl = FindEnclosingOpaqueDecl(decl);

    enumInfo = std::make_shared<EnumInfo>();
    enumInfo->decl = decl;
    enumInfo->location = GetLocation(decl, m_astContext);
    
    SetupNamedDeclInfo(decl, enumInfo.get(), m_astContext);
    
    enumInfo->isScoped = decl->isScoped();
    
    for (const EnumConstantDecl* itemDecl : decl->enumerators())
    {
        EnumItemInfo item;
        item.itemName = itemDecl->getNameAsString();
        item.itemValue = itemDecl->getInitVal().toString(10);
        item.location = GetLocation(itemDecl, m_astContext);
        enumInfo->items.push_back(std::move(item));
    }

    if (ns)
        ns->enums.push_back(enumInfo);

    return enumInfo;
}

ClassInfoPtr AstReflector::ReflectClass(const CXXRecordDecl* decl, NamespacesTree* nsTree)
{
    const DeclContext* nsContext = decl->getEnclosingNamespaceContext();
    
    NamespaceInfoPtr ns;
    ClassInfoPtr classInfo;
    
    if (nsTree)
    {
        ns = nsTree->GetNamespace(nsContext);        
        classInfo = FindExisting(ns->classes, decl->getQualifiedNameAsString());
    }
    
    if (classInfo)
        return classInfo;
    
    classInfo = std::make_shared<ClassInfo>();
    classInfo->decl = decl;
    
    SetupNamedDeclInfo(decl, classInfo.get(), m_astContext);
    classInfo->isAbstract = decl->isAbstract();
    classInfo->isTrivial = decl->isTrivial();
    classInfo->isUnion = decl->isUnion();
    classInfo->location = GetLocation(decl, m_astContext);
    
    for (auto methodDecl : decl->methods())
    {
        MethodInfoPtr methodInfo = ReflectMethod(methodDecl, nsTree);
        classInfo->methods.push_back(methodInfo);
    }
    
    for (auto& base : decl->bases())
    {
        ;
    }
    
    if (ns)
        ns->classes.push_back(classInfo);

    return classInfo;    
}

MethodInfoPtr AstReflector::ReflectMethod(const CXXMethodDecl* decl, NamespacesTree* nsTree)
{
    MethodInfoPtr methodInfo = std::make_shared<MethodInfo>();

    const DeclContext* nsContext = decl->getEnclosingNamespaceContext();
    
    NamespaceInfoPtr ns;
    if (nsTree)
    {
        ns = nsTree->GetNamespace(nsContext);
        SetupNamedDeclInfo(decl, methodInfo.get(), m_astContext);
    }
    
    QualType fnQualType = decl->getType();
    
    while (const ParenType *parenType = llvm::dyn_cast<ParenType>(fnQualType)) 
        fnQualType = parenType->getInnerType();
    
    const FunctionProtoType* fnProtoType = nullptr;
    if (const FunctionType *fnType = fnQualType->getAs<FunctionType>())
    {
        if (decl->hasWrittenPrototype())
            fnProtoType = llvm::dyn_cast<FunctionProtoType>(fnType);
    }
    
    methodInfo->isCtor = llvm::dyn_cast_or_null<const clang::CXXConstructorDecl>(decl) != nullptr;
    methodInfo->isDtor = llvm::dyn_cast_or_null<const clang::CXXDestructorDecl>(decl) != nullptr;
    methodInfo->isOperator = decl->isOverloadedOperator();
    methodInfo->isDeleted = decl->isDeleted();
    methodInfo->isConst = decl->isConst();
    methodInfo->isImplicit = decl->isImplicit();
    if (fnProtoType)
        methodInfo->isNoExcept = isNoexceptExceptionSpec(fnProtoType->getExceptionSpecType());
    // methodInfo->isNoExcept = decl->is
    methodInfo->isPure = decl->isPure();
    methodInfo->isStatic = decl->isStatic();
    methodInfo->isVirtual = decl->isVirtual();
    methodInfo->accessType = ConvertAccessType(decl->getAccess());
    methodInfo->fullPrototype = EntityToString(decl, m_astContext);
    methodInfo->decl = decl;
    methodInfo->returnType = TypeInfo::Create(decl->getReturnType(), m_astContext);
    
    methodInfo->declLocation = GetLocation(decl, m_astContext);
    auto defDecl = decl->getDefinition();
    if (defDecl != nullptr)
        methodInfo->defLocation = GetLocation(defDecl, m_astContext);    
    
    for (const clang::ParmVarDecl* param: decl->parameters())
    {
        MethodParamInfo paramInfo;
        paramInfo.name = param->getNameAsString();
        paramInfo.type = TypeInfo::Create(param->getType(), m_astContext);
        paramInfo.fullDecl = EntityToString(param, m_astContext);
        methodInfo->params.push_back(std::move(paramInfo));
    }
    
    return methodInfo;
}

void AstReflector::SetupNamedDeclInfo(const NamedDecl* decl, NamedDeclInfo* info, const clang::ASTContext* astContext)
{
    info->name = decl->getNameAsString();
    
    auto declContext = decl->getDeclContext();
    const clang::NamespaceDecl* encNs = nullptr; // = clang::NamespaceDecl::castFromDeclContext(declContext->isNamespace() ? declContext : declContext->getEnclosingNamespaceContext());
    // auto encNsCtx = clang::NamespaceDecl::castToDeclContext(encNs);
    
    std::string scopeQualifier = "";
    
    for (const DeclContext* parentCtx = declContext; parentCtx != nullptr; parentCtx = parentCtx->getParent())
    {
        const NamedDecl* namedScope = llvm::dyn_cast_or_null<const NamedDecl>(Decl::castFromDeclContext(parentCtx));
        if (namedScope == nullptr)
            continue;
        
        if (parentCtx->isNamespace())
        {
            encNs = clang::NamespaceDecl::castFromDeclContext(parentCtx);
            break;
        }
        
        auto scopeName = namedScope->getNameAsString();
        if (scopeName.empty())
            continue;

        if (scopeQualifier.empty())
            scopeQualifier = scopeName;
        else
            scopeQualifier = scopeName + "::" + scopeQualifier;
    }
    
    info->scopeSpecifier = scopeQualifier;
    
    if (encNs != nullptr && !encNs->isTranslationUnit())
    {
        clang::PrintingPolicy policy(astContext->getLangOpts());
        SetupDefaultPrintingPolicy(policy);

        llvm::raw_string_ostream os(info->namespaceQualifier);
        encNs->printQualifiedName(os, policy);
    }
    
    boost::algorithm::replace_all(info->namespaceQualifier, "(anonymous)::", "");
    boost::algorithm::replace_all(info->namespaceQualifier, "::(anonymous)", "");
    boost::algorithm::replace_all(info->namespaceQualifier, "(anonymous)", "");
}


} // reflection
