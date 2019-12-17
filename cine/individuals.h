#ifndef CINE2_INDIVIDUALS_H_INCLUDED
#define CINE2_INDIVIDUALS_H_INCLUDED

#include <vector>
#include <type_traits>
#include <iterator>
#include "rndutils.hpp"
#include "ann.hpp"
#include "rnd.hpp"
#include "landscape.h"    // Coordinate


namespace cine2 {


  struct Individual
  {
    Individual() : pos(0,0), food(0), ancestor(0)
    {
    }

    void sprout(Coordinate Pos, int ancestor_idx)
    {
      pos = Pos;
      food = 0.f;
      ancestor = ancestor_idx;
    }
    
    bool alive() const { return food >= 0.f; }
    void die() { food = -1.f; }

    Coordinate pos;
    float food;       
    int ancestor;
  };
  

}



#endif
