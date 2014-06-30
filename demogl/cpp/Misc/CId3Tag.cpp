#include "Misc\CId3Tag.h"
#include <stdio.h>

/* ----------------------------------------------------------
   CId3Tag class is used to retrieve and to save(!!) an
   information from an ID3v1.0/v1.1 tag and load thatinto
   a usable structure.

   This code will be well commented, so that everyone can
   understand, as it's made for the public and not for
   private use, although private use is allowed. :)

   all functions specified both in the header and .cpp file
   will have explanations in both locations.

   everything here by: Gustav "Grim Reaper" Munkby
                       http://home.swipnet.se/grd/
                       grd@swipnet.se
   ---------------------------------------------------------- */

// this function is specially designed to take
// care of ID3 tags generated with spaces instead
// of null characters as they are supposed to.
void gStrCpy( char* dst, char* src, int length ) {

    // what this does is that it searches for a
    // the first non-space or non-null from the
    // right side (and while doing so replacing)
    // " " with '\0'
    for	( int i = (length-1); i >= 0; i-- ) {

        if ( (src[i] == 0x20) || (src[i] == '\0') ) dst[i] = '\0';

        else break;
            
    }

    // this should copy the remaining part straight over
    memcpy(dst, src, (i+1) );

    // set last byte to zero to ensure string compability
    dst[length] = '\0';

}


BOOL CId3Tag::loadTag( char inputtag[128] ) {

    // in the ID3 tag the first 3 bytes are always TAG
    // so that you can separate a TAGed file from a
    // non TAGed one
    if ( memcmp(inputtag, "TAG", 3) ) return false;

    // using special string-copying routine
    // to copy the title string
    gStrCpy( title, inputtag+3, 30 );

    // using special string-copying routine
    // to copy the artist string
    gStrCpy( artist, inputtag+33, 30 );

    // using special string-copying routine
    // to copy the title string
    gStrCpy( album, inputtag+63, 30 );

    // because I store year as an integer
    // and the ID3 tag stores it as a string
    // the year will be handled like this:
    char yearstr[4];
    memcpy( yearstr, inputtag+93, 4);
    year = atoi( yearstr );

    // now lets detect whether it's version 1.1
    // or 1.0 (if there is tracknumber info or not)
    // if there is some tracknumber info, then it's in
    // the last char of the comment field, and the
    // one before is null
    if ( (inputtag[125]=='\0' ) && (inputtag[126] != '\0') ) {

        version = (float)1.1;
        trackNumber = (int)inputtag[126];

        // using special string-copying routine
        // to copy the comment string
        gStrCpy(comment, inputtag+97, 28);

    } else {
        
        version = (float)1.0;

        // using special string-copying routine
        // to copy the title string
        gStrCpy(comment, inputtag+97, 30);

    }

    genre = inputtag[127];

    return true;

}



BOOL CId3Tag::saveTag (char outputtag[128]) {

    
    // begin with clearing the values of the memory
    // containing the "id3string", mainly to ensure
    // that any "un-set" chars is 0 and nothing else
    memset(outputtag,0,128);

    // to append the rest of the strings I'll
    // use memcpy like this:
    //
    // memcpy(outputtag + [IN-TAG-LOCATION], [SOURCE], [SOURCE-LENGTH]);
    //
    // shouldn't be to hard to understand, look beneath:

    // first of all, we add the identifier
    memcpy(outputtag, "TAG", 3);

    // apending title
    gStrCpy(outputtag+3, title, 30 );
    
    // inserting the artist information
    gStrCpy(outputtag+33, artist, 30 );
    
    // add the album name to the id3 tag
    gStrCpy(outputtag+63, album, 30 );

    // this ones got to be 4 chars long, in my code, just to make the code simpler
    // and exactly who's got music recorded older than 1000 years? (and who's got
    // music recorded from the 101th century?)

    // turn integer into string (using char nr 5 for null)
    char yearstring[5];
    _itoa(year,yearstring,10);

    // add the four chars describing the year
    memcpy(outputtag + 93, yearstring, 4 );

    // add 30 chars of comment
    gStrCpy(outputtag+97, comment, 30 );
    
    if (version==(float)1.1) { // if version equals 1.1 we've got a tracknumber

        // make sure comment[28] == '\0' & then set comment[29] to trackNumber
        outputtag[125] = '\0';
        outputtag[126] = (UCHAR)(UINT)trackNumber;

    }

    // set the char desiding the genre
    outputtag[127] = genre;

    return TRUE;
}


// get title info
void CId3Tag::getTitle(char* input) {

    strcpy (input, title);

}
// set title info
void CId3Tag::setTitle(char* input) {

    strcpy (title, input);

}

// get artist info
void CId3Tag::getArtist(char* input) {

    strcpy (input, artist);

}
// set artist info
void CId3Tag::setArtist(char* input) {

    strcpy (artist, input);

}

// get album info
void CId3Tag::getAlbum(char* input) {

    strcpy (input, album);

}
// set album info
void CId3Tag::setAlbum(char* input) {

    strcpy (album, input);

}

// get comment info
void CId3Tag::getComment(char* input) {

    strcpy (input, comment);

}
// set comment info
void CId3Tag::setComment(char* input) {

    strcpy (comment, input);

}

// get/set genre string
// to use this function to get info
// specify true as second parameter
// if you want to set then make it false
void CId3Tag::genreString(char* input, BOOL get = true) {

    // this table of constant strings will be used in all cases..
    const char* table[126] = {
                              "Blues","Classic Rock","Country","Dance","Disco","Funk","Grunge","Hip-Hop","Jazz",
                              "Metal","New Age","Oldies","Other","Pop","R&B","Rap","Reggae","Rock","Techno",
                              "Industrial","Alternative","Ska","Death Metal","Pranks","Soundtrack","Euro-Techno",
                              "Ambient","Trip-Hop","Vocal","Jazz+Funk","Fusion","Trance","Classical","Instrumental",
                              "Acid","House","Game","Sound Clip","Gospel","Noise","AlternRock","Bass","Soul","Punk",
                              "Space","Meditative","Instrumental Pop","Instrumental Rock","Ethnic","Gothic",
                              "Darkwave","Techno-Industrial","Electronic","Pop-Folk","Eurodance","Dream",
                              "Southern Rock","Comedy","Cult","Gangsta","Top 40","Christian Rap","Pop/Funk","Jungle",
                              "Native American","Cabaret","New Wave","Psychadelic","Rave","Showtunes","Trailer",
                              "Lo-Fi","Tribal","Acid Punk","Acid Jazz","Polka","Retro","Musical","Rock & Roll",
                              "Hard Rock","Folk","Folk-Rock","National Folk","Swing","Fast Fusion","Bebob","Latin",
                              "Revival","Celtic","Bluegrass","Avantgarde","Gothic Rock","Progressive Rock",
                              "Psychedelic Rock","Symphonic Rock","Slow Rock","Big Band","Chorus","Easy Listening",
                              "Acoustic","Humour","Speech","Chanson","Opera","Chamber Music","Sonata","Symphony",
                              "Booty Bass","Primus","Porn Groove","Satire","Slow Jam","Club","Tango","Samba",
                              "Folklore","Ballad","Power Ballad","Rhythmic Soul","Freestyle","Duet","Punk Rock",
                              "Drum Solo","Acapella","Euro-House","Dance Hall"
                             };

    if (get) {

        // if get operation then check whether it's supported
        // and if it is set it according table above
        if (genre>=126)    strcpy(input, "not supported");
        else               strcpy (input, table[genre]);

    }
    else {

        for (int i=0; i<126; i++) {

            // search table for match
            if ( !_stricmp(input, table[i]) ) {

                genre=(UCHAR)(UINT)i;
                return; // end function

            }

        }
        // you'll only get here if no match is found

        // alert error message
        char buf[200];
        sprintf(buf,"the genre %s couldn't be found\ndo you want to set it to \"not supported\"?",input);
        int ret = MessageBox(0,buf, "MP3Info Example",MB_YESNO | MB_ICONQUESTION);

        // from the error message you've got two possibilities
        // 1/ answer no and the genre will remain unchanged
        // 2/ answer yes and the genre will be altered to "not supported"
        // set genre to something unspecified (in this case 255)
        if (ret==IDYES) genre = 255;

    }

}
// set the genre index info
void CId3Tag::setGenreIndex(int input) {

    genre = (UCHAR)(input%256);

}
// set the track number info
// to remove a track number execute this
// function
void CId3Tag::setTrackNumber(int input) {

    // no number avaliable
    if (input==-1) { 
        
        version = (float)1.0;
        trackNumber = -1;

    } else if (input<0 || input>255) {

        // an invalid tracknumber was specified
        MessageBox(0,"The tracknumber, should be between 0 and 255", "MP3Info Example", MB_OK);

    } else { // no errors, set the tracknumber

        version = (float)1.1;
        trackNumber = input;

    }

}
// set year info
void CId3Tag::setYear(int input) {

    // check whether input is out of range
    if (input<1000 || input>3000) {
         
        char mybuf[200];
        sprintf(mybuf, "The way the year is stored, it's best\nto use a year between 1000 and 3000,\nPersonally I don't think other years is needed\n\n%d seams like a stupid number",input);

        MessageBox(0, mybuf, "MP3Info Example", MB_OK|MB_ICONEXCLAMATION);

    } else { // everything alright, just set it

        year = input;

    }

}