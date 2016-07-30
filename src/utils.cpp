// Copyright 2016 Marco Di Cristina, Alessandro Fabbri
#include "utils.h"

int myrounder(const double &x) {
  int n;
  if (x >= 0)
    n = int(x);
  else
    n = -int(-x) - 1;
  return n;
}
