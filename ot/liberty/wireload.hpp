#ifndef OT_LIBERTY_WIRELOAD_HPP_
#define OT_LIBERTY_WIRELOAD_HPP_

namespace ot {
    
    struct Wireload {
        std::string name;
        
        float capacitance;
        float slope;
        float resistance;
    };
};

#endif