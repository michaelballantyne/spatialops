/* This file was generated by fulmar version 0.9.0. */

/*
 * Copyright (c) 2014 The University of Utah
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
   #define NEBO_ASSIGNMENT_H

   namespace SpatialOps {
      template<typename FieldType>
       inline FieldType const & operator <<=(FieldType & lhs,
                                             typename FieldType::value_type
                                             const & rhs) {
          NeboScalar<Initial, typename FieldType::value_type> typedef RhsType;

          NeboField<Initial, FieldType>(lhs).template assign<RhsType>(true,
                                                                      RhsType(rhs));

          return lhs;
       };

      template<typename FieldType>
       inline FieldType const & operator <<=(FieldType & lhs,
                                             FieldType const & rhs) {
          NeboConstField<Initial, FieldType> typedef RhsType;

          NeboField<Initial, FieldType>(lhs).template assign<RhsType>(true,
                                                                      RhsType(rhs));

          return lhs;
       };

      template<typename RhsType, typename FieldType>
       inline FieldType const & operator <<=(FieldType & lhs,
                                             NeboExpression<RhsType, FieldType>
                                             const & rhs) {
          NeboField<Initial, FieldType>(lhs).template assign<RhsType>(true, rhs.expr());

          return lhs;
       };

      template<typename FieldType>
       inline FieldType const & operator <<=(FieldType & lhs,
                                             SpatialOps::structured::
                                             SpatialField<SpatialOps::structured::
                                                          SingleValue,
                                                          typename FieldType::
                                                          value_type> const &
                                             rhs) {
          NeboConstSingleValueField<Initial, typename FieldType::value_type>
          typedef RhsType;

          NeboField<Initial, FieldType>(lhs).template assign<RhsType>(true,
                                                                      RhsType(rhs));

          return lhs;
       };

      template<typename RhsType, typename FieldType>
       inline FieldType const & operator <<=(FieldType & lhs,
                                             NeboSingleValueExpression<RhsType,
                                                                       typename
                                                                       FieldType::
                                                                       value_type>
                                             const & rhs) {
          NeboField<Initial, FieldType>(lhs).template assign<RhsType>(true, rhs.expr());

          return lhs;
       };

      template<typename T>
       inline SpatialOps::structured::SpatialField<SpatialOps::structured::
                                                   SingleValue,
                                                   T> const & operator <<=(SpatialOps::
                                                                           structured::
                                                                           SpatialField<SpatialOps::
                                                                                        structured::
                                                                                        SingleValue,
                                                                                        T>
                                                                           & lhs,
                                                                           typename
                                                                           SpatialOps::
                                                                           structured::
                                                                           SpatialField<SpatialOps::
                                                                                        structured::
                                                                                        SingleValue,
                                                                                        T>::
                                                                           value_type
                                                                           const
                                                                           & rhs) {
          NeboScalar<Initial,
                     typename SpatialOps::structured::SpatialField<SpatialOps::
                                                                   structured::
                                                                   SingleValue,
                                                                   T>::
                     value_type> typedef RhsType;

          NeboField<Initial,
                    SpatialOps::structured::SpatialField<SpatialOps::structured::
                                                         SingleValue,
                                                         T> >(lhs).template
                                                                   assign<RhsType>(true,
                                                                                   RhsType(rhs));

          return lhs;
       };

      template<typename T>
       inline SpatialOps::structured::SpatialField<SpatialOps::structured::
                                                   SingleValue,
                                                   T> const & operator <<=(SpatialOps::
                                                                           structured::
                                                                           SpatialField<SpatialOps::
                                                                                        structured::
                                                                                        SingleValue,
                                                                                        T>
                                                                           & lhs,
                                                                           SpatialOps::
                                                                           structured::
                                                                           SpatialField<SpatialOps::
                                                                                        structured::
                                                                                        SingleValue,
                                                                                        T>
                                                                           const
                                                                           & rhs) {
          NeboConstSingleValueField<Initial, T> typedef RhsType;

          NeboField<Initial,
                    SpatialOps::structured::SpatialField<SpatialOps::structured::
                                                         SingleValue,
                                                         T> >(lhs).template
                                                                   assign<RhsType>(true,
                                                                                   RhsType(rhs));

          return lhs;
       };

      template<typename RhsType, typename T>
       inline SpatialOps::structured::SpatialField<SpatialOps::structured::
                                                   SingleValue,
                                                   T> const & operator <<=(SpatialOps::
                                                                           structured::
                                                                           SpatialField<SpatialOps::
                                                                                        structured::
                                                                                        SingleValue,
                                                                                        T>
                                                                           & lhs,
                                                                           NeboSingleValueExpression<RhsType,
                                                                                                     T>
                                                                           const
                                                                           & rhs) {
          NeboField<Initial,
                    SpatialOps::structured::SpatialField<SpatialOps::structured::
                                                         SingleValue,
                                                         T> >(lhs).template
                                                                   assign<RhsType>(true,
                                                                                   rhs.expr());

          return lhs;
       };

      template<typename FieldType>
       inline FieldType const & interior_assign(FieldType & lhs,
                                                typename FieldType::value_type
                                                const & rhs) {
          NeboScalar<Initial, typename FieldType::value_type> typedef RhsType;

          NeboField<Initial, FieldType>(lhs).template assign<RhsType>(false,
                                                                      RhsType(rhs));

          return lhs;
       };

      template<typename FieldType>
       inline FieldType const & interior_assign(FieldType & lhs,
                                                FieldType const & rhs) {
          NeboConstField<Initial, FieldType> typedef RhsType;

          NeboField<Initial, FieldType>(lhs).template assign<RhsType>(false,
                                                                      RhsType(rhs));

          return lhs;
       };

      template<typename RhsType, typename FieldType>
       inline FieldType const & interior_assign(FieldType & lhs,
                                                NeboExpression<RhsType,
                                                               FieldType> const
                                                & rhs) {
          NeboField<Initial, FieldType>(lhs).template assign<RhsType>(false, rhs.expr());

          return lhs;
       };

      template<typename FieldType>
       inline FieldType const & interior_assign(FieldType & lhs,
                                                SpatialOps::structured::
                                                SpatialField<SpatialOps::
                                                             structured::
                                                             SingleValue,
                                                             typename FieldType::
                                                             value_type> const &
                                                rhs) {
          NeboConstSingleValueField<Initial, typename FieldType::value_type>
          typedef RhsType;

          NeboField<Initial, FieldType>(lhs).template assign<RhsType>(false,
                                                                      RhsType(rhs));

          return lhs;
       };

      template<typename RhsType, typename FieldType>
       inline FieldType const & interior_assign(FieldType & lhs,
                                                NeboSingleValueExpression<RhsType,
                                                                          typename
                                                                          FieldType::
                                                                          value_type>
                                                const & rhs) {
          NeboField<Initial, FieldType>(lhs).template assign<RhsType>(false, rhs.expr());

          return lhs;
       };

      template<typename T>
       inline SpatialOps::structured::SpatialField<SpatialOps::structured::
                                                   SingleValue,
                                                   T> const & interior_assign(SpatialOps::
                                                                              structured::
                                                                              SpatialField<SpatialOps::
                                                                                           structured::
                                                                                           SingleValue,
                                                                                           T>
                                                                              &
                                                                              lhs,
                                                                              typename
                                                                              SpatialOps::
                                                                              structured::
                                                                              SpatialField<SpatialOps::
                                                                                           structured::
                                                                                           SingleValue,
                                                                                           T>::
                                                                              value_type
                                                                              const
                                                                              &
                                                                              rhs) {
          NeboScalar<Initial,
                     typename SpatialOps::structured::SpatialField<SpatialOps::
                                                                   structured::
                                                                   SingleValue,
                                                                   T>::
                     value_type> typedef RhsType;

          NeboField<Initial,
                    SpatialOps::structured::SpatialField<SpatialOps::structured::
                                                         SingleValue,
                                                         T> >(lhs).template
                                                                   assign<RhsType>(false,
                                                                                   RhsType(rhs));

          return lhs;
       };

      template<typename T>
       inline SpatialOps::structured::SpatialField<SpatialOps::structured::
                                                   SingleValue,
                                                   T> const & interior_assign(SpatialOps::
                                                                              structured::
                                                                              SpatialField<SpatialOps::
                                                                                           structured::
                                                                                           SingleValue,
                                                                                           T>
                                                                              &
                                                                              lhs,
                                                                              SpatialOps::
                                                                              structured::
                                                                              SpatialField<SpatialOps::
                                                                                           structured::
                                                                                           SingleValue,
                                                                                           T>
                                                                              const
                                                                              &
                                                                              rhs) {
          NeboConstSingleValueField<Initial, T> typedef RhsType;

          NeboField<Initial,
                    SpatialOps::structured::SpatialField<SpatialOps::structured::
                                                         SingleValue,
                                                         T> >(lhs).template
                                                                   assign<RhsType>(false,
                                                                                   RhsType(rhs));

          return lhs;
       };

      template<typename RhsType, typename T>
       inline SpatialOps::structured::SpatialField<SpatialOps::structured::
                                                   SingleValue,
                                                   T> const & interior_assign(SpatialOps::
                                                                              structured::
                                                                              SpatialField<SpatialOps::
                                                                                           structured::
                                                                                           SingleValue,
                                                                                           T>
                                                                              &
                                                                              lhs,
                                                                              NeboSingleValueExpression<RhsType,
                                                                                                        T>
                                                                              const
                                                                              &
                                                                              rhs) {
          NeboField<Initial,
                    SpatialOps::structured::SpatialField<SpatialOps::structured::
                                                         SingleValue,
                                                         T> >(lhs).template
                                                                   assign<RhsType>(false,
                                                                                   rhs.expr());

          return lhs;
       };
   } /* SpatialOps */

#endif
/* NEBO_ASSIGNMENT_H */
