#pragma once

#include <algorithm>
#include <iostream>
#include <vector>

template <size_t N, size_t M, typename T = int64_t>
class Matrix {
 public:
  Matrix();
  Matrix(std::vector<std::vector<T>> vec);
  Matrix(T elem);

  Matrix& operator+=(const Matrix& obj);
  Matrix operator+(const Matrix& obj) const;
  Matrix& operator-=(const Matrix& obj);
  Matrix operator-(const Matrix& obj) const;
  Matrix& operator*=(T elem);
  Matrix operator*(T elem) const;
  Matrix<M, N, T> Transposed() const;
  bool operator==(const Matrix& obj);
  T& operator()(size_t i, size_t j);
  const T& operator()(size_t i, size_t j) const;

  template <size_t K>
  Matrix<N, K, T> operator*(const Matrix<M, K, T>& obj) const;

  T Trace();
  T TraceCounter(Matrix<N, N, T> obj);

 private:
  std::vector<std::vector<T>> arr_;
};

template <size_t N, size_t M, typename T>
T Matrix<N, M, T>::Trace() {
  return TraceCounter(*this);
}

template <size_t N, size_t M, typename T>
T Matrix<N, M, T>::TraceCounter(Matrix<N, N, T> obj) {
  T res;
  for (size_t i = 0; i < N; ++i) {
    res += obj.arr_[i][i];
  }
  return res;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix() {
  arr_.resize(N, std::vector<T>(M));
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix(std::vector<std::vector<T>> vec) {
  arr_ = vec;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix(T elem) {
  arr_.resize(N, std::vector<T>(M));
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      arr_[i][j] = elem;
    }
  }
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator+=(const Matrix<N, M, T>& obj) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      arr_[i][j] += obj.arr_[i][j];
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator-=(const Matrix<N, M, T>& obj) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      arr_[i][j] -= obj.arr_[i][j];
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator+(const Matrix<N, M, T>& obj) const {
  Matrix<N, M, T> copy = *this;
  copy += obj;
  return copy;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator-(const Matrix<N, M, T>& obj) const {
  Matrix<N, M, T> copy = *this;
  copy -= obj;
  return copy;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator*=(const T kElem) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      arr_[i][j] *= kElem;
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator*(const T kElem) const {
  Matrix<N, M, T> copy = *this;
  copy *= kElem;
  return copy;
}

template <size_t N, size_t M, typename T>
Matrix<M, N, T> Matrix<N, M, T>::Transposed() const {
  std::vector<std::vector<T>> vec(M, std::vector<T>(N));
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      vec[j][i] = arr_[i][j];
    }
  }
  Matrix<M, N, T> newmatrix = vec;
  return newmatrix;
}

template <size_t N, size_t M, typename T>
bool Matrix<N, M, T>::operator==(const Matrix<N, M, T>& obj) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      if (arr_[i][j] != obj.arr_[i][j]) {
        return false;
      }
    }
  }
  return true;
}

template <size_t N, size_t M, typename T>
const T& Matrix<N, M, T>::operator()(const size_t kI, const size_t kJ) const {
  return arr_[kI][kJ];
}

template <size_t N, size_t M, typename T>
T& Matrix<N, M, T>::operator()(const size_t kI, const size_t kJ) {
  return arr_[kI][kJ];
}

template <size_t N, size_t M, typename T>
template <size_t K>
Matrix<N, K, T> Matrix<N, M, T>::operator*(const Matrix<M, K, T>& obj) const {
  Matrix<N, K, T> res;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < K; ++j) {
      for (size_t k = 0; k < M; ++k) {
        res(i, j) += (*this)(i, k) * obj(k, j);
      }
    }
  }
  return res;
}