//#ifndef CHORDPRO_H_
//#define CHORDPRO_H_

#pragma once


#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Constants.h"

using namespace juce;

struct ChordDiagramKey
{
    bool keyOn = false;
};

class ChordDiagramKeyboard : public Component
{
public:
    ChordDiagramKeyboard ();
    void paint(Graphics& g) override;
    void updateChord(String newChord, StringArray newChordNotes);
    String getChord();
    void updateChordDiagram(int transpose, FLAT_SHARP_DISPLAY accidental);
    void updateKeyOnColour(Colour newColour);
    void allNotesOff();

   
private:
    OwnedArray<ChordDiagramKey> keyboard;
    int numberOfWhiteKeys = 7;
    int numberOfBlackKeys = 5;
    int octaves = 2;
    String chord = "";
    String chordLabel = "";
    StringArray chordNotes;
    Colour onColour = Colour(0xFFE5E5E5);
};

class ChordPro
{
public:
    String static CP_Transpose(String chord, int transpose, FLAT_SHARP_DISPLAY display);
    String static CP_GetRootNote(String chord);
    int static CP_GetRootNoteIndex(String rootNote);

};



//#endif