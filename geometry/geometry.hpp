#include <cstdint>
#include <iostream>

class Point;
class Segment;
class Line;
class Ray;
class Circle;

class Vector {
 public:
  Vector();
  Vector(int64_t xcomp, int64_t ycomp);
  Vector(const Point& first, const Point& second);
  int64_t operator*(const Vector& obj) const;
  int64_t operator^(const Vector& obj) const;
  Vector& operator*=(int64_t val);
  Vector& operator+=(const Vector& obj);
  Vector& operator-=(const Vector& obj);
  Vector operator*(int64_t val) const;
  Vector operator+(const Vector& obj) const;
  Vector operator-(const Vector& obj) const;

  friend Vector operator*=(int64_t val, Vector& obj);
  friend Vector operator*(int64_t val, const Vector& obj);

  Vector operator-() const;
  int64_t GetX() const;
  int64_t GetY() const;

 private:
  int64_t xcomp_;
  int64_t ycomp_;
};

class IShape {
 public:
  virtual void Move(const Vector& vec) = 0;
  virtual bool ContainsPoint(const Point& point) = 0;
  virtual bool CrossSegment(const Segment& seg) = 0;
  virtual IShape* Clone() = 0;
  virtual ~IShape() = default;
};

class Point : public IShape {
 public:
  Point();
  Point(int64_t xcord, int64_t ycord);
  int64_t GetX() const;
  int64_t GetY() const;
  void Move(const Vector& vec) override;
  bool ContainsPoint(const Point& point) override;
  bool CrossSegment(const Segment& seg) override;
  IShape* Clone() override;
  Vector operator-(const Point& obj) const;
  bool operator==(const Point& obj) const;

 private:
  int64_t xcord_;
  int64_t ycord_;
};

class Segment : public IShape {
 public:
  Segment();
  Segment(const Point& k_first, const Point& k_second);
  Point GetA() const;
  Point GetB() const;
  void Move(const Vector& vec) override;
  bool ContainsPoint(const Point& point) override;
  bool CrossSegment(const Segment& seg) override;
  IShape* Clone() override;

 private:
  Point first_;
  Point second_;
};

class Line : public IShape {
 public:
  Line();
  Line(const Point& k_first, const Point& k_second);
  int64_t GetA() const;
  int64_t GetB() const;
  int64_t GetC() const;
  void Move(const Vector& vec) override;
  bool ContainsPoint(const Point& point) override;
  bool CrossSegment(const Segment& seg) override;
  IShape* Clone() override;

 private:
  int64_t acoef_;
  int64_t bcoef_;
  int64_t ccoef_;
};

class Ray : public IShape {
 public:
  Ray();
  Ray(const Point& k_first, const Point& k_second);
  Point GetA() const;
  Vector GetVector() const;
  void Move(const Vector& vec) override;
  bool ContainsPoint(const Point& point) override;
  bool CrossSegment(const Segment& seg) override;
  IShape* Clone() override;

 private:
  Point start_;
  Vector direction_;
};

class Circle : public IShape {
 public:
  Circle();
  Circle(const Point& k_center, size_t radius);
  Point GetCentre() const;
  size_t GetRadius() const;
  void Move(const Vector& vec) override;
  bool ContainsPoint(const Point& point) override;
  bool CrossSegment(const Segment& seg) override;
  IShape* Clone() override;

 private:
  Point center_;
  size_t radius_;
};