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

#include "IParamElement.h"
#include "ParamCheckbox.h"
#include "ParamDial.h"
#include "ParamSlider.h"
#include "ParamTextEdit.h"
#include "ParamSelect.h"
#include "ParamNull.h"

IParamElement::IParamElement(QObject *parent)
	:QObject(parent)
{
}

IParamElement* IParamElement::makeWidget(const ControlParam &param)
{
	if(param.type==ControlParam::CHECKBOX)return new ParamCheckbox(param);
	else if(param.type==ControlParam::DIAL)return new ParamDial(param);
	else if(param.type==ControlParam::SLIDER)return new ParamSlider(param);
	else if(param.type==ControlParam::TEXT_EDIT)return new ParamTextEdit(param);
	else if(param.type==ControlParam::SELECT)return new ParamSelect(param);
	else return new ParamNull;
}
