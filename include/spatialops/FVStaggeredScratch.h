#ifndef FVStaggeredScratch_h
#define FVStaggeredScratch_h

#include <spatialops/FVStaggeredIndexHelper.h>
#include <spatialops/FVStaggeredTypes.h>

#include <spatialops/SpatialField.h>
#include <spatialops/SpatialOperator.h>
#include <spatialops/SpatialOpsDefs.h>

#include <algorithm>

namespace SpatialOps{


  // forward declaration.
  namespace FVStaggered{ template<typename T1,typename T2> class ScratchAssembler; }

  // this is required for the SpatialOperator class.  It specifies
  // that we should use the ScratchAssembler to construct
  // Scratch objects.
  template< typename SrcFieldT, typename DestFieldT >
  struct OpAssemblerSelector< Scratch, SrcFieldT, DestFieldT >
  {
    typedef FVStaggered::ScratchAssembler<SrcFieldT,DestFieldT>  Assembler;
  };

  // scratch ops that look like grad ops
  template<>
  struct OpAssemblerSelector< Scratch, FVStaggered::SVolField, FVStaggered::SSurfXField >
  {
    typedef FVStaggered::GradientAssembler<FVStaggered::SVolField,FVStaggered::SSurfXField> Assembler;
  };
  template<>
  struct OpAssemblerSelector< Scratch, FVStaggered::SVolField, FVStaggered::SSurfYField >
  {
    typedef FVStaggered::GradientAssembler<FVStaggered::SVolField,FVStaggered::SSurfYField> Assembler;
  };
  template<>
  struct OpAssemblerSelector< Scratch, FVStaggered::SVolField, FVStaggered::SSurfZField >
  {
    typedef FVStaggered::GradientAssembler<FVStaggered::SVolField,FVStaggered::SSurfZField> Assembler;
  };



  template<>
  struct OpAssemblerSelector< Scratch, FVStaggered::XVolField, FVStaggered::XSurfXField >
  {
    typedef FVStaggered::GradientAssembler<FVStaggered::XVolField,FVStaggered::XSurfXField> Assembler;
  };
  template<>
  struct OpAssemblerSelector< Scratch, FVStaggered::XVolField, FVStaggered::XSurfYField >
  {
    typedef FVStaggered::GradientAssembler<FVStaggered::XVolField,FVStaggered::XSurfYField> Assembler;
  };
  template<>
  struct OpAssemblerSelector< Scratch, FVStaggered::XVolField, FVStaggered::XSurfZField >
  {
    typedef FVStaggered::GradientAssembler<FVStaggered::XVolField,FVStaggered::XSurfZField> Assembler;
  };


  template<>
  struct OpAssemblerSelector< Scratch, FVStaggered::YVolField, FVStaggered::YSurfXField >
  {
    typedef FVStaggered::GradientAssembler<FVStaggered::YVolField,FVStaggered::YSurfXField> Assembler;
  };
  template<>
  struct OpAssemblerSelector< Scratch, FVStaggered::YVolField, FVStaggered::YSurfYField >
  {
    typedef FVStaggered::GradientAssembler<FVStaggered::YVolField,FVStaggered::YSurfYField> Assembler;
  };
  template<>
  struct OpAssemblerSelector< Scratch, FVStaggered::YVolField, FVStaggered::YSurfZField >
  {
    typedef FVStaggered::GradientAssembler<FVStaggered::YVolField,FVStaggered::YSurfZField> Assembler;
  };


  template<>
  struct OpAssemblerSelector< Scratch, FVStaggered::ZVolField, FVStaggered::ZSurfXField >
  {
    typedef FVStaggered::GradientAssembler<FVStaggered::ZVolField,FVStaggered::ZSurfXField> Assembler;
  };
  template<>
  struct OpAssemblerSelector< Scratch, FVStaggered::ZVolField, FVStaggered::ZSurfYField >
  {
    typedef FVStaggered::GradientAssembler<FVStaggered::ZVolField,FVStaggered::ZSurfYField> Assembler;
  };
  template<>
  struct OpAssemblerSelector< Scratch, FVStaggered::ZVolField, FVStaggered::ZSurfZField >
  {
    typedef FVStaggered::GradientAssembler<FVStaggered::ZVolField,FVStaggered::ZSurfZField> Assembler;
  };





namespace FVStaggered{


  template< typename SrcFieldT, typename DestFieldT >
  class ScratchAssembler
  {
  public:

    unsigned int num_nonzeros() const;

    ScratchAssembler( const std::vector<int>& dimExtent,
                      const int dir,
                      const bool hasPlusXSideFaces,
                      const bool hasPlusYSideFaces,
                      const bool hasPlusZSideFaces );

    ~ScratchAssembler(){}

    int get_ncols() const;   ///< Return the number of columns in this operator
    int get_nrows() const;   ///< Return the number of rows in this operator

    /**
     *  @brief Obtain the nonzero values for this operator on the given row.
     *
     *  @param irow The index for the row to obtain entries for.
     *  @param vals A vector containing the values for the nonzero
     *         entries in this row.
     *  @param ixs A vector containing the column indices
     *         corresponding to each nonzero entry.
     */
    void get_row_entries( const int irow,
                          std::vector<double> & vals,
                          std::vector<int> & ixs ) const;

    void get_ghost_cols( std::set<int>& ghostCols ) const
    {
      ghostCols = get_ghost_set<SrcFieldT>( dim_, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    }
    void get_ghost_rows( std::set<int>& ghostRows ) const
    {
      ghostRows = get_ghost_set<DestFieldT>( dim_, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    }

  private:

    IndexHelper<SrcFieldT,DestFieldT> indexHelper_;
    const std::vector<int>& dim_;
    const int dir_;
    const bool hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_;
  };


  //==================================================================






  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //
  //                           Implementation
  //
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%






  //==================================================================


  //------------------------------------------------------------------
  template<typename SrcFieldT, typename DestFieldT>
  ScratchAssembler<SrcFieldT,DestFieldT>::
  ScratchAssembler( const std::vector<int>& dimExtent,
                    const int dir,
                    const bool hasPlusXSideFaces,
                    const bool hasPlusYSideFaces,
                    const bool hasPlusZSideFaces )
    : indexHelper_( dimExtent, hasPlusXSideFaces, hasPlusZSideFaces, hasPlusZSideFaces ),
      dim_( dimExtent ),
      dir_( dir ),
      hasPlusXSideFaces_( hasPlusXSideFaces ),
      hasPlusYSideFaces_( hasPlusYSideFaces ),
      hasPlusZSideFaces_( hasPlusZSideFaces )
  {
  }
  //------------------------------------------------------------------
  template<typename SrcFieldT, typename DestFieldT>
  unsigned int
  ScratchAssembler<SrcFieldT,DestFieldT>::
  num_nonzeros() const
  {
    return 3;
  }
  //------------------------------------------------------------------
  template<typename SrcFieldT, typename DestFieldT>
  int
  ScratchAssembler<SrcFieldT,DestFieldT>::
  get_ncols() const
  {
    int n=1;
    switch( dir_ ){
    case XDIR::value :
      n = get_nx<SrcFieldT>(dim_,hasPlusXSideFaces_);
      break;                                                               
    case YDIR::value :                                             
      n = get_ny<SrcFieldT>(dim_,hasPlusYSideFaces_);
      break;                                                               
    case ZDIR::value :                                             
      n = get_nz<SrcFieldT>(dim_,hasPlusZSideFaces_);
      break;
    }
    if( n>1 ) n = get_n_tot<SrcFieldT>(dim_,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_);
    return n;
  }
  //------------------------------------------------------------------
  template<typename SrcFieldT, typename DestFieldT>
  int
  ScratchAssembler<SrcFieldT,DestFieldT>::
  get_nrows() const
  {
    int n=1;
    switch( dir_ ){
    case XDIR::value :
      n = get_nx<DestFieldT>(dim_,hasPlusXSideFaces_);
      break;                                                               
    case YDIR::value :                                             
      n = get_ny<DestFieldT>(dim_,hasPlusYSideFaces_);
      break;                                                               
    case ZDIR::value :                                             
      n = get_nz<DestFieldT>(dim_,hasPlusZSideFaces_);
      break;
    }
    if( n>1 ) n = get_n_tot<DestFieldT>(dim_,hasPlusXSideFaces_,hasPlusYSideFaces_,hasPlusZSideFaces_);
    return n;
  }
  //------------------------------------------------------------------
  template<typename SrcFieldT, typename DestFieldT>
  void
  ScratchAssembler<SrcFieldT,DestFieldT>::
  get_row_entries( const int irow,
                   std::vector<double> & vals,
                   std::vector<int> & ixs ) const
  {
    // self:
    ixs.push_back( irow );
    vals.push_back( -1.0 );

    if( get_nrows() <= 1 ) return;

    const int nx = get_nx<SrcFieldT>( dim_, hasPlusXSideFaces_ );

    const IndexTriplet trow = flat2ijk<DestFieldT>::value( dim_, irow, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
    IndexTriplet t;

    switch ( dir_ ){

    case XDIR::value : {
      if( dim_[0]>1 ){
        const int stride = 1;
        t = flat2ijk<SrcFieldT>::value( dim_, irow+stride, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_  );
        IndexHelper<SrcFieldT,DestFieldT>::shift_dest_index( dim_, t );
        if( t.i >= 0 && t.i < nx  &&  trow.j==t.j && trow.k==t.k ){
          ixs.push_back( irow+stride );
          vals.push_back(-1.0);
        }
        t = flat2ijk<SrcFieldT>::value( dim_, irow-stride, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
        IndexHelper<SrcFieldT,DestFieldT>::shift_dest_index( dim_, t );
        if( t.i >= 0 && t.i < nx  &&  trow.j==t.j && trow.k==t.k ){
          ixs.push_back( irow-stride );
          vals.push_back( -1.0 );
        }
      }
      break;
    }

    case YDIR::value : {
      if( dim_[1]>1 ){
        const int ny = get_ny<SrcFieldT>( dim_, hasPlusYSideFaces_ );
        const int stride = nx>1 ? nx : 1;
        t = flat2ijk<SrcFieldT>::value( dim_, irow+stride, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
        IndexHelper<SrcFieldT,DestFieldT>::shift_dest_index( dim_, t );
        if( t.j >= 0 && t.j < ny  &&  trow.i==t.i && trow.k==t.k ){
          ixs.push_back( irow+stride );
          vals.push_back( -1.0 );
        }
        t = flat2ijk<SrcFieldT>::value( dim_, irow-stride, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
        IndexHelper<SrcFieldT,DestFieldT>::shift_dest_index( dim_, t );
        if( t.j >= 0 && t.j < ny  &&  trow.i==t.i && trow.k==t.k ){
          ixs.push_back( irow-stride );
          vals.push_back( -1.0 );
        }
      }
      break;
    }

    case ZDIR::value : {
      if( dim_[2]>1 ){
        const int ny = get_ny<SrcFieldT>( dim_, hasPlusYSideFaces_ );
        const int nz = get_nz<SrcFieldT>( dim_, hasPlusZSideFaces_ );
        const int stride = (nx>1 || ny>1) ? nx*ny : 1;
        t = flat2ijk<SrcFieldT>::value( dim_, irow+stride, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
        IndexHelper<SrcFieldT,DestFieldT>::shift_dest_index( dim_, t );
        if( t.k >= 0 && t.k < nz  &&  trow.i==t.i && trow.j==t.j ){
          ixs.push_back( irow+stride );
          vals.push_back( -1.0 );
        }
        t = flat2ijk<SrcFieldT>::value( dim_, irow-stride, hasPlusXSideFaces_, hasPlusYSideFaces_, hasPlusZSideFaces_ );
        IndexHelper<SrcFieldT,DestFieldT>::shift_dest_index( dim_, t );
        if( t.k >= 0 && t.k < nz  &&  trow.i==t.i && trow.j==t.j ){
          ixs.push_back( irow-stride );
          vals.push_back( -1.0 );
        }
      }
      break;
    }
    } // end switch

  }
  //------------------------------------------------------------------



}// namespace FVStaggered
}// namespace SpatialOps

#endif
