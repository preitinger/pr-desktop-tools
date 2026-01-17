
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Widget.H>

#include <iostream>
#include <iomanip>

#include <vector>
#include <algorithm>
#include <string>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <filesystem>
#include <cassert>
#include <cstdint>

#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

const bool DEV_MODE = false;

using bsoncxx::oid;
template <typename T, typename U>
std::tuple<T &&, U &&> kv(T &&t, U &&u)
{
    return std::tuple<T &&, U &&>(std::forward<T>(t), std::forward<U>(u));
}

using bsoncxx::builder::basic::make_array;
template <typename... Args>
bsoncxx::v_noabi::array::value arr(Args &&...args)
{
    bsoncxx::builder::basic::array array;
    array.append(std::forward<Args>(args)...);
    return array.extract();
}

using bsoncxx::builder::basic::make_document;
// template <typename... Args>
// bsoncxx::v_noabi::document::value doc(Args&&... args) {
//     return make_document<Args...>(args...);
// }
template <typename... Args>
bsoncxx::v_noabi::document::value doc(Args &&...args)
{
    bsoncxx::v_noabi::builder::basic::document document;
    document.append(std::forward<Args>(args)...);
    return document.extract();
}

using bsoncxx::v_noabi::type;
using bsoncxx::v_noabi::document::view;
using S = std::string;
using Col = mongocxx::v_noabi::collection;
using Pipe = mongocxx::pipeline;

pid_t rhythmboxPid = -1;

S humanNameToString(const view &name)
{
    if (name["type"].get_string() == std::string_view("split"))
    {
        return S(name["prename"].get_string().value) + S(" ") + S(name["surname"].get_string());
    }
    return S(name["name"].get_string());
}

class HumanName
{
public:
    HumanName(const view &nameView);
    virtual ~HumanName() {}

    S toString() const { return humanNameToString(nameView); }

private:
    view nameView;
};

HumanName::HumanName(const view &nameView) : nameView(nameView) {}

class Push
{
public:
    Push(const std::string_view &key, std::uint64_t timestamp);
    const S &getKey() const { return key; }
    std::uint64_t getTimestamp() const { return timestamp; }

private:
    S key;
    std::uint64_t timestamp;
};

Push::Push(const std::string_view &key, std::uint64_t timestamp) : key(key), timestamp(timestamp) {}

class PresentedPush
{
public:
    PresentedPush(const std::string_view &name, const Push &push);

    S toString() const { return name; }

    const S &getName() const { return name; }
    const S &getKey() const { return key; }
    std::uint64_t getTimestamp() const { return timestamp; }

private:
    S name;
    S key;
    std::uint64_t timestamp;
};
using Pushes = std::vector<PresentedPush>;

PresentedPush::PresentedPush(const std::string_view &name, const Push &push)
    : name(name), key(push.getKey()), timestamp(push.getTimestamp())
{
}

// Create an instance.
mongocxx::instance inst{};

/**
 * @deprecated
 */
Pushes detectPushes()
{
    Pushes res;

    try
    {
        const auto uri = mongocxx::uri{"mongodb+srv://vercel-admin-user-6399bd75294a8a205e043956:bla@cluster0.fpgmlu2.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0"};

        // Set the version of the Stable API on the client
        mongocxx::options::client client_options;
        const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
        client_options.server_api_opts(api);

        // Setup the connection and get a handle on the "admin" database.
        mongocxx::client conn{uri, client_options};
        mongocxx::database db = conn["pr-push-me"];

        // Ping the database.
        const auto ping_cmd = doc(kv("ping", 1));
        db.run_command(ping_cmd.view());
        std::cout << "Pinged your deployment. You successfully connected to MongoDB!" << std::endl;
        auto colUsers = db["users"];
        auto user = colUsers.find_one(doc(kv("_id", oid("681a2e3d452dc9a79bb3ea5e"))));

        if (!user)
        {
            std::cout << "user empty\n";
            return res;
        }

        auto hasValue = user.has_value();
        std::cout << "hasValue " << hasValue << "\n";
        auto dataExt = (*user)["dataExt"];
        assert(dataExt.type() == type::k_document);
        auto pushers = dataExt.get_document().value["pushers"];
        assert(pushers.type() == type::k_array);
        auto pushersArray = pushers.get_array().value;

        auto pushes = user.value()["dataExt"].get_document().value["pushes"].get_array().value;

        std::vector<std::string_view> pushingKeys;

        for (auto push : pushes)
        {
            pushingKeys.push_back(push.get_string().value);
        }

        for (auto pusher : pushers.get_array().value)
        {
            auto pusherDoc = pusher.get_document().value;
            if (std::find(pushingKeys.begin(), pushingKeys.end(), pusherDoc["key"].get_string()) != pushingKeys.end())
            {
                // found
                // nicht mehr kompatibel
                // res.push_back(PresentedPush(humanNameToString(pusherDoc["name"].get_document().value), pusherDoc["key"].get_string().value));
            }
        }

        // std::cout << "type of dataExt " << dataExt.type() << "\n";

        // std::cout << "email: " << (*user)["email"].get_string() << "\n";

        conn.reset();
    }
    catch (const std::exception &e)
    {
        // Handle errors
        std::cout << "Exception: " << e.what() << std::endl;
    }

    return res;
}

bool removePush(Col &usersCol, const S &userId, const S &key)
{
    S keyCopy(key);
    std::cout << "keyCopy " << keyCopy << "\n";
    Pipe p;
    p.append_stage(
        doc(
            kv("$set", doc(
                           kv("dataExt.pushes", /* "$dataExt.pushes" */ doc(
                                  kv("$filter", doc(
                                                    kv("input", "$dataExt.pushes"),
                                                    kv("cond", doc(
                                                                   kv("$ne", arr("$$this.key", key)))))))))))

    );
    // p.append_stages()
    usersCol.update_one(
        doc(
            kv("_id", oid(userId))),
        p);

    return false;
}

Pushes detectPushes(const S &userId, Col &colUsers)
{
    Pushes res;
    auto user = colUsers.find_one(make_document(kv("_id", oid(userId))));


    if (!user)
    {
        std::cout << "user empty\n";
        return res;
    }
    std::cout << "user as json: " << bsoncxx::to_json(user.value());

    auto hasValue = user.has_value();
    std::cout << "hasValue " << hasValue << "\n";
    auto dataExt = (*user)["dataExt"];
    assert(dataExt.type() == type::k_document);
    auto pushers = dataExt.get_document().value["pushers"];
    assert(pushers.type() == type::k_array);
    auto pushersArray = pushers.get_array().value;

    auto pushesArray = user.value()["dataExt"].get_document().value["pushes"].get_array().value;

    std::vector<Push> pushes;

    for (auto push : pushesArray)
    {
        auto pushDoc = push.get_document().value;
        pushes.push_back(Push(pushDoc["key"].get_string(), pushDoc["timestamp"].get_double()));
    }

    for (auto pusher : pushers.get_array().value)
    {
        auto pusherDoc = pusher.get_document().value;
        auto foundPush = std::find_if(pushes.begin(), pushes.end(), [pusherDoc](auto const &o)
                                      {
            const std::string_view& asView = o.getKey();
            return asView == pusherDoc["key"].get_string(); });

        if (foundPush != pushes.end())
        {
            // found
            res.push_back(PresentedPush(humanNameToString(pusherDoc["name"].get_document().value), *foundPush));
        }
    }

    return res;
}

Pushes waitWithChangeStream(const S &userId, Col &colUsers)
{
    Pushes res = detectPushes(userId, colUsers);
    std::cout << "num pushes before waitstream: " << res.size() << "\n";
    if (!res.empty())
        return res;

    try
    {
        mongocxx::pipeline pipeline;
        std::cout << "got pipeline\n";
        // db.testWatch.aggregate([{$addFields: { testArray: {$objectToArray: '$updateDescription.updatedFields'}, testKeys: {$map: {input: {$objectToArray: '$updateDescription.updatedFields'}, in: '$$this.k'}},
        // containsPushes: {$gt:  [          {                  $size:  {                  $filter:  {                  input:  {                  $map: {                   input:  {                  $objectToArray:  '$updateDescription.updatedFields' },      in: '$$this.k'   } },       cond:  {                  $gte:  [          {                  $indexOfCP:  [          '$$this', 'dataExt.pushes'] }, 0] },       limit:  1 } } }, 0] } } } ])
        pipeline.match(make_document(
            kv("operationType", "update"),
            kv("documentKey._id", oid(userId))

                ));
        pipeline.add_fields(make_document(kv("updatesPushes", make_document(kv("$gt", make_array(make_document(kv("$size", make_document(kv("$filter", make_document(kv("input", make_document(kv("$map", make_document(kv("input", make_document(kv("$objectToArray", "$updateDescription.updatedFields"))), kv("in", "$$this.k"))))), kv("cond", make_document(kv("$gte", make_array(make_document(kv("$indexOfCP", make_array("$$this", "dataExt.pushes"))), 0)))), kv("limit", 1)))))), 0))))));
        pipeline.match(make_document(kv("updatesPushes", true)));

        auto stream = colUsers.watch(pipeline);
        std::cout << "got stream\n";

        while (true)
        {
            // std::cout << "while (true)\n";
            bool any = false;
            for (const auto &event : stream)
            {
                std::cout << "for\n";
                std::cout << bsoncxx::to_json(event) << std::endl;
                any = true;
            }

            if (any)
            {
                return detectPushes(userId, colUsers);
            }
        }
    }
    catch (const std::exception &e)
    {
        // Handle errors
        std::cout << "Exception: " << e.what() << std::endl;
    }

    return res;
}

/**
 * @deprecated
 */
Pushes waitForPush(bool &cancel, std::unique_lock<std::mutex> &l, std::condition_variable &cv)
{

    while (!cancel)
    {
        // std::cout << "cancel in waitForPush " << cancel << "\n";
        Pushes pushes = detectPushes();
        if (!pushes.empty())
            return pushes;
        cv.wait_for(l, std::chrono::seconds(3));
    }

    return Pushes();
}

// void resizeCb(Fl_Widget *w, void *voidWindow)
// {
//     Fl_Window *window = (Fl_Window *)voidWindow;
//     if (!window->active())
//     {
//         std::cout << "in resizeCb: window not active\n";
//         return;
//     }
//     window->position(0, 0);
//     window->color(FL_FOREGROUND_COLOR);
//     window->redraw();
//     std::cout << "after redraw\n";
// }

class ColorTimeout
{
public:
    ColorTimeout(Fl_Window *window);
    bool run();

private:
    Fl_Window *window;
    int state;
};
ColorTimeout::ColorTimeout(Fl_Window *window) : window(window), state(0) {}

bool ColorTimeout::run()
{
    if (!window->active())
    {
        // std::cout << "run returning false because window not active\n";
        return false;
    }
    switch (state)
    {
    case 0:
        window->color(FL_RED);
        state = 1;
        break;
    default:
        window->color(FL_BACKGROUND_COLOR);
        state = 0;
        break;
    }

    window->redraw();
    // std::cout << "run returns true after redraw of window\n";

    return true;
}

void timeoutCb(void *voidColorTimeout)
{
    // std::cout << "timeoutCb\n";
    ColorTimeout *timeout = (ColorTimeout *)voidColorTimeout;
    if (timeout->run())
    {
        Fl::add_timeout(0.5, &timeoutCb, voidColorTimeout);
    }
}

class RemovePushCb
{
public:
    RemovePushCb(const S &userId, Col &usersCol, const S &key) : userId(userId), usersCol(usersCol), key(key), button(nullptr) {}
    void run() const;
    void setButton(Fl_Button *b) { button = b; }

    const S &userId;
    Col &usersCol;
    const S &key;
    Fl_Button *button;
};

void removePushCb(Fl_Widget *w, void *arg)
{
    const RemovePushCb *cb = (const RemovePushCb *)arg;
    cb->run();
}

void RemovePushCb::run() const
{
    removePush(usersCol, userId, key);
    if (button)
    {
        Fl_Group *parent = button->parent();
        delete button;
        if (parent)
            parent->redraw();
    }
}

void cbHideTimeout(void *data)
{
    Fl_Widget *w = (Fl_Widget *)data;
    w->hide();
}

void cbClose(Fl_Widget *w, void *data)
{
    std::cout << "cbClose\n";
    Fl_Window *window = (Fl_Window *)data;
    // window->hide();
    Fl::add_timeout(0.1, cbHideTimeout, window);
}

void cbAudioOff(Fl_Widget *w)
{
    if (rhythmboxPid > 0)
    {
        kill(rhythmboxPid, SIGKILL);
    }
}

void presentPushes(const S &userId, Col &usersCol, const Pushes &pushes)
{
    std::cout << "after waitForPush - pushes by:\n";
    for (const auto &push : pushes)
    {
        std::cout << push.toString() << "\n";
    }

    // std::cout << "Continue with enter ...\n";
    // S line;
    // std::getline(std::cin, line);

    // std::cout << "after getline\n";

    Fl_Window *window = new Fl_Window(400, 20 + 70 * (pushes.size() + 2), "Du wurdest angestupst!");
    // window->set_modal();
    ColorTimeout colorTimeout(window);
    // window->resizable(window);
    std::vector<S> buttonLabels;
    std::vector<RemovePushCb> cbs;
    for (const auto &push : pushes)
    {
        buttonLabels.push_back(push.toString());
        cbs.push_back(RemovePushCb(userId, usersCol, push.getKey()));
    }
    std::vector<Fl_Button *> buttons;
    int y = 0;
    for (y = 0; y < pushes.size(); ++y)
    {
        const auto &push = pushes[y];
        const auto &label = buttonLabels[y];
        Fl_Button *b = new Fl_Button(20, 20 + y * 70, 360, 50, label.c_str());
        buttons.push_back(b);
        cbs[y].setButton(b);
        void *pKey = (void *)&(cbs[y]);

        b->callback(&removePushCb, pKey);
    }

    Fl_Button *bAudioOff = new Fl_Button(20, 20 + y++ * 70, 100, 50, "Audio off");
    bAudioOff->callback(cbAudioOff);
    Fl_Button *bClose = new Fl_Button(20, 20 + y * 70, 100, 50, "Close");
    bClose->callback(cbClose, window);
    window->end();

    Fl::lock();
    window->show();
    Fl::add_timeout(0.5, timeoutCb, &colorTimeout);

    {
        // call rhythmbox and play sound in forked process
        pid_t pid = fork();

        if (pid == 0)
        {
            // child process
            std::cout << "in child process: current path " << std::filesystem::current_path() << "\n";
            int res = execl("/usr/bin/rhythmbox", "rhythmbox", "/home/peter/my_projects/individual-gits/pr-desktop-tools/onNudge.mp3", (char *)NULL);
            perror("Could not play rhythmbox");
            exit(1);
        }
        std::cout << "child process with rhythmbox: " << pid << "\n";
        rhythmboxPid = pid;
    }

    int res = Fl::run();

    std::cout << "res of Fl::run() " << res << "\n";
    Fl::remove_timeout(timeoutCb, &colorTimeout);
    // std::cout << "vor hide\n";
    // window->hide();
    // std::cout << "nach hide\n";
    buttons.clear();
    // delete window;
    std::cout << "after delete window - ende presentPushes\n";

    res = Fl::run();
    std::cout << "res of 2nd Fl::run " << res << "\n";
    // TODO show window
}

mongocxx::client waitForMongoClient(const mongocxx::v_noabi::uri &uri, const mongocxx::options::client &client_options)
{
    do
    {
        try
        {
            return mongocxx::client(uri, client_options);
        }
        catch (const std::exception &e)
        {
            // Handle errors
            std::cout << "Exception: " << e.what() << std::endl;
            std::cout << "trying connection in 10s ...\n";
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    } while (true);
}

int main()
{

    if (DEV_MODE)
    {
        std::cout << "*************** WARNING: DEV_MODE ****************\n";
    }

    // Fl_Window *mainWnd = new Fl_Window(100, 100, "pr-push-me");
    // mainWnd->end();
    // mainWnd->callback(cbClose, mainWnd);
    // mainWnd->show();

    S userId(DEV_MODE ? "681d930d3268a28361e6ccc7" : "681a2e3d452dc9a79bb3ea5e");
    try
    {

        const auto uri = mongocxx::uri{"mongodb+srv://vercel-admin-user-6399bd75294a8a205e043956:bla@cluster0.fpgmlu2.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0"};

        // Set the version of the Stable API on the client
        mongocxx::options::client client_options;
        const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
        client_options.server_api_opts(api);

        // Setup the connection and get a handle on the "admin" database.
        std::cout << "before conn\n";
        auto conn = waitForMongoClient(uri, client_options);
        std::cout << "got conn\n";
        auto db = conn[DEV_MODE ? "pr-push-me_dev" : "pr-push-me"];
        std::cout << "got db\n";

        auto colUsers = db["users"];
        Pushes pushes;

        do
        {
            std::cout << "vor waitWithChangeStream\n";
            pushes = waitWithChangeStream(userId, colUsers);
            if (!pushes.empty())
            {
                presentPushes(userId, colUsers, pushes);
            }
        } while (true);
    }
    catch (const std::exception &e)
    {
        // Handle errors
        std::cout << "Exception: " << e.what() << std::endl;
    }

    return 0;

    // bool cancel = false;
    // std::mutex m;
    // std::condition_variable cv;
    // Pushes pushes;

    // std::thread t(
    //     [&cancel, &m, &cv, &pushes]()
    //     {
    //         std::unique_lock l(m);

    //         while (!cancel)
    //         {
    //             std::cout << "cancel " << cancel << "\n";
    //             pushes = waitForPush(cancel, l, cv);

    //             l.unlock();
    //             if (!pushes.empty())
    //             {
    //                 presentPushes(userId, colUsers, pushes);
    //             }
    //             l.lock();
    //         }
    //     });

    // std::this_thread::sleep_for(std::chrono::seconds(10));
    // std::cout << "nach sleep\n";
    // {
    //     std::unique_lock l(m);
    //     cancel = true;
    //     std::cout << "cancel set to true\n";
    //     cv.notify_all();
    // }

    // std::cout << "Joining thread ...\n";
    // t.join();
    // std::cout << "done.\n";

    // return 0;
}