#include <cmath>
#include <set>
#include <map>
#include "mesh/mesh.h"
#include "mesh/geometry.h"

using namespace mesh;

Mesh catmullClark(int n){
  Mesh base = unitCircleCube();
  for (int i = 0; i < n; i++) {
    // New mesh
    std::vector<Face3D> new_faces;
    std::vector<Face3D> faces = base.get_faces();
    // Vertex related points
    std::map<Vertex3D, std::set<Vertex3D>> vertex_edgepoints;
    std::map<Vertex3D, std::set<Vertex3D>> vertex_facepoints;
    for (Vertex3D v : base.get_vertices()) {
      vertex_edgepoints[v] = {};
      vertex_facepoints[v] = {};
    }

    // Loop faces
    for (auto f : faces) {
      // Get Face points
      Vertex3D face_midpoint = f.get_midpoint();
      // Add face point to map
      for (auto v : f.vertices) {
        vertex_facepoints[v].insert(face_midpoint);
      }
      // Get Edge points
      std::vector<Vertex3D> edge_newpoints;
      std::vector<Edge3D> edges = f.get_edges();
      for (auto e : edges) {
        // Get faces with edge
        std::vector<Face3D> faces_with_edge = base.get_faces_with_edge(e);
        // Get values
        Vertex3D v0 = e.v1;
        Vertex3D v1 = e.v2;
        Vertex3D fm0 = faces_with_edge[0].get_midpoint();
        Vertex3D fm1 = faces_with_edge[1].get_midpoint();
        // Get edge
        Vertex3D edge_point = {
          (v0.x + v1.x + fm0.x + fm1.x) / 4.0f,
          (v0.y + v1.y + fm0.y + fm1.y) / 4.0f,
          (v0.z + v1.z + fm0.z + fm1.z) / 4.0f
        };
        edge_newpoints.push_back(edge_point);
        // Add edge midpoints to map
        Vertex3D edge_midpoint = e.get_midpoint();
        vertex_edgepoints[v0].insert(edge_midpoint);
        vertex_edgepoints[v1].insert(edge_midpoint);
      }
      // Get new faces
      for (int i = 0; i < f.vertices.size(); i++) {
        Vertex3D v0 = f.vertices[i];
        Vertex3D v1 = edge_newpoints[i];
        Vertex3D v2 = face_midpoint;
        Vertex3D v3 = edge_newpoints[(i + f.vertices.size() - 1) % f.vertices.size()];
        new_faces.push_back(Face3D({v0, v1, v2, v3}));
      }
    }
    // Get original points
    auto corners = base.get_vertices();
    // Update base
    base = Mesh(new_faces);
    // Update vertices
    for (auto corner: corners) {
      std::set<Vertex3D> edgepoints = vertex_edgepoints[corner];
      std::set<Vertex3D> facepoints = vertex_facepoints[corner];
      // F + 2R + (n-3)P / n
      // F=Average of face points
      // R=Average of edge midpoints
      // P=Current point
      // n=Number of edges/face
      int n = edgepoints.size();
      //std::cout << "corner: " << corner.x << " " << corner.y << " " << corner.z << std::endl;
      //std::cout << "n: " << n << std::endl;
      //std::cout << "facepoints: " << facepoints.size() << std::endl;
      //std::cout << "edgepoints: " << edgepoints.size() << std::endl;

      Vertex3D face_avg = {0, 0, 0};
      Vertex3D edge_avg = {0, 0, 0};
      for (auto f : facepoints) {
        face_avg.x += f.x;
        face_avg.y += f.y;
        face_avg.z += f.z;
      }
      for (auto e : edgepoints) {
        edge_avg.x += e.x;
        edge_avg.y += e.y;
        edge_avg.z += e.z;
      }
      face_avg.x /= n;
      face_avg.y /= n;
      face_avg.z /= n;
      edge_avg.x /= n;
      edge_avg.y /= n;
      edge_avg.z /= n;
      Vertex3D new_point = {
        (face_avg.x + 2*edge_avg.x + (n-3)*corner.x) / n,
        (face_avg.y + 2*edge_avg.y + (n-3)*corner.y) / n,
        (face_avg.z + 2*edge_avg.z + (n-3)*corner.z) / n
      };

      base.move_point(corner, new_point);
    }

  }
  return base;

}

int main() {
  Mesh sphere = catmullClark(5);
  sphere.save_ply("outputs/ck.ply");
  return 0;
}