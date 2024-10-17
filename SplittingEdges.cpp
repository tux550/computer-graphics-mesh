#include <cmath>
#include "mesh/mesh.h"

using namespace mesh;

Mesh get_tetrahedron() {
  // Vertex of tetrahedron
  // > (0, 0, 0)
  // > (0, 1, 1)
  // > (1, 0, 1)
  // > (1, 1, 0)
  // Faces of tetrahedron
  // > v0, v1, v3
  // > v0, v3, v2
  // > v0, v2, v1
  // > v1, v2, v3
  float d = std::sqrt(1.0f/3.0f);

  std::vector<Vertex3D> v = {
    Vertex3D(-d, -d, -d),
    Vertex3D(-d, d, d),
    Vertex3D(d, -d, d),
    Vertex3D(d, d, -d)
  };
  std::vector<Face3D> f = {
    Face3D({v[0], v[1], v[3]}),
    Face3D({v[0], v[3], v[2]}),
    Face3D({v[0], v[2], v[1]}),
    Face3D({v[1], v[2], v[3]})
  };

  return Mesh(f);
}

Vertex3D toUnitSphere(const Vertex3D& vertex) {
  float x = vertex.x;
  float y = vertex.y;
  float z = vertex.z;
  float norm = std::sqrt(x*x + y*y + z*z);
  return Vertex3D(x/norm, y/norm, z/norm);
}

Mesh sphereBySplittingEdges(int n){
  Mesh base = get_tetrahedron();
  for (int i = 0; i < n; i++) {
    // New mesh
    std::vector<Face3D> new_faces;
    // Loop faces
    for (Face3D face : base.get_faces()) {
      // Get vertices
      Vertex3D v0 = face.vertices[0];
      Vertex3D v1 = face.vertices[1];
      Vertex3D v2 = face.vertices[2];
      // Get new vertices
      Vertex3D v01 = toUnitSphere(Vertex3D((v0.x + v1.x)/2, (v0.y + v1.y)/2, (v0.z + v1.z)/2));
      Vertex3D v12 = toUnitSphere(Vertex3D((v1.x + v2.x)/2, (v1.y + v2.y)/2, (v1.z + v2.z)/2));
      Vertex3D v20 = toUnitSphere(Vertex3D((v2.x + v0.x)/2, (v2.y + v0.y)/2, (v2.z + v0.z)/2));
      // Insert new faces
      new_faces.push_back(Face3D({v0, v01, v20}));
      new_faces.push_back(Face3D({v1, v12, v01}));
      new_faces.push_back(Face3D({v2, v20, v12}));
      new_faces.push_back(Face3D({v01, v12, v20}));
    }
    // Update base
    base = Mesh(new_faces);
  }
  return base;

}



int main() {
  Mesh sphere = sphereBySplittingEdges(3);
  sphere.save("outputs/sphere.ply");
  return 0;
}