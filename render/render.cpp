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


  Line3D getRay(int px, int py){
    // Get film point
    double x = (px - width / 2.0) * scale;
    double y = (py - height / 2.0) * scale;
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



int main(){
  auto camera = Camera{200, 200, 5, 1};
  auto meshes = std::vector<Mesh>{unitCircleTetrahedron()};
  // Create circle
  std::cout << "Creating circle" << std::endl;
  auto sphere = sphereBySplittingEdges(3);
  std::cout << "Circle created with " << sphere.get_faces().size() << " faces" << std::endl;
  // Move by 20 away from camera
  sphere.displace(Vertex3D(0, 0, 20));
  // Get sample vertex of first face
  auto face = sphere.get_face(0);
  for (auto vertex : face.vertices){
    std::cout << "Vertex: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;
  }

  meshes.push_back(sphere);
  
  std::cout << "Rendering" << std::endl;
  // Render
  for (auto x = 0; x < camera.width; x++){
    for (auto y = 0; y < camera.height; y++){
      if (x % 100 == 0 && y % 100 == 0){
        std::cout << "Rendering pixel (" << x << ", " << y << ")" << std::endl;
      }
      // Draw ray
      auto ray = camera.getRay(x, y);
      // Find intersection
      auto min_distance = std::numeric_limits<double>::max();
      for (auto mesh : meshes){
        for (auto face : mesh.get_faces()){
          auto intersection_opt = face.intersect(ray);
          if (intersection_opt.has_value()){
            auto intersection_point = intersection_opt.value();
            // If intersection is behind the camera, ignore
            if (intersection_point.z < camera.distance){
              std::cout << "Intersection behind camera at:" << intersection_point.x << ", " << intersection_point.y << ", " << intersection_point.z << std::endl;
              continue;
            }
            auto distance = (ray.point - intersection_point).magnitude();
            if (distance < min_distance){
              min_distance = distance;
              // Get color
              auto color = RGB{255, 255, 255};
              // Multiply by cos of angle between ray and normal
              auto normal = face.get_normal();
              auto direction = ray.direction;
              auto cos_angle = dot_product(normal, direction) / (normal.magnitude() * direction.magnitude());
              color = color * std::abs(cos_angle);
              // set pixel
              camera.setPixel(x, y, color);
            }
          }
        }
      }
    }
  }
  // Save
  camera.savePGM("outputs/render.ppm");

}