#include <stdio.h>
#include <stdlib.h>
#include "Image.hpp"


int main(void) {
    Image x("sky.jpeg"); 
    Image y = x.GetGrayImage();
    y.SaveJPEGImg("gray", 100); 
}