/**
 * @file
 * @brief
 */
#ifndef LLU_INCLUDE_LLU_CONTAINERS_GENERIC_IMAGE
#define LLU_INCLUDE_LLU_CONTAINERS_GENERIC_IMAGE

#include "LLU/Containers/Generic/Base.hpp"

namespace LLU {

	template<class PassingMode>
	class MContainer<MArgumentType::Image, PassingMode>;

	/// MContainer specialization for MImage is called GenericImage
	template<class PassingMode>
	using GenericImage = MContainer<MArgumentType::Image, PassingMode>;

	/**
	 *  @brief  MContainer specialization for MImage
	 *  @tparam PassingMode - passing policy
	 */
	template<class PassingMode>
	class MContainer<MArgumentType::Image, PassingMode> : public MContainerBase<MArgumentType::Image, PassingMode> {
	public:
		/// Inherit constructors from MContainerBase
		using MContainerBase<MArgumentType::Image, PassingMode>::MContainerBase;

		/// Default constructor, the MContainer does not manage any instance of MImage.
		MContainer() = default;

		/**
		 * @brief Create new 2D MImage based on given parameters
		 * @param width - image width in pixels (number of columns)
		 * @param height - image height in pixels (number of rows)
		 * @param channels - number of color channels (for example 3 for RGB, 1 for greyscale)
		 * @param type - image data type (1-bit, 8-bit, Real32, etc.)
		 * @param colorSpace - image color space
		 * @param interleaving - whether the image data should be interleaved or not
		 */
		MContainer(mint width, mint height, mint channels, imagedata_t type, colorspace_t colorSpace, mbool interleaving) :
				MContainer(0, width, height, channels, type, colorSpace, interleaving) {
		}

		/**
		 * @brief Create new 2D or 3D MImage based on given parameters
		 * @param slices - number of slices (0 for 2D Image, any positive number for Image3D)
		 * @param width - image width in pixels (number of columns)
		 * @param height - image height in pixels (number of rows)
		 * @param channels - number of color channels (for example 3 for RGB, 1 for greyscale)
		 * @param type - image data type (1-bit, 8-bit, Real32, etc.)
		 * @param colorSpace - image color space
		 * @param interleaving - whether the image data should be interleaved or not
		 */
		MContainer(mint slices, mint width, mint height, mint channels, imagedata_t type, colorspace_t colorSpace, mbool interleaving) {
			RawContainer tmp {};
			if (slices ? LibraryData::ImageAPI()->MImage_new3D(slices, width, height, channels, type, colorSpace, interleaving, &tmp) :
			    LibraryData::ImageAPI()->MImage_new2D(width, height, channels, type, colorSpace, interleaving, &tmp)) {
				ErrorManager::throwException(ErrorName::ImageNewError);
			}
			this->setContainer(tmp);
		}

		/**
		 * @brief   Create GenericImage from another GenericImage with different passing mode.
		 * @tparam  P - some passing mode
		 * @param   mc - different GenericImage
		 */
		template<class P>
		explicit MContainer(const MContainer<MArgumentType::Image, P>& mc) : Base(mc) {
		}

		MContainer(const MContainer& mc) = default;

		MContainer(MContainer&& mc) noexcept = default;

		MContainer &operator=(const MContainer &) = default;

		MContainer &operator=(MContainer &&) noexcept = default;

		/**
		 * @brief   Assign a GenericImage with different passing mode.
		 * @tparam  P - some passing mode
		 * @param   mc - different GenericImage
		 * @return  this
		 */
		template<class P>
		MContainer& operator=(const MContainer<MArgumentType::Image, P>& mc) {
			Base::operator=(mc);
			return *this;
		}

		/// Destructor which triggers the appropriate cleanup action which depends on the PassingMode
		~MContainer() override {
			this->cleanup();
		};

		/**
		 * @brief   Convert this object to a new GenericImage of given datatype, optionally changing interleaving
		 * @param   t - destination data type
		 * @param   interleavingQ - whether the converted GenericImage should be interleaved or not
		 * @return  converted GenericImage with Manual passing mode
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_convertType.html>
		 */
		GenericImage<Passing::Manual> convert(imagedata_t t, mbool interleavingQ) const {
			auto newImage = LibraryData::ImageAPI()->MImage_convertType(this->getContainer(), t, interleavingQ);
			if (!newImage) {
				ErrorManager::throwException(ErrorName::ImageNewError, "Conversion to type " + std::to_string(static_cast<int>(t)) + " failed.");
			}
			return newImage;
		}

		/**
		 * @brief   Convert this object to a new GenericImage of given datatype
		 * @param   t - destination data type
		 * @return  converted GenericImage with Manual passing mode
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_convertType.html>
		 */
		GenericImage<Passing::Manual> convert(imagedata_t t) const {
			return convert(t, interleavedQ());
		}

		/**
		 *   @brief Get colorspace of internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColorSpace.html>
		 **/
		colorspace_t colorspace() const noexcept {
			return LibraryData::ImageAPI()->MImage_getColorSpace(this->getContainer());
		}

		/**
		 *   @brief Get number of rows in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRowCount.html>
		 **/
		mint rows() const noexcept {
			return LibraryData::ImageAPI()->MImage_getRowCount(this->getContainer());
		}

		/**
		 *   @brief Get number of columns in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColumnCount.html>
		 **/
		mint columns() const noexcept {
			return LibraryData::ImageAPI()->MImage_getColumnCount(this->getContainer());
		}

		/**
		 *   @brief Get number of slices in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getSliceCount.html>
		 **/
		mint slices() const noexcept {
			return LibraryData::ImageAPI()->MImage_getSliceCount(this->getContainer());
		}

		/**
		 *   @brief Get number of channels in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getChannels.html>
		 **/
		mint channels() const noexcept {
			return LibraryData::ImageAPI()->MImage_getChannels(this->getContainer());
		}

		/**
		 *   @brief Check if there is an alpha channel in internal MImage
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_alphaChannelQ.html>
		 **/
		bool alphaChannelQ() const noexcept {
			return LibraryData::ImageAPI()->MImage_alphaChannelQ(this->getContainer());
		}

		/**
		 *   @brief Check if internal MImage is interleaved
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_interleavedQ.html>
		 **/
		bool interleavedQ() const noexcept {
			return LibraryData::ImageAPI()->MImage_interleavedQ(this->getContainer());
		}

		/**
		 *   @brief Check if internal MImage is 3D
		 **/
		bool is3D() const noexcept {
			return LibraryData::ImageAPI()->MImage_getRank(this->getContainer()) == 3;
		}

		/**
		 * @brief   Get the rank of this GenericImage.
		 * @return  number of dimensions in this GenericImage
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRank.html>
		 */
		mint getRank() const noexcept {
			return LibraryData::ImageAPI()->MImage_getRank(this->getContainer());
		}

		/**
		 * @brief   Get the length of this GenericImage.
		 * @return  total number of elements
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getFlattenedLength.html>
		 */
		mint getFlattenedLength() const {
			return LibraryData::ImageAPI()->MImage_getFlattenedLength(this->getContainer());
		}

		/**
		 * @brief   Get the type of this GenericImage
		 * @return  type of elements (see definition of \c imagedata_t)
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getDataType.html>
		 */
		imagedata_t type() const noexcept {
			return LibraryData::ImageAPI()->MImage_getDataType(this->getContainer());
		}

		/**
		 * @brief   Get access to raw MImage data.
		 * @return  pointer to the raw data
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRawData.html>
		 */
		void* rawData() const noexcept {
			return LibraryData::ImageAPI()->MImage_getRawData(this->getContainer());
		}

	private:
		using Base = MContainerBase<MArgumentType::Image, PassingMode>;
		using RawContainer = typename Base::Container;

		/**
		 *   @copydoc   MContainerBase::clone()
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_clone.html>
		 **/
		RawContainer cloneImpl() const override {
			RawContainer tmp {};
			if (LibraryData::ImageAPI()->MImage_clone(this->getContainer(), &tmp)) {
				ErrorManager::throwException(ErrorName::ImageCloneError);
			}
			return tmp;
		}

		/**
		 * @copydoc MContainerBase::shareCount()
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_shareCount.html>
		 */
		mint shareCountImpl() const noexcept override {
			return LibraryData::ImageAPI()->MImage_shareCount(this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::disown()
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_disown.html>
		 **/
		void disownImpl() const noexcept override {
			LibraryData::ImageAPI()->MImage_disown(this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::free()
		 *   @see 		<http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_free.html>
		 **/
		void freeImpl() const noexcept override {
			LibraryData::ImageAPI()->MImage_free(this->getContainer());
		}

		/**
		 *   @copydoc   MContainerBase::pass
		 **/
		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMImage(res, this->getContainer());
		}

	};

}

#endif //LLU_INCLUDE_LLU_CONTAINERS_GENERIC_IMAGE
