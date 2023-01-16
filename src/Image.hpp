#pragma once

#include <string> 
#include <QImage>



#include "Kernel.hpp"

class Image {
    public:
        Image(std::string name);
        Image(std::string name, int channels);
        Image(int width, int height, int channels); 
        Image& operator=(Image other);   
        ~Image(); 
        Image(Image &ex); 
        bool SavePNGImg(std::string name); 
        bool SaveJPEGImg(std::string name, int quality); 
        uint8_t &getRed(int index);
        uint8_t &getBlue(int index);
        uint8_t &getGreen(int index);
        uint8_t &getGray(int index);
        int getWidth();
        int getHeight();
        int getChannels(); 
        int getOrigChannels(); 
        size_t getImageSize(); 
        size_t getImageDimensions();
        unsigned char *getImagePtr(); 
        Image AdjustBrightness(int bias);
        Image AdjustContrast(double gain);
        Image Negative();
        Image EqualizeHistogram();
        Image RotateMinus();
        Image RotatePlus(); 
        Image ZoomOut(int sy, int sx);
        Image ZoomIn();
        Image Convolution(Kernel& image);
        Image MatchHistogram(Image& image);
        Image GetMirrorImageVertical();
        Image GetMirrorImageHorizontal();
        Image GetGrayImage(int desired_channels);
        QImage GetQImage();  
        bool IsMonochromatic();
        Image quantizeImage(int colors); 
        
    private:
        int _width, _height, _channels, _orig_channels;
        unsigned char *_img_ptr;
        uint8_t roundPixel(int pixel); 
        void verifyBounds(int index); 
};