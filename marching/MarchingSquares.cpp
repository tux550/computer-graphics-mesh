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
  // Color of the vertex: 1 if positive, 0 if negative
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
  double x_end,
  double y_end,
  double precision,
  size_t samples = 1000
) {
  // Return value
  std::vector<Line> lines;
  auto width = x_end - x_start;
  auto height = y_end - y_start;
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
        // If dx and dy are small enough, then return
        if (dx < precision && dy < precision) {
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
            func, x, y, x + dx, y + dy, precision, samples
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
  
  const double OUT_HEIGHT = 1000;
  const double OUT_WIDTH = 1000;

  const double width = max_x - min_x;
  const double height = max_y - min_y;

  const double rescale_factor = std::min(OUT_HEIGHT / height, OUT_WIDTH / width);

  auto rescale_x = [min_x, rescale_factor](double x) {
    return (x - min_x) * rescale_factor;
  };
  auto rescale_y = [min_y, rescale_factor](double y) {
    return (y - min_y) * rescale_factor;
  };


  std::ofstream file(filename);
  file << "%!PS-Adobe-3.0 EPSF-3.0" << std::endl;
  file << "%%BoundingBox: " << 0 << " " << 0 << " " << OUT_WIDTH << " " << OUT_HEIGHT << std::endl;
  file << "0.01 setlinewidth" << std::endl;
  for (const Line& line : lines) {
    file << "newpath" << std::endl;
    file << rescale_x(line.x0) << " " << rescale_y(line.y0) << " moveto" << std::endl;
    file << rescale_x(line.x1) << " " << rescale_y(line.y1) << " lineto" << std::endl;
    file << "closepath" << std::endl;
    file << "stroke" << std::endl;
  }
  file << "showpage" << std::endl;
}

void draw_curve(
  std::function<double(double, double)> f,
  const std::string& filename,
  double x_min, double y_min,
  double x_max, double y_max,
  double precision
) {
  std::vector<Line> lines = adaptativeMarchingSquares(
      f,
      x_min, y_min,
      x_max, y_max,
      precision);
  writeToEPS(
    x_min, x_max, y_min, y_max,
    lines, filename);
}

int main() {
  draw_curve(
    f,
    "outputs/implicit.eps",
    -4, -4,
    4, 4,
    0.01
  );
  return 0;
}
