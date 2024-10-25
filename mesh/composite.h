// Composite functions

#ifndef MESH_COMPOSITE_H_
#define MESH_COMPOSITE_H_

#include <vector>
#include <functional>


namespace mesh{

enum OperationType {
  UNION,
  INTERSECT,
  SUBSTRACT,
};


template <typename F1, typename F2>
struct CompositeFunctor3D {
  F1 base1;
  F2 base2;
  OperationType operation;

  // Constructor
  CompositeFunctor3D(F1 base1, F2 base2, OperationType operation) : base1(base1), base2(base2), operation(operation) {}

  // Overload operator() to call the function
  double operator()(double x, double y, double z) const {
    double value1 = base1(x, y, z);
    double value2 = base2(x, y, z);
    if (operation == UNION) {
      return std::min(value1, value2);
    } else if (operation == INTERSECT) {
      return std::max(value1, value2);
    } else if (operation == SUBSTRACT) {
      return std::max(value1, -value2);
    }
    return 0;
  }
};


}; // namespace mesh


#endif // MESH_COMPOSITE_H_