// Rackspace and song selector extension for Gig Performer by @rank13

#pragma once

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "Constants.h"
#include "LookAndFeel.h"
#include "Timer.h"
#include <regex>

using namespace juce;

extern Colour viewPortBackground;

class MyDraggableComponent : public Component 
{
public:
  MyDraggableComponent() { constrainer.setMinimumOnscreenAmounts (0xffffff, 0xffffff, 0xffffff, 0xffffff); }
  ComponentDragger dragger;
  ComponentBoundsConstrainer constrainer;
  void mouseDown (const MouseEvent& e) override
  {
      dragger.startDraggingComponent (this, e);
  }
  void mouseDrag (const MouseEvent& e) override
  {
      dragger.dragComponent (this, e, &constrainer);
  }
  void paint(Graphics& g) override 
  { 
      if (isMouseOver(true)) {
               g.setColour (Colour (0xff3B4D60));
      } else { 
            g.setColour (Colour (0xff555555));
      } 
      const juce::Rectangle<float> area = getLocalBounds().toFloat();
      g.fillRect(area.withWidth(5)); // Override displayed width of separator
  }

  void mouseEnter (const MouseEvent&) override {
      repaint();
  }

  void mouseExit (const MouseEvent&) override {
      repaint();
  }
};

class PopOver : public Component
{
public:
  void paint(Graphics& g) override {
    auto brightness = (viewPortBackground.getBrightness() == 0.0) ? viewPortBackground.getBrightness() + 0.2 :  viewPortBackground.getBrightness() - 0.2;
    g.fillAll (viewPortBackground.withBrightness(brightness).withAlpha(0.9f));
  } 
};

class ChordProContainer : public Component
{
public:
  void mouseDown (const MouseEvent& e) override;

};

class SetlistContainer : public Component
{
public:
  void paint(Graphics& g) override {
    g.fillAll (Colour (0xFF1A1A1A));
  } ;

};

class PreferencesContainer : public Component
{
public:
  void paint(Graphics& g) override {
    //g.fillAll (Colour (0xFF1A1A1A));
    int indent = 50;
    int prefIndent = 100;
    int colorIndent = 140;
    int titleHeight = 50;
    int subtitleHeight = 40;
    int prefHeight = 35;
    int runningY = titleHeight;

    g.setColour(Colours::white);
    g.setFont (Font (40.0f, Font::plain).withTypefaceStyle ("Regular"));
    g.drawFittedText ("Preferences",
      indent, 10, getWidth(), titleHeight,
      Justification::left, 1, 1.0f);

    runningY += titleHeight;
    
    g.setColour(Colour (0xFFD0D0D0));
    g.setFont (Font (30.0f, Font::plain).withTypefaceStyle ("Regular"));
    g.drawFittedText ("Song List",
      indent, runningY, getWidth(), subtitleHeight,
      Justification::left, 1, 1.0f);
    
    runningY += titleHeight;

    g.setFont (Font (20.0f, Font::plain).withTypefaceStyle ("Regular"));
    g.drawFittedText ("Zero-based numbering",
      prefIndent, runningY, getWidth(), prefHeight,
      Justification::left, 1, 1.0f);

    runningY += prefHeight;
    
    g.drawFittedText ("Large scroll area",
      prefIndent, runningY, getWidth(), prefHeight,
      Justification::left, 1, 1.0f);
    
    runningY += prefHeight;

    g.drawFittedText ("Thick borders for selected songs/parts",
      prefIndent, runningY, getWidth(), prefHeight,
      Justification::left, 1, 1.0f);
    
    runningY += prefHeight;
    
    g.drawFittedText ("Display variation names instead of song part names",
      prefIndent, runningY, getWidth(), prefHeight,
      Justification::left, 1, 1.0f);

    runningY += titleHeight;

    g.setFont (Font (30.0f, Font::plain).withTypefaceStyle ("Regular"));
    g.drawFittedText ("Lyrics/Chords",
      indent, runningY, getWidth(), subtitleHeight,
      Justification::left, 1, 1.0f);

    runningY += titleHeight;
    
    g.setFont (Font (20.0f, Font::plain).withTypefaceStyle ("Regular"));
    g.drawFittedText ("Display section labels in left margin",
      prefIndent, runningY, getWidth(), prefHeight,
      Justification::left, 1, 1.0f);
    
    runningY += prefHeight;

    g.drawFittedText ("Use smaller font for chords above lyrics",
      prefIndent, runningY, getWidth(), prefHeight,
      Justification::left, 1, 1.0f);

    runningY += (prefHeight + 12);

    g.drawFittedText ("Light mode chord color",
      colorIndent, runningY, getWidth(), prefHeight,
      Justification::left, 1, 1.0f);

    runningY += prefHeight;

    g.drawFittedText ("Light mode lyric color",
      colorIndent, runningY, getWidth(), prefHeight,
      Justification::left, 1, 1.0f);
    
    runningY += prefHeight;

    g.drawFittedText ("Dark mode chord color",
      colorIndent, runningY, getWidth(), prefHeight,
      Justification::left, 1, 1.0f);
    
    runningY += prefHeight;

    g.drawFittedText ("Dark mode lyric color",
      colorIndent, runningY, getWidth(), prefHeight,
      Justification::left, 1, 1.0f);
  } ;

};

class RightViewPort : public Viewport
{
public:
  void paint(Graphics& g) override {
    g.fillAll (viewPortBackground);
  } ;

  //void mouseDown (const MouseEvent& e) override;

};

class MyDocumentWindow : public DocumentWindow
{
  public:
  MyDocumentWindow() :
  DocumentWindow("GP Selecter", Colours::lightgrey, DocumentWindow::allButtons, true)
  {
    setWantsKeyboardFocus(true);
  }
  virtual ~MyDocumentWindow() { }
  virtual void closeButtonPressed () override;
  virtual void moved () override;
};

class ExtensionWindow  :  public juce::Component,
                          public juce::Button::Listener                 
{
public:
  ExtensionWindow ();
  virtual ~ExtensionWindow() override;
  void closeButtonPressed();
  void static initialize(); // Call this to show the component
  void static finalize(); // Call this when library is about to be unloaded to free resources
  void paint (Graphics& g) override;
  void resized() override;
  void buttonClicked (Button* buttonThatWasClicked) override;
  void closeWindow();
  void static updateButtonLabel(const String& label);
  void static addButtons(int count);
  void static updateButtonNames(std::vector<std::string> buttonNames);
  void static compareButtonNames(std::vector<std::string> buttonNames);
  void static addSubButtons(int count);
  void static updateSubButtonNames(std::vector<std::string> buttonNames);
  void static compareSubButtonNames(std::vector<std::string> buttonNames);
  std::vector<std::string> static getSubButtonNamesByIndex(int index);
  std::string static getSubButtonNameByIndex(int index, int subIndex);
  void static updateSetlistButtons(std::vector<std::string> buttonNames);
  bool static isButtonSelected(int index);
  int static getButtonSelected();
  int static getVisibleButtonCount();
  int static getVisibleSubButtonCount();
  void static incrementButton(int increment);
  bool static getDisplayVariationForSongPartStatus();
  void static selectButton(int index);
  void static selectSetlistButton(int index);
  //void static updatePrevCurrNext(int index);
  bool static isSubButtonSelected(int index);
  bool static isSubButtonsCollapsed(); 
  void static selectSubButton(int index);
  void static selectButtonAndSubButton(int index, std::vector<std::string> buttonNames);
  void static selectSongForCurrentButton();
  void static updateClock(const String& formattedTime);
  void static setZeroBasedNumbering(bool zeroBased);
  void static setImmediateSwitching(bool immediateSwitch);
  void static setLargeScrollArea(bool largeScrollArea);
  void static setDarkMode(bool darkMode);
  void static setDisplayVariationNames(bool display);
  void static setFitToPageHeight(bool fitToHeight);
  void static setDisplayTwoPages(bool display);
  void static chordProSetFontSize(float newSize);
  void static chordProSetLeftMarginLabels(bool onLeft);
  void static chordProSetSmallChordFont(bool isSmall);
  void static updateViewportPositionForSubButtons();
  void static toggleZeroBasedNumbering();
  void static toggleImmediateSwitching();
  void static toggleLargeScrollArea();  
  void static toggleThickBorders();  
  void static toggleLockToSetlistMode();
  void static toggleVariationsInSetlistMode();
  void static toggleLeftMarginLabels();
  void static toggleDarkMode();
  void static toggleSmallChordFont();
  //void static displayRackspaceVariationInSetlistMode(bool display);
  //void static toggleRackspaceVariationInSetlistMode();
  String buttonName(int index);
  void static displayWindow(bool display);
  void static displayExpandedWindow(bool display);
  void static checkSongListPosition();
  void static scrollWindow(double value);
  void static setTitleBarName(const String& name);
  void static processPreferencesDefaults(StringPairArray prefs);
  void static processPreferencesColors(StringPairArray prefs);
  void static processPreferencesChordProColors(StringPairArray prefs);
  void static processPreferencesWindowState(StringPairArray prefs);
  void static removeColorKeywordFromName(bool remove);
  void static refreshUI();
  void static setWindowPositionAndSize(int x, int y, int w, int h);
  void static setSongLabel();
  void static displayFontContainer(bool display);
  void static displaySetlistContainer(bool display);
  void static displayPreferences();

  Image static getWindowIcon();
  void mouseDrag ( const MouseEvent& /*e*/) override
    {
        resized();
    }
  void static chordProScrollWindow(double value);
  void static chordProUp();
  void static chordProDown();
  void static chordProProcessText(std::string text);
  void static chordProReadFile(int index);
  void static chordProScrollToSongPart(std::string text);
  void chordProDisplayGUI(bool display);
  void chordProSetColors();
  //void chordProSetFontSize(float newSize);
  void chordProImagesCheckAndAdd(int index);
  int chordProGetVisibleImageCount();
  void static chordProCreateInvertedImages();
  void static saveWindowState();
  void static savePreferences();
  void static restartWindowTimer();

  static ExtensionWindow* extension;
  MyDraggableComponent draggableResizer;
  WindowChangeTimer windowTimer;
  SharedResourcePointer<buttonLookAndFeel> buttonsLnF;
  SharedResourcePointer<gridButtonLookAndFeel> gridButtonsLnF;
  SharedResourcePointer<headerSongs> headerSongsLnF;
  SharedResourcePointer<headerRackspaces> headerRackspacesLnF;
  SharedResourcePointer<headerLookAndFeel> headerLnF;
  SharedResourcePointer<subButtonLookAndFeel> subButtonsLnF;
  SharedResourcePointer<minimalistSong> minimalistSongLnF;
  SharedResourcePointer<subButtonHighlightLookAndFeel> highlightLnF;
  SharedResourcePointer<blankButtonLookAndFeel> blankLnF;
  SharedResourcePointer<chordPro> chordProLnF;
  SharedResourcePointer<chordProTitle> chordProTitleLnF;
  SharedResourcePointer<chordProSubTitle> chordProSubTitleLnF;
  SharedResourcePointer<chordProComment> chordProCommentLnF;
  SharedResourcePointer<chordProLabel> chordProLabelLnF;  
  SharedResourcePointer<chordProTab> chordProTabLnF;
  SharedResourcePointer<chordProGrid> chordProGridLnF;
  SharedResourcePointer<popOverLookAndFeel> popOverLnf;
  SharedResourcePointer<popOverLabel> popOverLabelLnf;
  SharedResourcePointer<setlistButtonLookAndFeel> setlistButtonLnF;
  SharedResourcePointer<setlistListButtonLookAndFeel> setlistListButtonLnF;
  SharedResourcePointer<noSongLabelLookAndFeel> noSongLabelLnF;
  SharedResourcePointer<noChordProLabelLookAndFeel> noChordProLabelLnF;

 private:
  void log(String text);

  void chordProRefresh();
  void chordProReset();
  String static getWindowState();
  String static getDefaults();
  String static getChordProColors();
  void static setWindowState();
  Rectangle<int> static getWindowPositionAndSize();
  void static setSongPanelPosition(bool display);
  void updatePreferencesWindow();
  void updatePreferencesColors();

  std::unique_ptr<MyDocumentWindow> extensionWindow;
  TooltipWindow tooltipWindow;
  Viewport viewport;
  RightViewPort viewportRight;
  Component container;
  Component containerRight;
  PreferencesContainer preferencesContainer;
  ChordProContainer chordProContainer;
  SetlistContainer setlistContainer;
  std::unique_ptr<DrawableButton> prefToggle;

  PopOver fontButtonContainer;
  PopOver missingImageContainer;
  OwnedArray<TextButton> buttons;
  OwnedArray<TextButton> subButtons;
  OwnedArray<TextButton> setlistButtons;
  OwnedArray<Label> chordProLines;
  OwnedArray<ImageComponent> chordProImages;
  OwnedArray<DrawableButton> prefButtons;
  OwnedArray<Label> prefColorLabels;
  StringPairArray buttonColors;
  StringPairArray chordProColors;
  ClockTimer clockTimer;
  RefreshTimer refreshTimer;
  CreateImageTimer imageTimer;
  bool displayRightPanel = true;
  bool displaySongPanel = true;
  bool displayWindowOnLoad = false;
  bool chordProForCurrentSong = false;
  bool chordProImagesOnly = false;
  bool chordProTwoColumns = false;
  //bool chordProDarkMode = false;
  bool fitHeight = false;
  bool prefsLoaded = false;
  bool pendingDisplayWindow = false;
  bool windowPinned = false;
  bool windowFullscreen = false;
  std::unique_ptr<int> switchImmediately;
  int prevButtonSelected = 0;
  std::unique_ptr<Label> highlight;
  std::unique_ptr<Label> header;
  std::unique_ptr<Label> clock;
  std::unique_ptr<Label> fontPopOverLabel;
  std::unique_ptr<Label> missingImageLabel;
  std::unique_ptr<Label> noSongsLabel;
  std::unique_ptr<Label> noChordProLabel;
  std::unique_ptr<TextButton> btnCurrent;
  std::unique_ptr<TextButton> btnPrev;
  std::unique_ptr<TextButton> btnNext;
  std::unique_ptr<TextButton> btnModeSwitch;
  std::unique_ptr<TextButton> fontUp;
  std::unique_ptr<TextButton> fontDown;
  std::unique_ptr<TextButton> fontMono;
  std::unique_ptr<TextButton> setlistButton;
  std::unique_ptr<TextButton> createInvertedImage;
  std::unique_ptr<DynamicObject> preferences;
  std::unique_ptr<DynamicObject> preferencesChordProColors;
  std::unique_ptr<ShapeButton> sidePanelOpenButton;
  std::unique_ptr<ShapeButton> sidePanelCloseButton;
  std::unique_ptr<ShapeButton> pinUnpinnedButton;
  std::unique_ptr<ShapeButton> pinPinnedButton;
  std::unique_ptr<ShapeButton> fullscreenActivateButton;
  std::unique_ptr<ShapeButton> fullscreenDeactivateButton;
  std::unique_ptr<ShapeButton> fontButton;
  std::unique_ptr<ShapeButton> lightDarkModeButton;
  std::unique_ptr<ShapeButton> columnsTwoButton;
  std::unique_ptr<ShapeButton> columnsOneButton;
  std::unique_ptr<ShapeButton> fitWidthButton;
  std::unique_ptr<ShapeButton> fitHeightButton;
  std::unique_ptr<ShapeButton> closeButton;
  std::unique_ptr<ShapeButton> preferencesButton;
  Image menuIcon;
  ImageComponent menuIconComponent;



  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtensionWindow)
};



