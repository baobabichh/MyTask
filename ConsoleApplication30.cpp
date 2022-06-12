#include "FileSearcherThread.h"



int main()
{

    using namespace sc;

    std::string file_name;
    std::cout << "Enter file name : ";
    std::getline(std::cin ,file_name,'\n' );


    {
        std::cout << "\n\n" << std::endl;
        FileSearcherThread file_searcher;
        std::chrono::time_point<std::chrono::system_clock> begin(std::chrono::system_clock::now());
        file_searcher(file_name);
        std::chrono::time_point<std::chrono::system_clock> end(std::chrono::system_clock::now());
        std::cout << "Time with thread : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
    }

    {
        std::cout << "\n\n"<< std::endl;
        std::chrono::time_point<std::chrono::system_clock> begin(std::chrono::system_clock::now());
        recursive_search(file_name);
        std::chrono::time_point<std::chrono::system_clock> end(std::chrono::system_clock::now());
        std::cout << "Time with no threads : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
    }








    std::cin.get();
    return 0;
}
