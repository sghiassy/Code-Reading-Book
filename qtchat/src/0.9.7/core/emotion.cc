#include "debug.h"
#include "emotion.h"
#include <iostream.h>

	Emotion::Emotion(const char *emotiontext, const char *displaytext) :
		QListBoxItem() {

		if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
			cerr << "Emotion(\"" << emotiontext << "\") constructor called." << endl;
		if(emotiontext)
			emotion_text = strdup(emotiontext);
		else
			emotion_text = strdup("");
		if(displaytext)
			display_text = strdup(displaytext);
		else
			display_text = strdup("");
	}
		
	Emotion::Emotion(const Emotion &e) {
		if(::debuglevel & DEBUGLEVEL_COPYCONSTRUCTOR)
			cerr << "Emotion copy constructor called." << endl;

			display_text = strdup(e.display_text);
			emotion_text= strdup(e.emotion_text);
	}

	Emotion::~Emotion() {
		if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
			cerr << "Emotion destructor called." << endl;

		free(display_text);
		free(emotion_text);
	}


	EmotionsWidget::EmotionsWidget(QWidget *parent, const char *name, WFlags f) :
		QListBox(parent, name, f) {
		if(::debuglevel & DEBUGLEVEL_CONSTRUCTOR)
			cerr << "EmotionsWidget constructor called." << endl;

		}
			
	EmotionsWidget::~EmotionsWidget() {
		if(::debuglevel & DEBUGLEVEL_DESTRUCTOR)
			cerr << "EmotionsWidget destructor called." << endl;
	}
