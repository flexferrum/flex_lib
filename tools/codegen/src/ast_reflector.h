#ifndef AST_REFLECTOR_H
#define AST_REFLECTOR_H

#include "decls_reflection.h"

namespace reflection 
{

class AstReflector
{
public:
    explicit AstReflector(clang::ASTContext* context)
        : m_astContext(context)
    {
    }
    
    EnumInfoPtr ReflectEnum(const clang::EnumDecl* decl, NamespacesTree& nsTree);
    ClassInfoPtr ReflectClass(const clang::CXXRecordDecl* decl, NamespacesTree& nsTree);
    MethodInfoPtr ReflectMethod(const clang::CXXMethodDecl* decl, NamespacesTree& nsTree);
    
private:
    const clang::NamedDecl* FindEnclosingOpaqueDecl(const clang::DeclContext* decl);
    void SetupNamedDeclInfo(const clang::NamedDecl* decl, NamespaceInfoPtr enclosingNamespace, NamedDeclInfo* info);

private:
    clang::ASTContext* m_astContext;
};

} // reflection

#endif // AST_REFLECTOR_H
