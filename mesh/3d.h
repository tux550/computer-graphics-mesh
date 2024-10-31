#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <optional>


#ifndef MESH_GENERICS_3D_H
#define MESH_GENERICS_3D_H

namespace mesh{ 

// GENERICS
  struct Vertex3D {
    double x, y, z;
    Vertex3D(double x, double y, double z);
    Vertex3D();
    double magnitude();
    bool operator==(const Vertex3D& other) const;
    bool operator<(const Vertex3D& other) const;
    Vertex3D normalized();
  };
  Vertex3D operator+(const Vertex3D& v1, const Vertex3D& v2);
  Vertex3D operator-(const Vertex3D& v1, const Vertex3D& v2);
  Vertex3D operator/(const Vertex3D& v1, double scalar);
  Vertex3D operator*(const Vertex3D& v1, double scalar);

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

  // Additional
  struct Plane3D {
    Vertex3D point;
    Vertex3D normal;
    Plane3D(const Vertex3D& point, const Vertex3D& normal);
  };

  struct Line3D {
    Vertex3D point;
    Vertex3D direction;
    Line3D(const Vertex3D& point, const Vertex3D& direction);
  };

  std::optional<Vertex3D> intersect_lp(const Line3D& l1, const Plane3D& p1);


  struct Face3D {
    std::vector<Vertex3D> vertices;
    int r,g,b;
    Face3D(const std::vector<Vertex3D>& vertices);
    Face3D();
    Face3D(const std::vector<Vertex3D>& vertices, int r, int g, int b) ;
    Vertex3D get_midpoint();
    Plane3D get_plane();
    Vertex3D get_normal();
    bool is_triangle();
    void displace(const Vertex3D& v);
    std::optional<Vertex3D> intersect(const Line3D& l1);
    std::vector<Edge3D> get_edges();
    void flip();
  };


}

#endif