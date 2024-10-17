#include <functional>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>

struct Square {
  double x, y;
  double width, height;
  double value;
};

double getSign(double value) {
  if (value == 0) {
    return 0;
  }
  return value > 0 ? 1 : -1;
}


double f(double x, double y) {
    return 0.004 
           + 0.110 * x - 0.177 * y 
           - 0.174 * x * x + 0.224 * x * y - 0.303 * y * y
           - 0.168 * x * x * x + 0.327 * x * x * y - 0.087 * x * y * y - 0.013 * y * y * y
           + 0.235 * x * x * x * x - 0.667 * x * x * x * y + 0.745 * x * x * y * y 
           - 0.029 * x * y * y * y + 0.072 * y * y * y * y;
}

void writeToEPS(std::vector<Square> squares, std::string filename) {
  // Get min and max values
  double min_x = std::numeric_limits<double>::max();
  double max_x = std::numeric_limits<double>::min();
  double min_y = std::numeric_limits<double>::max();
  double max_y = std::numeric_limits<double>::min();
  for (auto square : squares) {
    min_x = std::min(min_x, square.x);
    max_x = std::max(max_x, square.x + square.width);
    min_y = std::min(min_y, square.y);
    max_y = std::max(max_y, square.y + square.height);
  }
  // Open file
  std::ofstream file(filename);
  file << "%!PS-Adobe-3.0 EPSF-3.0" << std::endl;
  file << "%%BoundingBox: " << min_x << " " << min_y << " " << max_x << " " << max_y << std::endl;
  for (auto square : squares) {
    if (square.value > 0) {
      file << "0 0 1 setrgbcolor" << std::endl;
    }
    else if (square.value < 0) {
      file << "1 0 0 setrgbcolor" << std::endl;
    }
    else {
      file << "0 1 0 setrgbcolor" << std::endl;
    }
    file << "newpath" << std::endl;
    file << square.x  << " " << square.y  << " moveto" << std::endl;
    file << square.width  << " 0 rlineto" << std::endl;
    file << "0 " << square.height  << " rlineto" << std::endl;
    file << -square.width  << " 0 rlineto" << std::endl;
    file << "closepath" << std::endl;
    file << "fill" << std::endl;
  }
  file << "showpage" << std::endl;
  file.close();

  
}

std::vector<Square> adaptativeMarchingSquares(
  std::function<double(double, double)> func,
  double x_start,
  double y_start,
  double width,
  double height,
  double px = 0.01, // Max precision in x
  double py = 0.01, // Max precision in y
  size_t samples = 1000
) {
  //std::cout << "MS: " << x_start << " " << y_start << " " << width << " " << height << std::endl;
  // Return value
  std::vector<Square> squares;
  // Split the space in squares
  double dx = width / 8;
  double dy = height / 8;
  // Random sample generator
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis_x(0, dx);
  std::uniform_real_distribution<> dis_y(0, dy);
  // Sample squares
  for (double x = x_start; x < x_start + width; x += dx) {
    for (double y = y_start; y < y_start + height; y += dy) {
      // Sample the square
      bool positive = false;
      bool negative = false;
      for (size_t i = 0; i < samples; i++) {
        double x_sample = x + dis_x(gen);
        double y_sample = y + dis_y(gen);
        double value = func(x_sample, y_sample);
        double sign = getSign(value);
        if (sign == 0) {
          positive = true;
          negative = true;
        } else if (sign > 0) {
          positive = true;
        } else {
          negative = true;
        }
      }
      // Only one sing, then return
      if (positive && !negative || !positive && negative) {
        squares.push_back(Square{x, y, dx, dy, positive ? 1.0 : -1.0});
      }
      // If dx and dy are small enough, then return
      else if (dx < px && dy < py) {
        squares.push_back(Square{x, y, dx, dy, 0.0});
      }
      // Mutiple signs, then recursive
      else {
        std::vector<Square> sub_squares = adaptativeMarchingSquares(
          func, x, y, dx, dy, px, py, samples
        );
        squares.insert(squares.end(), sub_squares.begin(), sub_squares.end());
      }
    }
  }
  return squares;
}


int main() {
  auto func = [](double x, double y) {
    return x * x + y * y - 1;
  };
  std::vector<Square> squares = adaptativeMarchingSquares(f, -4, -4, 8, 8);
  writeToEPS(squares, "implicit.eps");
  return 0;
}
