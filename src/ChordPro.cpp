// Lyrics and Chords extension for Gig Performer by @rank13

#include "ChordPro.h"
#include <regex>

ChordDiagramKeyboard::ChordDiagramKeyboard () 
{
    for (int i = 0; i < octaves; ++i) {
        for (int j = 0; j < 12; ++j) {
            keyboard.add(new ChordDiagramNote());
        }
    }
    setInterceptsMouseClicks(false, false);
}

void ChordDiagramKeyboard::paint(Graphics& g) {
    auto bounds = getLocalBounds();
    int labelHeight = getHeight() * 0.35;
    int keyboardHeight = getHeight() * 0.65;

    g.setFont(Font(labelHeight * 0.8f));
    g.setColour(darkMode ? Colour(0xFFF0F0F0) : Colour(0xFF0F0F0F));
    g.drawFittedText(chordLabel, bounds.withHeight(labelHeight), Justification::centred, 1, 1.0f);
    g.setColour(darkMode ? Colour(0xFFD0D0D0) : Colour(0xFF1F1F1F));
   
    int x = 0;
    int whiteWidth = bounds.getWidth() / (numberOfWhiteKeys * octaves);
    int blackWidth = whiteWidth * 0.6;
    int blackToWhiteX = blackWidth / 2;
    int whiteToBlackX = whiteWidth - (blackWidth / 2);

    // White keys
    for (int i = 0; i < keyboard.size(); ++i) {
        int key = i % 12;
        g.setColour(darkMode ? Colour(0xFFF0F0F0) : Colour(0xFF0F0F0F));
        if (key == 1 || key == 3 || key == 6 || key == 8 || key == 10 ) { // Black
            x += blackToWhiteX;
        } else { // White
            g.drawRect(x, labelHeight, whiteWidth, keyboardHeight);
            if(keyboard[i]->ChordDiagramNote::noteOn) {
                g.setColour(onColour);
                g.fillRect(x + 1, labelHeight + 1, whiteWidth - 2, keyboardHeight - 2);
            }
            x += (key == 4 || key == 11 ? whiteWidth : whiteToBlackX);
        }
    }

    // Black keys
    x = 0;
    for (int i = 0; i < keyboard.size(); ++i) {
        int key = i % 12;
        if (key == 1 || key == 3 || key == 6 || key == 8 || key == 10 ) { // Black
            g.setColour(darkMode ? Colour(0xFFF0F0F0) : Colour(0xFF0F0F0F));
            g.fillRect(x, labelHeight, blackWidth, keyboardHeight * 0.6);
            if(keyboard[i]->ChordDiagramNote::noteOn) {
                g.setColour(onColour);
                g.fillRect(x + 1, labelHeight + 1, blackWidth - 2, keyboardHeight * 0.6 - 2);
            }
            x += blackToWhiteX;
        } else { // White
            x += (key == 4 || key == 11 ? whiteWidth : whiteToBlackX);
        }
    }
}

void ChordDiagramKeyboard::updateChord(String newChord, StringArray newChordNotes) {
    chord = newChord;
    chordNotes.swapWith(newChordNotes);
}

String ChordDiagramKeyboard::getChord() {
    return chord;
}

void ChordDiagramKeyboard::updateChordDiagram(int transpose = 0, FLAT_SHARP_DISPLAY accidental = original) {
    StringArray chordParts = StringArray::fromTokens(chord,"/","");
    String root = ChordPro::CP_GetRootNote(chordParts[0]);
    int rootIndex = ChordPro::CP_GetRootNoteIndex(root);
    allNotesOff();
    for (int i = 0; i < chordNotes.size(); ++i) {
        int keyIndex = (chordNotes[i].getIntValue() + rootIndex + transpose) % keyboard.size();
        //int keyIndex = chordNotes[i].getIntValue() + rootIndex + transpose;
        /*
        if (keyIndex >= keyboard.size()) {
            keyIndex -= keyboard.size();
        } else if (keyIndex < 0) {
            keyIndex += keyboard.size();
        }
        */
        keyboard[keyIndex]->noteOn = true;
    }
    chordLabel = ChordPro::CP_Transpose(chord, transpose, accidental);
    repaint();
}

void ChordDiagramKeyboard::updateKeyOnColour(Colour newColour) {
    onColour = newColour;
}

void ChordDiagramKeyboard::setDarkMode(bool isDarkMode) {
    darkMode = isDarkMode;
    repaint();
}

void ChordDiagramKeyboard::allNotesOff() {
    for (int i = 0; i < keyboard.size(); ++i) {
        keyboard[i]->noteOn = false;
    }
}

ChordDiagramFretboard::ChordDiagramFretboard () 
{
    for (int i = 0; i < numberOfStrings; ++i) {
        fretboard.add(new ChordDiagramFret());
    }
}

void ChordDiagramFretboard::paint(Graphics& g) {
    int labelHeight = getHeight() * 0.24;
    int fretboardHeight = getHeight() * 0.75;
    int fretboardWidth = getWidth() * 0.98;
    int fretHeight = fretboardHeight / (numberOfFrets - 1);
    int stringWidth = fretboardWidth / (numberOfStrings + 1);
    auto labelBounds = Rectangle<int>(0, 0, getWidth(), labelHeight);

    g.setFont(Font(labelHeight * 0.86f));
    g.setColour(darkMode ? Colour(0xFFF0F0F0) : Colour(0xFF0F0F0F));

    // Chord label
    g.drawFittedText(chordLabel, labelBounds, Justification::centredBottom, 1, 1.0f);
    g.setColour(darkMode ? Colour(0xFFF0F0F0) : Colour(0xFF0F0F0F));

    // Strings
    int x = stringWidth;
    for (int i = 0; i < numberOfStrings; ++i) {
        g.fillRect(x, labelHeight + fretHeight, 1, fretHeight * (numberOfFrets - 2));
        x += stringWidth;
    }

    // Frets
    for (int i = 1; i < numberOfFrets; ++i) {
        g.fillRect(stringWidth, labelHeight + (i * fretHeight), stringWidth * (numberOfStrings - 1), 1);
    }

    // Base Fret
    if (this->baseFretDisplay <= 1) {
        int height = (int)(0.3 * fretHeight);
        g.fillRect(stringWidth, labelHeight + fretHeight - height, stringWidth * (numberOfStrings - 1) + 1, height);
    } else {
        g.setFont(Font(fretHeight));
        auto bounds = Rectangle<int>(0, labelHeight + fretHeight, stringWidth - (fretHeight * 0.3f), fretHeight);
        g.drawFittedText(String(this->baseFretDisplay), bounds, Justification::centredLeft, 1, 0.5f);
    }

    // Note markers
    x = stringWidth;
    float size = fretHeight * 0.6f;
    float adjustment = size * 0.5f;
    for (int i = 0; i < fretboard.size(); ++i) {
        auto bounds = Rectangle<int>(x - (stringWidth * 0.5), labelHeight - (fretHeight * 0.3), stringWidth, fretHeight);
        if (fretboard[i]->fret <= 0) {
            String character;
            Font font (Font(fretHeight, Font::plain).withTypefaceStyle ("Regular"));
            if (fretboard[i]->fret == 0) {
                character = juce::String::charToString(0x25CB); // Circle
                font.setHeight(fretHeight * 1.5f);
            } else {
                character = "x";
                font.setHeight(fretHeight * 1.2f);
            }
            #if JUCE_WINDOWS
                font.setTypefaceName("Lucida Sans Unicode");
            #endif
            g.setFont(font);
            g.drawFittedText(character, bounds, Justification::centred, 1, 1.0f);
        } else {
            g.fillEllipse(x - adjustment, labelHeight + (fretHeight * 0.5) - adjustment + (fretHeight * fretboard[i]->fret), size, size);
        }
        x += stringWidth;
    }
}

void ChordDiagramFretboard::updateChord(String newChord, StringArray newChordNotes) {
    this->chord = newChord;
    // Convert to integers
     for (int i = 0; i < numberOfStrings; ++i) {
        if (newChordNotes[i].toLowerCase() == "x") {
            int note = -1;
            this->chordNotes.add(note);
        } else {
            this->chordNotes.add(newChordNotes[i].getIntValue());
        }
     }
    // Calculate base fret
    int base = 1;
    if (newChordNotes.size() == 7) {
       base = newChordNotes[6].getIntValue();
    } else { // Infer from lowest fret (excluding open)
        int lowestFret = 99;
        for (int i = 0; i < this->chordNotes.size(); ++i) {
            if (this->chordNotes[i] >= 0) { // Ignore muted notes
                int fret = this->chordNotes[i];
                if (fret < lowestFret) lowestFret = fret;
            } 
        }
        base = lowestFret < 3 ? 1 : lowestFret;
    }
    this->baseFret = base;
}

String ChordDiagramFretboard::getChord() {
    return chord;
}

void ChordDiagramFretboard::updateChordDiagram(int transpose = 0, FLAT_SHARP_DISPLAY accidental = original) {    
    // Calculate base fret to display
    int openNoteAdjust = chordNotes.contains(0) ? 1 : 0; // Special handling of a chord with open notes
    int transposedBaseFret = ((this->baseFret + transpose - openNoteAdjust - 1) % 11) + 1;
    if (transposedBaseFret < 0) transposedBaseFret = transposedBaseFret + 12;
    int fretAdjust = (transposedBaseFret - 1 ) % 2;

    // Adjust fretted notes if necessary
    for (int i = 0; i < chordNotes.size(); ++i) {
        int fretNumber = chordNotes[i] >= 0 ? chordNotes[i] + fretAdjust + openNoteAdjust : -1; // Don't adjust muted notes
        fretboard[i]->fret = fretNumber;
    }

    this->chordLabel = ChordPro::CP_Transpose(chord, transpose, accidental);
    this->baseFretDisplay = transposedBaseFret - fretAdjust;
    repaint();
}

void ChordDiagramFretboard::setDarkMode(bool isDarkMode) {
    darkMode = isDarkMode;
    repaint();
}

void ChordDiagramFretboard::allNotesOff() {
    for (int i = 0; i < fretboard.size(); ++i) {
        fretboard[i]->fret = 0;
    }
}

String ChordPro::CP_Transpose(String chord, int transpose, FLAT_SHARP_DISPLAY accidental) {
    if (transpose != 0) {
        // Split out bass note 
        StringArray chordParts = StringArray::fromTokens(chord,"/","");
        // Transpose
        for (int i = 0; i < chordParts.size(); ++i) {
            String root = CP_GetRootNote(chordParts[i]);
            int rootIndex = CP_GetRootNoteIndex(root);
            int newIndex = -1;
            
            if (rootIndex >= 0) {
                newIndex = rootIndex + transpose;
                if (newIndex < 0) {
                    newIndex += (NOTES_SHARP.size());
                } else if (newIndex > NOTES_SHARP.size() - 1) {
                    newIndex -= NOTES_SHARP.size();
                }
                chordParts.set(i, chordParts[i].replace(root, accidental == sharp ? NOTES_SHARP[newIndex] : NOTES_FLAT[newIndex]));
            }
        }
        chord = chordParts.joinIntoString("/");
    }
    return chord;
} 

String ChordPro::CP_GetRootNote(String chord) {
    String root;
    if (chord.substring(1,2) == "#" || chord.substring(1,2) == "b") {
        root = chord.substring(0,2);
    } else {
        root = chord.substring(0,1);
    }
    return root;
}

int ChordPro::CP_GetRootNoteIndex(String rootNote) {
    int rootIndex = -1;
    // Substitute any enharmonic equivalents
    int enharmonicIndex = NOTES_ENHARMONIC.indexOf(rootNote);
    if (enharmonicIndex >= 0) {
        rootNote = NOTES_ENHARMONIC_EQUIVALENT[enharmonicIndex];
    }
    if (rootNote.substring(1,2) == "#") {
        rootIndex = NOTES_SHARP.indexOf(rootNote);
    } else {
        rootIndex = NOTES_FLAT.indexOf(rootNote);
    }
    return rootIndex;
}