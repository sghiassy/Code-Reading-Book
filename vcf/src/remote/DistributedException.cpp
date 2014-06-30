//DistributedException.h
/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/
#include "RemoteObjectKit.h"
#include "DistributedException.h"

using namespace VCF;

using namespace VCFRemote;


DistributedException::DistributedException():
	BasicException("")
{

}

DistributedException::~DistributedException()
{

}

