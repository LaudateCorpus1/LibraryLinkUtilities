/**
 * @file	MArgumentManager.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 *
 * @brief	Declaration of MArgumentManager class
 *
 */

#ifndef LLUTILS_MARGUMENTMANAGER_H_
#define LLUTILS_MARGUMENTMANAGER_H_

#include <complex>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "LLU/ErrorLog/ErrorManager.h"
#include "LLU/Containers/DataList.h"
#include "LLU/Containers/Image.h"
#include "LLU/Containers/LibDataHolder.h"
#include "LLU/Containers/NumericArray.h"
#include "LLU/Containers/Tensor.h"
#include "LLU/Containers/Passing/Automatic.hpp"
#include "LLU/MArgument.h"
#include "LLU/ProgressMonitor.h"

#include "WolframLibrary.h"

namespace LibraryLinkUtils {

	/**
	 * @class	MArgumentManager
	 * @brief	Manages arguments exchanged between the paclet C++ code and LibraryLink interface.
	 *
	 * MArgumentManager provides a safe way to access MArguments received from LibraryLink and takes care of memory management both for in- and out- arguments.
	 * Using MArgumentManager one can perform generic operations on NumericArrays, Tensors and Images independent of their data type.
	 *
	 * @todo	Make sure all MArgument passing modes ("Constant", "Shared", etc.) are correctly handled
	 **/
	class MArgumentManager : public LibDataHolder {
	public:

		/**
		 *   @brief         Constructor
		 *   @param[in]     Argc - number of MArguments provided
		 *   @param[in]     Args - MArguments provided
		 *   @param[in]		Res - reference to output MArgument
		 *   @throws        LLErrorName::MArgumentInitError - if static member libData is not initialized
		 **/
		MArgumentManager(mint Argc, MArgument* Args, MArgument& Res);

		/**
		 *   @brief         Constructor
		 *   @param[in]     ld - library data
		 *   @param[in]     Argc - number of MArguments provided
		 *   @param[in]     Args - MArguments provided
		 *   @param[in]		Res - reference to output MArgument
		 **/
		MArgumentManager(WolframLibraryData ld, mint Argc, MArgument* Args, MArgument& Res);

		/**
		 *   @brief Default destructor
		 **/
		virtual ~MArgumentManager() = default;

		/**
		 *   @brief         Get MArgument of type \b mbool at position \c index
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MArgument of type \b bool at position \c index
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		bool getBoolean(unsigned int index) const;

		/**
		 *   @brief         Set \c result as output MArgument
		 *   @param[in]     result - boolean value to be returned to LibraryLink
		 **/
		void setBoolean(bool result) const noexcept;

		/**
		 *   @brief         Get MArgument of type \b mreal at position \c index
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MArgument of type \b double at position \c index
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		double getReal(unsigned int index) const;

		/**
		 *   @brief         Set \c result as output MArgument
		 *   @param[in]     result - value of type \b double to be returned to LibraryLink
		 **/
		void setReal(double result) const noexcept;

		/**
		 *   @brief         Get MArgument of type \b mint at position \c index
		 *   @tparam		T - integral type to convert \b mint to
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MArgument value at position \c index converted to \b T
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		template<typename T>
		T getInteger(unsigned int index) const;

		/**
		 *   @brief         Set \c result as output MArgument
		 *   @param[in]     result - value of type \b mint to be returned to LibraryLink
		 *   @warning		\c result will be implicitly casted to \b mint with no overflow check
		 **/
		void setInteger(mint result) const noexcept;

		/**
		 *   @brief         Set \c result as output MArgument and check for overflow
		 *   @tparam		T - integral type to be casted to \b mint
		 *   @param[in]     result - value to be returned to LibraryLink
		 **/
		template<typename T>
		bool setMintAndCheck(T result) const noexcept;

		/**
		 *   @brief         Get MArgument of type \b mcomplex at position \c index
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MArgument value at position \c index converted to \b std::complex<double>
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		std::complex<double> getComplex(unsigned int index) const;

		/**
		 *   @brief         Set \c c as output MArgument
		 *   @param[in]     c - value of type \b std::complex<double> to be returned to LibraryLink
		 **/
		void setComplex(std::complex<double> c) const noexcept;

		/**
		 *   @brief         Get value of MArgument of type \b "UTF8String" at position \c index
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       C-string which was received from LibraryLink
		 *   @throws        LLErrorCode::MArgumentIndexError - if \c index is out-of-bounds
		 *
		 *   @note			MArgumentManager is responsible for disowning string arguments. Do not call free() or delete() on resulting pointer.
		 **/
		char* getCString(unsigned int index);

		/**
		 *   @brief         Get value of MArgument of type \b "UTF8String" at position \c index
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       \b std::string which is created from MArgument at position \c index
		 *   @throws        LLErrorCode::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		std::string getString(unsigned int index);

		/**
		 *   @brief         Set \c str as output MArgument
		 *   @param[in]     str - reference to \b std::string to be returned to LibraryLink
		 **/
		void setString(const std::string& str);

		/**
		 *   @overload
		 **/
		void setString(const char* str);

		/**
		 *   @brief         Set \c str as output MArgument
		 *   @param[in]     str - r-value reference to \b std::string to be returned to LibraryLink
		 **/
		void setString(std::string&& str);

		/**
		 *   @brief         Get MArgument of type MNumericArray at position \p index and wrap it into NumericArray
		 *   @tparam		T - type of data stored in NumericArray
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       NumericArray wrapper of MArgument at position \c index
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @see			NumericArray<T>::NumericArray(const MNumericArray);
		 **/
		template<typename T>
		NumericArray<T> getNumericArray(unsigned int index) const;
		
		/**
		 *   @brief         Get MArgument of type MNumericArray at position \c index
		 *   @warning       Use of this function is discouraged. Use getNumericArray instead, if possible.
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MArgument at position \c index interpreted as MNumericArray
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		MNumericArray getMNumericArray(unsigned int index) const;

		/**
		 *   @brief         Get MArgument of type MTensor at position \c index.
		 *   @warning       Use of this function is discouraged. Use getTensor instead, if possible.
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MTensor of MArgument at position \c index
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		MTensor getMTensor(unsigned int index) const;

		/**
		 *   @brief         Set MNumericArray wrapped by \c ra as output MArgument
		 *   @tparam		T - NumericArray data type
		 *   @param[in]     ra - reference to NumericArray which should pass its internal MNumericArray to LibraryLink
		 **/
		template<typename T>
		void setNumericArray(NumericArray<T>& ra);

		/**
		 *   @brief         Set MNumericArray as output MArgument
		 *   @param[in]     ra - MNumericArray to be passed to LibraryLink
		 **/
		void setMNumericArray(MNumericArray ra);

		/**
		 *   @brief         Get type of MNumericArray at position \c index in \c Args
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MNumericArray type
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		numericarray_data_t getNumericArrayType(unsigned int index) const;

		/**
		 *   @brief         Perform operation on NumericArray created from MNumericArray argument at position \p index in \c Args
		 *   @tparam		Operator - any callable class
		 *   @tparam		OpArgs... - types of arguments of \c operator() in class \c Operator
		 *   @param[in]     index - position of MNumericArray in \c Args
		 *   @param[in]     args - arguments of Operator::operator()
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @warning		Operator::operator() has to be a template that takes a const NumericArray<T>& as first argument
		 **/
		template<class Operator, class ... OpArgs>
		void operateOnNumericArray(unsigned int index, OpArgs&&... args);

		/**
		 *   @brief         Perform operation on NumericArray created from MNumericArray argument at position \p index in \c Args
		 *   @tparam		Operator - any callable class
		 *   @param[in]     index - position of MNumericArray in \c Args
		 *   @param[in]     op - callable object (possibly lambda) that takes only one argument - a NumericArray
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		template<class Operator>
		void operateOnNumericArray(unsigned int index, Operator&& op);

		/**
		 *   @brief         Get MArgument of type MTensor at position \p index and wrap it into Tensor object
		 *   @tparam		T - type of data stored in Tensor
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       Tensor wrapper of MArgument at position \c index
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @see			Tensor<T>::Tensor(const MTensor);
		 **/
		template<typename T>
		Tensor<T> getTensor(unsigned int index) const;

		/**
		 *   @brief         Set MTensor wrapped by \c ten as output MArgument
		 *   @tparam		T - Tensor data type
		 *   @param[in]     ten - reference to Tensor which should pass its internal MTensor to LibraryLink
		 **/
		template<typename T>
		void setTensor(Tensor<T>& ten);

		/**
		 *   @brief         Set MTensor as output MArgument
		 *   @param[in]     t - MTensor to be passed to LibraryLink
		 **/
		void setMTensor(MTensor t);

		/**
		 *   @brief         Get type of MTensor at position \c index in \c Args
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MTensor type
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		unsigned char getTensorType(unsigned int index) const;

		/**
		 *   @brief         Perform operation on Tensor created from MTensor argument at position \p index in \c Args
		 *   @tparam		Operator - any callable class
		 *   @tparam		OpArgs... - types of arguments of \c operator() in class \c Operator
		 *   @param[in]     index - position of MTensor in \c Args
		 *   @param[in]     args - arguments of Operator::operator()
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @throws        LLErrorName::MArgumentTensorError - if MTensor argument has incorrect type
		 *   @warning		Operator::operator() has to be a template that takes a const Tensor<T>& as first argument
		 **/
		template<class Operator, class ... Args>
		void operateOnTensor(unsigned int index, Args&&... args);

		/**
		 *   @brief         Perform operation on Tensor created from MTensor argument at position \p index in \c Args
		 *   @tparam		Operator - any callable class
		 *   @param[in]     index - position of MTensor in \c Args
		 *   @param[in]     op - callable object (possibly lambda) that takes only one argument - a Tensor
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @throws        LLErrorName::MArgumentTensorError - if MTensor argument has incorrect type
		 **/
		template<class Operator>
		void operateOnTensor(unsigned int index, Operator&& op);

		/**
		 *   @brief         Get MArgument of type MImage at position \p index and wrap it into Image object
		 *   @tparam		T - type of data stored in Image
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       Image wrapper of MArgument at position \c index
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @see			Image<T>::Image(const MImage ra);
		 **/
		template<typename T>
		Image<T> getImage(unsigned int index) const;

		/**
		 *   @brief         Set MImage wrapped by \c mi as output MArgument
		 *   @tparam		T - Image data type
		 *   @param[in]     mi - reference to Image which should pass its internal MImage to LibraryLink
		 **/
		template<typename T>
		void setImage(Image<T>& mi);

		/**
		 *   @brief         Get type of MImage at position \c index in \c Args
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       MImage type
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		imagedata_t getImageType(unsigned int index) const;

		/**
		 *   @brief         Perform operation on Image created from MImage argument at position \p index in \c Args
		 *   @tparam		Operator - any callable class
		 *   @tparam		OpArgs... - types of arguments of \c operator() in class \c Operator
		 *   @param[in]     index - position of MImage in \c Args
		 *   @param[in]     args - arguments of Operator::operator()
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @throws        LLErrorName::MArgumentImageError - if MImage argument has incorrect type
		 *   @warning		Operator::operator() has to be a template that takes a const Image<T>& as first argument
		 **/
		template<class Operator, class ... Args>
		void operateOnImage(unsigned int index, Args&&... args);

		/**
		 *   @brief         Perform operation on Image created from MImage argument at position \p index in \c Args
		 *   @tparam		Operator - any callable class
		 *   @param[in]     index - position of MImage in \c Args
		 *   @param[in]     op - callable object (possibly lambda) that takes only one argument - an Image
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @throws        LLErrorName::MArgumentImageError - if MImage argument has incorrect type
		 **/
		template<class Operator>
		void operateOnImage(unsigned int index, Operator&& op);

		/**
		 *   @brief         Get DataStore with all nodes of the same type from MArgument at position \c index
		 *   @tparam		T - type of data stored in each node of DataStore, it T is MArgumentType::MArgument it will accept any node
		 *   @param[in]     index - position of desired MArgument in \c Args
		 *   @returns       DataList wrapper of MArgument at position \c index
		 *   @throws        LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 *   @see			DataList<T>::DataList(DataStore ds);
		 **/
		template<MArgumentType T, template<typename> class PassingMode = Passing::Automatic>
		DataList<T, PassingMode> getDataList(unsigned int index) const;

		/**
		 *   @brief         Set DataStore wrapped in DataList \c ds as output MArgument
		 *   @tparam		T - type of data stored in each node of DataStore
		 *   @param[in]     ten - const reference to DataList which should pass its internal DataStore to LibraryLink
		 **/
		template<MArgumentType T, template<typename> class PassingMode = Passing::Automatic>
		void setDataList(const DataList<T, PassingMode>& ds);

		/**
		 *
		 * @param ds
		 */
		void setDataStore(DataStore ds);

		/**
		 * @brief 	Get ProgressMonitor shared with WL Kernel.
		 * @param 	step - step value for progress monitor
		 * @return	A new instance of ProgressMonitor class.
		 * @warning If you haven't specified "ProgressMonitor" option when loading the library function
		 * 			with SafeLibraryFunction, then the behavior of \c getProgressMonitor is undefined.
		 */
		ProgressMonitor getProgressMonitor(double step = .1) const;

	private:
		// Efficient and memory-safe type for storing string arguments from LibraryLink
		using LLStringPtr = std::unique_ptr<char[], decltype(st_WolframLibraryData::UTF8String_disown)>;

	private:
		/**
		 *   @brief			Get MArgument at position \c index
		 *   @param[in]		index - position of desired MArgument in \c Args
		 *   @throws		LLErrorName::MArgumentIndexError - if \c index is out-of-bounds
		 **/
		MArgument getArgs(unsigned int index) const;

		/**
		 * @brief Helper function to initialize string arguments vector
		 */
		void initStringArgs();

		/**
		 * @brief Take ownership of UTF8String argument passed via LibraryLink.
		 *
		 * This wraps the raw char* into unique_ptr and all further accesses to the argument happen via the unique_ptr.
		 * The string argument is automatically deallocated when MArgumentManager instance is destroyed.
		 *
		 * @param index - position of desired MArgument in \c Args
		 */
		void acquireUTF8String(unsigned int index);

		/// Here we store a string that was most recently returned to LibraryLink
		/// [LLDocs]: https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithMathematica.html#262826223 "LibraryLink docs"
		/// @see [LibraryLink docs][LLDocs]
		static std::string stringResultBuffer;

		/// Max \b mint value
		static constexpr mint MINT_MAX = (std::numeric_limits<mint>::max)();

		/// Min \b mint value
		static constexpr mint MINT_MIN = (std::numeric_limits<mint>::min)();

		/// Number of input arguments expected from LibraryLink
		mint argc;

		/// "Array" of input arguments from LibraryLink
		MArgument *args;

		/// Output argument for LibraryLink
		MArgument& res;

		/// Structure to manage string arguments after taking their ownership from LibraryLink
		/// [LLDocs]: https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithMathematica.html#262826223 "LibraryLink docs"
		/// @see [LibraryLink docs][LLDocs]
		std::vector<LLStringPtr> stringArgs;
	};

	template<typename T>
	T MArgumentManager::getInteger(unsigned int index) const {
		return static_cast<T>(MArgument_getInteger(getArgs(index)));
	}

	template<typename T>
	bool MArgumentManager::setMintAndCheck(T result) const noexcept {
		if (result >= MINT_MAX) {
			setInteger(MINT_MAX);
			return true;
		}
		else
			if (result <= MINT_MIN) {
				setInteger(MINT_MIN);
				return true;
			}
			else {
				setInteger(result);
				return false;
			}
	}

	template<typename T>
	NumericArray<T> MArgumentManager::getNumericArray(unsigned int index) const {
		return NumericArray<T>(MArgument_getMNumericArray(getArgs(index)));
	}

	template<typename T>
	void MArgumentManager::setNumericArray(NumericArray<T>& ra) {
		ra.passAsResult(res);
	}

	template<class Operator, class ... Args>
	void MArgumentManager::operateOnNumericArray(unsigned int index,  Args&&... args) {
		Operator op;
		switch (getNumericArrayType(index)) {
			case MNumericArray_Type_Bit8:
				op(this->getNumericArray<int8_t>(index), std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_UBit8:
				op(this->getNumericArray<uint8_t>(index), std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Bit16:
				op(this->getNumericArray<int16_t>(index), std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_UBit16:
				op(this->getNumericArray<uint16_t>(index), std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Bit32:
				op(this->getNumericArray<int32_t>(index), std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_UBit32:
				op(this->getNumericArray<uint32_t>(index), std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Bit64:
				op(this->getNumericArray<int64_t>(index), std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_UBit64:
				op(this->getNumericArray<uint64_t>(index), std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Real32:
				op(this->getNumericArray<float>(index), std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Real64:
				op(this->getNumericArray<double>(index), std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Complex_Real32:
				op(this->getNumericArray<std::complex<float>>(index), std::forward<Args>(args)...);
				break;
			case MNumericArray_Type_Complex_Real64:
				op(this->getNumericArray<std::complex<double>>(index), std::forward<Args>(args)...);
				break;
			default:
				ErrorManager::throwExceptionWithDebugInfo(LLErrorName::MArgumentNumericArrayError, "Incorrect type of NumericArray argument. Argument index: " + std::to_string(index));
		}
	}

	template<class Operator>
	void MArgumentManager::operateOnNumericArray(unsigned int index, Operator&& op) {
		switch (getNumericArrayType(index)) {
			case MNumericArray_Type_Bit8:
				op(this->getNumericArray<int8_t>(index));
				break;
			case MNumericArray_Type_UBit8:
				op(this->getNumericArray<uint8_t>(index));
				break;
			case MNumericArray_Type_Bit16:
				op(this->getNumericArray<int16_t>(index));
				break;
			case MNumericArray_Type_UBit16:
				op(this->getNumericArray<uint16_t>(index));
				break;
			case MNumericArray_Type_Bit32:
				op(this->getNumericArray<int32_t>(index));
				break;
			case MNumericArray_Type_UBit32:
				op(this->getNumericArray<uint32_t>(index));
				break;
			case MNumericArray_Type_Bit64:
				op(this->getNumericArray<int64_t>(index));
				break;
			case MNumericArray_Type_UBit64:
				op(this->getNumericArray<uint64_t>(index));
				break;
			case MNumericArray_Type_Real32:
				op(this->getNumericArray<float>(index));
				break;
			case MNumericArray_Type_Real64:
				op(this->getNumericArray<double>(index));
				break;
			case MNumericArray_Type_Complex_Real32:
				op(this->getNumericArray<std::complex<float>>(index));
				break;
			case MNumericArray_Type_Complex_Real64:
				op(this->getNumericArray<std::complex<double>>(index));
				break;
			default:
				ErrorManager::throwExceptionWithDebugInfo(LLErrorName::MArgumentNumericArrayError, "Incorrect type of NumericArray argument. Argument index: " + std::to_string(index));
		}
	}

	template<typename T>
	Tensor<T> MArgumentManager::getTensor(unsigned int index) const {
		return Tensor<T>(MArgument_getMTensor(getArgs(index)));
	}

	template<typename T>
	void MArgumentManager::setTensor(Tensor<T>& ten) {
		ten.passAsResult(res);
	}


	template<class Operator, class... Args>
	void MArgumentManager::operateOnTensor(unsigned int index, Args&&... args) {
		Operator op;
		switch (getTensorType(index)) {
			case MType_Integer:
				op(this->getTensor<mint>(index), std::forward<Args>(args)...);
				break;
			case MType_Real:
				op(this->getTensor<double>(index), std::forward<Args>(args)...);
				break;
			case MType_Complex:
				op(this->getTensor<std::complex<double>>(index), std::forward<Args>(args)...);
				break;
			default:
				ErrorManager::throwExceptionWithDebugInfo(LLErrorName::MArgumentTensorError, "Incorrect type of Tensor argument. Argument index: " + std::to_string(index));
		}
	}


	template<class Operator>
	void MArgumentManager::operateOnTensor(unsigned int index, Operator&& op) {
		switch (getTensorType(index)) {
			case MType_Integer:
				op(this->getTensor<mint>(index));
				break;
			case MType_Real:
				op(this->getTensor<double>(index));
				break;
			case MType_Complex:
				op(this->getTensor<std::complex<double>>(index));
				break;
			default:
				ErrorManager::throwExceptionWithDebugInfo(LLErrorName::MArgumentTensorError, "Incorrect type of Tensor argument. Argument index: " + std::to_string(index));
		}
	}


	template<typename T>
	Image<T> MArgumentManager::getImage(unsigned int index) const {
		return Image<T>(MArgument_getMImage(getArgs(index)));
	}

	template<typename T>
	void MArgumentManager::setImage(Image<T>& mi) {
		mi.passAsResult(res);
	}

	template<class Operator, class... Args>
	void MArgumentManager::operateOnImage(unsigned int index, Args&&... args) {
		Operator op;
		switch (getImageType(index)) {
			case MImage_Type_Bit:
				op(this->getImage<int8_t>(index), std::forward<Args>(args)...);
				break;
			case MImage_Type_Bit8:
				op(this->getImage<uint8_t>(index), std::forward<Args>(args)...);
				break;
			case MImage_Type_Bit16:
				op(this->getImage<uint16_t>(index), std::forward<Args>(args)...);
				break;
			case MImage_Type_Real32:
				op(this->getImage<float>(index), std::forward<Args>(args)...);
				break;
			case MImage_Type_Real:
				op(this->getImage<double>(index), std::forward<Args>(args)...);
				break;
			default:
				ErrorManager::throwExceptionWithDebugInfo(LLErrorName::MArgumentImageError, "Incorrect type of Image argument. Argument index: " + std::to_string(index));
		}
	}

	template<class Operator>
	void MArgumentManager::operateOnImage(unsigned int index, Operator&& op) {
		switch (getImageType(index)) {
			case MImage_Type_Bit:
				op(this->getImage<int8_t>(index));
				break;
			case MImage_Type_Bit8:
				op(this->getImage<uint8_t>(index));
				break;
			case MImage_Type_Bit16:
				op(this->getImage<uint16_t>(index));
				break;
			case MImage_Type_Real32:
				op(this->getImage<float>(index));
				break;
			case MImage_Type_Real:
				op(this->getImage<double>(index));
				break;
			default:
				ErrorManager::throwExceptionWithDebugInfo(LLErrorName::MArgumentImageError, "Incorrect type of Image argument. Argument index: " + std::to_string(index));
		}
	}


	template<MArgumentType T, template<typename> class PassingMode>
	DataList<T, PassingMode> MArgumentManager::getDataList(unsigned int index) const {
		return DataList<T, PassingMode>(MArgument_getDataStore(getArgs(index)));
	}

	template<MArgumentType T, template<typename> class PassingMode>
	void MArgumentManager::setDataList(const DataList<T, PassingMode>& ds) {
		ds.passAsResult(res);
	}

} /* namespace LibraryLinkUtils */


#endif /* LLUTILS_MARGUMENTMANAGER_H_ */
