#include "3d.h"

namespace mesh{

  Vertex3D::Vertex3D(double x, double y, double z) :
    x(x), y(y), z(z) {}
  Vertex3D::Vertex3D() : 
    x(0), y(0), z(0) {}

  bool Vertex3D::operator==(const Vertex3D& other) const {
    return x == other.x && y == other.y && z == other.z;
  }

  bool Vertex3D::operator<(const Vertex3D& other) const {
    return x < other.x || (x == other.x && y < other.y) || (x == other.x && y == other.y && z < other.z);
  }

  double Vertex3D::magnitude() {
    return std::sqrt(x * x + y * y + z * z);
  }

  Vertex3D Vertex3D::normalized() {
    double norm = sqrt(x * x + y * y + z * z);
    return Vertex3D(x / norm, y / norm, z / norm);
  }

  Vertex3D operator+(const Vertex3D& v1, const Vertex3D& v2) {
    return Vertex3D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
  }

  Vertex3D operator-(const Vertex3D& v1, const Vertex3D& v2) {
    return Vertex3D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
  }

  Vertex3D operator/(const Vertex3D& v1, double scalar) {
    return Vertex3D(v1.x / scalar, v1.y / scalar, v1.z / scalar);
  }

  Vertex3D operator*(const Vertex3D& v1, double scalar) {
    return Vertex3D(v1.x * scalar, v1.y * scalar, v1.z * scalar);
  }

  Vertex3D cross_product(const Vertex3D& v1, const Vertex3D& v2) {
    return Vertex3D(
      v1.y * v2.z - v1.z * v2.y,
      v1.z * v2.x - v1.x * v2.z,
      v1.x * v2.y - v1.y * v2.x
    );
  }
  double dot_product(const Vertex3D& v1, const Vertex3D& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
  }

  Face3D::Face3D(const std::vector<Vertex3D>& vertices):
    vertices(vertices),
    r(0), g(0), b(0) {}

  Face3D::Face3D() :
    vertices({}),
    r(0), g(0), b(0) {}

  Face3D::Face3D(const std::vector<Vertex3D>& vertices, int r, int g, int b) :
    vertices(vertices),
    r(r), g(g), b(b) {}

  void Face3D::flip() {
    std::reverse(vertices.begin(), vertices.end());
  }

  bool Face3D::is_triangle() {
    return vertices.size() == 3;
  }

  std::optional<Vertex3D> Face3D::intersect(const Line3D& l1) {
    Plane3D plane = get_plane();
    auto intersection = intersect_lp(l1, plane);
    // If no value return nullopt
    if (!intersection.has_value()) {
      return std::nullopt;
    }
    // If value check if it is inside the face
    Vertex3D intersection_point = intersection.value();
    // Crossproduct must have the same sign as normal
    for (Edge3D edge : get_edges()) {
      Vertex3D v1 = edge.v1;
      Vertex3D v2 = edge.v2;
      Vertex3D normal = cross_product(v2 - v1, intersection_point - v1);
      auto direction = dot_product(normal, plane.normal);
      if (direction < 0) {
        return std::nullopt;
      }
    }
    // Return intersection
    return intersection;
  }

  Vertex3D Face3D::get_midpoint() {
    double x = 0, y = 0, z = 0;
    for (Vertex3D vertex : vertices) {
      x += vertex.x;
      y += vertex.y;
      z += vertex.z;
    }
    return Vertex3D(x / vertices.size(), y / vertices.size(), z / vertices.size());
  }

  void Face3D::displace(const Vertex3D& v) {
    for (Vertex3D& vertex : vertices) {
      vertex = vertex + v;
    }
  }

  Plane3D Face3D::get_plane() {
    Vertex3D v1 = vertices[0];
    Vertex3D v2 = vertices[1];
    Vertex3D v3 = vertices[2];
    Vertex3D normal = cross_product(v2 - v1, v3 - v1).normalized();
    return Plane3D(v1, normal);
  }

  Vertex3D Face3D::get_normal() {
    return get_plane().normal;
  }

  std::vector<Edge3D> Face3D::get_edges() {
    std::vector<Edge3D> edges;
    for (int i = 0; i < vertices.size(); i++) {
      edges.push_back(Edge3D(vertices[i], vertices[(i + 1) % vertices.size()]));
    }
    return edges;
  }

  Edge3D::Edge3D(const Vertex3D& v1, const Vertex3D& v2) :
    v1(v1), v2(v2) {}

  Edge3D::Edge3D() :
    v1(Vertex3D()), v2(Vertex3D()) {}

  Edge3D Edge3D::reversed() {
    return Edge3D(v2, v1);
  }

  Vertex3D Edge3D::get_midpoint() {
    return Vertex3D((v1.x + v2.x) / 2.0, (v1.y + v2.y) / 2.0, (v1.z + v2.z) / 2.0);
  }

  bool Edge3D::operator==(const Edge3D& other) const {
    return (v1 == other.v1 && v2 == other.v2) || (v1 == other.v2 && v2 == other.v1);
  }

  Plane3D::Plane3D(const Vertex3D& point, const Vertex3D& normal) :
    point(point), normal(normal) {}

  Line3D::Line3D(const Vertex3D& point, const Vertex3D& direction) :
    point(point), direction(direction) {}
  
  std::optional<Vertex3D> intersect_lp(const Line3D& l1, const Plane3D& p1) {
    // Plane equation:
    // ax + by + cz + d = 0
    double a = p1.normal.x;
    double b = p1.normal.y;
    double c = p1.normal.z;
    double d = - (p1.point.x * p1.normal.x + p1.point.y * p1.normal.y + p1.point.z * p1.normal.z);
    // Line equation:
    // x = x_0 + d_x * t
    // y = y_0 + d_y * t
    // z = z_0 + d_z * t
    double x_0 = l1.point.x;
    double y_0 = l1.point.y;
    double z_0 = l1.point.z;
    double d_x = l1.direction.x;
    double d_y = l1.direction.y;
    double d_z = l1.direction.z;

    // Substitute line equation into plane equation
    // a(x_0 + d_x * t) + b(y_0 + d_y * t) + c(z_0 + d_z * t) + d = 0
    // a * x_0 + a * d_x * t + b * y_0 + b * d_y * t + c * z_0 + c * d_z * t + d = 0
    // t = - (a * x_0 + b * y_0 + c * z_0 + d) / (a * d_x + b * d_y + c * d_z)


    // Denomindaor
    auto t_denominator = a * d_x + b * d_y + c * d_z;
    if (t_denominator == 0) {
      return std::nullopt;
    }
    // Numerator
    auto t_numerator = - (a * x_0 + b * y_0 + c * z_0 + d);

    // t
    auto t = t_numerator / t_denominator;
    
    //Get the intersection point
    return Vertex3D(x_0 + d_x * t, y_0 + d_y * t, z_0 + d_z * t);
  }
}