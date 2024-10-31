#include "mesh.h"
#include <sstream>

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

  // Constructor from PLY file
  Mesh::Mesh(const std::string& filename) {

    std::ifstream file;
    file.open(filename);
    std::string line;
    // Read header
    std::getline(file, line);
    if (line != "ply") {
      std::cerr << "Invalid PLY file" << std::endl;
      return;
    }
    std::getline(file, line);
    if (line != "format ascii 1.0") {
      std::cerr << "Invalid PLY fileformat" << std::endl;
      return;
    }
    // Elements
    std::vector<std::string> elements;
    std::map<std::string, int> element_count;
    std::getline(file, line);
    std::string current_element = "";
    while (line != "end_header"){
      // Split line by spaces
      std::istringstream iss(line);
      std::string t1, t2, t3;
      iss >> t1 >> t2 >> t3;
      // Determine input
      if (t1 == "element"){
        // Read element
        current_element = t2;
        auto current_count = std::stoi(t3);
        elements.push_back(current_element);
        element_count[current_element] = current_count;
      }
      else if (t1 == "property"){
        // skip
      }
      // Read next line
      std::getline(file, line);
    } 
    // Read vertices
    for (auto element : elements){
      if (element == "vertex"){
        for (int i = 0; i < element_count[element]; i++){
          std::getline(file, line);
          std::istringstream iss(line);
          double x, y, z;
          iss >> x >> y >> z;
          vertices.push_back(Vertex3D(x, y, z));
        }
      }
      else if (element == "face"){
        for (int i = 0; i < element_count[element]; i++){
          std::getline(file, line);
          std::istringstream iss(line);
          int vertex_count;
          iss >> vertex_count;
          std::vector<int> vertex_ids;
          for (int j = 0; j < vertex_count; j++){
            int vertex_id;
            iss >> vertex_id;
            vertex_ids.push_back(vertex_id);
          }
          // If iss is not empty, read color
          int r, g, b;
          if (iss >> r >> g >> b){}
          else {
            // Default color
            r = 255;
            g = 255;
            b = 255;
          }
          // Create face
          MeshFace face;
          face.vertices = vertex_ids;
          face.r = r;
          face.g = g;
          face.b = b;
          faces.push_back(face);
        }
      }
      else {
        // Skip element
        for (int i = 0; i < element_count[element]; i++){
          std::getline(file, line);
        }
      }
    }
    // Close file
    file.close();
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

  void Mesh::save_ply(const char* filename) {
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