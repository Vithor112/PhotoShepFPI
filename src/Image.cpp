#include "Image.hpp"
#include <stdexcept>
#include "InvalidHue.hpp" 
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"

Image::Image(std::string name){
    _img_ptr = stbi_load("sky.jpeg", &_width, &_height, &_orig_channels, 0);
    this->_channels = _orig_channels; 
    if(_img_ptr == NULL) {
        printf("Error in loading the image %s \n", name.c_str());
        throw std::runtime_error("Error in loading the image " +  name ); 
    }
}

Image::Image(std::string name, int channels){
    _img_ptr = stbi_load("sky.jpeg", &_width, &_height, &_orig_channels, channels);
    this->_channels = channels; 
    if(_img_ptr == NULL) {
        printf("Error in loading the image\n");
        throw std::runtime_error("Error in loading the image"); 
    }
}

Image::~Image() {
    stbi_image_free(_img_ptr);  
}

Image::Image(Image &other){
    this->_channels = other.getChannels();
    this->_width = other.getWidth();
    this->_height = other.getHeight();
    this->_orig_channels = other.getOrigChannels();
    this->_img_ptr = (unsigned char *) malloc(other.getImageSize()*sizeof(unsigned char));
    memcpy(this->_img_ptr, other.getImagePtr(),  other.getImageSize()*sizeof(unsigned char));
}

Image& Image::operator=(Image& other){
    this->_channels = other.getChannels();
    this->_width = other.getWidth();
    this->_height = other.getHeight();
    this->_orig_channels = other.getOrigChannels();
    this->_img_ptr = (unsigned char *) malloc(other.getImageSize()*sizeof(unsigned char));
    memcpy(this->_img_ptr, other.getImagePtr(),  other.getImageSize()*sizeof(unsigned char));
    return *this; 
}   

bool Image::SavePNGImg(std::string name){
    return !stbi_write_png(name.append(".png").c_str(), _width, _height, _channels, _img_ptr, _width * _channels);
} 

bool Image::SaveJPEGImg(std::string name, int quality){
    return !stbi_write_jpg("sky2.jpeg", _width, _height, _channels, _img_ptr, quality);
}

uint8_t &Image::getRed(int index){
    verifyBounds(index); 
    if (_channels < 3) {
        throw InvalidHue("Image is monochromatic"); 
    }
    return *(_img_ptr + index * _channels); 
}
uint8_t &Image::getBlue(int index){
        verifyBounds(index); 
    if (_channels < 3) {
        throw InvalidHue("Image is monochromatic"); 
    }
    return *(_img_ptr + index * _channels + 2); 
}
uint8_t &Image::getGreen(int index){
    verifyBounds(index); 
    if (_channels < 3) {
        throw InvalidHue("Image is monochromatic"); 
    }
    return *(_img_ptr + index * _channels + 1); 
}
uint8_t &Image::getGray(int index){
        verifyBounds(index); 
    if (_channels > 2) {
        throw InvalidHue("Image is monochromatic"); 
    }
    return *(_img_ptr + index * _channels); 
}


void Image::verifyBounds(int index){
    if (index >= getImageDimensions()) {
        throw std::out_of_range("Index is out of range of image"); 
    }
}

int Image::getWidth(){
    return _width; 
}

int Image::getHeight(){
    return _height; 
}

int Image::getChannels(){
    return _channels; 
}
        
int Image::getOrigChannels(){
    return _orig_channels; 
}
        
size_t Image::getImageSize(){
    return _width * _height * _channels; 
}
    
size_t Image::getImageDimensions(){
    return _width * _height; 
}
        
const unsigned char *Image::getImagePtr(){
    return _img_ptr; 
}