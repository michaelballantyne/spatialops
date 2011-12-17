#ifndef SpatialOps_SpatialField_h
#define SpatialOps_SpatialField_h

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <sstream>

#include <spatialops/SpatialOpsConfigure.h>

#include <spatialops/structured/ExternalAllocators.h>
#include <spatialops/structured/MemoryTypes.h>
#include <spatialops/structured/MemoryWindow.h>

#ifdef SOPS_BOOST_SERIALIZATION
# include <boost/serialization/serialization.hpp>
# include <boost/serialization/split_member.hpp>
# include <boost/serialization/binary_object.hpp>
#endif

class RHS;

namespace SpatialOps {
namespace structured {

enum StorageMode {
  InternalStorage, ExternalStorage
};

/**
 *  \class SpatialField
 *  \ingroup structured
 *  \ingroup fields
 *  \author James C. Sutherland
 *
 *  \brief Abstracts a field.
 *
 *  \tparam FieldLocation - type traits to describe the location of
 *    this field.  On staggered meshes, this will describe the mesh
 *    this field is associated with.  It also defines whether this
 *    field is on a volume or surface.
 *
 *  \tparam GhostTraits - The ghost type traits.  Must define an
 *    enum value \c NGHOST that gives the number of ghost cells for
 *    this field.
 *
 *  \tparam T - the underlying datatype (defaults to \c double)
 *
 *  \par Related classes:
 *   - \ref MemoryWindow
 *   - \ref SpatialFieldStore
 *   - \ref SpatialOperator
 *
 *  \par Public Typedefs
 *   - \c field_type - this field's type
 *   - \c Ghost - the ghost type traits
 *   - \c Location - the location type traits
 *   - \c value_type  - the type of underlying data being stored in this SpatialField
 *   - \c iterator, \c const_iterator - iterators to the elements in this field
 *   - \c interior_iterator, \c const_interior_iterator - iterators to the interior elements in this field (excludes ghost cells).
 */
template<typename FieldLocation, typename GhostTraits, typename T = double>
class SpatialField {
    typedef SpatialField<FieldLocation, GhostTraits, T> MyType;

    const MemoryWindow fieldWindow_;
    MemoryWindow interiorFieldWindow_;

    T* fieldValues_;
    const bool builtField_;

    MemoryType memType_; ///< Indicates the type of device on which this field is allocated
    unsigned short deviceIndex_; ///< Indicates which device is this field stored on
    void* fieldValuesExtDevice_; ///< External field pointer ( This pointer will only be valid on the device it was created )

    unsigned long int allocatedBytes_;

    inline void reset_values(const T* values);

#ifdef SOPS_BOOST_SERIALIZATION
    friend class boost::serialization::access;

    template<typename Archive>
    void save( Archive& ar, const unsigned int version ) const
    {
      ar << fieldWindow_;
      ar << interiorFieldWindow_;
      ar << builtField_;

      const size_t npts = fieldWindow_.glob_dim(0)
      * fieldWindow_.glob_dim(1)
      * fieldWindow_.glob_dim(2);
      ar << npts;
      for( size_t i=0; i<npts; ++i ) {
        ar << fieldValues_[i];
      }

    }

    template<typename Archive>
    void load( Archive& ar, const unsigned int version )
    {
      ar >> const_cast<MemoryWindow&>(fieldWindow_);
      ar >> interiorFieldWindow_;
      ar >> const_cast<bool&>(builtField_);

      size_t npts;
      ar >> npts;
      if( builtField_ ) {
        fieldValues_ = new T[ npts ];
      }

      for( size_t i=0; i<npts; ++i ) {
        ar >> fieldValues_[i];
      }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif

  public:

    typedef SpatialField<FieldLocation, GhostTraits, T> field_type;
    typedef GhostTraits Ghost;
    typedef FieldLocation Location;
    typedef T AtomicT;
    typedef T value_type;
    typedef FieldIterator<field_type> iterator;
    typedef FieldIterator<field_type> interior_iterator;
    typedef ConstFieldIterator<field_type> const_iterator;
    typedef ConstFieldIterator<field_type> const_interior_iterator;

    /**
     *  \brief Construct a SpatialField
     *  \param window - the MemoryWindow that specifies this field
     *         including ghost cells.
     *  \param mode Storage options.  If InternalStorage then the
     *         fieldValues will be copied into an internal buffer.  If
     *         ExternalStorage then the fieldValues will be stored
     *         externally.  Efficiency suggests that ExternalStorage
     *         is best, since it will avoid excessive copies.  Safety
     *         suggests that InternalStorage is best, since it
     *         protects against memory corruption and inadvertent
     *         deletion of the field's underlying memory.
     */
    SpatialField(const MemoryWindow window, T* const fieldValues,
        const StorageMode mode = InternalStorage, const MemoryType mtype =
            LOCAL_RAM, const unsigned short int devIdx = 0);

    /**
     *  \brief Shallow copy constructor.  This results in two fields
     *  that share the same underlying memory.
     */
    SpatialField(const SpatialField& other);

    virtual ~SpatialField();

    /**
     *  \brief Given the index for this field 0-based including
     *  ghosts, obtain a reference to the field value.
     *  WARNING: slow!
     */
    T& operator()(const size_t i, const size_t j, const size_t k);

    /**
     *  \brief Given the index for this field 0-based including
     *  ghosts, obtain a const reference to the field value.
     *  WARNING: slow!
     */
    const T& operator()(const size_t i, const size_t j, const size_t k) const;

    /**
     *  \brief Given the index for this field 0-based including
     *  ghosts, obtain a reference to the field value.
     *  WARNING: slow!
     */
    T& operator()(const IntVec& ijk);

    /**
     *  \brief Given the index for this field 0-based including
     *  ghosts, obtain a const reference to the field value.
     *  WARNING: slow!
     */
    const T& operator()(const IntVec& ijk) const;

    /**
     *  Index into this field (global index, 0-based in ghost cells).
     *  Note that if this field is windowed, this is still the global
     *  (not windowed) flat index.
     */
    inline T& operator[](const size_t i);
    inline T& operator[](const size_t i) const;

    inline const_iterator begin() const {
      if (memType_ != LOCAL_RAM) {
        throw;
      }
      return const_iterator(fieldValues_,
          fieldWindow_.flat_index(IntVec(0, 0, 0)), fieldWindow_);
    }

    inline iterator begin() {
      if (memType_ != LOCAL_RAM) {
        throw;
      }
      return iterator(fieldValues_, fieldWindow_.flat_index(IntVec(0, 0, 0)),
          fieldWindow_);
    }

    inline const_iterator end() const;
    inline iterator end();

    inline const_interior_iterator interior_begin() const {
      if (memType_ != LOCAL_RAM) {
        throw;
      }
      return const_interior_iterator(fieldValues_,
          interiorFieldWindow_.flat_index(IntVec(0, 0, 0)),
          interiorFieldWindow_);
    }
    inline interior_iterator interior_begin() {
      if (memType_ != LOCAL_RAM) {
        throw;
      }
      return interior_iterator(fieldValues_,
          interiorFieldWindow_.flat_index(IntVec(0, 0, 0)),
          interiorFieldWindow_);
    }

    inline const_interior_iterator interior_end() const;
    inline interior_iterator interior_end();

    inline MyType& operator =(const MyType&);
    inline MyType& operator+=(const MyType&);
    inline MyType& operator-=(const MyType&);
    inline MyType& operator*=(const MyType&);
    inline MyType& operator/=(const MyType&);

    inline MyType& operator =(const T);
    inline MyType& operator+=(const T);
    inline MyType& operator-=(const T);
    inline MyType& operator*=(const T);
    inline MyType& operator/=(const T);

    inline SpatialField& operator=(const RHS&); ///< Assign a RHS to this field (doesn't affect ghosts)
    inline SpatialField& operator+=(const RHS&); ///< Add a RHS to this field (doesn't affect ghosts)
    inline SpatialField& operator-=(const RHS&); ///< Subtract a RHS from this field (doesn't affect ghosts)

    bool operator!=(const MyType&) const;
    bool operator==(const MyType&) const;

    const MemoryWindow& window_without_ghost() const {
      if (memType_ != LOCAL_RAM) {
        throw;
      }
      return interiorFieldWindow_;
    }
    const MemoryWindow& window_with_ghost() const {
      if (memType_ != LOCAL_RAM) {
        throw;
      }
      return fieldWindow_;
    }

    MemoryType memory_device_type() const {
      return memType_;
    }
    unsigned short int device_index() const {
      return deviceIndex_;
    }
};

//==================================================================
//
//                          Implementation
//
//==================================================================

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>::SpatialField(const MemoryWindow window,
    T* const fieldValues, const StorageMode mode, const MemoryType mtype,
    const unsigned short int devIdx) :
    fieldWindow_(window), interiorFieldWindow_(window), // reset with correct info later
    fieldValues_(
        (mtype == LOCAL_RAM) ?
            ((mode == ExternalStorage) ?
                fieldValues :
                new T[window.glob_dim(0) * window.glob_dim(1)
                    * window.glob_dim(2)]) :
            (NULL)), builtField_(mode == InternalStorage), deviceIndex_(devIdx), memType_(
        mtype), allocatedBytes_(0) {
  //InteriorStorage => we build a new field
  //Exterior storage => we wrap T*
  IntVec ext = window.extent();
  IntVec ofs = window.offset();

  for (size_t i = 0; i < 3; ++i) {
    if (ext[i] > 1) {
      ext[i] -= 2 * GhostTraits::NGHOST;
      ofs[i] += GhostTraits::NGHOST;
    }
  }
  interiorFieldWindow_ = MemoryWindow(window.glob_dim(), ofs, ext,
      window.has_bc(0), window.has_bc(1), window.has_bc(2));

  switch (mtype) {
    case LOCAL_RAM: {

    }
      break;
#ifdef ENABLE_CUDA
      case EXTERNAL_CUDA_GPU: {
        allocatedBytes_ = sizeof(T)*(window.glob_dim(0) * window.glob_dim(1)
            * window.glob_dim(2));
        ema::cuda::CUDADeviceInterface& CDI = ema::cuda::CUDADeviceInterface::self();
        fieldValues_ = NULL;
        fieldValuesExtDevice_ = CDI.get_raw_pointer( allocatedBytes_, deviceIndex_ );
        break;
      }
#endif
    default: {
      std::ostringstream msg;
      msg << "Attempt to create ( "
          << DeviceTypeTools::get_memory_type_description(memType_)
          << " ) field type\n";
      msg << "\t " << __FILE__ << " : " << __LINE__;
      throw(std::runtime_error(msg.str()));
    }
      break;
  }

  if (mode == InternalStorage)
    reset_values(fieldValues);
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>::SpatialField(const SpatialField& other) :
    fieldWindow_(other.fieldWindow_), interiorFieldWindow_(
        other.interiorFieldWindow_), fieldValues_(other.fieldValues_), builtField_(
        false), deviceIndex_(other.deviceIndex_), memType_(other.memType_), fieldValuesExtDevice_(
        other.fieldValuesExtDevice_) {
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>::~SpatialField() {
  if (builtField_) {
    switch (memType_) {
      case LOCAL_RAM: {
        delete[] fieldValues_;
      }
        break;
#ifdef ENABLE_CUDA
        case EXTERNAL_CUDA_GPU: {
          ema::cuda::CUDADeviceInterface::self().release( fieldValuesExtDevice_, deviceIndex_);
        }
        break;
#endif
      default:
        std::ostringstream msg;
        msg << "Attempt to release ( "
            << DeviceTypeTools::get_memory_type_description(memType_)
            << " ) field without supporting libraries\n";
        msg << "\t " << __FILE__ << " : " << __LINE__;
    }
  }
}

//------------------------------------------------------------------

template<typename FieldLocation, typename GhostTraits, typename T>
void SpatialField<FieldLocation, GhostTraits, T>::reset_values(
    const T* values) {
  switch (memType_) {
    case LOCAL_RAM: {
      iterator ifld = begin();
      const iterator iflde = end();
      if (NULL == values) {
        for (; ifld != iflde; ++ifld)
          *ifld = 0.0;
      } else {
        for (; ifld != iflde; ++ifld, ++values)
          *ifld = *values;
      }
    }
      break;
#ifdef ENABLE_CUDA
      case EXTERNAL_CUDA_GPU: {
        ema::cuda::CUDADeviceInterface& CDI = ema::cuda::CUDADeviceInterface::self();

        if( values == NULL ) {
          CDI.memset( fieldValuesExtDevice_, 0, allocatedBytes_, deviceIndex_ );
        } else {
          void* src = (void*)(values);
          CDI.memcpy_to( fieldValuesExtDevice_, src, allocatedBytes_, deviceIndex_ );
        }
      }
      break;
#endif
    default:
      std::ostringstream msg;
      msg << "reset_values called for unsupported field type ( "
          << DeviceTypeTools::get_memory_type_description(memType_) << " )";
      msg << "\t " << __FILE__ << " : " << __LINE__;
      throw(std::runtime_error(msg.str()));
      break;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
typename SpatialField<Location, GhostTraits, T>::const_iterator SpatialField<
    Location, GhostTraits, T>::end() const {
  switch (memType_) {
    case LOCAL_RAM: {
      IntVec ijk = fieldWindow_.extent();
      for (size_t i = 0; i < 3; ++i)
        ijk[i] -= 1;
      const size_t n = fieldWindow_.flat_index(ijk);
      const_iterator i(fieldValues_, n, fieldWindow_);
      return ++i;
    }
      break;
    default:
      std::ostringstream msg;
      msg << "Unsupported request for iterator to external field type ( "
          << DeviceTypeTools::get_memory_type_description(memType_) << " )";
      msg << "\t " << __FILE__ << " : " << __LINE__;
      throw(std::runtime_error(msg.str()));
      break;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
typename SpatialField<Location, GhostTraits, T>::iterator SpatialField<Location,
    GhostTraits, T>::end() {
  switch (memType_) {
    case LOCAL_RAM: {
      IntVec ijk = fieldWindow_.extent();
      for (size_t i = 0; i < 3; ++i)
        ijk[i] -= 1;
      const size_t n = fieldWindow_.flat_index(ijk);
      iterator i(fieldValues_, n, fieldWindow_);
      return ++i;
      break;
    }
    default:
      std::ostringstream msg;
      msg << "Unsupported request for iterator to external field type ( "
          << DeviceTypeTools::get_memory_type_description(memType_) << " )";
      msg << "\t " << __FILE__ << " : " << __LINE__;
      throw(std::runtime_error(msg.str()));
      break;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
typename SpatialField<Location, GhostTraits, T>::const_interior_iterator SpatialField<
    Location, GhostTraits, T>::interior_end() const {
  switch (memType_) {
    case LOCAL_RAM: {
      IntVec ijk = interiorFieldWindow_.extent();
      for (size_t i = 0; i < 3; ++i)
        ijk[i] -= 1;
      const_interior_iterator i(fieldValues_,
          interiorFieldWindow_.flat_index(ijk), interiorFieldWindow_);
      return ++i;
    }
      break;
    default:
      std::ostringstream msg;
      msg << "Unsupported request for iterator to external field type ( "
          << DeviceTypeTools::get_memory_type_description(memType_) << " )";
      msg << "\t " << __FILE__ << " : " << __LINE__;
      throw(std::runtime_error(msg.str()));
      break;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
typename SpatialField<Location, GhostTraits, T>::interior_iterator SpatialField<
    Location, GhostTraits, T>::interior_end() {
  switch (memType_) {
    case LOCAL_RAM: {
      IntVec ijk = interiorFieldWindow_.extent();
      for (size_t i = 0; i < 3; ++i)
        ijk[i] -= 1;
      interior_iterator i(fieldValues_, interiorFieldWindow_.flat_index(ijk),
          interiorFieldWindow_);
      return ++i;
    }
      break;
    default:
      std::ostringstream msg;
      msg << "Unsupported request for iterator to external field type ( "
          << DeviceTypeTools::get_memory_type_description(memType_) << " )";
      msg << "\t " << __FILE__ << " : " << __LINE__;
      throw(std::runtime_error(msg.str()));
      break;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
T&
SpatialField<Location, GhostTraits, T>::operator()(const size_t i,
    const size_t j, const size_t k) {
  switch (memType_) {
    case LOCAL_RAM: {
      return (*this)(IntVec(i, j, k));
    }
      break;
    default:
      throw;
      break;
  }
}

template<typename Location, typename GhostTraits, typename T>
T&
SpatialField<Location, GhostTraits, T>::operator()(const IntVec& ijk) {
  switch (memType_) {
    case LOCAL_RAM: {
#   ifndef NDEBUG
      assert(ijk[0] < fieldWindow_.extent(0));
      assert(ijk[1] < fieldWindow_.extent(1));
      assert(ijk[2] < fieldWindow_.extent(2));
      assert(ijk[0] >= fieldWindow_.offset(0));
      assert(ijk[1] >= fieldWindow_.offset(1));
      assert(ijk[2] >= fieldWindow_.offset(2));
#   endif
      return fieldValues_[fieldWindow_.flat_index(ijk)];
    }
      break;
    default:
      throw;
      break;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
const T&
SpatialField<Location, GhostTraits, T>::operator()(const size_t i,
    const size_t j, const size_t k) const {
  switch (memType_) {
    case LOCAL_RAM: {
      return (*this)(IntVec(i, j, k));
    }
      break;
    default:
      throw;
      break;
  }
}

template<typename Location, typename GhostTraits, typename T>
const T&
SpatialField<Location, GhostTraits, T>::operator()(const IntVec& ijk) const {
  switch (memType_) {
    case LOCAL_RAM: {
#   ifndef NDEBUG
      assert(
          ijk[0] < fieldWindow_.extent(0) && ijk[0] >= fieldWindow_.offset(0));
      assert(
          ijk[1] < fieldWindow_.extent(1) && ijk[1] >= fieldWindow_.offset(1));
      assert(
          ijk[2] < fieldWindow_.extent(2) && ijk[2] >= fieldWindow_.offset(2));
#   endif
      return fieldValues_[fieldWindow_.flat_index(ijk)];
    }
      break;
    default:
      throw;
      break;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
T&
SpatialField<Location, GhostTraits, T>::operator[](const size_t i) {
  switch (memType_) {
    case LOCAL_RAM: {
      return fieldValues_[i];
    }
      break;
    default:
      throw;
      break;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
T&
SpatialField<Location, GhostTraits, T>::operator[](const size_t i) const {
  switch (memType_) {
    case LOCAL_RAM: {
      return fieldValues_[i];
    }
      break;
    default:
      throw;
      break;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>&
SpatialField<Location, GhostTraits, T>::operator=(const MyType& other) {
  switch (memType_) {
    case LOCAL_RAM: {
      const_iterator iother = other.begin();
      const iterator iend = this->end();
      for (iterator ifld = this->begin(); ifld != iend; ++ifld, ++iother) {
        *ifld = *iother;
      }
      return *this;
    }
      break;
    default:
      throw;
      break;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>&
SpatialField<Location, GhostTraits, T>::operator+=(const MyType& other) {
  if (memType_ == LOCAL_RAM) {
    const_iterator iother = other.begin();
    const iterator iend = this->end();
    for (iterator ifld = this->begin(); ifld != iend; ++ifld, ++iother) {
      *ifld += *iother;
    }
    return *this;
  } else {
    throw;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>&
SpatialField<Location, GhostTraits, T>::operator-=(const MyType& other) {
  if (memType_ == LOCAL_RAM) {
    const_iterator iother = other.begin();
    const iterator iend = this->end();
    for (iterator ifld = this->begin(); ifld != iend; ++ifld, ++iother) {
      *ifld -= *iother;
    }
    return *this;
  } else {
    throw;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>&
SpatialField<Location, GhostTraits, T>::operator*=(const MyType& other) {
  if (memType_ == LOCAL_RAM) {
    const_iterator iother = other.begin();
    const iterator iend = this->end();
    for (iterator ifld = this->begin(); ifld != iend; ++ifld, ++iother) {
      *ifld *= *iother;
    }
    return *this;
  } else {
    throw;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>&
SpatialField<Location, GhostTraits, T>::operator/=(const MyType& other) {
  if (memType_ == LOCAL_RAM) {
    const_iterator iother = other.begin();
    const iterator iend = this->end();
    for (iterator ifld = this->begin(); ifld != iend; ++ifld, ++iother) {
      *ifld /= *iother;
    }
    return *this;
  } else {
    throw;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
bool SpatialField<Location, GhostTraits, T>::operator!=(
    const MyType& other) const {
  return !(*this == other);
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
bool SpatialField<Location, GhostTraits, T>::operator==(
    const MyType& other) const {
  switch (memType_) {
    case LOCAL_RAM: {
      const_iterator iother = other.begin();
      const_iterator iend = this->end();
      for (const_iterator ifld = this->begin(); ifld != iend;
          ++ifld, ++iother) {
        if (*ifld != *iother)
          return false;
      }
      return true;
    }
      break;
    default:
      throw;
      break;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>&
SpatialField<Location, GhostTraits, T>::operator=(const T a) {
  switch (memType_) {
    case LOCAL_RAM: {
      const iterator iend = this->end();
      for (iterator ifld = this->begin(); ifld != iend; ++ifld)
        *ifld = a;
      return *this;
    }
      break;
    default:
      throw;
      break;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>&
SpatialField<Location, GhostTraits, T>::operator+=(const T a) {
  if (memType_ == LOCAL_RAM) {
    const iterator iend = this->end();
    for (iterator ifld = this->begin(); ifld != iend; ++ifld)
      *ifld += a;
    return *this;
  } else {
    throw;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>&
SpatialField<Location, GhostTraits, T>::operator-=(const T a) {
  if (memType_ == LOCAL_RAM) {
    const iterator iend = this->end();
    for (iterator ifld = this->begin(); ifld != iend; ++ifld)
      *ifld -= a;
    return *this;
  } else {
    throw;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>&
SpatialField<Location, GhostTraits, T>::operator*=(const T a) {
  if (memType_ == LOCAL_RAM) {
    const iterator iend = this->end();
    for (iterator ifld = this->begin(); ifld != iend; ++ifld)
      *ifld *= a;
    return *this;
  } else {
    throw;
  }
}

//------------------------------------------------------------------

template<typename Location, typename GhostTraits, typename T>
SpatialField<Location, GhostTraits, T>&
SpatialField<Location, GhostTraits, T>::operator/=(const T a) {
  if (memType_ == LOCAL_RAM) {
    const iterator iend = this->end();
    for (iterator ifld = this->begin(); ifld != iend; ++ifld)
      *ifld /= a;
    return *this;
  } else {
    throw;
  }
}

//------------------------------------------------------------------

}// namespace structured
} // namespace SpatialOps

#endif // SpatialOps_SpatialField_h
