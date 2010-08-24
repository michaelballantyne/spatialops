#ifndef FVStaggeredIndexHelper_h
#define FVStaggeredIndexHelper_h

#include <spatialops/SpatialOpsConfigure.h>

#include <spatialops/structured/FVTools.h>
#include <spatialops/structured/FVStaggeredTypes.h>


namespace SpatialOps{
namespace structured{


  //==================================================================

  /**
   *  @class IndexHelper
   *  @author James C. Sutherland
   *
   *  @todo Check on all implementations of shift_dest_index for consistency w.r.t. ghosting.
   *  @todo Document all of this so it is comprehensible.
   */
  template< typename SrcFieldT, typename DestFieldT >
  class IndexHelper
  {
  public:

    /**
     *  @param dim A vector with three elements indicating the domain
     *  extent (number of cells) in each coordinate direction.
     *
     *  @param hasPlusXSideFaces Boolean flag to indicate if the
     *  operator is to be constructed including face cells on the +X
     *  side of the domain.
     *
     *  @param hasPlusYSideFaces Boolean flag to indicate if the
     *  operator is to be constructed including face cells on the +Y
     *  side of the domain.
     *
     *  @param hasPlusZSideFaces Boolean flag to indicate if the
     *  operator is to be constructed including face cells on the +Z
     *  side of the domain.
     */
    IndexHelper( const std::vector<int>& dim,
                 const bool hasPlusXSideFaces,
                 const bool hasPlusYSideFaces,
                 const bool hasPlusZSideFaces );
    void get_cols( const int irow, std::vector<int>& cols ) const;
    int get_ncol() const;
    int get_nrow() const;

    /**
     *  @brief Shift the index for the destination field to include ghost cells.
     *
     *  @param dim A vector with three elements indicating the domain
     *  extent (number of cells) in each coordinate direction.
     *
     *  @param triplet The ijk location. On entry, this contains the
     *  interior location (0-based at interior). On exit, it is 0-based
     *  at the first ghost point.
     */
    static void shift_dest_index( const std::vector<int>& dim,
                                  IndexTriplet& triplet );

  private:
    const std::vector<int>& dim_;
    const bool hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_;


    int calculate_stride() const;

    template<typename T1, typename T2, int IsSurf> struct DirSelector{};
    template<typename T1, typename T2> struct DirSelector<T1,T2,1>{ typedef T1 Type; };
    template<typename T1, typename T2> struct DirSelector<T1,T2,0>{ typedef T2 Type; };
  };

  //==================================================================

  inline bool is_valid_entry( const std::vector<int>& dim,
                              const IndexTriplet& ixt1,
                              const IndexTriplet& ixt2 );

  //==================================================================

  /**
   *  @brief Determines if the row and column correspond to interior
   *  points for the given source/destination fields.
   *
   *  @param ixs Flat index for the source field
   *
   *  @param ixd Flat index for the destination field.
   *
   *  @param dim A vector with three elements indicating the domain
   *  extent (number of cells) in each coordinate direction.
   *
   *  @param hasPlusXSideFaces Boolean flag to indicate if the
   *  operator is to be constructed including face cells on the +X
   *  side of the domain.
   *
   *  @param hasPlusYSideFaces Boolean flag to indicate if the
   *  operator is to be constructed including face cells on the +Y
   *  side of the domain.
   *
   *  @param hasPlusZSideFaces Boolean flag to indicate if the
   *  operator is to be constructed including face cells on the +Z
   *  side of the domain.
   */
  template< typename SrcFieldT,
            typename DestFieldT >
  bool is_in_bounds( const std::vector<int>& dim,
                     const int ixs,
                     const int ixd,
                     const bool hasPlusXSideFaces,
                     const bool hasPlusYSideFaces,
                     const bool hasPlusZSideFaces );

  //==================================================================






  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //
  //                          Implementation
  //
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%






  //==================================================================



  template<typename SrcFieldT, typename DestFieldT>
  void
  IndexHelper<SrcFieldT,DestFieldT>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet ){}

  template<>
  inline void
  IndexHelper<SSurfXField,SVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SSurfXField::Ghost::NGHOST-SVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (SSurfXField::Ghost::NGHOST-SVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (SSurfXField::Ghost::NGHOST-SVolField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<SSurfYField,SVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SSurfYField::Ghost::NGHOST-SVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (SSurfYField::Ghost::NGHOST-SVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (SSurfYField::Ghost::NGHOST-SVolField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<SSurfZField,SVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SSurfZField::Ghost::NGHOST-SVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (SSurfZField::Ghost::NGHOST-SVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (SSurfZField::Ghost::NGHOST-SVolField::Ghost::NGHOST);
  }

  template<>
  inline void
  IndexHelper<XSurfXField,XVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (XSurfXField::Ghost::NGHOST-XVolField::Ghost::NGHOST)-1;
    if( dim[1]>1 ) triplet.j += (XSurfXField::Ghost::NGHOST-XVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (XSurfXField::Ghost::NGHOST-XVolField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<XSurfYField,XVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (XSurfYField::Ghost::NGHOST-XVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (XSurfYField::Ghost::NGHOST-XVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (XSurfYField::Ghost::NGHOST-XVolField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<XSurfZField,XVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (XSurfZField::Ghost::NGHOST-XVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (XSurfZField::Ghost::NGHOST-XVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (XSurfZField::Ghost::NGHOST-XVolField::Ghost::NGHOST);
  }

  template<>
  inline void
  IndexHelper<YSurfXField,YVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (YSurfXField::Ghost::NGHOST-YVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (YSurfXField::Ghost::NGHOST-YVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (YSurfXField::Ghost::NGHOST-YVolField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<YSurfYField,YVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (YSurfYField::Ghost::NGHOST-YVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (YSurfYField::Ghost::NGHOST-YVolField::Ghost::NGHOST)-1;
    if( dim[2]>1 ) triplet.k += (YSurfYField::Ghost::NGHOST-YVolField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<YSurfZField,YVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (YSurfZField::Ghost::NGHOST-YVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (YSurfZField::Ghost::NGHOST-YVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (YSurfZField::Ghost::NGHOST-YVolField::Ghost::NGHOST);
  }

  template<>
  inline void
  IndexHelper<ZSurfXField,ZVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (ZSurfXField::Ghost::NGHOST-ZVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (ZSurfXField::Ghost::NGHOST-ZVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (ZSurfXField::Ghost::NGHOST-ZVolField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<ZSurfYField,ZVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (ZSurfYField::Ghost::NGHOST-ZVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (ZSurfYField::Ghost::NGHOST-ZVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (ZSurfYField::Ghost::NGHOST-ZVolField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<ZSurfZField,ZVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (ZSurfZField::Ghost::NGHOST-ZVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (ZSurfZField::Ghost::NGHOST-ZVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (ZSurfZField::Ghost::NGHOST-ZVolField::Ghost::NGHOST)-1;
  }

  template<>
  inline void
  IndexHelper<SVolField,SSurfXField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - SSurfXField::Ghost::NGHOST)-1;
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - SSurfXField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - SSurfXField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<SVolField,SSurfYField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - SSurfYField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - SSurfYField::Ghost::NGHOST)-1;
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - SSurfYField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<SVolField,SSurfZField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - SSurfZField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - SSurfZField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - SSurfZField::Ghost::NGHOST)-1;
  }

  template<>
  inline void
  IndexHelper<SVolField,XVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - XVolField::Ghost::NGHOST)-1;
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - XVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - XVolField::Ghost::NGHOST);
  }

  template<>
  inline void
  IndexHelper<SVolField,YVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - YVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - YVolField::Ghost::NGHOST)-1;
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - YVolField::Ghost::NGHOST);
  }

  template<>
  inline void
  IndexHelper<SVolField,ZVolField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - ZVolField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - ZVolField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - ZVolField::Ghost::NGHOST)-1;
  }

  template<>
  inline void
  IndexHelper<SVolField,XSurfXField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - XSurfXField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - XSurfXField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - XSurfXField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<SVolField,XSurfYField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - XSurfYField::Ghost::NGHOST)-1;
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - XSurfYField::Ghost::NGHOST)-1;
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - XSurfYField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<SVolField,XSurfZField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - XSurfZField::Ghost::NGHOST)-1;
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - XSurfZField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - XSurfZField::Ghost::NGHOST)-1;
  }

  template<>
  inline void
  IndexHelper<SVolField,YSurfXField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - YSurfXField::Ghost::NGHOST)-1;
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - YSurfXField::Ghost::NGHOST)-1;
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - YSurfXField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<SVolField,YSurfYField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - YSurfYField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - YSurfYField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - YSurfYField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<SVolField,YSurfZField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - YSurfZField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - YSurfZField::Ghost::NGHOST)-1;
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - YSurfZField::Ghost::NGHOST)-1;
  }


  template<>
  inline void
  IndexHelper<SVolField,ZSurfXField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - ZSurfXField::Ghost::NGHOST)-1;
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - ZSurfXField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - ZSurfXField::Ghost::NGHOST)-1;
  }
  template<>
  inline void
  IndexHelper<SVolField,ZSurfYField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - ZSurfYField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - ZSurfYField::Ghost::NGHOST)-1;
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - ZSurfYField::Ghost::NGHOST)-1;
  }
  template<>
  inline void
  IndexHelper<SVolField,ZSurfZField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (SVolField::Ghost::NGHOST - ZSurfZField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (SVolField::Ghost::NGHOST - ZSurfZField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (SVolField::Ghost::NGHOST - ZSurfZField::Ghost::NGHOST);
  }


  template<>
  inline void
  IndexHelper<XVolField,XSurfXField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (XVolField::Ghost::NGHOST - XSurfXField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (XVolField::Ghost::NGHOST - XSurfXField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (XVolField::Ghost::NGHOST - XSurfXField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<XVolField,XSurfYField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (XVolField::Ghost::NGHOST - XSurfYField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (XVolField::Ghost::NGHOST - XSurfYField::Ghost::NGHOST)-1;
    if( dim[2]>1 ) triplet.k += (XVolField::Ghost::NGHOST - XSurfYField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<XVolField,XSurfZField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (XVolField::Ghost::NGHOST - XSurfZField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (XVolField::Ghost::NGHOST - XSurfZField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (XVolField::Ghost::NGHOST - XSurfZField::Ghost::NGHOST)-1;
  }
  template<>
  inline void
  IndexHelper<XVolField,YSurfXField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (XVolField::Ghost::NGHOST - YSurfXField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (XVolField::Ghost::NGHOST - YSurfXField::Ghost::NGHOST)-1;
    if( dim[2]>1 ) triplet.k += (XVolField::Ghost::NGHOST - YSurfXField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<XVolField,ZSurfXField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (XVolField::Ghost::NGHOST - ZSurfXField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (XVolField::Ghost::NGHOST - ZSurfXField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (XVolField::Ghost::NGHOST - ZSurfXField::Ghost::NGHOST)-1;
  }

  template<>
  inline void
  IndexHelper<YVolField,YSurfXField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (YVolField::Ghost::NGHOST - YSurfXField::Ghost::NGHOST)-1;
    if( dim[1]>1 ) triplet.j += (YVolField::Ghost::NGHOST - YSurfXField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (YVolField::Ghost::NGHOST - YSurfXField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<YVolField,YSurfYField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (YVolField::Ghost::NGHOST - YSurfYField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (YVolField::Ghost::NGHOST - YSurfYField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (YVolField::Ghost::NGHOST - YSurfYField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<YVolField,YSurfZField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (YVolField::Ghost::NGHOST - YSurfZField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (YVolField::Ghost::NGHOST - YSurfZField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (YVolField::Ghost::NGHOST - YSurfZField::Ghost::NGHOST)-1;
  }
  template<>
  inline void
  IndexHelper<YVolField,XSurfYField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (YVolField::Ghost::NGHOST - XSurfYField::Ghost::NGHOST)-1;
    if( dim[1]>1 ) triplet.j += (YVolField::Ghost::NGHOST - XSurfYField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (YVolField::Ghost::NGHOST - XSurfYField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<YVolField,ZSurfYField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (YVolField::Ghost::NGHOST - ZSurfYField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (YVolField::Ghost::NGHOST - ZSurfYField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (YVolField::Ghost::NGHOST - ZSurfYField::Ghost::NGHOST)-1;
  }

  template<>
  inline void
  IndexHelper<ZVolField,ZSurfXField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (ZVolField::Ghost::NGHOST - ZSurfXField::Ghost::NGHOST)-1;
    if( dim[1]>1 ) triplet.j += (ZVolField::Ghost::NGHOST - ZSurfXField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (ZVolField::Ghost::NGHOST - ZSurfXField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<ZVolField,ZSurfYField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (ZVolField::Ghost::NGHOST - ZSurfYField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (ZVolField::Ghost::NGHOST - ZSurfYField::Ghost::NGHOST)-1;
    if( dim[2]>1 ) triplet.k += (ZVolField::Ghost::NGHOST - ZSurfYField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<ZVolField,ZSurfZField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (ZVolField::Ghost::NGHOST - ZSurfZField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (ZVolField::Ghost::NGHOST - ZSurfZField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (ZVolField::Ghost::NGHOST - ZSurfZField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<ZVolField,XSurfZField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (ZVolField::Ghost::NGHOST - XSurfZField::Ghost::NGHOST)-1;
    if( dim[1]>1 ) triplet.j += (ZVolField::Ghost::NGHOST - XSurfZField::Ghost::NGHOST);
    if( dim[2]>1 ) triplet.k += (ZVolField::Ghost::NGHOST - XSurfZField::Ghost::NGHOST);
  }
  template<>
  inline void
  IndexHelper<ZVolField,YSurfZField>::
  shift_dest_index( const std::vector<int>& dim, IndexTriplet& triplet )
  {
    if( dim[0]>1 ) triplet.i += (ZVolField::Ghost::NGHOST - YSurfZField::Ghost::NGHOST);
    if( dim[1]>1 ) triplet.j += (ZVolField::Ghost::NGHOST - YSurfZField::Ghost::NGHOST)-1;
    if( dim[2]>1 ) triplet.k += (ZVolField::Ghost::NGHOST - YSurfZField::Ghost::NGHOST);
  }

  //------------------------------------------------------------------


  //==================================================================


  //------------------------------------------------------------------ 

  template<typename SrcFieldT, typename DestFieldT>
  IndexHelper<SrcFieldT,DestFieldT>::
  IndexHelper( const std::vector<int>& dim,
               const bool hasPlusXSideFaces,
               const bool hasPlusYSideFaces,
               const bool hasPlusZSideFaces )
    : dim_( dim ),
      hasPlusXSideFaces_( hasPlusXSideFaces ),
      hasPlusYSideFaces_( hasPlusYSideFaces ),
      hasPlusZSideFaces_( hasPlusZSideFaces )
  {
  }
  //------------------------------------------------------------------ 
  template<typename SrcFieldT, typename DestFieldT>
  void
  IndexHelper<SrcFieldT,DestFieldT>::
  get_cols( const int irow, std::vector<int>& cols ) const
  {
    // from the row (flat index of destination field), determine the
    // ijk indices for the destination field
    IndexTriplet ijk = flat2ijk<DestFieldT>::value( dim_, irow, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );

    shift_dest_index( dim_, ijk );

    if( ijk.i<0 || ijk.j<0 || ijk.k<0 ) return;

    // from the ijk index, determine the flat index for the source field.
    const int icol = ijk2flat<SrcFieldT>::value( dim_, ijk, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );

    const int stride = calculate_stride();

    if( is_in_bounds<SrcFieldT,DestFieldT>(dim_,icol,       irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SrcFieldT,DestFieldT>(dim_,icol+stride,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) ){

      const IndexTriplet ijk2 = flat2ijk<SrcFieldT>::value( dim_, icol+stride, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );

      if( is_valid_entry( dim_, ijk, ijk2 ) ){
        cols.push_back( icol        );
        cols.push_back( icol+stride );
      }

    }
  }
  //------------------------------------------------------------------
  template<typename SrcFieldT, typename DestFieldT>
  int
  IndexHelper<SrcFieldT,DestFieldT>::
  get_ncol() const
  {
    return get_n_tot<SrcFieldT>(dim_,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_);
  }
  //------------------------------------------------------------------
  template<typename SrcFieldT, typename DestFieldT>
  int
  IndexHelper<SrcFieldT,DestFieldT>::
  get_nrow() const
  {
    return get_n_tot<DestFieldT>(dim_,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_);
  }
  //------------------------------------------------------------------
  template<typename SrcFieldT, typename DestFieldT>
  int
  IndexHelper<SrcFieldT,DestFieldT>::
  calculate_stride() const
  {
    // jcs this is not robust at all...
    typedef typename DirSelector< SrcFieldT, DestFieldT, IsSameType<typename DestFieldT::Location::FaceDir,NODIR>::result >::Type FieldDirSelect;

    int n=-1;
    switch( FieldDirSelect::Location::FaceDir::value ){
    case XDIR::value:
      n=1;
      break;
    case YDIR::value:
      n = get_nx<SrcFieldT>(dim_,hasPlusXSideFaces_);
      break;
    case ZDIR::value:
      n = get_nx<SrcFieldT>(dim_,hasPlusXSideFaces_) * get_ny<SrcFieldT>(dim_,hasPlusYSideFaces_);
      break;
    default:
      std::cout << "ERROR: field location enum value=" << FieldDirSelect::Location::FaceDir::value << std::endl;
      assert(0);
    }
    return n;
  }
  template<> inline int IndexHelper<SVolField,XVolField>::calculate_stride() const
  {
    return 1;
  }
  template<> inline int IndexHelper<SVolField,YVolField>::calculate_stride() const
  {
    return get_nx<SVolField>(dim_,hasPlusXSideFaces_);
  }
  template<> inline int IndexHelper<SVolField,ZVolField>::calculate_stride() const
  {
    return get_nx<SVolField>(dim_,hasPlusXSideFaces_) * get_ny<SVolField>(dim_,hasPlusYSideFaces_);
  }
  template<> inline int IndexHelper<XVolField,YSurfXField>::calculate_stride() const
  {
    return get_nx<XVolField>(dim_,hasPlusXSideFaces_);
  }
  template<> inline int IndexHelper<XVolField,ZSurfXField>::calculate_stride() const
  {
    return get_nx<XVolField>(dim_,hasPlusXSideFaces_) * get_ny<XVolField>(dim_,hasPlusYSideFaces_);
  }
  template<> inline int IndexHelper<YVolField,XSurfYField>::calculate_stride() const
  {
    return 1;
  }
  template<> inline int IndexHelper<YVolField,ZSurfYField>::calculate_stride() const
  {
    return get_nx<YVolField>(dim_,hasPlusXSideFaces_) * get_ny<YVolField>(dim_,hasPlusYSideFaces_);
  }
  template<> inline int IndexHelper<ZVolField,XSurfZField>::calculate_stride() const
  {
    return 1;
  }
  template<> inline int IndexHelper<ZVolField,YSurfZField>::calculate_stride() const
  {
    return get_nx<ZVolField>(dim_,hasPlusXSideFaces_);
  }
  template<> inline int IndexHelper<XVolField,SVolField>::calculate_stride() const
  {
    return 1;
  }
  template<> inline int IndexHelper<YVolField,SVolField>::calculate_stride() const
  {
    return get_nx<YVolField>(dim_,hasPlusXSideFaces_);
  }
  template<> inline int IndexHelper<ZVolField,SVolField>::calculate_stride() const
  {
    return get_nx<ZVolField>(dim_,hasPlusXSideFaces_) * get_ny<ZVolField>(dim_,hasPlusYSideFaces_);
  }
  //------------------------------------------------------------------

  //==================================================================

  bool is_valid_entry( const std::vector<int>& dim,
                       const IndexTriplet& ixt1,
                       const IndexTriplet& ixt2 )
  {
    if( std::abs(ixt2.i - ixt1.i) > 1 ) return false;
    if( std::abs(ixt2.j - ixt1.j) > 1 ) return false;
    if( std::abs(ixt2.k - ixt1.k) > 1 ) return false;
    return true;
  }
  
  //==================================================================

  template<typename SrcFieldT, typename DestFieldT>
  bool is_in_bounds( const std::vector<int>& dim, const int ixs, const int ixd,
                     const bool hasPlusXSideFaces, const bool hasPlusYSideFaces, const bool hasPlusZSideFaces )
  {
    const IndexTriplet it = flat2ijk<SrcFieldT>::value(dim,ixs,hasPlusXSideFaces,hasPlusYSideFaces,hasPlusZSideFaces);

    if( it.i<0 || it.j<0 || it.k<0 ) return false;

    const int nx = get_nx<SrcFieldT>(dim,hasPlusXSideFaces);
    const int ny = get_ny<SrcFieldT>(dim,hasPlusYSideFaces);
    const int nz = get_nz<SrcFieldT>(dim,hasPlusZSideFaces);

    if( it.i<nx && it.j<ny && it.k<nz )  return true;

    return false;
  }

  //==================================================================


  //------------------------------------------------------------------

  template<> inline void
  IndexHelper<SVolField,XSurfXField>::
  get_cols( const int irow, std::vector<int>& cols ) const
  {
    IndexTriplet ijk = flat2ijk<XSurfXField>::value( dim_, irow, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    this->shift_dest_index( dim_, ijk );
    if( ijk.i<0 || ijk.j<0 || ijk.k<0 ) return;
    const int icol = ijk2flat<SVolField>::value( dim_, ijk, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    if( is_in_bounds<SVolField,XSurfXField>(dim_,icol,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) ){
      cols.push_back( icol );
    }
  }

  template<> inline void
  IndexHelper<SVolField,XSurfYField>::
  get_cols( const int irow, std::vector<int>& cols ) const
  {
    IndexTriplet ijk = flat2ijk<XSurfYField>::value( dim_, irow, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    shift_dest_index( dim_, ijk );
    if( ijk.i<0 || ijk.j<0 || ijk.k<0 ) return;

    const int icol = ijk2flat<SVolField>::value( dim_, ijk, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    const int icol2 = icol + 1;
    const int icol3 = icol + get_nx<SVolField>(dim_,hasPlusXSideFaces_);
    const int icol4 = icol3 + 1;

    if( is_in_bounds<SVolField,XSurfYField>(dim_,icol ,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,XSurfYField>(dim_,icol2,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,XSurfYField>(dim_,icol3,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,XSurfYField>(dim_,icol4,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) ){
      cols.push_back( icol  );
      cols.push_back( icol2 );
      cols.push_back( icol3 );
      cols.push_back( icol4 );
    }
  }

  template<> inline void
  IndexHelper<SVolField,XSurfZField>::
  get_cols( const int irow, std::vector<int>& cols ) const
  {
    IndexTriplet ijk = flat2ijk<XSurfZField>::value( dim_, irow, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    shift_dest_index( dim_, ijk );
    if( ijk.i<0 || ijk.j<0 || ijk.k<0 ) return;

    const int icol = ijk2flat<SVolField>::value( dim_, ijk, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    const int icol2 = icol + 1;
    const int icol3 = icol + get_nx<SVolField>(dim_,hasPlusXSideFaces_)*get_ny<SVolField>(dim_,hasPlusYSideFaces_);
    const int icol4 = icol3 + 1;

    if( is_in_bounds<SVolField,XSurfZField>(dim_,icol ,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,XSurfZField>(dim_,icol2,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,XSurfZField>(dim_,icol3,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,XSurfZField>(dim_,icol4,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) ){
      cols.push_back( icol  );
      cols.push_back( icol2 );
      cols.push_back( icol3 );
      cols.push_back( icol4 );
    }
  }


  template<> inline void
  IndexHelper<SVolField,YSurfXField>::
  get_cols( const int irow, std::vector<int>& cols ) const
  {
    IndexTriplet ijk = flat2ijk<YSurfXField>::value( dim_, irow, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    shift_dest_index( dim_, ijk );
    if( ijk.i<0 || ijk.j<0 || ijk.k<0 ) return;

    const int icol = ijk2flat<SVolField>::value( dim_, ijk, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    const int icol2 = icol + 1;
    const int icol3 = icol + get_nx<SVolField>(dim_,hasPlusXSideFaces_);
    const int icol4 = icol3 + 1;

    if( is_in_bounds<SVolField,YSurfXField>(dim_,icol ,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,YSurfXField>(dim_,icol2,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,YSurfXField>(dim_,icol3,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,YSurfXField>(dim_,icol4,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) ){
      cols.push_back( icol  );
      cols.push_back( icol2 );
      cols.push_back( icol3 );
      cols.push_back( icol4 );
    }
  }

  template<> inline void
  IndexHelper<SVolField,YSurfYField>::
  get_cols( const int irow, std::vector<int>& cols ) const
  {
    IndexTriplet ijk = flat2ijk<YSurfYField>::value( dim_, irow, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    shift_dest_index( dim_, ijk );
    if( ijk.i<0 || ijk.j<0 || ijk.k<0 ) return;
    const int icol = ijk2flat<SVolField>::value( dim_, ijk, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    if( is_in_bounds<SVolField,YSurfYField>(dim_,icol,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) ){
      cols.push_back( icol );
    }
  }

  template<> inline void
  IndexHelper<SVolField,YSurfZField>::
  get_cols( const int irow, std::vector<int>& cols ) const
  {
    IndexTriplet ijk = flat2ijk<YSurfZField>::value( dim_, irow, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    shift_dest_index( dim_, ijk );
    if( ijk.i<0 || ijk.j<0 || ijk.k<0 ) return;

    const int nx = get_nx<SVolField>(dim_,hasPlusXSideFaces_);
    const int ny = get_ny<SVolField>(dim_,hasPlusYSideFaces_);

    const int icol = ijk2flat<SVolField>::value( dim_, ijk, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    const int icol2 = icol + nx;
    const int icol3 = icol + nx*ny;
    const int icol4 = icol3 + nx;

    if( is_in_bounds<SVolField,YSurfZField>(dim_,icol ,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,YSurfZField>(dim_,icol2,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,YSurfZField>(dim_,icol3,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,YSurfZField>(dim_,icol4,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) ){
      cols.push_back( icol  );
      cols.push_back( icol2 );
      cols.push_back( icol3 );
      cols.push_back( icol4 );
    }
  }


  template<> inline void
  IndexHelper<SVolField,ZSurfXField>::
  get_cols( const int irow, std::vector<int>& cols ) const
  {
    IndexTriplet ijk = flat2ijk<ZSurfXField>::value( dim_, irow, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    shift_dest_index( dim_, ijk );
    if( ijk.i<0 || ijk.j<0 || ijk.k<0 ) return;

    const int icol = ijk2flat<SVolField>::value( dim_, ijk, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    const int icol2 = icol + 1;
    const int icol3 = icol + get_nx<SVolField>(dim_,hasPlusXSideFaces_)*get_ny<SVolField>(dim_,hasPlusYSideFaces_);
    const int icol4 = icol3 + 1;

    if( is_in_bounds<SVolField,ZSurfXField>(dim_,icol ,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,ZSurfXField>(dim_,icol2,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,ZSurfXField>(dim_,icol3,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,ZSurfXField>(dim_,icol4,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) ){
      cols.push_back( icol  );
      cols.push_back( icol2 );
      cols.push_back( icol3 );
      cols.push_back( icol4 );
    }
  }

  template<> inline void
  IndexHelper<SVolField,ZSurfYField>::
  get_cols( const int irow, std::vector<int>& cols ) const
  {
    IndexTriplet ijk = flat2ijk<ZSurfYField>::value( dim_, irow, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    shift_dest_index( dim_, ijk );
    if( ijk.i<0 || ijk.j<0 || ijk.k<0 ) return;

    const int nx = get_nx<SVolField>(dim_,hasPlusXSideFaces_);
    const int ny = get_ny<SVolField>(dim_,hasPlusYSideFaces_);

    const int icol = ijk2flat<SVolField>::value( dim_, ijk, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    const int icol2 = icol + nx;
    const int icol3 = icol + nx*ny;
    const int icol4 = icol3 + nx;

    if( is_in_bounds<SVolField,ZSurfYField>(dim_,icol ,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,ZSurfYField>(dim_,icol2,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,ZSurfYField>(dim_,icol3,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) &&
        is_in_bounds<SVolField,ZSurfYField>(dim_,icol4,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) ){
      cols.push_back( icol  );
      cols.push_back( icol2 );
      cols.push_back( icol3 );
      cols.push_back( icol4 );
    }
  }

  template<> inline void
  IndexHelper<SVolField,ZSurfZField>::
  get_cols( const int irow, std::vector<int>& cols ) const
  {
    IndexTriplet ijk = flat2ijk<ZSurfZField>::value( dim_, irow, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    shift_dest_index( dim_, ijk );
    if( ijk.i<0 || ijk.j<0 || ijk.k<0 ) return;
    const int icol = ijk2flat<SVolField>::value( dim_, ijk, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    if( is_in_bounds<SVolField,ZSurfZField>(dim_,icol,irow,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_) ){
      cols.push_back( icol );
    }
  }

  //------------------------------------------------------------------

}// namespace structured
}// namespace SpatialOps

#endif
