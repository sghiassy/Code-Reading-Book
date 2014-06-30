#include "Misc\CMP3Info.h"
#include <stdio.h>
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

#define ERR_FILEOPEN    0x0001
#define ERR_NOSUCHFILE  0x0002
#define ERR_NOMP3FILE   0x0004
#define ERR_ID3TAG      0x0008


int 
CMP3Info::loadInfo(byte *byBuffer, long lFileSize)
{
    char headerchars[4]; // char variable used for header-loading
    char vbrchars[12];
    char tagchars[128]; // load tag as string

    // get file size, by setting the pointer in the end and tell the position
    fileSize = (int)lFileSize;

    int pos = 0; // current position in file...

    /******************************************************/
    /* search and load the first frame-header in the file */
    /******************************************************/
    

    do {
        // if no header has been found after 200kB
        // or the end of the file has been reached
        // then there's probably no mp3-file
        if((pos>(1024*200))||(pos>fileSize)) 
		{
            return ERR_NOMP3FILE;
        }

        // read in four characters
		strncpy(&headerchars[0], (char *)(byBuffer + pos),4);

        // move file-position forward
        pos++;
        // convert four chars to CFrameHeader structure
        header.loadHeader(headerchars);
    }
    while(!header.isValidHeader());  // test for correct header

    // to correct the position to be right after the frame-header
    // we'll need to add another 3 to the current position
    pos += 3;

    /******************************************************/
    /* check for an vbr-header, to ensure the info from a */
    /* vbr-mp3 is correct                                 */
    /******************************************************/

    
    // determine offset from first frame-header
    // it depends on two things, the mpeg-version
    // and the mode(stereo/mono)

    if( header.getVersionIndex()==3 ) 
	{  
		// mpeg version 1
        if( header.getModeIndex()==3 ) pos += 17; // Single Channel
        else                           pos += 32;

    } else {                             // mpeg version 2 or 2.5

        if( header.getModeIndex()==3 ) pos +=  9; // Single Channel
        else                           pos += 17;

    }

    // read next twelve bits in
	strncpy(&vbrchars[0],(char *)(byBuffer + pos),12);

    // turn 12 chars into a CVBitRate class structure
    VBitRate = vbr.loadHeader(vbrchars);        

    /******************************************************/
    /* get tag from the last 128 bytes in an .mp3-file    */
    /******************************************************/

    // get last 128 bytes
	strncpy(&tagchars[0],(char *)(byBuffer + (fileSize-128)),128);
    // turn 128 chars into a CId3Tag structure
    Tagged = tag.loadTag(tagchars);
    return 0;
}


int CMP3Info::getBitrate() {

    if (VBitRate) {

        // get average frame size by deviding fileSize by the number of frames
        float medFrameSize = (float)fileSize / (float)getNumberOfFrames();
        
        /* Now using the formula for FrameSizes which looks different,
           depending on which mpeg version we're using, for mpeg v1:
        
           FrameSize = 12 * BitRate / SampleRate + Padding (if there is padding)

           for mpeg v2 the same thing is:

           FrameSize = 144 * BitRate / SampleRate + Padding (if there is padding)

           remember that bitrate is in kbps and sample rate in Hz, so we need to
           multiply our BitRate with 1000.

           For our purpose, just getting the average frame size, will make the
           padding obsolete, so our formula looks like:

           FrameSize = (mpeg1?12:144) * 1000 * BitRate / SampleRate;
        */

        return (int)( 
                     ( medFrameSize * (float)header.getFrequency() ) / 
                     ( 1000.0 * ( (header.getLayerIndex()==3) ? 12.0 : 144.0))
                    );

    }
    else return header.getBitrate();
}


float CMP3Info::getLengthInSeconds() {

    // kiloBitFileSize to match kiloBitPerSecond in bitrate...
    int kiloBitFileSize = (8 * fileSize) / 1000;
    
    return (float)(kiloBitFileSize/getBitrate());
}



int CMP3Info::getNumberOfFrames() {

    if (!VBitRate) {

        /* Now using the formula for FrameSizes which looks different,
           depending on which mpeg version we're using, for layer 1:
        
           FrameSize = 12 * BitRate / SampleRate + Padding (if there is padding)

           for layer 2 & 3 the same thing is:

           FrameSize = 144 * BitRate / SampleRate + Padding (if there is padding)

           remember that bitrate is in kbps and sample rate in Hz, so we need to
           multiply our BitRate with 1000.

           For our purpose, just getting the average frame size, will make the
           padding obsolete, so our formula looks like:

           FrameSize = (layer1?12:144) * 1000 * BitRate / SampleRate;
        */
           
        float medFrameSize = (float)( 
                                     ( (header.getLayerIndex()==3) ? 12 : 144 ) *
                                     (
                                      (1000.0 * (float)header.getBitrate() ) /
                                      (float)header.getFrequency()
                                     )
                                    );
        
        return (int)(fileSize/medFrameSize);

    }
    else return vbr.getNumberOfFrames();
}


void CMP3Info::getVersion(char* input) {

    char versionchar[32]; // temporary string
    char tempchar2[4]; // layer

    // call CFrameHeader member function
    float ver = header.getVersion();

    // create the layer information with the amounts of I
    for( int i=0; i<header.getLayer(); i++ ) tempchar2[i] = 'I';
    tempchar2[i] = '\0';

    // combine strings
    sprintf(versionchar,"MPEG %g Layer %s", (double)ver, tempchar2);

    // copy result into inputstring
    strcpy(input, versionchar);
}


void CMP3Info::getMode(char* input) {

    char modechar[32]; // temporary string

    // call CFrameHeader member function
    header.getMode(modechar);

    // copy result into inputstring
    strcpy(input, modechar);
}


void CMP3Info::getTitle(char* input) {

    char titlechar[32]; // temporary string

    // call CFrameHeader member function
    tag.getTitle(titlechar);

    // copy result into inputstring
    strcpy(input, titlechar);
}


void CMP3Info::getArtist(char* input) {

    char artistchar[32]; // temporary string

    // call CFrameHeader member function
    tag.getArtist(artistchar);

    // copy result into inputstring
    strcpy(input, artistchar);
}


void CMP3Info::getAlbum(char* input) {

    char albumchar[32]; // temporary string

    // call CFrameHeader member function
    tag.getAlbum(albumchar);

    // copy result into inputstring
    strcpy(input, albumchar);
}


void CMP3Info::getComment(char* input) {

    char commentchar[32]; // temporary string

    // call CFrameHeader member function
    tag.getComment(commentchar);

    // copy result into inputstring
    strcpy(input, commentchar);
}


void CMP3Info::getGenre(char* input) {

    char genrechar[32]; // temporary string

    // call CFrameHeader member function
    tag.genreString(genrechar, true);

    // copy result into inputstring
    strcpy(input, genrechar);
}

