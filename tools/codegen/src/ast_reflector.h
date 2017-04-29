#ifndef AST_REFLECTOR_H
#define AST_REFLECTOR_H

#include "decls_reflection.h"

namespace reflection 
{

class AstReflector
{
public:
    EnumInfoPtr ReflectEnum(const clang::EnumDecl* decl, NamespacesTree& nsTree, clang::ASTContext* context);
    
private:
    const clang::NamedDecl* FindEnclosingOpaqueDecl(const clang::DeclContext* decl);
    void SetupNamedDeclInfo(const clang::NamedDecl* decl, NamespaceInfoPtr enclosingNamespace, NamedDeclInfo* info);
};

} // reflection

#endif // AST_REFLECTOR_H
