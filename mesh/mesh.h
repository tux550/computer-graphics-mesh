#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include "3d.h"

#ifndef MESH_STRUCTURE_H
#define MESH_STRUCTURE_H



namespace mesh{ 
  // MESH
  using MeshVertex = Vertex3D;
  struct MeshFace {
    std::vector<int> vertices;
    int r, g, b;
  };

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
    Mesh(const std::string& filename);
    void save_ply(const char* filename);

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
    void displace(const Vertex3D& v);
  };
}

#endif