/* This file was generated by fulmar version 0.8.0. */

/*
 * Copyright (c) 2013 The University of Utah
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

#ifndef NEBO_ASSIGNMENT_H
#  define NEBO_ASSIGNMENT_H

   namespace SpatialOps {
      template<typename FieldType>
       inline FieldType const & operator <<=(FieldType & lhs,
                                             typename FieldType::value_type
                                             const & rhs) {
          NeboScalar<Initial, FieldType> typedef RhsType;

          NeboField<Initial, FieldType>(lhs).template assign<All, RhsType>(RhsType(rhs));

          return lhs;
       };

      template<typename FieldType>
       inline FieldType const & operator <<=(FieldType & lhs,
                                             FieldType const & rhs) {
          NeboConstField<Initial, FieldType> typedef RhsType;

          NeboField<Initial, FieldType>(lhs).template assign<All, RhsType>(RhsType(rhs));

          return lhs;
       };

      template<typename RhsType, typename FieldType>
       inline FieldType const & operator <<=(FieldType & lhs,
                                             NeboExpression<RhsType, FieldType>
                                             const & rhs) {
          NeboField<Initial, FieldType>(lhs).template assign<All, RhsType>(rhs.expr());

          return lhs;
       };

      template<typename FieldType>
       inline FieldType const & interior_assign(FieldType & lhs,
                                                typename FieldType::value_type
                                                const & rhs) {
          NeboScalar<Initial, FieldType> typedef RhsType;

          NeboField<Initial, FieldType>(lhs).template assign<InteriorOnly,
                                                             RhsType>(RhsType(rhs));

          return lhs;
       };

      template<typename FieldType>
       inline FieldType const & interior_assign(FieldType & lhs,
                                                FieldType const & rhs) {
          NeboConstField<Initial, FieldType> typedef RhsType;

          NeboField<Initial, FieldType>(lhs).template assign<InteriorOnly,
                                                             RhsType>(RhsType(rhs));

          return lhs;
       };

      template<typename RhsType, typename FieldType>
       inline FieldType const & interior_assign(FieldType & lhs,
                                                NeboExpression<RhsType,
                                                               FieldType> const
                                                & rhs) {
          NeboField<Initial, FieldType>(lhs).template assign<InteriorOnly,
                                                             RhsType>(rhs.expr());

          return lhs;
       };
   } /* SpatialOps */

#endif
/* NEBO_ASSIGNMENT_H */
