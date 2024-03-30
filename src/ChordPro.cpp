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
}

void ChordDiagramKeyboard::paint(Graphics& g) {
    auto bounds = getLocalBounds();
    int labelHeight = getHeight() * 0.35;
    int keyboardHeight = getHeight() * 0.65;
    g.setFont(Font(labelHeight * 0.8f));
    g.setColour(darkMode ? Colour(0xFFF0F0F0) : Colour(0xFF0F0F0F));

    g.drawFittedText(chordLabel, bounds.withHeight(labelHeight), Justification::centred, 1, 1.0f);
    g.setColour(darkMode ? Colour(0xFFD0D0D0) : Colour(0xFF1F1F1F));
    ///g.drawRect(bounds.withHeight(labelHeight));
    int pad = getHeight() * 0.05;
    int x = 0;
    int whiteWidth = bounds.getWidth() / (numberOfWhiteKeys * octaves);
    int blackWidth = whiteWidth * 0.6;
    int blackToWhiteX = blackWidth / 2;
    int whiteToBlackX = whiteWidth - (blackWidth / 2);
    //g.drawRect(0,0,whiteWidth*numberOfWhiteKeys * octaves, labelHeight);
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
            //x += (key == 4 || key == 11 ? whiteWidth - 1 : whiteWidth * 0.7 - 1);
            //x += whiteWidth;
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
        int keyIndex = chordNotes[i].getIntValue() + rootIndex + transpose;
        if (keyIndex >= keyboard.size()) {
            keyIndex -= keyboard.size();
        } else if (keyIndex < 0) {
            keyIndex += keyboard.size();
        }
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
        fretboard.add(new ChordDiagramNote());
    }
}

void ChordDiagramFretboard::paint(Graphics& g) {
    auto bounds = getLocalBounds();
    int labelHeight = getHeight() * 0.25;
    int fretboardHeight = getHeight() * 0.75;
    int fretboardWidth = fretboardHeight;
    int fretHeight = fretboardHeight / (numberOfFrets - 1);
    int stringWidth = fretHeight;
    g.setFont(Font(labelHeight));
    g.setColour(darkMode ? Colour(0xFFF0F0F0) : Colour(0xFF0F0F0F));

    g.drawFittedText(chordLabel, bounds.withHeight(labelHeight), Justification::centredTop, 1, 1.0f);
    //g.setColour(darkMode ? Colour(0xFFD0D0D0) : Colour(0xFF1F1F1F));
    g.setColour(darkMode ? Colour(0xFFF0F0F0) : Colour(0xFF0F0F0F));

    g.drawRect(bounds);
    int pad = getHeight() * 0.05;
    int x = 0;


    // Strings
    for (int i = 0; i < numberOfStrings; ++i) {
        g.fillRect(x, labelHeight, 1, fretHeight * (numberOfFrets - 1));
        x += stringWidth;
    }

    // Frets
    int y = labelHeight;
    for (int i = 0; i < numberOfFrets; ++i) {
        g.fillRect(0, y, stringWidth * (numberOfStrings - 1), 1);
        y += fretHeight;
    }
}

void ChordDiagramFretboard::updateChord(String newChord, StringArray newChordNotes) {
    chord = newChord;
    chordNotes.swapWith(newChordNotes);
}

String ChordDiagramFretboard::getChord() {
    return chord;
}

void ChordDiagramFretboard::updateChordDiagram(int transpose = 0, FLAT_SHARP_DISPLAY accidental = original) {
    StringArray chordParts = StringArray::fromTokens(chord,"/","");
    String root = ChordPro::CP_GetRootNote(chordParts[0]);
    int rootIndex = ChordPro::CP_GetRootNoteIndex(root);
    allNotesOff();
    for (int i = 0; i < chordNotes.size(); ++i) {
        int keyIndex = chordNotes[i].getIntValue() + rootIndex + transpose;
        if (keyIndex >= fretboard.size()) {
            keyIndex -= fretboard.size();
        } else if (keyIndex < 0) {
            keyIndex += fretboard.size();
        }
        fretboard[keyIndex]->noteOn = true;
    }
    chordLabel = ChordPro::CP_Transpose(chord, transpose, accidental);
    repaint();
}

void ChordDiagramFretboard::updateKeyOnColour(Colour newColour) {
    onColour = newColour;
}

void ChordDiagramFretboard::setDarkMode(bool isDarkMode) {
    darkMode = isDarkMode;
    repaint();
}

void ChordDiagramFretboard::allNotesOff() {
    for (int i = 0; i < fretboard.size(); ++i) {
        fretboard[i]->noteOn = false;
    }
}

String ChordPro::CP_Transpose(String chord, int transpose, FLAT_SHARP_DISPLAY accidental) {
    // Split out bass note 
    StringArray chordParts = StringArray::fromTokens(chord,"/","");
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

       /*
        if (chordParts[i].substring(1,2) == "#") {
            root = chordParts[i].substring(0,2);
            int rootIndex = NOTES_SHARP.indexOf(root);
            if (rootIndex >= 0) {
                newIndex = rootIndex + transpose;
                if (newIndex < 0) {
                    newIndex += (NOTES_SHARP.size());
                } else if (newIndex > NOTES_SHARP.size() - 1) {
                    newIndex -= NOTES_SHARP.size();
                }
                chordParts.set(i, chordParts[i].replace(root, accidental == flat ? NOTES_FLAT[newIndex] : NOTES_SHARP[newIndex]));
            }
        } //else if (chordParts[i].contains("b")) {
           else if (chordParts[i].substring(1,2) == "b") {
            root = chordParts[i].substring(0,2);
            int rootIndex = NOTES_FLAT.indexOf(root);
            if (rootIndex >= 0) {
                newIndex = rootIndex + transpose;
                if (newIndex < 0) {
                    newIndex += (NOTES_FLAT.size());
                } else if (newIndex > NOTES_FLAT.size() - 1) {
                    newIndex -= NOTES_FLAT.size();
                }
                chordParts.set(i, chordParts[i].replace(root, accidental == sharp ? NOTES_SHARP[newIndex] : NOTES_FLAT[newIndex]));
            }
        } else {
            root = chordParts[i].substring(0,1);
            int rootIndex = NOTES_SHARP.indexOf(root);
            if (rootIndex >= 0) {
                newIndex = rootIndex + transpose;
                if (newIndex < 0) {
                    newIndex += (NOTES_SHARP.size());
                } else if (newIndex > NOTES_SHARP.size() - 1) {
                    newIndex -= NOTES_SHARP.size();
                }
                chordParts.set(i, chordParts[i].replace(root, accidental == flat ? NOTES_FLAT[newIndex] : NOTES_SHARP[newIndex]));
            }
        }
        */
    
    chord = chordParts.joinIntoString("/");
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
    if (rootNote.substring(1,2) == "#") {
        rootIndex = NOTES_SHARP.indexOf(rootNote);
    } else {
        rootIndex = NOTES_FLAT.indexOf(rootNote);
    }
    return rootIndex;
}