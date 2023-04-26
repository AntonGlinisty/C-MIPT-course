#include "geometry.hpp"

Vector::Vector() : xcomp_(0), ycomp_(0) {}

Vector::Vector(const int64_t kXcomp, const int64_t kYcomp)
    : xcomp_(kXcomp), ycomp_(kYcomp) {}

Vector::Vector(const Point& first, const Point& second) {
  xcomp_ = second.GetX() - first.GetX();
  ycomp_ = second.GetY() - first.GetY();
}

int64_t Vector::operator*(const Vector& obj) const {
  return xcomp_ * obj.xcomp_ + ycomp_ * obj.ycomp_;
}

int64_t Vector::operator^(const Vector& obj) const {
  return xcomp_ * obj.ycomp_ - ycomp_ * obj.xcomp_;
}

Vector& Vector::operator+=(const Vector& obj) {
  xcomp_ += obj.xcomp_;
  ycomp_ += obj.ycomp_;
  return *this;
}

Vector& Vector::operator-=(const Vector& obj) {
  xcomp_ -= obj.xcomp_;
  ycomp_ -= obj.ycomp_;
  return *this;
}

Vector Vector::operator+(const Vector& obj) const {
  Vector copy = *this;
  copy += obj;
  return copy;
}

Vector Vector::operator-(const Vector& obj) const {
  Vector copy = *this;
  copy -= obj;
  return copy;
}

Vector& Vector::operator*=(const int64_t kVal) {
  xcomp_ *= kVal;
  ycomp_ *= kVal;
  return *this;
}

Vector Vector::operator*(const int64_t kVal) const {
  Vector copy = *this;
  copy *= kVal;
  return copy;
}

Vector operator*=(const int64_t kVal, Vector& obj) {
  obj.xcomp_ *= kVal;
  obj.ycomp_ *= kVal;
  return obj;
}

Vector operator*(const int64_t kVal, const Vector& obj) {
  Vector copy = obj;
  kVal *= copy;
  return copy;
}

Vector Vector::operator-() const {
  Vector copy = *this;
  copy *= -1;
  return copy;
}

int64_t Vector::GetX() const { return xcomp_; }

int64_t Vector::GetY() const { return ycomp_; }

Point::Point() : xcord_(0), ycord_(0) {}

Point::Point(const int64_t kXcord, const int64_t kYcord)
    : xcord_(kXcord), ycord_(kYcord) {}

int64_t Point::GetX() const { return xcord_; }

int64_t Point::GetY() const { return ycord_; }

void Point::Move(const Vector& vec) {
  xcord_ += vec.GetX();
  ycord_ += vec.GetY();
}

bool Point::operator==(const Point& obj) const {
  return ((xcord_ == obj.xcord_) && (ycord_ == obj.ycord_));
}

bool Point::ContainsPoint(const Point& point) { return *this == point; }

bool Point::CrossSegment(const Segment& seg) {
  Segment copy = seg;
  return copy.ContainsPoint(*this);
}

Vector Point::operator-(const Point& obj) const { return Vector(obj, *this); }

IShape* Point::Clone() {
  Point* obj = new Point(*this);
  return obj;
}

Segment::Segment() {
  first_ = Point();
  second_ = Point();
}

Segment::Segment(const Point& k_first, const Point& k_second)
    : first_(k_first), second_(k_second) {}

Point Segment::GetA() const { return first_; }

Point Segment::GetB() const { return second_; }

void Segment::Move(const Vector& vec) {
  first_.Move(vec);
  second_.Move(vec);
}

bool Segment::ContainsPoint(const Point& point) {
  bool xstate = (std::min(first_.GetX(), second_.GetX()) <= point.GetX()) &&
                (point.GetX() <= std::max(first_.GetX(), second_.GetX()));
  bool ystate = (std::min(first_.GetY(), second_.GetY()) <= point.GetY()) &&
                (point.GetY() <= std::max(first_.GetY(), second_.GetY()));
  return (((Vector(first_, second_) ^ Vector(point, second_)) == 0) && xstate &&
          ystate);
}

bool Segment::CrossSegment(const Segment& seg) {
  Vector thisvec(first_, second_);
  Vector segvec(seg.first_, seg.second_);
  int64_t thisprod = (thisvec ^ Vector(first_, seg.first_)) *
                     (thisvec ^ Vector(first_, seg.second_));
  int64_t segprod = (segvec ^ Vector(seg.first_, first_)) *
                    (segvec ^ Vector(seg.first_, second_));
  if ((thisprod == 0) && (segprod == 0)) {
    Segment copy = seg;
    return (ContainsPoint(seg.first_) || ContainsPoint(seg.second_) ||
            copy.ContainsPoint(first_) || copy.ContainsPoint(second_));
  }
  return (thisprod <= 0) && (segprod <= 0);
}

IShape* Segment::Clone() {
  Segment* obj = new Segment(*this);
  return obj;
}

Line::Line() : acoef_(0), bcoef_(0), ccoef_(0) {}

Line::Line(const Point& k_first, const Point& k_second) {
  acoef_ = k_second.GetY() - k_first.GetY();
  bcoef_ = k_first.GetX() - k_second.GetX();
  ccoef_ = k_first.GetY() * (k_second.GetX() - k_first.GetX()) +
           k_first.GetX() * (k_first.GetY() - k_second.GetY());
}

int64_t Line::GetA() const { return acoef_; }

int64_t Line::GetB() const { return bcoef_; }

int64_t Line::GetC() const { return ccoef_; }

void Line::Move(const Vector& vec) {
  Point first;
  Point second;
  if (acoef_ == 0) {
    first = Point(0, (-ccoef_) / bcoef_);
    second = Point(1, (-ccoef_) / bcoef_);
  } else {
    first = Point((-ccoef_) / acoef_, 0);
    second = Point((-ccoef_ - bcoef_) / acoef_, 1);
  }
  first.Move(vec);
  second.Move(vec);
  *this = Line(first, second);
}

bool Line::ContainsPoint(const Point& point) {
  return (acoef_ * point.GetX() + bcoef_ * point.GetY() + ccoef_ == 0);
}

bool Line::CrossSegment(const Segment& seg) {
  Point first;
  Point second;
  if (acoef_ == 0) {
    first = Point(0, (-ccoef_) / bcoef_);
    second = Point(1, (-ccoef_) / bcoef_);
  } else {
    first = Point((-ccoef_) / acoef_, 0);
    second = Point((-ccoef_ - bcoef_) / acoef_, 1);
  }
  return ((Vector(first, second) ^
           Vector(first, seg.GetA()) *
               (Vector(first, second) ^ Vector(first, seg.GetB()))) <= 0);
}

IShape* Line::Clone() {
  Line* obj = new Line(*this);
  return obj;
}

Ray::Ray() {
  start_ = Point();
  direction_ = Vector();
}

Ray::Ray(const Point& k_first, const Point& k_second) {
  start_ = k_first;
  direction_ = Vector(k_first, k_second);
}

Point Ray::GetA() const { return start_; }

Vector Ray::GetVector() const { return direction_; }

void Ray::Move(const Vector& vec) { start_.Move(vec); }

bool Ray::ContainsPoint(const Point& point) {
  Point second = start_;
  second.Move(direction_);
  Point newstart = Point(0, 0);
  Point newsecond =
      Point(second.GetX() - start_.GetX(), second.GetY() - start_.GetY());
  Point newpoint =
      Point(point.GetX() - start_.GetX(), point.GetY() - start_.GetY());
  return ((newsecond.GetX() * newpoint.GetY() ==
           newsecond.GetY() * newpoint.GetX()) &&
          (newsecond.GetX() * newpoint.GetX() >= 0) &&
          (newsecond.GetY() * newpoint.GetY() >= 0));
}

bool Ray::CrossSegment(const Segment& seg) {
  Point second = start_;
  second.Move(direction_);
  Line thisline = Line(start_, second);
  Line segline = Line(seg.GetA(), seg.GetB());
  double det =
      thisline.GetA() * segline.GetB() - thisline.GetB() * segline.GetA();
  if (det == 0) {
    return (ContainsPoint(seg.GetA()) || ContainsPoint(seg.GetB()));
  }
  Point res(
      -(thisline.GetC() * segline.GetB() - thisline.GetB() * segline.GetC()) /
          det,
      -(thisline.GetA() * segline.GetC() - thisline.GetC() * segline.GetA()) /
          det);
  Segment copy = seg;
  return ((copy.ContainsPoint(res)) && (this->ContainsPoint(res)));
}

IShape* Ray::Clone() {
  Ray* obj = new Ray(*this);
  return obj;
}

Circle::Circle() : radius_(0) { center_ = Point(); }

Circle::Circle(const Point& k_center, const size_t kRadius)
    : center_(k_center), radius_(kRadius) {}

Point Circle::GetCentre() const { return center_; }

size_t Circle::GetRadius() const { return radius_; }

void Circle::Move(const Vector& vec) { center_.Move(vec); }

bool Circle::ContainsPoint(const Point& point) {
  return (static_cast<int64_t>(radius_ * radius_) >=
          ((center_.GetX() - point.GetX()) * (center_.GetX() - point.GetX()) +
           (center_.GetY() - point.GetY()) * (center_.GetY() - point.GetY())));
}

double DistToPoint(const Point& first, const Point& second) {
  double val = static_cast<double>(
      (second.GetX() - first.GetX()) * (second.GetX() - first.GetX()) +
      (second.GetY() - first.GetY()) * (second.GetY() - first.GetY()));
  return val;
}

double DistToLine(const Point& point, const Line kLine) {
  double numer = static_cast<double>(
      kLine.GetA() * point.GetX() + kLine.GetB() * point.GetY() + kLine.GetC());
  double denom = static_cast<double>(kLine.GetA() * kLine.GetA() +
                                     kLine.GetB() * kLine.GetB());
  return numer * numer / denom;
}

bool Circle::CrossSegment(const Segment& seg) {
  double disttofp = DistToPoint(center_, seg.GetA());
  double disttosp = DistToPoint(center_, seg.GetB());
  double strdist = DistToLine(center_, Line(seg.GetA(), seg.GetB()));
  double dist = std::min(disttofp, disttosp);
  dist = std::min(dist, strdist);
  return ((dist <= radius_ * radius_) &&
          (std::max(disttofp, disttosp) >= radius_ * radius_));
}

IShape* Circle::Clone() {
  Circle* obj = new Circle(*this);
  return obj;
}