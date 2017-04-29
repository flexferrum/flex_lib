#include "ast_reflector.h"

namespace reflection 
{

using namespace clang;

template<typename Cont>
auto FindExisting(const Cont& cont, const std::string& qualifiedName)
{
    for (auto& item : cont)
    {
        if (item->decl->getQualifiedNameAsString() == qualifiedName)
            return item;
    }
    
    return Cont::value_type();
}                                                                                     \

EnumInfoPtr AstReflector::ReflectEnum(const clang::EnumDecl *decl, NamespacesTree &nsTree, clang::ASTContext *context)
{
    const DeclContext* nsContext = decl->getEnclosingNamespaceContext();
    
    NamespaceInfoPtr ns = nsTree.GetNamespace(nsContext);
    
    EnumInfoPtr enumInfo = FindExisting(ns->enums, decl->getQualifiedNameAsString());
    if (enumInfo)
        return enumInfo;
    
    // const NamedDecl* parentDecl = FindEnclosingOpaqueDecl(decl);

    enumInfo = std::make_shared<EnumInfo>();
    enumInfo->decl = decl;
    
    SetupNamedDeclInfo(decl, ns, enumInfo.get());
    
    enumInfo->isScoped = decl->isScoped();
    
    for (const EnumConstantDecl* itemDecl : decl->enumerators())
    {
        EnumItemInfo item;
        item.itemName = itemDecl->getNameAsString();
        item.itemValue = itemDecl->getInitVal().toString(10);
        enumInfo->items.push_back(std::move(item));
    }

    ns->enums.push_back(enumInfo);

    return enumInfo;
}

void AstReflector::SetupNamedDeclInfo(const NamedDecl *decl, NamespaceInfoPtr enclosingNamespace, NamedDeclInfo *info)
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
