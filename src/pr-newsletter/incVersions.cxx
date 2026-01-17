#include "incVersions.H"

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>

#include <cassert>

typedef std::string S;

/**
 * @return string != "" if error, otherwise "" if successful.
 */
S checkAllFilesExist(const S &basePath, const std::vector<const S *> &relPaths)
{
    for (const S *relPath : relPaths)
    {
        S path = basePath + "/" + *relPath;

        if (!std::filesystem::exists(path))
        {
            std::ostringstream o;
            o << "\"" << path << "\" nicht gefunden.";
            return o.str();
        }
    }

    return "";
}

static bool strStartsWith(const S &s, const S &prefix)
{
    auto prefixSize = prefix.size();
    auto size = s.size();
    return (s.compare(0, prefixSize, prefix) == 0);
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
            auto pos = prefix.size();
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

std::string incVersionsBuildStart(std::string folder, const std::string& project)
{

    S swProj("../serviceWorker-for-pr-push-newsletter3");
    S messages = swProj + "/src/messages.ts";
    S serviceWorkerMessages = "app/_lib/client/serviceWorkerMessages.ts";

    std::vector<const S *> relPaths = {
        &messages,
        &serviceWorkerMessages};

    if (folder.empty())
    {
        return "Kein Verzeichnis eingegeben.";
    }
    assert(!folder.empty());
    std::cout << "folder vorher " << folder << "\n";
    if (folder[folder.size() - 1] != '/')
        folder += '/';
    std::cout << "folder nachher " << folder << "\n";
    S error = checkAllFilesExist(folder, relPaths);
    if (!error.empty())
    {
        return error;
    }

    error = incVersions(folder, messages, serviceWorkerMessages);
    if (!error.empty())
    {
        return error;
    }

    // In project serviceWorker-for-pr-push-newsletter3, execute npm run 'build & copy'
    S cmd = "cd '" + (folder + swProj) + "' && npm run build && npm run 'copy to " + project + "'";
    int res = std::system(cmd.c_str());

    if (res)
    {
        return "Error code of " + cmd + ": " + std::to_string(res);
    }

    // In project pr-newsletter, execute npm run 'build+start'
    cmd = "cd '" + folder + "' && npm run 'build+start'";
    res = std::system(cmd.c_str());

    if (res)
    {
        return "Error code of " + cmd + ": " + std::to_string(res);
    }

    return "";
}