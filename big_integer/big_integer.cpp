#include "big_integer.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <iostream>
#include <string>

int const kRank = 10;

BigInt::BigInt() { sign_ = true; }

BigInt::BigInt(int64_t val) {
  sign_ = val >= 0;
  if (!sign_ && (val != std::numeric_limits<int64_t>::min())) {
    val *= -1;
  } else if (val == std::numeric_limits<int64_t>::min()) {
    ++val;
    val *= -1;
    while (val != 0) {
      arr_.push_back(val % kRank);
      val /= kRank;
    }
    ++arr_[0];
  }
  if (val == 0) {
    arr_.push_back(0);
  }
  while (val != 0) {
    arr_.push_back(val % kRank);
    val /= kRank;
  }
}

BigInt::BigInt(std::string string) {
  sign_ = true;
  for (size_t i = string.size() - 1; i > 0; --i) {
    arr_.push_back(string[i] - '0');
  }
  if (string[0] == '-' && !((string.size() == 2) && (string[1] == '0'))) {
    sign_ = false;
  } else if (!((string.size() == 2) && (string[1] == '0'))) {
    arr_.push_back(string[0] - '0');
  }
}

BigInt::BigInt(const BigInt& other) {
  arr_ = other.arr_;
  sign_ = other.sign_;
}

BigInt& BigInt::operator=(const BigInt& other) {
  arr_ = other.arr_;
  sign_ = other.sign_;
  return *this;
}

BigInt::~BigInt() {}

size_t BigInt::Size() const { return arr_.size(); }

bool BigInt::operator>=(const BigInt& other) const {
  if (sign_ != other.sign_) {
    return sign_;
  }
  if (Size() != other.Size()) {
    return ((Size() > other.Size()) ^ sign_) == 0;
  }
  if (Size() == other.Size()) {
    for (size_t i = Size() - 1; i > 0; --i) {
      if ((sign_ && arr_[i] < other.arr_[i]) ||
          (!sign_ && arr_[i] > other.arr_[i])) {
        return false;
      }
      if ((sign_ && arr_[i] > other.arr_[i]) ||
          (!sign_ && arr_[i] < other.arr_[i])) {
        return true;
      }
    }
    if ((sign_ && arr_[0] < other.arr_[0]) ||
        (!sign_ && arr_[0] > other.arr_[0])) {
      return false;
    }
  }
  return true;
}

bool BigInt::operator<=(const BigInt& other) const {
  if (sign_ != other.sign_) {
    return !sign_;
  }
  if (Size() != other.Size()) {
    return ((Size() < other.Size()) ^ sign_) == 0;
  }
  if (Size() == other.Size()) {
    for (size_t i = Size() - 1; i > 0; --i) {
      if ((sign_ && arr_[i] > other.arr_[i]) ||
          (!sign_ && arr_[i] < other.arr_[i])) {
        return false;
      }
      if ((sign_ && arr_[i] < other.arr_[i]) ||
          (!sign_ && arr_[i] > other.arr_[i])) {
        return true;
      }
    }
    if ((sign_ && arr_[0] > other.arr_[0]) ||
        (!sign_ && arr_[0] < other.arr_[0])) {
      return false;
    }
  }
  return true;
}

bool BigInt::operator==(const BigInt& other) const {
  return (*this >= other && *this <= other);
}

bool BigInt::operator!=(const BigInt& other) const {
  return !(*this >= other && *this <= other);
}

bool BigInt::operator<(const BigInt& other) const { return !(*this >= other); }

bool BigInt::operator>(const BigInt& other) const { return !(*this <= other); }

BigInt& BigInt::operator+=(const BigInt& other) {
  if (sign_ == other.sign_) {
    UnsignedPlus(other);
  } else {
    SignedPlus(other);
  }
  ZiroChecker(*this);
  return *this;
}

BigInt BigInt::operator+(const BigInt& other) const {
  BigInt copy = *this;
  copy += other;
  return copy;
}

BigInt& BigInt::operator-=(const BigInt& other) {
  sign_ = !sign_;
  *this += other;
  sign_ = !sign_;
  if ((Size() == 1) && (arr_[0] == 0)) {
    sign_ = true;
  }
  return *this;
}

BigInt BigInt::operator-(const BigInt& other) const {
  BigInt copy = *this;
  copy -= other;
  return copy;
}

void BigInt::UnsignedPlus(const BigInt& other) {
  int overflow = 0;
  int buf = 0;
  for (size_t i = 0; i < std::max(Size(), other.Size()); ++i) {
    if (i < Size() && i < other.Size()) {
      buf = arr_[i];
      arr_[i] = (arr_[i] + other.arr_[i] + overflow) % kRank;
      overflow = (buf + other.arr_[i] + overflow) / kRank;
    } else if (i < other.Size()) {
      arr_.push_back((other.arr_[i] + overflow) % kRank);
      overflow = (other.arr_[i] + overflow) / kRank;
    } else if (i < Size()) {
      buf = arr_[i];
      arr_[i] = (arr_[i] + overflow) % kRank;
      overflow = (buf + overflow) / kRank;
    }
  }
  if (overflow != 0) {
    arr_.push_back(overflow);
  }
}

bool BigInt::ModuleMore(BigInt& obj, const BigInt& other) {
  bool more = true;
  if (obj.sign_) {
    obj.sign_ = false;
    more = (obj <= other);
    obj.sign_ = true;
  } else {
    obj.sign_ = true;
    more = (obj >= other);
    obj.sign_ = false;
  }
  return more;
}

void BigInt::SignedPlusMore(const BigInt& other) {
  int overflow = 0;
  int buf = 0;
  for (size_t i = 0; i < Size(); ++i) {
    buf = arr_[i];
    if (i < other.Size()) {
      arr_[i] = (arr_[i] - other.arr_[i] - overflow + kRank) % kRank;
      overflow = (other.arr_[i] + overflow - buf > 0) ? 1 : 0;
    } else {
      arr_[i] = (arr_[i] - overflow + kRank) % kRank;
      overflow = (overflow - buf > 0) ? 1 : 0;
    }
  }
}

void BigInt::SignedPlusLess(const BigInt& other) {
  int overflow = 0;
  int buf = 0;
  for (size_t i = 0; i < other.Size(); ++i) {
    if (i < Size()) {
      buf = arr_[i];
      arr_[i] = (kRank - arr_[i] + other.arr_[i] - overflow) % kRank;
      overflow = (buf - other.arr_[i] + overflow > 0) ? 1 : 0;
    } else {
      arr_.push_back((kRank + other.arr_[i] - overflow) % kRank);
      overflow = (-other.arr_[i] + overflow > 0) ? 1 : 0;
    }
  }
}

void BigInt::SignedPlus(const BigInt& other) {
  bool more = ModuleMore(*this, other);
  if (more) {
    SignedPlusMore(other);
  } else {
    sign_ = !sign_;
    SignedPlusLess(other);
  }
}

std::istream& operator>>(std::istream& is, BigInt& obj) {
  std::string newstr;
  is >> newstr;
  obj += newstr;
  return is;
}

std::ostream& operator<<(std::ostream& os, const BigInt& obj) {
  if (!obj.sign_) {
    os << '-';
  }
  for (size_t i = obj.Size() - 1; i > 0; --i) {
    os << obj.arr_[i];
  }
  os << obj.arr_[0];
  return os;
}

void BigInt::ZiroChecker(BigInt& obj) {
  for (size_t i = Size() - 1; i > 0; --i) {
    if ((arr_[i] != 0)) {
      obj.arr_.resize(i + 1);
      break;
    }
    if (i - 1 == 0) {
      obj.arr_.resize(1);
      break;
    }
  }
  if ((obj.arr_.size() == 1) && (obj.arr_[0] == 0)) {
    obj.sign_ = true;
  }
}

BigInt& BigInt::operator++() {
  *this += 1;
  return *this;
}

BigInt BigInt::operator++(int) {
  BigInt copy = *this;
  ++(*this);
  return copy;
}

BigInt& BigInt::operator--() {
  *this += -1;
  return *this;
}

BigInt BigInt::operator--(int) {
  BigInt copy = *this;
  --(*this);
  return copy;
}

BigInt BigInt::operator-() {
  BigInt copy = *this;
  copy.sign_ = !(sign_);
  ZiroChecker(copy);
  return copy;
}

BigInt& BigInt::operator*=(const BigInt& other) {
  BigInt res;
  BigInt step;
  for (size_t i = 0; i < other.Size(); ++i) {
    step = MultDigit(other.arr_[i], i);
    res += step;
  }
  bool sign = (sign_ ^ other.sign_) == 0;
  *this = res;
  sign_ = sign;
  ZiroChecker(*this);
  return *this;
}

BigInt BigInt::MultDigit(int digit, int shift) {
  BigInt copy;
  copy.arr_.resize(Size() + shift);
  int overflow = 0;
  int buf = 0;
  for (size_t i = 0; i < Size(); ++i) {
    copy.arr_[i + shift] = (arr_[i] * digit + overflow) % kRank;
    overflow = (arr_[i] * digit + overflow) / kRank;
  }
  if (overflow != 0) {
    copy.arr_.push_back(overflow);
  }
  return copy;
}

BigInt BigInt::operator*(const BigInt& other) const {
  BigInt copy = *this;
  copy *= other;
  return copy;
}

BigInt& BigInt::operator/=(const BigInt& other) {
  if ((other.Size() == 1) && (other.arr_[0] == 1) && (other.sign_)) {
    return *this;
  }
  if ((other.Size() == 1) && (other.arr_[0] == 0)) {
    return *this;
  }
  if (sign_ == other.sign_) {
    UnsignedDiv(other);
  } else {
    SignedDiv(other);
  }
  ZiroChecker(*this);
  return *this;
}

BigInt Pow(int pow) {
  BigInt res = 1;
  for (int i = 0; i < pow; ++i) {
    res *= kRank;
  }
  return res;
}

void BigInt::UnsignedDiv(const BigInt& other) {
  BigInt res = *this;
  BigInt ans;
  BigInt one = 1;
  ans.arr_.resize(Size());
  int diff = res.Size() - other.Size();
  while (!(res >= other) ^ sign_) {
    BigInt pow = Pow(diff);
    while ((res - (other * pow) >= 0) ^ !sign_) {
      ans += (one * pow);
      res -= (other * pow);
    }
    diff -= 1;
  }
  *this = ans;
}

void BigInt::SignedDiv(const BigInt& other) {
  BigInt res = -*this;
  BigInt ans;
  BigInt one = 1;
  ans.arr_.resize(Size());
  int diff = res.Size() - other.Size();
  while ((res >= other) ^ sign_) {
    BigInt pow = Pow(diff);
    while ((res - (other * pow) >= 0) ^ sign_) {
      ans += (one * pow);
      res -= (other * pow);
    }
    diff -= 1;
  }
  *this = -ans;
}

BigInt BigInt::operator/(const BigInt& other) const {
  BigInt copy = *this;
  copy /= other;
  return copy;
}

BigInt BigInt::operator%(const BigInt& other) const {
  BigInt res = *this - (*this / other) * other;
  return res;
}