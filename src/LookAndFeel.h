// Lyrics and Chords extension for Gig Performer by @rank13

#pragma once

#include "Constants.h"

using namespace juce;

extern Colour chordProLyricColor;
extern Colour chordProChordColor;
extern bool chordProMonospaceFont;
extern float chordProFontSize;
extern bool chordProSmallChordFont;
extern bool chordProLeftLabels;
extern bool chordProDarkMode;
extern int chordProTranspose;
extern FLAT_SHARP_DISPLAY chordProTransposeDisplay;
extern bool searchCaratOn;
extern Colour headerSongColor;

class buttonLookAndFeel : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics& g, TextButton& button,	bool, bool)
	{
		Font font (button.getHeight () * 0.25f);
		g.setFont (font);
		g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
			: TextButton::textColourOffId)
      .withMultipliedAlpha (0.5f));
    
    const auto buttonWidth = (double) button.getWidth();
    const auto buttonHeight = (double) button.getHeight();
    const String buttonText = button.getButtonText();
    const auto yIndent = buttonHeight * 0.1;
		const auto leftIndent = buttonWidth > 160 ? yIndent * 2 : 5;
    auto availableWidth = buttonWidth - leftIndent;
    const int rows = 1;
		
    // Button number
    Font font1 (juce::jmax(button.getHeight () * 0.4f, 16.f));
    g.setFont (font1);
    auto buttonNumber = button.getProperties()["displayIndex"];
    auto numberWidth = font1.getStringWidthFloat(button.getName()); 
    g.drawFittedText (buttonNumber,
      leftIndent, yIndent, numberWidth, buttonHeight - yIndent * 2,
      Justification::right, rows, 0.5f);

    // Button Name
    Font font2 (juce::jmax(button.getHeight () * 0.4f, 16.f));
    g.setFont (font2);
    g.setColour (button.getToggleState () ? Colours::white : Colour(0xffc5c5c5));
    g.drawFittedText (buttonText,
      (int)(leftIndent + (numberWidth * 1.5)), (int)yIndent, (int)(availableWidth - (numberWidth * 1.5)), (int)(buttonHeight - yIndent * 2.0),
      Justification::left, rows, 1.0f);
	}

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                              bool isButtonHighlighted, bool isButtonDown) {
    auto buttonArea = button.getLocalBounds().toFloat();
    float borderSize = buttonArea.getHeight() * ((button.getProperties()["thickBorder"]) ? 0.08 : 0.04);
    float cornerSize = buttonArea.getHeight() * 0.08;
    Colour buttonColor = Colour::fromString(button.getProperties()["colour"].toString());

    if (button.getToggleState()) {
      g.setColour (Colour(0xff6a6a6a));
    } else if (isButtonHighlighted && !isButtonDown) {
      g.setColour (Colour(0xff2f2f2f));
    } else if (isButtonDown) {
      g.setColour (Colour(0xff9a9a9a));
    } else {
      g.setColour (buttonColor);
    }   
    g.fillRoundedRectangle (buttonArea, cornerSize);
    if (button.getToggleState()) {
      g.setColour (Colours::white);
      buttonArea = buttonArea.withSizeKeepingCentre(buttonArea.getWidth() - borderSize, buttonArea.getHeight() - borderSize);      
      g.drawRoundedRectangle (buttonArea, cornerSize, borderSize);  
    }               
  }
};

class subButtonLookAndFeel : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics& g, TextButton& button,	bool, bool)
	{
		Font font (button.getHeight () * 0.25f);
		g.setFont (font);
		g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
			: TextButton::textColourOffId)
      .withMultipliedAlpha (0.5f));
    const auto buttonWidth = (double) button.getWidth();
    const auto buttonHeight = (double) button.getHeight();
    const auto padding = buttonHeight * 0.3;
    const String buttonText = button.getButtonText();
    const auto yIndent = buttonHeight * 0.1;
		const auto leftIndent = buttonWidth > 160 ? padding + (yIndent * 4) : padding + 5;

    // Button Name
    Font font2 (juce::jmax(buttonHeight * 0.4, 16.0));
    g.setFont (font2);
    g.setColour (button.getToggleState() ? Colours::white : Colour(0xffe5e5e5));
    auto availableWidth = buttonWidth - leftIndent;
    g.drawFittedText (buttonText,
      leftIndent, yIndent, availableWidth, buttonHeight - yIndent * 2,
      Justification::left, 1, 1.0f);
	}

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                              bool isButtonHighlighted, bool isButtonDown) {
    auto buttonArea = button.getLocalBounds().toFloat();
    auto highlightArea = button.getLocalBounds().toFloat();;
    auto padding = buttonArea.getHeight() * 0.3;
    buttonArea.setLeft(padding);
    float borderSize = buttonArea.getHeight() * (button.getProperties()["thickBorder"] ? 0.08 : 0.04);
    float cornerSize = buttonArea.getHeight() * 0.08;
    Colour buttonColor = Colour::fromString(button.getProperties()["colour"].toString());
    Colour borderColour = Colour::fromString(button.getProperties()["borderColor"].toString());

    if (button.getToggleState()) {
      if (buttonColor == Colour::fromString(DEFAULT_SUBBUTTON_COLOR)) {
        g.setColour (Colour(0xff6A6A6A));
      } else {
        g.setColour (buttonColor);
      }      
    } else if (isButtonHighlighted && !isButtonDown) {
      if (buttonColor == Colour::fromString(DEFAULT_SUBBUTTON_COLOR)) {
        g.setColour (buttonColor.withBrightness(buttonColor.getBrightness() + 0.1f));
      } else {
        g.setColour (buttonColor.withBrightness(buttonColor.getBrightness() - 0.1f));
      }
    } else if (isButtonDown) {
      g.setColour (Colour(0xff9a9a9a));
    } else {
      g.setColour (buttonColor);
    }   
    g.fillRoundedRectangle (buttonArea, cornerSize);  
    if (button.getToggleState()) {
      g.setColour (borderColour);
      buttonArea = buttonArea.withSizeKeepingCentre(buttonArea.getWidth() - borderSize, buttonArea.getHeight() - borderSize);
      g.drawRoundedRectangle (buttonArea, cornerSize, borderSize);  

      // Button Highlight
      highlightArea.setX(padding/4.0);
      highlightArea.setWidth(padding/6.0);
      g.setColour (Colours::white);
      g.fillRoundedRectangle (highlightArea, borderSize);  
    }            
  }
};

class subButtonHighlightLookAndFeel : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    auto highlightArea = label.getLocalBounds().toFloat();
    auto padding = (float)label.getProperties()["buttonHeight"] * 0.3;
    float cornerSize = 5.f;
    g.setColour (Colours::grey);
    highlightArea.setX(padding/4.0);
    highlightArea.setWidth(padding/6.0);
    g.fillRoundedRectangle (highlightArea, cornerSize);
  }
};

class blankButtonLookAndFeel : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics&, TextButton&, bool, bool) {};
  void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) {};
};

class colorButtonLookAndFeel : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics&, TextButton&, bool, bool) {};
  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& colour, bool isButtonHighlighted, bool isButtonDown) 
  {
     auto buttonArea = button.getLocalBounds().toFloat();
     g.setColour(colour);
     g.fillRect(buttonArea);
     if (isButtonHighlighted | isButtonDown) {
      g.setColour(Colours::white);
      g.drawRect(buttonArea);
     }
  };
};

class setlistButtonLookAndFeel : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics& g, TextButton& button,	bool, bool)
	{
		Font font (button.getHeight () * 0.4f);
		g.setFont (font);
		g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
			: TextButton::textColourOffId)
      .withMultipliedAlpha (0.8f));
    
    const auto buttonWidth = (double) button.getWidth();
    const auto buttonHeight = (double) button.getHeight();
    const String buttonText = button.getButtonText();
    const auto yIndent = buttonHeight * 0.1;
		const auto leftIndent = 25;
    auto availableWidth = buttonWidth - leftIndent - 30;
    const int rows = 1;

    // Button Name
    Font font2 (button.getHeight() * 0.38f);
    g.setFont (font2);
    g.setColour(Colours::white);
    g.drawFittedText (buttonText,
      leftIndent, yIndent, availableWidth, buttonHeight - yIndent * 2,
      Justification::left, rows, 1.0f);
	}

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                              bool isButtonHighlighted, bool isButtonDown) {
    auto buttonArea = button.getLocalBounds().toFloat();
    float cornerSize = buttonArea.getHeight() * 0.08;
    Colour buttonColor = Colour::fromString(button.getProperties()["colour"].toString());

    if (button.getToggleState()) {
      g.setColour (Colour(0xff6a6a6a));
    } else if (isButtonHighlighted && !isButtonDown) {
      g.setColour (Colour(0xff2f2f2f));
    } else if (isButtonDown) {
      g.setColour (Colour(0xff9a9a9a));
    } else {
      g.setColour (buttonColor);
    }   
    g.fillRoundedRectangle (buttonArea, cornerSize);
  }
};

class setlistListButtonLookAndFeel : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics& g, TextButton& button,	bool, bool)
	{
		Font font (button.getHeight () * 0.4f);
		g.setFont (font);
		g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
			: TextButton::textColourOffId)
      .withMultipliedAlpha (0.8f));
    
    const auto buttonWidth = (double) button.getWidth();
    const auto buttonHeight = (double) button.getHeight();
    const String buttonText = button.getButtonText();
    const auto yIndent = buttonHeight * 0.1;
		const auto leftIndent = 25;
    auto availableWidth = buttonWidth - leftIndent - 30;
    const int rows = 1;

    // Button Name
    Font font2 (button.getHeight() * 0.38f);
    g.setFont (font2);
    g.setColour(Colours::white);
    g.drawFittedText (buttonText,
      leftIndent, yIndent, availableWidth, buttonHeight - yIndent * 2,
      Justification::left, rows, 1.0f);
	}

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                              bool isButtonHighlighted, bool isButtonDown) {
    auto buttonArea = button.getLocalBounds().toFloat();
    float cornerSize = buttonArea.getHeight() * 0.08;
    Colour buttonColor = Colour::fromString(button.getProperties()["colour"].toString());

    if (button.getToggleState()) {
      g.setColour (Colour(0xff6a6a6a));
    } else if (isButtonHighlighted && !isButtonDown) {
      g.setColour (Colour(0xff2f2f2f));
    } else if (isButtonDown) {
      g.setColour (Colour(0xff9a9a9a));
    } else {
      g.setColour (buttonColor);
    }   
    g.fillRoundedRectangle (buttonArea, cornerSize);
  }
};

class gridButtonLookAndFeel : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics& g, TextButton& button,
		bool, bool)
	{
		Font font (button.getHeight () * 0.25f);
		g.setFont (font);
		g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
			: TextButton::textColourOffId)
      .withMultipliedAlpha (0.5f));
    
		const int yIndent = button.proportionOfHeight (0.1f) * 2;
		const int cornerSize = jmin (button.getHeight (), button.getWidth ()) / 2;
		const int leftIndent = cornerSize / (button.isConnectedOnLeft () ?
                  yIndent * 2 : yIndent);
		const int rightIndent = cornerSize / (button.isConnectedOnRight () ? 
                  yIndent * 2 : yIndent);
		const int textWidth = button.getWidth () - leftIndent - rightIndent;
    const int rows = 1;
		if (textWidth > 100) {
      // Button number
      Font font1 (juce::jmax(button.getHeight () * 0.4f, 16.f));
		  g.setFont (font1);
      int numberWidth = (int) font1.getStringWidthFloat(button.getName());  // Base width on 2 digits
      auto buttonNumber = button.getProperties()["displayIndex"];
      g.drawFittedText (buttonNumber,
      leftIndent*4, yIndent, numberWidth, button.getHeight () - yIndent * 2,
      Justification::right, rows, 0.5f);
      
      // Button Name
      Font font2 (juce::jmax(button.getHeight () * 0.4f, 16.f));
		  g.setFont (font2);
      g.setColour  (button.getToggleState () ? Colours::white : Colour(0xffc5c5c5));
			g.drawFittedText (button.getButtonText (),
				(leftIndent*4) + (numberWidth*1.5), yIndent, textWidth-(leftIndent*20), button.getHeight () - yIndent * 2,
				Justification::left, rows, 1.0f);
    }
	}

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                              bool isButtonHighlighted, bool isButtonDown) {
    auto buttonArea = button.getLocalBounds().toFloat().reduced (0.5f);
    float cornerSize = 5.f;
    Colour myColour = Colour(0xff1f1f1f);
    String btnColour = button.getProperties()["colour"];
    myColour = Colour::fromString(btnColour);

    if (button.getToggleState()) {
      g.setColour (Colour(0xff6a6a6a));
      
    } else if (isButtonHighlighted && !isButtonDown) {
      g.setColour (Colour(0xff2f2f2f));
    } else if (isButtonDown) {
      g.setColour (Colour(0xff9a9a9a));      
    } else {
      g.setColour (myColour);
      
    }   
    g.fillRoundedRectangle (buttonArea, cornerSize);
    if (button.getToggleState()) {
      g.setColour (Colours::red);
      g.drawRoundedRectangle (buttonArea, cornerSize, 1.f);  
    }               
  }
};

class headerLookAndFeel : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    g.setFont (Font (25.00f, Font::plain).withTypefaceStyle ("Regular"));
    g.setColour (Colours::white);
    g.drawFittedText (label.getText(),
				0, 0, label.getWidth(), label.getHeight (),
				Justification::centred, 1, 1.0f);
  }
};

class headerSongs : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    g.fillAll (headerSongColor);

    g.setColour (headerSongColor.withLightness(0.18f));
    g.fillRect (label.getX(), label.getY() + label.getHeight() - 1, label.getWidth(), 1);

    g.setFont (Font (25.00f, Font::plain).withTypefaceStyle ("Regular"));
    g.setColour (Colours::white);
    g.drawFittedText (label.getText(),
				15, 0, label.getWidth(), label.getHeight (),
				Justification::centredLeft, 1, 1.0f);
  }
};

class headerRackspaces : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    //g.fillAll (headerRackspaceColor);
    auto labelArea = label.getLocalBounds();
    
    g.setFont (Font (25.00f, Font::plain).withTypefaceStyle ("Regular"));
    g.setColour (Colours::white);
    String title = (labelArea.getWidth() < 240 ? "Racks" : "Rackspaces");
    g.drawFittedText (title,
				15, 0, label.getWidth(), label.getHeight (),
				Justification::centredLeft, 1, 1.0f);
  }
};

class minimalistSong : public LookAndFeel_V4 {
public:
	void drawButtonText (Graphics& g, TextButton& button,
		bool, bool)
	{
		const int yIndent = button.proportionOfHeight (0.1f);
		const int cornerSize = jmin (button.getHeight (), button.getWidth ()) / 2;
		const int leftIndent = cornerSize / (button.isConnectedOnLeft () ?
                  yIndent * 2 : yIndent);
		const int rightIndent = cornerSize / (button.isConnectedOnRight () ? 
                  yIndent * 2 : yIndent);
		const int textWidth = button.getWidth () - leftIndent - rightIndent;

    g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
      : TextButton::textColourOffId)
      .withMultipliedAlpha (button.isEnabled () ? 1.0f : 0.5f));
		if (textWidth > 100) {
      if (button.getName() == "btnCurrent") {
    		Font font (button.getHeight () * 0.5f);
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
          : TextButton::textColourOffId)
          .withMultipliedAlpha (1.0f));
      }
      else if (button.getName() == "btnNext") {
        Font font (button.getHeight () * 0.5f);
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
          : TextButton::textColourOffId)
          .withMultipliedAlpha (0.8f));
      } else {
        Font font (button.getHeight () * 0.5f);
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState () ? TextButton::textColourOnId
          : TextButton::textColourOffId)
          .withMultipliedAlpha (0.5f));
      }
      g.drawFittedText (button.getButtonText (),
				leftIndent, yIndent, textWidth, button.getHeight () - yIndent * 2,
				Justification::centred, 2, 0.8f);
    }
	}

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                               bool, bool) {
     if (button.getName() == "btnCurrent") {
        g.fillAll (Colour (0xff1C1C1C));
     }
  }
};

class searchBoxLookAndFeel : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    Font font (Font (22.00f, Font::plain).withTypefaceStyle ("Regular"));
    g.setFont (font);
    g.setColour (Colours::white);
    g.fillRoundedRectangle(0, 10, 310, label.getHeight() - 20, 15.0f);
    if (label.getText() == "") {
      g.setColour (Colours::grey);
      g.setFont (Font (16.00f, Font::plain).withTypefaceStyle ("Regular").italicised());
      g.drawFittedText ("RETURN selects the first search result.",
          15, 0, label.getWidth(), label.getHeight (),
          Justification::centredLeft, 1, 1.0f);
    } else {
      g.setColour (Colours::black);
      g.drawFittedText (label.getText(),
          15, 0, label.getWidth(), label.getHeight (),
          Justification::centredLeft, 1, 1.0f);
    }
    
    if (searchCaratOn) {
      g.setFont(font);
      g.setColour(Colours::black);
      int textWidth = (int) font.getStringWidthFloat(label.getText());
      g.drawRect(15 + textWidth, 14, 1, label.getHeight() - 28);
    }
  }
};

class chordPro : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    StringArray wordParts;
    StringArray finalText;
    double lastWordPosition = 0.0;
    double lastChordPosition = 0.0;
    int leftPad = chordProLeftLabels ? CP_EXPANDED_LEFT_MARGIN * chordProFontSize : CP_DEFAULT_LEFT_MARGIN;
    auto labelArea = label.getLocalBounds();

    Font font (Font (label.getHeight(), Font::plain));
    Font chordFont (Font (label.getHeight(), Font::plain));
    Font labelFont = font.italicised();
    Font monoFont = font;
    monoFont.setTypefaceName(Font::getDefaultMonospacedFontName());

    if (chordProMonospaceFont) {
      font.setTypefaceName(Font::getDefaultMonospacedFontName());
      chordFont.setTypefaceName(Font::getDefaultMonospacedFontName());      
    } else {
      font = font.withTypefaceStyle("Regular").withExtraKerningFactor(0.06f);
      chordFont = chordFont.withTypefaceStyle("Regular");
    }
    if (label.getProperties()["type"] == "chordAndLyrics") {
      font = font.withHeight(label.getHeight() * 0.5f);
      labelFont = labelFont.withHeight(label.getHeight() * 0.5f);
      if (chordProSmallChordFont) {
        chordFont = chordFont.withHeight(label.getHeight() * 0.4f).boldened();
      } else {
        chordFont = chordFont.withHeight(label.getHeight() * 0.5f);
      }
    } 
    if (label.getProperties()["section"] == "chorus" && label.getProperties()["type"] != "directive") {
      g.setColour (chordProLyricColor);
      g.fillRect((int)(leftPad - (25 * chordProFontSize)), labelArea.getY(), (int)(5 * chordProFontSize), labelArea.getHeight());
    }
    
    if (chordProLeftLabels) {
      String sectionLabel = label.getProperties()["sectionLabel"];
      g.setFont (labelFont);
      g.setColour (chordProLyricColor.withAlpha(0.8f));
      g.drawFittedText ( sectionLabel,
            0, 0, (int)(leftPad - (CP_DEFAULT_LEFT_MARGIN * chordProFontSize)), label.getHeight(),
            Justification::bottomRight, 1, 1.0f);
    }

    StringArray words = StringArray::fromTokens(label.getText(),false);
    for (int i = 0; i < words.size(); ++i) { 
      wordParts = StringArray::fromTokens(words[i],"[]","");
      if (i == 0 || label.getProperties()["type"] == "chordOnly") wordParts.removeEmptyStrings();
      String chordCheck;
      String chord;

      for (int j = 0; j < wordParts.size(); ++j) {
        chordCheck = "["+wordParts[j]+"]";
        if (words[i].contains(chordCheck) || label.getProperties()["type"] == "chordOnly") { // Is a chord    
          chord = wordParts[j];
          g.setFont (chordFont);
          g.setColour(chordProChordColor);
          if (lastWordPosition < lastChordPosition) {
            lastWordPosition = lastChordPosition;
          } 

          // Apply transpose and flat/sharp conversion
          chord = ChordPro::CP_Transpose(chord, chordProTranspose, chordProTransposeDisplay);
        
          if (chordProSmallChordFont && label.getProperties()["type"] == "chordAndLyrics") {
            g.drawFittedText (chord,
            lastWordPosition + leftPad, label.getHeight() * 0.05, label.getWidth(), label.getHeight() / 2,
            Justification::centredLeft, 1, 1.0f);
          } else {
            g.drawFittedText (chord,
            lastWordPosition + leftPad, label.getHeight() * 0.025, label.getWidth(), label.getHeight() / 2,
            Justification::topLeft, 1, 1.0f);
          }
          lastChordPosition = lastWordPosition + font.getStringWidthFloat(chord);
          if (label.getProperties()["type"] == "chordAndLyrics") {
            lastChordPosition = lastChordPosition + font.getStringWidthFloat(" "); // Force minimum gap
          } else if (label.getProperties()["type"] == "chordOnly") {
            lastChordPosition = lastChordPosition + font.getStringWidthFloat("  ");  // Minimum gap for chord only lines
          }
        } else {
          g.setColour (chordProLyricColor);
          g.setFont (font);
          g.drawFittedText ( wordParts[j],
            lastWordPosition + leftPad, 0, label.getWidth(), label.getHeight(),
            Justification::bottomLeft, 1, 1.0f);
          chord = "";
          lastWordPosition += font.getStringWidthFloat(wordParts[j]);
        }
      }
      lastWordPosition += font.getStringWidthFloat(" "); 
    }
  }
};

class chordProTitle : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    int leftPad = chordProLeftLabels ? CP_EXPANDED_LEFT_MARGIN * chordProFontSize : CP_DEFAULT_LEFT_MARGIN;
    g.setFont (Font (label.getHeight() * 0.85f, Font::plain).withTypefaceStyle ("Regular"));
    g.setColour (chordProLyricColor);
    g.drawFittedText (label.getText(),
				leftPad, 0, label.getWidth(), label.getHeight (),
				Justification::centredLeft, 1, 1.0f);
    g.setColour (Colours::white);
  }
};

class chordProSubTitle : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    int leftPad = chordProLeftLabels ? CP_EXPANDED_LEFT_MARGIN * chordProFontSize : CP_DEFAULT_LEFT_MARGIN;
    g.setFont (Font (label.getHeight(), Font::plain).withTypefaceStyle ("Regular").italicised());
    g.setColour (chordProLyricColor.withAlpha(0.8f));
    g.drawFittedText (label.getText(),
				leftPad, 0, label.getWidth(), label.getHeight (),
				Justification::centredLeft, 1, 1.0f);
    g.setColour (Colours::white);
  }
};

class chordProComment : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    int leftPad = chordProLeftLabels ? CP_EXPANDED_LEFT_MARGIN * chordProFontSize : CP_DEFAULT_LEFT_MARGIN;
    auto labelArea = label.getLocalBounds();
    g.setColour (Colours::lightgrey.withAlpha(0.3f));
    Font font (Font (label.getHeight() * 0.8f, Font::plain).withTypefaceStyle ("Regular").italicised());
    g.setFont (font);
    auto textWidth = (int) font.getStringWidthFloat(label.getText());
    auto textHeight = (int) font.getHeight();
    auto y = (int) (labelArea.getHeight() - textHeight) / 2;
    g.fillRect( leftPad - 10, y - 5, textWidth + 25, textHeight + 10);
    g.setColour (chordProLyricColor.withAlpha(0.8f));
    g.drawFittedText (label.getText(),
				leftPad, y, label.getWidth(), textHeight,
				Justification::centredLeft, 1, 1.0f);
    g.setColour (Colours::white);
  }
};

class chordProLabel : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    auto labelArea = label.getLocalBounds();
    int leftPad = chordProLeftLabels ? CP_EXPANDED_LEFT_MARGIN * chordProFontSize : CP_DEFAULT_LEFT_MARGIN;
    Font font ((Font (label.getHeight() * 2 / 3, Font::plain).withTypefaceStyle ("Regular").boldened().italicised()));
    g.setFont (font);
    g.setColour (chordProLyricColor.withAlpha(0.8f));
    g.drawFittedText (label.getText(),
				leftPad, 0, label.getWidth(), label.getHeight (),
				Justification::centredLeft, 1, 1.0f);
    g.setColour (Colours::white);
    auto textWidth = (int) font.getStringWidthFloat(label.getText());
    g.setColour (Colours::grey.withAlpha(0.8f));
    g.drawLine(leftPad, labelArea.getBottom() * 0.9, leftPad + textWidth, labelArea.getBottom() * 0.9, labelArea.getHeight() * 0.05);
  }
};

class chordProTab : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    int leftPad = chordProLeftLabels ? CP_EXPANDED_LEFT_MARGIN * chordProFontSize : CP_DEFAULT_LEFT_MARGIN;
    Font font (Font (label.getHeight(), Font::plain));
    font.setTypefaceName(Font::getDefaultMonospacedFontName());
    g.setFont (font);
    auto textWidth = (int) font.getStringWidthFloat(label.getText());
    auto textHeight = (int) font.getHeight();
    g.setColour (chordProDarkMode ? Colours::white.withAlpha(0.08f) : Colours::grey.withAlpha(0.08f));
    g.fillRect( leftPad, 0, textWidth, textHeight);
    String text = label.getText().toStdString();
    int runningTextWidth = 0;
    for (int i = 0; i < text.length(); i++) {
      g.setColour (chordProLyricColor.withAlpha(0.8f));
      runningTextWidth = (int) font.getStringWidthFloat(text.substring(0, i));
      if (text.substring(i, i+1) == "-" || text.substring(i, i+1) == "|") {
        g.setColour (chordProLyricColor.withAlpha(0.3f));
      }
      g.drawFittedText (text.substring(i, i+1),
        leftPad + runningTextWidth, 0, label.getWidth(), label.getHeight (),
        Justification::centredLeft, 1, 1.0f);
    }
  }
};

class chordProGrid : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    int leftPad = chordProLeftLabels ? CP_EXPANDED_LEFT_MARGIN * chordProFontSize : CP_DEFAULT_LEFT_MARGIN;
    Font font (Font (label.getHeight() * 1.0f, Font::plain));
   
    font.setTypefaceName(Font::getDefaultMonospacedFontName());
    g.setFont (font);
    int gridLength = label.getProperties()["gridBarLength"];
    int gridBars = label.getProperties()["gridBars"];
    int gridBeats = jmax((int)label.getProperties()["gridBeats"], 1);
    gridLength = juce::jmax(gridLength > gridBeats ? gridLength + 4 : gridLength, 8);
    
    // Background
    Font fontSpacer = font;
    int textWidth = (int) (fontSpacer.getStringWidthFloat(" ") * gridLength * gridBars);
    auto textHeight = (int) font.getHeight();
    g.setColour (chordProDarkMode ? Colours::white.withAlpha(0.08f) : Colours::grey.withAlpha(0.08f));
    if (label.getText() != "")
      g.fillRect( leftPad, 0, textWidth + (int)(label.getHeight() * 0.16), textHeight);

    // Post Comments
    Font commentFont (Font (label.getHeight(), Font::plain).withTypefaceStyle ("Regular"));
    g.setColour (chordProLyricColor.withAlpha(0.8f));
    g.setFont(commentFont);
    g.drawFittedText (label.getProperties()["gridComment"],
          leftPad + textWidth + label.getHeight(), 0, label.getWidth() - textWidth - leftPad, label.getHeight(),
          Justification::centredLeft, 1, 1.0f);

    // Bars and Chords
    int runningTextWidth = 0;
    int barCount = 1;
    StringArray parts = StringArray::fromTokens(label.getText(),false);
    parts.removeEmptyStrings();
    for (int i = 0; i < parts.size(); ++i) { 
      g.setColour (chordProLyricColor.withAlpha(0.8f));
      juce::AttributedString barCharacter;
      juce::String partCharacter = parts[i];
      if (partCharacter == "|" || partCharacter == "||" || partCharacter == "|:" || partCharacter == ":|") {
        // Draw bar characters
        if (partCharacter == "|") {
          barCharacter.setText(juce::String::charToString(0x1D100));
        } else if (partCharacter == "||") {
          barCharacter.setText(juce::String::charToString(0x1D101));
        } else if (partCharacter == "|:") {
          barCharacter.setText(juce::String::charToString(0x1D106));
        } else if (partCharacter == ":|") {
          barCharacter.setText(juce::String::charToString(0x1D107));
        }
        Font barFont (Font (label.getHeight() * 1.4f, Font::plain));
        barFont.setTypefaceName(Font::getDefaultMonospacedFontName());
        #if JUCE_WINDOWS
           barFont.setTypefaceName("Lucida Sans Unicode");
           barFont.setHeight(label.getHeight() * 2.0f);
        #endif
        barCharacter.setFont( barFont );
        barCharacter.setJustification( juce::Justification::centredLeft );
        barCharacter.setColour( chordProLyricColor.withAlpha(0.6f));
        if (runningTextWidth > 0) {
          runningTextWidth = (int)(fontSpacer.getStringWidthFloat(" ") * gridLength * barCount);
          if (partCharacter == "||") runningTextWidth = runningTextWidth - (int)(label.getHeight() * 0.12); // Last bar adjustment
          if (partCharacter == ":|") runningTextWidth = runningTextWidth - (int)(label.getHeight() * 0.4); // Repeat end adjustment
          barCount++;
        }
        barCharacter.draw( g, label.getLocalBounds().toFloat().withX(leftPad + runningTextWidth).withY((int)(label.getHeight() * 0.15)));
        runningTextWidth += (int)fontSpacer.getStringWidthFloat(" ");
      } else {
        // Draw Chords
        font = font.withHeight(label.getHeight());
        g.setFont(font);
        g.setColour(chordProChordColor);
        if (partCharacter.getIntValue() > 0) { // Volta
          g.setColour( chordProLyricColor.withAlpha(0.6f));
          g.setFont(font.withHeight(label.getHeight() * 0.5f));
          int adjustment = (int)(fontSpacer.getStringWidthFloat(" ") * 0.6f);
          g.drawFittedText (partCharacter,
            leftPad + runningTextWidth - adjustment, 0, label.getWidth(), label.getHeight (),
            Justification::topLeft, 1, 1.0f);

        } else {
          if (partCharacter != ".") {
            partCharacter = ChordPro::CP_Transpose(partCharacter, chordProTranspose, chordProTransposeDisplay);
            g.drawFittedText (partCharacter,
              leftPad + runningTextWidth, 0, label.getWidth(), label.getHeight (),
              Justification::centredLeft, 1, 1.0f);
          } 
          runningTextWidth += (int)(font.getStringWidthFloat(" ") * (float)(gridLength / gridBeats));
        }

      }
    }

    // Section Label
    if (chordProLeftLabels) {
      String sectionLabel = label.getProperties()["sectionLabel"];
      g.setFont (commentFont.italicised());
      g.setColour (chordProLyricColor.withAlpha(0.8f));
      g.drawFittedText ( sectionLabel,
            0, 0, (int)(leftPad - (CP_DEFAULT_LEFT_MARGIN * chordProFontSize)), label.getHeight(),
            Justification::bottomRight, 1, 1.0f);
    }    
  }
};

class popOverLookAndFeel : public LookAndFeel_V4 {
public:
  void drawButtonText (Graphics& g, TextButton& button,
    bool, bool)
	{
    int y = 0;
    int fontHeight = button.getHeight() * 0.9;
    if (button.getName() == "Mono" || button.getName() == "Create" || button.getName() == "Save") {
      fontHeight = button.getHeight() * 0.5;
    } else if (button.getName() == "transposeSharp") {
      fontHeight = button.getHeight() * 0.75;
    } 
		Font font (fontHeight);
    auto height = ((button.getName() == "Mono" || button.getName() == "Create" || button.getName() == "Save") ? button.getHeight() : button.getHeight() * 0.85);
		if (button.getName() == "transposeFlat" || button.getName() == "transposeSharp") {
      font.setTypefaceName(Font::getDefaultMonospacedFontName());
      #if JUCE_WINDOWS
        font.setTypefaceName("Lucida Sans Unicode");
      #endif
      y = button.getHeight() * (button.getName() == "transposeFlat" ? 0.05 : 0.08);
    }
    g.setFont (font);
		g.setColour (Colours::white);
    if (button.getName() == "autoscrollPlay") {
      juce::AttributedString play;
      play.setText(juce::String::charToString(0x25B6));
      Font playFont (Font (button.getHeight() * 0.8f, Font::plain));
      playFont.setTypefaceName(Font::getDefaultMonospacedFontName());
      #if JUCE_WINDOWS
          playFont.setTypefaceName("Lucida Sans Unicode");
          playFont.setHeight(label.getHeight());
      #endif
      play.setFont( playFont );
      play.setJustification( juce::Justification::centred );
      play.setColour(Colours::white);
      play.draw( g, button.getLocalBounds().toFloat().withTrimmedBottom(button.getHeight() * 0.125f));
    } else if (button.getName() == "autoscrollResetNOT") {
      juce::AttributedString reset;
      reset.setText(juce::String::charToString(0x2912));
      Font resetFont (Font (button.getHeight(), Font::plain));
      resetFont.setTypefaceName(Font::getDefaultMonospacedFontName());
      #if JUCE_WINDOWS
          playFont.setTypefaceName("Lucida Sans Unicode");
          playFont.setHeight(label.getHeight());
      #endif
      reset.setFont( resetFont );
      reset.setJustification( juce::Justification::centred );
      reset.setColour(Colours::white);
      reset.draw( g, button.getLocalBounds().toFloat());
    } 
    else {
      g.drawFittedText (button.getButtonText(),
          0, y, button.getWidth(), height,
          Justification::centred, 1, 1.0f);
    }
  }

  void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                              bool isButtonHighlighted, bool isButtonDown) {
    auto buttonArea = button.getLocalBounds().toFloat().reduced (0.5f);
    float cornerSize = 5.f;
    if (button.getToggleState()) {
      g.setColour (Colour(0xff6a6a6a)); 
    } else if (isButtonHighlighted && !isButtonDown) {
      g.setColour (Colours::darkgrey);
    } else if (isButtonDown) {
      g.setColour (Colour(0xff9a9a9a));
    } else {
      g.setColour (Colours::darkgrey.withLightness(0.15f));
    }   
    g.fillRoundedRectangle (buttonArea, cornerSize);
    g.setColour(Colours::white.withAlpha(0.1f));
    g.drawRoundedRectangle (buttonArea, cornerSize, 2.f);   
  }
};

class popOverLabel : public LookAndFeel_V4 {
public:
	void drawLabel (Graphics& g, Label& label) {
		const int textWidth = label.getWidth();
    g.setFont (Font (22.00f, Font::plain).withTypefaceStyle ("Regular"));
    if (label.getProperties()["source"] == "estimate") {
      g.setColour (Colours::grey);
    } else {
      g.setColour (chordProLyricColor.withMultipliedBrightness(0.8f));
    }
    g.drawFittedText (label.getText(),
      15, 0, textWidth, label.getHeight(),
      Justification::left, 1, 1.0f);
    }
};

class noSongLabelLookAndFeel : public LookAndFeel_V4 {
public:
	void drawLabel (Graphics& g, Label& label) {
    g.setFont (Font (16.00f, Font::plain).withTypefaceStyle ("Regular").italicised());
    g.setColour (Colours::white.withAlpha(0.6f));
    g.drawFittedText (label.getText(),
      0, 0, label.getWidth(), label.getHeight(),
      Justification::centred, 1, 1.0f);
    }
};

class noChordProLabelLookAndFeel : public LookAndFeel_V4 {
public:
	void drawLabel (Graphics& g, Label& label) {
    g.setFont (Font (24.00f, Font::plain).withTypefaceStyle ("Regular").italicised());
    g.setColour (Colours::white.withAlpha(0.6f));
    g.drawFittedText (label.getText(),
      0, 0, label.getWidth(), label.getHeight(),
      Justification::centred, 2, 1.0f);
    }
};

class chordProEditorLookAndFeel : public LookAndFeel_V4 {
public:
  void drawLabel (Graphics& g, Label& label) {
    g.setFont (Font (24.00f, Font::plain).withTypefaceStyle ("Regular").italicised());
    g.setColour (Colours::white.withAlpha(0.6f));
    g.drawFittedText (label.getText(),
      0, 0, label.getWidth(), label.getHeight(),
      Justification::centred, 2, 1.0f);
  }
};