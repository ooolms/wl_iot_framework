/*******************************************
Copyright 2017 OOO "LMS"

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

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
