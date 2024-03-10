//#ifndef CHORDPRO_H_
//#define CHORDPRO_H_

#pragma once


#include <juce_core/juce_core.h>
#include "Constants.h"

using namespace juce;

//const StringArray NOTES_SHARP = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
//const StringArray NOTES_FLAT = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};

class ChordPro
{
public:
String static CP_Transpose(String chord, int transpose, FLAT_SHARP_DISPLAY display);
/*
String CP_Transpose(String chord, int transpose) {
    // Split out bass note 
          StringArray chordParts = StringArray::fromTokens(chord,"/","");
        
          for (int i = 0; i < chordParts.size(); ++i) {
            String root;
            int newIndex = -1;
            if (chordParts[i].contains("#")) {
              root = chordParts[i].substring(0,2);
              newIndex = NOTES_SHARP.indexOf(root) + transpose;
              if (newIndex < 0) {
                newIndex += (NOTES_SHARP.size());
              } else if (newIndex > NOTES_SHARP.size() - 1) {
                newIndex -= NOTES_SHARP.size();
              }
              if (newIndex != -1) chordParts.set(i, chordParts[i].replace(root, NOTES_SHARP[newIndex]));
            } else if (chordParts[i].contains("b")) {
              root = chordParts[i].substring(0,2);
              newIndex = NOTES_FLAT.indexOf(root) + transpose;
              if (newIndex < 0) {
                newIndex += (NOTES_FLAT.size());
              } else if (newIndex > NOTES_FLAT.size() - 1) {
                newIndex -= NOTES_FLAT.size();
              }
              if (newIndex != -1) chordParts.set(i, chordParts[i].replace(root, NOTES_FLAT[newIndex]));
            } else {
              root = chordParts[i].substring(0,1);
              newIndex = NOTES_SHARP.indexOf(root) + transpose;
              if (newIndex < 0) {
                newIndex += (NOTES_SHARP.size());
              } else if (newIndex > NOTES_SHARP.size() - 1) {
                newIndex -= NOTES_SHARP.size();
              }
              if (newIndex != -1) chordParts.set(i, chordParts[i].replace(root, NOTES_SHARP[newIndex]));
            }
          }
          chord = chordParts.joinIntoString("/");
          return chord;
} 
*/

};

//#endif