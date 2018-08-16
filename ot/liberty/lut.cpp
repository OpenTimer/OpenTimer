#include <ot/liberty/lut.hpp>

namespace ot {

// Function: is_time_lut_var
bool is_time_lut_var(LutVar v) {
  switch(v) {
    case LutVar::INPUT_NET_TRANSITION:
    case LutVar::CONSTRAINED_PIN_TRANSITION:
    case LutVar::RELATED_PIN_TRANSITION:
    case LutVar::INPUT_TRANSITION_TIME:
      return true;
    break;

    default:
      return false;
    break;
  }
}

// Function: is_capacitance_lut_var
bool is_capacitance_lut_var(LutVar v) {
  switch(v) {
    case LutVar::TOTAL_OUTPUT_NET_CAPACITANCE:
      return true;
    break;

    default:
      return false;
    break;
  }
}

// Function: to_string
std::string to_string(LutVar v) {
  
  switch(v) {
    case LutVar::TOTAL_OUTPUT_NET_CAPACITANCE:
      return "total_output_net_capacitance";
    break;

    case LutVar::INPUT_NET_TRANSITION:
      return "input_net_transition";
    break;

    case LutVar::CONSTRAINED_PIN_TRANSITION:
      return "constrained_pin_transition";
    break;

    case LutVar::RELATED_PIN_TRANSITION:
      return "related_pin_transition";
    break;
    
    case LutVar::INPUT_TRANSITION_TIME:
      return "input_transition_time";
    break;

    default:
      return "undefined";
    break;
  }
}

// ------------------------------------------------------------------------------------------------

// Operator: <<
std::ostream& operator << (std::ostream& os, const LutTemplate& lut) {

  // Write the lut template name.
  os << "lu_table_template (" << lut.name << ") {\n";
  
  // Write variables.
  if(lut.variable1) {
    os << "  variable_1: " << to_string(*(lut.variable1)) << ";\n";
  }

  if(lut.variable2) {
    os << "  variable_2: " << to_string(*(lut.variable2)) << ";\n";
  }

  // Write indices.
  if(!lut.indices1.empty()) {
    os << "  index_1 (\"";
    for(size_t i=0; i<lut.indices1.size(); i++) {
      if(i) {
        os << ", ";
      }
      os << lut.indices1[i];
    }
    os << "\");\n";
  }
  
  if(!lut.indices2.empty()) {
    os << "  index_2 (\"";
    for(size_t i=0; i<lut.indices2.size(); i++) {
      if(i) {
        os << ", ";
      }
      os << lut.indices2[i];
    }
    os << "\");\n";
  }

  // Write the lut template ending group symbol.
  os <<"}\n";
  
  return os;
}

// ------------------------------------------------------------------------------------------------

// Function: scale_time
void Lut::scale_time(float s) {
   
  if(lut_template) {
    if(auto v1 = lut_template->variable1; v1 && is_time_lut_var(*v1)) {
      for(auto& v : indices1) {
        v *= s;
      }
    }
    if(auto v2 = lut_template->variable2; v2 && is_time_lut_var(*v2)) {
      for(auto& v : indices2) {
        v *= s;
      }
    }
  }

  // scale the table
  for(auto& v : table) {
    v *= s;
  }
}

// Function: scale_capacitance
void Lut::scale_capacitance(float s) {
   
  if(lut_template) {
    if(auto v1 = lut_template->variable1; v1 && is_capacitance_lut_var(*v1)) {
      for(auto& v : indices1) {
        v *= s;
      }
    }
    if(auto v2 = lut_template->variable2; v2 && is_capacitance_lut_var(*v2)) {
      for(auto& v : indices2) {
        v *= s;
      }
    }
  }
}

// Function: is_scalar
bool Lut::is_scalar() const {
  return indices1.size() == 1 && indices2.size() == 1;
}

// Function: empty
inline bool Lut::empty() const {
  return indices1.size() == 0 && indices2.size() == 0;
}

// Function: lut
// Performs the linear inter/extra polation between a segment (x1, x2) which satisfies the 
// function f(x1) = y1 and f(x2) = y2. There are five cases: 1) x < x1, 2) x = x1,
// 3) x1 < x < x2, 4) x = x2, and 5) x > x2. For cases 1) and 5), extra-polation is needed.
// Cases 2) and 4) are boundary cases. Case 3) requires the inter-polation.
float Lut::operator()(float val1, float val2) const {
  
  if(indices1.size() < 1 || indices2.size() < 1) {
    OT_LOGF("invalid lut indices size");
  }
  
  // Interpolation
  constexpr auto interpolate = [] (float x, float x1, float x2, float y1, float y2) {

    assert(x1 < x2);

    if(x >= std::numeric_limits<float>::max() || x <= std::numeric_limits<float>::lowest()) {
      return x;
    }
  
    float slope = (y2 - y1) / (x2 - x1);
  
    if(x < x1) return y1 - (x1 - x) * slope;                  // Extrapolation.
    else if(x > x2)  return y2 + (x - x2) * slope;            // Extrapolation.
    else if(x == x1) return y1;                               // Boundary case.
    else if(x == x2) return y2;                               // Boundary case.
    else return y1 + (x - x1) * slope;                        // Interpolation.
  };

  // Case 1: scalar
  if(is_scalar()) return table[0];

  int idx1[2], idx2[2];

  idx1[1] = std::lower_bound(indices1.begin(), indices1.end(), val1) - indices1.begin();
  idx2[1] = std::lower_bound(indices2.begin(), indices2.end(), val2) - indices2.begin();
  
  // Case 2: linear inter/extra polation.
  idx1[1] = std::max(1, std::min(idx1[1], (int)(indices1.size() - 1)));
  idx2[1] = std::max(1, std::min(idx2[1], (int)(indices2.size() - 1)));
  idx1[0] = idx1[1] - 1;
  idx2[0] = idx2[1] - 1;

  //printf("Perform the linear interpolation on val1=%.5f (%d %d) and val2=%.5f (%d %d)\n", 
  //        val1, idx1[0], idx1[1], val2, idx2[0], idx2[1]);
  
  // 1xN array (N>=2)
  if(indices1.size() == 1) {  
    return interpolate(
      val2, 
      indices2[idx2[0]], 
      indices2[idx2[1]], 
      table[idx2[0]],
      table[idx2[1]]
    );
  }
  // Nx1 array (N>=2)
  else if(indices2.size() == 1) {   
    return interpolate(
      val1, 
      indices1[idx1[0]], 
      indices1[idx1[1]], 
      table[idx1[0]*indices2.size()], 
      table[idx1[1]*indices2.size()]
    );
  }
  // NxN array (N>=2)
  else {      
    float numeric[2];
    
    numeric[0] = interpolate(
      val1, 
      indices1[idx1[0]], 
      indices1[idx1[1]], 
      table[idx1[0]*indices2.size() + idx2[0]],
      table[idx1[1]*indices2.size() + idx2[0]]
    );

    numeric[1] = interpolate(
      val1, 
      indices1[idx1[0]], 
      indices1[idx1[1]], 
      table[idx1[0]*indices2.size() + idx2[1]],
      table[idx1[1]*indices2.size() + idx2[1]]
    );

    return interpolate(val2, indices2[idx2[0]], indices2[idx2[1]], numeric[0], numeric[1]);
  }
}

// operator
std::ostream& operator << (std::ostream& os, const Lut& lut) {

  // Write the indices1.
  if(!lut.indices1.empty()) {
    os << "        index_1 (\"";
    for(size_t i=0; i<lut.indices1.size(); ++i) {
      if(i) {
        os << ", ";
      }
      os << lut.indices1[i];
    }
    os << "\");\n";
  }
         
  // Write the indices2.
  if(!lut.indices2.empty()) {
    os << "        index_2 (\"";
    for(size_t i=0; i<lut.indices2.size(); ++i) {
      if(i) {
        os << ", ";
      }
      os << lut.indices2[i];
    }
    os << "\");\n";
  }

  // Write the values.
  if(!lut.table.empty()) {
    os << "        values (\n";
    for(size_t i=0; i<lut.indices1.size(); ++i) {
      os << "          \"";
      for(size_t j=0; j<lut.indices2.size(); ++j) {
        if(j) {
          os << ", ";
        }
        os << lut.table[i*lut.indices2.size()+j];
      }
      os << "\",\n";
    }
    os << "        );\n";
  }

  return os;
}




};  // end of namespace ot ------------------------------------------------------------------------
