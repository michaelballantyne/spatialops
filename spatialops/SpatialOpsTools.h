/*
 * Copyright (c) 2011 The University of Utah
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef SpatialOpsTools_h
#define SpatialOpsTools_h

#include <spatialops/SpatialOpsConfigure.h>

/**
 *  \file SpatialOpsTools.h
 */

namespace SpatialOps{

  /**
   * \struct IsSameType
   * \brief Compares two types for equality
   *
   * Examples:
   * \code assert( IsSameType<int,double>::result == 0 ); \endcode
   * \code assert( IsSameType<int,int>::result == 1 ); \endcode
   */
  template< typename T1, typename T2> struct IsSameType       { enum{ result=0 }; };
  template< typename T1             > struct IsSameType<T1,T1>{ enum{ result=1 }; };

}

#endif

