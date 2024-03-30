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

bool isGigFileLoading = true;
bool isFirstGigFileOpened = true;
std::string extensionPath;

int LibMain::GetMenuCount()
{
   return menuNames.size();
}

 std::string  LibMain::GetMenuName(int index)
{
   std::string text;
   if (index >= 0 && index < (int)menuNames.size())
       text =  menuNames[index];
         
   return text;      
}

void LibMain::InvokeMenu(int index)
{
   if (index >= 0 && index < (int)menuNames.size())
         {
            switch (index)
               {
                  case 0:
                      ExtensionWindow::displayWindow(true);
                     break;
                /*
                  case 1:
                    ExtensionWindow::toggleZeroBasedNumbering();
                    break;
                  case 2:
                    ExtensionWindow::toggleLargeScrollArea();
                    break;
                  case 3:
                    ExtensionWindow::toggleThickBorders();
                    break;
                  case 4:
                    ExtensionWindow::toggleVariationsInSetlistMode();
                    break;
                  case 5:
                    ExtensionWindow::toggleLeftMarginLabels();
                    break;
                */
                  case 1:
                    ExtensionWindow::displayWindow(true);
                    ExtensionWindow::displayPreferencesContainer(true);
                    break;
                  default:
                     break;   
               }
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
            //consoleLog("Gig Finished Loading");
            isGigFileLoading = false;
            if (isFirstGigFileOpened) {
                readPreferencesFile();
                isFirstGigFileOpened = false;
            }
            ExtensionWindow::updateButtonNames(getSongNames());
            ExtensionWindow::updateSetlistButtons(getSetlistNames());
            ExtensionWindow::chordProReadFile(0);
            setWidgetValue(WIDGET_CP_SCROLL, 0.0);
            if (!ExtensionWindow::isButtonSelected(0)) { // If selected in GP directly, ensure buttons are in sync
                ExtensionWindow::selectButton(0);
                ExtensionWindow::updateSubButtonNames(ExtensionWindow::getDisplayVariationForSongPartStatus() ? getVariationNamesForSong(0) : getSongPartNames(0));
                ExtensionWindow::selectSubButton(getCurrentSongpartIndex());
            } else {
                ExtensionWindow::updateSubButtonNames(ExtensionWindow::getDisplayVariationForSongPartStatus() ? getVariationNamesForSong(0) : getSongPartNames(0));
            }

            ExtensionWindow::refreshUI();
            break;
        case GPStatus_RackspaceListModified:
            //consoleLog("Rackspace list changed");
            break;
        case GPStatus_SongListModifed:
            //consoleLog("Song list changed");
            break;
        case GPStatus_SongPartListModified:
            //consoleLog("SongPart list changed");
            break;
        case GPStatus_VariationListModified:
            //consoleLog("Variation list changed");
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

void LibMain::OnRackspaceActivated() {
    /*
    if (isGigFileLoading) return;
    if (!inSetlistMode()) {
        int index = getCurrentRackspaceIndex();
        if (index >= 0) {
            ExtensionWindow::updateButtonNames(getRackspaceNames());
            if (!ExtensionWindow::isButtonSelected(index)) { // If selected in GP directly, ensure buttons are in sync
                ExtensionWindow::selectButton(index);
                ExtensionWindow::updateSubButtonNames(getVariationNames(index));
                ExtensionWindow::selectSubButton(getCurrentVariationIndex());
            } else {
                ExtensionWindow::updateSubButtonNames(getVariationNames(index));
            }
        }
    }
    */
}

void LibMain::OnVariationChanged(int oldIndex, int newIndex) {
    /*
    if (isGigFileLoading) return;
    if (newIndex >= 0 && oldIndex != newIndex && !inSetlistMode()) {
        int rackspaceIndex = getCurrentRackspaceIndex();
        if (!ExtensionWindow::isSubButtonSelected(newIndex)) {
            ExtensionWindow::selectSubButton(newIndex);
        ExtensionWindow::updateSubButtonNames(getVariationNames(rackspaceIndex));
        }
    }
    */
}

void LibMain::OnSongChanged(int, int newIndex) {
    if (isGigFileLoading) return;
    if (newIndex >= 0 && inSetlistMode()) {
        ExtensionWindow::updateButtonNames(getSongNames());
        ExtensionWindow::chordProReadFile(newIndex);
        setWidgetValue(WIDGET_CP_SCROLL, 0.0);
        if (!ExtensionWindow::isButtonSelected(newIndex)) { // If selected in GP directly, ensure buttons are in sync
            ExtensionWindow::selectButton(newIndex);
            ExtensionWindow::updateSubButtonNames(ExtensionWindow::getDisplayVariationForSongPartStatus() ? getVariationNamesForSong(newIndex) : getSongPartNames(newIndex));
            ExtensionWindow::selectSubButton(getCurrentSongpartIndex());
        } else {
            ExtensionWindow::updateSubButtonNames(ExtensionWindow::getDisplayVariationForSongPartStatus() ? getVariationNamesForSong(newIndex) : getSongPartNames(newIndex));
        }
    }
}

void LibMain::OnSongPartChanged(int oldIndex, int newIndex) {
    if (isGigFileLoading) return;
    if (newIndex >= 0 && oldIndex != newIndex && inSetlistMode()) {
        int songIndex = getCurrentSongIndex();
        if (!ExtensionWindow::isSubButtonSelected(newIndex)) {
            ExtensionWindow::updateSubButtonNames(ExtensionWindow::getDisplayVariationForSongPartStatus() ? getVariationNamesForSong(songIndex) : getSongPartNames(songIndex));
            ExtensionWindow::selectSubButton(newIndex);
            ExtensionWindow::chordProScrollToSongPart(getSongpartName(getCurrentSongIndex(), newIndex));
        }
    }
}

void LibMain::OnSetlistChanged(const std::string&) {
    if (isGigFileLoading) return;
    if (inSetlistMode()) {
        int songIndex = getCurrentSongIndex();
        ExtensionWindow::updateSetlistButtons(getSetlistNames());
        ExtensionWindow::updateButtonNames(getSongNames());
        //ExtensionWindow::selectButton(getCurrentSongIndex());
        ExtensionWindow::selectSetlistButton(getCurrentSetlistIndex());
        ExtensionWindow::chordProReadFile(songIndex);
        if (!ExtensionWindow::isButtonSelected(songIndex)) { // If selected in GP directly, ensure buttons are in sync
            ExtensionWindow::selectButton(songIndex);
            ExtensionWindow::updateSubButtonNames(ExtensionWindow::getDisplayVariationForSongPartStatus() ? getVariationNamesForSong(songIndex) : getSongPartNames(songIndex));
            ExtensionWindow::selectSubButton(getCurrentSongpartIndex());
        }
    }
}

void LibMain::OnModeChanged(int mode) {
    if (isGigFileLoading) return;
    //readPreferencesFile("colors");
    //if (mode == GP_SetlistMode) setWidgetValue(WIDGET_CP_SCROLL, 0.0);
    //ExtensionWindow::refreshUI();
    if (mode == GP_SetlistMode) ExtensionWindow::selectSongForCurrentButton();
}

// Handle widget changes
void LibMain::OnWidgetValueChanged(const std::string& widgetName, double newValue) {
    if (widgetName == WIDGET_DISPLAY) {
        if (newValue == 1.0) {
         ExtensionWindow::displayWindow(true);
        } else {
            ExtensionWindow::displayWindow(false);
        }
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
    } 
}

void LibMain::readPreferencesFile() {
    std::string prefsFileText;
    std::string prefsFilePath = getPathToMe() + PATH_SEPARATOR() + PREF_FILENAME;
    gigperformer::sdk::GPUtils::loadTextFile(prefsFilePath, prefsFileText);
    StringArray lines = StringArray::fromLines(prefsFileText);
    StringArray keyValue;
    StringPairArray defaults;
    StringPairArray colors;
    StringPairArray chordpro;
    StringPairArray windowstate;
    String line;
    String prefSection;
    //consoleLog(lines[1].toStdString());
    for (int i = 0; i < lines.size(); ++i) { 
        line = lines[i].toStdString();
        if (line.contains("[")) { // Preference Heading/Section
            prefSection = line.removeCharacters("[]");
            //consoleLog("Line " + std::to_string(i) + " is heading: " + prefSection.toStdString());
        } else if (line.trim() != "") { // Process Preferences, assuming key/value pairs
            line = line.removeCharacters(" ");
            keyValue = StringArray::fromTokens(line,"=","");
            //consoleLog(keyValue[0].toStdString() + " / " + keyValue[1].toStdString() + " / " + prefSection.toStdString());
            if (prefSection.contains("Defaults")) {
                defaults.set(keyValue[0], keyValue[1]);
            } else if (prefSection.contains("SongPartColors")) {
                colors.set(keyValue[0], keyValue[1]);
            } else if (prefSection.contains("LyricChordColors")) {
                chordpro.set(keyValue[0], keyValue[1]);
            } else if (prefSection.contains("WindowLastSavedState")) {
                windowstate.set(keyValue[0], keyValue[1]);
            }
        }
    }
    ExtensionWindow::processPreferencesDefaults(defaults);
    ExtensionWindow::processPreferencesColors(colors);
    ExtensionWindow::processPreferencesChordProColors(chordpro);
    ExtensionWindow::processPreferencesWindowState(windowstate);
}

std::string LibMain::GetProductDescription()  // This MUST be defined in your class
{
    return XMLProductDescription;
}