//GridRulerBtn.h
#include "ApplicationKit.h"
#include "GridRulerBtn.h"

using namespace VCF;

using namespace VCFBuilder;


GridRulerBtn::GridRulerBtn():
	CustomControl( false )
{

}

GridRulerBtn::~GridRulerBtn()
{

}

void GridRulerBtn::paint( GraphicsContext* ctx )
{
	CustomControl::paint( ctx );
}