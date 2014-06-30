
#ifndef ACTIVEXCONTROLHOST_H
#define ACTIVEXCONTROLHOST_H

#include "CustomControl.h"

namespace VCF
{

/**
 * This is a control that can host other ActiveX controls. This will have to implement whatever 
 *functionality is neccesary on an ActiveX site container. 
 *
 *@version 1.0
 *@author Jim Crafton
 */
class ActiveXControlHost :  public VCF::CustomControl{
public:
	virtual ~ActiveXControlHost(){};
};

};
#endif //ACTIVEXCONTROLHOST_H