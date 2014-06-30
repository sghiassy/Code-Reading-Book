//Model.h
/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/
#include "ApplicationKit.h"

#include <algorithm>

using namespace VCF;

Model::~Model()
{
	
}

void Model::addView( View* view )
{
	m_views.push_back( view );
}

void Model::removeView( View* view )
{
	std::vector<View*>::iterator found = std::find( m_views.begin(), m_views.end(), view );
	if ( found != m_views.end() ) {
		m_views.erase( found );
	}
}

void Model::updateAllViews()
{
	std::vector<View*>::iterator it = m_views.begin();
	while ( it != m_views.end() ) {
		View* view = *it;
		view->updateView( this );
		it++;
	}
}