#include "MainManager.h"

int main() {
    auto manager = std::make_shared<MainManager>();
    manager->run();
    return 0;


}

