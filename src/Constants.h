// Lyrics and Chords extension for Gig Performer by @rank13

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// Script logger prefix
const std::string LOG_PREFIX = "[GPLyricsChords] ";

// Define the file separator (Windows vs Mac) to be used when reading the preferences file
inline char PATH_SEPARATOR() {
    #if JUCE_WINDOWS
        return '\\';
    #else
        return '/';
    #endif
}

// Window title bar and header labels
const std::string SONG_TITLE = "Songs";
const std::string SONG_WINDOW_TITLE = "GP Lyrics/Chords";

// Widget names used in the extension
const std::string WIDGET_DISPLAY = "GPLC_OPEN";
const std::string WIDGET_SCROLL = "GPLC_SONG_SCROLL";
const std::string WIDGET_PREVIOUS = "GPLC_PREV";
const std::string WIDGET_NEXT = "GPLC_NEXT";
const std::string WIDGET_CP_SCROLL = "GPLC_SCROLL";
const std::string WIDGET_CP_UP = "GPLC_UP";
const std::string WIDGET_CP_DOWN = "GPLC_DOWN";

// List of menu items
const std::vector<std::string> menuNames = { "Show Window", "Preferences..."};

// Preferences file
const std::string PREF_FILENAME = "GPLyricsChordsPrefs.txt";

// Default number of rackspaces/song and variations/song part buttons that are created by the extension
const int DEFAULT_RACKSPACES_SONGS = 50;
const int DEFAULT_VARIATIONS_SONGPARTS = 10;
const int DEFAULT_SETLISTS = 10;

// GUI defaults
const std::string DEFAULT_WINDOW_POSITION = "80,80,1000,800";
const juce::String DEFAULT_BUTTON_COLOR = "FF3F3F3F";
const juce::String DEFAULT_SUBBUTTON_COLOR = "FF353535";
const juce::String DEFAULT_BORDER_COLOR = "FFE5E5E5";
const juce::String BACKGROUND_COLOR = "FF2A2A2A";
const juce::String HEADER_SONG_COLOR = "FF1E1E1E";
const juce::String HEADER_RACKSPACE_COLOR = "FF2C5E1E";
const juce::String NO_CHORDPRO_MESSAGE = "No ChordPro file found. Use Gig Performer's Song Lyrics/Chords Editor to add a ChordPro file.";
const juce::String WINDOW_ICON = "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAABf2lDQ1BzUkdCIElFQzYxOTY2LTIuMQAAKJF1kc8rRFEUxz8zQ8SIIllYTMJqyI+a2CgzaahJGqMMNm+e+aFmxuu9J022ynaKEhu/FvwFbJW1UkRK1tbEBj3nGjWSObdzz+d+7z2ne88Fdyyr56yqPsjlbTMaDvpm43O+mieqacVLE35Nt4zRqakIFe3tFpeK1z2qVuVz/1r9YtLSwVUrPKIbpi08LhxZtQ3FW8ItekZbFD4R9ptyQeEbpSdK/KQ4XeIPxWYsGgJ3k7Av/YsTv1jPmDlheTmdueyK/nMf9RJvMj8zLbFDvB2LKGGC+JhgjBAB+hmWOUAPA/TKigr5fd/5kyxLri6zQQGTJdJksPGLuiLVkxJToidlZCmo/v/tq5UaHChV9wah+tFxXrqgZhM+i47zfuA4n4fgeYDzfDl/eR+GXkUvlrXOPWhch9OLspbYhrMNaLs3NFP7ljzi7lQKno+hIQ7NV1A3X+rZzz5HdxBbk6+6hJ1d6JbzjQtfVgtn3qo5yfIAAAAJcEhZcwAAPYQAAD2EAdWsr3QAAAZASURBVFiFtZdfSFTbHsc/+8/MME5XMB2d0UQSlMrLpTC99CIHgh6iQqJLcHvQh7xIWEFEWBT0aPgSPgQRx8vN4F4IIoSCwAdfJLoFjkdjskgq/0xpaqPOOHvPWnvdh87ezKQePffUD36w2Wv9fuu7v7/f+q69NL6xpaWlvwMHgAbgz0Dg2zm/0yxgFPgvcKuwsHA8d1BzHxYXF/8E/KyU+tuLFy+IxWK8efMGy7L+0OqBQIDa2lr27dtHfX09wL+B00VFRWkPwNzcXAj4ZWpqqvry5cvE43FCoRAFBQVomrZx9i2YUop0Ok0qlaKuro6uri6i0ehr4C/hcNjSABKJxH8SicTJw4cPEw6HiUajOI7zhxb+1nRdZ2ZmhsXFRR4/fkxpaem/otFoqzY5OVkNvG1paeHjx49Eo1GUUt91cdc0TWNmZoaqqiru3LkDUKFLKc/EYjFGRkYoKytDSonjOD/EpZREIhGePXtGPB5HSnlGF0I0DA8PEwqFcBwHpdQPdcdxCIVCDA8PI4RoMIUQdePj4wSDwTXUl5aW0tTUREVFBeFwOG9scHCQgYEB9u7dy8mTJzFNE5/Ph2mamKaJpmkYhoHjOMTjce7du8fc3BwAwWCQ8fFxhBB1phDCl81mAfIAHD16lBMnTmAYxpo6AhQWFqKUIhQKUV1djd/vx+/3eyB0XUfTNJRS1NTUIISgp6fHy2NZFkIIvymEyKMeoKGhgba2NjKZDJZl8fTpU969e4fP56OyspL9+/d7gJVSGIaBaZr09vby5csXD2RFRQWtra1IKRFCePndOCEEeQAcx0HTNC5cuIDP5yOdTnP9+nVev36dx0JhYSHFxcVenPvlQ0NDTE1NefMaGxtx8w8MDHhb213LA5DbJNu3b6eiogIhBA8ePGB8PE85AUgmkySTSa8kPp8Pn8+XV0a/3097eztCCMbGxojH43klzgOQi6yqqgoA0zQZGRlZVxMuXrxIdXU1APfv38fn82EYRl4ZW1paKC8vZ2Vlhe7u7jxhyy2B/i0DCwsLSCkByGaz626l9+/fU1xcTGlpKZqmYZqmR6tSit27d9Pc3Ixt2/T29jI5ObnudhRCoLvC476cmJhgeXkZIQS7du1aV1DS6TSGYRAIBDw6bdvGcRz8fj/nz5/Htm1isRgPHz5cE++CkFKiZ7PZNS/HxsawLIs9e/ZsKCiBQIBgMIiUkkwmQyaTQUpJa2srJSUlrKys0NPTg5RyTSyAlJJsNru2BEopRkdHWV1d5cCBA1RWVq4Z13Wdbdu2YRgGtm2TSqVIpVJEIhEOHjxINpvl7t27TE9P/6Yqej2Q24RKKZ48eUIymcS2bU6fPp03ppTymm55eZmsEGQyGYQQBAIBT/kePXq0qSTnAcj1Dx8+0NfXRzabpa6ujqampjVzPn/+zPLyMs6vDeuCtCzLo36jQ2kNgG8VSilFf38/L1++BKCjo4P6+npvzLbtr7X9NZmrB0op+vr6SCQSv/n1uTqgr9ckLsKbN29iWRaBQIBr167R2Njo9YBpmuiGgXIUmqahaRqJRGJT6nNdSrl+D7j+6dMnrly5QjKZpKCggBs3bnD16lWKioowDOPrV/MVEEAqldryke4x4G7DjfzVq1d0dHQwMTGBpmkcOnSIU6dOYRiGp36apqHrupd4M3fnedtwIwZymejo6GBoaMhrOPfEc0uSC2Izd+PcJrR1Xd+UulQqxaVLl2hvb2dwcBApJZqu5TFQUlKyZfp1XUcIYZlCiLFIJPLT6Oioh24jU0oRi8WIxWKUl5dz5MgRlpaWvOM6EolsmgMgk8lQVlaGEGLMFEI837Fjx0+rq6tbCnZtenqa27dvA9DW1rblOBdAZWUlQojnuhDiViQSYefOnczPz/+wP2LX5+fnqa2tpbi4GCHELb2zs/OdEKLv+PHjZDIZFhcXt7yPf68vLCxg2zbNzc0IIX7u7Oz8aAIEg8F/SCn/evbs2dr+/n7evn1LIBDA7/d/l6uZbdtYlkVNTQ3Hjh3D7/fHo9FoO+RcTru7u4PpdPqOpmmnZmdnmZmZYXZ2FveP+f81n89HWVkZ5eXlhMNhlFL/LCoqaj937pydB8C1zs7OKuAM3/d6/gvwHLjV1dU1mTv4P/15p6Ed/VZ7AAAAAElFTkSuQmCC";
const juce::String MENU_ICON = "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";
const juce::String TOGGLE_ON = "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";
const juce::String TOGGLE_OFF = "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";

const int DEFAULT_SONG_LIST_WIDTH = 250;
const int MIN_WINDOW_WIDTH = 180;
const int SCROLL_AREA_WIDTH = 50;
const int HEADER_HEIGHT = 50;
const int PANE_SEPARATOR_WIDTH = 15;

// ChordPro defaults
const float CP_DEFAULT_FONT_SIZE = 0.7f;
const float CP_MIN_FONT_SIZE = 0.4f;
const float CP_MAX_FONT_SIZE = 1.6f;
const float CP_FONT_STEP_SIZE = 0.1f;
const int CP_TOP_PADDING = 20;
const int CP_KEYBOARD_HEIGHT = 120;
const int CP_KEYBOARD_WIDTH = 220;
const int CP_KEYBOARD_SEPARATOR = 20;
const int CP_FRETBOARD_HEIGHT = 160;
const int CP_FRETBOARD_WIDTH = 160;
const int CP_FRETBOARD_SEPARATOR = 10;
const int CP_DEFAULT_LEFT_MARGIN = 50;
const int CP_EXPANDED_LEFT_MARGIN = 250;
const int CP_EDITOR_DEFAULT_X = 600;

const juce::String CP_LIGHT_LYRIC_COLOR = "FF000000";
const juce::String CP_LIGHT_CHORD_COLOR = "FF1496F1"; 
const juce::String CP_LIGHT_BACKGROUND_COLOR = "FFFFFFFF";
const juce::String CP_LIGHT_IMAGES_BACKGROUND_COLOR = "FFF0F0F0";
const juce::String CP_DARK_LYRIC_COLOR = "FFFFFFFF";
const juce::String CP_DARK_CHORD_COLOR = "FF1496F1";
const juce::String CP_DARK_BACKGROUND_COLOR = "FF000000";
const juce::String CP_DARK_IMAGES_BACKGROUND_COLOR = BACKGROUND_COLOR;

const juce::String CP_DARK_MODE_FILE_SUFFIX = "-DarkMode";
const juce::String CP_NO_DARK_MODE_IMAGES = "No dark mode image(s)";
const juce::String CP_DARK_MODE_IMAGE_PROCESSING = "Creating image(s)...";

const juce::StringArray NOTES_SHARP = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
const juce::StringArray NOTES_FLAT = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
const juce::StringArray NOTES_ENHARMONIC = {"E#", "B#", "Cb", "Fb"};
const juce::StringArray NOTES_ENHARMONIC_EQUIVALENT = {"F", "C", "B", "E"};

typedef enum FLAT_SHARP_DISPLAY {original, flat, sharp} FLAT_SHARP_DISPLAY;

// Timer callback defaults
const int TIMER_CLOCK = 5000;
const int TIMER_UI_REFRESH = 1000;
const int TIMER_WINDOW_STATE = 1000;
const int TIMER_IMAGE_CONVERT = 300;
const int TIMER_AUTOSCROLL = 20;
#endif