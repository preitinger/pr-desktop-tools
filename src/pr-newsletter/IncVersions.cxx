#include "IncVersions.H"
#include "incVersions.H"
#include "../Macros.H"

#include <iostream>

#include <FL/Fl_Input.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Output.H>

#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <vector>

#include <cassert>
#include <cstdlib>
#include <csignal>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace prNewsletter
{

    IncVersions::IncVersions(BaseActionWP &&parent, const WidgetAccessSP<Fl_Group> &group)
        : BaseAction(std::move(parent)),
          group(group),
          cancelB(),
          folderInput(),
          search(),
          runOrStop(),
          errorOutput(),
          pid(0)
    //   nextTimeout()
    {
        std::cout << "IncVersionsBuildAllStart(&&)\n";
    }

    // virtual
    IncVersions::~IncVersions()
    {
    }

    void IncVersions::start() /* override final */
    {
        std::cout << "BEGIN IncVersionsBuildAllStart::start\n";
        assert(group);
        if (auto g = group->get())
        {
            int gx = g->x();
            int gy = g->y();
            int gw = g->w();
            int gh = g->h();
            if (g->as_window())
            {
                gx = 0;
                gy = 0;
            }
            int x = gx + 10;
            int y = gy + 10;
            int w = gw - 20;
            int h = 20;
            int lh = h + 5;
            auto innerGroup = widget<IncVersions, Fl_Group>(x, y += 10, w, gh - 30, "GenActionCode", nullptr);
            innerGroup->get()->box(Fl_Boxtype::FL_DOWN_BOX);
            x += 10;
            w -= 20;
            y += 10;

            cancelB = button(x, y, w, h, "Cancel", &IncVersions::onCancel);
            // typedef void (IncVersions::*MyMethodPtr)();
            folderInput = widget<IncVersions, Fl_Input>(x + 100, y += lh, w - 100, h, "Projektverzeichnis", nullptr);
            search = button(x + 100, y += h, 100, h, "Durchsuchen...", &IncVersions::onSearch);
            runOrStop = button(x + 100, y += lh, 100, h, "Run", &IncVersions::onRunOrStop);
            errorOutput = widget<IncVersions, Fl_Output>(x + 100, y += lh, w - 100, h, "Fehler", nullptr);
            innerGroup->get()->end();
            std::cout << "end IncVersions::start\n";
        }
    }

    void IncVersions::onCancel()
    {
        exit();
    }

    // void IncVersionsBuildAllStart::onInput()
    // {
    //     // TODO your action on callback to nameInput
    // }

    // void IncVersionsBuildAllStart::onTimeout()
    // {

    //     // TODO your timer action
    //     // maybe set next timer
    //     double seconds = 0.5;
    //     nextTimeout = timeout(seconds, &IncVersionsBuildAllStart::onTimeout);
    // }
    void cbFileChooser(Fl_File_Chooser *fc, void *data)
    {
        CB_CAST(IncVersions, x);
        x->onFileChooser(fc);
    }

    void IncVersions::onSearch()
    {
        // fileChooser = std::make_shared<Fl_File_Chooser>(nullptr, nullptr, Fl_File_Chooser::Type::CREATE | Fl_File_Chooser::Type::DIRECTORY, "Choose the project directory");
        // fileChooser->callback(cbFileChooser, this);
        // fileChooser->show();
        // if (auto w1 = folderInput->get())
        // {
        //     const char *chosenDir = fl_dir_chooser("Projektverzeichnis wählen", w1->value());

        //     if (chosenDir)
        //     {
        //         if (auto w = folderInput->get())
        //         {
        //             w->value(chosenDir);
        //         }
        //     }
        // }
        // TODO nyi

        if (auto w1 = folderInput->get())
        {
            Fl_Native_File_Chooser fnfc;
            fnfc.title("Projektverzeichnis wählen");
            fnfc.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
            // fnfc.filter("Text\t*.txt\n"
            //             "C Files\t*.{cxx,h,c}");
            fnfc.directory(w1->value()); // default directory to use
            // Show native chooser
            switch (fnfc.show())
            {
            case -1:
                printf("ERROR: %s\n", fnfc.errmsg());
                break; // ERROR
            case 1:
                printf("CANCEL\n");
                break; // CANCEL
            default:
            {
                printf("PICKED: %s\n", fnfc.filename());
                if (auto w = folderInput->get())
                {
                    w->value(fnfc.filename());
                }
                break; // FILE CHOSEN
            }
            }
        }
    }

    typedef std::string S;

    void IncVersions::onFileChooser(Fl_File_Chooser *fc)
    {
        const char *file = fc->value();
        // std::cout << "onFileChooser: file " << file << ", shown " << fc->shown() << ", when " << fc-><< "\n";

        if (!file)
            return;
        if (auto w = folderInput->get())
        {
            w->value(file);
        }
    }

    static bool strStartsWith(const S &s, const S &prefix)
    {
        auto prefixSize = prefix.size();
        auto size = s.size();
        return size >= prefixSize && (s.compare(0, prefixSize, prefix) == 0);
    }

    template <class F>
    static void lineByLine(std::ifstream &is, F f)
    {
        S line;
        while (std::getline(is, line), is.good())
        {
            if (!f(line))
                break;
        }
    }

    static void readVersion(const S &file, const S &prefix, int &v, bool &found)
    {
        std::cout << "in readVersion: file '" << file << "'\n";
        std::ifstream is(file);
        std::cout << "is.bad(): " << is.bad() << "\n";
        // assert(is.good());

        lineByLine(is, [prefix, &v, &found](S &line)
                   {
            std::cout << "line: " << line << "\n";
            if (strStartsWith(line, prefix))
            {
                auto lineSize = line.size();
                for (auto pos = prefix.size(); pos < lineSize; ++pos)
                {
                    char c = line[pos];
                    if (!(c >= '0' && c <= '9'))
                        break;
                    (v *= 10) += c - '0';
                }
                std::cout << "v: " << v << "\n";

                found = true;
                return false;
            }
            return true; });
        is.close();
        std::cout << "readVersion ende\n";
    }

    static S backupFileName(const S &file, int i)
    {
        return file + '.' + std::to_string(i);
    }

    static void replaceVersion(const S &file, const S &prefix, const S &oldV, const S &newV)
    {
        // Checks fanden vorher statt. Jetzt voraussetzen, dass die Datei mit dem erwarteten Inhalt existiert.

        // Ersten verfuegbaren Dateinamen suchen der Form ${file}.${i} fuer i = 1, 2, 3, ...
        int i = 0;
        bool copied = false;
        S backupFile;

        for (i = 1; i <= 100; ++i)
        {
            if ((copied = std::filesystem::copy_file(file, (backupFile = backupFileName(file, i)), std::filesystem::copy_options::skip_existing)))
                break;
        }

        assert(copied);
        if (!copied)
            return;

        // After the copy, read the copy and write into the original line by line.
        std::ifstream copy(backupFile);
        std::ofstream orig(file);
        S searched = prefix + oldV;

        auto f = [&orig, &searched, &prefix, &newV](S &line)
        {
            if (strStartsWith(line, searched))
            {
                orig << prefix << newV << line.substr(searched.length()) << "\n";
            }
            else
            {
                orig << line << "\n";
            }
            return true;
        };

        lineByLine(copy, f);
    }

    S incVersions(const S &folder, const S &messages, const S &serviceWorkerMessages)
    {
        assert(!folder.empty() && folder[folder.size() - 1] == '/');
        std::ifstream is;
        S line;
        int version = 0, swVersion = 0;
        bool versionFound = false, swVersionFound = false;
        const S prefix = "export const VERSION = ";

        readVersion(folder + messages, prefix, version, versionFound);
        if (!versionFound)
        {
            std::ostringstream os;
            os << "Nicht in \"" << folder << messages << "\" gefunden: " << prefix;
            return os.str();
        }
        // {
        //     S prefix = "export const VERSION = ";
        //     is.open(folder + messages);
        //     assert(is.good());
        //     auto& v = version;
        //     auto& found = versionFound;

        //     while (std::getline(is, line), is.good())
        //     {
        //         if (strStartsWith(line, prefix))
        //         {
        //             auto pos = prefix.size();
        //             auto lineSize = line.size();
        //             for (auto pos = prefix.size(); pos < lineSize; ++pos)
        //             {
        //                 char c = line[pos];
        //                 if (!(c >= '0' && c <= '9'))
        //                     break;
        //                 (v *= 10) += c - '0';
        //             }

        //             versionFound = true;
        //             break;
        //         }
        //     }
        //     is.close();
        // }
        const S prefixSw = "export const SUB_VERSION = ";
        std::cout << "vor 2. readVersion\n";
        readVersion(folder + serviceWorkerMessages, prefixSw, swVersion, swVersionFound);
        if (!swVersionFound)
        {
            std::ostringstream os;
            os << "Nicht in \"" << folder << serviceWorkerMessages << "\" gefunden: " << prefixSw;
            return os.str();
        }

        if (version != swVersion)
        {
            std::ostringstream os;
            os << "Unterschiedliche Versionen: " << version << " und " << swVersion;
            return os.str();
        }

        replaceVersion(folder + messages, prefix, std::to_string(version), std::to_string(version + 1));
        replaceVersion(folder + serviceWorkerMessages, prefixSw, std::to_string(swVersion), std::to_string(swVersion + 1));

        return "";
    }

    void IncVersions::onRunOrStop()
    {
        if (pid > 0)
        {
            // stop
            std::cout << "sending SIGTERM to pid " << pid << "\n";
            int res = kill(pid, SIGTERM);
            if (res)
            {
                setError(S("kill failed: ") + strerror(errno));
            }
            else
            {
                int wstatus;
                int res = waitpid(pid, &wstatus, 0);
                if (res == -1)
                {
                    setError(S("waitpid failed: ") + strerror(errno));
                    return;
                }
                else if (res == 0)
                {
                    setError(S("waitpid returned 0"));
                }
                pid = 0;

                if (auto w = runOrStop->get())
                {
                    w->label("Run");
                }
            }
        }
        else
        {
            // run
            setError("");

            S folder;
            if (auto w = folderInput->get())
            {
                folder = w->value();
            }
            else
            {
                return;
            }
            S swProj("../serviceWorker-for-pr-push-newsletter3");
            S messages = swProj + "/src/messages.ts";
            S serviceWorkerMessages = "app/_lib/client/serviceWorkerMessages.ts";

            std::vector<const S *> relPaths = {
                &messages,
                &serviceWorkerMessages};

            if (folder.empty())
            {
                setError("Kein Verzeichnis eingegeben.");
                return;
            }
            assert(!folder.empty());
            std::cout << "folder vorher " << folder << "\n";
            if (folder[folder.size() - 1] != '/')
                folder += '/';
            std::cout << "folder nachher " << folder << "\n";
            S error = checkAllFilesExist(folder, relPaths);
            if (!error.empty())
            {
                setError(error);
                return;
            }

            error = incVersions(folder, messages, serviceWorkerMessages);
            if (!error.empty())
            {
                setError(error);
                return;
            }
        }
    }

    void IncVersions::setError(const S &error)
    {
        if (auto w = errorOutput->get())
        {
            w->value(error.c_str());
        }
    }

} // namespace prNewsletter
