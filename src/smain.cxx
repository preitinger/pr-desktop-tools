#include "CountDown.H"
#include "TrackMouse.H"
#include "Stocks.H"
#include "TestWidgetAccess.H"
#include "Macros.H"
#include "DbgButton.H"
#include "SmartAction.H"
#include "BaseAction.H"
#include "ExampleAction.H"
#include "StocksAction.H"
#include "GenActionCodeAction.H"
#include "pr-newsletter/IncVersions.H"

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Int_Input.H>
#include <FL/fl_callback_macros.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Group.H>

#include <iostream>
#include <functional>
#include <memory>
#include <thread>

template <class Activity>
void addActivity(Fl_Window *window, const OnFinish &onFinish, int &Y, const char *L)
{
    Fl_Button *button = new DbgButton(0, Y, 400, 20, L);
    Y += 20;
    FL_INLINE_CALLBACK_2(button, Fl_Window *, window, window, OnFinish, onFinish, onFinish, {
        new Activity(window, onFinish); // deleted sich selbst und ruft im Destructor den onFinish-Callback auf, falls er gesetzt ist.
    });
}

SmartActionUP smartAction;

void onFinishSmartAction(Fl_Window *window);

void setMainMenu(void *data)
{
    CB_CAST(Fl_Window, window);

    // Nach jeder Activity wieder das Hauptmenu setzen.
    OnFinish onFinish = std::bind(setMainMenu, window);

    window->clear();
    window->begin();

    int y = 0;
    addActivity<CountDown>(window, onFinish, y, "Count-Down");
    addActivity<TrackMouse>(window, onFinish, y, "Track Mouse Position");
    addActivity<Stocks>(window, onFinish, y, "Stocks");
    addActivity<TestWidgetAccess>(window, onFinish, y, "TestWidgetAccess");

    auto smartActionB = WidgetAccess<Fl_Button>::create(0, y, 400, 20, "SmartAction");
    FL_INLINE_CALLBACK_1(smartActionB->get(), Fl_Window *, window, window, {
        window->clear();
        window->begin();
        smartAction = SmartAction::create(std::bind(onFinishSmartAction, window));
        window->end();
        window->redraw();
    });

    window->end();
    window->redraw();
}

void onFinishSmartAction(Fl_Window *window)
{
    std::cout << "before smartAction.reset()\n";
    smartAction.reset();
    std::cout << "after smartAction.reset()\n";
    setMainMenu(window);
}

SCLASS(MainAction) : public BaseAction
{
public:
    MainAction(const WidgetAccessSP<Fl_Window> &window);
    virtual ~MainAction();
    virtual void afterSubAction() override;
    void start() override final;

private:
    void onExampleAction();
    void onStocksAction();
    void onGenActionCode();
    void onPrNewsletterIncVersionsBuildAllStart();

private:
    WidgetAccessSP<Fl_Window> window;
    ExampleActionSP exampleAction;
    BaseAction::WidgetSP<Fl_Button> exampleButton;
    // WidgetAccessSP<Fl_Button> exampleButton;
    WidgetAccessSP<Fl_Group> groupForExampleAction;
    StocksActionSP stocksAction;
    BaseAction::ButtonSP stocksButton;
    GenActionCodeActionSP genActionCodeAction;
    ButtonSP genActionCodeB;
    prNewsletter::IncVersionsSP prNewsletterIncVersionsBuildAllStartAction;
    ButtonSP prNewsletterIncVersionsBuildAllStartB;
};

MainAction::MainAction(const WidgetAccessSP<Fl_Window> &window)
    : BaseAction(std::weak_ptr<BaseAction>()), window(window)
{
}

void MainAction::start()
{
    Fl_Window *w = window->get();
    if (w)
    {
        w->clear();
        // call of weak_from_this() in this constructor should be a problem...
        std::cout << "before creating ExampleAction\n";
        exampleButton = this->button(0, 0, 400, 20, "ExampleAction", &MainAction::onExampleAction);
        std::cout << "after creating ExampleAction\n";
        stocksButton = button(0, 20, 400, 20, "Stocks", &MainAction::onStocksAction);
        genActionCodeB = button(0, 40, 400, 20, "Generate Action Code", &MainAction::onGenActionCode);
        prNewsletterIncVersionsBuildAllStartB = button(0, 60, 400, 20, "pr-newsletter/IncVersionBuildAllStart", &MainAction::onPrNewsletterIncVersionsBuildAllStart);
    }
}

MainAction::~MainAction()
{
}

void MainAction::onExampleAction()
{
    std::cout << "onExampleAction\n";
    Fl_Window *w = window->get();
    if (w)
    {
        w->begin();
        groupForExampleAction = WidgetAccess<Fl_Group>::create(10, 100, 380, 290, "ExampleAction");
        Fl_Group *group = groupForExampleAction->get();
        group->box(FL_UP_BOX);
        this->exampleAction = std::make_shared<ExampleAction>(weak_from_this(), groupForExampleAction);
        this->exampleAction->start();
        group->end();
        w->end();
        assert(exampleButton);
        Fl_Button *b = exampleButton->get();
        assert(b);
        b->deactivate();
        w->redraw();
    }
}

void MainAction::onStocksAction()
{
    std::cout << "onStocksAction\n";
    if (auto w = window->get())
    {
        w->begin();
        w->clear();
        // WidgetAccessSP<Fl_Group> windowAsGroup(window);
        // Fl_Window* wnd = window->get();
        WidgetAccessSP<Fl_Group> windowAsGroup = window->convert<Fl_Group>();
        stocksAction = std::make_shared<StocksAction>(weak_from_this(), windowAsGroup);
        w->end();
        w->redraw();
        stocksAction->start();
        if (auto exampleB = exampleButton->get())
        {
            exampleB->deactivate();
        }
    }
}

void MainAction::onGenActionCode()
{
    if (auto w = window->get())
    {
        w->begin();
        w->clear();
        genActionCodeAction = std::make_shared<GenActionCodeAction>(weak_from_this(), window->convert<Fl_Group>());
        genActionCodeAction->start();
        w->end();
        w->redraw();
    }
}

void MainAction::onPrNewsletterIncVersionsBuildAllStart()
{
    if (auto w = window->get())
    {
        w->begin();
        w->clear();
        prNewsletterIncVersionsBuildAllStartAction = std::make_shared<prNewsletter::IncVersions>(weak_from_this(), window->convert<Fl_Group>());
        prNewsletterIncVersionsBuildAllStartAction->start();
        w->end();
        w->redraw();
    }
}


// virtual
void MainAction::afterSubAction()
{
    std::cout << "MainAction::afterSubAction\n";
    if (groupForExampleAction)
    {
        Fl_Group *group = groupForExampleAction->get();
        if (group)
            delete group;
        Fl_Window *w = window->get();
        if (w)
        {
            w->redraw();
        }
    }
    if (exampleAction)
    {
        exampleAction.reset();

        assert(exampleButton);
        Fl_Button *b = exampleButton->get();
        assert(b);
        b->activate();
    }
    else
    {
        stocksAction.reset();
        genActionCodeAction.reset();
        if (auto w = window->get())
        {
            w->clear();
            w->begin();
            // call of weak_from_this() in this constructor should be a problem...
            std::cout << "before creating ExampleAction\n";
            exampleButton = this->button(0, 0, 400, 20, "ExampleAction", &MainAction::onExampleAction);
            std::cout << "after creating ExampleAction\n";
            stocksButton = button(0, 20, 400, 20, "Stocks", &MainAction::onStocksAction);
            genActionCodeB = button(0, 40, 400, 20, "Generate Action Code", &MainAction::onGenActionCode);
            w->end();
            w->redraw();
        }
    }
}

void closeCallback(Fl_Widget* w) {
    std::cout << "closeCallback: w " << w << "\n";
    w->hide();
    std::cout << "nach w->hide()\n";
    Fl::add_timeout(10.0, [](void*) {
        std::cout << "Timeout 10s nach hide.\n";
    }, nullptr);
}

int main()
{
    auto window = WidgetAccess<Fl_Window>::create(100, 100, 400, 400, "pr-desktop-tools");
    // Fl_Window *window = new Fl_Window(400, 400, "pr-desktop-tools");
    // setMainMenu(window);
    // window->end();
    window->get()->callback(closeCallback);
    auto mainAction = std::make_shared<MainAction>(window);
    mainAction->start();
    window->get()->show();
    int res = Fl::run();
    std::cout << "nach Fl::run res " << res << "\n";
    Fl_Window *w = window->get();
    if (w)
    {
        Fl_Group* parent = w->parent();
        std::cout << "parent of window " << parent << "\n";
        w->clear();
        delete w;
    }
    window.reset();
    mainAction.reset();
    assert(res == 0);

    std::cout << "sleeping 20 s before return from main\n";
    std::this_thread::sleep_for(std::chrono::seconds(20));
    return res;
}