#include <functional>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>

struct Line {
  double x0, y0;
  double x1, y1;
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

std::vector<Line> squareCases(
  double x0, double y0,
  double x1, double y1,
  std::function<double(double, double)> func
) {
  double v_00 = func(x0, y0);
  double v_10 = func(x1, y0);
  double v_11 = func(x1, y1);
  double v_01 = func(x0, y1);
  unsigned int c_00 = v_00 > 0 ? 1 : 0;
  unsigned int c_10 = v_10 > 0 ? 1 : 0;
  unsigned int c_11 = v_11 > 0 ? 1 : 0;
  unsigned int c_01 = v_01 > 0 ? 1 : 0;

  double x_mid = (x0 + x1) / 2.0;
  double y_mid = (y0 + y1) / 2.0;
  // Bitwise to get the case
  unsigned int case_id = c_00 + 2 * c_10 + 4 * c_11 + 8 * c_01;
  // Get lines
  switch (case_id)
  {
    // Fully in or out cases
    case 0:
    case 15:
      return {};
    // Corner Low-left line
    case 1:
    case 14:
      return {Line{x0, y_mid, x_mid, y0}};
    // Corner Low-right line
    case 2:
    case 13:
      return {Line{x_mid, y0, x1, y_mid}};
    // Corner Top-right line
    case 4:
    case 11:
      return {Line{x_mid, y1, x1, y_mid}};
    // Corner Top-left line
    case 7:
    case 8:
      return {Line{x0, y_mid, x_mid, y1}};
    // Horizontal Line
    case 3:
    case 12:
      return {Line{x0, y_mid, x1, y_mid}};
    // Vertical Line
    case 6:
    case 9:
      return {Line{x_mid, y0, x_mid, y1}};
    // Double diagonal (//)
    case 5:
      return {
        Line{x0, y_mid, x_mid, y1},
        Line{x_mid, y0, x1, y_mid}
      };
    // Double diagonal (\\)
    case 10:
      return {
        Line{x_mid, y1, x1, y_mid},
        Line{x0, y_mid, x_mid, y0}
      };
    // Unknown case
    default:
      std::cerr << "Unknown case: " << case_id << std::endl;
      return {};
  }

}

std::vector<Line> adaptativeMarchingSquares(
  std::function<double(double, double)> func,
  double x_start,
  double y_start,
  double width,
  double height,
  double px = 0.01, // Max precision in x
  double py = 0.01, // Max precision in y
  size_t samples = 1000
) {
  // Return value
  std::vector<Line> lines;
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
      // If different signs
      if (positive && negative) {
        // If dx and dy are small enough, then return
        if (dx < px && dy < py) {
          // Get lines from cases
          std::vector<Line> sqlines = squareCases(
            x, y, x + dx, y + dy,
            func
          );
          // Add lines to the list
          for (const Line& line : sqlines) {
            lines.push_back(line);
          }
        }
        // Else recursive
        else {
          std::vector<Line> sub_lines = adaptativeMarchingSquares(
            func, x, y, dx, dy, px, py, samples
          );
          for (const Line& line : sub_lines) {
            lines.push_back(line);
          }
        }
      }
    }
  }
  return lines;
}

void writeToEPS(
  double min_x, double max_x,
  double min_y, double max_y,
  
  const std::vector<Line>& lines, const std::string& filename) {
  // Get min and max values
  //double min_x = std::numeric_limits<double>::max();
  //double max_x = std::numeric_limits<double>::min();
  //double min_y = std::numeric_limits<double>::max();
  //double max_y = std::numeric_limits<double>::min();
  /*for (const Line& line : lines) {
    min_x = std::min(min_x, std::min(line.x0, line.x1));
    max_x = std::max(max_x, std::max(line.x0, line.x1));
    min_y = std::min(min_y, std::min(line.y0, line.y1));
    max_y = std::max(max_y, std::max(line.y0, line.y1));
  }*/

  std::ofstream file(filename);
  file << "%!PS-Adobe-3.0 EPSF-3.0" << std::endl;
  file << "%%BoundingBox: " << min_x << " " << min_y << " " << max_x << " " << max_y << std::endl;
  file << "0.01 setlinewidth" << std::endl;
  for (const Line& line : lines) {
    file << "newpath" << std::endl;
    file << line.x0 << " " << line.y0 << " moveto" << std::endl;
    file << line.x1 - line.x0 << " " << line.y1 - line.y0 << " rlineto" << std::endl;
    file << "closepath" << std::endl;
    file << "stroke" << std::endl;
  }
  file << "showpage" << std::endl;
}

int main() {
  double min_x = -2;
  double min_y = -2;
  double width = 4;
  double height = 4;

  std::vector<Line> lines = adaptativeMarchingSquares(
    f,
    min_x, min_y,
    width, height);
  // Get stats
  std::cout << "Lines: " << lines.size() << std::endl;

  writeToEPS(
    min_x, min_x + width, min_y, min_y + height,
    lines, "implicit_line.eps");
  return 0;
}
