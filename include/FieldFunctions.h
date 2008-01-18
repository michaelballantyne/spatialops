#ifndef FieldFunctions_h
#define FieldFunctions_h

#include <boost/static_assert.hpp>

#include <SpatialOpsTools.h>


//====================================================================

namespace SpatialOps{


  //==================================================================

  namespace FFLocal{
    struct NULLPatch
    {
      struct FieldID{ FieldID(){} };
    };
  }

  //==================================================================


/**
 *  @class FieldFunction3D
 *  @author James C. Sutherland
 *  @date  September, 2007
 *
 *  @brief Supports implementation of 3D functions to assign values to
 *  SpatialField objects.
 *
 *  @par Template Parameters
 *  
 *   <ul>
 *
 *   <li> \b FieldT The type of field that this function applies to.
 *
 *   <li> \b PatchT The patch type (if applicable) that this field
 *   lives on.  If no patch is specified, then field references for
 *   the spatial coordinates must be provded at construction.
 *
 *   \b PatchT must define the FieldID type for field identifiers.  It
 *   must also provide access to FieldManager objects.
 *
 *   Note that \b PatchT is an optional template parameter.  In the
 *   case where a patch is not being used, or where field references
 *   are available at construction, this template parameter may be
 *   omitted.
 */
template<typename FieldT, typename PatchT=FFLocal::NULLPatch>
class FieldFunction3D
{
  typedef typename PatchT::FieldID  FieldID;
public:

  typedef FieldT  FieldType;
  typedef PatchT  PatchType;

  virtual void evaluate( FieldT& phi ) const =0;

  virtual void dx( FieldT& gradPhi ) const{ assert(0); }
  virtual void dy( FieldT& gradPhi ) const{ assert(0); }
  virtual void dz( FieldT& gradPhi ) const{ assert(0); }

  virtual void d2x( FieldT& gradPhi ) const{ assert(0); }
  virtual void d2y( FieldT& gradPhi ) const{ assert(0); }
  virtual void d2z( FieldT& gradPhi ) const{ assert(0); }


  const FieldT& get_x() const{set_fields(); return *x_;}
  const FieldT& get_y() const{set_fields(); return *y_;}
  const FieldT& get_z() const{set_fields(); return *z_;}

  virtual ~FieldFunction3D(){}

protected:

  /** Use this constructor when the PatchT was not specified. */
  FieldFunction3D( const FieldT& x, const FieldT& y, const FieldT& z );

  /** Use this constructor when the PatchT was specified. */
  FieldFunction3D( PatchT& p, const FieldID xid, const FieldID yid, const FieldID zid );

  /**
   *  In the case where field ids were provided and this is built
   *  using a patch, then this binds fields.  It must be called before
   *  using any of the fields.
   */
  void set_fields() const;

private:
  PatchT* const patch_;
  const FieldID xid_, yid_, zid_;
  mutable const FieldT* x_;
  mutable const FieldT* y_;
  mutable const FieldT* z_;
};

//====================================================================

/**
 *  @class FieldFunction2D
 *  @author James C. Sutherland
 *  @date  September, 2007
 *
 *  @brief Supports implementation of 1D functions to assign values to
 *  SpatialField objects.
 *
 *  See documentation on FieldFunction3D for more information about template parameters.
 */
template<typename FieldT, typename PatchT=FFLocal::NULLPatch>
class FieldFunction2D
{
  typedef typename PatchT::FieldID  FieldID;
public:

  typedef FieldT  FieldType;
  typedef PatchT  PatchType;

  virtual void evaluate( FieldT& phi ) const =0;


  virtual void dx( FieldT& gradPhi ) const{ const bool implemented=false; assert(implemented); }
  virtual void dy( FieldT& gradPhi ) const{ const bool implemented=false; assert(implemented); }

  virtual void d2x( FieldT& gradPhi ) const{ const bool implemented=false; assert(implemented); }
  virtual void d2y( FieldT& gradPhi ) const{ const bool implemented=false; assert(implemented); }


  const FieldT& get_x() const{set_fields(); return *x_;}
  const FieldT& get_y() const{set_fields(); return *y_;}

  virtual ~FieldFunction2D(){}

protected:

  FieldFunction2D( const FieldT& x, const FieldT& y );
  FieldFunction2D( PatchT& p, const FieldID xid, const FieldID yid, const FieldID zid );

  void set_fields() const;

private:
  PatchT* const patch_;
  const FieldID xid_, yid_;
  mutable const FieldT* x_;
  mutable const FieldT* y_;
};

//====================================================================

/**
 *  @class FieldFunction1D
 *  @author James C. Sutherland
 *  @date  September, 2007
 *
 *  @brief Supports implementation of 1D functions to assign values to
 *  SpatialField objects.
 *
 *  See documentation on FieldFunction3D for more information about template parameters.
 */
template< typename FieldT, typename PatchT=FFLocal::NULLPatch >
class FieldFunction1D
{
  typedef typename PatchT::FieldID  FieldID;
public:

  virtual void evaluate( FieldT& f ) const =0;

  virtual void dx ( FieldT& gradPhi ) const{ const bool implemented=false; assert(implemented); }
  virtual void d2x( FieldT& gradPhi ) const{ const bool implemented=false; assert(implemented); }

  const FieldT& get_x() const{set_fields(); return *x_;}

  virtual ~FieldFunction1D(){}

protected:

  FieldFunction1D( const FieldT& x );
  FieldFunction1D( PatchT& p, const FieldID xid );

  void set_fields() const;

private:
  PatchT* const patch_;
  const FieldID xid_;
  mutable const FieldT* x_;
};


//====================================================================


/**
 *   NOTE: this populates ghost fields as well.
 */
template< typename FieldT,
	  typename PatchT=FFLocal::NULLPatch >
class LinearFunction1D : public FieldFunction1D<FieldT,PatchT>
{
  typedef typename PatchT::FieldID  FieldID;
public:

  LinearFunction1D( const FieldT& x,
		    const double slope,
		    const double intercept )
    : FieldFunction1D<FieldT,PatchT>(x),
      m_(slope),
      b_(intercept)
  {}

  LinearFunction1D( PatchT& p,
		    const FieldID xid,
		    const double slope,
		    const double intercept )
    : FieldFunction1D<FieldT,PatchT>(p,xid), m_(slope), b_(intercept)
  {}

  ~LinearFunction1D(){}

  void evaluate( FieldT& f ) const;
  void dx( FieldT& gradPhi ) const;
  void d2x( FieldT& gradPhi ) const;

protected:

private:
  const double m_, b_;
};


//====================================================================

/**
 *  @class SinFunction
 *  @author James C. Sutherland
 *  @date September, 2007
 *
 *  Evaluates a*sin(b*x), where a is the amplitude and b is the period
 *  of the sin function.
 */
template< typename FieldT,
	  typename PatchT=FFLocal::NULLPatch >
class SinFunction : public FieldFunction1D<FieldT,PatchT>
{
  typedef typename PatchT::FieldID FieldID;
 public:
  SinFunction( PatchT& p,
	       const FieldID xid,
	       const double amplitude,
	       const double period );
  SinFunction( const FieldT& x,
	       const double amplitude,
	       const double period );
  ~SinFunction(){}
  void evaluate( FieldT& f ) const;
  void dx( FieldT& df ) const;
  void d2x( FieldT& d2f ) const;
 protected:
  void evaluate_implement( FieldT& f, const FieldT& x ) const;

 private:
  const double a_, b_;
};

//====================================================================







// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//                           Implementations
//
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%







//====================================================================


namespace FFLocal{
  template<typename FieldT, typename PatchT>
  struct FieldSetter
  {
    static void set( const FieldT* const f, PatchT* p, typename PatchT::FieldID id )
    {
      f = p->template field_manager<FieldT>().field_ref(id);
    }
  };

//====================================================================

  template<typename FieldT>
  struct FieldSetter<FieldT,NULLPatch>
  {
    static void set( const FieldT* const f, NULLPatch* p, typename NULLPatch::FieldID id )
    {
    }
  };
}


//====================================================================


//--------------------------------------------------------------------
template<typename FieldT,typename PatchT>
FieldFunction1D<FieldT,PatchT>::FieldFunction1D( const FieldT& x )
  : patch_( NULL ),
    x_( &x )
{}
//--------------------------------------------------------------------
template<typename FieldT,typename PatchT>
FieldFunction1D<FieldT,PatchT>::FieldFunction1D( PatchT& p, const FieldID xid )
  : patch_( &p ),
    xid_( xid )
{
  BOOST_STATIC_ASSERT( bool(!SpatialOps::IsSameType<PatchT,FFLocal::NULLPatch>::result) );
  x_ = NULL;
}
//--------------------------------------------------------------------
template<typename FieldT,typename PatchT>
void
FieldFunction1D<FieldT,PatchT>::set_fields() const
{
  if(x_==NULL){
    FFLocal::FieldSetter<FieldT,PatchT>::set( x_, patch_, xid_ );
  }
}
//--------------------------------------------------------------------


//====================================================================


//--------------------------------------------------------------------
template<typename FieldT,typename PatchT>
FieldFunction2D<FieldT,PatchT>::FieldFunction2D( const FieldT& x, const FieldT& y )
  : patch_( NULL ),
    x_(&x), y_(&y)
{
}
//--------------------------------------------------------------------
template<typename FieldT,typename PatchT>
FieldFunction2D<FieldT,PatchT>::FieldFunction2D( PatchT& p, const FieldID xid, const FieldID yid, const FieldID zid )
  : patch_(&p),
  xid_(xid), yid_(yid)
{
  BOOST_STATIC_ASSERT( bool(!SpatialOps::IsSameType<PatchT,FFLocal::NULLPatch>::result) );
  x_ = y_ = NULL;
}
//--------------------------------------------------------------------
template<typename FieldT,typename PatchT>
void
FieldFunction2D<FieldT,PatchT>::set_fields() const
{
  if(x_==NULL){
    FFLocal::FieldSetter<FieldT,PatchT>::set( x_, patch_, xid_ );
    FFLocal::FieldSetter<FieldT,PatchT>::set( y_, patch_, yid_ );
  }
}
//--------------------------------------------------------------------


//====================================================================


//--------------------------------------------------------------------
template<typename FieldT,typename PatchT>
FieldFunction3D<FieldT,PatchT>::FieldFunction3D( const FieldT& x, const FieldT& y, const FieldT& z )
  : patch_( NULL ),
    x_( &x ),
    y_( &y ),
    z_( &z )
{}
//--------------------------------------------------------------------
template<typename FieldT,typename PatchT>
FieldFunction3D<FieldT,PatchT>::FieldFunction3D( PatchT& p, const FieldID xid, const FieldID yid, const FieldID zid )
  : patch_( &p ),
    xid_(xid), yid_(yid), zid_(zid)
{
  BOOST_STATIC_ASSERT( bool(!SpatialOps::IsSameType<PatchT,FFLocal::NULLPatch>::result) );
  x_ = y_ = z_ = NULL;
}
//--------------------------------------------------------------------
template<typename FieldT,typename PatchT>
void
FieldFunction3D<FieldT,PatchT>::set_fields() const
{
  if(x_==NULL){
    FFLocal::FieldSetter<FieldT,PatchT>::set( x_, patch_, xid_ );
    FFLocal::FieldSetter<FieldT,PatchT>::set( y_, patch_, yid_ );
    FFLocal::FieldSetter<FieldT,PatchT>::set( z_, patch_, zid_ );
  }
}

//====================================================================


//------------------------------------------------------------------
template<typename FieldT, typename PatchT>
void
LinearFunction1D<FieldT,PatchT>::
evaluate( FieldT& f ) const
{
  this->set_fields();
  typename FieldT::const_iterator ix = this->get_x().begin();
  typename FieldT::iterator ifld = f.begin();
  typename FieldT::iterator iflde= f.end();

  for( ; ifld!=iflde; ++ifld, ++ix ){
    *ifld = *ix*m_ + b_;
  }
}
//------------------------------------------------------------------
template<typename FieldT, typename PatchT>
void
LinearFunction1D<FieldT,PatchT>::
dx( FieldT& f ) const
{
  f = m_;
}
//------------------------------------------------------------------
template<typename FieldT, typename PatchT>
void
LinearFunction1D<FieldT,PatchT>::
d2x( FieldT& f ) const
{
  f = 0.0;
}
//--------------------------------------------------------------------


//====================================================================


//--------------------------------------------------------------------
template<typename FieldT, typename PatchT>
SinFunction<FieldT,PatchT>::
SinFunction( const FieldT& x,
	     const double a,
	     const double b )
  : FieldFunction1D<FieldT,PatchT>(x),
    a_( a ),
    b_( b )
{
}
//--------------------------------------------------------------------
template<typename FieldT, typename PatchT>
SinFunction<FieldT,PatchT>::
SinFunction( PatchT& p,
	     const FieldID xid,
	     const double a,
	     const double b )
  : FieldFunction1D<FieldT,PatchT>( p, xid ),
    a_( a ),
    b_( b )
{
}
//------------------------------------------------------------------
template<typename FieldT, typename PatchT>
void
SinFunction<FieldT,PatchT>::
evaluate( FieldT& f ) const
{
  this->set_fields();
  typename FieldT::iterator   ix = this->get_x().begin();
  typename FieldT::iterator ifld = f.begin();
  typename FieldT::iterator iflde= f.end();

  for( ; ifld!=iflde; ++ifld, ++ix ){
    *ifld = a_ * std::sin(*ix*b_);
  }
}
//------------------------------------------------------------------
template<typename FieldT, typename PatchT>
void
SinFunction<FieldT,PatchT>::
dx( FieldT& f ) const
{
  this->set_fields();
  typename FieldT::iterator   ix = this->get_x().begin();
  typename FieldT::iterator ifld = f.begin();
  typename FieldT::iterator iflde= f.end();

  for( ; ifld!=iflde; ++ifld, ++ix ){
    *ifld = a_*b_ * std::cos(*ix*b_);
  }
}
//------------------------------------------------------------------
template<typename FieldT, typename PatchT>
void
SinFunction<FieldT,PatchT>::
d2x( FieldT& f ) const
{
  this->set_fields();
  typename FieldT::iterator   ix = this->get_x().begin();
  typename FieldT::iterator ifld = f.begin();
  typename FieldT::iterator iflde= f.end();

  const double tmp = a_*b_*b_;

  for( ; ifld!=iflde; ++ifld, ++ix ){
    *ifld =  -tmp * std::sin(*ix*b_);
  }
}
//--------------------------------------------------------------------




} // namespace SpatialOps

#endif
