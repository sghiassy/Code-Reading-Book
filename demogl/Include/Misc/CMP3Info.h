#ifndef CMP3INFO_H
#define CMP3INFO_H

#include <windows.h>
#include "Misc\CFrameHeader.h"
#include "Misc\CId3Tag.h"
#include "Misc\CVBitRate.h"

/* ----------------------------------------------------------
   CMP3Info class is your complete guide to the 
   MP3 file format in the C++ language. It's a large class
   with three different, quite large "sub-classes" so it's
   a fair amount of code to look into.

   This code will be well commented, so that everyone can
   understand, as it's made for the public and not for
   private use, although private use is allowed. :)

   all functions specified both in the header and .cpp file
   will have explanations in both locations.

   everything here by: Gustav "Grim Reaper" Munkby
                       http://home.swipnet.se/grd/
                       grd@swipnet.se
   ---------------------------------------------------------- */


class CMP3Info {

    public:

    // function to load a file into this structure
    // the argument passed is the path to a MP3 file
    int   loadInfo(byte *byBuffer, long lFileSize);

    // information that is avaliable in FrameHeader & VBR header
    void  getVersion(char* input);
    int   getBitrate();
    int   getFrequency() { return header.getFrequency(); };
    void  getMode(char* input);
    
    int   getNumberOfFrames();

    // functions to calculate the length of the song
    // and to present it nicely
    float getLengthInSeconds();
    
    // information that is avaliable in the ID3 tag
    void  getTitle(char* input);
    void  getArtist(char* input);
    void  getAlbum(char* input);
    void  getComment(char* input);
    void  getGenre(char* input);

    int   getYear() { return tag.getYear(); };
    int   getTrackNumber() { return tag.getTrackNumber(); };

    // just to know what kind of file it is.
    BOOL  isVBitRate() { return VBitRate; };
    BOOL  isTagged() { return Tagged; };
    
    private:

    // these are the "sub-classes"
    CFrameHeader header;
    CVBitRate    vbr;
    CId3Tag      tag;

    // just to know what kind of file it is
    BOOL VBitRate;
    BOOL Tagged;

    // the file information can not be found elsewhere
    int fileSize;

};

#endif
