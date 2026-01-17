#ifndef __INC_VERSIONS_BUILD_ALL_START_H__
#define __INC_VERSIONS_BUILD_ALL_START_H__

#include "../BaseAction.H"
#include "../WidgetAccess.H"

#include <sys/types.h>

class Fl_Input;
class Fl_File_Chooser;


namespace prNewsletter {

SCLASS(IncVersions) : public BaseAction
{
public:
    IncVersions(BaseActionWP&& parent, const WidgetAccessSP<Fl_Group> &group);
    virtual ~IncVersions();
    void start() override final;

    private:
    void onCancel();
    void onSearch();
    void onRunOrStop();
    // void onInput();
    // void onTimeout();
    void onFileChooser(Fl_File_Chooser * fc);
    void setError(const std::string& error);

    private:
    WidgetAccessSP<Fl_Group> group;
    ButtonSP cancelB;
    WidgetInActionSP<Fl_Input> folderInput;
    // TimeoutInActionSP nextTimeout;
    ButtonSP search;
    std::shared_ptr<Fl_File_Chooser> fileChooser;
    ButtonSP runOrStop;
    WidgetInActionSP<Fl_Output> errorOutput;
    pid_t pid;

    friend void cbFileChooser(Fl_File_Chooser * fc, void* data);

};

} // namespace prNewsletter

#endif // __INC_VERSIONS_BUILD_ALL_START_H__
