#include "SButtonAccess.H"
#include "Button.H"

#include <iostream>
#include <thread>
#include <atomic>
#include <cassert>

void threadFunc(std::shared_ptr<Button> b, std::atomic<bool>* afterReset)
{
    // b.reset();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    *afterReset = true;
    b.reset();
}

int main() {
    Button* b = new Button(0, 0, 100, 20, "Test");
    auto access = b->access();
    std::atomic<bool> afterReset(false);
    std::thread t(threadFunc, access->tryLock(), &afterReset);

    // must be deadlock
    std::cout << "Must freeze now\n";
    delete b;
    assert(afterReset);
    std::cout << "After delete b\n";
    t.join();

    
    return 0;
}