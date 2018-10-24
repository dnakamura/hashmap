#include "HashTable.hpp"
#include "AST.hpp"
#include "linenoise.h"
#include "Environ.hpp"

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
		auto ast = Parse({ txt, std::strlen(txt) });

		Value result = ast->Eval();
		std::cout << result << std::endl;
		linenoiseHistoryAdd(txt);
		std::free(txt);

	}
	//GlobalStrings.Initialize(32);
	
	/*
	Value v0((int64_t)0), v1(1), v2(2), v3(3), v4(4), v5(5), v6(6), v7(7), v8(8), v9(9);
	HashTable hash;

	hash.Initialize(4);
	hash.Set(1, 9);
	hash.Set(5, 8);
	hash.Set(2, 7);
	std::cout << *hash.Get(1) << "\n" << *hash.Get(5) << "\n" << *hash.Get(2) << std::endl;
	*/
	linenoiseHistoryFree();
	return 0;
}