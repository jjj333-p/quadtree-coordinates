#include <fstream>
#include <iostream>
#include <valarray>
#include <vector>
#include "state.h"
#include "quadtree.h"
// #include "genBST.h"

float geo_to_mile(
    float lat1,
    float longe1,
    float lat2,
    float longe2
);

void option2();

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

        if (tree == nullptr) {
            tree = new quadtree(
                new state(&state_name, latitude, longitude)
            );
        } else {
            tree->insert(
                new state(&state_name, latitude, longitude)
            );
        }
    }

    statesTXT.close();

    std::string menu_msg = "Please select an option to continue:\n"
            "1. Quit\n"
            "2. Compute distance between two coordinate points\n"
            "> ";

    std::cout << menu_msg;

    for (std::string input; std::cin >> input;) {
        if (input.size() != 1) {
            std::cout << "Invalid input `" << input << "`.\n" << menu_msg;
            continue;
        }

        switch (input[0]) {
            case '1':
                std::cout << "Exiting..." << std::endl;
                return 0;
            case '2':
                option2();
                break;

            default:
                std::cout << "Invalid input `" << input << "`.\n";
        }

        std::cout << menu_msg;
    }
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

void option2() {
    //read in values to compute
    std::cout << "Latitude 1: ";
    std::string lat1_str;
    std::cin >> lat1_str;
    std::cout << "Longitude 1: ";
    std::string longe1_str;
    std::cin >> longe1_str;
    std::cout << "Latitude 2: ";
    std::string lat2_str;
    std::cin >> lat2_str;
    std::cout << "Longitude 2: ";
    std::string longe2_str;
    std::cin >> longe2_str;

    //convert to numbers now, to include in output for debug
    const float lat1 = std::stof(lat1_str);
    const float longe1 = std::stof(longe1_str);
    const float lat2 = std::stof(lat2_str);
    const float longe2 = std::stof(longe2_str);

    std::cout << "The distance between ("
            << lat1
            << ","
            << longe1
            << ")  and ("
            << lat2
            << ","
            << longe2
            << ") is "
            << geo_to_mile(lat1, longe1, lat2, longe2)
            << " miles.\n";
}


