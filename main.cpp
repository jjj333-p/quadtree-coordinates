#include <fstream>
#include <iostream>
#include <valarray>
#include <vector>
#include "state.h"
#include "quadtree.h"
// #include "genBST.h"

int main() {
    //open file
    std::ifstream statesTXT("states.txt");
    if (!statesTXT.is_open()) {
        std::cerr << "Could not open file states.txt" << std::endl;
        statesTXT.close();
        return 1;
    }

    quadtree *tree = nullptr;

    //read in each line separately
    std::string line;
    for (int lineNumber = 1; getline(statesTXT, line); lineNumber++) {
        //skip empty or commented lines
        if (line[0] == '#' || line[0] == '\r' || line[0] == '\n') continue;

        //buffer to hold the word string
        std::string word = "";
        std::vector<std::string> words;

        //split on breaks, the latter 2 split characters should never happen but idk
        for (char c: line) {
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                if (!word.empty()) {
                    words.push_back(word);
                    word = "";
                }
            } else {
                word += c;
            }
        }

        //catch the last word of the last line
        if (!word.empty()) {
            words.push_back(word);
        }

        //parse out coords
        float longitude = std::stof(words.back());
        words.pop_back();
        float latitude = std::stof(words.back());
        words.pop_back();

        //put rest of line back into the name
        //probably more efficient way but i cba
        std::string state_name;
        for (const std::string &word_of_city: words) {
            state_name += word_of_city + " ";
        }

        state cs(&state_name, longitude, latitude);

        if (tree == nullptr) {
            tree = new quadtree(&cs);
        } else {
            tree->insert(&cs);
        }
    }

    statesTXT.close();
}

float geo_to_mile(
    const float lat1,
    const float longe1,
    const float lat2,
    const float longe2
) {
    const float lat1_rad = lat1 * M_PIf / 180.0f;
    const float longe1_rad = longe1 * M_PIf / 180.0f;
    const float lat2_rad = lat2 * M_PIf / 180.0f;
    const float longe2_rad = longe2 * M_PIf / 180.0f;

    return (
        3956.0f * std::acos(
            (std::sin(lat1_rad) * std::sin(lat2_rad)) +
            (
                std::cos(lat1_rad) *
                std::cos(lat2_rad) *
                std::cos(longe2_rad - longe1_rad)
            )
        )
    );
}

