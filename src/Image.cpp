#include "Image.hpp"
#include <stdexcept>
#include "InvalidHue.hpp" 
#include <string.h>
#include <math.h> 

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"

Image::Image(std::string name){
    _img_ptr = stbi_load(name.c_str(), &_width, &_height, &_orig_channels, 0);
    this->_channels = _orig_channels; 
    if(_img_ptr == NULL) {
        printf("Error in loading the image %s \n", name.c_str());
        throw std::runtime_error("Error in loading the image " +  name ); 
    }
}

Image::Image(std::string name, int channels){
    _img_ptr = stbi_load(name.c_str(), &_width, &_height, &_orig_channels, channels);
    this->_channels = channels; 
    if(_img_ptr == NULL) {
        printf("Error in loading the image\n");
        throw std::runtime_error("Error in loading the image"); 
    }
}

Image::Image(int width, int height, int channels){
    this->_width = width;
    this->_height = height;
    this->_channels = channels; 
    this->_orig_channels = channels; 
    this->_img_ptr = (unsigned char *) malloc(sizeof(unsigned char)*getImageSize());
}

Image Image::GetMirrorImageVertical(){
    Image ret(_width, _height, _channels); 
    unsigned char * img_ret = ret.getImagePtr();
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            for (int k = 0; k < _channels; k++) {
                img_ret[i*_width*_channels + j*_channels + k] = _img_ptr[i*_width*_channels + (_width - j - 1)*_channels + k];
            }
        }
    }
    return ret; 
}

Image Image::GetMirrorImageHorizontal(){
    Image ret(_width, _height, _channels); 
    unsigned char * img_ret = ret.getImagePtr();
    for (int i = 0; i < _height; i++) {
        memcpy(img_ret + i*_width*_channels, _img_ptr + (_height-i - 1)*_width*_channels, sizeof(unsigned char) * _width * _channels);
    }
    return ret; 
}

Image Image::GetGrayImage(int desired_channels){
    Image ret(_width, _height, desired_channels); 
    unsigned char * img_ret = ret.getImagePtr();
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            for (int k = 0; k < desired_channels; k++) 
                img_ret[(i*_width + j)*desired_channels + k] = 0.114 * getBlue(i*_width + j) + 0.587 * getGreen(i*_width + j)  + 0.299 * getRed(i*_width + j); 
        }
    }
    return ret; 
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

Image& Image::operator=(Image other){
    this->_channels = other.getChannels();
    this->_width = other.getWidth();
    this->_height = other.getHeight();
    this->_orig_channels = other.getOrigChannels();
    this->_img_ptr = (unsigned char *) malloc(other.getImageSize()*sizeof(unsigned char));
    memcpy(this->_img_ptr, other.getImagePtr(),  other.getImageSize()*sizeof(unsigned char));
    return *this; 
}   

// TODO USE SMART POINTER
int *Image::GetHistogram(){
    int *histogram = (int *) calloc(HISTOGRAM_SIZE,sizeof(int)); 
    for (int i = 0 ; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            histogram[getGray(i*_width +j)]++; 
        }
    }
    return histogram; 
}

Image Image::quantizeImage(int n){
    if (!n) throw std::invalid_argument("quantizeImage: N should not be zero"); 
    if (!IsMonochromatic()) {
        throw InvalidHue("quantizeImage: Image is not Monochromatic to quantize"); 
    }
    Image ret = *this; 
    int *histogram = GetHistogram(); 
    int t1 = -1, t2 = -1; 
    for (int i = 0 ;  i < HISTOGRAM_SIZE; i++){
        if (t1 == -1 && histogram[i] != 0) t1 = i; 
        if (t2 == -1 && histogram[HISTOGRAM_SIZE - i  - 1] != 0) t2 = HISTOGRAM_SIZE - i  - 1; 
        if (t1 != -1 && t2 != -1)  break;
    }
    int tam_int = t2 - t1 + 1;
    if (n >= tam_int) return *this; 
    double tb =  (double) tam_int/n;
    double interval = t1 - 0.5;
    for (int i = 0 ; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            u_int8_t& gray = ret.getGray(i*_width +j);  
            while (gray > interval) interval += tb; 
            gray =  round(interval-tb/2); 
            interval = t1 - 0.5; 
        }
    }

    return ret; 

}

bool Image::IsMonochromatic(){
    return _channels < 3; 
}

bool Image::SavePNGImg(std::string name){
    return !stbi_write_png(name.append(".png").c_str(), _width, _height, _channels, _img_ptr, _width * _channels);
} 

bool Image::SaveJPEGImg(std::string name, int quality){
    return !stbi_write_jpg(name.append(".jpeg").c_str(), _width, _height, _channels, _img_ptr, quality);
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
        
unsigned char *Image::getImagePtr(){
    return _img_ptr; 
}
