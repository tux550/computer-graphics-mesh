#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#ifndef MESH_H
#define MESH_H

namespace mesh{ 
  // GENERICS
  struct Vertex3D {
    float x, y, z;
    Vertex3D(float x, float y, float z);
    Vertex3D();
    bool operator==(const Vertex3D& other) const;
    bool operator<(const Vertex3D& other) const;
  };

  struct Edge3D {
    Vertex3D v1, v2;
    Edge3D(const Vertex3D& v1, const Vertex3D& v2);
    Edge3D();
    bool operator==(const Edge3D& other) const;
    Edge3D reversed();
    Vertex3D get_midpoint();
  };

  struct Face3D {
    std::vector<Vertex3D> vertices;
    Face3D(const std::vector<Vertex3D>& vertices);
    Face3D();
    Vertex3D get_midpoint();
    std::vector<Edge3D> get_edges();
  };

  // MESH

  using MeshVertex = Vertex3D;
  using MeshFace = std::vector<int>;

  class Mesh {
  private:
    std::vector<MeshVertex> vertices;
    std::vector<MeshFace> faces;

    std::string get_header();
    std::string get_vertex_string();
    std::string get_face_string();

    Vertex3D to_vertex(const MeshVertex& vertex);
    Face3D to_face(const MeshFace& face);
  public:
    Mesh(const std::vector<Face3D>& faces);
    void save(const char* filename);
    int insert_face(const Face3D& face);
    int insert_vertex(const Vertex3D& vertex);
    std::vector<Vertex3D> get_vertices();
    Vertex3D get_vertex(int index);
    std::vector<Face3D> get_faces();
    Face3D get_face(int index);
    // Utility
    std::vector<Face3D> get_faces_with_edge(const Edge3D& edge);
    Vertex3D get_face_midpoint(const MeshFace& face);
    Vertex3D get_face_midpoint(int face_index);
    void move_point(const Vertex3D& point, const Vertex3D& target);
  };
}

#endif