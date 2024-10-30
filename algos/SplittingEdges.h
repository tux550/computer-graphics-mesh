#ifndef SPLITTINGEDGES_H
#define SPLITTINGEDGES_H

#include <cmath>
#include "mesh.h"
#include "geometry.h"

using namespace mesh;


Mesh sphereBySplittingEdges(int n);

#endif

/*
int main() {
  Mesh sphere = sphereBySplittingEdges(3);
  sphere.save("outputs/sphere.ply");
  return 0;
}
*/