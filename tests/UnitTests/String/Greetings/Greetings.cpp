#include <string>

#include "LLU/MArgumentManager.h"

EXTERN_C DLLEXPORT int Greetings(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	auto name = mngr.getString(0);
	mngr.setString(std::string("Greetings ") + name + "!");
	return LIBRARY_NO_ERROR;
}
