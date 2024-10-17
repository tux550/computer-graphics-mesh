#include "mesh.h"

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
  return Vertex3D((v1.x + v2.x) / 2, (v1.y + v2.y) / 2, (v1.z + v2.z) / 2);
}

bool Edge3D::operator==(const Edge3D& other) const {
  return (v1 == other.v1 && v2 == other.v2) || (v1 == other.v2 && v2 == other.v1);
}

Vertex3D Mesh::to_vertex(const MeshVertex& vertex) {
  return vertex;
}

Face3D Mesh::to_face(const MeshFace& face) {
  std::vector<Vertex3D> vertices;
  for (int vertex_id : face) {
    vertices.push_back(get_vertex(vertex_id));
  }
  return Face3D(vertices);
}

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

int Mesh::insert_face(const Face3D& face) {
  MeshFace mesh_face;
  for (Vertex3D vertex : face.vertices) {
    int vertex_id = insert_vertex(vertex);
    mesh_face.push_back(vertex_id);
  }
  faces.push_back(mesh_face);
  return faces.size() - 1;
}

Mesh::Mesh(const std::vector<Face3D>& faces) {
  for (Face3D face : faces) {
    insert_face(face);
  }
}

std::vector<Vertex3D> Mesh::get_vertices() {
  return vertices;
}

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


std::string Mesh::get_header() {
  return "ply\nformat ascii 1.0\nelement vertex " + std::to_string(vertices.size()) + "\nproperty float x\nproperty float y\nproperty float z\nelement face " + std::to_string(faces.size()) + "\nproperty list uchar int vertex_index\nend_header\n";
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
    face_string += std::to_string(face.size()) + " ";
    for (int vertex_id : face) {
      face_string += std::to_string(vertex_id) + " ";
    }
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

std::vector<Face3D> Mesh::get_faces_with_edge(const Edge3D& edge) {
  std::vector<Face3D> faces_with_edge;
  for (MeshFace face : faces) {
    for (int i = 0; i < face.size(); i++) {
      Vertex3D v1 = get_vertex(face[i]);
      Vertex3D v2 = get_vertex(face[(i + 1) % face.size()]);
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

void Mesh::move_point(const Vertex3D& point, const Vertex3D& target) {
  // Search for the point
  auto it = std::find(vertices.begin(), vertices.end(), point);
  if (it != vertices.end()) {
    // If the point is found, move it
    *it = target;
  }
}