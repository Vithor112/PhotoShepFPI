#include <stdio.h>
#include <stdlib.h>
#include "Image.hpp"


int main(void) {
    Image x("sky.jpeg"); 
    for (int i = 0; i < 50; i++) {
        Image y = x.GetGrayImage(1).quantizeImage(i); 
        y.SaveJPEGImg(std::to_string(i), 100); 
    }

}