
#pragma warning(disable : 4141) // disable warning C4141: 'inline': used more than once
#pragma warning(disable : 4244) // disable warning C4244: 'argument': conversion from 'uint64_t' to 'size_t', possible loss of data
#pragma warning(disable : 4800) // disable warning C4800: 'clang::SanitizerMask': forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable : 4624) // disable warning C4624: 'llvm::detail::AlignmentCalcImpl<T,false>': destructor was implicitly defined as deleted
#pragma warning(disable : 4146) // disable warning C4146: unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable : 4291) // disable warning C4291: 'void *clang::Decl::operator new(size_t,const clang::ASTContext &,clang::DeclContext *,size_t)': no matching operator delete found; memory will not be freed if initialization throws an exception
#pragma warning(disable : 4800) // disable warning C4800: 'clang::SanitizerMask': forcing value to bool 'true' or 'false' (performance warning)

#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>
#include <memory>
#include <string>

clang::Rewriter g_rewriter{};
unsigned int g_numFunctions{};

template< typename OStreamT, typename ArgT >
void print(OStreamT& oStream, const ArgT& arg)
{
    oStream << arg;
}

template< typename OStreamT, typename ArgT0, typename... ArgTs >
void print(OStreamT& oStream, const ArgT0& arg0, const ArgTs&... args)
{
    print(oStream, arg0);
    print(oStream, args...);
}

template< typename OStreamT, typename... ArgTs >
void println(OStreamT& oStream, const ArgTs&... args)
{
    print(oStream, args...);
    print(oStream, "\n");
}

class ExampleVisitor : public clang::RecursiveASTVisitor<ExampleVisitor>
{
public:

    explicit ExampleVisitor(clang::CompilerInstance& ci)
        : m_astContext(ci.getASTContext())
    {
        g_rewriter.setSourceMgr(m_astContext.getSourceManager(), m_astContext.getLangOpts());
    }

    bool VisitFunctionDecl(clang::FunctionDecl* func)
    {
        g_numFunctions++;
        const auto funcName = func->getNameInfo().getName().getAsString();
        if (funcName == "do_math")
        {
            g_rewriter.ReplaceText(func->getLocation(), funcName.length(), "add5");
            println(llvm::errs(), "** Rewrote function def: ", funcName);
        }
        return true;
    }

#if defined(VISIT_ALL_STATEMENTS)
    bool VisitStmt(clang::Stmt* st)
    {
        if (const auto ret = clang::dyn_cast<clang::ReturnStmt>(st))
        {
            g_rewriter.ReplaceText(ret->getRetValue()->getLocStart(), 6, "val");
            println(llvm::errs(), "** Rewrote ReturnStmt");
        }
        if (const auto call = clang::dyn_cast<clang::CallExpr>(st)) {
            g_rewriter.ReplaceText(call->getLocStart(), 7, "add5");
            println(llvm::errs(), "** Rewrote function call");
        }
        return true;
    }
#else
    bool VisitReturnStmt(clang::ReturnStmt* ret)
    {
        g_rewriter.ReplaceText(ret->getRetValue()->getLocStart(), 6, "val");
        println(llvm::errs(), "** Rewrote ReturnStmt 2");
        return true;
    }

    bool VisitCallExpr(clang::CallExpr* call)
    {
        g_rewriter.ReplaceText(call->getLocStart(), 7, "add5");
        println(llvm::errs(), "** Rewrote function call 2");
        return true;
    }
#endif

private:

    clang::ASTContext& m_astContext;
};


class ExampleASTConsumer : public clang::ASTConsumer
{
public:

    explicit ExampleASTConsumer(clang::CompilerInstance& ci)
        : m_upVisitor(std::make_unique<ExampleVisitor>(ci))
    { }

#if defined(VISIT_TRANSLATION_UNIT)
    virtual void HandleTranslationUnit(clang::ASTContext& context) override
    {
        m_upVisitor->TraverseDecl(context.getTranslationUnitDecl());
    }
#else
    virtual bool HandleTopLevelDecl(clang::DeclGroupRef declGroup) override
    {
        for(const auto& pDecl : declGroup)
        {
            m_upVisitor->TraverseDecl(pDecl);
        }
        return true;
    }
#endif
private:

    std::unique_ptr<ExampleVisitor> m_upVisitor;
};

class ExampleFrontendAction : public clang::ASTFrontendAction
{
public:

    // Inherited via ASTFrontendAction
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance& ci,
        StringRef inFile
        ) override
    {
        return std::make_unique<ExampleASTConsumer>(ci);
    }
};


int main(int argc, const char** argv)
{
    llvm::cl::OptionCategory optionCategory("my-tool options");
    clang::tooling::CommonOptionsParser optionsParser(argc, argv, optionCategory);

    clang::tooling::ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());

    println(llvm::outs(),"Applying rewriter for source paths:");
    auto sourcePaths = optionsParser.getSourcePathList();
    for (const auto& sourcePath : sourcePaths)
    {
        println(llvm::outs(), sourcePath, "");
    }

    const auto exampleFactory = clang::tooling::newFrontendActionFactory<ExampleFrontendAction>();
    const auto result = tool.run(exampleFactory.get());

    println(llvm::outs(), "Tool Result = ", result, "");

    println(llvm::errs(), "Found ", g_numFunctions, " functions");

    g_rewriter.getEditBuffer(g_rewriter.getSourceMgr().getMainFileID()).write(llvm::errs());
    return result;
}
