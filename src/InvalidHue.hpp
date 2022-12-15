
#pragma once
#include <exception>
#include <string> 

class InvalidHue : std::exception {
    public:
        std::string message;
        InvalidHue(const std::string mes){
            message = mes; 
        }
        std::string what () {
            return "Invalid Hue Exception "  + message;
        }
};