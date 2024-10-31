#include "mesh.h"

namespace mesh{
  // Transform to generic Vertex3D
  Vertex3D Mesh::to_vertex(const MeshVertex& vertex) {
    return vertex;
  }

  // Transform to generic Face3D
  Face3D Mesh::to_face(const MeshFace& face) {
    std::vector<Vertex3D> vertices;
    for (int vertex_id : face.vertices) {
      vertices.push_back(get_vertex(vertex_id));
    }
    return Face3D(vertices, face.r, face.g, face.b);
  }

  // Insert vertex into mesh
  int Mesh::insert_vertex(const Vertex3D& vertex) {
    // Searcg for the vertex
    auto it = std::find(vertices.begin(), vertices.end(), vertex);
    if (it == vertices.end()) {
      // If the vertex is not found, add it
      vertices.push_back(vertex);
      return vertices.size() - 1;
    }
    else {
      return it - vertices.begin();
    }
  }

  // Insert face into mesh
  int Mesh::insert_face(const Face3D& face) {
    MeshFace mesh_face;
    for (Vertex3D vertex : face.vertices) {
      int vertex_id = insert_vertex(vertex);
      mesh_face.vertices.push_back(vertex_id);
    }
    mesh_face.r = face.r;
    mesh_face.g = face.g;
    mesh_face.b = face.b;
    faces.push_back(mesh_face);
    return faces.size() - 1;
  }

  // Constructor from generic Face3D
  Mesh::Mesh(const std::vector<Face3D>& faces) {
    for (Face3D face : faces) {
      insert_face(face);
    }
  }

  // Generic getter for vertices
  std::vector<Vertex3D> Mesh::get_vertices() {
    return vertices;
  }
  
  // Generic getter for faces
  std::vector<Face3D> Mesh::get_faces() {
    // From faces to Face3D
    std::vector<Face3D> face3d_faces;
    for (size_t i = 0; i < faces.size(); i++) {
      face3d_faces.push_back(get_face(i));
    }
    return face3d_faces;
  }

  Vertex3D Mesh::get_vertex(int index) {
    return vertices[index];
  }

  Face3D Mesh::get_face(int index) {
    return to_face(faces[index]);
  }

  // === To PLY ===
  std::string Mesh::get_header() {
    return "ply\nformat ascii 1.0\nelement vertex " + std::to_string(vertices.size()) + "\nproperty double x\nproperty double y\nproperty double z\nelement face " + std::to_string(faces.size()) + "\nproperty list uchar int vertex_index\nproperty uchar red\nproperty uchar green\nproperty uchar blue\nend_header\n";
  }

  std::string Mesh::get_vertex_string() {
    std::string vertex_string = "";
    for (Vertex3D vertex : vertices) {
      vertex_string += std::to_string(vertex.x) + " " + std::to_string(vertex.y) + " " + std::to_string(vertex.z) + "\n";
    }
    return vertex_string;
  }

  std::string Mesh::get_face_string() {
    std::string face_string = "";
    for (MeshFace face : faces) {
      face_string += std::to_string(face.vertices.size()) + " ";
      for (int vertex_id : face.vertices) {
        face_string += std::to_string(vertex_id) + " ";
      }
      // RGB data
      face_string += std::to_string(face.r) + " " + std::to_string(face.g) + " " + std::to_string(face.b);
      face_string += "\n";
    }
    return face_string;
  }

  void Mesh::save(const char* filename) {
    std::ofstream file;
    file.open(filename);
    file << get_header();
    file << get_vertex_string();
    file << get_face_string();
    file.close();
  }

  // === Query ===
  std::vector<Face3D> Mesh::get_faces_with_edge(const Edge3D& edge) {
    std::vector<Face3D> faces_with_edge;
    for (MeshFace face : faces) {
      for (int i = 0; i < face.vertices.size(); i++) {
        Vertex3D v1 = get_vertex(face.vertices[i]);
        Vertex3D v2 = get_vertex(face.vertices[(i + 1) % face.vertices.size()]);
        Edge3D forward_edge = Edge3D(v1, v2);
        Edge3D reversed_edge = forward_edge.reversed();
        if (forward_edge == edge || reversed_edge == edge) {
          faces_with_edge.push_back( to_face(face) );
          break;
        }
      }
    }
    return faces_with_edge;
  }

  // Modify mesh
  void Mesh::move_point(const Vertex3D& point, const Vertex3D& target) {
    // Search for the point
    auto it = std::find(vertices.begin(), vertices.end(), point);
    if (it != vertices.end()) {
      // If the point is found, move it
      *it = target;
    }
  }

  // Displace
  void Mesh::displace(const Vertex3D& v) {
    for (Vertex3D& vertex : vertices) {
      vertex = vertex + v;
    }
  }

}