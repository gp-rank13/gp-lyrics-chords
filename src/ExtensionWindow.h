// Lyrics and Chords extension for Gig Performer by @rank13

#pragma once

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "Constants.h"
#include "ChordPro.h"
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

  void mouseUp (const MouseEvent& ) override
  {
      dragging = false;
  }

  void mouseDrag (const MouseEvent& e) override
  {
      dragger.dragComponent (this, e, &constrainer);
      dragging = true;
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

  void mouseEnter (const MouseEvent& ) override {
      repaint();
  }

  void mouseExit (const MouseEvent& ) override {
      repaint();
  }

  bool isDragging() {
    return dragging;
  }

private:
   bool dragging = false;
};

class ChordDiagramKeys : public Component
{
public:
  String label = "Chord";
  void paint(Graphics& g) override {
      int newY = getHeight() / 3;
      g.setFont(Font(newY));
      g.setColour(Colour(0xFFF0F0F0));
      auto bounds = getLocalBounds();
      g.drawFittedText(label, bounds.withHeight(newY), Justification::centred, 1, 1.0f);
      //g.setColour(Colours::red);
      //g.fillRect(0, newY, getWidth(), getHeight() - newY);//.withTrimmedTop(getHeight() / 4));
      //g.fillAll(Colours::red);
      g.setColour(Colour(0xFFD0D0D0));
      int w = getWidth() / 13;
      int pad = getHeight() * 0.05;
      for (int i = 0; i < 14; ++i) {
        g.drawRect(i * (w - 1), newY + pad, w, getHeight() - newY - pad);
      }
        

  }

  void updateLabel(String newLabel) {
    label = newLabel;
  }
};

class PopOver : public Component
{
public:
  void paint(Graphics& g) override {
    auto brightness = (viewPortBackground.getBrightness() == 0.0) ? viewPortBackground.getBrightness() + 0.2 :  viewPortBackground.getBrightness() - 0.2;
    if (viewPortBackground == Colour::fromString(CP_DARK_IMAGES_BACKGROUND_COLOR)) brightness = 0.2;
    g.fillAll (viewPortBackground.withBrightness(brightness).withAlpha(0.9f));
  } 
};

class ChordProContainer : public Component
{
public:
  void mouseDown (const MouseEvent& e) override;

};

class ChordProEditorContainer : public Component
{
public:
  void paint(Graphics& g) override {
    g.fillAll (backgroundColour);
    /*
    g.setFont (Font (22.00f, Font::plain).withTypefaceStyle ("Regular"));
    g.setColour (Colours::black);
    g.drawFittedText ("Editor",
      15, 0, getWidth(), HEADER_HEIGHT,
      Justification::left, 1, 1.0f);
    */
  };
  
  void setBackgroundColour(Colour newColour) {
    backgroundColour = newColour;
    repaint();
  }

private:
  Colour backgroundColour = viewPortBackground;

};

class ChordProEditor : public TextEditor,
                       public TextEditor::Listener
{
public:
  ChordProEditor() {
    addListener(this);
  }
  virtual void textEditorTextChanged(TextEditor& editor) override;

};

class SetlistContainer : public Component
{
public:
  void paint(Graphics& g) override {
    g.fillAll (Colour (0xFF1A1A1A));
  } ;

};

class SetlistHeaderContainer : public Component
{
public:
  void paint(Graphics& g) override {
     g.fillAll (Colour::fromString(BACKGROUND_COLOR));
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

class FloatingViewPort : public Viewport
{
public:
  void paint(Graphics& g) override {
    g.fillAll (Colour::fromString(BACKGROUND_COLOR));
  } ;
};

class IconButton : public ShapeButton
{
public:
IconButton() :
  juce::ShapeButton("", Colours::white, Colours::lightgrey, Colours::white )
  {
   setWantsKeyboardFocus(false);
  }
  virtual ~IconButton() { }
};

class ColourChangeButton  : public TextButton,
                            public ChangeListener,
                            private Value::Listener
{
public:
    ColourChangeButton (const String& name = String())
        : TextButton (name)
    {
        colourValue.addListener (this);
        updateColour();
    }

    Value& getColourValueObject()
    {
        return colourValue;
    }

    void clicked() override
    {
      auto  colourSelector = std::make_unique<ColourSelector>(
                    ColourSelector::showColourspace
                    | ColourSelector::showColourAtTop
                    | ColourSelector::editableColour
                    );
      colourSelector->setName("ColorSelector");
      colourSelector->setCurrentColour( findColour (TextButton::buttonColourId) );
      colourSelector->addChangeListener(this); 
      colourSelector->setColour(ColourSelector::backgroundColourId, Colours::black);
      colourSelector->setSize(200, 200);
      juce::CallOutBox::launchAsynchronously(std::move(colourSelector), getScreenBounds(),nullptr);
    }

    void changeListenerCallback (ChangeBroadcaster* source) override;
    //void focusLost (FocusChangeType cause) override;
    /*
    {
        if (ColourSelector* cs = dynamic_cast<ColourSelector*> (source))
            colourValue = cs->getCurrentColour().toString();
    }
    */

private:
    Value colourValue;

    void valueChanged (Value&) override
    {
        updateColour();
    }

    void updateColour()
    {
        setColour (TextButton::buttonColourId, Colour::fromString (colourValue.toString()));
    }
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
  virtual bool keyPressed(const KeyPress& key) override;

};

class ExtensionWindow  :  public juce::Component,
                          public juce::Button::Listener,
                          public juce::ChangeListener   
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
  void static chordProSetTranspose(int transpose);
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
  void static displayTransposeContainer(bool display);
  void static displaySearchContainer(bool display);
  void static displayPreferencesContainer(bool display);
  void static displayEditorContainer(bool display);
  void static logToGP(std::string text);
  void static songSearch(String searchCharacter, bool append);
  void static songSearchBackspace();
  void static songSearchSelect();
  void static clearSearch();
  void static displaySearchCarat(); 
  //void static exitSearch();
  //void static displaySearch(bool display);
  bool static isActiveSearch();
  void static filterButtons(String text);


  Image static getWindowIcon();
  void mouseDrag ( const MouseEvent& /*e*/) override
    {
        resized();
    }
  
  void changeListenerCallback (ChangeBroadcaster* source) override
    {
        //if (juce::ColourSelector* cs = dynamic_cast<juce::ColourSelector*> (source))
        //    colourValue = cs->getCurrentColour().toString();
      if (ColourSelector* cs = dynamic_cast <ColourSelector*> (source))
      {
          const String& name = cs->getName();
          logToGP(name.toStdString());
          
      }
    }
  
  void static chordProScrollWindow(double value);
  void static chordProUp();
  void static chordProDown();
  void static chordProProcessText(std::string text);
  void static chordProReadFile(int index);
  void static chordProScrollToSongPart(std::string text);
  void chordProDisplayGUI(bool display);
  void chordProSetColors();
  void chordProSetImageDarkMode(bool darkMode);
  //void chordProSetFontSize(float newSize);
  //void chordProImagesCheckAndAdd(int index);
  //void chordProDiagramKeyboardCheckAndAdd(int index);
  void addChordProLine(int atIndex = -1, String name = "");
  bool addChordProImage(String path);
  void addChordProDiagramKeyboard();
  void addChordProDiagramFretboard();
  int chordProGetVisibleImageCount();
  //int chordProDiagramKeysVisibleCount();
  void static chordProCreateInvertedImages();
  void static saveWindowState();
  void static savePreferences();
  void static saveChordProFile();
  void static restartWindowTimer();

  static ExtensionWindow* extension;
  MyDraggableComponent draggableResizer;
  MyDraggableComponent draggableResizerEditor;
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
  SharedResourcePointer<searchBoxLookAndFeel> searchBoxLnF;
  SharedResourcePointer<colorButtonLookAndFeel> colorButtonLnF;
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
  SharedResourcePointer<chordProEditorLookAndFeel> chordProEditorLnF;
  bool prefsLoaded = false;

 private:
  void log(String text);

  void chordProRefresh();
  void chordProReset();
  void chordProUpdateDiagramColors();
  void colorSelectorHelper(juce::StringRef name, juce::Colour currentColor, Rectangle<int> bounds);
  String static getWindowState();
  String static getDefaults();
  String static getChordProColors();
  void static setWindowState();
  Rectangle<int> static getWindowPositionAndSize();
  void static setSongPanelPosition(bool display);
  void static setSongPanelToFloating(bool isFloating);
  void updatePreferencesWindow();
  void updatePreferencesColors();


  std::unique_ptr<MyDocumentWindow> extensionWindow;
  TooltipWindow tooltipWindow;
  Viewport viewport;
  RightViewPort viewportRight;
  Component container;
  Component containerRight;
  ChordProEditorContainer containerEditor;
  PreferencesContainer preferencesContainer;
  ChordProContainer chordProContainer;
  SetlistContainer setlistContainer;
  SetlistHeaderContainer setlistHeaderContainer;
  std::unique_ptr<DrawableButton> prefToggle;
  FloatingViewPort floatingViewport;

  PopOver fontButtonContainer;
  PopOver missingImageContainer;
  PopOver transposeContainer;
  PopOver searchContainer;
  PopOver editorHeaderContainer;
  OwnedArray<TextButton> buttons;
  OwnedArray<TextButton> subButtons;
  OwnedArray<TextButton> setlistButtons;
  OwnedArray<Label> chordProLines;
  OwnedArray<ImageComponent> chordProImages;
  OwnedArray<DrawableButton> prefButtons;
  //OwnedArray<Label> prefColorLabels;
  OwnedArray<ColourChangeButton> prefColorButtons;
  //OwnedArray<ChordDiagramKeys> chordProDiagramKeyboard;
  OwnedArray<ChordDiagramKeyboard> chordProDiagramKeyboard;
  OwnedArray<ChordDiagramFretboard> chordProDiagramFretboard;
  StringPairArray buttonColors;
  StringPairArray chordProColors;
  ClockTimer clockTimer;
  RefreshTimer refreshTimer;
  CaratTimer caratTimer;
  CreateImageTimer imageTimer;
  bool displayRightPanel = true;
  bool displaySongPanel = true;
  bool displayWindowOnLoad = false;
  bool chordProForCurrentSong = false;
  bool chordProImagesOnly = false;
  bool chordProTwoColumns = false;
  //bool chordProDarkMode = false;
  bool fitHeight = false;
  bool pendingDisplayWindow = false;
  bool windowPinned = false;
  bool windowFullscreen = false;
  std::unique_ptr<int> switchImmediately;
  int prevButtonSelected = 0;
  String searchText;
  std::unique_ptr<Label> highlight;
  std::unique_ptr<Label> header;
  std::unique_ptr<Label> clock;
  std::unique_ptr<Label> fontPopOverLabel;
  std::unique_ptr<Label> missingImageLabel;
  std::unique_ptr<Label> transposeLabel;
  std::unique_ptr<Label> searchLabel;
  std::unique_ptr<Label> noSongsLabel;
  std::unique_ptr<Label> noChordProLabel;
  std::unique_ptr<Label> searchBox;
  std::unique_ptr<Label> editorLabel;
  std::unique_ptr<TextButton> btnCurrent;
  std::unique_ptr<TextButton> btnPrev;
  std::unique_ptr<TextButton> btnNext;
  std::unique_ptr<TextButton> btnModeSwitch;
  std::unique_ptr<TextButton> fontUp;
  std::unique_ptr<TextButton> fontDown;
  std::unique_ptr<TextButton> fontMono;
  std::unique_ptr<TextButton> transposeUp;
  std::unique_ptr<TextButton> transposeDown;
  std::unique_ptr<TextButton> transposeSharp;
  std::unique_ptr<TextButton> transposeFlat; 
  std::unique_ptr<TextButton> setlistButton;
  std::unique_ptr<TextButton> createInvertedImage;
  std::unique_ptr<TextButton> editorSaveButton;
  //std::unique_ptr<ColourChangeButton> testButton;
  std::unique_ptr<DynamicObject> preferences;
  std::unique_ptr<DynamicObject> preferencesChordProColors;
  std::unique_ptr<IconButton> sidePanelOpenButton;
  std::unique_ptr<IconButton> sidePanelCloseButton;
  std::unique_ptr<IconButton> pinUnpinnedButton;
  std::unique_ptr<IconButton> pinPinnedButton;
  std::unique_ptr<IconButton> fullscreenActivateButton;
  std::unique_ptr<IconButton> fullscreenDeactivateButton;
  std::unique_ptr<IconButton> fontButton;
  std::unique_ptr<IconButton> lightDarkModeButton;
  std::unique_ptr<IconButton> columnsTwoButton;
  std::unique_ptr<IconButton> columnsOneButton;
  std::unique_ptr<IconButton> fitWidthButton;
  std::unique_ptr<IconButton> fitHeightButton;
  std::unique_ptr<IconButton> preferencesCloseButton;
  std::unique_ptr<IconButton> preferencesButton;
  std::unique_ptr<IconButton> transposeButton;
  std::unique_ptr<IconButton> searchButton;
  std::unique_ptr<IconButton> editButton;
  std::unique_ptr<IconButton> editorCloseButton;
  std::unique_ptr<ChordProEditor> chordProEditor;

  Image menuIcon;
  ImageComponent menuIconComponent;


  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExtensionWindow)
};



