//
// Created by joseph on 11/20/24.
//

#ifndef STATE_H
#define STATE_H
#include <ostream>
#include <string>
#include <utility>

class state {
public:
    //unique identifier
    float latitude;
    float longitude;

    std::string name;

    //default constructor, just to make the binary tree class happy
    state() : latitude(0), longitude(0), name("empty") {
    }

    //init with that line, and that word as the identifier
    state(const std::string *_name, const float _latitude, const float _longitude) : latitude(_latitude),
        longitude(_longitude),
        name(*_name) {
    }

    //check against the unique identifier
    bool operator==(const state &other) const {
        return (latitude == other.latitude) && (longitude == other.longitude);
    }

    //pretty printing
    friend std::ostream &operator<<(std::ostream &os, const state &obj) {
        os << obj.name << "is found on latitude " << obj.latitude << " and longitude " << obj.longitude << std::endl;

        //??
        return os;
    }
};
#endif //STATE_H
