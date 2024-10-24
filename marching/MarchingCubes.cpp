#include <functional>
#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <random>
#include "mesh.h"
#include "geometry.h"

#define DEBUG_COLOR

using namespace mesh;

using CubeVertexes = std::array<Vertex3D, 8>;
using CubeColors = std::array<bool, 8>;

double getSign(double value) {
  if (value == 0) {
    return 0;
  }
  return value > 0 ? 1 : -1;
}
double getColor(double value) {
  return value > 0 ? 1 : 0;
}


// Sphere function
double f(double x, double y, double z) {
  return x * x + y * y + z * z - 1;
}

void flipColors(CubeColors& colors) {
  for (size_t i = 0; i < 8; i++) {
    colors[i] = !colors[i];
  }
}

// Front face 0-3 in counter-clockwise order, back face 4-7 in counter-clockwise order
std::vector<size_t> getVertexNeighbors(size_t index) {
  switch (index)
  {
    case 0:
      return {1, 3, 4};
    case 1:
      return {0, 2, 5};
    case 2:
      return {1, 3, 6};
    case 3:
      return {0, 2, 7};
    case 4:
      return {0, 5, 7};
    case 5:
      return {1, 4, 6};
    case 6:
      return {2, 5, 7};
    case 7:
      return {3, 4, 6};
    default:
      std::cerr << "Invalid vertex index: " << index << std::endl;
      return {};
  }
}

std::vector<size_t> getVertexNeighborsExcluding(size_t index, const std::vector<size_t> &exclude) {
  std::vector<size_t> neighbors = getVertexNeighbors(index);
  for (size_t i = 0; i < exclude.size(); i++) {
    neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), exclude[i]), neighbors.end());
  }
  return neighbors;
}

bool vertexAreAdjacent(size_t a, size_t b) {
  std::vector<size_t> neighbors = getVertexNeighbors(a);
  return std::find(neighbors.begin(), neighbors.end(), b) != neighbors.end();
}

std::vector<std::vector<size_t>> getAdyacentColoredVertices(const CubeColors& colors) {
  std::vector<std::vector<size_t>> adyacent_colored_vertices;
  CubeColors visited = {false, false, false, false, false, false, false, false};
  for (size_t i = 0; i < 8; i++) {
    // DFS
    if (colors[i] && !visited[i]) {
      std::vector<size_t> adyacent_colored;
      std::vector<size_t> stack = {i};
      while (!stack.empty()) {
        size_t vertex = stack.back();
        stack.pop_back();
        if (visited[vertex]) {
          continue;
        }
        visited[vertex] = true;
        adyacent_colored.push_back(vertex);
        std::vector<size_t> neighbors = getVertexNeighbors(vertex);
        for (size_t neighbor : neighbors) {
          if (colors[neighbor] && !visited[neighbor]) {
            stack.push_back(neighbor);
          }
        }
      }
      adyacent_colored_vertices.push_back(adyacent_colored);
    }
  }
  return adyacent_colored_vertices;
}


std::vector<Face3D> getFacesVertex1(const CubeVertexes& cube, std::vector<size_t> vertex_indexes) {
  auto vertex_index = vertex_indexes[0];
  // Get neighbors
  Vertex3D vertex = cube[vertex_index];
  std::vector<size_t> n = getVertexNeighbors(vertex_index);
  // Triangle is midpoint between vertex and neighbors
  return {
    Face3D( {
      Edge3D(vertex, cube[n[0]]).get_midpoint(),
      Edge3D(vertex, cube[n[1]]).get_midpoint(),
      Edge3D(vertex, cube[n[2]]).get_midpoint()
      }
      #ifdef DEBUG_COLOR
      , 255, 0, 0
      #endif
    )
  };

}

std::vector<Face3D> getFacesVertex2(const CubeVertexes& cube, std::vector<size_t> vertex_indexes) {
  // Get points
  size_t i1 = vertex_indexes[0];
  size_t i2 = vertex_indexes[1];
  // Get neighbors from p1 and p2
  std::vector<size_t> neighbors1 = getVertexNeighborsExcluding(i1, {i2});
  auto a1 = neighbors1[0];
  auto a2 = neighbors1[1];
  std::vector<size_t> neighbors2 = getVertexNeighborsExcluding(i2, {i1});
  auto b1 = neighbors2[0];
  auto b2 = neighbors2[1];
  // Make sure a1 and b1 are adjacent
  if (!vertexAreAdjacent(a1, b1)) {
    std::swap(b1, b2);
  }
  // Build plane
  auto e1 = Edge3D(cube[i1], cube[a1]);
  auto e2 = Edge3D(cube[i1], cube[a2]);
  auto e3 = Edge3D(cube[i2], cube[b2]);
  auto e4 = Edge3D(cube[i2], cube[b1]);
  return {
    Face3D( {e1.get_midpoint(), e2.get_midpoint(), e3.get_midpoint(), e4.get_midpoint()}
      #ifdef DEBUG_COLOR
      ,0, 255, 0
      #endif
    )
  };
}

std::vector<Face3D> getFacesVertex3(const CubeVertexes& cube, std::vector<size_t> vertex_indexes) {
  // Get points
  size_t i1 = vertex_indexes[0];
  size_t i2 = vertex_indexes[1];
  size_t i3 = vertex_indexes[2];
  // Get neighbors from p1 and p2
  std::vector<size_t> n1 = getVertexNeighborsExcluding(i1, vertex_indexes);
  std::vector<size_t> n2 = getVertexNeighborsExcluding(i2, vertex_indexes);
  std::vector<size_t> n3 = getVertexNeighborsExcluding(i3, vertex_indexes);
  // Move so that i1/n1 have the least neighbors
  if ( n1.size() > n2.size() ) {
    std::swap(n1, n2);
    std::swap(i1, i2);
  }
  if ( n1.size() > n3.size() ) {
    std::swap(n1, n3);
    std::swap(i1, i3);
  }
  // Get the triangle
  auto a1 = n1[0];
  auto b1 = n2[0];
  auto b2 = n2[1];
  auto c1 = n3[0];
  auto c2 = n3[1];
  // Make sure a1-b1 and a1-c1 are adjacent
  if (!vertexAreAdjacent(a1, b1)) {
    std::swap(b1, b2);
  }
  if (!vertexAreAdjacent(a1, c1)) {
    std::swap(c1, c2);
  }
  // Build triangel from edges (a1,b1,c1) and cuadrilateral from (a1,a2,b2,b1)
  auto ea1 = Edge3D(cube[i1], cube[a1]);
  auto eb1 = Edge3D(cube[i2], cube[b1]);
  auto eb2 = Edge3D(cube[i2], cube[b2]);
  auto ec1 = Edge3D(cube[i3], cube[c1]);
  auto ec2 = Edge3D(cube[i3], cube[c2]);
  return {
    Face3D( {ea1.get_midpoint(), eb1.get_midpoint(), ec1.get_midpoint()}
      #ifdef DEBUG_COLOR
      ,0, 0, 255
      #endif
    ),
    Face3D( {eb1.get_midpoint(), eb2.get_midpoint(), ec2.get_midpoint(), ec1.get_midpoint()}
      #ifdef DEBUG_COLOR
      ,0, 0, 255
      #endif
    )
  };  
}


std::vector<Face3D> getFacesVertex4(const CubeVertexes& cube, std::vector<size_t> vertex_indexes) {
  // How many neighbors has the least vertex not in the list
  size_t i1 = vertex_indexes[0];
  size_t i2 = vertex_indexes[1];
  size_t i3 = vertex_indexes[2];
  size_t i4 = vertex_indexes[3];
  auto n1 = getVertexNeighborsExcluding(i1, {i2, i3, i4});
  auto n2 = getVertexNeighborsExcluding(i2, {i1, i3, i4});
  auto n3 = getVertexNeighborsExcluding(i3, {i1, i2, i4});
  auto n4 = getVertexNeighborsExcluding(i4, {i1, i2, i3});
  // Get min neighbors
  size_t min_neighbors = std::min({n1.size(), n2.size(), n3.size(), n4.size()});
  size_t max_neighbors = std::max({n1.size(), n2.size(), n3.size(), n4.size()});
  // CASES:
  // 1. No unique neighbors. Then it is a diagonal plane
  if (min_neighbors == 0) {
    // Make sure that i4 is the one with no unique neighbors
    if (n4.size() != 0) {
      std::swap(i4, i3);
      std::swap(n4, n3);
    } 
    if (n4.size() != 0) {
      std::swap(i4, i2);
      std::swap(n4, n2);
    }
    if (n4.size() != 0) {
      std::swap(i4, i1);
      std::swap(n4, n1);
    }
    // Get points
    auto a1 = n1[0];
    auto a2 = n1[1];
    auto b1 = n2[0];
    auto b2 = n2[1];
    auto c1 = n3[0];
    auto c2 = n3[1];
    // Make sure a2 and b1 are equal
    if (a2 != b1) {
      std::swap(b1, b2);
    }
    // Make sure b2 and c1 are equal
    if (b2 != c1) {
      std::swap(c1, c2);
    }
    // Build hexagon for the diagonal plane
    auto e1 = Edge3D(cube[i1], cube[a1]);
    auto e2 = Edge3D(cube[i1], cube[a2]);
    auto e3 = Edge3D(cube[i2], cube[b1]);
    auto e4 = Edge3D(cube[i2], cube[b2]);
    auto e5 = Edge3D(cube[i3], cube[c1]);
    auto e6 = Edge3D(cube[i3], cube[c2]);

    return {
      Face3D( {
        e1.get_midpoint(), e2.get_midpoint(), // Points close to i1
        e3.get_midpoint(), e4.get_midpoint(), // Points close to i2
        e5.get_midpoint(), e6.get_midpoint()  // Points close to i3
        }
        #ifdef DEBUG_COLOR
        ,128, 0, 128 // purple
        #endif
      )
    };
  }
  // 2. All have 1 neighbor. Then it is a square parallel to the faces
  else if (min_neighbors == 1 && max_neighbors == 1) {
    // Make sure i2 is adjacent to i1
    if (!vertexAreAdjacent(i1, i2)) {
      std::swap(i2, i3);
      std::swap(n2, n3);
    }
    // Make sure i4 is adjacent to i1
    if (!vertexAreAdjacent(i1, i4)) {
      std::swap(i4, i3);
      std::swap(n4, n3);
    }
    // Get the square
    auto e1 = Edge3D(cube[i1], cube[n1[0]]);
    auto e2 = Edge3D(cube[i2], cube[n2[0]]);
    auto e3 = Edge3D(cube[i3], cube[n3[0]]);
    auto e4 = Edge3D(cube[i4], cube[n4[0]]);
    return {
      Face3D( {e1.get_midpoint(), e2.get_midpoint(), e3.get_midpoint(), e4.get_midpoint()}
        #ifdef DEBUG_COLOR
        ,255, 255, 0 // yellow
        #endif
      )
    };
  }
  // CASE: Curved 
  else if (min_neighbors == 1 && max_neighbors == 2) {
    // Make sure i1 and i4 have the most neighbors
    if (n1.size() != 2) {
      std::swap(i1, i2);
      std::swap(n1, n2);
    }
    if (n4.size() != 2) {
      std::swap(i4, i3);
      std::swap(n4, n3);
    }
    // Make sure i2 is adjacent to i1
    if (!vertexAreAdjacent(i1, i2)) {
      std::swap(i2, i3);
      std::swap(n2, n3);
    }
    // i1<->i2<->i3<->i4 adjacency
    std::vector<Face3D> faces;
    // Get interest points
    auto a1 = n1[0];
    auto a2 = n1[1];
    auto b1 = n2[0];
    auto c1 = n3[0];
    auto d1 = n4[0];
    auto d2 = n4[1];
    // Make sure a1 and b1 are adjacent
    if (!vertexAreAdjacent(a1, b1)) {
      std::swap(a1, a2);
    }
    // Make sure d1 and c1 are adjacent
    // If this holds, then d2 and b1 are the same point
    if (!vertexAreAdjacent(d1, c1)) {
      std::swap(d1, d2);
    }
    // Build edges
    auto e_a1 = Edge3D(cube[i1], cube[a1]);
    auto e_a2 = Edge3D(cube[i1], cube[a2]);
    auto e_b1 = Edge3D(cube[i2], cube[b1]);
    auto e_c1 = Edge3D(cube[i3], cube[c1]);
    auto e_d1 = Edge3D(cube[i4], cube[d1]);
    auto e_d2 = Edge3D(cube[i4], cube[d2]);
    // Build faces
    // > Triangle 1
    faces.push_back(
      Face3D( {e_a1.get_midpoint(), e_b1.get_midpoint(),  e_a2.get_midpoint()}
        #ifdef DEBUG_COLOR
        ,128, 128, 128 // teal
        #endif
      )
    );
    // > Cuadrilateral
    faces.push_back(
      Face3D( {e_b1.get_midpoint(), e_d2.get_midpoint(), e_d1.get_midpoint(), e_a2.get_midpoint()}
        #ifdef DEBUG_COLOR
        ,128, 128, 128 // teal
        #endif
      )
    );
    // > Triangle 2
    faces.push_back(
      Face3D( {e_d1.get_midpoint(), e_c1.get_midpoint(), e_a2.get_midpoint()}
        #ifdef DEBUG_COLOR
        ,128, 128, 128 // teal
        #endif
      )
    );
    return faces;
  }
  else {
    std::cerr << "Unknown case of V4: " << "X" << std::endl;
    return {};
  }
}


std::vector<Face3D> SingleVertexCase(const CubeVertexes& cube, const CubeColors &colors) {
  // Find vertex with color
  size_t vertex_index = 0;
  for (size_t i = 0; i < 8; i++) {
    if (colors[i]) {
      vertex_index = i;
      break;
    }
  }
  // Create faces
  auto face = getFacesVertex1(cube, {vertex_index});
  return {face};
}




std::vector<Face3D> cubeCases(
  CubeVertexes cube,
  std::function<double(double, double, double)> func
) {
  CubeColors colors;
  // Get colors for each vertex
  for (size_t i = 0; i < 8; i++) {
    colors[i] = getColor(func(cube[i].x, cube[i].y, cube[i].z));
  }
  // Get color count
  int color_count = 0;
  for (size_t i = 0; i < 8; i++) {
    if (colors[i]) {
      color_count++;
    }
  }
  // If color count >4 flip, so all cases are <=4
  if (color_count > 4) {
    flipColors(colors);
    color_count = 0;
    for (size_t i = 0; i < 8; i++) {
      if (colors[i]) {
        color_count++;
      }
    }
  }
  // Split into subproblems
  auto subproblems = getAdyacentColoredVertices(colors);
  // Get faces for each subproblem
  std::vector<Face3D> result_faces;
  std::vector<Face3D> temp_faces;
  //if (subproblems.size() > 1) {
  //  std::cout << "Subproblems: " << subproblems.size() << std::endl;
  //}
  //if (subproblems.size() ==0 ) {
  //std::cout << "No subproblems" << std::endl;
  //}
  for (const std::vector<size_t>& subproblem : subproblems) {
    // Get the number of vertices
    size_t subproblem_size = subproblem.size();
    // Get the faces
    switch (subproblem_size)
    {
      case 0:
        //std::cout << "Empty subproblem" << std::endl;
        temp_faces = {};
        break;
      case 1:
        temp_faces = getFacesVertex1(cube, subproblem);
        break;
      case 2:
        temp_faces = getFacesVertex2(cube, subproblem);
        break;
      case 3:
        temp_faces = getFacesVertex3(cube, subproblem);
        break;
      case 4:
        temp_faces = getFacesVertex4(cube, subproblem);
        break;
      default:
        temp_faces = {};
        std::cerr << "Unknown case: " << subproblem_size << std::endl;
        break;
    }
    // Add faces to the list
    for (const Face3D& face : temp_faces) {
      result_faces.push_back(face);
    }
  }
  return result_faces;
}


std::vector<Face3D> adaptativeMarchingCubes(
  std::function<double(double, double, double)> func,
  double x_start,
  double y_start,
  double z_start,
  double x_end,
  double y_end,
  double z_end,
  double precision,
  size_t samples = 50000
) {
  // Return value
  std::vector<Face3D> faces;
  auto width = x_end - x_start;
  auto height = y_end - y_start;
  auto depth = z_end - z_start;
  // Split the space in cubes
  double dx = width / 2.0;
  double dy = height / 2.0;
  double dz = depth / 2.0;
  // Random sample generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dis_x(0, dx);
  std::uniform_real_distribution<double> dis_y(0, dy);
  std::uniform_real_distribution<double> dis_z(0, dz);
  // Sample squares
  for (double x = x_start; x < x_end; x += dx) {
    for (double y = y_start; y < y_end; y += dy) {
      for (double z = z_start; z < z_end; z += dz) {
        // Sample the cube
        bool positive = false;
        bool negative = false;
        for (size_t i = 0; i < samples; i++) {
          double x_sample = x + dis_x(gen);
          double y_sample = y + dis_y(gen);
          double z_sample = z + dis_z(gen);
          double value = func(x_sample, y_sample, z_sample);
          double sign = getSign(value);
          if (sign == 0) {
            positive = true;
            negative = true;
            break;
          } else if (sign > 0) {
            positive = true;
            if (negative) {break;} // Early exit
          } else {
            negative = true;
            if (positive) {break;} // Early exit
          }
        }
        // If different signs
        if (positive && negative) {

          // If dx, dy, dz are less than precision
          if (dx < precision && dy < precision && dz < precision) {
            // Get faces from cases
            CubeVertexes cube = {
              Vertex3D(x, y, z),
              Vertex3D(x + dx, y, z),
              Vertex3D(x + dx, y + dy, z),
              Vertex3D(x, y + dy, z),
              Vertex3D(x, y, z + dz),
              Vertex3D(x + dx, y, z + dz),
              Vertex3D(x + dx, y + dy, z + dz),
              Vertex3D(x, y + dy, z + dz)
            };
            std::vector<Face3D> cube_faces = cubeCases(cube, func);
            // Add faces to the list
            for (const Face3D& face : cube_faces) {
              if (face.vertices.size() <= 1) {
                std::cerr << "Face with less than 2 vertices" << std::endl;
                continue;
              }
              faces.push_back(face);
            }
          }
          // Else recursive
          else {
            std::vector<Face3D> sub_faces = adaptativeMarchingCubes(
              func, x, y, z, x + dx, y + dy, z + dz, precision, samples
            );
            for (const Face3D& face : sub_faces) {
              faces.push_back(face);
            }
          }
        }
      }
    }
  }
  return faces;
}

void draw_mesh(
  std::function<double(double, double, double)> f,
  const std::string& filename,
  double x_min, double y_min, double z_min,
  double x_max, double y_max, double z_max,
  double precision
) {
  std::vector<Face3D> faces = adaptativeMarchingCubes(
      f,
      x_min, y_min, z_min,
      x_max, y_max, z_max,
      precision
  );
    std::cout << "Faces: " << faces.size() << std::endl;
  /*
  std::cout << "Faces: " << faces.size() << std::endl;
  for (size_t i = 0; i < faces.size(); i++) {
    std::cout << "Face " << i << ": " << std::endl;
    for (size_t j = 0; j < faces[i].vertices.size(); j++) {
      std::cout << "  Vertex " << j << ": (" << faces[i].vertices[j].x << ", " << faces[i].vertices[j].y << ", " << faces[i].vertices[j].z << ")" << std::endl;
    }
  }
  */
  // Create mesh
  Mesh mesh(faces);
  // Save mesh
  mesh.save(filename.c_str());
}

int main() {
  draw_mesh(
    f,
    "out.ply",
    -2,-2,-2,
    2,2,2,
    //0.5
    0.125
  );
  return 0;
}
