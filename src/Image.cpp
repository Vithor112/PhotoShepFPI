#include "Image.hpp"
#include <stdexcept>
#include "InvalidHue.hpp" 
#include <string.h>
#include <iostream>
#include <math.h> 
#include "Histogram.hpp"

#define PIXEL_UPPER_BOUND 255
#define PIXEL_LOWER_BOUND 0
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
    unsigned char * img_ret = (unsigned char *) malloc(sizeof(unsigned char)*getImageSize());
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            for (int k = 0; k < _channels; k++) {
                img_ret[i*_width*_channels + j*_channels + k] = _img_ptr[i*_width*_channels + (_width - j - 1)*_channels + k];
            }
        }
    }
    free(_img_ptr);
    _img_ptr = img_ret;
    return *this; 
}

Image Image::GetMirrorImageHorizontal(){
    unsigned char * img_ret = (unsigned char *) malloc(sizeof(unsigned char)*getImageSize());
    for (int i = 0; i < _height; i++) {
        memcpy(img_ret + i*_width*_channels, _img_ptr + (_height-i - 1)*_width*_channels, sizeof(unsigned char) * _width * _channels);
    }
    free(_img_ptr);
    _img_ptr = img_ret; 
    return *this; 
}

Image Image::GetGrayImage(int desired_channels){
    if (IsMonochromatic()){
        return *this; 
    }
    unsigned char * img_ret = (unsigned char *) malloc(sizeof(unsigned char)*desired_channels*_width*_height);
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            for (int k = 0; k < desired_channels; k++) 
                img_ret[(i*_width + j)*desired_channels + k] = 0.114 * getBlue(i*_width + j) + 0.587 * getGreen(i*_width + j)  + 0.299 * getRed(i*_width + j); 
        }
    }
    free(_img_ptr);
    _img_ptr = img_ret; 
    _channels = desired_channels; 
    return *this; 
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

Image Image::quantizeImage(int n){
    if (n <= 0) throw std::invalid_argument("quantizeImage: N should be a positive number"); 
    if (!IsMonochromatic()) {
        throw InvalidHue("quantizeImage: Image is not Monochromatic to quantize"); 
    }
    Histogram histogram(*this); 
    int t1 = -1, t2 = -1; 
    for (int i = 0 ;  i < HISTOGRAM_SIZE; i++){
        if (t1 == -1 && histogram.getValue(i) != 0) t1 = i; 
        if (t2 == -1 && histogram.getValue(HISTOGRAM_SIZE - i  - 1) != 0) t2 = HISTOGRAM_SIZE - i  - 1; 
        if (t1 != -1 && t2 != -1)  break;
    }
    int tam_int = t2 - t1 + 1;
    if (n >= tam_int) return *this; 
    double tb =  (double) tam_int/n;
    double interval = t1 - 0.5;
    for (int i = 0 ; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            u_int8_t& gray = getGray(i*_width +j);  
            while (gray > interval) interval += tb; 
            gray =  round(interval-tb/2); 
            interval = t1 - 0.5; 
        }
    }
    return *this; 
}

Image Image::ZoomIn(){
    int newWidth = _width*2 - 1;
    int newHeight = _height*2 -1; 
    uint8_t *ret = (uint8_t *) malloc(sizeof(uint8_t)*newWidth*newHeight*_channels); 
    for (int i = 0; i < newHeight; i++) {
        for (int j = 0; j < newWidth; j++) {
            for (int k = 0; k < _channels; k++) {
                if (i % 2 && j % 2) {
                    ret[i*newWidth*_channels + j*_channels + k] = roundPixel(((int)_img_ptr[(i/2)*_width*_channels + (j/2)*_channels + k] + _img_ptr[(i/2)*_width*_channels + (j/2+1)*_channels + k] + _img_ptr[(i/2+1)*_width*_channels + (j/2)*_channels + k] + _img_ptr[(i/2+1)*_width*_channels + (j/2+1)*_channels + k])/4);
                } else if (i % 2) {
                    ret[i*newWidth*_channels + j*_channels + k] = roundPixel(((int)_img_ptr[(i/2)*_width*_channels + (j/2)*_channels + k] + _img_ptr[(i/2+1)*_width*_channels + (j/2)*_channels + k])/2);
                } else if (j % 2) {
                    ret[i*newWidth*_channels + j*_channels + k] = roundPixel(((int)_img_ptr[(i/2)*_width*_channels + (j/2)*_channels + k] + _img_ptr[(i/2)*_width*_channels + (j/2+1)*_channels + k])/2);
                } else {
                    ret[i*newWidth*_channels + j*_channels + k] = _img_ptr[(i/2)*_width*_channels + (j/2)*_channels + k];
                }
            }
        }
    }

    free(_img_ptr); 

    _width = newWidth;
    _height = newHeight;
    _img_ptr =  ret; 

    return *this; 
}

Image Image::ZoomOut(int sy, int sx){
    int newWidth = std::ceil(_width/(double)sx); 
    int newHeight = std::ceil(_height/(double)sy); 
    uint8_t *ret = (uint8_t *) malloc(sizeof(uint8_t)*newWidth*newHeight*_channels); 
    for (int i = 0; i < newHeight; i++){
        for (int j = 0; j < newWidth; j++){
            int sum[_channels];
            for (int k = 0; k < _channels; k++) sum[k] = 0; 
            for (int i2 = sy*i; i2 < _height && i2 < sy*(i+1); i2++){
                for (int j2 = sx*j; j2 < _width && j2 < sx*(j+1); j2++){
                    for (int k = 0; k < _channels; k++) sum[k] += _img_ptr[i2*_width*_channels + j2*_channels + k]; 
                }
            }
            for (int k = 0; k < _channels; k++) ret[i*newWidth*_channels + j*_channels + k] = roundPixel(sum[k]/(sx*sy));
        }
    }

    free(_img_ptr); 

    _width = newWidth;
    _height = newHeight;
    _img_ptr =  ret; 

    return *this; 
}

Image Image::Convolution(Kernel& kernel){
    if (!kernel.isGaussian() && !IsMonochromatic()) throw InvalidHue("Image must be monochromatic to apply convolution");
    uint8_t *ret = (uint8_t *) malloc(sizeof(uint8_t)*_width*_channels*_height);
    int add = kernel.isGaussian() || kernel.isHighPassGeneric() || kernel.isLaplacian() ? 0 : 127;
    for (int i = 0; i < _height; i++){
        for (int j = 0; j < _width; j++){
            if (i == 0 || j == 0 || j == _width -1 || i  == _height - 1){
                for (int k = 0; k < _channels; k++) ret[i*_width*_channels + j*_channels + k] = _img_ptr[i*_width*_channels + j*_channels + k];
            } else {
                double sum[_channels];
                for (int k = 0; k < _channels; k++) sum[k] = 0;  
                for (int i2 = 0; i2 < KERNEL_SIZE; i2++){
                    for (int j2 = 0; j2 < KERNEL_SIZE; j2++){
                        for (int k = 0; k < _channels; k++) sum[k] += kernel.getValue(i2, j2) *  _img_ptr[(i-1+i2)*_width*_channels + (j-1+j2)*_channels + k];
                    }
                }
                for (int k = 0; k < _channels; k++) 
                    ret[i*_width*_channels + j*_channels + k] = roundPixel(std::round(sum[k]) + add); 
            } 
        }

    }
    free(_img_ptr); 

    _img_ptr =  ret; 

    return *this; 
}

QImage Image::GetQImage(){
    if (IsMonochromatic()){
        return QImage(getImagePtr(), getWidth(), getHeight(), getWidth(), QImage::Format_Grayscale8);
    } else {
        return QImage(getImagePtr(), getWidth(), getHeight(), getWidth()*getChannels(), QImage::Format_RGB888);
    }
}

Image Image::MatchHistogram(Image& imageTarget){
    if (!IsMonochromatic() || !imageTarget.IsMonochromatic()) {
        throw InvalidHue("Image need to be monochromatic to match histogram:"); 
    }
    Histogram histogramSrc(*this);
    Histogram histogramTarget(imageTarget);

    int map[HISTOGRAM_SIZE]; 
    for (int i = 0; i < HISTOGRAM_SIZE; i++) {
        int diff = INT_MAX; 
        int value;
        for (int j = 0; j < HISTOGRAM_SIZE; j++) {
            if (std::abs(histogramSrc.getHistogramCumValue(i) - histogramTarget.getHistogramCumValue(j)) < diff){
                diff = std::abs(histogramSrc.getHistogramCumValue(i) - histogramTarget.getHistogramCumValue(j));
                value = j; 
            }
        }
        map[i] =  value; 
    }
    for (int i = 0; i < _height; i++){
        for (int j = 0; j < _width; j++) {
            for (int k = 0; k < _channels; k++) {
                _img_ptr[i*_width*_channels + j*_channels + k] = map[_img_ptr[i*_width*_channels + j*_channels + k]];
            }
        }
    }
    return *this; 
}

Image Image::EqualizeHistogram(){
    std::unique_ptr<Histogram> histogram;
    if (IsMonochromatic()) {
        histogram = std::make_unique<Histogram>(*this); 
    } else {
        Image copy = *this;
        copy.GetGrayImage(1);
        histogram = std::make_unique<Histogram>(copy); 
    }

    for (int i = 0; i < _height; i++){
        for (int j = 0; j < _width; j++) {
            for (int k = 0; k < _channels; k++) {
                _img_ptr[i*_width*_channels + j*_channels + k] = histogram->getHistogramCumValue(_img_ptr[i*_width*_channels + j*_channels + k]);
            }
        }
    }
    return *this; 
}

uint8_t Image::roundPixel(int pixel){
    if (pixel > PIXEL_UPPER_BOUND) return PIXEL_UPPER_BOUND; 
    if (pixel < PIXEL_LOWER_BOUND) return PIXEL_LOWER_BOUND; 
    return pixel; 
}


Image Image::AdjustBrightness(int bias){
    if (bias >  PIXEL_UPPER_BOUND || bias < -PIXEL_UPPER_BOUND) {
        throw std::invalid_argument("abs(bias) cannot be to be greater than PIXEL_UPPER_BOUND");
    }

    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            for (int k = 0; k < _channels; k++) {
                int result = _img_ptr[i*_width*_channels + j*_channels + k] + bias; 
                _img_ptr[i*_width*_channels + j*_channels + k] = roundPixel(result);
            }
        }
    } 
    return *this; 
}

Image Image::RotateMinus(){
    uint8_t *ret = (uint8_t *) malloc(sizeof(uint8_t)*_width*_height*_channels); 
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            for (int k = 0; k < _channels; k++) {
                ret[j*_height*_channels + (_height-1-i)*_channels + k] = _img_ptr[i*_width*_channels + j*_channels + k];
            }
        }
    }

    int newWidth = _height;
    _height = _width;
    _width = newWidth; 

    free(_img_ptr);

    _img_ptr = ret; 

    return  *this; 
}

Image Image::RotatePlus(){
    uint8_t *ret = (uint8_t *) malloc(sizeof(uint8_t)*_width*_height*_channels); 
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            for (int k = 0; k < _channels; k++) {
                ret[(_width-1-j)*_height*_channels + i*_channels + k] = _img_ptr[i*_width*_channels + j*_channels + k];
            }
        }
    }

    int newWidth = _height;
    _height = _width;
    _width = newWidth; 

    free(_img_ptr);

    _img_ptr = ret; 

    return  *this; 
}

Image Image::AdjustContrast(double gain){
    if (gain >  PIXEL_UPPER_BOUND || gain <= PIXEL_LOWER_BOUND) {
        throw std::invalid_argument("gain cannot be equal or less than " + std::to_string(PIXEL_LOWER_BOUND) + " or greater than " + std::to_string(PIXEL_UPPER_BOUND));
    }
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            for (int k = 0; k < _channels; k++) {
                int result = std::round(_img_ptr[i*_width*_channels + j*_channels + k]*gain); 
                _img_ptr[i*_width*_channels + j*_channels + k] = roundPixel(result); 
            }
        }
    } 
    return *this; 
}

Image Image::Negative(){
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            for (int k = 0; k < _channels; k++) {
                _img_ptr[i*_width*_channels + j*_channels + k] = PIXEL_UPPER_BOUND - _img_ptr[i*_width*_channels + j*_channels + k]; 
            }
        }
    } 
    return *this; 
}

bool Image::IsMonochromatic(){
    return _channels < 3; 
}

bool Image::SavePNGImg(std::string name){
    return !stbi_write_png(name.c_str(), _width, _height, _channels, _img_ptr, _width * _channels);
} 

bool Image::SaveJPEGImg(std::string name, int quality){
    return !stbi_write_jpg(name.c_str(), _width, _height, _channels, _img_ptr, quality);
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
