#include "MainManager.h"

int main() {
    auto manager = std::make_shared<MainManager>();
    manager->run();
    //thread 1
    // thread 2
    return 0;
}
