// Lyrics and Chords extension for Gig Performer by @rank13

#include <regex>
#include "LibMain.h"
#include "Panels.h"

namespace gigperformer {
   namespace sdk {
      GigPerformerAPI *CreateGPExtension(LibraryHandle handle) {
         return new LibMain(handle);
      }
   }
}

bool isGigFileLoading = false;
bool isFirstGigFileOpened = false;
std::string extensionPath;

int LibMain::GetMenuCount() {
   return menuNames.size();
}

std::string  LibMain::GetMenuName(int index) {
   std::string text;
   if (index >= 0 && index < (int)menuNames.size())
       text =  menuNames[index];
         
   return text;      
}

void LibMain::InvokeMenu(int index) {
   if (index >= 0 && index < (int)menuNames.size()) {
        switch (index) {
            case 0:
                ExtensionWindow::displayWindow(true);
                break;
            case 1:
                ExtensionWindow::displayWindow(true);
                ExtensionWindow::displayPreferencesContainer(true);
                break;
            default:
                break;   
        }
        isFirstGigFileOpened = true; // Handle scenario where extension is manually activated in GP
    }
}

int LibMain::GetPanelCount() {
   return panelNames.size();
}

std::string LibMain::GetPanelName(int index) {
   std::string text; 
   size_t panelIndex = (size_t)index;
   if (panelIndex >= 0 && panelIndex < panelNames.size())
      text = panelNames[index];
         
   return text;      
}

std::string LibMain::GetPanelXML(int index) {
   std::string text;
   size_t panelIndex = (size_t)index;
   if (panelIndex >= 0 && panelIndex < panelNames.size()) {
        text = panelXML[index];
   }
   return text;
}

std::vector<std::string> LibMain::getSetlistNames() {
    std::vector<std::string> names;
    std::string name;
    int count = getSetlistCount();
    for (int i = 0; i < count; ++i) { 
        name = getSetlistName(i);
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> LibMain::getSongNames() {
    std::vector<std::string> songNames;
    std::string songName;
    int songCount = getSongCount();
    for (int i = 0; i < songCount; ++i) { 
        songName = getSongName(i);
        songNames.push_back(songName);
    }
    return songNames;
}

std::vector<std::string> LibMain::getSongPartNames(int songIndex) {
    std::vector<std::string> songPartNames;
    std::string songPartName;
    int songPartCount = getSongpartCount(songIndex);
    for (int i = 0; i < songPartCount; ++i) { 
        songPartName = getSongpartName(songIndex, i);
        songPartNames.push_back(songPartName);
    }
    return songPartNames;
}

std::vector<std::string> LibMain::getVariationNamesForSong(int songIndex) {
    std::vector<std::string> variationNames;
    std::string variationName;
    int songPartCount = getSongpartCount(songIndex);
    for (int i = 0; i < songPartCount; ++i) { 
        variationName = getVariationNameForSongPart(songIndex, i);
        variationNames.push_back(variationName);
    }
    return variationNames;
}

std::vector<std::string> LibMain::getRackspaceNames() {
    std::vector<std::string> rackspaceNames;
    std::string rackspaceName;
    int rackspaceCount = getRackspaceCount();
    for (int i = 0; i < rackspaceCount; ++i) { 
        rackspaceName = getRackspaceName(i);
        rackspaceNames.push_back(rackspaceName);
    }
    return rackspaceNames;
}

std::vector<std::string> LibMain::getVariationNames(int rackspaceIndex) {
    std::vector<std::string> variationNames;
    std::string variationName;
    int variationCount = getVariationCount(rackspaceIndex);
    for (int i = 0; i < variationCount; ++i) { 
        variationName = getVariationName(rackspaceIndex, i);
        variationNames.push_back(variationName);
    }
    return variationNames;
}

std::vector<std::vector<std::string>> LibMain::getAllVariationNames() {
    std::vector<std::vector<std::string>> allVariationNames;
    std::vector<std::string> variationNames;
    int rackspaceCount = getRackspaceCount();
    for (int i = 0; i < rackspaceCount; ++i) { 
        variationNames = getVariationNames(i);
        allVariationNames.push_back(variationNames);
    }
    return allVariationNames;
}

void LibMain::OnStatusChanged(GPStatusType status) {
    switch (status) {
        case GPStatus_GigStartedLoading:
            isGigFileLoading = true;
            break;
        case GPStatus_GigFinishedLoading:
            isGigFileLoading = false;
            setWidgetValue(WIDGET_CP_SCROLL, 0.0);
            setWidgetValue(WIDGET_CP_AUTOSCROLL_PLAY, 0.0);
            if (!isFirstGigFileOpened) {
                setWidgetValue(WIDGET_DISPLAY, 0.0);
                ExtensionWindow::readPreferencesFile();
                isFirstGigFileOpened = true;
            }
            ExtensionWindow::gigLoaded(getSongNames(),getSetlistNames());
            break;
        case GPStatus_SaveRequest:
            ExtensionWindow::savePreferences();
            break;
        default:
            break;
    }
}

void LibMain::OnOpen() {
    extensionPath = getPathToMe();
    ExtensionWindow::initialize();
}

void LibMain::OnClose() {
    ExtensionWindow::finalize();
}

void LibMain::OnSongChanged(int, int newIndex) {
    if (isGigFileLoading) return;
    if (newIndex >= 0 && inSetlistMode()) {
        setWidgetValue(WIDGET_CP_SCROLL, 0.0);
        setWidgetValue(WIDGET_CP_AUTOSCROLL_PLAY, 0.0);
        ExtensionWindow::songChanged(newIndex, getSongNames());
    }
}

void LibMain::OnSongPartChanged(int oldIndex, int newIndex) {
    if (isGigFileLoading) return;
    if (newIndex >= 0 && oldIndex != newIndex && inSetlistMode()) {
        ExtensionWindow::songPartChanged(newIndex, getCurrentSongIndex());
    }
}

void LibMain::OnSetlistChanged(const std::string&) {
    if (isGigFileLoading) return;
    if (inSetlistMode()) {
        ExtensionWindow::setlistChanged(getCurrentSetlistIndex(), getCurrentSongIndex(), getSetlistNames(), getSongNames());
    }
}

void LibMain::OnModeChanged(int mode) {
    if (isGigFileLoading || !isFirstGigFileOpened) 
        return;
    if (mode == GP_SetlistMode) 
        ExtensionWindow::selectSongForCurrentButton();
}

void LibMain::OnWidgetValueChanged(const std::string& widgetName, double newValue) {
    if (isGigFileLoading) return;
    if (widgetName == WIDGET_DISPLAY) {
        ExtensionWindow::displayWindow(newValue == 1.0);
    } else if (widgetName == WIDGET_SCROLL) {
        ExtensionWindow::scrollWindow(newValue);
    } else if (widgetName == WIDGET_CP_SCROLL) {
        ExtensionWindow::chordProScrollWindow(newValue);
    } else if(widgetName == WIDGET_CP_UP && newValue == 1.0) {
        ExtensionWindow::chordProUp();
    } else if(widgetName == WIDGET_CP_DOWN && newValue == 1.0) {
        ExtensionWindow::chordProDown();
    } else if(widgetName == WIDGET_PREVIOUS && newValue == 1.0) {
        ExtensionWindow::incrementButton(-1);
    } else if(widgetName == WIDGET_NEXT && newValue == 1.0) {
        ExtensionWindow::incrementButton(1);
    } else if(widgetName == WIDGET_CP_AUTOSCROLL_PLAY) {
        ExtensionWindow::chordProAutoScrollPlay(newValue == 1.0);
    } else if(widgetName == WIDGET_CP_AUTOSCROLL_RESET && newValue == 1.0) {
        ExtensionWindow::chordProAutoScrollReset();
    } 
}

void LibMain::OnWidgetCaptionChanged(const std::string &widgetName, const std::string &newValue) {
    if (widgetName == WIDGET_CP_MARKER) {
        ExtensionWindow::chordProScrollToMarker(newValue);
    }
}

void LibMain::OnGlobalPlayStateChanged(double playing) {
    ExtensionWindow::playheadChange(playing > 0.0);
}

std::string LibMain::GetProductDescription() {
    return XMLProductDescription;
}