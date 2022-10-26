import <iostream>;

import <exception>;

import DBHandler;

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