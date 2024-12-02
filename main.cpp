/*
 * Code by Joseph Winkie for CS321 on 12/02/2024
 * Reads in a list of state capitals and coordinate points
 * and allows computation of nearest cities to a geolocation
 */

#include <fstream>
#include <iostream>
#include <queue>
#include <valarray>
#include <vector>
#include "state.h"
#include "quadtree.h"
// #include "genBST.h"

//calculates the distance between coordinate points using the formula in assignment instructions
float geo_to_mile(
    float lat1,
    float longe1,
    float lat2,
    float longe2
);

//compute distance between two coordinate points
void option2();

//find the closest city to a coordinate point
void option3(quadtree *tree);

//insert new city and search for other cities within x miles
void option4(quadtree *tree);

int main() {
    //get file to read from
    std::string filename;
    std::cout << "Enter filepath to read from: ";
    std::cin >> filename;

    //open file
    std::ifstream statesTXT(filename);
    if (!statesTXT.is_open()) {
        std::cerr << "Could not open file " << filename << std::endl;
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
            "3. Find the nearest city to a coordinate point\n"
            "4. Add a city to the database, and find cities within x miles\n"
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
            case '3':
                option3(tree);
                break;
            case '4':
                option4(tree);
                break;
            default:
                std::cout << "Invalid input `" << input << "`.\n";
        }

        std::cout << menu_msg;
    }
}

//calculates the distance between coordinate points using the formula in assignment instructions
float geo_to_mile(
    const float lat1,
    const float longe1,
    const float lat2,
    const float longe2
) {
    //convert from degrees to radians
    const float lat1_rad = lat1 * M_PIf / 180.0f;
    const float longe1_rad = longe1 * M_PIf / 180.0f;
    const float lat2_rad = lat2 * M_PIf / 180.0f;
    const float longe2_rad = longe2 * M_PIf / 180.0f;

    //radians to miles
    const float miles = (
        3956.0f * std::acos(
            (std::sin(lat1_rad) * std::sin(lat2_rad)) +
            (
                std::cos(lat1_rad) *
                std::cos(lat2_rad) *
                std::cos(longe2_rad - longe1_rad)
            )
        )
    );

    assert(miles >= 0); //sanity check
    return miles;
}

//compute distance between two coordinate points
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

//find the closest city to a coordinate point
void option3(quadtree *tree) {
    assert(tree); //can only operate on a nonnull value

    std::cout << "Latitude: ";
    std::string lat_str;
    std::cin >> lat_str;
    std::cout << "Longitude: ";
    std::string longe_str;
    std::cin >> longe_str;

    //print number conversion to make sure string is parsed right + responsiveness
    const float lat = std::stof(lat_str);
    const float longe = std::stof(longe_str);
    std::cout << "The closest city to (" << lat << ", " << longe << ") is... ";

    //breadth first search
    std::queue<quadtree *> city_queue;
    city_queue.push(tree);

    // float best_lat_difference = std::abs(tree->this_el->latitude - lat);
    // float best_longe_difference = std::abs(tree->this_el->longitude - longe);
    float best_distance = geo_to_mile(lat, longe, tree->this_el->latitude, tree->this_el->longitude);
    state *best_city = tree->this_el;

    while (!city_queue.empty()) {
        quadtree *city_subtree = city_queue.front();
        if (!city_subtree) break; //the while loop should take care of this, but I don't trust
        city_queue.pop();

        //get current distance
        const float curr_distance = geo_to_mile(lat, longe, city_subtree->this_el->latitude,
                                                city_subtree->this_el->longitude);

        float parent_distance = 0.0f;

        if (city_subtree->predecessor) {
            parent_distance = geo_to_mile(
                lat, longe,
                city_subtree->predecessor->this_el->latitude,
                city_subtree->predecessor->this_el->longitude
            );
        }

        //update bests
        if (curr_distance < best_distance) {
            best_city = city_subtree->this_el;
            best_distance = curr_distance;
        }

        //if at a better distance or on the first item
        if (curr_distance <= parent_distance || city_subtree == tree) {
            //check all elements if we're moving in the right direction
            if (city_subtree->nw)
                city_queue.push(city_subtree->nw);
            if (city_subtree->ne)
                city_queue.push(city_subtree->ne);
            if (city_subtree->se)
                city_queue.push(city_subtree->se);
            if (city_subtree->sw)
                city_queue.push(city_subtree->sw);

            //return to the stack
            continue;
        }

        //if nw exists and is going back in the right direction
        if (city_subtree->nw && (
                curr_distance >= geo_to_mile(lat, longe, city_subtree->nw->this_el->latitude,
                                             city_subtree->nw->this_el->longitude)
            )
        ) {
        }

        //if ne exists and is going back in the right direction
        if (city_subtree->ne && (
                curr_distance >= geo_to_mile(lat, longe, city_subtree->ne->this_el->latitude,
                                             city_subtree->ne->this_el->longitude)
            )
        ) {
            city_queue.push(city_subtree->ne);
        }

        //if sw exists and is going back in the right direction
        if (city_subtree->sw && (
                curr_distance >= geo_to_mile(lat, longe, city_subtree->sw->this_el->latitude,
                                             city_subtree->sw->this_el->longitude)
            )
        ) {
            city_queue.push(city_subtree->sw);
        }

        //if se exists and is going back in the right direction
        if (city_subtree->se && (
                curr_distance >= geo_to_mile(lat, longe, city_subtree->se->this_el->latitude,
                                             city_subtree->se->this_el->longitude)
            )
        ) {
            city_queue.push(city_subtree->se);
        }
    }

    std::cout << best_city->name
            << " at ("
            << best_city->latitude
            << ", "
            << best_city->longitude
            << "), or "
            << best_distance
            << " miles away.\n";
}

//insert new city and search for other cities within x miles
void option4(quadtree *tree) {
    std::cout << "Not implemented yet.\n";

    assert(tree); //can only operate on a nonnull value

    std::cout << "Latitude: ";
    std::string lat_str;
    std::cin >> lat_str;
    std::cout << "Longitude: ";
    std::string longe_str;
    std::cin >> longe_str;
    std::cout << "Name: ";
    std::string name_str;
    std::cin >> name_str;

    float lat = std::stof(lat_str);
    float longe = std::stof(longe_str);
    std::cout << "Inserting " << name_str << " at (" << lat << ", " << longe << ")\n";

    state *new_city = new state(&name_str, lat, longe);
    try {
        tree->insert(new_city);
    } catch (std::runtime_error &e) {
        std::cout << e.what() << "\n";
    }

    std::cout << "Radius to look for nearby cities within, in miles: ";
    std::string search_distance_str;
    std::cin >> search_distance_str;
    float search_distance = std::stof(search_distance_str);
    std::cout << "Searching for cities within " << search_distance << " miles of " << name_str << "...\n";

    //breadth first search
    std::queue<quadtree *> city_queue;
    city_queue.push(tree);

    while (!city_queue.empty()) {
        quadtree *city_subtree = city_queue.front();
        if (!city_subtree) break; //the while loop should take care of this, but I don't trust
        city_queue.pop();

        //get current distance
        const float curr_distance = geo_to_mile(lat, longe, city_subtree->this_el->latitude,
                                                city_subtree->this_el->longitude);

        float parent_distance = 0.0f;

        if (city_subtree->predecessor) {
            parent_distance = geo_to_mile(
                lat, longe,
                city_subtree->predecessor->this_el->latitude,
                city_subtree->predecessor->this_el->longitude
            );
        }

        //print found in range
        if (curr_distance < search_distance) {
            std::cout << '\t' << *city_subtree->this_el << " ("
                    << geo_to_mile(lat, longe, city_subtree->this_el->latitude, city_subtree->this_el->longitude)
                    << "mi away).\n";
        }

        //if at a better distance, within range, or the first item
        if (curr_distance <= parent_distance || curr_distance < search_distance || city_subtree == tree) {
            //check all elements if we're moving in the right direction
            if (city_subtree->nw)
                city_queue.push(city_subtree->nw);
            if (city_subtree->ne)
                city_queue.push(city_subtree->ne);
            if (city_subtree->se)
                city_queue.push(city_subtree->se);
            if (city_subtree->sw)
                city_queue.push(city_subtree->sw);

            //return to the stack
            continue;
        }

        //if nw exists and is going back in the right direction
        if (city_subtree->nw && (
                curr_distance >= geo_to_mile(lat, longe, city_subtree->nw->this_el->latitude,
                                             city_subtree->nw->this_el->longitude)
            )
        ) {
        }

        //if ne exists and is going back in the right direction
        if (city_subtree->ne && (
                curr_distance >= geo_to_mile(lat, longe, city_subtree->ne->this_el->latitude,
                                             city_subtree->ne->this_el->longitude)
            )
        ) {
            city_queue.push(city_subtree->ne);
        }

        //if sw exists and is going back in the right direction
        if (city_subtree->sw && (
                curr_distance >= geo_to_mile(lat, longe, city_subtree->sw->this_el->latitude,
                                             city_subtree->sw->this_el->longitude)
            )
        ) {
            city_queue.push(city_subtree->sw);
        }

        //if se exists and is going back in the right direction
        if (city_subtree->se && (
                curr_distance >= geo_to_mile(lat, longe, city_subtree->se->this_el->latitude,
                                             city_subtree->se->this_el->longitude)
            )
        ) {
            city_queue.push(city_subtree->se);
        }
    }
}


