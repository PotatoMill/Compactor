#pragma once

class Fulness {
private:
    int distance{0}; //Distance in mm
    int cutoffDistance{500}; //distances over this will be ignored
    int fulnessValue{0}; //How many percent full the compressor is
    

public:
    Fulness(); //Default contructor

    ~Fulness(); // Destructor

    int readFulness(); //Funs the distance sensor
    int getDistance(); //Returns distance in mm
    int getFulness(); //Returns, compress where it's compressable, full when its not possible
    //to compress more, empty when no compression is needed
    int getFulnessValue(); //How many percent full the compressor is
    


};