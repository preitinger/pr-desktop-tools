#include "GenActionCodeAction.H"

#include <iostream>
#include <fstream>
#include <string>

#include <FL/Fl_Input.H>
#include <FL/Fl_Group.H>

GenActionCodeAction::GenActionCodeAction(BaseActionWP &&parent, const WidgetAccessSP<Fl_Group> &group)
    : BaseAction(std::move(parent)),
      group(group)
{
    std::cout << "GenActionCodeAction(&&)\n";
}

// virtual
GenActionCodeAction::~GenActionCodeAction()
{
}

void GenActionCodeAction::start() /* override final */
{
    std::cout << "BEGIN GenActionCodeAction::start\n";
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
        auto innerGroup = widget<GenActionCodeAction, Fl_Group>(x, y += 10, w, gh - 30, "GenActionCode", nullptr);
        innerGroup->get()->box(Fl_Boxtype::FL_DOWN_BOX);
        x += 10;
        w -= 20;
        y += 10;

        cancelB = button(x, y, w, h, "Cancel", &GenActionCodeAction::onCancel);
        // typedef void (GenActionCodeAction::*MyMethodPtr)();
        nameInput = widget<GenActionCodeAction, Fl_Input>(x + 100, y += lh, w - 100, h, "Klassenname", &GenActionCodeAction::onInput);
        nameInput->get()->when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
        innerGroup->get()->end();
        std::cout << "end GenActionCodeAction::start\n";
    }
}

void GenActionCodeAction::onCancel()
{
    exit();
}

static std::string defineNameFromClassName(const std::string &className)
{
    std::string defineName("_");
    for (char c : className)
    {
        if ('A' <= c && c <= 'Z') {
            (defineName += '_') += c;
        } else if ('a' <= c && c <= 'z')
        {
            defineName += (c - ('a' - 'A'));
        }
        else
        {
            defineName += c;
        }

    }
    return defineName + "_H__";
}

static std::string replaceAll(const std::string &src, const std::string &searched, const std::string &replaced)
{
    std::string res;
    std::size_t pos = 0;
    do
    {
        std::size_t pos2 = src.find(searched, pos);
        if (pos2 == std::string::npos) {
            // not found
            res.append(src.begin() + pos, src.end());
            break;
        } else {
            res.append(src.begin() + pos, src.begin() + pos2);
            res.append(replaced);
            pos = pos2 + searched.size();
        }
    } while (true);

    return res;
}

static void genFromTemplate(const std::string& templateFile, const std::string& outFile, const std::vector<std::string>& replacements)
{

    std::ifstream f(templateFile);
    std::string line;
    std::ofstream out(outFile.c_str(), std::ios_base::app);

    while (std::getline(f, line), f.good())
    {
        std::cout << "original line: " << line << "\n";
        for (auto it = replacements.begin(); it != replacements.end(); ++it)
        {
            auto toFind = *it;
            ++it;
            assert (it != replacements.end());
            auto toReplace = *it;
            line = replaceAll(line, toFind, toReplace);
        }
        std::cout << "Would now write line: " << line << "\n";
        out << line << "\n";
    }

    f.close();
    out.close();

}

void GenActionCodeAction::onInput()
{
    // TODO generate header and source file for the new class with the name in input
    std::string className(nameInput->get()->value());
    std::cout << "Gonna create source for new action class: '" << className << "'\n";
    std::string defineName = defineNameFromClassName(className);

    std::vector<std::string> replacements = {
        "${define}", defineName,
        "${class}", className
    };

    genFromTemplate("templates/Action_H.txt", std::string("src/") + className + ".H", replacements);
    genFromTemplate("templates/Action_cxx.txt", std::string("src/") + className + ".cxx", replacements);
}
