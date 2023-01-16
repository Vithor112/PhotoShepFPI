#pragma once

#define KERNEL_SIZE 3

class Kernel {
    public: 
        Kernel(double m[KERNEL_SIZE][KERNEL_SIZE]);
        double getValue(int row, int col); 
        bool isGaussian();
        bool isLaplacian();
        bool isHighPassGeneric();
        bool isPrewittHx();
        bool isPrewittHy();
        bool isSobelHx();
        bool isSobelHy();

    private:
        double values[KERNEL_SIZE][KERNEL_SIZE]; 
}; 