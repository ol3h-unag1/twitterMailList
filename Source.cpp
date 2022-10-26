import <vector>;

import <iostream>;
import <fstream>;

import <string>;
import <string_view>;

import <memory>;

import <format>;

import <algorithm>;

import <chrono>;

import <exception>;

import <filesystem>;

import <source_location>;


int app()
{
    return 0;
}

int main()
{
    try {
        return app();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unhandled exception!" << std::endl;
    }


    // Designing app
    // 
    // LS: show tags based on frequency of usage   

}