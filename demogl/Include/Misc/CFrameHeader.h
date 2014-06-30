#ifndef CFRAMEHEADER_H
#define CFRAMEHEADER_H

#include <windows.h>

/* ----------------------------------------------------------
   CFrameHeader class is used to retrieve a MP3's FrameHeader
   and load that into a usable structure.

   This code will be well commented, so that everyone can
   understand, as it's made for the public and not for
   private use, although private use is allowed. :)

   all functions specified both in the header and .cpp file
   will have explanations in both locations.

   everything here by: Gustav "Grim Reaper" Munkby
                       http://home.swipnet.se/grd/
                       grd@swipnet.se
   ---------------------------------------------------------- */


class CFrameHeader {

    public:

    // This function is quite easy to understand, it loads
    // 4 chars of information into the CFrameHeader class
    // The validity is not tested, so qith this function
    // an invalid FrameHeader could be retrieved
    void loadHeader(char c[4]);

    // This function is a supplement to the loadHeader
    // function, the only purpose is to detect if the
    // header loaded by loadHeader is a valid header
    // or just four different chars
    BOOL isValidHeader();

    // Following är functions to get the "indexes" for the various
    // information avaliable. To know which meaning the different
    // bits you need to look into a table, instead of having to
    // do this everytime these functions get the correct index
    // from the correct bits. :)
    int getFrameSync()     { return ((bithdr>>21) & 2047); };
    int getVersionIndex()  { return ((bithdr>>19) & 3);  };
    int getLayerIndex()    { return ((bithdr>>17) & 3);  };
    int getProtectionBit() { return ((bithdr>>16) & 1);  };
    int getBitrateIndex()  { return ((bithdr>>12) & 15); };
    int getFrequencyIndex(){ return ((bithdr>>10) & 3);  };
    int getPaddingBit()    { return ((bithdr>> 9) & 1);  };
    int getPrivateBit()    { return ((bithdr>> 8) & 1);  };
    int getModeIndex()     { return ((bithdr>> 6) & 3);  };
    int getModeExtIndex()  { return ((bithdr>> 4) & 3);  };
    int getCoprightBit()   { return ((bithdr>> 3) & 1);  };
    int getOrginalBit()    { return ((bithdr>> 2) & 1);  };
    int getEmphasisIndex() { return ((bithdr    ) & 3);  };


    // now comes som function to make life easier once again
    // you don't even have to know what the different indexes
    // means. to get the version, just use the function
    // getVersion. You can't have it easier
       

    // this returns the MPEG version [1.0-2.5]
    float getVersion();

    // this returns the Layer [1-3]
    int  getLayer();

    // this returns the current bitrate [8-448 kbps]
    int  getBitrate();

    // this returns the current frequency [8000-48000 Hz]
    int getFrequency();

    // the purpose of getMode is to get information about
    // the current playing mode, such as:
    // "Joint Stereo"
    void getMode(char* input);

    private:

    // this contains the orginal header (bit-by-bit) information
    // declared private because there is not really any reason
    // to use it, as all the "indexes" functions exists
    unsigned long bithdr;

};


#endif