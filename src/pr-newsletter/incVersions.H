#include <string>
#include <vector>

/**
 * @return string != "" if error, otherwise "" if successful.
 */
std::string checkAllFilesExist(const std::string &basePath, const std::vector<const std::string *> &relPaths);

std::string incVersions(const std::string &folder, const std::string &messages, const std::string &serviceWorkerMessages);

std::string incVersionsBuildStart(std::string folder, const std::string& project);
