//ImageControl.h
/**
Copyright (c) 2000-2001, Jim Crafton
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in 
	the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

NB: This software will not save the world. 
*/
#include "ApplicationKit.h"
#include "Panel.h"
#include "DefaultPropertyEditors.h"
#include "ImageControl.h"


using namespace VCF;
ImageControl::ImageControl():
	CustomControl( false )
{
	m_image = NULL;
	m_transparent = false;
	m_filename = "";
}

ImageControl::~ImageControl()
{

}

Image* ImageControl::getImage()
{
	return m_image;
}

void ImageControl::setImage( Image* image )
{
	m_image = image;
}

bool ImageControl::getTransparent()
{
	return m_transparent;
}

void ImageControl::setTransparent( const bool& transparent )
{
	m_transparent = transparent;
}

ImageFilenameString& ImageControl::getFilename()
{
	return m_filename;
}

void ImageControl::setFilename( const ImageFilenameString& filename )
{
	m_filename = filename;
	if ( NULL != m_image ) {
		delete m_image;
		m_image = NULL;
	}

	m_image = GraphicsToolkit::getDefaultGraphicsToolkit()->createImage( m_filename );
}

void ImageControl::paint( GraphicsContext* context )
{
	CustomControl::paint( context );
	if ( NULL != m_image ) {	
		double w = getWidth();
		double h = getHeight();
		if ( (w < m_image->getWidth()) && (h < m_image->getHeight()) ) {
			context->drawPartialImage( 0, 0, &Rect(0, 0, w, h ), m_image );
		}
		else {
			context->drawImage( 0, 0, m_image );
		}		
	}
}




ImageFilenamePropertyEditor::ImageFilenamePropertyEditor()
{

}

ImageFilenamePropertyEditor::~ImageFilenamePropertyEditor()
{

}

Control* ImageFilenamePropertyEditor::getCustomEditor()
{
	return new ModalPropertyEditorControl<ImageFilenamePropertyEditor>( 
			ImageFilenamePropertyEditor::showFilenameEditor, this->getValue(), this );
}

void ImageFilenamePropertyEditor::showFilenameEditor( VariantData* data )
{
	CommonFileOpen dlg;
	
	ImageFilenameString& obj = *((ImageFilenameString*)(Object*)*data);

	String s = obj;
	dlg.setFileName( s );
	if ( true == dlg.execute() ) {
		s = dlg.getFileName();
		obj = s;
	}
}




ImagePropertyEditor::ImagePropertyEditor()
{

}

ImagePropertyEditor::~ImagePropertyEditor()
{

}

Control* ImagePropertyEditor::getCustomEditor()
{
	return new ModalPropertyEditorControl<ImagePropertyEditor>( 
			ImagePropertyEditor::showImageEditor, this->getValue(), this );
}

void ImagePropertyEditor::showImageEditor( VariantData* data )
{
	Image* image = (Image*)(Object*)(*data);
	
	Dialog dlg;
	dlg.setBounds( &Rect( 0, 0, 410, 520 ) );
	CommandButton* okBtn = new CommandButton();
	okBtn->setBounds( &Rect(240, 460, 320, 485) );
	okBtn->setCaption( "OK" );
	dlg.add( okBtn );

	CommandButton* cancelBtn = new CommandButton();
	cancelBtn->setBounds( &Rect(330, 460, 400, 485) );
	cancelBtn->setCaption( "Cancel" );
	dlg.add( cancelBtn );
	dlg.showModal();
	
}