/** 
 * @file	LibraryLinkError.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 *
 * @brief	Contains definitions of ErrorManager members and implementation of interface function sendRegisteredErrors used by PacletFailure framework in LLU
 *
 */

#include "mathlink.h"

#include "LLU/LibraryLinkError.h"

#include "LLU/Containers/LibDataHolder.h"
#include "LLU/Utilities.hpp"
#include "LLU/ML/MLStream.hpp"
#include "LLU/ML/Utilities.h"


namespace LibraryLinkUtils {

	auto ErrorManager::errors() -> ErrorManager::ErrorMap& {
		static ErrorMap errMap = registerLLUErrors({

			// Original LibraryLink error codes:
			{ LLErrorName::VersionError,	"An error was caused by an incompatible function call. The library was compiled with a previous WolframLibrary version." },
			{ LLErrorName::FunctionError,	"An error occurred in the library function." },
			{ LLErrorName::MemoryError,		"An error was caused by failed memory allocation or insufficient memory." },
			{ LLErrorName::NumericalError,	"A numerical error was encountered." },
			{ LLErrorName::DimensionsError,	"An error caused by inconsistent dimensions or by exceeding array bounds." },
			{ LLErrorName::RankError,		"An error was caused by a tensor with an inconsistent rank." },
			{ LLErrorName::TypeError,		"An error caused by inconsistent types was encountered." },
			{ LLErrorName::NoError,			"No errors occurred." },

			// MArgument errors:
			{ LLErrorName::MArgumentLibDataError,		"WolframLibraryData is not set." },
			{ LLErrorName::MArgumentIndexError,		"An error was caused by an incorrect argument index." },
			{ LLErrorName::MArgumentNumericArrayError,	"An error was caused by a NumericArray argument." },
			{ LLErrorName::MArgumentTensorError,	"An error was caused by a Tensor argument." },
			{ LLErrorName::MArgumentImageError,		"An error was caused by an Image argument." },

			// ErrorManager errors:
			{ LLErrorName::ErrorManagerThrowIdError,	"An exception was thrown with a non-existent id." },
			{ LLErrorName::ErrorManagerThrowNameError,	"An exception was thrown with a non-existent name." },
			{ LLErrorName::ErrorManagerCreateNameError,	"An exception was registered with a name that already exists." },

			// NumericArray errors:
			{ LLErrorName::NumericArrayInitError,	"Failed to construct NumericArray." },
			{ LLErrorName::NumericArrayNewError,	"Failed to create a new NumericArray." },
			{ LLErrorName::NumericArrayCloneError,	"Failed to clone NumericArray." },
			{ LLErrorName::NumericArrayTypeError,	"An error was caused by an NumericArray type mismatch." },
			{ LLErrorName::NumericArraySizeError,	"An error was caused by an incorrect NumericArray size." },
			{ LLErrorName::NumericArrayIndexError,	"An error was caused by attempting to access a nonexistent NumericArray element." },
			{ LLErrorName::NumericArrayConversionError, "Failed to convert NumericArray from different type."},

			// MTensor errors:
			{ LLErrorName::TensorInitError,		"Failed to construct Tensor." },
			{ LLErrorName::TensorNewError,		"Failed to create a new MTensor." },
			{ LLErrorName::TensorCloneError,	"Failed to clone MTensor." },
			{ LLErrorName::TensorTypeError,		"An error was caused by an MTensor type mismatch." },
			{ LLErrorName::TensorSizeError,		"An error was caused by an incorrect Tensor size." },
			{ LLErrorName::TensorIndexError,	"An error was caused by attempting to access a nonexistent Tensor element." },

			// MImage errors:
			{ LLErrorName::ImageInitError,	"Failed to construct Image." },
			{ LLErrorName::ImageNewError,	"Failed to create a new MImage." },
			{ LLErrorName::ImageCloneError,	"Failed to clone MImage." },
			{ LLErrorName::ImageTypeError,	"An error was caused by an MImage type mismatch." },
			{ LLErrorName::ImageSizeError,	"An error was caused by an incorrect Image size." },
			{ LLErrorName::ImageIndexError,	"An error was caused by attempting to access a nonexistent Image element." },

			// MathLink errors:
			{ LLErrorName::MLTestHeadError,			"MLTestHead failed (wrong head or number of arguments)." },
			{ LLErrorName::MLPutSymbolError,		"MLPutSymbol failed." },
			{ LLErrorName::MLPutFunctionError,		"MLPutFunction failed." },
			{ LLErrorName::MLTestSymbolError,		"MLTestSymbol failed (different symbol on the link than expected)." },
			{ LLErrorName::MLWrongSymbolForBool,    R"(Tried to read something else than "True" or "False" as boolean.)" },
			{ LLErrorName::MLGetListError,			"Could not get list from MathLink." },
			{ LLErrorName::MLGetScalarError,		"Could not get scalar from MathLink." },
			{ LLErrorName::MLGetStringError,		"Could not get string from MathLink." },
			{ LLErrorName::MLGetArrayError,			"Could not get array from MathLink." },
			{ LLErrorName::MLPutListError,			"Could not send list via MathLink." },
			{ LLErrorName::MLPutScalarError,		"Could not send scalar via MathLink." },
			{ LLErrorName::MLPutStringError,		"Could not send string via MathLink." },
			{ LLErrorName::MLPutArrayError,			"Could not send array via MathLink." },
			{ LLErrorName::MLGetSymbolError,		"MLGetSymbol failed." },
			{ LLErrorName::MLGetFunctionError,		"MLGetFunction failed." },
			{ LLErrorName::MLPacketHandleError,		"One of the packet handling functions failed." },
			{ LLErrorName::MLFlowControlError,			"One of the flow control functions failed." },
			{ LLErrorName::MLTransferToLoopbackError,	"Something went wrong when transferring expressions from loopback link." },
			{ LLErrorName::MLCreateLoopbackError,		"Could not create a new loopback link." },
			{ LLErrorName::MLLoopbackStackSizeError,	"Loopback stack size too small to perform desired action." },

           // DataList errors:
           { LLErrorName::DLNullRawNode,			"DataStoreNode passed to Node wrapper was null" },
           { LLErrorName::DLInvalidNodeType,		"DataStoreNode passed to Node wrapper carries data of invalid type" },
           { LLErrorName::DLGetNodeDataError,	    "DataStoreNode_getData failed" },
           { LLErrorName::DLNullRawDataStore,       "DataStore passed to DataList was null" },
           { LLErrorName::DLPushBackTypeError,      "Element to be added to the DataList has incorrect type" },

           // MArgument errors:
           { LLErrorName::ArgumentCreateNull,       "Trying to create Argument object from nullptr" },
           { LLErrorName::ArgumentAddNodeMArgument, "Trying to add DataStore Node of type MArgument (aka MType_Undef)" },

           // ProgressMonitor errors:
           { LLErrorName::Aborted, "Computation aborted by the user." },
		});
		return errMap;
	}

	std::string ErrorManager::exceptionDetailsSymbol = "LLU`$LastFailureParameters";

	void ErrorManager::setExceptionDetailsSymbol(std::string newSymbol) {
		exceptionDetailsSymbol = std::move(newSymbol);
	}

	const std::string& ErrorManager::getExceptionDetailsSymbol() {
		return exceptionDetailsSymbol;
	}

	int& ErrorManager::nextErrorId() {
		static int id = LLErrorCode::VersionError;
		return id;
	}

	auto ErrorManager::registerLLUErrors(std::initializer_list<ErrorStringData> initList) -> ErrorMap {
		ErrorMap e;
		for (auto&& err : initList) {
			e.emplace(err.first, LibraryLinkError { nextErrorId()--, err.first, err.second });
		}
		return e;
	}

	void ErrorManager::registerPacletErrors(const std::vector<ErrorStringData>& errs) {
		for (auto&& err : errs) {
			set(err);
		}
	}

	void ErrorManager::set(const ErrorStringData& errorData) {
		auto& errorMap = errors();
		auto elem = errorMap.emplace(errorData.first, LibraryLinkError { nextErrorId()--, errorData.first, errorData.second });
		if (!elem.second) {
			// Revert nextErrorId because nothing was inserted
			nextErrorId()++;

			// Throw only if someone attempted to insert an error with existing key but different message
			if (elem.first->second.message() != errorData.second) {
				throw errors().find("ErrorManagerCreateNameError")->second;
			}
		}
	}

	template<typename... T>
	void ErrorManager::throwException(const std::string& errorName, T&&... args) {
		throwException(LibDataHolder::getLibraryData(), errorName, std::forward<T>(args)...);
	}

	template<typename... T>
	void ErrorManager::throwException(WolframLibraryData libData, const std::string& errorName, T&&... args) {
		constexpr auto argCount = sizeof...(T);
		MLStream<ML::Encoding::UTF8> mls { libData->getWSLINK(libData) };
		mls << ML::Function("EvaluatePacket", 1);
		mls << ML::Function("Set", 2);
		mls << ML::Symbol(exceptionDetailsSymbol);
		mls << ML::List(argCount);
		static_cast<void>(std::initializer_list<int> { (mls << args, 0)... });
		libData->processWSLINK(mls.get());
		auto pkt = MLNextPacket(mls.get());
		if ( pkt == RETURNPKT) {
			mls << ML::NewPacket;
		}
		throw findError(errorName);
	}

	const LibraryLinkError& ErrorManager::findError(int errorId) {
		for (auto&& err : errors()) {
			if (err.second.id() == errorId) {
				return err.second;
			}
		}
		throw errors().find("ErrorManagerThrowIdError")->second;
	}

	const LibraryLinkError& ErrorManager::findError(const std::string& errorName) {
		const auto& exception = errors().find(errorName);
		if (exception == errors().end()) {
			throw errors().find("ErrorManagerThrowNameError")->second;
		}
		return exception->second;
	}

	void ErrorManager::sendRegisteredErrorsViaMathlink(MLINK mlp) {
		MLStream<ML::Encoding::UTF8> ms(mlp, "List", 0);

		ms << ML::NewPacket << ML::Association(static_cast<int>(errors().size()));

		for (const auto& err : errors()) {
			ms << ML::Rule << err.first << ML::List(2) << err.second.id() << err.second.message();
		}

		ms << ML::EndPacket << ML::Flush;
	}

	EXTERN_C DLLEXPORT int sendRegisteredErrors(WolframLibraryData libData, MLINK mlp) {
		Unused(libData);
		auto err = LLErrorCode::NoError;
		try {
			ErrorManager::sendRegisteredErrorsViaMathlink(mlp);
		}
		catch (LibraryLinkError& e) {
			err = e.which();
		}
		catch (...) {
			err = LLErrorCode::FunctionError;
		}
		return err;
	}


#define LLU_DEFINE_ERROR_NAME(errorIdentifier) const std::string errorIdentifier = #errorIdentifier

	namespace LLErrorName {
		LLU_DEFINE_ERROR_NAME(VersionError);
		LLU_DEFINE_ERROR_NAME(FunctionError);
		LLU_DEFINE_ERROR_NAME(MemoryError);
		LLU_DEFINE_ERROR_NAME(NumericalError);
		LLU_DEFINE_ERROR_NAME(DimensionsError);
		LLU_DEFINE_ERROR_NAME(RankError);
		LLU_DEFINE_ERROR_NAME(TypeError);
		LLU_DEFINE_ERROR_NAME(NoError);

		LLU_DEFINE_ERROR_NAME(MArgumentLibDataError);
		LLU_DEFINE_ERROR_NAME(MArgumentIndexError);
		LLU_DEFINE_ERROR_NAME(MArgumentNumericArrayError);
		LLU_DEFINE_ERROR_NAME(MArgumentTensorError);
		LLU_DEFINE_ERROR_NAME(MArgumentImageError);

		LLU_DEFINE_ERROR_NAME(ErrorManagerThrowIdError);
		LLU_DEFINE_ERROR_NAME(ErrorManagerThrowNameError);
		LLU_DEFINE_ERROR_NAME(ErrorManagerCreateNameError);

		LLU_DEFINE_ERROR_NAME(NumericArrayInitError);
		LLU_DEFINE_ERROR_NAME(NumericArrayNewError);
		LLU_DEFINE_ERROR_NAME(NumericArrayCloneError);
		LLU_DEFINE_ERROR_NAME(NumericArrayTypeError);
		LLU_DEFINE_ERROR_NAME(NumericArraySizeError);
		LLU_DEFINE_ERROR_NAME(NumericArrayIndexError);
		LLU_DEFINE_ERROR_NAME(NumericArrayConversionError);

		LLU_DEFINE_ERROR_NAME(TensorInitError);
		LLU_DEFINE_ERROR_NAME(TensorNewError);
		LLU_DEFINE_ERROR_NAME(TensorCloneError);
		LLU_DEFINE_ERROR_NAME(TensorTypeError);
		LLU_DEFINE_ERROR_NAME(TensorSizeError);
		LLU_DEFINE_ERROR_NAME(TensorIndexError);

		LLU_DEFINE_ERROR_NAME(ImageInitError);
		LLU_DEFINE_ERROR_NAME(ImageNewError);
		LLU_DEFINE_ERROR_NAME(ImageCloneError);
		LLU_DEFINE_ERROR_NAME(ImageTypeError);
		LLU_DEFINE_ERROR_NAME(ImageSizeError);
		LLU_DEFINE_ERROR_NAME(ImageIndexError);

		LLU_DEFINE_ERROR_NAME(MLTestHeadError);
		LLU_DEFINE_ERROR_NAME(MLPutSymbolError);
		LLU_DEFINE_ERROR_NAME(MLPutFunctionError);
		LLU_DEFINE_ERROR_NAME(MLTestSymbolError);
		LLU_DEFINE_ERROR_NAME(MLWrongSymbolForBool);
		LLU_DEFINE_ERROR_NAME(MLGetListError);
		LLU_DEFINE_ERROR_NAME(MLGetScalarError);
		LLU_DEFINE_ERROR_NAME(MLGetStringError);
		LLU_DEFINE_ERROR_NAME(MLGetArrayError);
		LLU_DEFINE_ERROR_NAME(MLPutListError);
		LLU_DEFINE_ERROR_NAME(MLPutScalarError);
		LLU_DEFINE_ERROR_NAME(MLPutStringError);
		LLU_DEFINE_ERROR_NAME(MLPutArrayError);
		LLU_DEFINE_ERROR_NAME(MLGetSymbolError);
		LLU_DEFINE_ERROR_NAME(MLGetFunctionError);
		LLU_DEFINE_ERROR_NAME(MLPacketHandleError);
		LLU_DEFINE_ERROR_NAME(MLFlowControlError);
		LLU_DEFINE_ERROR_NAME(MLTransferToLoopbackError);
		LLU_DEFINE_ERROR_NAME(MLCreateLoopbackError);
		LLU_DEFINE_ERROR_NAME(MLLoopbackStackSizeError);

		LLU_DEFINE_ERROR_NAME(DLNullRawNode);
		LLU_DEFINE_ERROR_NAME(DLInvalidNodeType);
		LLU_DEFINE_ERROR_NAME(DLGetNodeDataError);
		LLU_DEFINE_ERROR_NAME(DLNullRawDataStore);
		LLU_DEFINE_ERROR_NAME(DLPushBackTypeError);

		LLU_DEFINE_ERROR_NAME(ArgumentCreateNull);
		LLU_DEFINE_ERROR_NAME(ArgumentAddNodeMArgument);

		LLU_DEFINE_ERROR_NAME(Aborted);
	}

} /* namespace LibraryLinkUtils */

