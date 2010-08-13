#include "ArgumentDescription.hpp"

using namespace RTT;

ArgumentDescription::ArgumentDescription (const std::string _name, const std::string _desc, const std::string _type)
    : name( _name), description(_desc), type(_type)
{}
