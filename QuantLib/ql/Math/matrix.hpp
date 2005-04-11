/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003, 2004 Ferdinando Ametrano
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file matrix.hpp
    \brief matrix used in linear algebra.
*/

#ifndef quantlib_matrix_hpp
#define quantlib_matrix_hpp

#include <ql/Math/array.hpp>
#include <ql/Utilities/steppingiterator.hpp>

namespace QuantLib {

    //! %Matrix used in linear algebra.
    /*! This class implements the concept of Matrix as used in linear
        algebra. As such, it is <b>not</b> meant to be used as a
        container.
    */
    class Matrix {
      public:
        //! \name Constructors, destructor, and assignment
        //@{
        //! creates a null matrix
        Matrix();
        //! creates a matrix with the given dimensions
        Matrix(Size rows, Size columns);
        //! creates the matrix and fills it with <tt>value</tt>
        Matrix(Size rows, Size columns, Real value);
        Matrix(const Matrix&);
        Matrix(const Disposable<Matrix>&);
        ~Matrix();
        Matrix& operator=(const Matrix&);
        Matrix& operator=(const Disposable<Matrix>&);
        //@}

        //! \name Algebraic operators
        /*! \pre all matrices involved in an algebraic expression must have
                 the same size.
        */
        //@{
        const Matrix& operator+=(const Matrix&);
        const Matrix& operator-=(const Matrix&);
        const Matrix& operator*=(Real);
        const Matrix& operator/=(Real);
        //@}

        typedef Real* iterator;
        typedef const Real* const_iterator;
        typedef boost::reverse_iterator<iterator> reverse_iterator;
        typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef Real* row_iterator;
        typedef const Real* const_row_iterator;
        typedef boost::reverse_iterator<row_iterator> reverse_row_iterator;
        typedef boost::reverse_iterator<const_row_iterator>
                                                const_reverse_row_iterator;
        typedef step_iterator<Real*> column_iterator;
        typedef step_iterator<const Real*> const_column_iterator;
        typedef boost::reverse_iterator<column_iterator>
                                                   reverse_column_iterator;
        typedef boost::reverse_iterator<const_column_iterator>
                                             const_reverse_column_iterator;
        //! \name Iterator access
        //@{
        const_iterator begin() const;
        iterator begin();
        const_iterator end() const;
        iterator end();
        const_reverse_iterator rbegin() const;
        reverse_iterator rbegin();
        const_reverse_iterator rend() const;
        reverse_iterator rend();
        const_row_iterator row_begin(Size i) const;
        row_iterator row_begin(Size i);
        const_row_iterator row_end(Size i) const;
        row_iterator row_end(Size i);
        const_reverse_row_iterator row_rbegin(Size i) const;
        reverse_row_iterator row_rbegin(Size i);
        const_reverse_row_iterator row_rend(Size i) const;
        reverse_row_iterator row_rend(Size i);
        const_column_iterator column_begin(Size i) const;
        column_iterator column_begin(Size i);
        const_column_iterator column_end(Size i) const;
        column_iterator column_end(Size i);
        const_reverse_column_iterator column_rbegin(Size i) const;
        reverse_column_iterator column_rbegin(Size i);
        const_reverse_column_iterator column_rend(Size i) const;
        reverse_column_iterator column_rend(Size i);
        //@}

        //! \name Element access
        //@{
        const_row_iterator operator[](Size) const;
        row_iterator operator[](Size);
        Disposable<Array> diagonal(void) const;
        //@}

        //! \name Inspectors
        //@{
        Size rows() const;
        Size columns() const;
        //@}

        //! \name Utilities
        //@{
        void swap(Matrix&);
        //@}
      private:
        void allocate(Size rows, Size columns);
        void copy(const Matrix&);
        Real* pointer_;
        Size rows_, columns_;
    };

    // algebraic operators

    /*! \relates Matrix */
    const Disposable<Matrix> operator+(const Matrix&, const Matrix&);
    /*! \relates Matrix */
    const Disposable<Matrix> operator-(const Matrix&, const Matrix&);
    /*! \relates Matrix */
    const Disposable<Matrix> operator*(const Matrix&, Real);
    /*! \relates Matrix */
    const Disposable<Matrix> operator*(Real, const Matrix&);
    /*! \relates Matrix */
    const Disposable<Matrix> operator/(const Matrix&, Real);


    // vectorial products

    /*! \relates Matrix */
    const Disposable<Array> operator*(const Array&, const Matrix&);
    /*! \relates Matrix */
    const Disposable<Array> operator*(const Matrix&, const Array&);
    /*! \relates Matrix */
    const Disposable<Matrix> operator*(const Matrix&, const Matrix&);

    // misc. operations

    /*! \relates Matrix */
    const Disposable<Matrix> transpose(const Matrix&);

    /*! \relates Matrix */
    const Disposable<Matrix> outerProduct(const Array& v1, const Array& v2);

    /*! \relates Matrix */
    template<class Iterator1, class Iterator2>
    const Disposable<Matrix> outerProduct(Iterator1 v1begin, Iterator1 v1end,
                                          Iterator2 v2begin, Iterator2 v2end);

    /*! \relates Matrix */
    std::ostream& operator<<(std::ostream&, const Matrix&);


    // inline definitions

    inline Matrix::Matrix()
    : pointer_(0), rows_(0), columns_(0) {}

    inline Matrix::Matrix(Size rows, Size columns)
    : pointer_(0), rows_(0), columns_(0) {
        if (rows > 0 && columns > 0)
            allocate(rows,columns);
    }

    inline Matrix::Matrix(Size rows,
                          Size columns,
                          Real value)
    : pointer_(0), rows_(0), columns_(0) {
        if (rows > 0 && columns > 0)
            allocate(rows,columns);
        std::fill(begin(),end(),value);
    }

    inline Matrix::Matrix(const Matrix& from)
    : pointer_(0), rows_(0), columns_(0) {
        allocate(from.rows(), from.columns());
        copy(from);
    }

    inline Matrix::Matrix(const Disposable<Matrix>& from)
    : pointer_(0), rows_(0), columns_(0) {
        swap(const_cast<Disposable<Matrix>&>(from));
    }

    inline Matrix::~Matrix() {
        if (pointer_ != 0 && rows_ != 0 && columns_ != 0)
            delete[] pointer_;
        pointer_ = 0;
        rows_ = columns_ = 0;
    }

    inline Matrix& Matrix::operator=(const Matrix& from) {
        if (this != &from) {
            allocate(from.rows(),from.columns());
            copy(from);
        }
        return *this;
    }

    inline Matrix& Matrix::operator=(const Disposable<Matrix>& from) {
        swap(const_cast<Disposable<Matrix>&>(from));
        return *this;
    }

    inline void Matrix::swap(Matrix& from) {
        std::swap(pointer_,from.pointer_);
        std::swap(rows_,from.rows_);
        std::swap(columns_,from.columns_);
    }

    inline void Matrix::allocate(Size rows, Size columns) {
        if (rows_ == rows && columns_ == columns)
            return;
        if (pointer_ != 0 && rows_ != 0 && columns_ != 0)
            delete[] pointer_;
        if (rows == 0 || columns == 0) {
            pointer_ = 0;
            rows_ = columns_ = 0;
        } else {
            pointer_ = new Real[rows*columns];
            rows_ = rows;
            columns_ = columns;
        }
    }

    inline void Matrix::copy(const Matrix& from) {
        std::copy(from.begin(),from.end(),begin());
    }

    inline const Matrix& Matrix::operator+=(const Matrix& m) {
        QL_REQUIRE(rows_ == m.rows_ && columns_ == m.columns_,
                   "matrices with different sizes cannot be added");
        std::transform(begin(),end(),m.begin(),
                       begin(),std::plus<Real>());
        return *this;
    }

    inline const Matrix& Matrix::operator-=(const Matrix& m) {
        QL_REQUIRE(rows_ == m.rows_ && columns_ == m.columns_,
                   "matrices with different sizes cannot be subtracted");
        std::transform(begin(),end(),m.begin(),begin(),
                       std::minus<Real>());
        return *this;
    }

    inline const Matrix& Matrix::operator*=(Real x) {
        std::transform(begin(),end(),begin(),
                       std::bind2nd(std::multiplies<Real>(),x));
        return *this;
    }

    inline const Matrix& Matrix::operator/=(Real x) {
        std::transform(begin(),end(),begin(),
                       std::bind2nd(std::divides<Real>(),x));
        return *this;
    }

    inline Matrix::const_iterator Matrix::begin() const {
        return pointer_;
    }

    inline Matrix::iterator Matrix::begin() {
        return pointer_;
    }

    inline Matrix::const_iterator Matrix::end() const {
        return pointer_+rows_*columns_;
    }

    inline Matrix::iterator Matrix::end() {
        return pointer_+rows_*columns_;
    }

    inline Matrix::const_reverse_iterator Matrix::rbegin() const {
        return const_reverse_iterator(end());
    }

    inline Matrix::reverse_iterator Matrix::rbegin() {
        return reverse_iterator(end());
    }

    inline Matrix::const_reverse_iterator Matrix::rend() const {
        return const_reverse_iterator(begin());
    }

    inline Matrix::reverse_iterator Matrix::rend() {
        return reverse_iterator(begin());
    }

    inline Matrix::const_row_iterator
    Matrix::row_begin(Size i) const {
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        QL_REQUIRE(i<rows_,
                   "matrix cannot be accessed out of range");
        #endif
        return pointer_+columns_*i;
    }

    inline Matrix::row_iterator Matrix::row_begin(Size i) {
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        QL_REQUIRE(i<rows_,
                   "matrix cannot be accessed out of range");
        #endif
        return pointer_+columns_*i;
    }

    inline Matrix::const_row_iterator Matrix::row_end(Size i) const{
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        QL_REQUIRE(i<rows_,
                   "matrix cannot be accessed out of range");
        #endif
        return pointer_+columns_*(i+1);
    }

    inline Matrix::row_iterator Matrix::row_end(Size i) {
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        QL_REQUIRE(i<rows_,
                   "matrix cannot be accessed out of range");
        #endif
        return pointer_+columns_*(i+1);
    }

    inline Matrix::const_reverse_row_iterator
    Matrix::row_rbegin(Size i) const {
        return const_reverse_row_iterator(row_end(i));
    }

    inline Matrix::reverse_row_iterator Matrix::row_rbegin(Size i) {
        return reverse_row_iterator(row_end(i));
    }

    inline Matrix::const_reverse_row_iterator
    Matrix::row_rend(Size i) const {
        return const_reverse_row_iterator(row_begin(i));
    }

    inline Matrix::reverse_row_iterator Matrix::row_rend(Size i) {
        return reverse_row_iterator(row_begin(i));
    }

    inline Matrix::const_column_iterator
    Matrix::column_begin(Size i) const {
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        QL_REQUIRE(i<columns_,
                   "matrix cannot be accessed out of range");
        #endif
        return const_column_iterator(pointer_+i,columns_);
    }

    inline Matrix::column_iterator Matrix::column_begin(Size i) {
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        QL_REQUIRE(i<columns_,
                   "matrix cannot be accessed out of range");
        #endif
        return column_iterator(pointer_+i,columns_);
    }

    inline Matrix::const_column_iterator
    Matrix::column_end(Size i) const {
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        QL_REQUIRE(i<columns_,
                   "matrix cannot be accessed out of range");
        #endif
        //return column_begin(i)+rows_;
        return const_column_iterator(pointer_+i+rows_*columns_,columns_);
    }

    inline Matrix::column_iterator Matrix::column_end(Size i) {
        #if defined(QL_EXTRA_SAFETY_CHECKS)
        QL_REQUIRE(i<columns_,
                   "matrix cannot be accessed out of range");
        #endif
        //return column_begin(i)+rows_;
        return column_iterator(pointer_+i+rows_*columns_,columns_);
    }

    inline Matrix::const_reverse_column_iterator
    Matrix::column_rbegin(Size i) const {
        return const_reverse_column_iterator(column_end(i));
    }

    inline Matrix::reverse_column_iterator
    Matrix::column_rbegin(Size i) {
        return reverse_column_iterator(column_end(i));
    }

    inline Matrix::const_reverse_column_iterator
    Matrix::column_rend(Size i) const {
        return const_reverse_column_iterator(column_begin(i));
    }

    inline Matrix::reverse_column_iterator
    Matrix::column_rend(Size i) {
        return reverse_column_iterator(column_begin(i));
    }

    inline Matrix::const_row_iterator
    Matrix::operator[](Size i) const {
        return row_begin(i);
    }

    inline Matrix::row_iterator Matrix::operator[](Size i) {
        return row_begin(i);
    }

    inline Disposable<Array> Matrix::diagonal(void) const{
        Size arraySize = std::min<Size>(rows(), columns());
        Array tmp(arraySize);
        for(Size i = 0; i < arraySize; i++)
            tmp[i] = (*this)[i][i];
        return tmp;
    }

    inline Size Matrix::rows() const {
        return rows_;
    }

    inline Size Matrix::columns() const {
        return columns_;
    }

    inline const Disposable<Matrix> operator+(const Matrix& m1,
                                              const Matrix& m2) {
        QL_REQUIRE(m1.rows() == m2.rows() &&
                   m1.columns() == m2.columns(),
                   "matrices with different sizes cannot be added");
        Matrix temp(m1.rows(),m1.columns());
        std::transform(m1.begin(),m1.end(),m2.begin(),temp.begin(),
                       std::plus<Real>());
        return temp;
    }

    inline const Disposable<Matrix> operator-(const Matrix& m1,
                                              const Matrix& m2) {
        QL_REQUIRE(m1.rows() == m2.rows() &&
                   m1.columns() == m2.columns(),
                   "matrices with different sizes cannot be subtracted");
        Matrix temp(m1.rows(),m1.columns());
        std::transform(m1.begin(),m1.end(),m2.begin(),temp.begin(),
                       std::minus<Real>());
        return temp;
    }

    inline const Disposable<Matrix> operator*(const Matrix& m, Real x) {
        Matrix temp(m.rows(),m.columns());
        std::transform(m.begin(),m.end(),temp.begin(),
                       std::bind2nd(std::multiplies<Real>(),x));
        return temp;
    }

    inline const Disposable<Matrix> operator*(Real x, const Matrix& m) {
        Matrix temp(m.rows(),m.columns());
        std::transform(m.begin(),m.end(),temp.begin(),
                       std::bind2nd(std::multiplies<Real>(),x));
        return temp;
    }

    inline const Disposable<Matrix> operator/(const Matrix& m, Real x) {
        Matrix temp(m.rows(),m.columns());
        std::transform(m.begin(),m.end(),temp.begin(),
                       std::bind2nd(std::divides<Real>(),x));
        return temp;
    }

    inline const Disposable<Array> operator*(const Array& v, const Matrix& m) {
        QL_REQUIRE(v.size() == m.rows(),
                   "vectors and matrices with different sizes "
                   "cannot be multiplied");
        Array result(m.columns());
        for (Size i=0; i<result.size(); i++)
            result[i] =
                std::inner_product(v.begin(),v.end(),
                                   m.column_begin(i),0.0);
        return result;
    }

    inline const Disposable<Array> operator*(const Matrix& m, const Array& v) {
        QL_REQUIRE(v.size() == m.columns(),
                   "vectors and matrices with different sizes "
                   "cannot be multiplied");
        Array result(m.rows());
        for (Size i=0; i<result.size(); i++)
            result[i] =
                std::inner_product(v.begin(),v.end(),m.row_begin(i),0.0);
        return result;
    }

    inline const Disposable<Matrix> operator*(const Matrix& m1,
                                              const Matrix& m2) {
        QL_REQUIRE(m1.columns() == m2.rows(),
                   "matrices with different sizes cannot be multiplied");
        Matrix result(m1.rows(),m2.columns());
        for (Size i=0; i<result.rows(); i++)
            for (Size j=0; j<result.columns(); j++)
                result[i][j] =
                    std::inner_product(m1.row_begin(i), m1.row_end(i),
                                       m2.column_begin(j), 0.0);
        return result;
    }

    inline const Disposable<Matrix> transpose(const Matrix& m) {
        Matrix result(m.columns(),m.rows());
        for (Size i=0; i<m.rows(); i++)
            std::copy(m.row_begin(i),m.row_end(i),result.column_begin(i));
        return result;
    }

    inline const Disposable<Matrix> outerProduct(const Array& v1,
                                                 const Array& v2) {
        return outerProduct(v1.begin(), v1.end(), v2.begin(), v2.end());
    }

    template<class Iterator1, class Iterator2>
    inline const Disposable<Matrix> outerProduct(Iterator1 v1begin,
                                                 Iterator1 v1end,
                                                 Iterator2 v2begin,
                                                 Iterator2 v2end) {

        Size size1 = std::distance(v1begin, v1end);
        QL_REQUIRE(size1>0, "null first vector");

        Size size2 = std::distance(v2begin, v2end);
        QL_REQUIRE(size2>0, "null second vector");

        Matrix result(size1, size2);

        for (Size i=0; v1begin!=v1end; i++, v1begin++)
            std::transform(v2begin, v2end, result.row_begin(i),
                           std::bind1st(std::multiplies<Real>(), *v1begin));

        return result;
    }

    inline std::ostream& operator<<(std::ostream& out, const Matrix& m) {
        std::streamsize width = out.width();
        for (Size i=0; i<m.rows(); i++) {
            out << "| ";
            for (Size j=0; j<m.columns(); j++)
                out << std::setw(width) << m[i][j] << " ";
            out << "|\n";
        }
        return out;
    }

}


#endif
