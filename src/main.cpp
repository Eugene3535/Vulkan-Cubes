#include "Application.hpp"


int main()
{
    try
    {
        Application().run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return EXIT_SUCCESS;
}
