/**
 * @file
 * @brief
 */

#ifndef LLU_INCLUDE_LLU_CONTAINERS_GENERIC_TENSOR
#define LLU_INCLUDE_LLU_CONTAINERS_GENERIC_TENSOR

#include "LLU/Containers/Generic/Base.hpp"

namespace LLU {

	template<class PassingMode>
	class MContainer<MArgumentType::Tensor, PassingMode>;

	/// MContainer specialization for MTensor is called GenericTensor
	template<class PassingMode>
	using GenericTensor = MContainer<MArgumentType::Tensor, PassingMode>;

	/**
	 *  @brief Abstract class that defines a basic set of operations on a tensor
	 */
	struct TensorInterface {
		/**
		 * @brief   Get rank
		 * @return  number of dimensions in this tensor
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_getRank.html>
		 */
		virtual mint getRank() const = 0;

		/**
		 * @brief   Get dimensions
		 * @return  raw pointer to dimensions of this tensor
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_getDimensions.html>
		 */
		virtual mint const* getDimensions() const = 0;

		/**
		 * @brief   Get total length
		 * @return  total number of elements
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_getFlattenedLength.html>
		 */
		virtual mint getFlattenedLength() const = 0;

		/**
		 * @brief   Get the data type of this tensor
		 * @return  type of elements (MType_Integer, MType_Real or MType_Complex)
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_getType.html>
		 */
		virtual mint type() const = 0;

		/**
		 * @brief   Get raw pointer to the data of this tensor
		 */
		virtual void* rawData() const = 0;
	};

	/**
	 * @brief   Simple, light-weight, non-owning wrappper over MTensor.
	 *
	 * Intended for use in functions that only need to access MTensor metadata, where it can alleviate the need for introducing template parameters
	 * for MTensor passing mode (like in GenericTensor) or data type (like in Tensor class).
	 */
	class TensorView : public TensorInterface {
	public:
		TensorView() = default;

		template<class Passing>
		/* implicit */ TensorView(const GenericTensor<Passing>& gTen) : t {gTen.getContainer()} {}

		/// @copydoc TensorInterface::getRank()
		mint getRank() const override {
			return LibraryData::API()->MTensor_getRank(t);
		}

		/// @copydoc TensorInterface::getDimensions()
		mint const* getDimensions() const override {
			return LibraryData::API()->MTensor_getDimensions(t);
		}

		/// @copydoc TensorInterface::getFlattenedLength()
		mint getFlattenedLength() const override {
			return LibraryData::API()->MTensor_getFlattenedLength(t);
		}

		/// @copydoc TensorInterface::type()
		mint type() const override {
			return LibraryData::API()->MTensor_getType(t);
		}

		/// @copybrief TensorInterface::rawData()
		/// @note MTensor does not offer a type-independent function to access raw data, so we access via a function specific to real-valued tensors
		/// and do a reinterpret_cast. Using such obtained pointer may result in undefined behavior.
		void* rawData() const override {
			return reinterpret_cast<void*>(LibraryData::API()->MTensor_getRealData(t));
		}

	private:
		MTensor t = nullptr;
	};
	
	/**
	 *  @brief  MContainer specialization for MTensor
	 *  @tparam PassingMode - passing policy
	 */
	template<class PassingMode>
	class MContainer<MArgumentType::Tensor, PassingMode> : public TensorInterface, public MContainerBase<MArgumentType::Tensor, PassingMode> {
	public:
		/// Inherit constructors from MContainerBase
		using MContainerBase<MArgumentType::Tensor, PassingMode>::MContainerBase;

		/// Default constructor, the MContainer does not manage any instance of MTensor.
		MContainer() = default;

		/**
		 * @brief   Create GenericTensor of given type and shape
		 * @param   type - new GenericTensor type (MType_Integer, MType_Real or MType_Complex)
		 * @param   rank - new GenericTensor rank
		 * @param   dims - new GenericTensor dimensions
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_new.html>
		 */
		MContainer(mint type, mint rank, const mint* dims) {
			RawContainer tmp {};
			if (LibraryData::API()->MTensor_new(type, rank, dims, &tmp)) {
				ErrorManager::throwException(ErrorName::TensorNewError);
			}
			this->setContainer(tmp);
		}

		/**
		 * @brief   Create GenericTensor from another GenericTensor with different passing mode.
		 * @tparam  P - some passing mode
		 * @param   mc - different GenericTensor
		 */
		template<class P>
		explicit MContainer(const MContainer<MArgumentType::Tensor, P>& mc) : Base(mc) {}

		MContainer(const MContainer& mc) = default;

		MContainer(MContainer&& mc) noexcept = default;

		MContainer& operator=(const MContainer&) = default;

		MContainer& operator=(MContainer&& mc) noexcept = default;

		/**
		 * @brief   Assign a GenericTensor with different passing mode.
		 * @tparam  P - some passing mode
		 * @param   mc - different GenericTensor
		 * @return  this
		 */
		template<class P>
		MContainer& operator=(const MContainer<MArgumentType::Tensor, P>& mc) {
			Base::operator=(mc);
			return *this;
		}

		/// Destructor which triggers the appropriate cleanup action which depends on the PassingMode
		~MContainer() {
			this->cleanup();
		};

		/// @copydoc TensorInterface::getRank()
		mint getRank() const override {
			return LibraryData::API()->MTensor_getRank(this->getContainer());
		}

		/// @copydoc TensorInterface::getDimensions()
		mint const* getDimensions() const override {
			return LibraryData::API()->MTensor_getDimensions(this->getContainer());
		}

		/// @copydoc TensorInterface::getFlattenedLength()
		mint getFlattenedLength() const override {
			return LibraryData::API()->MTensor_getFlattenedLength(this->getContainer());
		}

		/// @copydoc TensorInterface::type()
		mint type() const override {
			return LibraryData::API()->MTensor_getType(this->getContainer());
		}

		/// @copybrief TensorInterface::rawData()
		/// @note MTensor does not offer a type-independent function to access raw data, so we access via a function specific to real-valued tensors
		/// and do a reinterpret_cast. Using such obtained pointer may result in undefined behavior.
		void* rawData() const override {
			return reinterpret_cast<void*>(LibraryData::API()->MTensor_getRealData(this->getContainer()));
		}
	private:
		using Base = MContainerBase<MArgumentType::Tensor, PassingMode>;
		using RawContainer = typename Base::Container;

		/**
		 * @copydoc MContainerBase::shareCount()
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_shareCount.html>
		 */
		mint shareCountImpl() const noexcept override {
			return LibraryData::API()->MTensor_shareCount(this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::disown()
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_disown.html>
		 **/
		void disownImpl() const noexcept override {
			LibraryData::API()->MTensor_disown(this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::free()
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_free.html>
		 **/
		void freeImpl() const noexcept override {
			LibraryData::API()->MTensor_free(this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::pass
		 **/
		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMTensor(res, this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::clone()
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_clone.html>
		 **/
		RawContainer cloneImpl() const override {
			RawContainer tmp {};
			if (LibraryData::API()->MTensor_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(ErrorName::TensorCloneError);
			}
			return tmp;
		}
	};

}

#endif	  // LLU_INCLUDE_LLU_CONTAINERS_GENERIC_TENSOR
