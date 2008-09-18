#include <iomanip>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>

#include <LagrangePoly.h>

using namespace std;

//--------------------------------------------------------------------

struct Output
{
  pair<double,double> errs;
  int npts;
  Output( const int n, const pair<double,double> er ) : errs(er), npts(n) {}
};

//--------------------------------------------------------------------

void check( const std::vector<double>& coefs,
	    const double* const cexpected,
	    const std::vector<int>& indices,
	    const int* const iexpected )
{
  cout << "Checking index & coefficient consistency ... " << flush;
  bool isFailed = false;
  vector<double>::const_iterator ic = coefs.begin();
  vector<int>   ::const_iterator ii = indices.begin();
  const double* ce = cexpected;
  const int* ie = iexpected;
  for( ; ic!=coefs.end(); ++ic, ++ii, ++ce, ++ie ){
    isFailed = fabs( *ic - *ce ) > 1e-16;
    isFailed = *ii != *ie;
    if( isFailed ){
      cout << "***FAIL***" << endl
	   << " Expected: ix=" << *ie << ", coef=" << *ce << endl
	   << " Found   : ix=" << *ii << ", coef=" << *ic << endl;
      return;
    }
  }
  if( isFailed ) cout << "FAIL" << endl;
  else cout << "PASS" << endl;
}

//--------------------------------------------------------------------

Output
test( const int npts,
      const int order,
      const double atolFun,
      const double atolDer )
{
  cout << "Testing interpolant and derivative accuracy (n=" << npts << ") ... " << flush;
  vector<double> x(npts), y(npts);
  
  const double dx = 3.1415 / (npts-1);

  vector<double>::iterator ix = x.begin(), iy=y.begin();
  for( int i=0; ix!=x.end(); ++ix, ++iy, ++i ){
    *ix = i*dx;
    *iy = std::sin( *ix );
  }

  const LagrangeInterpolant interp( x, y, order );
  const LagrangeDerivative  der   ( x, y, order );

  bool isFailed = false;
  double maxDerErr=0.0, maxIntErr=0.0;

  for( int k=0; k<50; ++k ){
    for( ix=x.begin(); ix!=x.end()-1; ++ix ){

      const double rn = fabs( dx * (rand()/RAND_MAX-0.5) );
      const double x = *ix  + rn;

      const double y = interp.value(x);
      double err = fabs( y - sin(x) );
      maxIntErr = max( maxIntErr, err );
      isFailed = isFailed || (err > atolFun);

      const double dy = der.value(x);
      err = fabs( dy - cos(x) );
      maxDerErr = max(maxDerErr,err);
      isFailed = isFailed || err>atolDer;
    }
  }

  if( isFailed ){
    cout << "FAIL" << endl
	 << "  Max err for interpolation: " << maxIntErr << ",  tolerance=" << atolFun << endl
	 << "  Max err for derivative   : " << maxDerErr << ",  tolerance=" << atolDer
	 << endl;
  }
  else cout << "PASS" << endl;
  return Output(npts,make_pair(maxIntErr,maxDerErr));
}

//--------------------------------------------------------------------

void test2( const int npts,
	    const int order )
{
  vector<double> x(npts), y(npts);
  const double dx = 3.1415 / (npts-1);
  vector<double>::iterator ix = x.begin(), iy=y.begin();
  for( int i=0; ix!=x.end(); ++ix, ++iy, ++i ){
    *ix = i*dx;
    *iy = std::sin( *ix );
  }

  const LagrangeInterpolant interp( x, y, order );
  const LagrangeDerivative  der   ( x, y, order );

  ofstream fout("./interp.out",ios::out);
  ofstream dout("./der.out",ios::out);
  fout << "# x yapprox   x  yexact" << endl;
  dout << "# x dyapprox  x  dyexact" << endl;
    for( ix=x.begin(); ix!=x.end(); ++ix ){
      const double rn = fabs( dx * (rand()/RAND_MAX-0.5) );
      const double x = *ix  + rn;

      const double y = interp.value(x);
      const double dy = der.value(x);
      fout << x << " " << y  << " " << *ix << " " << sin(*ix) << endl;
      dout << x << " " << dy << " " << *ix << " " << cos(*ix) << endl;
    }
    fout.close();
    dout.close();
}

//--------------------------------------------------------------------
void test_coefs()
{
  vector<double> coefs;
  vector<int> indices;

  // nonuniform mesh
  {
    std::vector<double> xpts;
    xpts.push_back( 0.00 );
    xpts.push_back( 0.20 );
    xpts.push_back( 0.30 );
    xpts.push_back( 0.45 );
    xpts.push_back( 0.60 );
    xpts.push_back( 0.71 );
    xpts.push_back( 1.00 );

    LagrangeCoefficients lagrangeCoefs( xpts );
    lagrangeCoefs.get_interp_coefs_indices( 0.5, 1, coefs, indices );
    const double c1 [] = { 0.666667, 0.333333 };
    const int    i1 [] = { 3, 4 };
    check( coefs, c1, indices, i1 );

    lagrangeCoefs.get_derivative_coefs_indices( 0.5, 1, coefs, indices );
    const double c2 [] = { 0.0, 0.0 };
    const int i2 [] = { 3, 4 };
    check( coefs, c2, indices, i2 );
 
    LagrangeInterpolant interp( xpts, xpts );
    assert( std::fabs(interp.value(0.768) - 0.768) < 1.0e-15 );

    LagrangeDerivative der( xpts, xpts );
    assert( std::fabs( der.value(0.912) - 1.0) < 1.0e-15 );
  }

  // uniform mesh:
  {
    int npts = 11;
    const double dx = 1.0/double(npts-1);
    vector<double> xpts;
    for( int i=0; i<npts; ++i ){
      xpts.push_back( i*dx );
    }

    LagrangeCoefficients lagrangeCoefs2(xpts);

    lagrangeCoefs2.get_interp_coefs_indices( 0.48, 1, coefs, indices );
    const double cexpected [] = { 0.2, 0.8 };
    const int iexpected [] = { 4, 5 };
    check( coefs, cexpected, indices, iexpected );
    
    lagrangeCoefs2.get_derivative_coefs_indices( 0.055, 3, coefs, indices );
    const double c2 [] = {-8.84583, 7.0375, 2.4625, -0.654167 };
    const int i2 [] = {0, 1, 2, 3};
    check( coefs, c2, indices, i2 );

    LagrangeInterpolant interp( xpts, xpts );
    assert( 0.47 == interp.value( 0.47 ) );

    LagrangeDerivative der( xpts, xpts );
    assert( 1.0 == der.value( 0.32 ) );
  }
}

//--------------------------------------------------------------------

int main()
{
  test_coefs();

  // error tolerances hard-coded for second order polynomial.
  const int order = 2;
  vector<Output> errs;
  errs.push_back( test( 10,  order, 1.29e-2, 1.14e-1 ) );
  errs.push_back( test( 20,  order, 1.41e-3, 2.61e-2 ) );
  errs.push_back( test( 40,  order, 1.64e-4, 6.21e-3 ) );
  errs.push_back( test( 80,  order, 1.97e-5, 1.52e-3 ) );
  errs.push_back( test( 160, order, 2.42e-6, 3.75e-4 ) );
  errs.push_back( test( 320, order, 2.99e-7, 9.30e-5 ) );
  errs.push_back( test( 640, order, 3.72e-8, 2.32e-5 ) );
  errs.push_back( test( 789, order, 1.99e-8, 1.53e-5 ) );

  ofstream fout( "converge.out", ios::out );
  fout << setw(7) << "# npts" << setw(15) << "FunErr" << setw(15) << "DerErr" << endl;
  for( vector<Output>::const_iterator ii=errs.begin(); ii!=errs.end(); ++ii ){
    fout << setw(7) << ii->npts
	 << setw(15) << ii->errs.first
	 << setw(15) << ii->errs.second << endl;
  }
  fout.close();
  
  //  test2( 10, 2 );
}

//--------------------------------------------------------------------
