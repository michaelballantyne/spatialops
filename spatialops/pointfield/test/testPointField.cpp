#include <spatialops/pointfield/PointFieldTypes.h>
#include <spatialops/pointfield/PointOperators.h>

#include <spatialops/structured/FVStaggeredBCTools.h>

#include <test/TestHelper.h>

#include <iostream>

typedef SpatialOps::Point::PointField FieldT;

int main()
{
  const size_t npts = 10;
  const SpatialOps::structured::MemoryWindow mw( SpatialOps::structured::IntVec( npts, 1, 1 ) );

  FieldT f( mw, NULL );

  double x=0.1;
  for( FieldT::iterator ifld=f.begin(); ifld!=f.end(); ++ifld, x+=1.0 ){
    *ifld = x;
  }

  TestHelper status(true);

  FieldT::interior_iterator i2=f.interior_begin();
  for( FieldT::iterator i=f.begin(); i!=f.end(); ++i, ++i2 ){
    status( *i==*i2, "value" );
    status( &*i == &*i2, "address" );
  }

  {
    typedef SpatialOps::structured::ConstValEval BCVal;
    typedef SpatialOps::structured::BoundaryCondition<FieldT,BCVal> BC;

    BC bc1( SpatialOps::structured::IntVec(2,1,1), BCVal(1.234) );
    BC bc2( SpatialOps::structured::IntVec(4,1,1), BCVal(3.456) );
  
    bc1(f);
    bc2(f);
    status( f[2] == 1.234, "point BC 1" );
    status( f[4] == 3.456, "point BC 2" );
  }

  {
    std::vector<size_t> ix;
    ix.push_back(4);
    ix.push_back(2);
    SpatialOps::Point::FieldToPoint<FieldT> ftp(ix);
    SpatialOps::Point::PointToField<FieldT> ptf(ix);

    const SpatialOps::structured::MemoryWindow mw2( SpatialOps::structured::IntVec(2,1,1) );
    FieldT f2( mw2, NULL );
    ftp.apply_to_field( f, f2 );
    status( f2[0] == 3.456, "Field2Point Interp (1)" );
    status( f2[1] == 1.234, "Field2Point Interp (2)" );

    f2[0] = -1.234;
    f2[1] = -3.456;
    ptf.apply_to_field( f2, f );
    status( f[2] == -3.456, "Point2Field Interp (1)" );
    status( f[4] == -1.234, "Point2Field Interp (2)" );
  }


  if( status.ok() ) return 0;
  return -1;
}