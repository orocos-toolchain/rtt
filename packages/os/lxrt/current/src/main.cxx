
/**
 * @file main.cxx
 * This file contains the start-up code of the framework
 * and will call in turn the user's ORO_main() function.
 */

#include <os/startstop.h>
#include <os/main.h>

int main(int argc, const char* argv)
{
    __os_init(argc, argv);

    int res = ORO_main(argc, argv);

    __os_exit();

    return res;
}
