#ifndef UT_LinearSystem_h
#define UT_LinearSystem_h

#include <vector>
#include <map>
#include <string>

#ifdef HAVE_MPI
#include <mpi.h>
#endif

// SpatialOps forward declarations:
namespace SpatialOps{
  class SpatialOperator;
  class SpatialField;
}


// trilinos foward declarations
class Epetra_Map;
class Epetra_Comm;
class Epetra_CrsMatrix;
class Epetra_Vector;
class Epetra_LinearProblem;
class AztecOO;


// local forward declarations
class LinearSystem;


//====================================================================

/**
 *  @struct LinSysInfo
 *  @author James C. Sutherland
 *  @date   January, 2007
 *
 *  Information required to obtain a LinearSystem from the
 *  LinSysFactory.
 */
struct LinSysInfo
{

  enum SolverPackage{
    TRILINOS  // currently only TRILINOS is supported.
  };

  enum Preconditioner{
    NONE,
    DEFAULT
  };

#ifdef HAVE_MPI
  LinSysInfo( const std::vector<int> & npts,
	      MPI_Comm & communicator );
#else
  LinSysInfo( const std::vector<int> & npts );
#endif

  ~LinSysInfo();

  SolverPackage solverPackage;
  Preconditioner preconditioner;

  //
  // these are required to build the linear system.
  //

  const std::vector<int> dimExtent;

#ifdef HAVE_MPI
  MPI_Comm & comm;
#endif

  bool operator ==(const LinSysInfo&) const;
  bool operator < (const LinSysInfo&) const;
};


//====================================================================


/**
 *  @class  LinSysFactory
 *  @author James C. Sutherland
 *  @date   January, 2007
 *
 *  Factory to produce linear systems.
 */
class LinSysFactory
{
public:
  static LinSysFactory& self();

  void bind_name_to_info( const LinSysInfo& info,
			  const std::string & name );

  LinearSystem & get_linsys( const LinSysInfo& info );

  LinearSystem & get_linsys( const std::string & name );

private:
  LinSysFactory();
  ~LinSysFactory();

  typedef std::map<LinSysInfo,LinearSystem*> InfoMap;
  InfoMap infoMap_;

  typedef std::map<std::string,LinSysInfo> NameInfoMap;
  NameInfoMap nameInfoMap_;
};


//====================================================================


/**
 *  @class  RHS
 *  @author James C. Sutherland
 *  @date   December, 2006
 *
 *  Abstraction for the RHS field.  This is intended for use in a
 *  segregated implicit scheme where the RHS is used in conjunction
 *  with a LHS operator, or in an explicit scheme where the RHS is
 *  simply used to obtain an update for the solution field directly.
 */
class RHS
{
public:

  RHS( const std::vector<int> & domainExtent );

  ~RHS();

  void reset( const double val = 0.0 );

  void reset( const int rownum, const double value=0.0 );

  void add_contribution( const SpatialOps::SpatialField & localField,
			 const double scaleFac = 1.0 );

  const std::vector<double> & get_field() const{return field_;}

        double* get_ptr()      { return &field_[0]; }
  const double* get_ptr() const{ return &field_[0]; }

  const std::vector<int> & get_extent() const{return extent_;}

private:
  bool consistency_check( const SpatialOps::SpatialField & f ) const;

  const std::vector<int> extent_;
  const int npts_;

  std::vector<double> field_;

  RHS();
  RHS(const RHS&);
};

//====================================================================

/**
 *  @class  LHS
 *  @author James C. Sutherland
 *  @date   December, 2006
 *
 *  Abstraction for the LHS operator.  This is a distributed LHS
 *  operator for use in a linear system.
 */
class LHS
{
public:

  LHS( const std::vector<int> & extent,
       Epetra_CrsMatrix& A );

  ~LHS();

  int nrows() const{ return nrows_; }
  int ncols() const{ return ncols_; }

  /** reset all values in the matrix */
  void reset( const double val = 0 );

  /**
   *  For setting dirichlet conditions.  Places unity on the diagonal
   *  entry of this row and zeros elsewhere.
   */
  void unit_diagonal_zero_else( const int irow );

  /** add non-ghost elements of the local matrix to this LHS operator */
  void add_contribution( const SpatialOps::SpatialOperator & localMat,
			 const double scaleFac = 1.0 );

  /** add non-ghost elements of the local field to the diagonal of LHS operator */
  void add_contribution( const SpatialOps::SpatialField & localField,
			 const double scaleFac = 1.0 );


  /** add a constant to the diagonal of the matrix */
  void add_diag_contribution( double x );

        Epetra_CrsMatrix& epetra_mat()      { return A_; }
  const Epetra_CrsMatrix& epetra_mat() const{ return A_; }

  void Print( std::ostream& c ) const;

private:

  bool compatibility_check( const SpatialOps::SpatialField& f ) const;
  bool compatibility_check( const SpatialOps::SpatialOperator& op ) const;

  Epetra_CrsMatrix & A_;
  const std::vector<int> extent_;
  const int nrows_;
  const int ncols_;

  LHS(const LHS& );
  LHS();

  std::vector<double> rowDWork_;
  std::vector<int>    rowIWork_;
};

//====================================================================

typedef RHS SOLN;

/**
 *  @class  LinearSystem
 *  @author James C. Sutherland
 *  @date   December, 2006
 *
 *  Basic support for a linear system distributed in parallel.
 *
 *  LinearSystem objects are constructed via the LinSysFactory.
 */
class LinearSystem
{
  friend class LinSysFactory;

public:

  /** Zero the lhs and rhs */
  void reset();

  void solve();

        RHS & get_rhs()      { return rhs_; }
  const RHS & get_rhs() const{ return rhs_; }

        LHS & get_lhs()      { return *lhs_; }
  const LHS & get_lhs() const{ return *lhs_; }

  const SOLN & get_soln_field() const{ return solnFieldValues_; }
        SOLN & get_soln_field()      { return solnFieldValues_; }

  const Epetra_Vector& get_soln_field_epetra_vec() const{ return *x_; }

  void set_tolerance( const double tol ){ solverTolerance_=tol; }

  void set_maxiter( const int maxit ){ maxIterations_=maxit; }



  /**
   *  Dirichlet conditions are accomplished by zeroing all columns of
   *  the given row and placing a "1" on the diagonal. The rhs vector
   *  is also set to accomplish the desired solution value.
   *
   *  In the case where this linear system is being applied to a
   *  residual update, the lhs value should be zero, which is set as a
   *  default.
   */
  void set_dirichlet_condition( const int irow,
				const double rhsVal = 0 );


protected:


#ifdef HAVE_MPI

  LinearSystem( const std::vector<int> & dimExtent,
		MPI_Comm & comm );

  int LinearSystem::get_global_npts( const std::vector<int> & extent,
				     MPI_Comm & comm );

#else

  LinearSystem( const std::vector<int> & dimExtent );

  int LinearSystem::get_global_npts( const std::vector<int> & extent );

#endif

  ~LinearSystem();


  void imprint( const std::vector<int> &, const int );

  const std::vector<int> extent_;
  const int npts_;
  RHS   rhs_;
  LHS * lhs_;
  SOLN  solnFieldValues_;

  int maxIterations_;
  double solverTolerance_;

private:

  Epetra_CrsMatrix * A_;  // the distributed matrix
  Epetra_Vector    * b_;  // the distributed RHS vector
  Epetra_Vector    * x_;  // the distributed solution vector

  Epetra_Comm      * comm_;

  Epetra_LinearProblem * linProb_;
  AztecOO * aztec_;

};

//====================================================================

#endif
