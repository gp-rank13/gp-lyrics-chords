# Song/Lyrics Extension for Gig Performer

## Overview



> _Disclaimer: This is a user-created extension and is not supported by Deskew Technologies. Extensions can cause unexpected behaviour in Gig Performer, and in extreme situations could cause it to crash. Please test it thoroughly before using it in any live performances!_

## Installation

Download the extension (library) files for either MacOS or Windows from the [Releases](https://github.com/gp-rank13/gp-lyrics-chords/releases) page.  

Mac users will need to move the _libGPLyricsChords.dylib_ file into this folder and then restart Gig Performer:
```
/Users/Shared/Gig Performer/Extensions
```
Mac users will also need to authorize the extension via _System Preferences_ > 
_Privacy & Security_. Scroll down until you locate an entry that references the extension. Allow it to be loaded and then restart Gig Performer.

Windows users will need to move the _GPLyricsChords.dll_ file into this folder and then restart Gig Performer:
```
C:\Users\Public\Documents\Gig Performer\Extensions
```
When opening Gig Performer, a prompt will appear asking to confirm whether to install the extension.

## Building the Extension

To customize/build the extension, refer to the [Gig Performer SDK](https://github.com/gigperformer/gp-sdk) GitHub page.
