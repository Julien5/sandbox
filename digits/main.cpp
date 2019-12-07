#include <iostream>
#include <pnm.hpp>
#include <cmath>
#include <algorithm>

#define PI 3.14159265

namespace {
  double sqr(double x) {
    return x*x;
  }
}

class Point2D {
  std::size_t m_x=0,m_y=0;
public:
  std::size_t x() const {
    return m_x;
  }
  std::size_t y() const {
    return m_y;
  }
  Point2D(std::size_t _x, std::size_t _y):m_x(_x),m_y(_y){}
  Point2D(unsigned int r, float theta)
    :m_x(std::round(r*std::cos(theta))),
     m_y(std::round(r*std::sin(theta)))
  {}
  double distance2(const Point2D &p) {
    return sqr(m_x-p.m_x)+sqr(m_y-p.m_y);
  }
  Point2D operator+(const Point2D &rhs) {
    return Point2D(m_x+rhs.m_x,m_y+rhs.m_y);
  }
  bool operator==(const Point2D &rhs) {
    return m_x==rhs.m_x && m_y==rhs.m_y;
  }
  
  std::vector<Point2D> circle(const std::size_t &radius) {
    std::vector<Point2D> ret;
    for(float theta=0; theta<2*PI; theta+=PI/300) {
      const Point2D c=*this + Point2D(radius,theta);
      if (std::find(ret.begin(),ret.end(),c)==ret.end())
	ret.push_back(c);
    }
    return ret;
  }
  
};

std::ostream& operator<<(std::ostream &s, const pnm::rgb_pixel &p) {
  s << "(r,g,b)=(" << int(p.red) << "," << int(p.green) << "," << int(p.blue) << ")";
  return s;
}

namespace {
  uint8_t gray(const pnm::rgb_pixel &p) {
    return p.red + p.green + p.blue;
  }
  double mean(const pnm::image<pnm::rgb_pixel> &ppm) {
    double sum=0;
    for(std::size_t y=0; y<ppm.y_size(); ++y) 
      for(std::size_t x=0; x<ppm.x_size(); ++x) 
	sum+=double(gray(ppm[y][x]));
    return sum/(ppm.y_size()*ppm.x_size());
  } 
}

class Image {
  pnm::pbm_image m_data;
public:
  const pnm::pbm_image data() {
    return m_data;
  }
  Image(const pnm::image<pnm::rgb_pixel> &ppm)
    :m_data(ppm.x_size(),ppm.y_size()) {
    auto T=mean(ppm);
    for(std::size_t y=0; y<ppm.y_size(); ++y) 
      for(std::size_t x=0; x<ppm.x_size(); ++x) 
	m_data[y][x] = gray(ppm[y][x])>T;
  }
  pnm::bit_pixel pixel(const Point2D &p) const {
    return m_data[p.y()][p.x()];
  }
  void setPixel(const Point2D &p) {
    m_data[p.y()][p.x()] = 0;
  }
};
  
int main()
{
  using namespace pnm::literals;
  pnm::image<pnm::rgb_pixel> ppm = pnm::read("images/0.ppm");
  std::cout << "width  = " << ppm.width()  << std::endl;
  std::cout << "height = " << ppm.height() << std::endl;
  Image i(ppm);
  pnm::write("out.ppm", i.data(), pnm::format::binary);

  Point2D center(15U,15U);
  for(auto p : center.circle(3)) {
    std::cout << i.pixel(p).value << std::endl;
  }

  using namespace pnm::literals;
  const pnm::bit_pixel b1 = 0_bit;
  const pnm::bit_pixel b2 = 1_bit;
  Image circle(ppm);
  for(auto p : center.circle(3)) {
    circle.setPixel(p);
  }
  pnm::write("circle.pbm", circle.data(), pnm::format::binary);

  return 0;
}
