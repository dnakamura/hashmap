#include "AST.hpp"
#include "Environ.hpp"
#include "HashTable.hpp"
#include "linenoise.h"

HashTable globalVariables;
int main() {
  globalVariables.Initialize(32);
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
    auto ast = Parse({txt, std::strlen(txt)});

    Value result = ast->Eval();
    std::cout << result << std::endl;
    linenoiseHistoryAdd(txt);
    std::free(txt);
  }

  linenoiseHistoryFree();
  return 0;
}
