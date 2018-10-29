#include <OMR/GC/MarkingFn.hpp>
#include <OMR/GC/RefSlotHandle.hpp>
#include "AST.hpp"
#include "Environ.hpp"
#include "HashTable.hpp"
#include "ObjectAllocator.hpp"
#include "linenoise.h"

using OMR::GC::MarkingVisitor;
OMR::GC::Context *ctx;

HashTable globalVariables;
int main() {
  OMR::Runtime runtime;
  OMR::GC::System system(runtime);
  OMR::GC::RunContext context(system);
  ctx = &context;
  OMR::GC::MarkingFn fn = [](MarkingVisitor &m) {};
  system.markingFns().emplace_back([](MarkingVisitor &m) {
    if (globalVariables.backingArray_ != nullptr) {
      m.edge(nullptr, OMR::GC::RefSlotHandle(reinterpret_cast<Object **>(
                          &globalVariables.backingArray_)));
    }
  });

  globalVariables.Initialize(32);
  std::cout << "DBG: backing array = " << globalVariables.backingArray_
            << std::endl;
  linenoiseInstallWindowChangeHandler();

  while (1) {
    char *txt = linenoise("> ");
    if (txt == nullptr) {
      break;
    }
    if (*txt == '\0') {
      std::free(txt);
      break;
    }
    try {
      auto ast = Parse({txt, std::strlen(txt)});

      Value result = ast->Eval();
      std::cout << result << std::endl;
      linenoiseHistoryAdd(txt);
    } catch (ParseError e) {
      std::cout << "parsing failed: " << e.what() << std::endl;
    } catch (EvalError e) {
      std::cout << "evaluation failed: " << e.what() << std::endl;
    }
    std::free(txt);
  }

  linenoiseHistoryFree();
  return 0;
}
