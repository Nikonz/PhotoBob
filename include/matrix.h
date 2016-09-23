#pragma once

#include <initializer_list>
#include <algorithm>
#include <memory>
#include <iostream>
#include <string>
#include <type_traits>

typedef unsigned int uint;

template<typename ValueT>
class Matrix
{
public:
    const uint n_rows;
    const uint n_cols;

    Matrix(uint row_count=0, uint col_count=0);

    Matrix(std::initializer_list<ValueT>);

    Matrix(std::initializer_list<std::initializer_list<ValueT>>);

    // Shallow copy. Be careful, this function just copies the pointer
    Matrix(const Matrix&);
    // Deep copy. Allocates memory and copies all values
    Matrix<ValueT> deep_copy() const;

    const Matrix<ValueT> &operator = (const Matrix<ValueT> &);

    Matrix(Matrix&&);
    ~Matrix();

    const ValueT &operator() (uint row, uint col) const;
    ValueT &operator() (uint row, uint col);

    template<typename UnaryMatrixOperator>
    Matrix<typename std::result_of<UnaryMatrixOperator(Matrix<ValueT>)>::type>
    unary_map(UnaryMatrixOperator &op) const;

    template<typename BinaryMatrixOperator>
    friend Matrix<ValueT> binary_map(const BinaryMatrixOperator&,
                                     const Matrix<ValueT>&,
                                     const Matrix<ValueT>&);

    // Matrix<int> a = { {1, 2, 3}, {4, 5, 6} };
    // cout << a.submatrix(1, 1, 1, 2); // 5 6
    const Matrix<ValueT> submatrix(uint prow, uint pcol,
                                   uint rows, uint cols) const;

private:
    // Stride - number of elements between two rows (needed for efficient
    // submatrix function without memory copy)
    const uint stride;
    // First row and col, useful for taking submatrices. By default is (0, 0).
    const uint pin_row, pin_col;
    // shared_ptr still has no support of c-style arrays and
    // <type[]> partial specialization like unique_ptr has.
    // works: unique_ptr<int[]>; doesn't: shared_ptr<int[]>.
    // so, for now we use shared_ptr just for counting links,
    // and work with raw pointer through get().
    std::shared_ptr<ValueT> _data;

    // Const cast for writing public const fields.
    template<typename T> inline T& make_rw(const T& val) const;
};

// Output for matrix. Useful for debugging
template<typename ValueT>
std::ostream &operator << (std::ostream &out, const Matrix<ValueT> &m)
{
    if (m.n_rows * m.n_cols == 0) {
        out << "empty matrix" << std::endl;
        return out;
    }
    for (uint i = 0; i < m.n_rows; ++i) {
        for (uint j = 0; j < m.n_cols; ++j) {
            out << m(i, j) << " ";
        }
        out << std::endl;
    }
    return out;
}

// Implementation of Matrix class
#include "matrix.hpp"
