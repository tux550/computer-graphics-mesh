#include <iostream>
#include <vector>
#include <fstream>
#include <optional>
#include <limits>
#include <string>
#include "mesh.h"
#include "3d.h"
#include "SplittingEdges.h"

using namespace mesh;

struct RGB{
  double r, g, b;

  RGB operator+(const RGB& other){
    return RGB{r + other.r, g + other.g, b + other.b};
  }

  RGB operator=(const RGB& other){
    r = other.r;
    g = other.g;
    b = other.b;
    return *this;
  }

  RGB operator*(double scalar){
    return RGB{r * scalar, g * scalar, b * scalar};
  }
};

struct Point2D{
  double x, y;
};

struct Camera{
  int width, height;
  int distance;
  double scale;
  std::vector<std::vector<RGB>> pixels;

  Camera(int width, int height, double distance, double scale=0.25) : width(width), height(height), distance(distance), scale(scale)
  {
    // Initialize pixels
    pixels = std::vector<std::vector<RGB>>(width, std::vector<RGB>(height, RGB{0, 0, 0}));
  }


  Point2D getFilmPoint(int px, int py){
    // Get film point
    double x = (px - width / 2.0) * scale;
    double y = (py - height / 2.0) * scale;
    return Point2D{x, y};
  }

  Point2D toPixel(Point2D point){
    // Transform point to pixel
    return Point2D{point.x * scale + width / 2.0, point.y * scale + height / 2.0};
  }

  Point2D projectToFilm(Vertex3D vertex){
    // Project vertex to film
    double x = distance* vertex.x / vertex.z;
    double y = distance* vertex.y / vertex.z;
    return Point2D{x, y};
  }

  Point2D projectToPixel(Vertex3D vertex) {
    // Project vertex to pixel
    auto film_point = projectToFilm(vertex);
    return toPixel(film_point);
  }



  Line3D getRay(int px, int py){
    // Get film point
    auto film_point = getFilmPoint(px, py);
    double x = film_point.x;
    double y = film_point.y;
    auto dir = Vertex3D(x, y, distance);
    // direction
    auto origin = Vertex3D(0, 0, 0);
    return Line3D(origin, dir);
  }

  void setPixel(int px, int py, RGB color){
    pixels[px][py] = color;
  }

  RGB getPixel(int px, int py){
    return pixels[px][py];
  }

  void savePGM(std::string filename){
    std::ofstream file;
    file.open(filename);
    file << "P3\n";
    file << width << " " << height << "\n";
    file << "255\n";
    for (auto x = 0; x < width; x++){
      for (auto y = 0; y < height; y++){
        auto color = pixels[x][y];
        file << (int) color.r << " " << (int) color.g << " " << (int) color.b << "\n";
      }
    }
    file.close();
  }
};


bool inside_convex_polygon(std::vector<Point2D> points, Point2D p){
  // Check if point is inside convex polygon
  for (int i = 0; i < points.size(); i++){
    auto p1 = points[i];
    auto p2 = points[(i + 1) % points.size()];
    auto v1 = Point2D{p2.x - p1.x, p2.y - p1.y};
    auto v2 = Point2D{p.x - p1.x, p.y - p1.y};
    // Cross product
    auto cross = v1.x * v2.y - v1.y * v2.x;
    if (cross < 0){
      return false;
    }
  }
  return true;
}


int main(){
  auto camera = Camera{200, 200, 5, 0.0078125}; //0.125};
  auto meshes = std::vector<Mesh>{}; //unitCircleTetrahedron()};
  // Create circle

  std::cout << "Creating circle" << std::endl;
  auto sphere = sphereBySplittingEdges(3);
  std::cout << "Circle created with " << sphere.get_faces().size() << " faces" << std::endl;
  // Move by 20 away from camera
  sphere.displace(Vertex3D(0, 0, 8));
  // Get sample vertex of first face
  auto face = sphere.get_face(0);
  for (auto vertex : face.vertices){
    std::cout << "Vertex: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;
  }

  meshes.push_back(sphere);

  std::cout << "Sorting" << std::endl;
  auto faces_distance = std::vector<std::pair<double, Face3D>>{};
  for (auto mesh : meshes){
    for (auto face : mesh.get_faces()){
      auto midpoint = face.get_midpoint();
      // If midpoint is behind camera, skip
      if (midpoint.z < camera.distance){
        continue;
      }
      auto distance = midpoint.magnitude();
      faces_distance.push_back(std::make_pair(distance, face));
    }
  }
  // Sort by distance. Furthest first
  std::sort(faces_distance.begin(), faces_distance.end(), [](auto a, auto b){
    return a.first > b.first;
  });
  // Get faces
  auto faces = std::vector<Face3D>{};
  for (auto pair : faces_distance){
    faces.push_back(pair.second);
  }




  std::cout << "Rendering" << std::endl;
  // Render
  int count = 0;
  int max = faces.size();
  // Get number of steps that equal 1%
  int step = max / 100.0;
  for (auto face : faces){
    // Output progress bar every 1%
    if (count % step == 0){
      std::cout << "Progress: " << count / step  << "%" << "(" << count << "/" << max << ")" << std::endl;
    }
    count++;

    // Get projection of each vertex
    std::vector<Point2D> points;
    for (auto vertex : face.vertices){
      auto point = camera.projectToFilm(vertex);
      points.push_back(point);
    }
    // Get bounding box
    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::min();
    double max_y = std::numeric_limits<double>::min();
    for (auto point : points){
      min_x = std::min(min_x, point.x);
      min_y = std::min(min_y, point.y);
      max_x = std::max(max_x, point.x);
      max_y = std::max(max_y, point.y);
    }
    // Draw face
    for (auto x = min_x; x < max_x; x++){
      for (auto y = min_y; y < max_y; y++){
        // Check if point is inside convex_polygon
        auto p = camera.getFilmPoint(x, y);
        if (inside_convex_polygon(points, p)){
          // Get previous color
          auto old_color = camera.getPixel(x, y);
          if (old_color.r == 0 && old_color.b == 0 && old_color.g == 0){
            std::cout << "First write on " << x << ", " << y << std::endl;
            std::cout << "Vertexes: " << std::endl;
            for (auto point : points){
              std::cout << "(" << point.x << ", " << point.y << ")" << std::endl;
            }
          }
          else {
            std::cout << "Overwriting on " << x << ", " << y << std::endl;
          }
          // Get color of face
          auto color = RGB{255, 255, 255};
          // Normal
          auto normal = face.get_normal();
          // Ray from 0,0,0 to center of face
          auto ray = face.get_midpoint();
          // Get intesity color
          auto cos_angle = dot_product(normal, ray) / (normal.magnitude() * ray.magnitude());
          // If is positive, ignore
          color = color * std::abs(cos_angle);
          // Draw pixel
          camera.setPixel(x, y, color);
        }
      }
    }
  
  }
  // Save
  camera.savePGM("outputs/pintor.ppm");
}