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

void option3();

void option4();

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
                option3();
                break;
            case '4':
                option4();
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
    //convert from degrees to radians
    const float lat1_rad = lat1 * M_PIf / 180.0f;
    const float longe1_rad = longe1 * M_PIf / 180.0f;
    const float lat2_rad = lat2 * M_PIf / 180.0f;
    const float longe2_rad = longe2 * M_PIf / 180.0f;

    //radians to miles
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

void option3(quadtree *tree) {
    std::cout << "Latitude: ";
    std::string lat_str;
    std::cin >> lat_str;
    std::cout << "Longitude: ";
    std::string longe_str;
    std::cin >> longe_str;

    //print number conversion to make sure string is parsed right + responsiveness
    const float lat = std::stof(lat_str);
    const float longe = std::stof(longe_str);
    std::cout << "Finding the closest city to (" << lat << longe << ")...\n";

    //as soon as we are looking at nodes further away, we can stop checking
    float closest_distance = geo_to_mile(
        lat,
        longe,
        tree->this_el->latitude,
        tree->this_el->longitude
    );
    quadtree *closest_node = tree;

    //breadth first search
    std::stack<quadtree *> city_stack;
    city_stack.push(tree);
    while (!city_stack.empty()) {
        quadtree *city = city_stack.top();
        city_stack.pop();

        //check each child node of the current element, since we started already knowing the root
        if (city->nw != nullptr) {
            //calculate the distance from our desired coords
            const float distance = geo_to_mile(
                lat,
                longe,
                city->nw->this_el->latitude,
                city->nw->this_el->longitude
            );

            //if its closer thats the right direction to move
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_node = city->nw;
                city_stack.push(closest_node);
            } else {
                //otherwise check if theres a closer way to move
                if (city->nw->nw != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->nw->nw->this_el->latitude,
                        city->nw->nw->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->nw->nw;
                        city_stack.push(closest_node);
                    }
                }
                if (city->nw->ne != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->nw->ne->this_el->latitude,
                        city->nw->ne->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->nw->ne;
                        city_stack.push(closest_node);
                    }
                }
                if (city->nw->sw != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->nw->sw->this_el->latitude,
                        city->nw->sw->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->nw->sw;
                        city_stack.push(closest_node);
                    }
                }
                if (city->nw->se != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->nw->se->this_el->latitude,
                        city->nw->se->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->nw->se;
                        city_stack.push(closest_node);
                    }
                }
            }
        }
        if (city->ne != nullptr) {
            //calculate the distance from our desired coords
            const float distance = geo_to_mile(
                lat,
                longe,
                city->ne->this_el->latitude,
                city->ne->this_el->longitude
            );

            //if its closer thats the right direction to move
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_node = city->ne;
                city_stack.push(closest_node);
            } else {
                //otherwise check if theres a closer way to move
                if (city->ne->nw != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->ne->nw->this_el->latitude,
                        city->ne->nw->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->ne->nw;
                        city_stack.push(closest_node);
                    }
                }
                if (city->ne->ne != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->ne->ne->this_el->latitude,
                        city->ne->ne->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->ne->ne;
                        city_stack.push(closest_node);
                    }
                }
                if (city->ne->sw != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->ne->sw->this_el->latitude,
                        city->ne->sw->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->ne->sw;
                        city_stack.push(closest_node);
                    }
                }
                if (city->ne->se != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->ne->se->this_el->latitude,
                        city->ne->se->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->ne->se;
                        city_stack.push(closest_node);
                    }
                }
            }
        }
        if (city->sw != nullptr) {
            //calculate the distance from our desired coords
            const float distance = geo_to_mile(
                lat,
                longe,
                city->sw->this_el->latitude,
                city->sw->this_el->longitude
            );

            //if its closer thats the right direction to move
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_node = city->sw;
                city_stack.push(closest_node);
            } else {
                //otherwise check if theres a closer way to move
                if (city->sw->nw != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->sw->nw->this_el->latitude,
                        city->sw->nw->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->sw->nw;
                        city_stack.push(closest_node);
                    }
                }
                if (city->sw->ne != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->sw->ne->this_el->latitude,
                        city->sw->ne->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->sw->ne;
                        city_stack.push(closest_node);
                    }
                }
                if (city->sw->sw != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->sw->sw->this_el->latitude,
                        city->sw->sw->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->sw->sw;
                        city_stack.push(closest_node);
                    }
                }
                if (city->sw->se != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->sw->se->this_el->latitude,
                        city->sw->se->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->sw->se;
                        city_stack.push(closest_node);
                    }
                }
            }
        }
        if (city->se != nullptr) {
            //calculate the distance from our desired coords
            const float distance = geo_to_mile(
                lat,
                longe,
                city->se->this_el->latitude,
                city->se->this_el->longitude
            );

            //if its closer thats the right direction to move
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_node = city->se;
                city_stack.push(closest_node);
            } else {
                //otherwise check if theres a closer way to move
                if (city->se->nw != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->se->nw->this_el->latitude,
                        city->se->nw->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->se->nw;
                        city_stack.push(closest_node);
                    }
                }
                if (city->se->ne != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->se->ne->this_el->latitude,
                        city->se->ne->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->se->ne;
                        city_stack.push(closest_node);
                    }
                }
                if (city->se->sw != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->se->sw->this_el->latitude,
                        city->se->sw->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->se->sw;
                        city_stack.push(closest_node);
                    }
                }
                if (city->se->se != nullptr) {
                    //calculate the distance from our desired coords
                    const float child_distance = geo_to_mile(
                        lat,
                        longe,
                        city->se->se->this_el->latitude,
                        city->se->se->this_el->longitude
                    );

                    //if its closer thats the right direction to move
                    if (child_distance < closest_distance) {
                        closest_distance = child_distance;
                        closest_node = city->se->se;
                        city_stack.push(closest_node);
                    }
                }
            }
        }
    }
}

void option4() {
    std::cout << "Not implemented yet.\n";
}


