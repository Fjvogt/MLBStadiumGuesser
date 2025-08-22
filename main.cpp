#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include <cctype>

using namespace std;

string curTop5[5] = {"", "", "", "", ""};// the user's top 5

double getDistanceBetweenCoordinates(double lat1, double lat2, double lon1, double lon2){ // calculates the distance between two sets of coordinates (in miles)
    double pi = atan(1)*4;
    double radLat1 = lat1 * (pi/180); 
    double radLat2 = lat2 * (pi/180);
    double radLon1 = lon1 * (pi/180);
    double radLon2 = lon2 * (pi/180);
    double distance = 3963.17*acos(sin(radLat1)*sin(radLat2) + cos(radLat1)*cos(radLat2)*cos(radLon2-radLon1));
    return distance;
}

double milesToFeet(double miles){ // takes a distance in miles, returns that distance in feet
    double feet = miles * 5280;
    return feet;
}
string trim(const string& str) { // trims endline chars from a string
    size_t first = str.find_first_not_of(" \t\r\n");  
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");   
    return str.substr(first, (last - first + 1));
}

bool hasUserWon(string curTop5[]){ // checks if the user has won the game
    for(int i = 0; i<5; i++){
        if(curTop5[i] == ""){
            return false;
        }
    }
    return true;
}

int main() {
    ifstream input;
    string filename = "mlbStadiums.txt";
    input.open(filename);
    string buffer;

    int fileLines = 0;

    while(getline(input, buffer)){ // gets size of file
        fileLines++;
    }

    input.close();
    input.open(filename);

    const int l = fileLines;

    double stadiumsLatitude[l]; // array holding the latitude of the stadiums
    double stadiumsLongitude[l]; // array holding the longitude of the stadiums
    string stadiumNames[l]; // array holding the names of the stadiums
    string teamNames[l]; // array holding the names of the teams

    for(int i = 0; i<fileLines; i++){ // take in the lat, long, and name of each stadium

        getline(input, buffer, ',');
        stadiumsLatitude[i] = stod(buffer);
        //std::cout << "test, lat: " << stadiumsLatitude[i];

        getline(input, buffer, ',');
        stadiumsLongitude[i] = stod(buffer);
        //std::cout << " long: " << stadiumsLatitude[i];

        getline(input, buffer, ',');
        stadiumNames[i] = buffer;
        //std::cout << " stadium: " << stadiumNames[i];

        getline(input, buffer);
        teamNames[i] = trim(buffer);
       

    }

    double cityLat = 38.546448; // latitude of the city to guess from 
    double cityLong = -89.612522; // longitude of the city to guess from 
    string cityName = "Albers, IL"; // name of the city to guess from 

    vector<tuple<double, string, string> >sortedStadiums; // vector of tuples containing the distacne form the city, name of the stadium, and name of the team

        double distance;
    for(int i = 0; i<l; i++){ // populate vector with distances from city to each stadium, stadium's name, and team name
        distance = getDistanceBetweenCoordinates(stadiumsLatitude[i], cityLat, stadiumsLongitude[i], cityLong);
        sortedStadiums.push_back(make_tuple(distance, stadiumNames[i], teamNames[i]));
    }

    sort(sortedStadiums.begin(), sortedStadiums.end()); // sort array from shortest to longest distance

    std::cout << "Welcome to the stadium distance guesser!" << endl;
    std::cout << "Today's city is: " << cityName << endl;
    std::cout << "Try to guess the five closest MLB Stadiums to " << cityName << "!" << endl;
    std::cout << "Guess by team name or by stadium name." << endl;

    int numStrikes = 0; // number of valid guesses the user has made
    string curUserGuess;// the current user guess

    while(1){ 
        cout << "You have " << 3 - numStrikes << " strikes remaining." << endl;

        curUserGuess = ""; // reset the current user guess

        std::cout << "Current top 5: " << endl;
        for(int i = 0; i<5; i++){ // print the user's current top 5
            std::cout << i+1 << ". " << curTop5[i] << endl;
        }

        std::cout << "Enter your guess:" << endl;
        getline(cin, curUserGuess); // get user input
        transform(curUserGuess.begin(), curUserGuess.end(), curUserGuess.begin(), ::tolower); // make user input lowercase

        string curStadium;
        string curTeam;
        bool breakflag = false;

        for(int i = 0; i<l; i++){ // check whole list of stadiums
            breakflag = false; // reset break flag

            curStadium = get<1>(sortedStadiums.at(i)); // make all comparisons use lowercase to eliminate case sensitivity
            curTeam = get<2>(sortedStadiums.at(i));
            transform(curStadium.begin(), curStadium.end(), curStadium.begin(), ::tolower);
            transform(curTeam.begin(), curTeam.end(), curTeam.begin(), ::tolower);

            if(curUserGuess == curStadium){ // check against stadium name

                if(i < 5){ // if the guess is in the top 5
                    std::cout << "Your guess was correct!" << endl;
                    curTop5[i] = get<1>(sortedStadiums.at(i)) + ", " + get<2>(sortedStadiums.at(i)) + " (" + to_string(get<0>(sortedStadiums.at(i))) + " miles)"; // put guess in the user top 5
                }
                else{
                    numStrikes++;
                    std::cout << "Your guess was not in the top 5 closest stadiums. Strike " << numStrikes << "!" << endl;
                }
                breakflag = true;
                break;
            }
            else if(curUserGuess == curTeam){ // check against team name
                if(i < 5){ // if the guess is in the top 5
                    std::cout << "Your guess was correct!" << endl;
                    curTop5[i] = get<1>(sortedStadiums.at(i)) + ", " + get<2>(sortedStadiums.at(i)) + " (" + to_string(get<0>(sortedStadiums.at(i))) + " miles)"; // put guess in the user top 5
                }
                else{
                    numStrikes++;
                    std::cout << "Your guess was not in the top 5 closest stadiums. Strike " << numStrikes << "!" << endl;
                }
                breakflag = true;
                break;
            }
        }
        if(!(breakflag)){// if the loop has ended without breaking out, 
            std::cout << "Your guess did not match any team name or stadium name. Try fixing any spelling errors. Note that guesses are not case sensitive." << endl;
        }

        if(numStrikes >= 3){ // if the user has 3 strikes
            std::cout << "You lost!" << endl;
            std::cout << "The top 5 closest stadiums to " << cityName << " are:" << endl;
            for(int i = 0; i<5; i++){
                std::cout << i+1 << ". " << get<1>(sortedStadiums.at(i)) + ", " + get<2>(sortedStadiums.at(i)) + " (" + to_string(get<0>(sortedStadiums.at(i))) + " miles)"<< endl;
            }
            return 0;
        }
        if(hasUserWon(curTop5)){ // if the user has won
            std::cout << "You Won! Congradulations!" << endl;
            std::cout << "The top 5 closest stadiums to " << cityName << " are:" << endl;
            for(int i = 0; i<5; i++){
                std::cout << i+1 << ". " << get<1>(sortedStadiums.at(i)) + ", " + get<2>(sortedStadiums.at(i)) + " (" + to_string(get<0>(sortedStadiums.at(i))) + " miles)" << endl;
            }
            return 0;
        }
    }

    return 0;
}

