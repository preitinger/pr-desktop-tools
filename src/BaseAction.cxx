#include "BaseAction.H"

#include <cassert>

BaseAction::BaseAction(const std::weak_ptr<BaseAction> &parent)
    : firstMember("First member of BaseAction"),
      parent(parent),
      tidyUpTasks(),
      lastMember("Last member of BaseAction")
{
    std::cout << "BaseAction::BaseAction( const&)\n";
}

BaseAction::BaseAction(std::weak_ptr<BaseAction> &&parent)
    : firstMember("First member of BaseAction"),
      parent(std::move(parent)),
      tidyUpTasks(),
      lastMember("Last member of BaseAction")
{
    std::cout << "BaseAction::BaseAction( &&)\n";
}

// virtual
BaseAction::~BaseAction()
{
    std::cout << "~BaseAction\n";
    std::cout << "# tidyUpTasks " << tidyUpTasks.size() << "\n";

    for (auto &task : tidyUpTasks)
    {
        task();
        std::cout << "did tidy up task\n";
    }
    std::cout << "ENDE !BaseAction\n";
}

void BaseAction::exit()
{
    std::cout << "BaseAction::exit\n";
    auto ref = parent.lock();
    std::cout << "BaseAction::exit: ref=" << ref << "\n";

    if (ref)
    {
        ref->afterSubAction();
    }
}

void BaseAction::afterSubAction()
{
    // should be overwritten
}

void BaseAction::removeTidyUp(TidyUpList::iterator &itTidyUpCb)
{
    if (itTidyUpCb == TidyUpList::iterator() || itTidyUpCb == tidyUpTasks.end())
    {
        return;
    }
    TidyUpList::size_type oldSize = tidyUpTasks.size();
    assert(oldSize > 0);
    (*itTidyUpCb)();
    tidyUpTasks.erase(itTidyUpCb);
    itTidyUpCb = tidyUpTasks.end();
    assert(oldSize - 1 == tidyUpTasks.size());
    std::cout << "really removed tidyup entry\n";
}
