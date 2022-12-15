#pragma once

#include <string> 


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

        Image GetMirrorImageVertical();
        Image GetMirrorImageHorizontal();
        Image GetGrayImage(); 
        
    private:
        int _width, _height, _channels, _orig_channels;
        unsigned char *_img_ptr;

        void verifyBounds(int index); 
};