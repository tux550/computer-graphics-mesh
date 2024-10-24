#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>


#ifndef MESH_GENERICS_3D_H
#define MESH_GENERICS_3D_H

namespace mesh{ 

// GENERICS
  struct Vertex3D {
    float x, y, z;
    Vertex3D(float x, float y, float z);
    Vertex3D();
    bool operator==(const Vertex3D& other) const;
    bool operator<(const Vertex3D& other) const;
    Vertex3D normalized();
  };
  Vertex3D operator+(const Vertex3D& v1, const Vertex3D& v2);
  Vertex3D operator-(const Vertex3D& v1, const Vertex3D& v2);
  Vertex3D operator/(const Vertex3D& v1, double scalar);

  Vertex3D cross_product(const Vertex3D& v1, const Vertex3D& v2);
  
  double dot_product(const Vertex3D& v1, const Vertex3D& v2);

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
    int r,g,b;
    Face3D(const std::vector<Vertex3D>& vertices);
    Face3D();
    Face3D(const std::vector<Vertex3D>& vertices, int r, int g, int b) ;
    Vertex3D get_midpoint();
    std::vector<Edge3D> get_edges();
    void flip();
  };

}

#endif