#ifndef _FONTDIALOG_H
#define _FONTDIALOG_H

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qgroupbox.h>
#include <qpushbutton.h>

class FontDialog : public QDialog {
	Q_OBJECT
	public:
		FontDialog(QFont initial_font, QWidget *parent=0, const char *name=0, WFlags f=0);
		~FontDialog();
	
		QFont getFont() const;	// return selected font

		static const char *fonts[];
		static const char *styles[];
		static const char *sizes[100-7];

	public slots:
		void loadSystemFonts();
		void setSampleFont(QFont f);

	private slots:
		void handleChangeEvent();

	protected:
		virtual void paintEvent(QPaintEvent *e);

	private:
		QComboBox *font;
		QComboBox *style;
		QComboBox *size;
		QCheckBox *bold;
		QCheckBox *underline;
		QCheckBox *italic;
		QPushButton *ok_button;
		QPushButton *cancel_button;
		QGroupBox *sample;

		void init(QFont initial_font);
};

#endif	// _FONTDIALOG_H

