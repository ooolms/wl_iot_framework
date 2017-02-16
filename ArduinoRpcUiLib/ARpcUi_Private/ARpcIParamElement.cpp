#include "ARpcIParamElement.h"
#include "ARpcParamCheckbox.h"
#include "ARpcParamDial.h"
#include "ARpcParamSlider.h"
#include "ARpcParamTextEdit.h"
#include "ARpcParamSelect.h"
#include "ARpcParamNull.h"

ARpcIParamElement::ARpcIParamElement(QObject *parent)
	:QObject(parent)
{
}

ARpcIParamElement* ARpcIParamElement::makeWidget(const ARpcControlParam &param)
{
	if(param.type==ARpcControlParam::CHECKBOX)return new ARpcParamCheckbox(param);
	else if(param.type==ARpcControlParam::DIAL)return new ARpcParamDial(param);
	else if(param.type==ARpcControlParam::SLIDER)return new ARpcParamSlider(param);
	else if(param.type==ARpcControlParam::TEXT_EDIT)return new ARpcParamTextEdit(param);
	else if(param.type==ARpcControlParam::SELECT)return new ARpcParamSelect(param);
	else return new ARpcParamNull;
}
