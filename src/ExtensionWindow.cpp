// Lyrics and Chords extension for Gig Performer by @rank13

#include <juce_events/juce_events.h>
#include "ExtensionWindow.h"
#include "LibMain.h"
#include "IconPaths.h"

ExtensionWindow* ExtensionWindow::extension = nullptr;
LibMain* lib = new LibMain(nullptr);   
Colour chordProLyricColor = Colour::fromString(CP_DARK_LYRIC_COLOR);  
Colour chordProChordColor = Colour::fromString(CP_DARK_CHORD_COLOR);
Colour viewPortBackground = Colour::fromString(BACKGROUND_COLOR);
Colour headerSongColor = Colour::fromString(HEADER_SONG_COLOR);
float chordProFontSize = CP_DEFAULT_FONT_SIZE;
int headerHeight = HEADER_HEIGHT;
int chordProTopPadding = CP_TOP_PADDING;
int chordProTranspose = 0;
FLAT_SHARP_DISPLAY chordProTransposeDisplay = original;
bool chordProMonospaceFont = false;
bool chordProSmallChordFont = false;
bool chordProLeftLabels = false;
bool chordProDarkMode = false;
bool chordProTwoColumnsExtern = false;
bool lockToSetlistMode = false;
bool autoscrollPanelPersist = false;
bool autoscrollStartWithPlayhead = false;
bool autoscrollPlaying = false;
bool displayVariationsForSong = false;
bool searchCaratOn = true;
extern std::string extensionPath;

ExtensionWindow::ExtensionWindow ()
{
    clockTimer.startTimer(TIMER_CLOCK);
    refreshTimer.startTimer(TIMER_UI_REFRESH);
    preferences.reset (new DynamicObject);
    preferencesChordProColors.reset (new DynamicObject);
    String imageBase64Off = TOGGLE_OFF;
    Image toggleOff;

    MemoryOutputStream mosOff;
    auto resultOff = Base64::convertFromBase64(mosOff, imageBase64Off);
    if (resultOff) {
        toggleOff = ImageFileFormat::loadFrom(mosOff.getData(), mosOff.getDataSize());
    }
    String imageBase64On = TOGGLE_ON;
    Image toggleOn;
    MemoryOutputStream mosOn;
    auto resultOn = Base64::convertFromBase64(mosOn, imageBase64On);
    if (resultOn) {
        toggleOn = ImageFileFormat::loadFrom(mosOn.getData(), mosOn.getDataSize());
    }
    DrawableImage on, off;
    on.setImage(toggleOn);
    off.setImage(toggleOff);

    for (int i = 0; i < 8; ++i) {
        auto button = new DrawableButton(std::to_string(i), DrawableButton::ImageFitted);
        prefButtons.add(button);
        prefButtons[i]->setImages(&off, 0, 0, 0, &on);
        prefButtons[i]->setClickingTogglesState(true);
        prefButtons[i]->setRepaintsOnMouseActivity(true); 
        prefButtons[i]->addListener (this);
        prefButtons[i]->setColour (DrawableButton::backgroundOnColourId, Colour(0x00000000));
        preferencesContainer.addAndMakeVisible(prefButtons[i]);
    }

    for (int i = 0; i < 4; ++i) {
        auto button = new ColourChangeButton (std::to_string(i));
        prefColorButtons.add(button);
        prefColorButtons[i]->setLookAndFeel(colorButtonLnF);
        prefColorButtons[i]->addListener (this);   
        preferencesContainer.addAndMakeVisible(prefColorButtons[i]);
    }

    header.reset (new Label ("header", SONG_TITLE));
    addAndMakeVisible (header.get());
    header->setEditable (false, false, false);
    header->setBounds (0, 0, getWidth(), headerHeight);
    header->setFont (Font (25.00f, Font::plain).withTypefaceStyle ("Regular"));
    header->setLookAndFeel(headerSongsLnF);
    
    highlight.reset (new Label ("subButtonHighlight", ""));
    highlight->setEditable (false, false, false);
    highlight->setLookAndFeel(highlightLnF);

    clock.reset (new Label ("clock", "00:00"));
    addAndMakeVisible (clock.get());
    clock->setFont (Font (25.00f, Font::plain).withTypefaceStyle ("Regular"));
    clock->setJustificationType (Justification::centred);
    clock->setEditable (false, false, false);
    clock->setLookAndFeel(headerLnF);
    clock->setVisible(false);
    clock->setText(Time::getCurrentTime().toString(false, true, false, true), dontSendNotification);

    Path p;
    p.loadPathFromData (sidePanelOpenPathData, sizeof (sidePanelOpenPathData));
    sidePanelOpenButton.reset (new IconButton ());
    sidePanelOpenButton->setShape (p, true, true, false);
    sidePanelOpenButton->setTooltip("Display song list");
    sidePanelOpenButton->addListener (this);
    addChildComponent (sidePanelOpenButton.get());

    Path p2;
    p2.loadPathFromData (sidePanelClosePathData, sizeof (sidePanelClosePathData));
    sidePanelCloseButton.reset (new IconButton ());
    sidePanelCloseButton->setShape (p2, true, true, false);
    sidePanelCloseButton->setTooltip("Hide song list");
    sidePanelCloseButton->addListener (this);
    addChildComponent (sidePanelCloseButton.get());

    Path p3;
    p3.loadPathFromData (pinUnpinnedPathData, sizeof (pinUnpinnedPathData));
    p3.applyTransform(juce::AffineTransform::rotation(3.14159));
    pinUnpinnedButton.reset (new IconButton ());
    pinUnpinnedButton->setShape (p3, true, true, false);
    pinUnpinnedButton->setTooltip("Keep window on top");
    pinUnpinnedButton->addListener (this);
    addChildComponent (pinUnpinnedButton.get());

    Path p4;
    p4.loadPathFromData (pinPinnedPathData, sizeof (pinPinnedPathData));
    p4.applyTransform(juce::AffineTransform::rotation(3.14159));
    pinPinnedButton.reset (new IconButton ());
    pinPinnedButton->setShape (p4, true, true, false);
    pinPinnedButton->setTooltip("Stop keeping window on top");
    pinPinnedButton->addListener (this);
    addChildComponent (pinPinnedButton.get());

    Path p5;
    p5.loadPathFromData (fullscreenActivatePathData, sizeof (fullscreenActivatePathData));
    p5.applyTransform(juce::AffineTransform::verticalFlip(0));
    fullscreenActivateButton.reset (new IconButton ());
    fullscreenActivateButton->setShape (p5, true, true, false);
    fullscreenActivateButton->setTooltip("Enter fullscreen");
    fullscreenActivateButton->addListener (this);
    addChildComponent (fullscreenActivateButton.get());

    Path p6;
    p6.loadPathFromData (fullscreenDeactivatePathData, sizeof (fullscreenDeactivatePathData));
    p6.applyTransform(juce::AffineTransform::verticalFlip(0));
    fullscreenDeactivateButton.reset (new IconButton ());
    fullscreenDeactivateButton->setShape (p6, true, true, false);
    fullscreenDeactivateButton->setTooltip("Exit fullscreen");
    fullscreenDeactivateButton->addListener (this);
    addChildComponent (fullscreenDeactivateButton.get());

    Path p7;
    p7.loadPathFromData (lightDarkModePathData, sizeof (lightDarkModePathData));
    p7.applyTransform(juce::AffineTransform::verticalFlip(0));
    lightDarkModeButton.reset (new IconButton ());
    lightDarkModeButton->setShape (p7, true, true, false);
    lightDarkModeButton->setTooltip("Light/dark mode");
    lightDarkModeButton->addListener (this);
    addChildComponent (lightDarkModeButton.get());

    Path p8;
    p8.loadPathFromData (fontPathData, sizeof (fontPathData));
    p8.applyTransform(juce::AffineTransform::verticalFlip(0));
    fontButton.reset (new IconButton ());
    fontButton->setShape (p8, true, true, false);
    fontButton->setTooltip("Show/hide font settings");
    fontButton->addListener (this);
    addChildComponent (fontButton.get());

    Path p9;
    p9.loadPathFromData (columnsTwoPathData, sizeof (columnsTwoPathData));
    p9.applyTransform(juce::AffineTransform::verticalFlip(0));
    columnsTwoButton.reset (new IconButton ());
    columnsTwoButton->setShape (p9, true, true, false);
    columnsTwoButton->setTooltip("Display two pages");
    columnsTwoButton->addListener (this);
    addChildComponent (columnsTwoButton.get());

    Path p10;
    p10.loadPathFromData (columnsOnePathData, sizeof (columnsOnePathData));
    p10.applyTransform(juce::AffineTransform::verticalFlip(0));
    columnsOneButton.reset (new IconButton ());
    columnsOneButton->setShape (p10, true, true, false);
    columnsOneButton->setTooltip("Display single page");
    columnsOneButton->addListener (this);
    addChildComponent (columnsOneButton.get());

    Path p11;
    p11.loadPathFromData (fitWidthPathData, sizeof (fitWidthPathData));
    fitWidthButton.reset (new IconButton ());
    fitWidthButton->setShape (p11, true, true, false);
    fitWidthButton->setTooltip("Fit to window width");
    fitWidthButton->addListener (this);
    addChildComponent (fitWidthButton.get());

    Path p12;
    p12.loadPathFromData (fitHeightPathData, sizeof (fitHeightPathData));
    fitHeightButton.reset (new IconButton ());
    fitHeightButton->setShape (p12, true, true, false);
    fitHeightButton->setTooltip("Fit to window height");
    fitHeightButton->addListener (this);
    addChildComponent (fitHeightButton.get());

    Path p13;
    p13.loadPathFromData (closePathData, sizeof (closePathData));
    preferencesCloseButton.reset (new IconButton ()); 
    preferencesCloseButton->setShape (p13, true, true, false);
    preferencesCloseButton->setTooltip("Close preferences");
    preferencesCloseButton->addListener (this);
    preferencesContainer.addAndMakeVisible (preferencesCloseButton.get());

    Path p14;
    p14.loadPathFromData (preferencesPathData, sizeof (preferencesPathData));
    preferencesButton.reset (new IconButton ());
    preferencesButton->setShape (p14, true, true, false);
    preferencesButton->setTooltip("Preferences");
    preferencesButton->addListener (this);
    addChildComponent (preferencesButton.get());

    Path p15;
    p15.loadPathFromData (transposePathData, sizeof (transposePathData));
    p15.applyTransform(juce::AffineTransform::verticalFlip(0));
    transposeButton.reset (new IconButton ());
    transposeButton->setShape (p15, true, true, false);
    transposeButton->setTooltip("Show/hide transpose settings");
    transposeButton->addListener (this);
    addChildComponent (transposeButton.get());

    Path p16;
    p16.loadPathFromData (searchPathData, sizeof (searchPathData));
    p16.applyTransform(juce::AffineTransform::verticalFlip(0));
    searchButton.reset (new IconButton ());
    searchButton->setShape (p16, true, true, false);
    searchButton->setTooltip("Song search");
    searchButton->addListener (this);
    addChildComponent (searchButton.get());

    Path p17;
    p17.loadPathFromData (editPathData, sizeof (editPathData));
    p17.applyTransform(juce::AffineTransform::verticalFlip(0));
    editButton.reset (new IconButton ());
    editButton->setShape (p17, true, true, false);
    editButton->setTooltip("Edit file");
    editButton->addListener (this);
    addChildComponent (editButton.get());

    Path p18;
    p18.loadPathFromData (closePathData, sizeof (closePathData));
    editorCloseButton.reset (new IconButton ()); 
    editorCloseButton->setShape (p18, true, true, false);
    editorCloseButton->setColours(Colours::black, Colours::darkgrey, Colours::grey);
    editorCloseButton->setTooltip("Close editor");
    editorCloseButton->addListener (this);
    editorHeaderContainer.addAndMakeVisible (editorCloseButton.get());

    Path p19;
    p19.loadPathFromData (autoscrollPathData, sizeof (autoscrollPathData));
    p19.applyTransform(juce::AffineTransform::verticalFlip(0));
    autoscrollButton.reset (new IconButton ()); 
    autoscrollButton->setShape (p19, true, true, false);
    autoscrollButton->setTooltip("Show/hide autoscroll settings");
    autoscrollButton->addListener (this);
    addChildComponent (autoscrollButton.get());

    Path p20;
    p20.loadPathFromData (autoscrollResetPathData, sizeof (autoscrollResetPathData));
    p20.applyTransform(juce::AffineTransform::verticalFlip(0));
    autoscrollResetButtonIcon.reset (new IconButton ()); 
    autoscrollResetButtonIcon->setShape (p20, true, true, false);
    autoscrollResetButtonIcon->setTooltip("Scroll to top");
    autoscrollResetButtonIcon->addListener (this);
    autoscrollResetButtonIcon->setInterceptsMouseClicks(false, true);

    setlistButton.reset (new TextButton ("All Songs"));
    setlistButton->setLookAndFeel(setlistButtonLnF);
    setlistButton->setButtonText ("All Songs");
    setlistButton->addListener (this);   
    setlistButton->setWantsKeyboardFocus(false);

    fontUp.reset (new TextButton ("fontUp"));
    fontUp->setLookAndFeel(popOverLnf);
    fontUp->setButtonText ("+");
    fontUp->addListener (this);   

    fontDown.reset (new TextButton ("fontDown"));
    fontDown->setLookAndFeel(popOverLnf);
    fontDown->setButtonText (juce::String::charToString(0x2013));
    fontDown->addListener (this);   

    fontMono.reset (new TextButton ("Mono"));
    fontMono->setLookAndFeel(popOverLnf);
    fontMono->setButtonText ("Monospace");
    fontMono->setClickingTogglesState(true);
    fontMono->addListener (this);  

    fontPopOverLabel.reset (new Label ("Font","Font"));
    fontPopOverLabel->setLookAndFeel(popOverLabelLnf);

    fontButtonContainer.addAndMakeVisible(fontUp.get());
    fontButtonContainer.addAndMakeVisible(fontDown.get());
    fontButtonContainer.addAndMakeVisible(fontMono.get());
    fontButtonContainer.addAndMakeVisible(fontPopOverLabel.get());

    transposeUp.reset (new TextButton ("transposeUp"));
    transposeUp->setLookAndFeel(popOverLnf);
    transposeUp->setButtonText ("+");
    transposeUp->addListener (this);   

    transposeDown.reset (new TextButton ("transposeDown"));
    transposeDown->setLookAndFeel(popOverLnf);
    transposeDown->setButtonText (juce::String::charToString(0x2013));
    transposeDown->addListener (this);   

    transposeSharp.reset (new TextButton ("transposeSharp"));
    transposeSharp->setLookAndFeel(popOverLnf);
    transposeSharp->setButtonText ("#");
    transposeSharp->setTooltip("Convert flats to sharps");
    transposeSharp->addListener (this);   

    transposeFlat.reset (new TextButton ("transposeFlat"));
    transposeFlat->setLookAndFeel(popOverLnf);
    transposeFlat->setButtonText (String::charToString(0x266D));
    transposeFlat->setTooltip("Convert sharps to flats");
    transposeFlat->addListener (this);   

    transposeLabel.reset (new Label ("Transpose","Transpose  0"));
    transposeLabel->setLookAndFeel(popOverLabelLnf);

    autoscrollPlayButton.reset (new TextButton ("autoscrollPlay"));
    autoscrollPlayButton->setLookAndFeel(popOverLnf);
    autoscrollPlayButton->setButtonText (">");
    autoscrollPlayButton->setClickingTogglesState(true);
    autoscrollPlayButton->addListener (this);  

    autoscrollResetButton.reset (new TextButton ("autoscrollReset"));
    autoscrollResetButton->setLookAndFeel(popOverLnf);
    autoscrollResetButton->setButtonText (" ");
    autoscrollResetButton->addListener (this); 

    autoscrollLabel.reset (new Label ("Autoscroll","Autoscroll"));
    autoscrollLabel->setLookAndFeel(popOverLabelLnf);
    autoscrollDurationLabel.reset (new Label ("AutoscrollDuration","00:00"));
    autoscrollDurationLabel->setLookAndFeel(popOverLabelLnf);
    autoscrollTimeLabel.reset (new Label ("AutoscrollTime","00:00"));
    autoscrollTimeLabel->setLookAndFeel(popOverLabelLnf);
    autoscrollTimeSeparatorLabel.reset (new Label ("AutoscrollTimeSeparator","/"));
    autoscrollTimeSeparatorLabel->setLookAndFeel(popOverLabelLnf);

    editorSaveButton.reset (new TextButton ("Save"));
    editorSaveButton->setLookAndFeel(popOverLnf);
    editorSaveButton->setButtonText ("Save");
    editorSaveButton->setClickingTogglesState(false);
    editorSaveButton->addListener (this);  

    editorLabel.reset (new Label ("Editor","Editor"));
    editorLabel->setLookAndFeel(popOverLabelLnf);

    editorHeaderContainer.addAndMakeVisible(editorLabel.get());
    editorHeaderContainer.addAndMakeVisible(editorSaveButton.get());

    transposeContainer.addAndMakeVisible(transposeUp.get());
    transposeContainer.addAndMakeVisible(transposeDown.get());
    transposeContainer.addAndMakeVisible(transposeSharp.get());
    transposeContainer.addAndMakeVisible(transposeFlat.get());
    transposeContainer.addAndMakeVisible(transposeLabel.get());

    autoscrollContainer.addAndMakeVisible(autoscrollLabel.get());
    autoscrollContainer.addAndMakeVisible(autoscrollPlayButton.get());
    autoscrollContainer.addAndMakeVisible(autoscrollResetButton.get());   
    autoscrollContainer.addAndMakeVisible(autoscrollResetButtonIcon.get());
    autoscrollContainer.addAndMakeVisible(autoscrollTimeLabel.get());
    autoscrollContainer.addAndMakeVisible(autoscrollTimeSeparatorLabel.get());
    autoscrollContainer.addAndMakeVisible(autoscrollDurationLabel.get());

    createInvertedImage.reset (new TextButton ("Create"));
    createInvertedImage->setLookAndFeel(popOverLnf);
    createInvertedImage->setButtonText ("Create");
    createInvertedImage->addListener (this);  

    missingImageLabel.reset (new Label ("missingImageLabel",CP_NO_DARK_MODE_IMAGES));
    missingImageLabel->setLookAndFeel(popOverLabelLnf);

    missingImageContainer.addAndMakeVisible(createInvertedImage.get());
    missingImageContainer.addAndMakeVisible(missingImageLabel.get());

    searchLabel.reset (new Label ("Search","Search"));
    searchLabel->setLookAndFeel(popOverLabelLnf);

    searchBox.reset (new Label ("searchBox", ""));
    searchBox->setEditable (false, false, false);
    searchBox->setLookAndFeel(searchBoxLnF);

    searchContainer.addAndMakeVisible(searchLabel.get());
    searchContainer.addAndMakeVisible(searchBox.get());

    // Song buttons
    for (int i = 0; i < DEFAULT_RACKSPACES_SONGS; ++i) {
        std::string number = std::to_string(i);
        std::string name = std::string(2 - number.length(), '0') + std::to_string(i); // Pad with leading zero
        auto button = new TextButton(name); 
        buttons.add(button);
        buttons[i]->setLookAndFeel(buttonsLnF);
        buttons[i]->setClickingTogglesState(true);
        buttons[i]->setWantsKeyboardFocus(false);
        buttons[i]->setRadioGroupId(1);
        buttons[i]->getProperties().set("index", i);
        buttons[i]->getProperties().set("displayIndex", i + 1); // Default to non zero-based
        buttons[i]->getProperties().set("type", "button"); 
        buttons[i]->setTriggeredOnMouseDown(true);
        buttons[i]->addListener(this);  
    }
    buttons[0]->setToggleState (true, dontSendNotification);

    // Song part buttons
    for (int i = 0; i < DEFAULT_VARIATIONS_SONGPARTS; ++i) {
        std::string number = std::to_string(i);
        auto button = new TextButton(number); 
        subButtons.add(button);
        subButtons[i]->setLookAndFeel(subButtonsLnF);
        subButtons[i]->setClickingTogglesState(true);
        subButtons[i]->setWantsKeyboardFocus(false);
        subButtons[i]->setRadioGroupId(2);
        subButtons[i]->getProperties().set("index", i);
        subButtons[i]->getProperties().set("displayIndex", i + 1); // Default to non zero-based
        subButtons[i]->getProperties().set("type", "subButton"); 
        subButtons[i]->setTriggeredOnMouseDown(true);
        subButtons[i]->addListener(this);  
    }
    subButtons[0]->setToggleState (true, dontSendNotification);
    container.addAndMakeVisible(highlight.get());

    for (int i = 0; i < buttons.size(); ++i) {
        container.addAndMakeVisible(buttons[i]);
        buttons[i]->setVisible(false);
    }

    for (int i = 0; i < subButtons.size(); ++i) {
        container.addAndMakeVisible(subButtons[i]);
        subButtons[i]->setVisible(false);
    }
    
    // Setlists
    for (int i = 0; i < DEFAULT_SETLISTS; ++i) {
        std::string number = std::to_string(i);
        auto button = new TextButton(number); 
        setlistButtons.add(button);
        setlistButtons[i]->setLookAndFeel(setlistListButtonLnF);
        setlistButtons[i]->setClickingTogglesState(true);
        setlistButtons[i]->setRadioGroupId(3);
        setlistButtons[i]->setTriggeredOnMouseDown(true);
        setlistButtons[i]->getProperties().set("type", "setlistButton"); 
        setlistButtons[i]->addListener(this);  
        setlistContainer.addChildComponent(setlistButtons[i]);
    }

    noSongsLabel.reset (new Label ("noSongs", "No Songs"));
    noSongsLabel->setEditable (false, false, false);
    noSongsLabel->setBounds (0, 0, getWidth(), headerHeight);
    noSongsLabel->setLookAndFeel(noSongLabelLnF);

    noChordProLabel.reset (new Label ("noSongs", NO_CHORDPRO_MESSAGE));
    noChordProLabel->setEditable (false, false, false);
    noChordProLabel->setBounds (0, 0, getWidth(), headerHeight);
    noChordProLabel->setLookAndFeel(noChordProLabelLnF);

    draggableResizer.addMouseListener(this, true);
    draggableResizer.setMouseCursor(MouseCursor::LeftRightResizeCursor);
    viewport.setViewedComponent(&container, false);
    viewportRight.setViewedComponent(&containerRight, false);
    viewport.getVerticalScrollBar().setColour(ScrollBar::thumbColourId, Colour::fromString(BACKGROUND_COLOR));
    viewportRight.getVerticalScrollBar().setColour(ScrollBar::thumbColourId, Colour::fromString(BACKGROUND_COLOR));
    viewport.setWantsKeyboardFocus(false);
    viewportRight.setWantsKeyboardFocus(false);
    floatingViewport.getVerticalScrollBar().setColour(ScrollBar::thumbColourId, Colour::fromString(BACKGROUND_COLOR));

    String imageBase64 = MENU_ICON;
    MemoryOutputStream mo;
    auto result = Base64::convertFromBase64(mo, imageBase64);
    if (result) {
        menuIcon = ImageFileFormat::loadFrom(mo.getData(), mo.getDataSize());
    }
    menuIconComponent.setImage(menuIcon);
    menuIconComponent.setSize(16,16);
    menuIconComponent.setInterceptsMouseClicks(false,false);
    
    setlistHeaderContainer.addAndMakeVisible(setlistButton.get());
    setlistHeaderContainer.addAndMakeVisible(menuIconComponent);
    addChildComponent(noSongsLabel.get());

    containerRight.addChildComponent(noChordProLabel.get());
    containerEditor.addAndMakeVisible(editorHeaderContainer);
    container.setWantsKeyboardFocus(false);

    addAndMakeVisible(viewport);
    addAndMakeVisible(viewportRight);
    addChildComponent(floatingViewport);

    addAndMakeVisible(setlistHeaderContainer);
    addChildComponent(fontButtonContainer);
    addChildComponent(missingImageContainer);
    addChildComponent(transposeContainer);
    addChildComponent(searchContainer);
    addChildComponent(autoscrollContainer);

    chordProEditor.reset(new ChordProEditor());
    containerEditor.addAndMakeVisible(chordProEditor.get());
    Font editorFont = Font(18.0f, Font::plain);
    editorFont.setTypefaceName(Font::getDefaultMonospacedFontName());
    chordProEditor->setFont(editorFont);
    chordProEditor->setLookAndFeel(chordProEditorLnF);
    chordProEditor->setColour(TextEditor::backgroundColourId, Colour(0x1AD5D5D5));
    chordProEditor->setColour(TextEditor::outlineColourId, Colours::transparentWhite);
    chordProEditor->setColour(TextEditor::focusedOutlineColourId, Colours::transparentWhite);
    chordProEditor->setColour(TextEditor::textColourId, Colours::black);
    chordProEditor->setColour(TextEditor::highlightColourId, Colours::grey);
    chordProEditor->getLookAndFeel().setColour(ScrollBar::thumbColourId, Colour::fromString(BACKGROUND_COLOR));
    chordProEditor->getLookAndFeel().setColour(PopupMenu::backgroundColourId, Colour::fromString(BACKGROUND_COLOR));
    chordProEditor->getLookAndFeel().setColour(PopupMenu::highlightedBackgroundColourId, Colours::black);
    chordProEditor->setIndents( 20, 20);
    chordProEditor->setMultiLine(true, false);
    chordProEditor->setReturnKeyStartsNewLine(true);
    chordProEditor->setReadOnly(false);
    chordProEditor->setScrollbarsShown(true);
    chordProEditor->setCaretVisible(true);
    chordProEditor->setPopupMenuEnabled(true);
    addChildComponent(containerEditor);
    draggableResizerEditor.addMouseListener(this, true);
    draggableResizerEditor.setMouseCursor(MouseCursor::LeftRightResizeCursor);
    addChildComponent(draggableResizerEditor);
    addChildComponent(preferencesContainer);
    addAndMakeVisible(draggableResizer);

    extensionWindow.reset(new MyDocumentWindow());
    extensionWindow->setContentNonOwned(this, true);
    extensionWindow->setResizable(true, true);
    extensionWindow->setUsingNativeTitleBar(true);

    setSize (Rectangle<int>::fromString(DEFAULT_WINDOW_POSITION).getWidth(), 
             Rectangle<int>::fromString(DEFAULT_WINDOW_POSITION).getHeight()
            );
            
    #if JUCE_MAC
        extensionWindow->setResizeLimits(180, 250, 10000, 10000);
    #else
        extensionWindow->getPeer()->setIcon(getWindowIcon());
        extensionWindow->setResizeLimits(200, 250, 10000, 10000);
    #endif
}

ExtensionWindow::~ExtensionWindow()
{
    clockTimer.stopTimer();
    refreshTimer.stopTimer();
}

void ExtensionWindow::paint (Graphics& g)
{
    g.fillAll (Colour::fromString(BACKGROUND_COLOR));
    g.drawImageAt(menuIcon, 5,5);
}

void ExtensionWindow::setWindowPositionAndSize(int x, int y, int w, int h) {
    extension->extensionWindow->setTopLeftPosition (x, y);
    extension->extensionWindow->setSize(w, h);
    extension->resized();
}

Rectangle<int> ExtensionWindow::getWindowPositionAndSize() {
    return extension->extensionWindow->getBounds();
}

Image ExtensionWindow::getWindowIcon() {
    Image img;
    String imageBase64 = WINDOW_ICON;
    MemoryOutputStream mo;
    auto result = Base64::convertFromBase64(mo, imageBase64);
    if (result) {
        img = ImageFileFormat::loadFrom(mo.getData(), mo.getDataSize());
    }
    return img;
}

void ExtensionWindow::resized()
{
    int minWindowWidth = displayRightPanel ? 0 : 180;
    int maxSongListWidth = 400;
    int minButtonHeight = 50;
    int largeScrollAreaWidth = 50;
    Point<int> viewPos = viewport.getViewPosition();
    Point<int> viewRightPos = viewportRight.getViewPosition();
    Point<int> viewFloatingPos = floatingViewport.getViewPosition();
    int columns = 1;
    int buttonHeightRatio = 5; // Ratio of width:height
    auto bounds = container.getBounds();
    bool largeScrollArea = preferences->getProperty("LargeScrollArea");
    auto buttonSize = largeScrollArea ? bounds.getWidth() - largeScrollAreaWidth 
                                      : bounds.getWidth();
    int buttonHeight = ((int)(buttonSize/buttonHeightRatio) < minButtonHeight) ? minButtonHeight 
                                                                               : (int)(buttonSize / buttonHeightRatio);
    float padding = buttonHeight * 0.1;
    float rowHeight = 0.0;
    auto x = displayRightPanel ? draggableResizer.getX() 
                               : getWidth();
    if (x > maxSongListWidth) x = maxSongListWidth;
    auto editorX = draggableResizerEditor.getX();
    int editorWidth = containerEditor.getWidth();

    extensionWindow->setResizeLimits(juce::jmax(x, 250), 250, 10000, 10000);
    
    // The increased width of the pane separator/resizer is to help with touchscreens. The actual displayed width is overridden in the Paint method of MyDraggableComponent.
    draggableResizer.setBounds (juce::jmax(minWindowWidth, x), headerHeight, PANE_SEPARATOR_WIDTH, getHeight()); 
    if (draggableResizerEditor.isDragging()) {
        draggableResizerEditor.setBounds (juce::jmin(juce::jmax(x + 250, editorX), getWidth() - 80), headerHeight, PANE_SEPARATOR_WIDTH, getHeight()); 
    }

    int buttonDisplayCount = 0;
    for (int i = 0; i < buttons.size(); ++i) {
        if (buttons[i]->isVisible()) ++buttonDisplayCount;
    }

    int subButtonDisplayCount = 0;
    for (int i = 0; i < subButtons.size(); ++i) {
        if (subButtons[i]->isVisible()) ++subButtonDisplayCount;
    }

    // Don't display if only 1 sub button (except if not switching immediately)
    bool switchRackSongImmediately = true;
    if (subButtonDisplayCount <= 1 && switchRackSongImmediately) {
        subButtonDisplayCount = 0;
        subButtons[0]->setVisible(false);
        highlight->setVisible(false);
    } else {
        highlight->setVisible(true);
    }

    // Calculate how many rows to display (based on column count)
    auto rows = (buttonDisplayCount + subButtonDisplayCount) / columns;
    rows = rows + 0.5 - (rows < 0); 
    int rowCount = (int)rows;

    String headerLabel = header->getText();
    Font headerLabelFont = header->getFont();
    int headerLabelWidth = headerLabelFont.getStringWidth(headerLabel);
    header->setBounds (0, 0, getWidth(), headerHeight);
    clock->setBounds (getWidth()/2 - 50, 0, 100, headerHeight);
    clock->setVisible(((getWidth() > 860 && chordProForCurrentSong && !chordProImagesOnly)
        || (getWidth() > 730 && chordProForCurrentSong && chordProImagesOnly)
        || (getWidth() > 420 &&!chordProForCurrentSong)) 
        && clock->getX() > headerLabelWidth ? true : false);
    if (chordProImagesOnly && getWidth() < 560) clock->setVisible(false);
    if (fontButtonContainer.getX() < x + 5) fontButtonContainer.setVisible(false);
    if (missingImageContainer.getX() < x + 5) missingImageContainer.setVisible(false);
    if (transposeContainer.getX() < x + 5) transposeContainer.setVisible(false);
    if (searchContainer.getX() < x + 5) searchContainer.setVisible(false);
    if (autoscrollContainer.getX() < x + 5) autoscrollContainer.setVisible(false);

    int iconX = header->getBounds().getWidth() - 60;
    int iconY = header->getBounds().getY();
    int iconW = 60;
    int iconH = header->getHeight();
    auto iconBounds = Rectangle(iconX, iconY, iconW, iconH);

    preferencesButton->setVisible(preferencesButton->getX() > headerLabelWidth + 30);
    preferencesButton->setBounds(iconBounds.withSizeKeepingCentre(22,22));
    
    iconBounds = iconBounds.withX(iconBounds.getX() - 35);
    searchButton->setVisible(searchButton->getX() > headerLabelWidth + 20);
    searchButton->setBounds(iconBounds.withSizeKeepingCentre(22,22));
    iconBounds = iconBounds.withX(iconBounds.getX() - 30);

    pinUnpinnedButton->setVisible(!windowPinned && pinUnpinnedButton->getX() > headerLabelWidth + 20);
    pinPinnedButton->setVisible(windowPinned && pinPinnedButton->getX() > headerLabelWidth + 20);
    pinUnpinnedButton->setBounds(iconBounds.withSizeKeepingCentre(20,20));
    pinPinnedButton->setBounds(iconBounds.withY(5).withSizeKeepingCentre(10,10));
    
    iconBounds = iconBounds.withX(iconBounds.getX() - 33);
    fullscreenActivateButton->setVisible(!windowFullscreen && fullscreenActivateButton->getX() > headerLabelWidth + 30);
    fullscreenDeactivateButton->setVisible(windowFullscreen && fullscreenDeactivateButton->getX() > headerLabelWidth + 30);
    fullscreenActivateButton->setBounds(iconBounds.withSizeKeepingCentre(20,20));
    fullscreenDeactivateButton->setBounds(iconBounds.withSizeKeepingCentre(20,20));
            
    if (chordProForCurrentSong) {
        iconBounds = iconBounds.withX(iconBounds.getX() - 35);
        auto iconBoundsAdjusted = iconBounds.withSizeKeepingCentre(25,25);
        editButton->setBounds(iconBoundsAdjusted.withY(iconBoundsAdjusted.getY() - 1));
        editButton->setVisible(chordProForCurrentSong && editButton->getX() > headerLabelWidth + 20);

        iconBounds = iconBounds.withX(iconBounds.getX() - 39);
        autoscrollButton->setBounds (iconBounds.withSizeKeepingCentre(22,22));
        autoscrollButton->setVisible(autoscrollButton->getX() > headerLabelWidth + 20);
  
        iconBounds = iconBounds.withX(iconBounds.getX() - 38);
        lightDarkModeButton->setBounds(iconBounds.withSizeKeepingCentre(22,22));
        lightDarkModeButton->setVisible(chordProForCurrentSong && lightDarkModeButton->getX() > headerLabelWidth + 20);
    }
    if (!chordProImagesOnly && chordProForCurrentSong) {
        iconBounds = iconBounds.withX(iconBounds.getX() - 35);
        transposeButton->setBounds (iconBounds.withSizeKeepingCentre(22,22));
        transposeButton->setVisible(transposeButton->getX() > headerLabelWidth + 20);
    }
    if (!chordProImagesOnly && chordProForCurrentSong) {
        iconBounds = iconBounds.withX(iconBounds.getX() - 35);
        fontButton->setBounds (iconBounds.withSizeKeepingCentre(27,27));
        fontButton->setVisible(fontButton->getX() > headerLabelWidth + 20);
    }

    if (chordProForCurrentSong) {
        iconBounds = iconBounds.withX(iconBounds.getX() - 40);
        columnsOneButton->setBounds (iconBounds.withSizeKeepingCentre(22,22));
        columnsTwoButton->setBounds (iconBounds.withSizeKeepingCentre(22,22));
        columnsOneButton->setVisible(chordProTwoColumns && columnsOneButton->getX() > headerLabelWidth + 20);
        columnsTwoButton->setVisible(!chordProTwoColumns && columnsTwoButton->getX() > headerLabelWidth + 20);
    }

    if (chordProImagesOnly && chordProForCurrentSong) {
        iconBounds = iconBounds.withX(iconBounds.getX() - 40);
        fitWidthButton->setBounds(iconBounds.withSizeKeepingCentre(22,22));
        fitHeightButton->setBounds(iconBounds.withSizeKeepingCentre(22,22));
        fitWidthButton->setVisible(fitHeight && chordProImagesOnly && fitWidthButton->getX() > headerLabelWidth + 20);
        fitHeightButton->setVisible(!fitHeight && chordProImagesOnly && fitHeightButton->getX() > headerLabelWidth + 20);
    }

    iconBounds = iconBounds.withX(iconBounds.getX() - 38);
    sidePanelCloseButton->setVisible(displaySongPanel && sidePanelCloseButton->getX() > headerLabelWidth + 30);
    sidePanelOpenButton->setVisible(!displaySongPanel && sidePanelOpenButton->getX() > headerLabelWidth + 30);
    sidePanelOpenButton->setBounds (iconBounds.withSizeKeepingCentre(22,22));
    sidePanelCloseButton->setBounds(iconBounds.withSizeKeepingCentre(22,22));

    int scrollbarBuffer = 2;
    int selectedButton = 999;
    int displayIndex = 0;
    for (int i = 0; i < buttons.size(); ++i) {
        if (buttons[i]->isVisible()) {
            buttons[i]->setBounds (buttonSize * (displayIndex % columns) + padding,
                                        buttonHeight * (displayIndex / columns) + padding + (displayIndex > selectedButton ? buttonHeight * subButtonDisplayCount : 0),
                                        buttonSize - padding - scrollbarBuffer,
                                        buttonHeight - padding);
            if (buttons[i]->getToggleState()) {  // Display sub buttons
                selectedButton = displayIndex;
                for (int j = 0; j < subButtonDisplayCount; ++j) {
                subButtons[j]->setBounds (buttonSize * (j % columns) + padding,
                                        buttonHeight * ((j+1) / columns) + (buttonHeight * (displayIndex / columns) + padding),
                                        buttonSize - padding - scrollbarBuffer,
                                        buttonHeight - padding);
                }
                highlight->setBounds (buttonSize * (displayIndex % columns) + padding,
                                        buttonHeight + (buttonHeight * (displayIndex / columns) + padding),
                                        buttonSize - padding - scrollbarBuffer,
                                        buttonHeight * subButtonDisplayCount - padding
                );
                highlight->getProperties().set("buttonHeight", buttonHeight - padding);
            }
            displayIndex++;
        }
    }

    container.setBounds(0, headerHeight, juce::jmax (minWindowWidth-10, x - 10), headerHeight + (buttonHeight * rowCount) + padding);
    if (searchContainer.isVisible() && !displaySongPanel) container.setBounds(0, 50, DEFAULT_SONG_LIST_WIDTH - 10, headerHeight + (buttonHeight * rowCount) + padding);
    containerRight.setBounds(juce::jmax (minWindowWidth-10, x - 10), headerHeight, getWidth()- juce::jmax (minWindowWidth, x), getHeight() - headerHeight);
    
    if (draggableResizerEditor.isDragging()) {
        containerEditor.setBounds(juce::jmin(juce::jmax (x + 250, editorX), getWidth() - 80), headerHeight, juce::jmax(getWidth() - juce::jmax (x + 250, editorX), 80), getHeight() - headerHeight);
    } else {
        containerEditor.setBounds(juce::jmax(x + 250, getWidth() - editorWidth), headerHeight, juce::jmax(getWidth() - juce::jmax(x + 250, getWidth() - editorWidth), 80), getHeight() - headerHeight); 
        draggableResizerEditor.setBounds (juce::jmax(x + 250, containerEditor.getX()), headerHeight, PANE_SEPARATOR_WIDTH, getHeight()); 
    }
    editorHeaderContainer.setBounds(0, 0, containerEditor.getWidth(), headerHeight);
    chordProEditor->setBounds(0, headerHeight, containerEditor.getWidth(), containerEditor.getHeight() - headerHeight);

    int popOverRightX = containerEditor.isVisible() ? containerEditor.getX() : getWidth();
    int rightPad = containerEditor.isVisible() ? 0 : 10;
    fontButtonContainer.setBounds(popOverRightX - rightPad - 330, headerHeight, 330, headerHeight);
    missingImageContainer.setBounds(popOverRightX - rightPad - 360, headerHeight, 360, headerHeight);
    transposeContainer.setBounds(popOverRightX - rightPad - 400, headerHeight, 400, headerHeight);
    searchContainer.setBounds(popOverRightX - rightPad - 420, headerHeight, 420, headerHeight);
    autoscrollContainer.setBounds(popOverRightX - rightPad - 380, headerHeight, 380, headerHeight);

    searchBox->setBounds(100, 0, 310, headerHeight);
    noSongsLabel->setBounds(0, headerHeight * 4, x, 100);
    noChordProLabel->setBounds(0, headerHeight * 4, getWidth() - x, 100);
    
    viewport.setBounds(0, headerHeight * 2, juce::jmax (minWindowWidth, x), getHeight() - headerHeight);
    viewport.setViewPosition(viewPos);
    viewportRight.setBounds(juce::jmax (minWindowWidth, x), headerHeight, getWidth() - juce::jmax (minWindowWidth, x), getHeight() - headerHeight);
    floatingViewport.setVisible(searchContainer.isVisible() && !displaySongPanel);
    floatingViewport.setBounds(0, headerHeight * 2, DEFAULT_SONG_LIST_WIDTH, getHeight() - headerHeight);
    floatingViewport.setViewPosition(viewFloatingPos);

    fontPopOverLabel->setBounds (0, 0, 70, headerHeight);
    fontDown->setBounds (70,5,50,headerHeight-10);
    fontUp->setBounds (130,5,50,headerHeight-10);
    fontMono->setBounds (190,5,130,headerHeight-10);

    transposeLabel->setBounds (0, 0, 160, headerHeight);
    transposeDown->setBounds (160,5,50,headerHeight-10);
    transposeUp->setBounds (220,5,50,headerHeight-10);
    transposeFlat->setBounds (280,5,50,headerHeight-10);
    transposeSharp->setBounds (340,5,50,headerHeight-10);

    autoscrollLabel->setBounds (0, 0, 120, headerHeight);
    autoscrollPlayButton->setBounds (120,5,50,headerHeight-10);
    autoscrollResetButton->setBounds (180,5,50,headerHeight-10);
    autoscrollResetButtonIcon->setBounds(autoscrollResetButton->getBounds().withSizeKeepingCentre(22,22));
    autoscrollTimeLabel->setBounds (225, 0, 85, headerHeight);
    autoscrollTimeSeparatorLabel->setBounds (285, 0, 30, headerHeight);
    autoscrollDurationLabel->setBounds (300, 0, 70, headerHeight);

    missingImageLabel->setBounds (0, 0, 260, headerHeight);
    createInvertedImage->setBounds (260, 5, 90, headerHeight-10);

    searchLabel->setBounds (0, 0, 100, headerHeight);

    editorLabel->setBounds(0, 0, 80, headerHeight);
    editorSaveButton->setBounds(80, 5, 70, headerHeight - 10);
    editorSaveButton->setVisible(editorTextEdited);
    editorCloseButton->setBounds(containerEditor.getWidth() - 40, 16, 17, 17);
    editorCloseButton->setVisible(editorCloseButton->getX() > 150);

    preferencesContainer.setBounds(x, headerHeight, getWidth() - x, getHeight());
    preferencesCloseButton->setVisible(preferencesCloseButton->getX() > 250);
    preferencesCloseButton->setBounds(getWidth() - x - 70, 20, 30, 30 );

    for (int i = 0; i < 4; ++i) {
        prefButtons[i]->setBounds(50, 148 + 35 * i, 40, 40);
    }
    prefButtons[4]->setBounds(50, 353, 40, 40);
    prefButtons[5]->setBounds(50, 388, 40, 40);
    prefButtons[6]->setBounds(50, 423, 40, 40);
    prefButtons[7]->setBounds(50, 458, 40, 40);

    for (int i = 0; i < 4; ++i) {
        prefColorButtons[i]->setBounds(50, 510 + 35 * i, 80, 30);
    }

    int setlistCount = 0;
    int setListWidth = (searchContainer.isVisible() && !displaySongPanel) ? DEFAULT_SONG_LIST_WIDTH : x;
    
    setlistHeaderContainer.setBounds(0, 50, setListWidth, headerHeight);
    setlistButton->setBounds(0, 0, juce::jmax (minWindowWidth, x), headerHeight);
    if (searchContainer.isVisible() && !displaySongPanel) 
        setlistButton->setBounds(0, 0, DEFAULT_SONG_LIST_WIDTH, headerHeight);

    menuIconComponent.setBounds(setListWidth - 30, (headerHeight / 2) - 6, 8, 12);
    for (int i = 0; i < setlistButtons.size(); ++i) {
        if (setlistButtons[i]->isVisible()) {
            setlistButtons[i]->setBounds(0, (buttonHeight * setlistCount), setListWidth, buttonHeight);
            setlistCount++;
        }
    }
    int w = setListWidth;
    int h1 = getHeight() - headerHeight;
    int h2 = headerHeight + (buttonHeight * setlistCount) + (int)padding;
    if (h2 > h1) {
        w -= 10;
        h1 = h2;
    }
    setlistContainer.setBounds(0, headerHeight * 2, w, h1);

    // ChordPro
    if (chordProForCurrentSong && viewportRight.isVisible()) {
        float runningHeight = 0.0;
        float columns = (chordProTwoColumns) ? 2.0 : 1.0;
        int columnIndex = 0;
        int pageIndex = 0;
        float padding = chordProImagesOnly ? chordProContainer.getWidth() * 0.005 : 0.0;
        int imageCount = chordProGetVisibleImageCount();
        if (!chordProImagesOnly) runningHeight += chordProTopPadding;

        for (auto i = 0; i < chordProLines.size(); ++i) {
            if (chordProLines[i]->isVisible() && !(chordProImagesOnly && chordProLines[i]->getProperties()["type"] == "")) {
                if (chordProLines[i]->getProperties()["type"] == "chordAndLyrics") {
                    rowHeight = 80.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "title") {
                    rowHeight = 80.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "subtitle") {
                    rowHeight = 50.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "label") {
                    rowHeight = 60.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "comment") {
                    rowHeight = 50.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "tab") {
                    rowHeight = 30.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "grid") {
                    rowHeight = 40.0 * chordProFontSize;
                } else if (chordProLines[i]->getProperties()["type"] == "image") {
                    rowHeight = 0.0;
                    int imageIndex = chordProLines[i]->getProperties()["imageIndex"]; 
                    float originalWidth = (float)(extension->chordProImages[imageIndex]->getImage().getWidth()); 
                    float originalHeight = (float)(extension->chordProImages[imageIndex]->getImage().getHeight());
                    float ratio = originalWidth / (float)(viewportRight.getWidth() - 10);
                    float newWidth = (float)(viewportRight.getWidth() - 10) / columns;
                    if (chordProTwoColumns && chordProImagesOnly) newWidth = newWidth - (padding / 2.0);
                    float newHeight = originalHeight / (ratio * columns);
                    int imageX = 0;
                    if (fitHeight && chordProImagesOnly) {
                        newHeight = (float)(viewportRight.getHeight() - padding);
                        newWidth = originalWidth / originalHeight * newHeight;
                        imageX = juce::jmax(0, (int)((viewportRight.getWidth() - newWidth) / 2));
                        if (chordProTwoColumns) imageX = juce::jmax(0, (int)((viewportRight.getWidth() - (newWidth * columns)) / 2));
                    }
                    rowHeight = newHeight; 
                    if (chordProTwoColumns && chordProImagesOnly && !fitHeight) {
                        extension->chordProImages[imageIndex]->setBounds((imageIndex % 2 == 0) ? 0 : newWidth + padding , runningHeight, truncatePositiveToUnsignedInt(newWidth), rowHeight);
                    } else if (chordProTwoColumns && chordProImagesOnly && fitHeight) {
                        extension->chordProImages[imageIndex]->setBounds((imageIndex % 2 == 0) ? imageX : imageX + newWidth + padding, runningHeight, truncatePositiveToUnsignedInt(newWidth), rowHeight);
                    } else {
                        extension->chordProImages[imageIndex]->setBounds(imageX, runningHeight, truncatePositiveToUnsignedInt(newWidth), rowHeight);
                    }
                    rowHeight = (chordProTwoColumns && chordProImagesOnly && imageIndex % 2 == 0 && imageIndex < imageCount - 1) ? 0 : newHeight + padding;
                } else if (chordProLines[i]->getProperties()["type"] == "diagramKeyboard") {
                    int leftPad = chordProLeftLabels ? CP_EXPANDED_LEFT_MARGIN * chordProFontSize : CP_DEFAULT_LEFT_MARGIN;
                    int x = leftPad;
                    int y = runningHeight;
                    int w = CP_KEYBOARD_WIDTH * chordProFontSize; 
                    int h = CP_KEYBOARD_HEIGHT * chordProFontSize;
                    int s = CP_KEYBOARD_SEPARATOR * chordProFontSize;
                    for (int i = 0; i < chordProDiagramKeyboard.size(); ++i) {
                        // Wrap diagram
                        if (x - s + w > chordProContainer.getWidth() / columns) {
                            x = leftPad;
                            y += h;
                        }
                        chordProDiagramKeyboard[i]->setBounds(x, y, w, h);
                        x += (w + s);
                    }
                    rowHeight = y + h - runningHeight;
                } else if (chordProLines[i]->getProperties()["type"] == "diagramFretboard") {
                    rowHeight = CP_FRETBOARD_WIDTH * chordProFontSize;
                    int leftPad = chordProLeftLabels ? CP_EXPANDED_LEFT_MARGIN * chordProFontSize : CP_DEFAULT_LEFT_MARGIN;
                    int diagramX = 0;
                    int diagramW = 0;
                    for (int i = 0; i < chordProDiagramFretboard.size(); ++i) {
                        diagramX = leftPad + (i * (CP_FRETBOARD_WIDTH + CP_FRETBOARD_SEPARATOR) * chordProFontSize);
                        diagramW = CP_FRETBOARD_WIDTH * chordProFontSize;
                        chordProDiagramFretboard[i]->setBounds(diagramX, runningHeight, diagramW, rowHeight);
                        bool display = (!chordProTwoColumns || (chordProTwoColumns && ((diagramX + diagramW - CP_FRETBOARD_SEPARATOR) < (viewportRight.getWidth() / columns)))); 
                        chordProDiagramFretboard[i]->setVisible(display);
                    }
                } else if (chordProLines[i]->getName() == "autoscrollSpacer") {
                    // Don't alter size
                    rowHeight = chordProLines[i]->getHeight();
                } else {
                    rowHeight = 40.0 * chordProFontSize;
                }
                int chordProX = 10;
                int chordProPad = 10;
                // Two pages layout
                if (chordProTwoColumns && !chordProImagesOnly && runningHeight + rowHeight > viewportRight.getHeight() * (pageIndex + 1)){
                    runningHeight = 20.0;
                    columnIndex = (columnIndex * -1) + 1;
                    if (columnIndex == 0) pageIndex++;
                    runningHeight += (viewportRight.getHeight() * pageIndex);
                }
                chordProX = columnIndex == 0 ? chordProPad : chordProContainer.getWidth() / columns + chordProPad;
                auto bounds = Rectangle(chordProX, (int)runningHeight, (int)(chordProContainer.getWidth() / columns - chordProPad), (int)rowHeight);
                chordProLines[i]->setBounds(bounds);
                // Adjust position of images for two column view
                if (chordProLines[i]->getProperties()["type"] == "image" && chordProTwoColumns && !chordProImagesOnly) {
                    int imageIndex = chordProLines[i]->getProperties()["imageIndex"]; 
                    extension->chordProImages[imageIndex]->setBounds(bounds);
                }
                runningHeight += rowHeight;

                // Wrapping text for lyrics and chords
                if (chordProLines[i]->getProperties()["type"] == "chordAndLyrics" || chordProLines[i]->getProperties()["type"] == "lyricOnly" || chordProLines[i]->getProperties()["type"] == "comment") {
                    Font labelFont (Font (chordProLines[i]->getHeight() * 0.5f, Font::plain));
                    if (chordProLines[i]->getProperties()["type"] == "lyricOnly") {
                        labelFont.setHeight(chordProLines[i]->getHeight());
                    } else if (chordProLines[i]->getProperties()["type"] == "comment") {
                        labelFont.setHeight(chordProLines[i]->getHeight() * 0.7f);
                    }
                    if (chordProMonospaceFont) {
                        labelFont.setTypefaceName(Font::getDefaultMonospacedFontName());
                    } else {
                        labelFont = labelFont.withTypefaceStyle("Regular").withExtraKerningFactor(0.06f);
                    }
                    int leftPad = chordProLeftLabels ? CP_EXPANDED_LEFT_MARGIN * chordProFontSize : CP_DEFAULT_LEFT_MARGIN;
                    String excludeChords = std::regex_replace(chordProLines[i]->getProperties()["originalText"].toString().toStdString(), std::regex("\\[(.*?)\\]"), "");
                    int labelWidth = labelFont.getStringWidth(excludeChords) + leftPad;
                    if (labelWidth > chordProContainer.getWidth() / columns) {
                        if (chordProLines[i]->getName() != "wrap") {
                            if (i < chordProLines.size() - 1) {
                                if (chordProLines[i+1]->getName() != "wrap") { // Wrapped line doesn't yet exist - create one
                                    addChordProLine(i+1, "wrap");
                                    chordProLines[i+1]->getProperties().set("type", chordProLines[i]->getProperties()["type"]);
                                    chordProLines[i+1]->getProperties().set("section", chordProLines[i]->getProperties()["section"]); 
                                    chordProLines[i+1]->setLookAndFeel(&chordProLines[i]->getLookAndFeel());
                                }
                                // Update label text on both wrapped lines
                                int wrapPosition = labelWidth - (labelWidth - (chordProContainer.getWidth() / columns));
                                StringArray words = StringArray::fromTokens(excludeChords,false);
                                words.removeEmptyStrings();
                                int runningTextWidth = 0;
                                int wordIndex = 0;
                                for (int i = 0; i < words.size(); ++i) { 
                                    runningTextWidth += labelFont.getStringWidth(words[i] + " ");
                                    if (runningTextWidth + leftPad > wrapPosition) {
                                        if (wordIndex == 0)
                                        wordIndex = i;
                                    }
                                }
                                String firstLine = "";
                                String secondLine = "";
                                words = StringArray::fromTokens(chordProLines[i]->getProperties()["originalText"].toString(),false);
                                words.removeEmptyStrings();
                                for (int i = 0; i < words.size(); ++i) {  
                                    if (i < wordIndex) firstLine += words[i] + " ";
                                    if (i >= wordIndex) secondLine += words[i] + " ";
                                }
                                // Update line type if it only contains lyrics
                                if (chordProLines[i]->getProperties()["type"] == "chordAndLyrics") {
                                    if (!secondLine.contains("[")) {
                                        chordProLines[i+1]->getProperties().set("type","lyricOnly");
                                    } else {
                                        chordProLines[i+1]->getProperties().set("type","chordAndLyrics");
                                    }
                                }
                                chordProLines[i]->setText(firstLine, dontSendNotification);
                                chordProLines[i+1]->setText(secondLine, dontSendNotification);
                            }
                        }
                    } else {
                        if (i < chordProLines.size() - 1) {
                            if (chordProLines[i + 1]->getName() == "wrap") {
                                chordProLines.remove(i+1, true);
                                chordProLines[i]->setText(chordProLines[i]->getProperties()["originalText"], dontSendNotification);
                            }
                        }   
                    }
                } 
            } else {
                chordProLines[i]->setBounds(10,runningHeight,chordProContainer.getWidth() / columns, 0);
            }
        }
        if (chordProTwoColumns && !chordProImagesOnly) {
            chordProContainer.setBounds(viewportRight.getX(), viewportRight.getY(), viewportRight.getWidth() - 10, jmax(viewportRight.getHeight(), viewportRight.getHeight() * (pageIndex + 1) ));
        } else {
            chordProContainer.setBounds(viewportRight.getX(), viewportRight.getY(), viewportRight.getWidth() - 10, jmax(viewportRight.getHeight(), (int)runningHeight));
        }
    }
    viewportRight.setViewPosition(viewRightPos);
}

void ExtensionWindow::gigLoaded(std::vector<std::string> songNames, std::vector<std::string> setlistNames) {
    if (extension == nullptr) return;
    MessageManager::getInstance()->callAsync([songNames, setlistNames]() {
        updateButtonNames(songNames);
        updateSetlistButtons(setlistNames);
        chordProReadFile(0);
        if (!isButtonSelected(0)) { 
            selectButton(0);
            updateSubButtonNames(getDisplayVariationForSongPartStatus() ? lib->getVariationNamesForSong(0) : lib->getSongPartNames(0));
            selectSubButton(0);
        } else {
            updateSubButtonNames(getDisplayVariationForSongPartStatus() ? lib->getVariationNamesForSong(0) : lib->getSongPartNames(0));
        }
        refreshUI();
    });
}

void ExtensionWindow::songChanged(int songIndex, std::vector<std::string> songNames) {
    if (extension == nullptr) return;
    MessageManager::getInstance()->callAsync([songIndex, songNames]() {
        updateButtonNames(songNames);
        chordProReadFile(songIndex);
        if (!isButtonSelected(songIndex)) { // If selected in GP directly, ensure buttons are in sync
            selectButton(songIndex);
            updateSubButtonNames(getDisplayVariationForSongPartStatus() ? lib->getVariationNamesForSong(songIndex) : lib->getSongPartNames(songIndex));
            selectSubButton(lib->getCurrentSongpartIndex());
            extension->resized();
        } else {
            updateSubButtonNames(getDisplayVariationForSongPartStatus() ? lib->getVariationNamesForSong(songIndex) : lib->getSongPartNames(songIndex));
            selectSubButton(0);
        }
    });
}

void ExtensionWindow::songPartChanged(int songPartIndex, int songIndex) {
    if (extension == nullptr) return;
    MessageManager::getInstance()->callAsync([songPartIndex, songIndex]() {
        if (!isSubButtonSelected(songPartIndex)) {
            compareSubButtonNames(lib->getSongPartNames(songIndex));
            selectSubButton(songPartIndex);
            chordProScrollToSongPart(lib->getSongpartName(songIndex, songPartIndex));
        }
    });
}

void ExtensionWindow::setlistChanged(int setlistIndex, int songIndex, std::vector<std::string> setlistNames, std::vector<std::string> songNames) {
    if (extension == nullptr) return;
    updateSetlistButtons(setlistNames);
    updateButtonNames(songNames);
    selectSetlistButton(setlistIndex);
    chordProReadFile(songIndex);
    selectButton(songIndex);
    updateSubButtonNames(getDisplayVariationForSongPartStatus() ? lib->getVariationNamesForSong(songIndex) : lib->getSongPartNames(songIndex));
    selectSubButton(lib->getCurrentSongpartIndex());
    extension->resized();
}

void ExtensionWindow::refreshUI() {
    int songIndex = lib->inSetlistMode() ? lib->getCurrentSongIndex() : extension->getButtonSelected();
    int songPartIndex = lib->inSetlistMode() ? lib->getCurrentSongpartIndex() : 0;

    // Reset all buttons
    for (int i = 0; i < extension->buttons.size(); ++i) {
        extension->buttons[i]->setToggleState(false, dontSendNotification);
        extension->buttons[i]->setVisible(false);
    }
    // Reset all sub buttons
    for (int i = 0; i < extension->subButtons.size(); ++i) {
        extension->subButtons[i]->setToggleState(false, dontSendNotification);
        extension->subButtons[i]->setVisible(false);
    }

    setTitleBarName(SONG_WINDOW_TITLE);
    updateButtonNames(lib->getSongNames());
    updateSubButtonNames(extension->getSubButtonNamesByIndex(songIndex));
    
    selectSubButton(songPartIndex);
    selectButton(songIndex);
  
    extension->resized();
}

void ExtensionWindow::setTitleBarName(const String& name) {
    extension->extensionWindow->setName(name);
}

void ExtensionWindow::setZeroBasedNumbering(bool zeroBased) {
    extension->preferences->setProperty("ZeroBasedNumbers", zeroBased); 
    int offset = zeroBased ? 0 : 1;
    for (int i = 0; i < extension->buttons.size(); ++i) {
        extension->buttons[i]->getProperties().set("displayIndex", i + offset);
    }
}

void ExtensionWindow::setImmediateSwitching(bool immediateSwitch) {
    extension->preferences->setProperty("ImmediateSwitching", immediateSwitch);     
}

void ExtensionWindow::setDarkMode(bool darkMode) {
    extension->preferences->setProperty("DarkMode", darkMode); 
    chordProDarkMode = darkMode;
    for (int i = 0; i < extension->chordProDiagramKeyboard.size(); ++i) {
        extension->chordProDiagramKeyboard[i]->setDarkMode(darkMode);
    }
    for (int i = 0; i < extension->chordProDiagramFretboard.size(); ++i) {
        extension->chordProDiagramFretboard[i]->setDarkMode(darkMode);
    }
    extension->chordProEditor->applyColourToAllText(chordProDarkMode ? Colours::white : Colours::black, true);
    if (chordProDarkMode) {
        extension->editorCloseButton->setColours(Colours::white, Colours::lightgrey, Colours::grey);
    } else {
        extension->editorCloseButton->setColours(Colours::black, Colours::darkgrey, Colours::grey);
    }
    extension->chordProSetImageDarkMode(darkMode);
    extension->chordProSetColors();

    extension->repaint();
}

void ExtensionWindow::setDisplayVariationNames(bool display) {
    extension->preferences->setProperty("DisplayVariationNames", display); 
    displayVariationsForSong = display;
}

void ExtensionWindow::setFitToPageHeight(bool fitToHeight) {
    extension->preferences->setProperty("FitToPageHeight", fitToHeight); 
    extension->fitHeight = fitToHeight;
}

void ExtensionWindow::setDisplayTwoPages(bool display) {
    extension->preferences->setProperty("DisplayTwoPages", display); 
    extension->chordProTwoColumns = display;
    chordProTwoColumnsExtern = display && !extension->chordProImagesOnly;
}

void ExtensionWindow::chordProSetLeftMarginLabels(bool onLeft) {
    extension->preferences->setProperty("LeftMarginLabels", onLeft); 
    chordProLeftLabels = onLeft;
}

void ExtensionWindow::toggleZeroBasedNumbering() {
    bool status = extension->preferences->getProperty("ZeroBasedNumbers");
    extension->preferences->setProperty("ZeroBasedNumbers", !status); 
    setZeroBasedNumbering(!status);
    extension->resized();
}

void ExtensionWindow::toggleImmediateSwitching() {
    bool status = extension->preferences->getProperty("ImmediateSwitching");
    extension->preferences->setProperty("ImmediateSwitching", !status);     
}

void ExtensionWindow::setLargeScrollArea(bool largeScrollArea) {
    extension->preferences->setProperty("LargeScrollArea", largeScrollArea); 
}

void ExtensionWindow::toggleLargeScrollArea() {
    bool status = extension->preferences->getProperty("LargeScrollArea");
    extension->preferences->setProperty("LargeScrollArea", !status); 
    extension->resized();   
}

void ExtensionWindow::toggleThickBorders() {
    bool status = extension->preferences->getProperty("ThickBorders");
    extension->preferences->setProperty("ThickBorders", !status); 
    refreshUI();
}

void ExtensionWindow::toggleLockToSetlistMode() {
    lockToSetlistMode = !lockToSetlistMode;
    if (!lib->inSetlistMode() && lockToSetlistMode) {
        lib->switchToSetlistView();
    } else if (!lib->inSetlistMode() && !lockToSetlistMode) {
        lib->switchToPanelView();
    }
}

void ExtensionWindow::toggleVariationsInSetlistMode() {
    bool status = extension->preferences->getProperty("DisplayVariationNames");
    extension->preferences->setProperty("DisplayVariationNames", !status); 
    displayVariationsForSong = !status;
    refreshUI();
}

void ExtensionWindow::toggleLeftMarginLabels() {
    bool status = extension->preferences->getProperty("LeftMarginLabels");
    extension->preferences->setProperty("LeftMarginLabels", !status);
    chordProLeftLabels = !status;
    refreshUI();
}

void ExtensionWindow::toggleDarkMode() {
    bool status = extension->preferences->getProperty("DarkMode");
    setDarkMode(!status);
}

void ExtensionWindow::toggleSmallChordFont() {
    bool status = extension->preferences->getProperty("SmallChordFont");
    extension->preferences->setProperty("SmallChordFont", !status);
    chordProSmallChordFont = !status;
    refreshUI();
}

void ExtensionWindow::toggleAutoscrollPanelPersist() {
    bool status = extension->preferences->getProperty("AutoscrollPanelPersist");
    extension->preferences->setProperty("AutoscrollPanelPersist", !status);
    autoscrollPanelPersist = !status;
}

void ExtensionWindow::toggleAutoscrollStartWithPlayhead() {
    bool status = extension->preferences->getProperty("AutoscrollStartWithPlayhead");
    extension->preferences->setProperty("AutoscrollStartWithPlayhead", !status);
    autoscrollStartWithPlayhead = !status;
}

String ExtensionWindow::buttonName(int index) {
    String name = "";
    if (index < buttons.size()) {
        name = extension->buttons[index]->getButtonText();
    }
    return name;
}

bool ExtensionWindow::isButtonSelected(int index) {
    if (index < extension->subButtons.size() && index >= 0) {
        return extension->buttons[index]->getToggleState();
    }
    return false;
}

int ExtensionWindow::getButtonSelected() {
    int selected = 0;
    for (int i = 0; i < extension->buttons.size(); ++i) {
        if (extension->buttons[i]->getToggleState()) {
            selected = i;
            break;
        }
    }
    return selected;
}

int ExtensionWindow::getSubButtonSelected() {
    int selected = 0;
    for (int i = 0; i < extension->subButtons.size(); ++i) {
        if (extension->subButtons[i]->getToggleState()) {
            selected = i;
            break;
        }
    }
    return selected;
}

void ExtensionWindow::filterButtons(String text) {
    String name = "";
    StringArray nameParts;
    bool match = false;
    text = text.toLowerCase();
    for (int i = 0; i < extension->buttons.size(); ++i) {
        name = extension->buttons[i]->getButtonText().toLowerCase();
        name = name.replace("-"," ");
        if (text.contains(" ")) {
            if (name.contains(text)) {
                match = true;
            }
        } else {
            nameParts = StringArray::fromTokens(name, false);
            for (int j = 0; j < nameParts.size(); ++j) {
                if (nameParts[j].startsWith(text)) {
                    match = true;
                }
            }
        }
        extension->buttons[i]->setVisible(match);
        match = false;
    }
    extension->resized();
}

int ExtensionWindow::getVisibleButtonCount() {
    int visible = 0;
    for (auto i = 0; i < extension->buttons.size(); ++i) {
        if (extension->buttons[i]->isVisible()) {
            ++visible;
        }
    }
    return visible;
}

int ExtensionWindow::getVisibleSubButtonCount() {
    int visible = 0;
    for (auto i = 0; i < extension->subButtons.size(); ++i) {
        if (extension->subButtons[i]->isVisible()) {
            ++visible;
        }
    }
    return visible;
}

void ExtensionWindow::selectSetlistButton(int index) {
    extension->setlistButtons[index]->setToggleState(true, dontSendNotification);
}

void ExtensionWindow::selectButton(int index) {
    if (index < extension->buttons.size() && index >= 0) {
        extension->buttons[index]->setToggleState(true, dontSendNotification);
        Rectangle<int> viewportBounds = extension->viewport.getViewArea();
        Rectangle<int> buttonBounds = extension->buttons[index]->getBounds();
        auto pad = buttonBounds.getWidth() / 40;
        pad = pad + 0.5 - (pad < 0); 
        int padding = (int)pad;
        int viewY = viewportBounds.getY();
        int btnY = buttonBounds.getY();
        if (btnY < viewY) {
            extension->viewport.setViewPosition(0, btnY - padding);
        }
        setSongLabel();
    }
}

void ExtensionWindow::selectSongForCurrentButton() {
    if (lib == nullptr) return;
        int buttonIndex = extension->getButtonSelected();
        if (!lib->inSetlistMode()) lib->switchToSetlistView();
        lib->switchToSong(buttonIndex, 0);
}

bool ExtensionWindow::isSubButtonSelected(int index) {
    if (index < extension->subButtons.size() && index >= 0) {
        return extension->subButtons[index]->getToggleState();
    }
    return false;
}

bool ExtensionWindow::isSubButtonsCollapsed() {
    bool collapsed = true;
    int buttonCount = extension->subButtons.size();
    if (buttonCount > 1) { // If only 1 button it will be collapsed by default
        for (int i = 0; i < buttonCount; ++i) {
            if (extension->subButtons[i]->isVisible()) {
                collapsed = false;
                break;
            }
        }
    }
    return collapsed;
}

void ExtensionWindow::selectSubButton(int index) {
    if (index < extension->subButtons.size() && index >= 0) {
        extension->subButtons[index]->setToggleState(true, dontSendNotification);
        updateViewportPositionForSubButtons();
    }
}

void ExtensionWindow::addButtons(int count) {
    int buttonCount = extension->buttons.size();
    int index;
    int offset = extension->preferences->getProperty("ZeroBasedNumbers") ? 0 : 1;
    for (auto i = 0; i < count; ++i) {
        index = buttonCount + i;
        std::string number = std::to_string(index);
        auto button = new TextButton(number); 
        extension->buttons.add(button);
        extension->buttons[index]->setLookAndFeel(extension->buttonsLnF);
        extension->buttons[index]->setClickingTogglesState(true);
        extension->buttons[index]->setRadioGroupId(1);
        extension->buttons[index]->getProperties().set("index", index);
        extension->buttons[index]->getProperties().set("displayIndex", index + offset);
        extension->buttons[index]->getProperties().set("type", "button"); 
        extension->buttons[index]->setTriggeredOnMouseDown(true);
        extension->buttons[index]->addListener(extension);  
        extension->container.addAndMakeVisible(extension->buttons[index]);
    }
}

void ExtensionWindow::addSetlistButtons(int count) {
    int buttonCount = extension->setlistButtons.size();
    int index;
    for (auto i = 0; i < count; ++i) {
        index = buttonCount + i;
        std::string number = std::to_string(index);
        auto button = new TextButton(number); 
        extension->setlistButtons.add(button);
        extension->setlistButtons[index]->setLookAndFeel(extension->setlistListButtonLnF);
        extension->setlistButtons[index]->setClickingTogglesState(true);
        extension->setlistButtons[index]->setRadioGroupId(3);
        extension->setlistButtons[index]->getProperties().set("type", "setlistButton"); 
        extension->setlistButtons[index]->setTriggeredOnMouseDown(true);
        extension->setlistButtons[index]->addListener(extension);  
        extension->setlistContainer.addAndMakeVisible(extension->setlistButtons[index]);
    }
}

void ExtensionWindow::updateButtonNames(std::vector<std::string> buttonNames) {
    int newButtonCount = buttonNames.size();
    int currentButtonCount = extension->buttons.size();
    bool border = extension->preferences->getProperty("ThickBorders");
    if (newButtonCount == 0) {
        extension->noSongsLabel->setVisible(true);
    } else {
        extension->noSongsLabel->setVisible(false);
        if (newButtonCount > currentButtonCount) {
            addButtons(newButtonCount-currentButtonCount);
            currentButtonCount = newButtonCount;
        }
        for (auto i = 0; i < currentButtonCount; ++i) {
            if (i < newButtonCount) {
                extension->buttons[i]->setButtonText(buttonNames[i]);
                extension->buttons[i]->setVisible(true);
                extension->buttons[i]->getProperties().set("colour", DEFAULT_BUTTON_COLOR);
                extension->buttons[i]->getProperties().set("thickBorder", border);
            } else {
                extension->buttons[i]->setButtonText("");
                extension->buttons[i]->setVisible(false);
            }
        } 
    }
 }

void ExtensionWindow::compareButtonNames(std::vector<std::string> newButtonNames) {
    int buttonCount = extension->buttons.size();
    int newButtonCount = newButtonNames.size();
    int visibleButtons = getVisibleButtonCount();
    if (visibleButtons != newButtonCount) {
        refreshUI();
    } else {
        for (auto i = 0; i < newButtonCount; ++i) {
            if (i < buttonCount && newButtonNames[i] != extension->buttons[i]->getButtonText()) {
                refreshUI();
            }
        }
    }
}

void ExtensionWindow::addSubButtons(int count) {
    int buttonCount = extension->subButtons.size();
    int index;
    int offset = extension->preferences->getProperty("ZeroBasedNumbers") ? 0 : 1;
    for (auto i = 0; i < count; ++i) {
        index = buttonCount + i;
        std::string number = std::to_string(index);
        auto button = new TextButton(number); 
        extension->subButtons.add(button);
        extension->subButtons[index]->setLookAndFeel(extension->subButtonsLnF);
        extension->subButtons[index]->setClickingTogglesState(true);
        extension->subButtons[index]->setRadioGroupId(2);
        extension->subButtons[index]->getProperties().set("index", index);
        extension->subButtons[index]->getProperties().set("displayIndex", index + offset);
        extension->subButtons[index]->getProperties().set("type", "subButton"); 
        extension->subButtons[index]->setTriggeredOnMouseDown(true);
        extension->subButtons[index]->addListener(extension);  
        extension->container.addAndMakeVisible(extension->subButtons[index]);
    }
}

void ExtensionWindow::updateSubButtonNames(std::vector<std::string> buttonNames) {
    int newButtonCount = buttonNames.size();
    int currentButtonCount = extension->subButtons.size();
    bool border = extension->preferences->getProperty("ThickBorders");
    String borderColor = extension->preferences->getProperty("BorderColor");

    if (newButtonCount > currentButtonCount) {
        addSubButtons(newButtonCount-currentButtonCount);
        currentButtonCount = newButtonCount;
    }
    for (auto i = 0; i < currentButtonCount; ++i) {
        if (i < newButtonCount) {
            String name = buttonNames[i];
            String color = DEFAULT_SUBBUTTON_COLOR;
            extension->subButtons[i]->setButtonText(name);
            extension->subButtons[i]->getProperties().set("name", name);
            extension->subButtons[i]->setVisible(true);
            StringArray keys = extension->buttonColors.getAllKeys();
            for (int j = 0; j < keys.size(); ++j ) {
                if (name.contains(keys[j])) {
                    color = extension->buttonColors.getValue(keys[j],"");
                    if (extension->preferences->getProperty("RemoveColorKeywordFromName")) {
                        name = name.replace(keys[j], "");
                        name = name.replace("  ", " ");
                        extension->subButtons[i]->setButtonText(name);
                    }
                }
            }
            extension->subButtons[i]->getProperties().set("colour", color);
            extension->subButtons[i]->getProperties().set("thickBorder", border);
            extension->subButtons[i]->getProperties().set("borderColor", borderColor);
        } else {
            extension->subButtons[i]->setButtonText("");
            extension->subButtons[i]->setVisible(false);
            extension->subButtons[i]->getProperties().set("colour", DEFAULT_SUBBUTTON_COLOR);
            extension->subButtons[i]->getProperties().set("name", "");
        }
    }
 }

 void ExtensionWindow::compareSubButtonNames(std::vector<std::string> newButtonNames) {
    int buttonCount = extension->subButtons.size();
    int visibleButtons = getVisibleSubButtonCount();
    int newButtonCount = newButtonNames.size();
    if (visibleButtons > 0) { // Ignore if collapsed
        if (visibleButtons != newButtonCount) {
            refreshUI();
        } else {
            for (auto i = 0; i < newButtonCount; ++i) {
                if (i < buttonCount && newButtonNames[i] != extension->subButtons[i]->getProperties()["name"]) {
                    refreshUI();
                }
            }
        }
    }
}

std::vector<std::string> ExtensionWindow::getSubButtonNamesByIndex(int index) {
    std::vector<std::string> names;
    names = displayVariationsForSong ? lib->getVariationNamesForSong(index) : lib->getSongPartNames(index);

    return names;
} 

std::string ExtensionWindow::getSubButtonNameByIndex(int index, int subIndex) {
    std::string name;
    name = displayVariationsForSong ? lib->getVariationNameForSongPart(index, subIndex) : lib->getSongpartName(index, subIndex);

    return name;
} 

void ExtensionWindow::updateSetlistButtons(std::vector<std::string> buttonNames) {
    String name = lib->getSetlistName(lib->getCurrentSetlistIndex());
    extension->setlistButton->setButtonText(name);
    int newButtonCount = buttonNames.size();
    int currentButtonCount = extension->setlistButtons.size();
    if (newButtonCount > currentButtonCount) {
        addSetlistButtons(newButtonCount-currentButtonCount);
        currentButtonCount = newButtonCount;
    }
    for (auto i = 0; i < currentButtonCount; ++i) {
        if (i < newButtonCount) {
            extension->setlistButtons[i]->setButtonText(buttonNames[i]);
            extension->setlistButtons[i]->setVisible(true);
        } else {
            extension->setlistButtons[i]->setVisible(false);
        }
    }
}

bool ExtensionWindow::getDisplayVariationForSongPartStatus() {
    return displayVariationsForSong;
}

void ExtensionWindow::updateButtonLabel(const String& text) {
    if (text == "Songs"){ 
        extension->header->setLookAndFeel(extension->headerSongsLnF);
    } else {
        extension->header->setLookAndFeel(extension->headerRackspacesLnF);
    }
    extension->resized();
}

void ExtensionWindow::incrementButton(int increment) {
    int currentIndex = getButtonSelected();
    int newIndex = currentIndex;
    if (lib->inSetlistMode()) {
        currentIndex = lib->getCurrentSongIndex();
        newIndex = currentIndex + increment;
        newIndex = juce::jmax(0, newIndex);
        newIndex = juce::jmin(newIndex, lib->getSongCount());
        lib->switchToSong(newIndex, 0);
    } else {
        newIndex = currentIndex + increment;
        newIndex = juce::jmax(0, newIndex);
        newIndex = juce::jmin(newIndex, extension->getVisibleButtonCount() - 1);
        extension->buttons[newIndex]->setToggleState (true, dontSendNotification);
        extension->updateSubButtonNames(getSubButtonNamesByIndex(newIndex));
        extension->updateViewportPositionForSubButtons();
        extension->selectButton(newIndex);
        extension->selectSubButton(0); // Force selection of first variation/part to avoid sync issues
        extension->setSongLabel();
        extension->chordProReadFile(newIndex);
    }
}

void ExtensionWindow::incrementSubButton(int increment) {
    int newSubButtonIndex = getSubButtonSelected() + increment;
    int buttonIndex = getButtonSelected();
    int visibleButtons = getVisibleButtonCount();
    int visibleSubButtons = getVisibleSubButtonCount();
    
    if (newSubButtonIndex < 0 && buttonIndex > 0) { // Previous Song
        extension->buttons[buttonIndex-1]->triggerClick();
        selectButton(buttonIndex-1);
    } else if (newSubButtonIndex >= visibleSubButtons && buttonIndex < visibleButtons - 1) { // Next Song
        extension->buttons[buttonIndex+1]->triggerClick();
        selectButton(buttonIndex+1);
    } else if (newSubButtonIndex >= 0 && newSubButtonIndex < visibleSubButtons) { // Change Song Part
        extension->subButtons[newSubButtonIndex]->triggerClick();
    }
    extension->updateViewportPositionForSubButtons();
}

void ExtensionWindow::updateViewportPositionForSubButtons() {
    Rectangle<int> viewportBounds = extension->viewport.getViewArea();
    Rectangle<int> buttonBounds = extension->buttons[getButtonSelected()]->getBounds();
    auto pad = buttonBounds.getHeight() * 0.1;
    pad = pad + 0.5 - (pad < 0); 
    int padding = (int)pad;
    int viewY = viewportBounds.getY();
    int viewH = viewportBounds.getHeight();
    int btnH = buttonBounds.getHeight();
    int btnY = padding + (getButtonSelected() * (btnH + padding));
    int subButtonCount = getVisibleSubButtonCount();
    int totalH = (btnH + padding) * (subButtonCount + 2);
    if ((btnY + totalH) > (viewY + viewH)) {
        int adjY = (viewH - totalH > 0) ? (viewH - totalH) : 0;
        extension->viewport.setViewPosition(0, (btnY - adjY) > 0 ? (btnY - adjY) : 0);
    } 
}

void ExtensionWindow::buttonClicked (Button* buttonThatWasClicked)
{
   if (buttonThatWasClicked == sidePanelOpenButton.get() || buttonThatWasClicked == sidePanelCloseButton.get())
    {
        displaySongPanel = !displaySongPanel;
        setSongPanelPosition(displaySongPanel);
        resized();
    } else if (buttonThatWasClicked == pinUnpinnedButton.get() || buttonThatWasClicked == pinPinnedButton.get()) {
        bool newPinnedStatus = !(extension->extensionWindow->isAlwaysOnTop());
        windowPinned = newPinnedStatus;
        pinUnpinnedButton->setVisible(!newPinnedStatus);
        pinPinnedButton->setVisible(newPinnedStatus);
        extension->extensionWindow->setAlwaysOnTop(newPinnedStatus);
        resized();
    } else if (buttonThatWasClicked == fullscreenActivateButton.get() || buttonThatWasClicked == fullscreenDeactivateButton.get()) {
        bool newFullscreenStatus = !extension->extensionWindow->isFullScreen();
        
        #if JUCE_WINDOWS
            newFullscreenStatus = !(Desktop::getInstance().getKioskModeComponent() == getTopLevelComponent());
            Desktop::getInstance().setKioskModeComponent(newFullscreenStatus ? getTopLevelComponent() : nullptr, false);
            if (!newFullscreenStatus) extension->extensionWindow->getPeer()->setIcon(getWindowIcon());
        #else
            extension->extensionWindow->setFullScreen(newFullscreenStatus);
        #endif
        windowFullscreen = newFullscreenStatus;
        fullscreenActivateButton->setVisible(!newFullscreenStatus);
        fullscreenDeactivateButton->setVisible(newFullscreenStatus);
        resized();
    } else if (buttonThatWasClicked->getProperties()["type"] == "button") {
        if (!buttonThatWasClicked->getToggleState()) return;
        bool switchRackSongImmediately = true;
        bool inSetlist = lib->inSetlistMode();
        int buttonIndex = buttonThatWasClicked->getProperties()["index"];
        int currentGPIndex = (inSetlist ? lib->getCurrentSongIndex() : -1);
        std::vector<std::string> blank;

        // Ensure other buttons are deselected
        for (int i = 0; i < buttons.size(); ++i) {
            if (buttonIndex != i) {
                buttons[i]->setToggleState(false, dontSendNotification);
            }
        }

        // Deactivate autoscroll widget if playing and not in setlist mode
        if (!inSetlist)
            lib->setWidgetValue(WIDGET_CP_AUTOSCROLL_PLAY, 0.0);

        if (isSubButtonsCollapsed() || (buttonIndex != prevButtonSelected)) {
            // Expand
            updateSubButtonNames(getSubButtonNamesByIndex(buttonIndex));
            if (buttonIndex != currentGPIndex && switchRackSongImmediately) {
                selectSubButton(0); // Force selection of first variation/part to avoid sync issues
            }
             // Ensure no button is selected on new song/rackspace if pref is set
            if (buttonIndex != currentGPIndex && !switchRackSongImmediately) {
                for (int i = 0; i < subButtons.size(); ++i) {
                    if (subButtons[i]->getToggleState()) {
                        subButtons[i]->setToggleState(false, dontSendNotification);
                    }
                }
            }
        } else {         
            // Collapse
            updateSubButtonNames(blank);
        }
        if (buttonIndex != currentGPIndex && switchRackSongImmediately) {
            if (inSetlist) {
                lib->switchToSong(buttonIndex, 0);
            } else {
                selectButton(buttonIndex);
                chordProReadFile(buttonIndex);          
            }
            setSongLabel();
        }
        resized();
        prevButtonSelected = buttonIndex;
    } else if (buttonThatWasClicked->getProperties()["type"] == "subButton") {
        if (!buttonThatWasClicked->getToggleState()) return;
        bool inSetlist = lib->inSetlistMode();
        int subButtonIndex = buttonThatWasClicked->getProperties()["index"];
        int buttonIndex = getButtonSelected();

        if (inSetlist) {
            lib->switchToSong(buttonIndex, subButtonIndex);
            std::string songpartName = getSubButtonNameByIndex(buttonIndex, subButtonIndex);
            chordProScrollToSongPart(songpartName);
        } else {
            std::string songpartName = getSubButtonNameByIndex(buttonIndex, subButtonIndex);
            chordProScrollToSongPart(songpartName);
        }
        // Ensure other buttons are toggled off
        for (int i = 0; i < subButtons.size(); ++i) {
            if (i != subButtonIndex && subButtons[i]->getToggleState()) {
                subButtons[i]->setToggleState(false, dontSendNotification);
            }
        }
    } else if (buttonThatWasClicked == fontDown.get()) {
        chordProSetFontSize((chordProFontSize <= CP_MIN_FONT_SIZE) ? chordProFontSize : chordProFontSize - CP_FONT_STEP_SIZE);
    } else if (buttonThatWasClicked == fontUp.get()) {
        chordProSetFontSize((chordProFontSize >= CP_MAX_FONT_SIZE) ? chordProFontSize : chordProFontSize + CP_FONT_STEP_SIZE);
    } else if (buttonThatWasClicked == fontMono.get()) {
        chordProMonospaceFont = !chordProMonospaceFont;
        resized();
    } else if (buttonThatWasClicked == fontButton.get()) {
        transposeContainer.setVisible(false);
        displaySearchContainer(false);
        displayAutoscrollContainer(false);
        fontButtonContainer.setVisible(!fontButtonContainer.isVisible());
        resized();
    } else if (buttonThatWasClicked == autoscrollButton.get()) {
        transposeContainer.setVisible(false);
        displaySearchContainer(false);
        displayFontContainer(false);
        autoscrollContainer.setVisible(!autoscrollContainer.isVisible());
        resized();
    } else if (buttonThatWasClicked == autoscrollPlayButton.get()) {
        chordProAutoScrollPlay(buttonThatWasClicked->getToggleState());
        lib->setWidgetValue(WIDGET_CP_AUTOSCROLL_PLAY, buttonThatWasClicked->getToggleState() ? 1.0 : 0.0);
        
    } else if (buttonThatWasClicked == autoscrollResetButton.get()) {
        chordProAutoScrollReset();        
    } else if (buttonThatWasClicked == transposeButton.get()) {
        fontButtonContainer.setVisible(false);
        displaySearchContainer(false);
        displayAutoscrollContainer(false);
        transposeContainer.setVisible(!transposeContainer.isVisible());
        resized();
    } else if (buttonThatWasClicked == transposeDown.get()) {
        int transpose = (chordProTranspose > -6) ? chordProTranspose - 1 : -6;
        chordProSetTranspose(transpose);        
    } else if (buttonThatWasClicked == transposeUp.get()) {
        int transpose = (chordProTranspose < 6) ? chordProTranspose + 1 : 6;
        chordProSetTranspose(transpose);
    } else if (buttonThatWasClicked == transposeFlat.get()) {
        chordProTransposeDisplay = flat;
        chordProSetTranspose(chordProTranspose);
    } else if (buttonThatWasClicked == transposeSharp.get()) {
        chordProTransposeDisplay = sharp;
        chordProSetTranspose(chordProTranspose);
    } else if (buttonThatWasClicked == editButton.get()) {
        displayEditorContainer(!containerEditor.isVisible());
        resized();
    } else if (buttonThatWasClicked == editorCloseButton.get()) {
        displayEditorContainer(false);
        resized();
    } else if (buttonThatWasClicked == editorSaveButton.get()) {
        saveChordProFile();
        resized();
    } else if (buttonThatWasClicked == lightDarkModeButton.get()) {
        toggleDarkMode();
    } else if (buttonThatWasClicked == columnsOneButton.get()) {
        setDisplayTwoPages(false);
        resized();
    } else if (buttonThatWasClicked == columnsTwoButton.get()) {
        setDisplayTwoPages(true);
        resized();
    } else if (buttonThatWasClicked == fitWidthButton.get()) {
        setFitToPageHeight(false);
        resized();
    }  else if (buttonThatWasClicked == fitHeightButton.get()) {
        setFitToPageHeight(true);
        resized();
    } else if (buttonThatWasClicked == createInvertedImage.get()) {
        extension->missingImageLabel->setText(CP_DARK_MODE_IMAGE_PROCESSING, dontSendNotification);
        createInvertedImage->setVisible(false);
        extension->repaint();
        imageTimer.startTimer(TIMER_IMAGE_CONVERT);
    } else if (buttonThatWasClicked == setlistButton.get()) {
        toggleSetlistContainer();
        if (!lib->inSetlistMode()) lib->switchToSetlistView();
    } else if (buttonThatWasClicked->getProperties()["type"] == "setlistButton" && buttonThatWasClicked->getToggleState()) {       
        lib->switchToSetlist(buttonThatWasClicked->getName().getIntValue());
        displaySetlistContainer(false);
        compareButtonNames(lib->getSongNames());
        lib->setWidgetValue(WIDGET_CP_AUTOSCROLL_PLAY, 0.0);
    } else if (buttonThatWasClicked == prefButtons[0]) {
        toggleZeroBasedNumbering();
    } else if (buttonThatWasClicked == prefButtons[1]) {
        toggleLargeScrollArea();
    } else if (buttonThatWasClicked == prefButtons[2]) {
       toggleThickBorders();
       displayPreferencesContainer(true);
    } else if (buttonThatWasClicked == prefButtons[3]) {
        toggleVariationsInSetlistMode();
        displayPreferencesContainer(true);
    } else if (buttonThatWasClicked == prefButtons[4]) {
        toggleLeftMarginLabels();
        displayPreferencesContainer(true);
    } else if (buttonThatWasClicked == prefButtons[5]) {
        toggleSmallChordFont();
        displayPreferencesContainer(true);
    } else if (buttonThatWasClicked == prefButtons[6]) {
        toggleAutoscrollPanelPersist();
        displayPreferencesContainer(true);
    } else if (buttonThatWasClicked == prefButtons[7]) {
        toggleAutoscrollStartWithPlayhead();
        displayPreferencesContainer(true);
    } else if (buttonThatWasClicked == preferencesCloseButton.get()) {
        updatePreferencesColors();
        savePreferences();
        if (chordProForCurrentSong) {
            chordProProcessText(chordProEditor->getText());
        } else {
            preferencesContainer.setVisible(false);
            viewportRight.setViewedComponent(&extension->containerRight, false);
            containerRight.setVisible(true);
        }
    } else if (buttonThatWasClicked == preferencesButton.get()) {
        displayPreferencesContainer(true);
    } else if (buttonThatWasClicked == searchButton.get()) {
        fontButtonContainer.setVisible(false);
        transposeContainer.setVisible(false);
        displayAutoscrollContainer(false);
        displaySearchContainer(!searchContainer.isVisible());
        resized();
    } 
}

void ExtensionWindow::displayWindow(bool display) {
    if (!extension->prefsLoaded) {
        extension->pendingDisplayWindow = true;
        return;
    }
    if (extension->extensionWindow->isVisible() != display) {
        extension->extensionWindow->setVisible(display);
        lib->setWidgetValue(WIDGET_DISPLAY, (display == true ? 1.0 : 0.0));
    }
    if (display)
        extension->extensionWindow->toFront(true);
}

void ExtensionWindow::scrollWindow(double value) {
    Point<int> viewportPosition = extension->viewport.getViewPosition();
    Rectangle<int> viewportBounds = extension->viewport.getViewArea();
    Rectangle<int> containerBounds = extension->container.getBounds();
    int deltaH = containerBounds.getHeight() - viewportBounds.getHeight();
    deltaH = (deltaH < 0) ? 0 : deltaH;
    int newY = (int) (value * (double) deltaH);
    viewportPosition.setY(newY);
    extension->viewport.setViewPosition(viewportPosition);
}

void ExtensionWindow::initialize() {
    MessageManager::getInstance()->callAsync([]() {
        if (extension == nullptr) {
            extension = new ExtensionWindow();
            extension->extensionWindow->setTopLeftPosition(
                                            Rectangle<int>::fromString(DEFAULT_WINDOW_POSITION).getX(), 
                                            Rectangle<int>::fromString(DEFAULT_WINDOW_POSITION).getY()
                                        );
        }
        jassert(extension != nullptr);
        extension->extensionWindow->setVisible(false);
    });
}

void ExtensionWindow::finalize()
{
    savePreferences();
    if (extension != nullptr) {
        delete extension;
        extension = nullptr;
    }
}

void ExtensionWindow::processPreferencesDefaults(StringPairArray prefs) {
    setZeroBasedNumbering(prefs.getValue("ZeroBasedNumbers", "") == "true" ? true : false);
    setLargeScrollArea(prefs.getValue("LargeScrollArea", "") == "true" ? true : false);
    removeColorKeywordFromName(prefs.getValue("RemoveColorKeywordFromName", "") == "true" ? true : false); 
    extension->preferences->setProperty("ThickBorders", prefs.getValue("ThickBorders", "") == "true" ? true : false);
    extension->preferences->setProperty("BorderColor", prefs.getValue("BorderColor", DEFAULT_BORDER_COLOR));
    setDarkMode(prefs.getValue("DarkMode", "") == "true" ? true : false);
    headerSongColor = Colour::fromString(prefs.getValue("HeaderSongColor", HEADER_SONG_COLOR));
    setDisplayVariationNames(prefs.getValue("DisplayVariationNames", "") == "true" ? true : false); 
    setFitToPageHeight(prefs.getValue("FitToPageHeight", "") == "true" ? true : false); 
    setDisplayTwoPages(prefs.getValue("DisplayTwoPages", "") == "true" ? true : false); 
    String fontSize = prefs.getValue("FontSize", "0");
    chordProSetFontSize(fontSize == "0" ? CP_DEFAULT_FONT_SIZE : fontSize.getFloatValue());
    chordProSetLeftMarginLabels(prefs.getValue("LeftMarginLabels", "") == "true" ? true : false); 
    chordProSetSmallChordFont(prefs.getValue("SmallChordFont", "") == "true" ? true : false); 
    chordProSetAutoscrollPanelPersist(prefs.getValue("AutoscrollPanelPersist", "") == "true" ? true : false); 
    chordProSetAutoscrollStartWithPlayhead(prefs.getValue("AutoscrollStartWithPlayhead", "") == "true" ? true : false); 
    extension->updatePreferencesWindow();
}

void ExtensionWindow::processPreferencesColors(StringPairArray prefs) {
    extension->buttonColors.addArray(prefs);
}

void ExtensionWindow::processPreferencesChordProColors(StringPairArray prefs) {
    extension->preferencesChordProColors->setProperty("LightModeChords", prefs.getValue("LightModeChords", CP_LIGHT_CHORD_COLOR));
    extension->preferencesChordProColors->setProperty("LightModeLyrics", prefs.getValue("LightModeLyrics", CP_LIGHT_LYRIC_COLOR));
    extension->preferencesChordProColors->setProperty("LightModeBackground", prefs.getValue("LightModeBackground", CP_LIGHT_BACKGROUND_COLOR));
    extension->preferencesChordProColors->setProperty("DarkModeChords", prefs.getValue("DarkModeChords", CP_DARK_CHORD_COLOR));
    extension->preferencesChordProColors->setProperty("DarkModeLyrics", prefs.getValue("DarkModeLyrics", CP_DARK_LYRIC_COLOR));
    extension->preferencesChordProColors->setProperty("DarkModeBackground", prefs.getValue("DarkModeBackground", CP_DARK_BACKGROUND_COLOR));
    extension->chordProSetColors();
    extension->updatePreferencesWindow();
}

void ExtensionWindow::processPreferencesWindowState(StringPairArray prefs) {
    StringArray positionSize = StringArray::fromTokens(prefs.getValue("PositionAndSize", DEFAULT_WINDOW_POSITION), ",", "");
    bool extendedWindow = prefs.getValue("ExpandedWindow", "") == "false" ? false : true;
    int divider = prefs.getValue("SongListDivider", String(DEFAULT_SONG_LIST_WIDTH)).getIntValue();
    int dividerEditor = prefs.getValue("EditorDivider", String(CP_EDITOR_DEFAULT_X)).getIntValue();
    extension->extensionWindow->setTopLeftPosition(positionSize[0].getIntValue(), positionSize[1].getIntValue());
    extension->extensionWindow->setSize(positionSize[2].getIntValue(), positionSize[3].getIntValue());
    if (extendedWindow) {
        extension->displayRightPanel = true;
        extension->draggableResizer.setBounds(extension->draggableResizer.getBounds().withX(divider));
        extension->draggableResizerEditor.setBounds(extension->draggableResizerEditor.getBounds().withX(dividerEditor));
        extension->containerEditor.setBounds(dividerEditor, HEADER_HEIGHT, extension->getWidth() - dividerEditor, extension->getHeight() - HEADER_HEIGHT);
    }
    extension->prefsLoaded = true;
    if (extension->pendingDisplayWindow)
        displayWindow(true);
}

void ExtensionWindow::updatePreferencesWindow() {
     bool status = extension->preferences->getProperty("ZeroBasedNumbers");
     extension->prefButtons[0]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("LargeScrollArea");
     extension->prefButtons[1]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("ThickBorders");
     extension->prefButtons[2]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("DisplayVariationNames");
     extension->prefButtons[3]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("LeftMarginLabels");
     extension->prefButtons[4]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("SmallChordFont");
     extension->prefButtons[5]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("AutoscrollPanelPersist");
     extension->prefButtons[6]->setToggleState(status, dontSendNotification);
     status = extension->preferences->getProperty("AutoscrollStartWithPlayhead");
     extension->prefButtons[7]->setToggleState(status, dontSendNotification);

    // Preferences colour buttons
    String prefColorText;
    StringArray prefColors;
    prefColorText = extension->preferencesChordProColors->getProperty("LightModeChords").toString();
    prefColors.add(prefColorText);
    prefColorText = extension->preferencesChordProColors->getProperty("LightModeLyrics").toString();
    prefColors.add(prefColorText);
    prefColorText = extension->preferencesChordProColors->getProperty("DarkModeChords").toString();
    prefColors.add(prefColorText);
    prefColorText = extension->preferencesChordProColors->getProperty("DarkModeLyrics").toString();
    prefColors.add(prefColorText);
    for (int i = 0; i < 4; ++i) {
        prefColorButtons[i]->setColour (TextButton::buttonColourId, Colour::fromString (prefColors[i]));
    }
}

void ExtensionWindow::updatePreferencesColors() {
    extension->preferencesChordProColors->setProperty("LightModeChords", extension->prefColorButtons[0]->findColour(TextButton::buttonColourId).toString());
    extension->preferencesChordProColors->setProperty("LightModeLyrics", extension->prefColorButtons[1]->findColour(TextButton::buttonColourId).toString());
    extension->preferencesChordProColors->setProperty("DarkModeChords", extension->prefColorButtons[2]->findColour(TextButton::buttonColourId).toString());
    extension->preferencesChordProColors->setProperty("DarkModeLyrics", extension->prefColorButtons[3]->findColour(TextButton::buttonColourId).toString());
}

void ExtensionWindow::removeColorKeywordFromName(bool remove) {
    extension->preferences->setProperty("RemoveColorKeywordFromName", remove); 
}

void ExtensionWindow::updateClock(const String& formattedTime) {
    extension->clock->setText(formattedTime, dontSendNotification);
}

void ExtensionWindow::chordProScrollWindow(double value) {
    Point<int> viewportPosition = extension->viewportRight.getViewPosition();
    Rectangle<int> viewportBounds = extension->viewportRight.getViewArea();
    Rectangle<int> containerBounds = extension->chordProContainer.getBounds();
    int deltaH = containerBounds.getHeight() - viewportBounds.getHeight();
    deltaH = (deltaH < 0) ? 0 : deltaH;
    int newY = (int) (value * (double) deltaH);
    viewportPosition.setY(newY);
    extension->viewportRight.setViewPosition(viewportPosition);
}

void ExtensionWindow::chordProUp() {
    Rectangle<int> viewArea = extension->viewportRight.getViewArea();
    extension->viewportRight.setViewPosition(0, viewArea.getY() - viewArea.getHeight());

}

void ExtensionWindow::chordProDown() {
    Rectangle<int> viewArea = extension->viewportRight.getViewArea();
    extension->viewportRight.setViewPosition(0, viewArea.getY() + viewArea.getHeight());

}

void ExtensionWindow::chordProScrollToSongPart(std::string songPartName) {
    for (int i = 0; i < extension->chordProLines.size(); ++i) { 
        if (extension->chordProLines[i]->getProperties()["type"] == "gp_songpartname") {
            if (extension->chordProLines[i]->getText().toStdString() == songPartName) {
                Rectangle<int> buttonBounds = extension->chordProLines[i]->getBounds();
                extension->viewportRight.setViewPosition(0, buttonBounds.getY());
            }
        }
    }
}

void ExtensionWindow::chordProCalculateAutoScroll() {
    chordProPause.clear();
    chordProRunningPause = 0;
    chordProTotalPause = 0;
    int runningHeight = chordProTopPadding;
    // Locate and store the pause positions
    for (auto i = 0; i < chordProLines.size(); ++i) {
        if (chordProLines[i]->isVisible()) {
            runningHeight += chordProLines[i]->getHeight();
        } else if (chordProLines[i]->getProperties()["type"] == "pause") {
            int pauseLength = chordProLines[i]->getProperties()["pauseLength"].toString().getIntValue();
            chordProPause.push_back(std::make_pair(runningHeight, pauseLength * 1000));
            chordProTotalPause += (pauseLength * 1000);
        }
    }
    // Display duration label
    Time time = Time(extension->chordProSongScrollDuration);
    String label = time.formatted("%M:%S");
    extension->autoscrollDurationLabel->setText(label, dontSendNotification);

    // Add extra height (1/3 page) so final text isn't at bottom of the page
    Rectangle<int> viewportBounds = extension->viewportRight.getViewArea();
    Rectangle<int> containerBounds = extension->chordProContainer.getBounds();
    float pages = (float)containerBounds.getHeight() / (float)viewportBounds.getHeight();
    if (chordProLines.getLast()->getName() == "autoscrollSpacer") {
        auto bounds = chordProLines.getFirst()->getBounds();
        chordProLines.getLast()->setBounds(bounds.withHeight((containerBounds.getHeight() / pages) * 0.3));
        chordProLines.getLast()->setVisible(true);
        resized();
    }

}

void ExtensionWindow::chordProAutoScrollWindow(double scrollTimeValue) {
    Point<int> viewportPosition = extension->viewportRight.getViewPosition();
    // Check whether a pause position has been reached
    for (int i = 0; i < (int)extension->chordProPause.size(); ++i) { 
        if (viewportPosition.getY() >= extension->chordProPause[i].first) {
            extension->songScrollTimer.stopTimer();
            extension->chordProRunningPause += extension->chordProPause[i].second;
            extension->songScrollPauseTimer.startTimer(extension->chordProPause[i].second);
            extension->chordProPause.erase(extension->chordProPause.begin());
            extension->viewportRight.repaint(); // Avoids artefacts
            extension->songScrollPauseDisplayTimer.startTimer(TIMER_AUTOSCROLL_PAUSE_DISPLAY);
            return;
        }
    }
    chordProScrollWindow(scrollTimeValue);
    if (scrollTimeValue >= 1.0) {
        extension->chordProAutoScrollPlay(false);
    }
}

void ExtensionWindow::chordProAutoScrollPlay(bool play) {
    if (play == autoscrollPlaying) return;
    if (!extension->chordProForCurrentSong) return;

    autoscrollPlaying = play;
    extension->autoscrollPlayButton->setToggleState(play, dontSendNotification);
    Point<int> viewportPosition = extension->viewportRight.getViewPosition();

    // Synchronize the autoscroll play widget
    int playValue = play ? 1.0 : 0.0;
    if (lib->getWidgetValue(WIDGET_CP_AUTOSCROLL_PLAY) != playValue) {
        lib->setWidgetValue(WIDGET_CP_AUTOSCROLL_PLAY, playValue);
    } 

    if (play) {
        extension->songScrollPauseDisplayTimer.stopTimer();
        extension->autoscrollTimeLabel->setVisible(true);
        if (viewportPosition.getY() > 0) {
            extension->chordProScrollStart = extension->chordProScrollStart + (juce::Time::getCurrentTime().toMilliseconds() - extension->chordProScrollPaused);
        } else {
            extension->chordProCalculateAutoScroll();
            extension->chordProScrollStart = juce::Time::getCurrentTime().toMilliseconds();
        }
        extension->songScrollTimer.startTimer(TIMER_AUTOSCROLL);
    } else {
        extension->chordProScrollPaused = juce::Time::getCurrentTime().toMilliseconds();
        extension->chordProAutoScrollStopTimers();
        Rectangle<int> viewportBounds = extension->viewportRight.getViewArea();
        Rectangle<int> containerBounds = extension->chordProContainer.getBounds();
        if (viewportPosition.getY() > 0 && (viewportPosition.getY() + viewportBounds.getHeight() < containerBounds.getHeight())) {
            extension->songScrollPauseDisplayTimer.startTimer(TIMER_AUTOSCROLL_PAUSE_DISPLAY);
        }
    }
}

void ExtensionWindow::chordProAutoScrollReset() {
    extension->viewportRight.setViewPosition(0,0);
    extension->autoscrollPlayButton->setToggleState(false, dontSendNotification);
    lib->setWidgetValue(WIDGET_CP_AUTOSCROLL_PLAY, 0.0);
    extension->chordProAutoScrollStopTimers();
    extension->songScrollPauseDisplayTimer.stopTimer();
    extension->autoscrollTimeLabel->setVisible(true);
    extension->autoscrollTimeLabel->setText("00:00", dontSendNotification);
    autoscrollPlaying = false;
}

void ExtensionWindow::chordProAutoScrollStopTimers() {
    if (extension->songScrollTimer.isTimerRunning()) extension->songScrollTimer.stopTimer();
    if (extension->songScrollPauseTimer.isTimerRunning()) extension->songScrollPauseTimer.stopTimer();
}

void ExtensionWindow::updateAutoscrollTime(Time time) {
    String label = time.formatted("%M:%S");
    if (label != extension->autoscrollTimeLabel->getText()) {
        extension->autoscrollTimeLabel->setText(label, dontSendNotification);
    }
}

void ExtensionWindow::flashAutoscrollTime(bool flash) {
    if (flash) {
        extension->autoscrollTimeLabel->setVisible(!extension->autoscrollTimeLabel->isVisible());
        if (extension->songScrollPauseTimer.isTimerRunning()) {
            int64 time = juce::Time::getCurrentTime().toMilliseconds();
            int elapsed = time - ExtensionWindow::extension->chordProScrollStart;
            ExtensionWindow::extension->updateAutoscrollTime(Time(elapsed));
        }
    } else {
        extension->songScrollPauseDisplayTimer.stopTimer();
        extension->autoscrollTimeLabel->setVisible(true);
    }
}

void ExtensionWindow::chordProProcessText(String text) {
    StringArray lines = StringArray::fromLines(text);
    String line;
    int firstLineWithContent = false;
    int keyboardDiagramCount = 0;
    int fretboardDiagramCount = 0;
    int lyricChordRowCount = 0;
    int transpose = 0;
    StringArray directiveParts;
    String directiveName;
    String directiveValue;
    bool tabLine = false;
    bool gridLine = false;
    int gridMaxChordLength = 0;
    int gridMaxBeats = 0;
    int gridMaxBeatsWithChords = 0;
    bool chorusLine = false;
    String sectionLabel;
    bool firstSectionLine = false;
    extension->chordProReset();
    bool chordProNonImage = false;

    for (int i = 0; i < lines.size() + 1; ++i) { 
        line = lines[i].toStdString();
        extension->addChordProLine();
        if (line.trim() != "") firstLineWithContent = true;
        if (firstLineWithContent) {   
            if (line.contains("{")) { // Directive
                extension->chordProLines[i]->getProperties().set("type", "directive"); 
                line = line.removeCharacters("{}");
                if (line.contains(":")) {
                    directiveParts = StringArray::fromTokens(line,":",""); // Split directive
                    directiveName = directiveParts[0].removeCharacters(" ").toLowerCase();
                    directiveValue = directiveParts[1].trim();
                    if (directiveName == "title" || directiveName == "t") {
                        extension->chordProLines[i]->setLookAndFeel(extension->chordProTitleLnF);
                        extension->chordProLines[i]->getProperties().set("type", "title"); 
                    } else if (directiveName == "subtitle" || directiveName == "st") {
                        extension->chordProLines[i]->setLookAndFeel(extension->chordProSubTitleLnF);
                        extension->chordProLines[i]->getProperties().set("type", "subtitle"); 
                    } else if (directiveName == "comment" || directiveName == "c" || directiveName == "comment_italic" || directiveName == "ci" || directiveName == "comment_box" || directiveName == "cb" || directiveName == "highlight") {
                        extension->chordProLines[i]->setLookAndFeel(extension->chordProCommentLnF);
                        extension->chordProLines[i]->getProperties().set("type", "comment"); 
                    } else if (directiveName == "songpartname") {
                        extension->chordProLines[i]->setVisible(false);
                        extension->chordProLines[i]->getProperties().set("type", "gp_songpartname"); 
                    } else if (directiveName.contains("start_of") || directiveName.contains("end_of") || (directiveName.length() == 3 && (directiveName.contains("so") || directiveName.contains("eo")))) {
                        extension->chordProLines[i]->setLookAndFeel(extension->chordProLabelLnF);
                        extension->chordProLines[i]->getProperties().set("type", "label"); 
                        extension->chordProLines[i]->setVisible(!chordProLeftLabels);
                    } else if (directiveName == "image") {
                            #if JUCE_WINDOWS
                                if (directiveParts.size() == 3) { // File path had a drive letter e.g. C:
                                    directiveValue = directiveParts[1].trim() + ":" + directiveParts[2].trim();
                                }  
                            #endif
                            String path = directiveValue.removeCharacters("\"");
                            bool isValid = extension->addChordProImage(path);
                            if (isValid) {
                                extension->chordProLines[i]->getProperties().set("imageIndex", extension->chordProImages.size() - 1);
                                extension->chordProLines[i]->getProperties().set("type", "image");  
                            }
                            directiveValue = "";
                    } else if (directiveName == "define") {
                        StringArray definition = StringArray::fromTokens(directiveValue, false);
                        String chord = definition[0];
                        StringArray chordParts = StringArray::fromTokens(chord,"/","");
                        chordParts.removeEmptyStrings();
                        String root;

                        if (definition[1] == "keys") {
                            if (keyboardDiagramCount == 0) { // First line. All diagrams will be displayed.
                                extension->chordProLines[i]->setVisible(true);
                                extension->chordProLines[i]->getProperties().set("type", "diagramKeyboard"); 
                                directiveValue = "";

                            } else {
                                extension->chordProLines[i]->setVisible(false);
                            }
                            definition.removeRange(0,2); // Leave only notes
                            extension->addChordProDiagramKeyboard();
                            extension->chordProDiagramKeyboard[keyboardDiagramCount]->updateChord(chord, definition);
                            extension->chordProDiagramKeyboard[keyboardDiagramCount]->updateChordDiagram(chordProTranspose, chordProTransposeDisplay);
                            extension->chordProDiagramKeyboard[keyboardDiagramCount]->updateKeyOnColour(chordProChordColor);
                            keyboardDiagramCount++;
                        } else if (definition[1].contains("fret")) {
                            if (fretboardDiagramCount == 0) { // First line. All diagrams will be displayed.
                                extension->chordProLines[i]->setVisible(true);
                                extension->chordProLines[i]->getProperties().set("type", "diagramFretboard"); 
                                directiveValue = "";
                            } else {
                                extension->chordProLines[i]->setVisible(false);
                            }
                            
                            int fretsIndex = definition.indexOf("frets", true, 1);
                            if (fretsIndex > 0) {
                                StringArray frets;
                                for (int j = fretsIndex + 1; j < fretsIndex + 7; ++j) {
                                    if (j < definition.size())
                                        frets.add(definition[j]);
                                }
                                if (frets.size() == 6) { // Valid diagram
                                    int baseIndex = definition.indexOf("base-fret", true, 1);
                                    if (baseIndex > 0) { // Append to fret list
                                        frets.add(definition[baseIndex + 1]);
                                    }
                                    extension->addChordProDiagramFretboard();
                                    extension->chordProDiagramFretboard[fretboardDiagramCount]->updateChord(chord, frets);
                                    extension->chordProDiagramFretboard[fretboardDiagramCount]->updateChordDiagram(chordProTranspose, chordProTransposeDisplay);
                                    fretboardDiagramCount++;
                                }
                            }
                        }
                    } else if (directiveName == "x_gp_transpose_accidental") {
                        if (directiveValue.toLowerCase() == "flat") {
                            chordProTransposeDisplay = flat;
                        } else if (directiveValue.toLowerCase() == "sharp") {
                            chordProTransposeDisplay = sharp;
                        }
                        extension->chordProLines[i]->setVisible(false);
                    } else if (directiveName == "transpose") {
                        transpose = directiveValue.getIntValue();
                        extension->chordProLines[i]->setVisible(false);
                    } else if (directiveName  == "duration" || directiveName  == "x_gp_duration") {
                        extension->chordProSongScroll = true;
                        StringArray durationParts = StringArray::fromTokens(line,":",""); // Split minutes and seconds
                        int seconds = 0;
                        if (durationParts.size() == 2) { // Seconds only
                            seconds = durationParts[1].trim().getIntValue();
                        } else  {
                            seconds = durationParts[1].trim().getIntValue() * 60; // Minutes
                            seconds += durationParts[2].trim().getIntValue(); // Add seconds
                        }
                        extension->chordProSongScrollDuration = seconds * 1000; // Save as milliseconds
                        extension->chordProLines[i]->setVisible(false);

                    } else if (directiveName  == "pause" ||directiveName == "x_gp_pause") {
                        extension->chordProLines[i]->getProperties().set("type", "pause");
                        extension->chordProLines[i]->getProperties().set("pauseLength", directiveValue);
                        extension->chordProLines[i]->setVisible(false);
                    } else {
                        extension->chordProLines[i]->setLookAndFeel(extension->chordProSubTitleLnF);
                        extension->chordProLines[i]->getProperties().set("type", "other"); 
                        directiveValue = directiveName.substring(0,1).toUpperCase() + directiveName.substring(1,directiveName.length()) + ": " + directiveValue;
                        extension->chordProLines[i]->setVisible(false);
                    } 
                } else {
                    extension->chordProLines[i]->setVisible(false);
                    directiveName = line.removeCharacters(" ");
                    directiveValue = "";
                }
                if (directiveName == "start_of_tab" || directiveName == "sot") {
                    tabLine = true;
                    firstSectionLine = true;
                    sectionLabel = directiveValue;
                } else if (directiveName == "end_of_tab" || directiveName == "eot") {
                    tabLine = false;
                } else if (directiveName.startsWith("start_of_grid") || directiveName.startsWith("sog")) {
                    gridLine = true;
                    firstSectionLine = true;
                    sectionLabel = directiveValue;
                } else if (directiveName == "end_of_grid" || directiveName == "eog") {
                    gridLine = false;
                } else if (directiveName == "start_of_chorus" || directiveName == "soc") {
                    chorusLine = true;
                    firstSectionLine = true;
                    sectionLabel = directiveValue == "" ? "Chorus" : directiveValue;
                } else if (directiveName == "end_of_chorus" || directiveName == "eoc") {
                    chorusLine = false;
                } else if (directiveName == "start_of_verse" || directiveName == "sov") {
                    firstSectionLine = true;
                    sectionLabel = directiveValue == "" ? "Verse" : directiveValue;
                } else if (directiveName.startsWith("start_of_")) {
                    firstSectionLine = true;
                    sectionLabel = directiveValue;
                } else {
                    firstSectionLine = false;
                }
                extension->chordProLines[i]->setText(directiveValue, dontSendNotification);
                extension->chordProLines[i]->getProperties().set("originalText", directiveValue); 
            } else { // Not directive line
                if (tabLine) {
                    extension->chordProLines[i]->setLookAndFeel(extension->chordProTabLnF);
                    extension->chordProLines[i]->getProperties().set("type", "tab");
                } else if (gridLine) {
                    extension->chordProLines[i]->setLookAndFeel(extension->chordProGridLnF);
                    extension->chordProLines[i]->getProperties().set("type", "grid");

                    // Store any grid comment and then remove from grid line
                    String postComment = line.substring(line.lastIndexOf("|") + 1, line.length()).trim(); 
                    extension->chordProLines[i]->getProperties().set("gridComment", postComment);
                    line = line.substring(0, line.lastIndexOf("|") + 1); // Exclude comments after the bar

                    // Calculate maximum character length of each bar
                    String temp = line;
                    temp = temp.replace("||","|").replace("|:","|").replace(":|","|"); // Convert special characters to single character
                    StringArray parts = StringArray::fromTokens(temp,"|","");
                    parts.removeEmptyStrings(true);
                    for (int j = 0; j < parts.size(); ++j) {
                        StringArray chords = StringArray::fromTokens(parts[j], false);
                        chords.removeEmptyStrings(true);
                        if (chords[0].getIntValue() > 0) { // Check and remove volta
                            chords.remove(0);
                        }
                        if (chords.size() > gridMaxBeats) gridMaxBeats = chords.size();
                        chords.removeString(".");
                        if (chords.size() > gridMaxBeatsWithChords) gridMaxBeatsWithChords = chords.size();
                        for (int k = 0; k < chords.size(); ++k) {
                            int length = juce::jmax(chords[k].length(), chords[k].contains("/") ? 7 : 3); // Allow a minimum of 3 characters for every chord to account for transposition
                            if (length > gridMaxChordLength) gridMaxChordLength = length;
                        }
                    }
                    extension->chordProLines[i]->getProperties().set("gridBars", parts.size());
                    lyricChordRowCount++;
                    // Other processing to ensure contents are space separated
                    line = line.replace("|"," | ").replace("."," . ");
                    line = line.replace("|  |","||").replace("| :","|:").replace(": |",":|");
                } else if (chorusLine) {
                    extension->chordProLines[i]->getProperties().set("section", "chorus"); 
                } 
                if (firstSectionLine) {
                    extension->chordProLines[i]->getProperties().set("sectionLabel", sectionLabel);
                    sectionLabel = "";
                    firstSectionLine = false;
                }
                if (line.contains("[")) {
                    String excludeChords = std::regex_replace(line.toStdString(), std::regex("\\[(.*?)\\]"), "");
                    if (excludeChords.trim() == "") {
                        extension->chordProLines[i]->getProperties().set("type", "chordOnly"); 
                        StringArray words = StringArray::fromTokens(lines[i],false);
                        for (int j = 0; j < words.size(); ++j) { 
                             String word = words[j].toStdString();
                            if  (word != "") {
                                if (!word.startsWith("[")) {
                                    word = "[" + word;
                                }
                                if (!word.endsWith("]")) {
                                    word = word + "]";
                                }  
                                words.set(j, word);
                            }
                        }
                        line = words.joinIntoString(" ", 0, -1);
                    } else {
                        extension->chordProLines[i]->getProperties().set("type", "chordAndLyrics"); 
                        // Minor cleanup for chords & lyrics
                        line = std::regex_replace(line.toStdString(), std::regex("\\  +"), " "); // Remove multiple spaces
                        StringArray parts = StringArray::fromTokens(line," ","");
                        String newLine;
                        parts.removeEmptyStrings(true);
                        for (int j = 0; j < parts.size(); ++j) {
                            if (parts[j].substring(0,1) == "[" && parts[j].substring(parts[j].length()-1,parts[j].length()) == "]") {
                                 // Remove space after chord but only if a space precedes the chord
                                newLine += parts[j];
                            } else {
                                newLine += (parts[j] + " ");
                            }
                        }
                        line = newLine;
                    }
                    lyricChordRowCount++;
                } else if (line.trim() != "" && !gridLine && !tabLine){
                    extension->chordProLines[i]->getProperties().set("type", "lyricOnly"); 
                    lyricChordRowCount++;
                }
                extension->chordProLines[i]->setText(line.trim(), dontSendNotification);
                extension->chordProLines[i]->getProperties().set("originalText", line.trim()); 
            }
        }
        if (!(directiveName == "image" || directiveName == "songpartname" || (extension->chordProLines[i]->getProperties()["type"] == ""))) {
            chordProNonImage = true;
        }
    }
    if (!chordProNonImage) extension->chordProImagesOnly = true;
    if (gridMaxChordLength > 0) {
        for (int i = 0; i < extension->chordProLines.size(); ++i) { 
            if (extension->chordProLines[i]->getProperties()["type"] == "grid") {
                extension->chordProLines[i]->getProperties().set("gridBarLength", gridMaxChordLength * gridMaxBeatsWithChords);
                extension->chordProLines[i]->getProperties().set("gridBeats", gridMaxBeats);
                extension->chordProLines[i]->getProperties().set("gridBeatsWithChords", gridMaxBeatsWithChords);
            }
        }
    }
    // Duplicate blank row suppression and row insertion before sections
    int lastBlank = -1;
    int rowCounter = 0;
    for (int i = 0; i < extension->chordProLines.size(); ++i) { 
        if (extension->chordProLines[i]->isVisible()) {
            if (extension->chordProLines[i]->getText() == "" && extension->chordProLines[i]->getProperties()["type"] != "image" && extension->chordProLines[i]->getProperties()["type"] != "diagramKeyboard" && extension->chordProLines[i]->getProperties()["type"] != "diagramFretboard") {
                if (rowCounter - 1 == lastBlank) {
                    extension->chordProLines[i]->setVisible(false);
                 } 
                lastBlank = rowCounter;
            } else if (((extension->chordProLines[i]->getProperties()["type"] == "label" || extension->chordProLines[i]->getProperties()["type"] == "diagramKeyboard" || extension->chordProLines[i]->getProperties()["type"] == "diagramFretboard" ) && rowCounter - 1 != lastBlank)
                        || (extension->chordProLines[i]->getProperties()["sectionLabel"] != "" && chordProLeftLabels && rowCounter - 1 != lastBlank)) {
                extension->addChordProLine(i-1);
                extension->chordProLines[i - 1]->getProperties().set("type", "blankLine"); 
                i++;
            }
            rowCounter++;
        }
    }
    // Estimate duration if not defined in the file
    if (!extension->chordProSongScroll) {
        int seconds = (lyricChordRowCount * 16) / lib->getSongTempo(lib->getCurrentSongIndex()) * 60;
        extension->chordProSongScroll = true;
        extension->chordProSongScrollDuration = juce::jmax(seconds, 120)  * 1000;
        extension->autoscrollDurationLabel->getProperties().set("source","estimate");
    }
    extension->chordProCalculateAutoScroll();
    extension->chordProDisplayGUI(true);
    extension->chordProSetTranspose(transpose);

    // Extra line for autoscroll padding at end
    extension->addChordProLine(extension->chordProLines.size(), "autoscrollSpacer");
    extension->chordProLines.getLast()->setVisible(true);
}

void ExtensionWindow::chordProReadFile(int index) {
    if (extension == nullptr) return;
    String chordProFileText;
    std::string chordProFile = lib->getChordProFilenameForSong(index);
    extension->chordProForCurrentSong = (chordProFile == "") ? false : true;
    extension->editorTextChangedCount = 0;
    extension->editorTextEdited = false;
    chordProTransposeDisplay = original;
    chordProSetTranspose(0);
    if (extension->chordProForCurrentSong) {
        File chordProFullPath = File(chordProFile);
        if (chordProFullPath.existsAsFile()) {
            chordProFileText = chordProFullPath.loadFileAsString();
            chordProProcessText(chordProFileText);  
            extension->chordProEditor->setText(chordProFileText, false);
            extension->noChordProLabel->setVisible(false);
            extension->viewportRight.setViewPosition(0,0);
        } else {
            extension->chordProForCurrentSong = false;
        }
    }
    if (!extension->chordProForCurrentSong) {
        extension->chordProDisplayGUI(false);
        extension->chordProReset();
    }
}

void ExtensionWindow::chordProReset() {
    extension->chordProLines.clear();
    extension->chordProImages.clear();
    extension->chordProDiagramKeyboard.clear();
    extension->chordProDiagramFretboard.clear();
    missingImageContainer.setVisible(false);
    extension->chordProImagesOnly = false;
    extension->chordProSongScroll = false;
    extension->noChordProLabel->setVisible(true);
    extension->autoscrollDurationLabel->getProperties().clear();
    extension->chordProSongScrollDuration = 0;
    extension->chordProAutoScrollReset();
}

void ExtensionWindow::chordProDisplayGUI(bool display) { 
    fontButton->setVisible(display && displayRightPanel && !chordProImagesOnly);
    transposeButton->setVisible(display && displayRightPanel && !chordProImagesOnly);
    editButton->setVisible(display && displayRightPanel);
    lightDarkModeButton->setVisible(display && displayRightPanel);
    columnsOneButton->setVisible(display && displayRightPanel && chordProImagesOnly && chordProTwoColumns);
    columnsTwoButton->setVisible(display && displayRightPanel && chordProImagesOnly && !chordProTwoColumns);
    fitWidthButton->setVisible(display && displayRightPanel && chordProImagesOnly && fitHeight);
    fitHeightButton->setVisible(display && displayRightPanel && chordProImagesOnly && !fitHeight);
    autoscrollButton->setVisible(display && displayRightPanel);

    displayPreferencesContainer(false);
    fontButtonContainer.setVisible(false);
    transposeContainer.setVisible(false);
    displaySearchContainer(false);
    if (!autoscrollPanelPersist) displayAutoscrollContainer(false);
    if (display) { 
        extension->viewportRight.setViewedComponent(&extension->chordProContainer, false);
        extension->chordProContainer.setVisible(true);
        extension->chordProSetColors();
        extension->chordProSetTranspose(chordProTranspose);
        chordProTwoColumnsExtern = extension->chordProTwoColumns && !extension->chordProImagesOnly;
    } else {
        missingImageContainer.setVisible(false);
        if (!chordProForCurrentSong) {
            extension->viewportRight.setViewedComponent(&extension->containerRight, false);
            extension->containerRight.setVisible(true);
            displayEditorContainer(false);
            viewPortBackground = Colour::fromString(BACKGROUND_COLOR);
        }
    }
    extension->resized();
}

void ExtensionWindow::chordProSetColors() { 
    Colour backgroundColor;
    if (chordProDarkMode) {
        chordProLyricColor = Colour::fromString(extension->preferencesChordProColors->getProperty("DarkModeLyrics").toString());
        chordProChordColor = Colour::fromString(extension->preferencesChordProColors->getProperty("DarkModeChords").toString());
        backgroundColor = Colour::fromString(extension->preferencesChordProColors->getProperty("DarkModeBackground").toString());

        if (chordProImagesOnly) {
            viewPortBackground = Colour::fromString(CP_DARK_IMAGES_BACKGROUND_COLOR);
        } else {
            viewPortBackground = backgroundColor;
        }
    } else {
        chordProLyricColor = Colour::fromString(extension->preferencesChordProColors->getProperty("LightModeLyrics").toString());
        chordProChordColor = Colour::fromString(extension->preferencesChordProColors->getProperty("LightModeChords").toString());
        backgroundColor = Colour::fromString(extension->preferencesChordProColors->getProperty("LightModeBackground").toString());

        if (chordProImagesOnly) {
            viewPortBackground = Colour::fromString(CP_LIGHT_IMAGES_BACKGROUND_COLOR);
        } else {
            viewPortBackground = backgroundColor;
        }
    }
    extension->containerEditor.setBackgroundColour(backgroundColor);
    chordProUpdateDiagramColors();
}

void ExtensionWindow::chordProUpdateDiagramColors() {
    for (int i = 0; i < extension->chordProDiagramKeyboard.size(); ++i) {
        extension->chordProDiagramKeyboard[i]->updateKeyOnColour(chordProChordColor);
        extension->chordProDiagramKeyboard[i]->repaint();
    }
}

void ExtensionWindow::chordProSetFontSize(float newSize) {
    extension->preferences->setProperty("FontSize", newSize);    
    chordProFontSize = newSize;
    extension->resized();
}

void ExtensionWindow::chordProSetSmallChordFont(bool isSmall) {
    extension->preferences->setProperty("SmallChordFont", isSmall);
    chordProSmallChordFont = isSmall;
}

void ExtensionWindow::chordProSetAutoscrollPanelPersist(bool persist) {
    extension->preferences->setProperty("AutoscrollPanelPersist", persist);
    autoscrollPanelPersist = persist;
}

void ExtensionWindow::chordProSetAutoscrollStartWithPlayhead(bool start) {
    extension->preferences->setProperty("AutoscrollStartWithPlayhead", start);
    autoscrollStartWithPlayhead = start;
}

void ExtensionWindow::chordProSetTranspose(int transpose) {
    chordProTranspose = transpose;
    String label = "Transpose ";
    if (chordProTranspose > 0) {
        label += "+";
    } else if (chordProTranspose == 0) {
        label += " ";
    }
    label += (String)std::to_string(chordProTranspose);
    extension->transposeLabel->setText(label,dontSendNotification);

    for (int i = 0; i < extension->chordProDiagramKeyboard.size(); ++i) {
        extension->chordProDiagramKeyboard[i]->updateChordDiagram(chordProTranspose, chordProTransposeDisplay);
        extension->chordProDiagramKeyboard[i]->repaint();
    }

    for (int i = 0; i < extension->chordProDiagramFretboard.size(); ++i) {
        extension->chordProDiagramFretboard[i]->updateChordDiagram(chordProTranspose, chordProTransposeDisplay);
        extension->chordProDiagramFretboard[i]->repaint();
    }
    extension->repaint();
}

void ExtensionWindow::chordProSetImageDarkMode(bool darkMode) {
    bool missingDarkModeImage = false;
    for (auto i = 0; i < extension->chordProImages.size(); ++i) {
        String path = extension->chordProImages[i]->getProperties()["path"];
        File file = File(path);
        Image image = ImageFileFormat::loadFrom(file);
        if (image.isValid()) {
            if (darkMode) {
                String pathDarkMode = file.getFileNameWithoutExtension() + CP_DARK_MODE_FILE_SUFFIX + file.getFileExtension();
                File fileDarkMode = file.getParentDirectory().getChildFile(pathDarkMode);
                Image imageDarkMode = ImageFileFormat::loadFrom(fileDarkMode);
                if (imageDarkMode.isValid()) {
                    extension->chordProImages[i]->setImage(imageDarkMode);
                } else {
                    missingDarkModeImage = true;
                }
            } else {
                extension->chordProImages[i]->setImage(image);
            }
        }
    }
    if (missingDarkModeImage) {
        extension->missingImageContainer.setVisible(true);
    }
}

void ExtensionWindow::addChordProLine(int atIndex, String name) {
    int index = atIndex >= 0 ? atIndex : chordProLines.size();
    auto label = new Label(name, ""); 
    chordProLines.insert(index, label);
    chordProLines[index]->setEditable (false, false, false);
    chordProLines[index]->setLookAndFeel(chordProLnF);
    chordProLines[index]->setInterceptsMouseClicks(false, false);
    chordProContainer.addAndMakeVisible(chordProLines[index]);
}

bool ExtensionWindow::addChordProImage(String path) {
    bool isValid = false;
    File file = File(path);
    Image image;
    if (chordProDarkMode) {
        String pathDarkMode = file.getFileNameWithoutExtension() + CP_DARK_MODE_FILE_SUFFIX + file.getFileExtension();
        File fileDarkMode = file.getParentDirectory().getChildFile(pathDarkMode);
        image = ImageFileFormat::loadFrom(fileDarkMode);
        if (!image.isValid()) { // Attempt to load original
            image = ImageFileFormat::loadFrom(file);
            if (image.isValid()) {
                missingImageContainer.setVisible(true);
            }
        }
    } else {
        image = ImageFileFormat::loadFrom(file);
    }
    if (image.isValid()) {
        isValid = true;
        int index = chordProImages.size();
        auto imageComponent = new ImageComponent(std::to_string(index));
        chordProImages.add(imageComponent);
        chordProImages[index]->setImage(image);
        chordProImages[index]->setImagePlacement(RectanglePlacement (RectanglePlacement::fillDestination | RectanglePlacement::xLeft | RectanglePlacement::yTop));
        chordProImages[index]->setVisible(true);
        chordProImages[index]->getProperties().set("path", file.getFullPathName());
        chordProImages[index]->setInterceptsMouseClicks(false, false);
        chordProContainer.addAndMakeVisible(chordProImages[index]);
    } else {
        log("Image Not Found: " + file.getFullPathName());
    }
    return isValid;
}

void ExtensionWindow::addChordProDiagramKeyboard() {
    int index = chordProDiagramKeyboard.size();
    auto diagram = new ChordDiagramKeyboard();
    chordProDiagramKeyboard.add(diagram);
    chordProDiagramKeyboard[index]->setDarkMode(chordProDarkMode);
    chordProContainer.addAndMakeVisible(chordProDiagramKeyboard[index]);
}

void ExtensionWindow::addChordProDiagramFretboard() {
    int index = chordProDiagramFretboard.size();
    auto diagram = new ChordDiagramFretboard();
    chordProDiagramFretboard.add(diagram);
    chordProDiagramFretboard[index]->setDarkMode(chordProDarkMode);
    chordProContainer.addAndMakeVisible(chordProDiagramFretboard[index]);
}

int ExtensionWindow::chordProGetVisibleImageCount() {
    int visible = 0;
    for (auto i = 0; i < extension->chordProImages.size(); ++i) {
        if (extension->chordProImages[i]->isVisible()) {
            ++visible;
        }
    }
    return visible;
}

void ExtensionWindow::chordProCreateInvertedImages() {
    for (int i = 0; i < extension->chordProImages.size(); ++i) { 
        if (extension->chordProImages[i]->isVisible()) {
            String path = extension->chordProImages[i]->getProperties()["path"];
            if (!path.contains(CP_DARK_MODE_FILE_SUFFIX)) {
                File file = File(path);
                Image image = ImageFileFormat::loadFrom(file);
                if (image.isValid()) {
                    for(int y = 0; y < image.getHeight(); y++) {
                        for(int x = 0; x < image.getWidth(); x++) {
                            Colour col = image.getPixelAt(x, y);
                            uint8 r = 255 - col.getRed();
                            uint8 g = 255 - col.getGreen();
                            uint8 b = 255 - col.getBlue();
                            image.setPixelAt(x, y, Colour::fromRGB(r, g, b));
                        }   
                    }
                    image = image.getClippedImage(image.getBounds().withTrimmedTop(1));
                    String pathDarkMode = file.getFileNameWithoutExtension() + CP_DARK_MODE_FILE_SUFFIX + file.getFileExtension();
                    File fileDarkMode = file.getParentDirectory().getChildFile(pathDarkMode);
                    auto destStream = fileDarkMode.createOutputStream();
                    PNGImageFormat pngimage;
                    pngimage.writeImageToStream(image, *destStream);
                    extension->chordProImages[i]->setImage(image);
                    extension->chordProImages[i]->getProperties().set("path", fileDarkMode.getFullPathName()); 
                }
            }
        }
    }
    extension->missingImageContainer.setVisible(false);
    extension->missingImageLabel->setText(CP_NO_DARK_MODE_IMAGES, dontSendNotification);
    extension->createInvertedImage->setVisible(true);
}

void ExtensionWindow::chordProEditorUpdate() {
    extension->editorTextEdited = true;
    extension->chordProProcessText(extension->chordProEditor->getText());
}

void ExtensionWindow::log(String text) {
    if (lib == nullptr) return;
    lib->consoleLog(LOG_PREFIX + text.toStdString());
}

void ExtensionWindow::setSongLabel() {
    int songIndex = lib->inSetlistMode() ? lib->getCurrentSongIndex() : extension->getButtonSelected();
    std::string songName = lib->getSongName(songIndex);
    extension->header->setText(songName, dontSendNotification);
}

void ExtensionWindow::displayFontContainer(bool display) {
    extension->fontButtonContainer.setVisible(display);
}

void ExtensionWindow::displayAutoscrollContainer(bool display) {
    extension->autoscrollContainer.setVisible(display);
}

void ExtensionWindow::displayTransposeContainer(bool display) {
    extension->transposeContainer.setVisible(display);
}

void ExtensionWindow::displaySearchContainer(bool display) {
    extension->searchBox->setText("", dontSendNotification);
    extension->searchContainer.setVisible(display);
    extension->searchText = "";
    
    if (display) {
        extension->caratTimer.startTimer(TIMER_CARAT);
    } else {
        extension->caratTimer.stopTimer();
    }
    if (!extension->displaySongPanel)
        extension->setSongPanelToFloating(display);
}

void ExtensionWindow::displayPreferencesContainer(bool display) {
    if (display) {
        extension->chordProContainer.setVisible(false);
        extension->containerRight.setVisible(false);
        displayFontContainer(false);
        displayTransposeContainer(false);
        displaySearchContainer(false);
        displayEditorContainer(false);
        if (!autoscrollPanelPersist) displayAutoscrollContainer(false);
        viewPortBackground = Colour::fromString(BACKGROUND_COLOR);
    } else {
        extension->updatePreferencesColors();
    }
    extension->preferencesContainer.setVisible(display);
}

void ExtensionWindow::displayEditorContainer(bool display) {
    extension->containerEditor.setVisible(display);
    extension->draggableResizerEditor.setVisible(display);
}

void ExtensionWindow::displaySetlistContainer(bool display) {
    extension->viewport.setViewedComponent(display ? &extension->setlistContainer : &extension->container, false);
}

void ExtensionWindow::toggleSetlistContainer() {
    bool display = false;
    bool floatingSongList = (extension->searchContainer.isVisible() && !extension->displaySongPanel);
    if (floatingSongList) {
        display = (extension->floatingViewport.getViewedComponent() == &extension->container);
        extension->floatingViewport.setViewedComponent(display ? &extension->setlistContainer : &extension->container, false);
    } else {
        display = (extension->viewport.getViewedComponent() == &extension->container);
        extension->viewport.setViewedComponent(display ? &extension->setlistContainer : &extension->container, false);
    }
}

void ExtensionWindow::setSongPanelPosition(bool display) {
    auto bounds = extension->draggableResizer.getBounds();
    int newX = display ? DEFAULT_SONG_LIST_WIDTH : 0;
    int newWidth = display ? DEFAULT_SONG_LIST_WIDTH - 10 : 0;
    extension->draggableResizer.setBounds(bounds.withX(newX));
    extension->container.setBounds(extension->container.getBounds().withWidth(newWidth));
    extension->sidePanelCloseButton->setVisible(display);
    extension->sidePanelOpenButton->setVisible(!display);
    extension->resized();
}

void ExtensionWindow::checkSongListPosition() {
    String headerLabel = extension->header->getText();
    Font headerLabelFont = extension->header->getFont();
    int headerLabelWidth = headerLabelFont.getStringWidth(headerLabel);
    bool display = extension->draggableResizer.getX() > 0;
    if (display != extension->displaySongPanel) {
        extension->sidePanelCloseButton->setVisible(display && extension->sidePanelCloseButton->getX() > headerLabelWidth + 20);
        extension->sidePanelOpenButton->setVisible(!display && extension->sidePanelOpenButton->getX() > headerLabelWidth + 20);
        extension->displaySongPanel = display;
        extension->resized();
    }
}

void ExtensionWindow::songSearch(String text, bool append = false) {
    if (!extension->searchContainer.isVisible()) return;
    if (append) {
        extension->searchText += text;
    } else {
        extension->searchText = text;
    }
    std::vector<std::string> blank;
    extension->updateSubButtonNames(blank);
    extension->searchBox->setText(extension->searchText, dontSendNotification);
    extension->filterButtons(extension->searchText);
}

void ExtensionWindow::songSearchBackspace() {
    String text = extension->searchText;
    if (text.length() >= 1) {
        text = text.substring(0, text.length() - 1);
        songSearch(text);
    }
}

void ExtensionWindow::songSearchSelect() {
    if (!isActiveSearch()) return;
    for (int i = 0; i < extension->buttons.size(); ++i) {
        if (extension->buttons[i]->isVisible()) {
             if (lib->inSetlistMode()) {
                lib->switchToSong(i, 0);
            } else {
                selectButton(i);
                chordProReadFile(i);          
            }
            setSongLabel();
            return;
        }
    }
}

void ExtensionWindow::setSongPanelToFloating(bool isFloating) {
    if (isFloating) {
        extension->viewport.setViewedComponent(nullptr, false);
        extension->floatingViewport.setViewedComponent(&extension->container, false);
        extension->container.setBounds(extension->container.getBounds().withWidth(DEFAULT_SONG_LIST_WIDTH-10));
        extension->resized();
    } else {
        extension->floatingViewport.setVisible(false);
        extension->floatingViewport.setViewedComponent(nullptr, false);
        extension->viewport.setViewedComponent(&extension->container, false);
    }
}

void ExtensionWindow::playheadChange(bool playing) {
    if (extension == nullptr) return;
    if (playing) {
        MessageManager::getInstance()->callAsync([]() {
            chordProAutoScrollPlay(true);
        });
    }
}

void ExtensionWindow::clearSearch() {
    extension->searchText = "";
}

bool ExtensionWindow::isActiveSearch() {
    return (extension->searchContainer.isVisible() && extension->searchText != "");
}

void ExtensionWindow::displaySearchCarat() {
    searchCaratOn = !searchCaratOn;
    extension->searchBox->repaint();
}

String ExtensionWindow::getDefaults() {
    String defaults;
    NamedValueSet prefs = extension->preferences->getProperties();
    for (int i = 0; i < prefs.size(); ++i) { 
        String value = prefs.getValueAt(i).toString();
        if (value == "0") { value = "false";}
        else if (value == "1") { value = "true";}
        defaults += (prefs.getName(i).toString() + " = " + value + "\n");
    }
    return defaults;
}

String ExtensionWindow::getChordProColors() {
    String text;
    NamedValueSet prefs = extension->preferencesChordProColors->getProperties();
    for (int i = 0; i < prefs.size(); ++i) { 
        String value = prefs.getValueAt(i).toString();
        text += (prefs.getName(i).toString() + " = " + value + "\n");
    }
    return text;
}

String ExtensionWindow::getWindowState() {
    Rectangle<int> window = getWindowPositionAndSize();
    String positionSize = "PositionAndSize = " + String(window.getX()) + "," + String(window.getY()) + "," + String(window.getWidth()) + "," + String(window.getHeight());
    String divider = "SongListDivider = " + String(extension->draggableResizer.getX());
    String dividerEditor = "EditorDivider = " + String(extension->draggableResizerEditor.getX());
    return positionSize + "\n" + divider + "\n" + dividerEditor;
}

void ExtensionWindow::saveWindowState() {
    if (!extension->prefsLoaded) return;
    String windowState = getWindowState();
    String path = extensionPath + PREF_FILENAME;
    File prefs = File(path);
    prefs.create();
    String content = prefs.loadFileAsString();
    int position = content.indexOf("[WindowLastSavedState]");
    if (position > 0) {
        content = content.substring(0, position);
    }
    content = content.trimEnd() + "\n\n[WindowLastSavedState]\n" + windowState;
    prefs.replaceWithText(content);
}

void ExtensionWindow::savePreferences() {
    if (!extension->prefsLoaded) return;

    String path = extensionPath + PREF_FILENAME;
    File prefs = File(path);
    prefs.create();
    String content;

    // Defaults
    String defaults = getDefaults();

    // ChordPro Colors
    String chordProColors = getChordProColors();

    // Window size/position
    String windowState = getWindowState();

    content = "[Defaults] \n" + defaults + "\n[LyricChordColors]\n" + chordProColors + "\n[WindowLastSavedState]\n" + windowState;
    prefs.replaceWithText(content);
}

void ExtensionWindow::saveChordProFile() {
    int songIndex = lib->inSetlistMode() ? lib->getCurrentSongIndex() : extension->getButtonSelected();
    String path = lib->getChordProFilenameForSong(songIndex);
    File file = File(path);
    if (file.existsAsFile()) {
        String newContent = extension->chordProEditor->getText();
        file.replaceWithText(newContent);
        extension->editorTextEdited = false;
    }
}

void ExtensionWindow::readPreferencesFile() {
    if (extension == nullptr) return;
    std::string prefsFilePath = extensionPath + PATH_SEPARATOR() + PREF_FILENAME;
    File file = File(prefsFilePath);
    String prefsFileText = file.loadFileAsString();
    StringArray lines = StringArray::fromLines(prefsFileText);
    StringArray keyValue;
    StringPairArray defaults;
    StringPairArray colors;
    StringPairArray chordpro;
    StringPairArray windowstate;
    String line;
    String prefSection;
    for (int i = 0; i < lines.size(); ++i) { 
        line = lines[i].toStdString();
        if (line.contains("[")) { // Preference Heading/Section
            prefSection = line.removeCharacters("[]");
        } else if (line.trim() != "") { // Process Preferences, assuming key/value pairs
            line = line.removeCharacters(" ");
            keyValue = StringArray::fromTokens(line,"=","");
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
    processPreferencesDefaults(defaults);
    processPreferencesColors(colors);
    processPreferencesChordProColors(chordpro);
    processPreferencesWindowState(windowstate);
}

void ExtensionWindow::restartWindowTimer() {
    extension->windowTimer.stopTimer();
    extension->windowTimer.startTimer(TIMER_WINDOW_STATE);
}

void ExtensionWindow::logToGP(std::string text) {
    bool display = true;
    if (display)
        lib->consoleLog(text);
}

void MyDocumentWindow::closeButtonPressed() { 
    ExtensionWindow::savePreferences();
    ExtensionWindow::displayWindow(false);
}

void ChordProContainer::mouseDown (const MouseEvent& )
{
    ExtensionWindow::displayFontContainer(false);
    ExtensionWindow::displaySetlistContainer(false);
    ExtensionWindow::displayTransposeContainer(false);
    ExtensionWindow::displaySearchContainer(false);
    if (!autoscrollPanelPersist) ExtensionWindow::displayAutoscrollContainer(false);
    ExtensionWindow::extension->resized();
}

void ClockTimer::timerCallback() {
    ExtensionWindow::updateClock(Time::getCurrentTime().toString(false, true, false, true));
}

void RefreshTimer::timerCallback() {
    if (lib == nullptr) return;
    if (ExtensionWindow::isActiveSearch()) return;
    if (!ExtensionWindow::extension->prefsLoaded) { // Useful when Extension is activated in GP after the gig file is already loaded.
        ExtensionWindow::extension->readPreferencesFile();
        ExtensionWindow::updateSetlistButtons(lib->getSetlistNames());
        ExtensionWindow::selectSetlistButton(lib->getCurrentSetlistIndex());
        ExtensionWindow::chordProReadFile(ExtensionWindow::getButtonSelected());
    }
    ExtensionWindow::compareButtonNames(lib->getSongNames());
    ExtensionWindow::compareSubButtonNames(ExtensionWindow::getSubButtonNamesByIndex(ExtensionWindow::getButtonSelected()));
    ExtensionWindow::checkSongListPosition();
}

void CaratTimer::timerCallback() {
    ExtensionWindow::displaySearchCarat();
}

void SongScrollPauseDisplayTimer::timerCallback() {
    ExtensionWindow::flashAutoscrollTime(true);
}

void CreateImageTimer::timerCallback() {
    ExtensionWindow::chordProCreateInvertedImages();
    this->stopTimer();
}

void WindowChangeTimer::timerCallback() {
    ExtensionWindow::savePreferences();
    this->stopTimer();
}

void SongScrollTimer::hiResTimerCallback() {
    int64 time = juce::Time::getCurrentTime().toMilliseconds();
    int elapsed = time - ExtensionWindow::extension->chordProScrollStart - ExtensionWindow::extension->chordProRunningPause;
    int total = ExtensionWindow::extension->chordProSongScrollDuration - ExtensionWindow::extension->chordProTotalPause;
    if ( elapsed > total ) {
        this->stopTimer();
        ExtensionWindow::extension->chordProAutoScrollWindow(1.0);
        ExtensionWindow::extension->updateAutoscrollTime(Time(ExtensionWindow::extension->chordProSongScrollDuration));
        ExtensionWindow::extension->repaint(); // Avoids artefacts
    } else {
        ExtensionWindow::extension->updateAutoscrollTime(Time(elapsed + ExtensionWindow::extension->chordProRunningPause));
        ExtensionWindow::extension->chordProAutoScrollWindow(float(elapsed) / float(total));
    }
}

void SongScrollPauseTimer::hiResTimerCallback() {
    this->stopTimer();
    ExtensionWindow::extension->songScrollTimer.startTimer(TIMER_AUTOSCROLL);
    ExtensionWindow::extension->flashAutoscrollTime(false);

}

bool MyDocumentWindow::keyPressed(const KeyPress& key)
{
    String character = String::charToString(key.getTextCharacter());
    if (key.getModifiers().isCommandDown() && key.getKeyCode() == 70) { // CTRL+f
       ExtensionWindow::displaySearchContainer(true);
    } else if (key.getKeyCode() == KeyPress::backspaceKey || key.getKeyCode() == KeyPress::deleteKey) {
        ExtensionWindow::songSearchBackspace();
    } else if (key.getKeyCode() == KeyPress::spaceKey) { // Spacebar
        ExtensionWindow::songSearch(" ", true);
    } else if (key.getKeyCode() == KeyPress::escapeKey) { // Escape
        ExtensionWindow::displaySearchContainer(false);     
    } else if (key.getKeyCode() == KeyPress::returnKey) { // Return
        ExtensionWindow::songSearchSelect();
    } else if (key.getKeyCode() == KeyPress::downKey) { // Down
        ExtensionWindow::incrementSubButton(1);
    } else if (key.getKeyCode() == KeyPress::upKey) { // Up
        ExtensionWindow::incrementSubButton(-1);
    } else if (key.getKeyCode() >= 65 && key.getKeyCode() <= 90) { // a to z
        ExtensionWindow::songSearch(String::charToString(key.getTextCharacter()), true);
    } else if (key.getKeyCode() >= 48 && key.getKeyCode() <= 57) { // 0 to 9
        ExtensionWindow::songSearch(String::charToString(key.getTextCharacter()), true);
    } else {
        ExtensionWindow::songSearch(String::charToString(key.getTextCharacter()), true);
    }
    return true;
}

void ColourChangeButton::changeListenerCallback (ChangeBroadcaster* source) 
    {
        if (ColourSelector* cs = dynamic_cast<ColourSelector*> (source))
            colourValue = cs->getCurrentColour().toString();
    }

void ChordProEditor::textEditorTextChanged(TextEditor& ) {
    if (getParentComponent()->isVisible()) {
        ExtensionWindow::chordProEditorUpdate();
    } 
}	
