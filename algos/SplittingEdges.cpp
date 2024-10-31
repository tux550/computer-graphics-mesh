#include "SplittingEdges.h"


Mesh sphereBySplittingEdges(int n){
  Mesh base = unitCircleTetrahedron();
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
      Vertex3D v01 = Vertex3D((v0.x + v1.x)/2, (v0.y + v1.y)/2, (v0.z + v1.z)/2).normalized();
      Vertex3D v12 = Vertex3D((v1.x + v2.x)/2, (v1.y + v2.y)/2, (v1.z + v2.z)/2).normalized();
      Vertex3D v20 = Vertex3D((v2.x + v0.x)/2, (v2.y + v0.y)/2, (v2.z + v0.z)/2).normalized();
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


/*
int main() {
  Mesh sphere = sphereBySplittingEdges(3);
  sphere.save_ply("outputs/sphere.ply");
  return 0;
}
*/