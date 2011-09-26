// ============================================================================
//
// This file are where the Conditions/Actions/Expressions are defined.
// You can manually enter these, or use CICK (recommended)
// See the Extension FAQ in this SDK for more info and where to download it
//
// ============================================================================

// Common Include
#include	"common.h"
#include	"ObjectSelection.h"

// Quick memo: content of the eventInformations arrays
// ---------------------------------------------------
// Menu ID
// String ID
// Code
// Flags
// Number_of_parameters
// Parameter_type [Number_of_parameters]
// Parameter_TitleString [Number_of_parameters]

// Definitions of parameters for each condition
short conditionsInfos[]=
	{
		ID_ANYOBJECTSTOPPED,		IDS_ANYOBJECTSTOPPED,		CND_ANYOBJECTSTOPPED,		0,								0,
		ID_SPECIFICOBJECTSTOPPED,	IDS_SPECIFICOBJECTSTOPPED,	CND_SPECIFICOBJECTSTOPPED,	0,								1, PARAM_OBJECT, IDS_SELECTOBJECTTOTEST,
		ID_ISOBJECTMOVING,			IDS_ISOBJECTMOVING,			CND_ISOBJECTMOVING,			EVFLAGS_ALWAYS+EVFLAGS_NOTABLE, 1, PARAM_OBJECT, IDS_SELECTOBJECTTOTEST,
	};

// Definitions of parameters for each action
short actionsInfos[]=
	{
		ID_MOVEOBJ,				IDS_MOVEOBJ,				ACT_MOVEOBJ,				0, 6, PARAM_OBJECT, PARAM_EASING, PARAM_EXPRESSION, PARAM_EXPRESSION, PARAM_TIMEMODE, PARAM_EXPRESSION,		IDS_SELECTOBJECTTOMOVE, 0, IDS_XCOORDINATE, IDS_YCOORDINATE, 0, IDS_TIME, 
		ID_STOPOBJECT,			IDS_STOPOBJECT,				ACT_STOPOBJECT,				0, 1, PARAM_OBJECT,	IDS_SELECTOBJECTTOSTOP,
		ID_STOPALLOBJECTS,		IDS_STOPALLOBJECTS,			ACT_STOPALLOBJECTS,			0, 0,
		ID_REVERSEOBJECT,		IDS_REVERSEOBJECT,			ACT_REVERSEOBJECT,			0, 1, PARAM_OBJECT, IDS_SELECTOBJECTTOMOVE,
		ID_SETAMPLITUDE,		IDS_SETAMPLITUDE,			ACT_SETAMPLITUDE,			0, 1, PARAM_EXPRESSION,	IDS_SETAMPLITUDE_VAL,
		ID_SETOVERSHOOT,		IDS_SETOVERSHOOT,			ACT_SETOVERSHOOT,			0, 1, PARAM_EXPRESSION,	IDS_SETOVERSHOOT_VAL,
		ID_SETPERIOD,			IDS_SETPERIOD,				ACT_SETPERIOD,				0, 1, PARAM_EXPRESSION,	IDS_SETPERIOD_VAL,

		ID_SETOBJECTAMPLITUDE,	IDS_SETOBJECTAMPLITUDE,		ACT_SETOBJECTAMPLITUDE,		0, 2, PARAM_OBJECT,PARAM_EXPRESSION,	IDS_SELECTOBJECT, IDS_SETAMPLITUDE_VAL,
		ID_SETOBJECTOVERSHOOT,	IDS_SETOBJECTOVERSHOOT,		ACT_SETOBJECTOVERSHOOT,		0, 2, PARAM_OBJECT,PARAM_EXPRESSION,	IDS_SELECTOBJECT, IDS_SETOVERSHOOT_VAL,
		ID_SETOBJECTPERIOD,		IDS_SETOBJECTPERIOD,		ACT_SETOBJECTPERIOD,		0, 2, PARAM_OBJECT,PARAM_EXPRESSION,	IDS_SELECTOBJECT, IDS_SETPERIOD_VAL,
	};

// Definitions of parameters for each expression
short expressionsInfos[]=
	{
		ID_GETNUMCONTROLLED,			IDS_GETNUMCONTROLLED,			EXP_GETNUMCONTROLLED,			0, 0,	
		ID_GETSTOPPEDFIXED,				IDS_GETSTOPPEDFIXED,			EXP_GETSTOPPEDFIXED,			0, 0,	

		ID_CALCULATE_EASEIN,			IDS_CALCULATE_EASEIN,			EXP_CALCULATE_EASEIN,			0, 2,	EXPPARAM_LONG, EXPPARAM_LONG,					IDS_FUNCTION, IDS_STEP,
		ID_CALCULATE_EASEOUT,			IDS_CALCULATE_EASEOUT,			EXP_CALCULATE_EASEOUT,			0, 2,	EXPPARAM_LONG, EXPPARAM_LONG,					IDS_FUNCTION, IDS_STEP,
		ID_CALCULATE_EASEINOUT,			IDS_CALCULATE_EASEINOUT,		EXP_CALCULATE_EASEINOUT,		0, 3,	EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG,	IDS_FUNCTIONA, IDS_FUNCTIONB, IDS_STEP,
		ID_CALCULATE_EASEOUTIN,			IDS_CALCULATE_EASEOUTIN,		EXP_CALCULATE_EASEOUTIN,		0, 3,	EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG,	IDS_FUNCTIONA, IDS_FUNCTIONB, IDS_STEP,
		
		ID_CALCULATEBETWEEN_EASEIN,		IDS_CALCULATEBETWEEN_EASEIN,	EXP_CALCULATEBETWEEN_EASEIN,	0, 4,	EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG,						IDS_VALUEA, IDS_VALUEB, IDS_FUNCTION, IDS_STEP,
		ID_CALCULATEBETWEEN_EASEOUT,	IDS_CALCULATEBETWEEN_EASEOUT,	EXP_CALCULATEBETWEEN_EASEOUT,	0, 4,	EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG,						IDS_VALUEA, IDS_VALUEB, IDS_FUNCTION, IDS_STEP,
		ID_CALCULATEBETWEEN_EASEINOUT,	IDS_CALCULATEBETWEEN_EASEINOUT,	EXP_CALCULATEBETWEEN_EASEINOUT,	0, 5,	EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG,		IDS_VALUEA, IDS_VALUEB, IDS_FUNCTIONA, IDS_FUNCTIONB, IDS_STEP,
		ID_CALCULATEBETWEEN_EASEOUTIN,	IDS_CALCULATEBETWEEN_EASEOUTIN,	EXP_CALCULATEBETWEEN_EASEOUTIN,	0, 5,	EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG, EXPPARAM_LONG,		IDS_VALUEA, IDS_VALUEB, IDS_FUNCTIONA, IDS_FUNCTIONB, IDS_STEP,

		ID_GETAMPLITUDE,				IDS_GETAMPLITUDE,				EXP_GETAMPLITUDE,				0, 1,	EXPPARAM_LONG, IDS_FIXED_VAL,
		ID_GETOVERSHOOT,				IDS_GETOVERSHOOT,				EXP_GETOVERSHOOT,				0, 1,	EXPPARAM_LONG, IDS_FIXED_VAL,
		ID_GETPERIOD,					IDS_GETPERIOD,					EXP_GETPERIOD,					0, 1,	EXPPARAM_LONG, IDS_FIXED_VAL,

		ID_GETDEFAULTAMPLITUDE,			IDS_GETDEFAULTAMPLITUDE,		EXP_GETDEFAULTAMPLITUDE,		0, 0,
		ID_GETDEFAULTOVERSHOOT,			IDS_GETDEFAULTOVERSHOOT,		EXP_GETDEFAULTOVERSHOOT,		0, 0,
		ID_GETDEFAULTPERIOD,			IDS_GETDEFAULTPERIOD,			EXP_GETDEFAULTPERIOD,			0, 0,
	};

#define EXP_GETAMPLITUDE                10
#define EXP_GETOVERSHOOT                11
#define EXP_GETPERIOD                   12


_inline float GetFloatParamEx(LPRDATA rdPtr)
{
	long val = CNC_GetFloatParameter(rdPtr);
	return *(float*)&val;
}

_inline float RecastFloat(long value)
{
	return *(float*)&value;
}

_inline long RecastFloatToLong(float value)
{
	return *(long*)&value;
}



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

long WINAPI DLLExport con_AnyObjectStopped(LPRDATA rdPtr, long param1, long param2)
{
	return true;
}			
			
long WINAPI DLLExport con_SpecificObjectStopped(LPRDATA rdPtr, long param1, long param2)
{
	short oi = ((eventParam*)param1)->evp.evpW.evpW0;
	LPRO object = LproFromFixed(rdPtr, rdPtr->currentMoved.fixedValue);
	ObjectSelection select = ObjectSelection(rdPtr->rHo.hoAdRunHeader, rdPtr->isHwa, rdPtr->isUnicode);

	if(object != NULL && select.ObjectIsOfType(object, oi))
	{
		select.SelectOneObject(object);
		return true;
	}
	return false;
}

//Should it select the given object?
bool filterMoving(LPRDATA rdPtr, LPRO object)
{
	for(unsigned int i=0; i<rdPtr->controlled.size(); ++i)
	{
		MoveStruct & moved = rdPtr->controlled.at(i);
		if(moved.fixedValue == FixedVal(object))
			return true;
	}
	return false;
}

long WINAPI DLLExport con_IsObjectMoving(LPRDATA rdPtr, long param1, long param2)
{
	ObjectSelection select = ObjectSelection(rdPtr->rHo.hoAdRunHeader, rdPtr->isHwa, rdPtr->isUnicode);

	PEVT pe = (PEVT)(((LPBYTE)param1)-CND_SIZE);
	bool isNegated = (pe->evtFlags2 & EVFLAG2_NOT);
	short oi = ((eventParam*)param1)->evp.evpW.evpW0;

	return select.FilterObjects(rdPtr, oi, isNegated, &filterMoving);
}
		

// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

short WINAPI DLLExport act_MoveObj(LPRDATA rdPtr, long param1, long param2)
{
	LPRO object =				(LPRO)CNC_GetParameter(rdPtr);

	paramExt* easeParam =		(paramExt*)CNC_GetParameter(rdPtr);
	EasingParam* easing =		(EasingParam*)easeParam->pextData;

	int x =						(int)CNC_GetParameter(rdPtr);
	int y =						(int)CNC_GetParameter(rdPtr);

	paramExt* timeParam =		(paramExt*)CNC_GetParameter(rdPtr);
	TimeModeParam* time =		(TimeModeParam*)timeParam->pextData;
	
	int timespan =				(int)CNC_GetParameter(rdPtr);

	if(object == NULL)
		return 0;

	//Remove object if it exists
	int fixed = FixedVal(object);
	for(unsigned int i = 0; i < rdPtr->controlled.size(); i++)
	{
		MoveStruct moved = rdPtr->controlled[i];
		if(moved.fixedValue == fixed)
		{
			rdPtr->controlled.erase( rdPtr->controlled.begin() + i );
			break;
		}
	}
	
	MoveStruct move;
	move.startX = object->roHo.hoX;
	move.startY = object->roHo.hoY;
	move.fixedValue = FixedVal(object);
	move.destX = x;
	move.destY = y;
	move.starttime = 0;

	move.easingMode = easing->method;
	move.functionA = easing->firstFunction;
	move.functionB = easing->secondFunction;

	move.timeMode = time->type;
	move.timespan = timespan;
	move.eventloop_step = 0;
	
	if(move.timeMode == 0)
	{
		move.starttime = CurrentTime();
	}

	move.vars = rdPtr->easingVars;

	rdPtr->controlled.push_back(move);

	return 0;
}


short WINAPI DLLExport act_StopObject(LPRDATA rdPtr, long param1, long param2)
{
	LPRO object = (LPRO)param1;

	int fixed = FixedVal(object);

	for(unsigned int i = 0; i < rdPtr->controlled.size(); i++)
	{
		MoveStruct moved = rdPtr->controlled[i];
		if(moved.fixedValue == fixed)
		{
			rdPtr->controlled.erase( rdPtr->controlled.begin() + i );
			return 0;
		}
	}


	return 0;
}

short WINAPI DLLExport act_StopAllObjects(LPRDATA rdPtr, long param1, long param2)
{
	rdPtr->controlled.clear();
	return 0;
}

short WINAPI DLLExport act_ReverseObject(LPRDATA rdPtr, long param1, long param2)
{
	LPRO object = (LPRO)param1;
	int fixed = FixedVal(object);
	
	MoveStruct reversed;
	memset(&reversed,0,sizeof(MoveStruct));

	//Otherwise remove the object and reinsert it with new coordinates.
	for(unsigned int i = 0; i < rdPtr->controlled.size(); i++)
	{
		MoveStruct moved = rdPtr->controlled.at(i);
		if(moved.fixedValue == fixed)
		{
			reversed = moved;
			rdPtr->controlled.erase( rdPtr->controlled.begin() + i );
			break;
		}
	}
	
	//If it was the object that was just stopped then use that one.
	if(reversed.fixedValue == 0)
	{
		if(rdPtr->currentMoved.fixedValue == fixed)
			reversed = rdPtr->currentMoved;
		else	//If no object found, abort
			return 0;
	}

	reversed.destX = reversed.startX;
	reversed.destY = reversed.startY;
	
	reversed.startX = object->roHo.hoX;
	reversed.startY = object->roHo.hoY;
	
	//Recalculate the time it should take moving to the previous position
	if(reversed.timeMode == 0)
	{
		__int64 currentTime = CurrentTime();
		__int64 timeSoFar = currentTime - reversed.starttime;

		reversed.timespan = (int)timeSoFar;
		reversed.starttime = currentTime;
	}
	else
	{
		reversed.timespan = reversed.eventloop_step;
		reversed.eventloop_step = 0;
	}

	rdPtr->controlled.push_back(reversed);

	return 0;
}

//Default parameters
short WINAPI DLLExport act_SetAmplitude(LPRDATA rdPtr, long param1, long param2)
{
	long tmpf=CNC_GetFloatValue(rdPtr, 0 );
	float value = *(float *)&tmpf;
	rdPtr->easingVars.amplitude = value;
	return 0;
}

short WINAPI DLLExport act_SetOvershoot(LPRDATA rdPtr, long param1, long param2)
{
	long tmpf=CNC_GetFloatValue(rdPtr, 0 );
	float value = *(float *)&tmpf;
	rdPtr->easingVars.overshoot = value;
	return 0;
}

short WINAPI DLLExport act_SetPeriod(LPRDATA rdPtr, long param1, long param2)
{

	long tmpf=CNC_GetFloatValue(rdPtr, 0 );
	float value = *(float *)&tmpf;
	rdPtr->easingVars.period = value;
	return 0;
}

//Object parameters
short WINAPI DLLExport act_SetObjectAmplitude(LPRDATA rdPtr, long param1, long param2)
{
	LPRO object = (LPRO)param1;
	int fixed = FixedVal(object);
	float value = RecastFloat(CNC_GetFloatValue(rdPtr, 1));

	for(unsigned int i=0; i<rdPtr->controlled.size(); ++i)
	{
		MoveStruct & moved = rdPtr->controlled.at(i);
		if(moved.fixedValue == fixed)
		{
			moved.vars.amplitude = value;
			return 0;
		}
	}
	return 0;
}

short WINAPI DLLExport act_SetObjectOvershoot(LPRDATA rdPtr, long param1, long param2)
{
	LPRO object = (LPRO)param1;
	int fixed = FixedVal(object);
	float value = RecastFloat(CNC_GetFloatValue(rdPtr, 1));

	for(unsigned int i=0; i<rdPtr->controlled.size(); ++i)
	{
		MoveStruct & moved = rdPtr->controlled.at(i);
		if(moved.fixedValue == fixed)
		{
			moved.vars.overshoot = value;
			return 0;
		}
	}
	return 0;
}

short WINAPI DLLExport act_SetObjectPeriod(LPRDATA rdPtr, long param1, long param2)
{
	LPRO object = (LPRO)param1;
	int fixed = FixedVal(object);
	float value = RecastFloat(CNC_GetFloatValue(rdPtr, 1));

	for(unsigned int i=0; i<rdPtr->controlled.size(); ++i)
	{
		MoveStruct & moved = rdPtr->controlled.at(i);
		if(moved.fixedValue == fixed)
		{
			moved.vars.period = value;
			return 0;
		}
	}
	return 0;
}


// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

long WINAPI DLLExport exp_GetNumControlled(LPRDATA rdPtr,long param1)
{
	return rdPtr->controlled.size();
}

long WINAPI DLLExport exp_GetStoppedFixed(LPRDATA rdPtr,long param1)
{
	return rdPtr->currentMoved.fixedValue;
}

long WINAPI DLLExport exp_EaseIn(LPRDATA rdPtr,long param1)
{
	int function = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	float step = RecastFloat(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT));

	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return RecastFloatToLong((float)calculateEasingValue(EASEIN, function, 0, step, rdPtr->easingVars));
}

long WINAPI DLLExport exp_EaseOut(LPRDATA rdPtr,long param1)
{
	int function = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	float step = RecastFloat(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT));

	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return RecastFloatToLong((float)calculateEasingValue(EASEOUT, function, 0, (float)step, rdPtr->easingVars));
}

long WINAPI DLLExport exp_EaseInOut(LPRDATA rdPtr,long param1)
{
	int functionA = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	int functionB = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	float step = RecastFloat(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT));

	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return RecastFloatToLong((float)calculateEasingValue(EASEINOUT, functionA, functionB, step, rdPtr->easingVars));
}

long WINAPI DLLExport exp_EaseOutIn(LPRDATA rdPtr,long param1)
{
	int functionA = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	int functionB = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	float step = RecastFloat(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT));

	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return RecastFloatToLong((float)calculateEasingValue(EASEOUTIN, functionA, functionB, step, rdPtr->easingVars));
}

//Between values

long WINAPI DLLExport exp_EaseInBetween(LPRDATA rdPtr,long param1)
{
	float valueA = RecastFloat(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT));
	float valueB = RecastFloat(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT));
	int function = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	float step = RecastFloat(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT));
	float ease = (float)calculateEasingValue(EASEIN, function, 0, step, rdPtr->easingVars);

	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return RecastFloatToLong(valueA + (valueB-valueA)*ease);
}

long WINAPI DLLExport exp_EaseOutBetween(LPRDATA rdPtr,long param1)
{
	float valueA = RecastFloat(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT));
	float valueB = RecastFloat(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT));
	int function = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	float step = RecastFloat(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT));
	float ease = (float)calculateEasingValue(EASEOUT, function, 0, step, rdPtr->easingVars);

	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return RecastFloatToLong(valueA + (valueB-valueA)*ease);
}

long WINAPI DLLExport exp_EaseInOutBetween(LPRDATA rdPtr,long param1)
{
	float valueA = RecastFloat(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT));
	float valueB = RecastFloat(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT));
	int functionA = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	int functionB = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	float step = RecastFloat(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT));
	float ease = (float)calculateEasingValue(EASEINOUT, functionA, functionB, step, rdPtr->easingVars);
	
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return RecastFloatToLong(valueA + (valueB-valueA)*ease);
}

long WINAPI DLLExport exp_EaseOutInBetween(LPRDATA rdPtr,long param1)
{
	float valueA = RecastFloat(CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_FLOAT));
	float valueB = RecastFloat(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT));
	int functionA = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	int functionB = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	float step = RecastFloat(CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_FLOAT));
	float ease = (float)calculateEasingValue(EASEOUTIN, functionA, functionB, step, rdPtr->easingVars);

	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return RecastFloatToLong(valueA + (valueB-valueA)*ease);
}

//Object parameters
long WINAPI DLLExport exp_GetAmplitude(LPRDATA rdPtr,long param1)
{
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	int fixed = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	for(unsigned int i=0; i<rdPtr->controlled.size(); ++i)
	{
		MoveStruct moved = rdPtr->controlled.at(i);
		if(moved.fixedValue == fixed)
			return RecastFloatToLong(moved.vars.amplitude);
	}
	
	return RecastFloatToLong(0);
}

long WINAPI DLLExport exp_GetOvershoot(LPRDATA rdPtr,long param1)
{
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	int fixed = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	for(unsigned int i=0; i<rdPtr->controlled.size(); ++i)
	{
		MoveStruct moved = rdPtr->controlled.at(i);
		if(moved.fixedValue == fixed)
			return RecastFloatToLong(moved.vars.overshoot);
	}
	
	return RecastFloatToLong(0);
}

long WINAPI DLLExport exp_GetPeriod(LPRDATA rdPtr,long param1)
{
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	int fixed = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	for(unsigned int i=0; i<rdPtr->controlled.size(); ++i)
	{
		MoveStruct moved = rdPtr->controlled.at(i);
		if(moved.fixedValue == fixed)
			return RecastFloatToLong(moved.vars.period);
	}
	
	return RecastFloatToLong(0);
}

//Default parameters
long WINAPI DLLExport exp_GetDefaultAmplitude(LPRDATA rdPtr,long param1)
{
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return RecastFloatToLong(rdPtr->easingVars.amplitude);
}

long WINAPI DLLExport exp_GetDefaultOvershoot(LPRDATA rdPtr,long param1)
{
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return RecastFloatToLong(rdPtr->easingVars.overshoot);
}

long WINAPI DLLExport exp_GetDefaultPeriod(LPRDATA rdPtr,long param1)
{
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return RecastFloatToLong(rdPtr->easingVars.period);
}



// ----------------------------------------------------------
// Condition / Action / Expression jump table
// ----------------------------------------------------------
// Contains the address inside the extension of the different
// routines that handle the action, conditions and expressions.
// Located at the end of the source for convinience
// Must finish with a 0
//
long (WINAPI * ConditionJumps[])(LPRDATA rdPtr, long param1, long param2) = 
			{ 
			con_AnyObjectStopped,
			con_SpecificObjectStopped,
			con_IsObjectMoving,
			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			act_MoveObj,
			act_StopObject,
			act_StopAllObjects,
			act_ReverseObject,
			act_SetAmplitude,
			act_SetOvershoot,
			act_SetPeriod,
			act_SetObjectAmplitude,
			act_SetObjectOvershoot,
			act_SetObjectPeriod,
			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			exp_GetNumControlled,
			exp_GetStoppedFixed,
			exp_EaseIn,
			exp_EaseOut,
			exp_EaseInOut,
			exp_EaseOutIn,
			exp_EaseInBetween,
			exp_EaseOutBetween,
			exp_EaseInOutBetween,
			exp_EaseOutInBetween,
			exp_GetAmplitude,
			exp_GetOvershoot,
			exp_GetPeriod,
			exp_GetDefaultAmplitude,
			exp_GetDefaultOvershoot,
			exp_GetDefaultPeriod,
			0
			};
