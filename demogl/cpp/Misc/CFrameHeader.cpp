#include "Misc\CFrameHeader.h"

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



// This function is quite easy to understand, it loads
// 4 chars of information into the CFrameHeader class
// The validity is not tested, so qith this function
// an invalid FrameHeader could be retrieved
void CFrameHeader::loadHeader(char c[4]) {

    // this thing is quite interesting, it works like the following
    // c[0] = 00000011
    // c[1] = 00001100
    // c[2] = 00110000
    // c[3] = 11000000
    // the operator << means that we'll move the bits in that direction
    // 00000011 << 24 = 00000011000000000000000000000000
    // 00001100 << 16 =         000011000000000000000000
    // 00110000 << 24 =                 0011000000000000
    // 11000000       =                         11000000
    //                +_________________________________
    //                  00000011000011000011000011000000

    bithdr = (unsigned long)(
                              ( (c[0] & 255) << 24) |
                              ( (c[1] & 255) << 16) |
                              ( (c[2] & 255) <<  8) |
                              ( (c[3] & 255)      )
                            ); 

}


// This function is a supplement to the loadHeader
// function, the only purpose is to detect if the
// header loaded by loadHeader is a valid header
// or just four different chars
BOOL CFrameHeader::isValidHeader() {

    return ( ((getFrameSync()      & 2047)==2047) &&
             ((getVersionIndex()   &    3)!=   1) &&
             ((getLayerIndex()     &    3)!=   0) && 
             ((getBitrateIndex()   &   15)!=   0) &&  // due to lack of support of the .mp3 format
                                                      // no "public" .mp3's should contain information
                                                      // like this anyway... :)
             ((getBitrateIndex()   &   15)!=  15) &&
             ((getFrequencyIndex() &    3)!=   3) &&
             ((getEmphasisIndex()  &    3)!=   2)    );

}

// this returns the MPEG version [1.0-2.5]
float CFrameHeader::getVersion() {

    // a table to convert the indexes into
    // something informative...
    float table[4] = {
                      2.5, 0.0, 2.0, 1.0
                     };

    // return modified value
    return table[getVersionIndex()];

}


// this returns the Layer [1-3]
int CFrameHeader::getLayer() {

    // when speaking of layers there is a 
    // cute coincidence, the Layer always
    // eauals 4 - layerIndex, so that's what
    // we will return
    return ( 4 - getLayerIndex() );

}


// this returns the current bitrate [8-448 kbps]
int CFrameHeader::getBitrate() {

    // a table to convert the indexes into
    // something informative...
    const int table[2][3][16] = {
        {         //MPEG 2 & 2.5
            {0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0}, //Layer III
            {0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0}, //Layer II
            {0, 32, 48, 56, 64, 80, 96,112,128,144,160,176,192,224,256,0}  //Layer I
        },{       //MPEG 1
            {0, 32, 40, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,0}, //Layer III
            {0, 32, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,384,0}, //Layer II
            {0, 32, 64, 96,128,160,192,224,256,288,320,352,384,416,448,0}  //Layer I
        }
    };

    // the bitrate is not only dependent of the bitrate index,
    // the bitrate also varies with the MPEG version and Layer version
    return table[(getVersionIndex() & 1)][(getLayerIndex() -1)][getBitrateIndex()];

}


// this returns the current frequency [8000-48000 Hz]
int CFrameHeader::getFrequency() {

    // a table to convert the indexes into
    // something informative...
    int table[4][3] = {

        {32000, 16000,  8000}, //MPEG 2.5
        {    0,     0,     0}, //reserved
        {22050, 24000, 16000}, //MPEG 2
        {44100, 48000, 32000}  //MPEG 1

    };

    // the frequency is not only dependent of the bitrate index,
    // the bitrate also varies with the MPEG version
    return table[getVersionIndex()][getFrequencyIndex()];

}

// the purpose of getMode is to get information about
// the current playing mode, such as:
// "Joint Stereo"
void CFrameHeader::getMode(char* input) {

    // here you could use a array of strings instead
    // but I think this method is nicer, at least
    // when not dealing with that many variations
    switch(getModeIndex()) {
        default:
            strcpy(input, "Stereo");
            break;

        case 1:
            strcpy(input, "Joint Stereo");
            break;

        case 2:
            strcpy(input, "Dual Channel");
            break;

        case 3:
            strcpy(input, "Single Channel");
            break;
    }

}