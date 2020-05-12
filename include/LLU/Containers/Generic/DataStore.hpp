/**
 * @file    DataStore.hpp
 * @brief   Definition and implementation of generic DataStore wrapper.
 */

#ifndef LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE
#define LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE

#include <LLU/Containers/Generic/Base.hpp>
#include <LLU/Containers/Iterators/DataStore.hpp>

#include <LLU/MArgument.h>
#include <LLU/TypedMArgument.h>

namespace LLU {

	/// MContainer specialization for DataStore is called GenericDataList
	using GenericDataList = MContainer<MArgumentType::DataStore>;

	/**
	 *  @brief  MContainer specialization for DataStore, provides basic list interface for the underlying raw DataStore.
	 */
	template<>
	class MContainer<MArgumentType::DataStore> : public MContainerBase<MArgumentType::DataStore> {

		template<typename T>
		using EnableIfArgumentType = std::enable_if_t<Argument::PrimitiveQ<remove_cv_ref<T>> || Argument::WrapperQ<remove_cv_ref<T>>, int>;

		template<MArgumentType T>
		using EnableIfUnambiguousWrapperType = std::enable_if_t<!Argument::PrimitiveQ<Argument::WrapperType<T>>, int>;

	public:
		using iterator = DataStoreIterator;
		using const_iterator = iterator;

		/// Inherit constructors from MContainerBase
		using MContainerBase<MArgumentType::DataStore>::MContainerBase;

		/**
		 * @brief   Default constructor, creates empty DataStore owned by the Library
		 */
		MContainer() : MContainer(LibraryData::DataStoreAPI()->createDataStore(), Ownership::Library) {}

		/**
		 * @brief   Create new MContainer wrapping a given raw DataStore
		 * @param   c - a DataStore
		 * @param   owner - who manages the memory the raw DataStore
		 * @note    An exception will be thrown if you try to create a Shared DataStore because LibraryLink does not allow for shared DataStores.
		 */
		MContainer(Container c, Ownership owner);

		/**
		 * @brief   Clone this MContainer, performs a deep copy of the underlying DataStore.
		 * @note    The cloned MContainer always belongs to the library (Ownership::Library) because LibraryLink has no idea of its existence.
		 * @return  new MContainer, by value
		 */
		MContainer clone() const {
			return MContainer {cloneContainer(), Ownership::Library};
		}

		/**
		 * @brief   Get the length of the DataStore.
		 * @return  total number of nodes in the DataStore
		 */
		mint length() const {
			return LibraryData::DataStoreAPI()->DataStore_getLength(this->getContainer());
		}

		/**
		 * @brief   Get the first node of the DataStore.
		 * @return  first node, if it doesn't exist the behavior is undefined
		 */
		DataStoreNode front() const {
			return LibraryData::DataStoreAPI()->DataStore_getFirstNode(this->getContainer());
		};

		/**
		 * @brief   Get the last node of the DataStore.
		 * @return  last node, if it doesn't exist the behavior is undefined
		 */
		DataStoreNode back() const {
			return LibraryData::DataStoreAPI()->DataStore_getLastNode(this->getContainer());
		};

		/// Proxy iterator to the first element of the DataStore
		iterator begin() const {
			return front();
		}

		/// Proxy iterator past the last element of the DataStore
		iterator end() const {
			return nullptr;
		}

		/// @copydoc begin()
		const_iterator cbegin() const {
			return begin();
		}

		/// @copydoc end()
		const_iterator cend() const {
			return end();
		}

		/**
		 * @brief   Add new nameless node at the end of the underlying DataStore
		 * @tparam  T - any valid argument type (either primitive or a wrapper) except for MTensor/MNumericArray
		 * @param   nodeValue - value to be moved to the new DataStore node
		 * @warning MTensor and MNumericArray are actually the same type, so this function cannot handle them correctly as LLU would not be able to figure out
		 * which function from the LibraryLink API to call. Use push_back templated with MArgumentType instead.
		 */
		template<typename T, EnableIfArgumentType<T> = 0>
		void push_back(T nodeValue);

		/**
		 * @brief   Add new named node at the end of the underlying DataStore
		 * @tparam  T - any valid argument type (either primitive or a wrapper) except for MTensor/MNumericArray
		 * @param   name - name of the new node, names in a DataStore do not have to be unique
		 * @param   nodeValue - value to be moved to the new DataStore node
		 * @warning MTensor and MNumericArray are actually the same type, so this function cannot handle them correctly as LLU would not be able to figure out
		 * which function from the LibraryLink API to call. Use push_back templated with MArgumentType instead.
		 */
		template<typename T, EnableIfArgumentType<T> = 0>
		void push_back(std::string_view name, T nodeValue);

		/**
		 * @brief   Add new nameless node at the end of the underlying DataStore
		 * @tparam  Type - type of the node data expressed via the MArgumentType enum
		 * @param   nodeValue - a value to be pushed as the new node, must be a wrapper over a primitive LibraryLink type
		 */
		template<MArgumentType Type, EnableIfUnambiguousWrapperType<Type> = 0>
		void push_back(Argument::WrapperType<Type> nodeValue) {
			PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), Argument::toPrimitiveType<Type>(nodeValue));
		}

		/**
		 * @brief   Add new named node at the end of the underlying DataStore
		 * @tparam  Type - type of the node data expressed via the MArgumentType enum
		 * @param   name - name of the new node, names in a DataStore do not have to be unique
		 * @param   nodeValue - a value to be pushed as the new node, must be a wrapper over a primitive LibraryLink type
		 */
		template<MArgumentType Type, EnableIfUnambiguousWrapperType<Type> = 0>
		void push_back(std::string_view name, Argument::WrapperType<Type> nodeValue) {
			PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), name, Argument::toPrimitiveType<Type>(nodeValue));
		}

		/**
		 * @brief   Add new nameless node at the end of the underlying DataStore
		 * @tparam  Type - type of the node data expressed via the MArgumentType enum
		 * @param   nodeValue - a value to be pushed as the new node, must be of a primitive LibraryLink type
		 */
		template<MArgumentType Type>
		void push_back(Argument::CType<Type> nodeValue) {
			PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), nodeValue);
		}

		/**
		 * @brief   Add new named node at the end of the underlying DataStore
		 * @tparam  Type - type of the node data expressed via the MArgumentType enum
		 * @param   name - name of the new node, names in a DataStore do not have to be unique
		 * @param   nodeValue - a value to be pushed as the new node, must be of a primitive LibraryLink type
		 */
		template<MArgumentType Type>
		void push_back(std::string_view name, Argument::CType<Type> nodeValue) {
			PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), name, nodeValue);
		}

		/**
		 * @brief   Add new nameless node at the end of the underlying DataStore
		 * @param   node - a value to be pushed as the new node
		 */
		void push_back(const Argument::Typed::Any& node);

		/**
		 * @brief   Add new named node at the end of the underlying DataStore
		 * @param   name - name of the new node, names in a DataStore do not have to be unique
		 * @param   node - a value to be pushed as the new node,
		 */
		void push_back(std::string_view name, const Argument::Typed::Any& node);

	private:
		/// Make a deep copy of the raw container
		Container cloneImpl() const override {
			return LibraryData::DataStoreAPI()->copyDataStore(this->getContainer());
		}

		/**
		 * @brief   Get a share count.
		 * @return  always 0 to indicate that DataStore cannot be shared
		 */
		mint shareCountImpl() const noexcept override {
			return 0;
		}

		/// @copydoc   MContainerBase::pass(MArgument&)
		void passImpl(MArgument& res) const noexcept override {
			MArgument_setDataStore(res, this->getContainer());
		}
	};

	template<typename T, GenericDataList::EnableIfArgumentType<T>>
	void GenericDataList::push_back(T nodeValue) {
		static_assert(!std::is_same_v<T, MTensor>, "Do not use push_back templated on the argument type with MTensor or MNumericArray.");
		if constexpr (Argument::PrimitiveQ<T>) {
			constexpr MArgumentType Type = Argument::PrimitiveIndex<T>;
			PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), nodeValue);
		} else if constexpr (Argument::WrapperQ<T>) {
			constexpr MArgumentType Type = Argument::WrapperIndex<T>;
			PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), Argument::toPrimitiveType<Type>(nodeValue));
		}
	}

	template<typename T,  GenericDataList::EnableIfArgumentType<T>>
	void  GenericDataList::push_back(std::string_view name, T nodeValue) {
		static_assert(!std::is_same_v<T, MTensor>, "Do not use push_back templated on the argument type with MTensor or MNumericArray.");
		if constexpr (Argument::PrimitiveQ<T>) {
			constexpr MArgumentType Type = Argument::PrimitiveIndex<T>;
			PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), name, nodeValue);
		} else if constexpr (Argument::WrapperQ<T>) {
			constexpr MArgumentType Type = Argument::WrapperIndex<T>;
			PrimitiveWrapper<Type>::addDataStoreNode(getContainer(), name, Argument::toPrimitiveType<Type>(nodeValue));
		}
	}

	template<typename T>
	T GenericDataNode::as() const {
		auto v = value();
		auto* ptr = std::get_if<T>(std::addressof(v));
		if (!ptr) {
			ErrorManager::throwException(ErrorName::DLInvalidNodeType);
		}
		return std::move(*ptr);
	}
}

#endif	  // LLU_INCLUDE_LLU_CONTAINERS_GENERIC_DATASTORE
