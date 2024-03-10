#include "ChordPro.h"

String ChordPro::CP_Transpose(String chord, int transpose, FLAT_SHARP_DISPLAY display) {
    // Split out bass note 
    StringArray chordParts = StringArray::fromTokens(chord,"/","");
    for (int i = 0; i < chordParts.size(); ++i) {
        String root;
        int newIndex = -1;
        if (chordParts[i].contains("#")) {
            root = chordParts[i].substring(0,2);
            int rootIndex = NOTES_SHARP.indexOf(root);
            if (rootIndex >= 0) {
                newIndex = rootIndex + transpose;
                if (newIndex < 0) {
                    newIndex += (NOTES_SHARP.size());
                } else if (newIndex > NOTES_SHARP.size() - 1) {
                    newIndex -= NOTES_SHARP.size();
                }
                chordParts.set(i, chordParts[i].replace(root, display == flat ? NOTES_FLAT[newIndex] : NOTES_SHARP[newIndex]));
            }
        } else if (chordParts[i].contains("b")) {
            root = chordParts[i].substring(0,2);
            int rootIndex = NOTES_FLAT.indexOf(root);
            if (rootIndex >= 0) {
                newIndex = rootIndex + transpose;
                if (newIndex < 0) {
                    newIndex += (NOTES_FLAT.size());
                } else if (newIndex > NOTES_FLAT.size() - 1) {
                    newIndex -= NOTES_FLAT.size();
                }
                chordParts.set(i, chordParts[i].replace(root, display == sharp ? NOTES_SHARP[newIndex] : NOTES_FLAT[newIndex]));
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
                chordParts.set(i, chordParts[i].replace(root, display == flat ? NOTES_FLAT[newIndex] : NOTES_SHARP[newIndex]));
            }
        }
    }
    chord = chordParts.joinIntoString("/");
    return chord;
} 