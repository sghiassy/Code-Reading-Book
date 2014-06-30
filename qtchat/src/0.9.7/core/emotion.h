#ifndef _EMOTIONS_H
#define _EMOTIONS_H

#include "prototypes.h"
#include "util.h"
#include <qlistbox.h>
#include <stdlib.h>
extern "C" {
#include <string.h>
}

class Emotion : public QListBoxItem {
	public:
		Emotion(const char *emotiontext, const char *displaytext);
		Emotion(const Emotion &e);
		~Emotion();

	// Accessors
		char *getEmotionText() const {
			return strdup(emotion_text);
		}
		char *getDisplayText() const {
			return strdup(display_text);
		}

	// Mutators
		void setEmotionText(const char *text) {
			free(emotion_text);
			emotion_text = strdup(text);
		}
		void setDisplayText(const char *text) {
			free(display_text);
			display_text = strdup(text);
		}
	protected:
		char *emotion_text;
		char *display_text;
};


class EmotionsWidget : public QListBox {
	public:
		EmotionsWidget(QWidget *parent=0, const char *name=0, WFlags f=0);
		~EmotionsWidget();
	protected:
};

#endif	// _EMOTIONS_H
