#include "3d.h"

namespace mesh{

  Vertex3D::Vertex3D(float x, float y, float z) :
    x(x), y(y), z(z) {}
  Vertex3D::Vertex3D() : 
    x(0), y(0), z(0) {}

  bool Vertex3D::operator==(const Vertex3D& other) const {
    return x == other.x && y == other.y && z == other.z;
  }

  bool Vertex3D::operator<(const Vertex3D& other) const {
    return x < other.x || (x == other.x && y < other.y) || (x == other.x && y == other.y && z < other.z);
  }

  Vertex3D Vertex3D::normalized() {
    float norm = sqrt(x * x + y * y + z * z);
    return Vertex3D(x / norm, y / norm, z / norm);
  }

  Face3D::Face3D(const std::vector<Vertex3D>& vertices):
    vertices(vertices) {}

  Face3D::Face3D() :
    vertices({}) {}

  Vertex3D Face3D::get_midpoint() {
    float x = 0, y = 0, z = 0;
    for (Vertex3D vertex : vertices) {
      x += vertex.x;
      y += vertex.y;
      z += vertex.z;
    }
    return Vertex3D(x / vertices.size(), y / vertices.size(), z / vertices.size());
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
}