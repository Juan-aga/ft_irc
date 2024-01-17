#include "includes/server.hpp"

int main(int argc, char const *argv[])
{
    (void)argc;
    Server serverirc(atoi(argv[1]), argv[2]);
    serverirc.createSocket();
    while (1)
        ;
    return 0;
}
