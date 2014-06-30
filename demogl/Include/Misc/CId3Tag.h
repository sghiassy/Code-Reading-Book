#ifndef CID3TAG_H
#define CID3TAG_H

#include <windows.h>

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


class CId3Tag {

    public:

    // this function takes 128 chars and if it's
    // an ID3 tag structure it'll be read into
    // this usable structure
    BOOL loadTag( char inputtag[128] );

    // this function saves the current settings
    // into the 128 chars sent to the function
    BOOL saveTag( char output[128] );

    // function to set & get Title info [ char[30] ]
    void  getTitle(char* input);
    void  setTitle(char* input);

    // function to set & get Artist info [ char[30] ]
    void  getArtist(char* input);
    void  setArtist(char* input);

    // function to set & get Album info [ char[30] ]
    void  getAlbum(char* input);
    void  setAlbum(char* input);

    // function to set & get Comment info [ char[30]/char[28] ]
    void  getComment(char* input);
    void  setComment(char* input);

    // functions to set & get the string for
    // the genre info.
    void  genreString(char* input, BOOL get);

    // functions to set & get the integer 
    // for the genre info
    int   getGenreIndex() { return (int)genre; };
    void  setGenreIndex(int input);

    // functions to set & get the year info [1000->3000]
    int   getYear() { return year; };
    void  setYear(int input);

    // functions to set & get the track number [1->255]
    int  getTrackNumber() { return (version==1.1)?trackNumber:-1; };
    void setTrackNumber(int input); 
    
    // function to get the version [1.0/1.1]
    float getVersion() { return version; };

    private:

    char  title  [31],
          artist [31],
          album  [31],
          comment[31];
    int   year;
    int   trackNumber; //ID3v1.1 specific
    float version;

    UCHAR genre;

};

#endif