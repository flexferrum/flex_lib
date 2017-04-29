#ifndef DECLS_REFLECTION_H
#define DECLS_REFLECTION_H

#include <clang/AST/DeclCXX.h>
#include <memory>
#include <map>

namespace reflection
{

struct NamedDeclInfo
{
    std::string name;
    std::string namespaceQualifier;
    std::string scopeSpecifier;
    
    std::string GetFullQualifiedScope() const
    {
        std::string result;
        if (!namespaceQualifier.empty())
            result = namespaceQualifier;
        
        if (!scopeSpecifier.empty())
            result += "::" + scopeSpecifier;

        return result;        
    }
    std::string GetFullQualifiedName() const
    {
        return !name.empty() ? GetFullQualifiedScope() + "::" + name : "";
    }
};

struct EnumItemInfo
{
    std::string itemName;
    std::string itemValue;
    
    const clang::EnumConstantDecl* decl = nullptr;
};

struct EnumInfo : public NamedDeclInfo
{
    bool isScoped = false;
    std::vector<EnumItemInfo> items;
    
    const clang::EnumDecl* decl = nullptr;
};

using EnumInfoPtr = std::shared_ptr<EnumInfo>;

struct NamespaceInfo;
using NamespaceInfoPtr = std::shared_ptr<NamespaceInfo>;

struct NamespaceInfo : public NamedDeclInfo
{
    bool isRootNamespace = false;
    std::vector<NamespaceInfoPtr> innerNamespaces;
    std::vector<EnumInfoPtr> enums;
    
    const clang::NamespaceDecl *decl = nullptr;
};

class NamespacesTree
{
public:
    
    auto GetRootNamespace() const {return m_rootNamespace;}
    NamespaceInfoPtr GetNamespace(const clang::DeclContext* decl)
    {
        auto p = m_namespaces.find(decl);
        if (p != m_namespaces.end())
            return p->second;
        
        if (decl->isTranslationUnit())
        {
            NamespaceInfoPtr nsInfo = std::make_shared<NamespaceInfo>();
            nsInfo->name = "";
            nsInfo->namespaceQualifier = "";
            nsInfo->scopeSpecifier = "";
            nsInfo->isRootNamespace = true;
            m_namespaces[decl] = nsInfo;
            m_rootNamespace = nsInfo;
            return nsInfo;
        }
        
        const clang::NamespaceDecl* nsDecl = clang::NamespaceDecl::castFromDeclContext(decl);
        if (nsDecl == nullptr)
            return NamespaceInfoPtr();
        
        const clang::DeclContext* parentContext = nullptr;
        do
        {
            parentContext = nsDecl->getParent();
        } while (!parentContext->isNamespace() && !parentContext->isTranslationUnit());
        
        auto parentNs = GetNamespace(parentContext);
        
        if (nsDecl->isAnonymousNamespace())
        {
            m_namespaces[decl] = parentNs;
            return parentNs;
        }
        
        NamespaceInfoPtr nsInfo = std::make_shared<NamespaceInfo>();
        nsInfo->name = nsDecl->getNameAsString();
        nsInfo->namespaceQualifier = parentNs->GetFullQualifiedName();
        nsInfo->scopeSpecifier = "";
        nsInfo->decl = nsDecl;
        m_namespaces[decl] = nsInfo;
        parentNs->innerNamespaces.push_back(nsInfo);
        
        return nsInfo;
    }

private:
    NamespaceInfoPtr m_rootNamespace;
    std::map<const clang::DeclContext*, NamespaceInfoPtr> m_namespaces;
};


}

#endif // DECLS_REFLECTION_H
