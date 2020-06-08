/**
 * @file	Interfaces.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	November 28, 2019
 * @brief
 */
#ifndef LLU_CONTAINERS_INTERFACES_H
#define LLU_CONTAINERS_INTERFACES_H

#include "LLU/LibraryData.h"

namespace LLU {

	/**
	*  @brief Abstract class that defines a basic set of operations on an image
	 */
	struct ImageInterface {
		
		// Provide special member functions
		ImageInterface() = default;
		virtual ~ImageInterface() = default;
		ImageInterface(const ImageInterface&) = default;
		ImageInterface& operator=(const ImageInterface&) = default;
		ImageInterface(ImageInterface&&) = default;
		ImageInterface& operator=(ImageInterface&&) = default;
		
		/**
		 *   @brief Get colorspace which describes how colors are represented as numbers
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColorSpace.html>
		 **/
		[[nodiscard]] virtual colorspace_t colorspace() const = 0;

		/**
		 *   @brief Get number of rows
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRowCount.html>
		 **/
		[[nodiscard]] virtual mint rows() const = 0;

		/**
		 *   @brief Get number of columns
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getColumnCount.html>
		 **/
		[[nodiscard]] virtual mint columns() const = 0;

		/**
		 *   @brief Get number of slices
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getSliceCount.html>
		 **/
		[[nodiscard]] virtual mint slices() const = 0;

		/**
		 *   @brief Get number of channels
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getChannels.html>
		 **/
		[[nodiscard]] virtual mint channels() const = 0;

		/**
		 *   @brief Check if there is an alpha channel in the image
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_alphaChannelQ.html>
		 **/
		[[nodiscard]] virtual bool alphaChannelQ() const = 0;

		/**
		 *   @brief Check if the image is interleaved
		 *   @see <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_interleavedQ.html>
		 **/
		[[nodiscard]] virtual bool interleavedQ() const = 0;

		/**
		 *   @brief Check if the image is 3D
		 **/
		[[nodiscard]] virtual bool is3D() const = 0;

		/**
		 * @brief   Get rank
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRank.html>
		 */
		[[nodiscard]] virtual mint getRank() const = 0;

		/**
		 * @brief   Get the total number of pixels in the image
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getFlattenedLength.html>
		 */
		[[nodiscard]] virtual mint getFlattenedLength() const = 0;

		/**
		 * @brief   Get the data type of the image
		 * @return  type of elements (see definition of \c imagedata_t)
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getDataType.html>
		 */
		[[nodiscard]] virtual imagedata_t type() const = 0;

		/**
		 * @brief   Get access to raw image data. Use with caution.
		 * @return  pointer to the raw data
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MImage_getRawData.html>
		 */
		[[nodiscard]] virtual void* rawData() const = 0;
	};

	/**
	*  @brief Abstract class that defines a basic set of operations on a numeric array
	*/
	struct NumericArrayInterface {

		// Provide special member functions
		NumericArrayInterface() = default;
		virtual ~NumericArrayInterface() = default;
		NumericArrayInterface(const NumericArrayInterface&) = default;
		NumericArrayInterface& operator=(const NumericArrayInterface&) = default;
		NumericArrayInterface(NumericArrayInterface&&) = default;
		NumericArrayInterface& operator=(NumericArrayInterface&&) = default;

		/**
		 * @brief   Get rank
		 * @return  number of dimensions in the array
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_getRank.html>
		 */
		[[nodiscard]] virtual mint getRank() const = 0;

		/**
		 * @brief   Get dimensions
		 * @return  raw pointer to dimensions of the array
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_getDimensions.html>
		 */
		[[nodiscard]] virtual mint const* getDimensions() const = 0;

		/**
		 * @brief   Get length
		 * @return  total number of elements
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_getFlattenedLength.html>
		 */
		[[nodiscard]] virtual mint getFlattenedLength() const = 0;

		/**
		 * @brief   Get the data type of this array
		 * @return  type of elements (see definition of \c numericarray_data_t)
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_getDataType.html>
		 */
		[[nodiscard]] virtual numericarray_data_t type() const = 0;

		/**
		 * @brief   Get access to the raw data. Use with caution.
		 * @return  pointer to the raw data
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MNumericArray_getData.html>
		 */
		[[nodiscard]] virtual void* rawData() const = 0;
	};

	/**
	*  @brief Abstract class that defines a basic set of operations on a tensor
	*/
	struct TensorInterface {

		// Provide special member functions
		TensorInterface() = default;
		virtual ~TensorInterface() = default;
		TensorInterface(const TensorInterface&) = default;
		TensorInterface& operator=(const TensorInterface&) = default;
		TensorInterface(TensorInterface&&) = default;
		TensorInterface& operator=(TensorInterface&&) = default;

		/**
		 * @brief   Get rank
		 * @return  number of dimensions in this tensor
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_getRank.html>
		 */
		[[nodiscard]] virtual mint getRank() const = 0;

		/**
		 * @brief   Get dimensions
		 * @return  raw pointer to dimensions of this tensor
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_getDimensions.html>
		 */
		[[nodiscard]] virtual mint const* getDimensions() const = 0;

		/**
		 * @brief   Get total length
		 * @return  total number of elements
		 * @see     <http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_getFlattenedLength.html>
		 */
		[[nodiscard]] virtual mint getFlattenedLength() const = 0;

		/**
		 * @brief   Get the data type of this tensor
		 * @return  type of elements (MType_Integer, MType_Real or MType_Complex)
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MTensor_getType.html>
		 */
		[[nodiscard]] virtual mint type() const = 0;

		/**
		 * @brief   Get raw pointer to the data of this tensor
		 */
		[[nodiscard]] virtual void* rawData() const = 0;
	};
}

#endif	  // LLU_CONTAINERS_INTERFACES_H
