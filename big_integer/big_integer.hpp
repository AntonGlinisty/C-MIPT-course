#pragma once
#include <cstdint>
#include <string>
#include <vector>

class BigInt {
 public:
  BigInt();
  BigInt(int64_t val);
  BigInt(std::string string);
  BigInt(const BigInt& other);
  BigInt& operator=(const BigInt& other);
  ~BigInt();

  bool operator==(const BigInt& other) const;
  bool operator<(const BigInt& other) const;
  bool operator>(const BigInt& other) const;
  bool operator!=(const BigInt& other) const;
  bool operator<=(const BigInt& other) const;
  bool operator>=(const BigInt& other) const;
  static bool ModuleMore(BigInt& obj, const BigInt& other);

  BigInt& operator+=(const BigInt& other);
  BigInt operator+(const BigInt& other) const;
  BigInt& operator-=(const BigInt& other);
  BigInt operator-(const BigInt& other) const;

  void UnsignedPlus(const BigInt& other);
  void SignedPlus(const BigInt& other);
  void SignedPlusMore(const BigInt& other);
  void SignedPlusLess(const BigInt& other);

  size_t Size() const;

  friend std::istream& operator>>(std::istream& is, BigInt& obj);
  friend std::ostream& operator<<(std::ostream& os, const BigInt& obj);

  BigInt& operator++();
  BigInt operator++(int);
  BigInt& operator--();
  BigInt operator--(int);
  BigInt operator-();

  BigInt& operator*=(const BigInt& other);
  BigInt MultDigit(int digit, int shift);
  BigInt operator*(const BigInt& other) const;
  BigInt& operator/=(const BigInt& other);
  void SignedDiv(const BigInt& other);
  void UnsignedDiv(const BigInt& other);
  BigInt operator/(const BigInt& other) const;
  BigInt operator%(const BigInt& other) const;

  void ZiroChecker(BigInt& obj);
  void ZiroCutter(BigInt& obj);

 private:
  std::vector<int> arr_;
  bool sign_;
};