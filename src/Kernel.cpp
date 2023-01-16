#include "Kernel.hpp"
#include <math.h>
#include <exception>
#include <iostream>

Kernel::Kernel(double m[KERNEL_SIZE][KERNEL_SIZE]){
    for (int i = 0; i < KERNEL_SIZE; i++){
        for (int j = 0; j < KERNEL_SIZE; j++) {
            values[std::abs(i-KERNEL_SIZE+1)][std::abs(j-KERNEL_SIZE+1)] = m[i][j];
            std::cout << values[std::abs(i-KERNEL_SIZE+1)][std::abs(j-KERNEL_SIZE+1)] << " "; 
        }
        std::cout << std::endl; 
    }
}

double Kernel::getValue(int row, int col){
    if (row < 0 || row >= KERNEL_SIZE) std::__throw_out_of_range("Invalid row for Kernel: " + row);
    if (col < 0 || col >= KERNEL_SIZE) std::__throw_out_of_range("Invalid row for Kernel: " + col);
    return values[row][col]; 
}

bool Kernel::isGaussian(){
    double sum = 0; 
    for (int i = 0;i < 3; i++){
        for (int j = 0; j < 3; j++) {
            if (values[i][j] < 0) {
                return false; 
            }
            sum += values[i][j];
        }
    }
    return sum == 1.0; 
}

bool Kernel::isLaplacian(){
    double sum = 0; 
    for (int i = 0;i < 3; i++){
        for (int j = 0; j < 3; j++) {
            if ((i==0 || i==2) && (j==0 || j==2) && values[i][j]) return false; 
            if ((i==1 || j==1) && (i != j) && (values[i][j] != -1 || values[i][j] != 1)) return false; 
            if (j==1 && i==1 && (values[i][j] != -4 || values[i][j] != 4))  return false; 
            sum += values[i][j];
        }
    }
    return !sum; 
}

bool Kernel::isHighPassGeneric(){
    double sum = 0; 
    for (int i = 0;i < 3; i++){
        for (int j = 0; j < 3; j++) {
            if (j==1 && i==1 && (values[i][j] != -8 || values[i][j] != 8))  return false; 
            else if ((values[i][j] != -1 || values[i][j] != 1)) return false; 
            sum += values[i][j];
        }
    }
    return !sum; 
}


bool Kernel::isPrewittHx(){
    double sum = 0; 
    for (int i = 0;i < 3; i++){
        for (int j = 0; j < 3; j++) {
            if (j!=1 && std::abs(values[i][j]) != 1) return false; 
            sum += values[i][j];
        }
    }
    return !sum; 
}

bool Kernel::isPrewittHy(){
    double sum = 0; 
    for (int i = 0;i < 3; i++){
        for (int j = 0; j < 3; j++) {
            if (i!=1 && std::abs(values[i][j]) != 1) return false; 
            sum += values[i][j];
        }
    }
    return !sum; 
}

bool Kernel::isSobelHx(){
    double sum = 0; 
    for (int i = 0;i < 3; i++){
        for (int j = 0; j < 3; j++) {
            if (j!=1 && std::abs(values[i][j]) != 1 && i != 1) return false; 
            if (j!=1 && std::abs(values[i][j]) != 2 && i == 1) return false; 
            sum += values[i][j];
        }
    }
    return !sum; 
}

bool Kernel::isSobelHy(){
    double sum = 0; 
    for (int i = 0;i < 3; i++){
        for (int j = 0; j < 3; j++) {
            if (i!=1 && std::abs(values[i][j]) != 1 && j != 1) return false; 
            if (i!=1 && std::abs(values[i][j]) != 2 && j == 1) return false; 
            sum += values[i][j];
        }
    }
    return !sum; 
}