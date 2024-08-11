// Lyrics and Chords extension for Gig Performer by @rank13

#pragma once

#include <gigperformer/sdk/GPUtils.h>
#include <gigperformer/sdk/GigPerformerAPI.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "ExtensionWindow.h"
#include "Constants.h"
#include "Version.h"

const std::string XMLProductDescription =
    "<Library>"
    "<Product Name=\"" + PROJECT_TITLE + "\" Version=\"" + PROJECT_VERSION + "\" BuildDate=\"" + PROJECT_BUILD_DATE + "\"></Product>"
    "<Description>" + PROJECT_DESCRIPTION + "</Description>"
    "<ImagePath></ImagePath>"
    "</Library>";

class LibMain : public gigperformer::sdk::GigPerformerAPI
{
protected:
    int GetMenuCount() override;
    std::string GetMenuName(int index) override;
    void InvokeMenu(int itemIndex) override;

    int GetPanelCount() override;
    std::string GetPanelName(int index) override;
    std::string GetPanelXML(int index) override;

public:
    LibMain();
    LibMain(LibraryHandle handle) : GigPerformerAPI(handle)  {}
    virtual ~LibMain() {}
    void OnOpen() override;
    void OnClose() override;
    void OnModeChanged(int mode) override;
    void OnSongChanged(int oldIndex, int newIndex) override;
    void OnSongPartChanged(int oldIndex, int newIndex) override;
    void OnSetlistChanged(const std::string &newSetlistName) override;
    void OnWidgetValueChanged(const std::string &widgetName, double newValue) override;    
    void OnStatusChanged(GPStatusType status) override; 
    void OnGlobalPlayStateChanged(double playing) override;
    void Initialization() override
    {
        registerCallback("OnOpen");
        registerCallback("OnClose");
        registerCallback("OnStatusChanged");
        registerCallback("OnSongChanged");
        registerCallback("OnSongPartChanged");
        registerCallback("OnSetlistChanged");
        registerCallback("OnModeChanged");
        registerCallback("OnWidgetValueChanged");
        registerCallback("OnGlobalPlayStateChanged");

        listenForWidget(WIDGET_DISPLAY, true);
        listenForWidget(WIDGET_SCROLL, true);
        listenForWidget(WIDGET_PREVIOUS, true);
        listenForWidget(WIDGET_NEXT, true);
        listenForWidget(WIDGET_CP_SCROLL, true);
        listenForWidget(WIDGET_CP_UP, true);
        listenForWidget(WIDGET_CP_DOWN, true);
        listenForWidget(WIDGET_CP_AUTOSCROLL_PLAY, true);
        listenForWidget(WIDGET_CP_AUTOSCROLL_RESET, true);
    }
    
    std::string GetProductDescription() override;
    std::vector<std::string> getRackspaceNames();
    std::vector<std::string> getVariationNames(int rackspaceIndex);
    std::vector<std::string> getSetlistNames();
    std::vector<std::string> getSongNames();
    std::vector<std::string> getSongPartNames(int songIndex);
    std::vector<std::string> getVariationNamesForSong(int songIndex);

private:
    std::vector<std::vector<std::string>> getAllVariationNames();
};