#include "ast_reflector.h"

#include <clang/AST/PrettyPrinter.h>

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

auto GetLocation(const clang::SourceLocation& loc, const ASTContext* context)
{
    SourceLocation result;
    clang::PresumedLoc ploc = context->getSourceManager().getPresumedLoc(loc, false);
    result.fileName = ploc.getFilename();
    result.line = ploc.getLine();
    result.column = ploc.getColumn();
    
    return result;
}

template<typename Decl>
auto GetDeclLocation(const Decl* decl, const ASTContext* context)
{
    return GetLocation(decl->getLocation(), context); 
}

template<typename Decl>
std::string DeclToString(const Decl* decl, const ASTContext* context)
{
    clang::PrintingPolicy policy(context->getLangOpts());
    
    policy.Bool = true;
    policy.AnonymousTagLocations = false;
    policy.SuppressUnwrittenScope = true;
    policy.Indentation = 4;
    policy.UseVoidForZeroParams = false;
    
    std::string result;
    {
        llvm::raw_string_ostream os(result);
    
        decl->print(os, policy);
    }
    return result;
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

EnumInfoPtr AstReflector::ReflectEnum(const clang::EnumDecl *decl, NamespacesTree &nsTree)
{
    const DeclContext* nsContext = decl->getEnclosingNamespaceContext();
    
    NamespaceInfoPtr ns = nsTree.GetNamespace(nsContext);
    
    EnumInfoPtr enumInfo = FindExisting(ns->enums, decl->getQualifiedNameAsString());
    if (enumInfo)
        return enumInfo;
    
    // const NamedDecl* parentDecl = FindEnclosingOpaqueDecl(decl);

    enumInfo = std::make_shared<EnumInfo>();
    enumInfo->decl = decl;
    enumInfo->location = GetDeclLocation(decl, m_astContext);
    
    SetupNamedDeclInfo(decl, ns, enumInfo.get());
    
    enumInfo->isScoped = decl->isScoped();
    
    for (const EnumConstantDecl* itemDecl : decl->enumerators())
    {
        EnumItemInfo item;
        item.itemName = itemDecl->getNameAsString();
        item.itemValue = itemDecl->getInitVal().toString(10);
        item.location = GetDeclLocation(itemDecl, m_astContext);
        enumInfo->items.push_back(std::move(item));
    }

    ns->enums.push_back(enumInfo);

    return enumInfo;
}

ClassInfoPtr AstReflector::ReflectClass(const CXXRecordDecl* decl, NamespacesTree& nsTree)
{
    const DeclContext* nsContext = decl->getEnclosingNamespaceContext();
    
    NamespaceInfoPtr ns = nsTree.GetNamespace(nsContext);
    
    ClassInfoPtr classInfo = FindExisting(ns->classes, decl->getQualifiedNameAsString());
    if (classInfo)
        return classInfo;
    
    classInfo = std::make_shared<ClassInfo>();
    classInfo->decl = decl;
    
    SetupNamedDeclInfo(decl, ns, classInfo.get());
    classInfo->isAbstract = decl->isAbstract();
    classInfo->isTrivial = decl->isTrivial();
    classInfo->isUnion = decl->isUnion();
    classInfo->location = GetDeclLocation(decl, m_astContext);
    
    for (auto methodDecl : decl->methods())
    {
        MethodInfoPtr methodInfo = ReflectMethod(methodDecl, nsTree);
        classInfo->methods.push_back(methodInfo);
    }
    
    for (auto& base : decl->bases())
    {
        ;
    }

    return classInfo;    
}

MethodInfoPtr AstReflector::ReflectMethod(const CXXMethodDecl* decl, NamespacesTree& nsTree)
{
    MethodInfoPtr methodInfo = std::make_shared<MethodInfo>();

    const DeclContext* nsContext = decl->getEnclosingNamespaceContext();
    
    NamespaceInfoPtr ns = nsTree.GetNamespace(nsContext);

    SetupNamedDeclInfo(decl, ns, methodInfo.get());
    methodInfo->isCtor = llvm::dyn_cast_or_null<const clang::CXXConstructorDecl>(decl) != nullptr;
    methodInfo->isDtor = llvm::dyn_cast_or_null<const clang::CXXDestructorDecl>(decl) != nullptr;
    methodInfo->isOperator = decl->isOverloadedOperator();
    methodInfo->isDeleted = decl->isDeleted();
    methodInfo->isConst = decl->isConst();
    methodInfo->isImplicit = decl->isImplicit();
    // methodInfo->isNoExcept = decl->is
    methodInfo->isPure = decl->isPure();
    methodInfo->isStatic = decl->isStatic();
    methodInfo->isVirtual = decl->isVirtual();
    methodInfo->accessType = ConvertAccessType(decl->getAccess());
    methodInfo->fullPrototype = DeclToString(decl, m_astContext);
    methodInfo->decl = decl;
    
    methodInfo->declLocation = GetDeclLocation(decl, m_astContext);
    auto defDecl = decl->getDefinition();
    if (defDecl != nullptr)
        methodInfo->defLocation = GetDeclLocation(defDecl, m_astContext);
    
    
    for (const clang::ParmVarDecl* param: decl->parameters())
    {
        MethodParamInfo paramInfo;
        paramInfo.name = param->getNameAsString();
    }
    
    return methodInfo;
}

void AstReflector::SetupNamedDeclInfo(const NamedDecl* decl, NamespaceInfoPtr enclosingNamespace, NamedDeclInfo* info)
{
    info->name = decl->getNameAsString();
    
    std::string scopeQualifier = "";
    
    for (const DeclContext* parentCtx = decl->getDeclContext(); parentCtx != enclosingNamespace->decl; parentCtx = parentCtx->getParent())
    {
        const NamedDecl* namedScope = llvm::dyn_cast_or_null<const NamedDecl>(Decl::castFromDeclContext(parentCtx));
        if (namedScope == nullptr)
            continue;
        
        auto scopeName = namedScope->getNameAsString();
        if (scopeName.empty())
            continue;

        if (scopeQualifier.empty())
            scopeQualifier = scopeName;
        else
            scopeQualifier = scopeName + "::" + scopeQualifier;
    }
    
    info->scopeSpecifier = scopeQualifier;
    info->namespaceQualifier = enclosingNamespace->GetFullQualifiedName();    
}

} // reflection
