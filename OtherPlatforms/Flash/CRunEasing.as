package Extensions
{
    import Actions.*;
	import Banks.CFont;
    import Conditions.*;
	import Events.CEventProgram;
    import Expressions.*;
	import flash.utils.ByteArray;
    import Objects.CObject;
	import Params.PARAM_OBJECT;
    import RunLoop.*;
    import Services.*;
    import Sprites.*;
	import flash.utils.Dictionary;

    public class CRunEasing extends CRunExtension
    {
        private static var CON_ANYOBJECTSTOPPED:int = 0;
        private static var CON_SPECIFICOBJECTSTOPPED:int = 1;
        private static var CON_ISOBJECTMOVING:int = 2;
        private static var CND_LAST:int = 3;

        private static var ACT_MOVEOBJ:int = 0;
        private static var ACT_STOPOBJECT:int = 1;
        private static var ACT_STOPALLOBJECTS:int = 2;
        private static var ACT_REVERSEOBJECT:int = 3;
        private static var ACT_SETAMPLITUDE:int = 4;
        private static var ACT_SETOVERSHOOT:int = 5;
        private static var ACT_SETPERIOD:int = 6;
        private static var ACT_SETOBJECTAMPLITUDE:int = 7;
        private static var ACT_SETOBJECTOVERSHOOT:int = 8;
        private static var ACT_SETOBJECTPERIOD:int = 9;
		private static var ACT_MOVEOBJEXPLICIT:int = 10;

        private static var EXP_GETNUMCONTROLLED:int = 0;
        private static var EXP_GETSTOPPEDFIXED:int = 1;
        private static var EXP_EASEIN:int = 2;
        private static var EXP_EASEOUT:int = 3;
        private static var EXP_EASEINOUT:int = 4;
        private static var EXP_EASEOUTIN:int = 5;
        private static var EXP_EASEINBETWEEN:int = 6;
        private static var EXP_EASEOUTBETWEEN:int = 7;
        private static var EXP_EASEINOUTBETWEEN:int = 8;
        private static var EXP_EASEOUTINBETWEEN:int = 9;
        private static var EXP_GETAMPLITUDE:int = 10;
        private static var EXP_GETOVERSHOOT:int = 11;
        private static var EXP_GETPERIOD:int = 12;
        private static var EXP_GETDEFAULTAMPLITUDE:int = 13;
        private static var EXP_GETDEFAULTOVERSHOOT:int = 14;
        private static var EXP_GETDEFAULTPERIOD:int = 15;
		
		private static var EASEIN:int = 0;
		private static var EASEOUT:int = 1;
		private static var EASEINOUT:int = 2;
		private static var EASEOUTIN:int = 3;

		private var controlled:Dictionary;
		private var deleted:Dictionary;
		private var easeVars:CRunEasingVars;
		private var controlledCount:int;
		
		private var currentMovedObj:CObject;
		private var currentMoved:CRunEasingVars;
		
        public function CRunEasing()
        {
        }
        public override function getNumberOfConditions():int
        {
            return CND_LAST;
        }
        public override function createRunObject(file:CBinaryFile, cob:CCreateObjectInfo, version:int):Boolean
        {
			controlled = new Dictionary();
			deleted = new Dictionary();
			easeVars = new CRunEasingVars();

			easeVars.overshoot = readFloat(file.data);
			easeVars.amplitude = readFloat(file.data);
			easeVars.period = readFloat(file.data);
			
            return false;
        }
		private function readFloat(array:ByteArray):Number
		{
			var res:ByteArray = new ByteArray();
			res[3] = array.readByte();
			res[2] = array.readByte();
			res[1] = array.readByte();
			res[0] = array.readByte();
			return res.readFloat();
		}
		
        public override function destroyRunObject(bFast:Boolean):void
        {
        }
        public override function handleRunObject():int
        {
			var finnishedMoving:Boolean = false;
			var step:Number;

			for (var key:Object in controlled)
			{
				var object:CObject = CObject(key);
				var moved:CRunEasingVars = CRunEasingVars(controlled[key]);

				if( object != null )
				{
					if(moved.timeMode == 0)
					{
						var currentTime:Date = new Date();
						var diff:Number = Number(currentTime.getTime() - moved.startdate.getTime());

						step = diff / Number(moved.timespan);

						if(diff >= moved.timespan){
							finnishedMoving = true;
						}
					}
					else
					{
						moved.eventloop_step++;
						step = moved.eventloop_step / Number(moved.timespan);

						if(moved.eventloop_step >= moved.timespan){
							finnishedMoving = true;
						}
					}

					var easeStep:Number = Number(calculateEasingValue(moved.easingMode, moved.functionA, moved.functionB, step, moved));
					
					object.hoX = int((moved.startX + (moved.destX-moved.startX)*easeStep + 0.5));
					object.hoY = int((moved.startY + (moved.destY-moved.startY)*easeStep + 0.5));
					object.roc.rcChanged = true;

					if(finnishedMoving)
					{
						finnishedMoving = false;

						object.hoX = moved.destX;
						object.hoY = moved.destY;
						
						deleted[object] = moved;
						delete controlled[object];
						controlledCount--;
					}
				}
				else
				{
					delete controlled[object];
					controlledCount--;
				}
			}

			//Trigger the 'Object stopped moving' events
			for (var dkey:Object in deleted)
			{
				currentMovedObj = CObject(dkey);
				currentMoved = CRunEasingVars(controlled[dkey]);
				ho.generateEvent(CON_ANYOBJECTSTOPPED, 0);
				ho.generateEvent(CON_SPECIFICOBJECTSTOPPED, 0);
				delete deleted[dkey];
			}
            return 0;
        }


        // Conditions
        // -------------------------------------------------
        public override function condition(num:int, cnd:CCndExtension):Boolean
        {
            switch (num)
            {
            case CON_ANYOBJECTSTOPPED:
                return con_AnyObjectStopped();
            case CON_SPECIFICOBJECTSTOPPED:
                return con_SpecificObjectStopped(cnd.getParamObject(rh,0));
            case CON_ISOBJECTMOVING:
                return con_IsObjectMoving(cnd.getParamObject(rh,0), cnd.negaFALSE());
            }
            return false;
        }

        // Actions
        // -------------------------------------------------
        public override function action(num:int, act:CActExtension):void
        {
            switch (num)
            {
            case ACT_MOVEOBJ:
                act_MoveObj(act.getParamObject(rh,0), act.getParamExtension(rh,1), act.getParamExpression(rh,2), act.getParamExpression(rh,3), act.getParamExtension(rh,4), act.getParamExpression(rh,5));
                break;
            case ACT_STOPOBJECT:
                act_StopObject(act.getParamObject(rh,0));
                break;
            case ACT_STOPALLOBJECTS:
                act_StopAllObjects();
                break;
            case ACT_REVERSEOBJECT:
                act_ReverseObject(act.getParamObject(rh,0));
                break;
            case ACT_SETAMPLITUDE:
                act_SetAmplitude(act.getParamExpDouble(rh,0));
                break;
            case ACT_SETOVERSHOOT:
                act_SetOvershoot(act.getParamExpDouble(rh,0));
                break;
            case ACT_SETPERIOD:
                act_SetPeriod(act.getParamExpDouble(rh,0));
                break;
            case ACT_SETOBJECTAMPLITUDE:
                act_SetObjectAmplitude(act.getParamObject(rh,0), act.getParamExpDouble(rh,1));
                break;
            case ACT_SETOBJECTOVERSHOOT:
                act_SetObjectOvershoot(act.getParamObject(rh,0), act.getParamExpDouble(rh,1));
                break;
            case ACT_SETOBJECTPERIOD:
                act_SetObjectPeriod(act.getParamObject(rh,0), act.getParamExpDouble(rh,1));
                break;
			case ACT_MOVEOBJEXPLICIT:
				act_MoveObjExplicit(act.getParamExpression(rh, 0), act.getParamExpression(rh, 1), act.getParamExpression(rh, 2), act.getParamExpression(rh, 3), act.getParamExpression(rh, 4), act.getParamExpression(rh, 5), act.getParamExpression(rh, 6), act.getParamExpression(rh, 7));
				break;
            }
        }

        // Expressions
        // -------------------------------------------------
        public override function expression(num:int):CValue
        {
            switch (num)
            {
            case EXP_GETNUMCONTROLLED:
                return exp_GetNumControlled();
            case EXP_GETSTOPPEDFIXED:
                return exp_GetStoppedFixed();
            case EXP_EASEIN:
                return exp_EaseIn(ho.getExpParam().getInt(), ho.getExpParam().getDouble());
            case EXP_EASEOUT:
                return exp_EaseOut(ho.getExpParam().getInt(), ho.getExpParam().getDouble());
            case EXP_EASEINOUT:
                return exp_EaseInOut(ho.getExpParam().getInt(), ho.getExpParam().getInt(), ho.getExpParam().getDouble());
            case EXP_EASEOUTIN:
                return exp_EaseOutIn(ho.getExpParam().getInt(), ho.getExpParam().getInt(), ho.getExpParam().getDouble());
            case EXP_EASEINBETWEEN:
                return exp_EaseInBetween(ho.getExpParam().getDouble(), ho.getExpParam().getDouble(), ho.getExpParam().getInt(), ho.getExpParam().getDouble());
            case EXP_EASEOUTBETWEEN:
                return exp_EaseOutBetween(ho.getExpParam().getDouble(), ho.getExpParam().getDouble(), ho.getExpParam().getInt(), ho.getExpParam().getDouble());
            case EXP_EASEINOUTBETWEEN:
                return exp_EaseInOutBetween(ho.getExpParam().getDouble(), ho.getExpParam().getDouble(), ho.getExpParam().getInt(), ho.getExpParam().getInt(), ho.getExpParam().getDouble());
            case EXP_EASEOUTINBETWEEN:
                return exp_EaseOutInBetween(ho.getExpParam().getDouble(), ho.getExpParam().getDouble(), ho.getExpParam().getInt(), ho.getExpParam().getInt(), ho.getExpParam().getDouble());
            case EXP_GETAMPLITUDE:
                return exp_GetAmplitude(ho.getExpParam().getInt());
            case EXP_GETOVERSHOOT:
                return exp_GetOvershoot(ho.getExpParam().getInt());
            case EXP_GETPERIOD:
                return exp_GetPeriod(ho.getExpParam().getInt());
            case EXP_GETDEFAULTAMPLITUDE:
                return exp_GetDefaultAmplitude();
            case EXP_GETDEFAULTOVERSHOOT:
                return exp_GetDefaultOvershoot();
            case EXP_GETDEFAULTPERIOD:
                return exp_GetDefaultPeriod();
            }
            return new CValue(0);
        }

        private function con_AnyObjectStopped():Boolean
        {
            return true;
        }

        private function con_SpecificObjectStopped(objType:PARAM_OBJECT):Boolean
        {
			var select:CRunObjectSelection = new CRunObjectSelection(rh.rhApp);
			if(currentMovedObj != null && select.objectIsOfType(currentMovedObj, objType.oi))
			{
				select.selectOneObject(currentMovedObj);
				return true;
			}
            return false;
        }

		//Should it select the given object?
		private function filterMoving(rdPtr:Object, object:CObject):Boolean
		{
			var easing:CRunEasing = CRunEasing(rdPtr);
			return (easing.controlled[object] != null);
		}
		
        private function con_IsObjectMoving(objType:PARAM_OBJECT, isNegated:Boolean):Boolean
        {
			var select:CRunObjectSelection = new CRunObjectSelection(rh.rhApp);
			var evtProg:CEventProgram = rh.rhEvtProg;
			var events:Array = evtProg.events;
			return select.filterObjects(this, objType.oi, isNegated, filterMoving);
        }

        private function act_MoveObj(obj:CObject, easingMode:CBinaryFile, targetX:int, targetY:int, timeMode:CBinaryFile, time:int):void
        {
			var easingVars:CRunEasingVars = new CRunEasingVars();
			easingVars.amplitude = easeVars.amplitude;
			easingVars.overshoot = easeVars.overshoot;
			easingVars.period = easeVars.period;
			easingVars.startX = obj.hoX;
			easingVars.startY = obj.hoY;
			easingVars.destX = targetX;
			easingVars.destY = targetY;
			easingVars.timespan = time;
			easingVars.fixed = GetFixedFromObject(obj);
			
			easingMode.skipBytes(1);
			easingVars.easingMode = easingMode.data.readUnsignedByte();
			easingVars.functionA = easingMode.data.readUnsignedByte();
			easingVars.functionB = easingMode.data.readUnsignedByte();
			
			easingVars.timeMode = timeMode.data.readUnsignedByte();
			if (easingVars.timeMode == 0){
				easingVars.startdate = new Date();
			}
			easingVars.starttime = 0;

			controlled[obj] = easingVars;
			controlledCount++;
        }

		private function act_MoveObjExplicit(fixed:int, easingMode:int, funcA:int, funcB:int, targetX:int, targetY:int, timeMode:int, time:int):void
        {
			var easingVars:CRunEasingVars = new CRunEasingVars();
			easingVars.amplitude = easeVars.amplitude;
			easingVars.overshoot = easeVars.overshoot;
			easingVars.period = easeVars.period;

			var obj:CObject = GetObjectFromFixed(fixed);
			if (obj == null)
				return;
			
			easingVars.fixed = fixed;
			easingVars.easingMode = easingMode;
			easingVars.functionA = funcA;
			easingVars.functionB = funcB;
			easingVars.startX = obj.hoX;
			easingVars.startY = obj.hoY;
			easingVars.destX = targetX;
			easingVars.destY = targetY;
			easingVars.timespan = time;
			
			easingVars.timeMode = timeMode;
			if (easingVars.timeMode == 0){
				easingVars.startdate = new Date();
			}
			easingVars.starttime = 0;

			controlled[obj] = easingVars;
			controlledCount++;
        }
		
        private function act_StopObject(obj:CObject):void
        {
			if (controlled[obj] != null)
			{
				delete controlled[obj];
				controlledCount--;	
			}
		}

        private function act_StopAllObjects():void
        {
			for (var obj:Object in controlled){
				delete controlled[obj];
			}
        }

        private function act_ReverseObject(object:CObject):void
        {
			var reversed:CRunEasingVars = null;
			var fixed:int = GetFixedFromObject(object);
			
			if (controlled[object] != null)
			{
				reversed = controlled[object];
				delete controlled[object];
			}
			if (reversed == null){
				reversed = new CRunEasingVars();
			}
			
			//If it was the object that was just stopped then use that one.
			if(reversed.fixed == 0)
			{
				if(currentMoved.fixed == fixed){
					reversed = currentMoved;
				}
				else{	//If no object found, abort
					return;
				}
			}

			reversed.destX = reversed.startX;
			reversed.destY = reversed.startY;
			
			reversed.startX = object.hoX;
			reversed.startY = object.hoY;
			
			//Recalculate the time it should take moving to the previous position			
			if(reversed.timeMode == 0)
			{			
				reversed.timespan = int(new Date().getTime() - reversed.startdate.getTime());
				reversed.startdate = new Date();
			}
			else
			{
				reversed.timespan = reversed.eventloop_step;
				reversed.eventloop_step = 0;
			}

			controlled[object] = reversed;
        }

        private function act_SetAmplitude(val:Number):void
        {
			easeVars.amplitude = val;
        }

        private function act_SetOvershoot(val:Number):void
        {
			easeVars.overshoot = val;
        }

        private function act_SetPeriod(val:Number):void
        {
			easeVars.period = val;
        }

        private function act_SetObjectAmplitude(obj:CObject, val:Number):void
        {
			var vars:CRunEasingVars = CRunEasingVars(controlled[obj]);
			if (vars == null){
				return;
			}
			
			vars.amplitude = val;
			controlled[obj] = vars;
        }

        private function act_SetObjectOvershoot(obj:CObject, val:Number):void
        {
			var vars:CRunEasingVars = CRunEasingVars(controlled[obj]);
			if (vars == null){
				return;
			}
			
			vars.overshoot = val;
			controlled[obj] = vars;
        }

        private function act_SetObjectPeriod(obj:CObject, val:Number):void
        {
			var vars:CRunEasingVars = CRunEasingVars(controlled[obj]);
			if (vars == null){
				return;
			}
			
			vars.period = val;
			controlled[obj] = vars;
        }

        private function exp_GetNumControlled():CValue
        {
            return new CValue(controlledCount);
        }

        private function exp_GetStoppedFixed():CValue
        {
			if (currentMovedObj == null){
				return new CValue(0);
			}
			return new CValue(GetFixedFromObject(currentMovedObj));
        }

        private function exp_EaseIn(func:int, step:Number):CValue
        {
            return new CValue(easeIn(func, step, easeVars));
        }

        private function exp_EaseOut(func:int, step:Number):CValue
        {
            return new CValue(easeOut(func, step, easeVars));
        }

        private function exp_EaseInOut(funcA:int, funcB:int, step:Number):CValue
        {
            return new CValue(easeInOut(funcA, funcB, step, easeVars));
        }

        private function exp_EaseOutIn(funcA:int, funcB:int, step:Number):CValue
        {
            return new CValue(easeOutIn(funcA, funcB , step, easeVars));
        }

        private function exp_EaseInBetween(start:Number, end:Number, func:int, step:Number):CValue
        {
			var ease:Number = easeIn(func, step, easeVars);
            return new CValue(start + (end-start)*ease);
        }

        private function exp_EaseOutBetween(start:Number, end:Number, func:int, step:Number):CValue
        {
			var ease:Number = easeOut(func, step, easeVars);
            return new CValue(start + (end-start)*ease);
        }

        private function exp_EaseInOutBetween(start:Number, end:Number, funcA:int, funcB:int, step:Number):CValue
        {
			var ease:Number = easeInOut(funcA, funcB, step, easeVars);
            return new CValue(start + (end-start)*ease);
        }

        private function exp_EaseOutInBetween(start:Number, end:Number, funcA:int, funcB:int, step:Number):CValue
        {
			var ease:Number = easeOutIn(funcA, funcB, step, easeVars);
            return new CValue(start + (end-start)*ease);
        }

        private function exp_GetAmplitude(fixed:int):CValue
        {
            var obj:CObject = GetControlledFromFixed(fixed);
			if (obj != null)
			{
				var vars:CRunEasingVars = CRunEasingVars(controlled[obj]);
				return new CValue(vars.amplitude);
			}
			return new CValue( -1);
        }

        private function exp_GetOvershoot(fixed:int):CValue
        {
            var obj:CObject = GetControlledFromFixed(fixed);
			if (obj != null)
			{
				var vars:CRunEasingVars = CRunEasingVars(controlled[obj]);
				return new CValue(vars.overshoot);
			}
			return new CValue( -1);
        }

        private function exp_GetPeriod(fixed:int):CValue
        {
			var obj:CObject = GetControlledFromFixed(fixed);
			if (obj != null)
			{
				var vars:CRunEasingVars = CRunEasingVars(controlled[obj]);
				return new CValue(vars.period);
			}
			return new CValue(-1);
        }

        private function exp_GetDefaultAmplitude():CValue
        {
            return new CValue(easeVars.amplitude);
        }

        private function exp_GetDefaultOvershoot():CValue
        {
            return new CValue(easeVars.overshoot);
        }

        private function exp_GetDefaultPeriod():CValue
        {
            return new CValue(easeVars.period);
        }

		
		//Easing functions:
		private function linear(step:Number, vars:CRunEasingVars):Number{ return step; }
		private function quad(step:Number, vars:CRunEasingVars):Number	{ return Math.pow(step, 2.0); }
		private function cubic(step:Number, vars:CRunEasingVars):Number	{ return Math.pow(step, 3.0); }
		private function quart(step:Number, vars:CRunEasingVars):Number	{ return Math.pow(step, 4.0); }
		private function quint(step:Number, vars:CRunEasingVars):Number	{ return Math.pow(step, 5.0); }
		private function sine(step:Number, vars:CRunEasingVars):Number	{ return 1.0-Math.sin((1-step)*90.0 * Math.PI/180.0); }
		private function expo(step:Number, vars:CRunEasingVars):Number	{ return Math.pow(2.0, step*10.0)/1024.0; }
		private function circ(step:Number, vars:CRunEasingVars):Number	{ return 1.0-Math.sqrt(1.0-Math.pow(step,2.0)); }
		private function back(step:Number, vars:CRunEasingVars):Number	{ return (vars.overshoot+1.0)* Math.pow(step, 3.0) - vars.overshoot*Math.pow(step, 2.0); }
		private function elastic(step:Number, vars:CRunEasingVars):Number {
			step -= 1.0;
			var amp:Number = Math.max(1.0, vars.amplitude);
			var s:Number = (vars.period / (2.0 * Math.PI) * Math.asin(1.0 / amp));
			return -(amp*Math.pow(2.0,10*step) * Math.sin((step-s)*(2*Math.PI)/vars.period));
		}
		private function bounce(step:Number, vars:CRunEasingVars):Number {
			step = 1-step;
			if (step < (8/22.0)){
				return 1 - 7.5625 * step * step;
			}
			else if (step < (16/22.0)){
				step -= 12/22.0;
				return 1 - vars.amplitude*(7.5625*step*step + 0.75) - (1-vars.amplitude);
			}
			else if (step < (20/22.0)){
				step -= 18/22.0;
				return 1 - vars.amplitude*(7.5625*step*step + 0.9375) - (1-vars.amplitude);
			}
			else{
				step -= 21/22.0;
				return 1 - vars.amplitude*(7.5625*step*step + 0.984375) - (1-vars.amplitude);
			}
		}

		private function doFunction(number:int, step:Number, vars:CRunEasingVars):Number
		{
			switch(number)
			{
			default:
			case 0: return linear(step, vars);
			case 1: return quad(step, vars);
			case 2: return cubic(step, vars);
			case 3: return quart(step, vars);
			case 4: return quint(step, vars);
			case 5: return sine(step, vars);
			case 6: return expo(step, vars);
			case 7: return circ(step, vars);
			case 8: return back(step, vars);
			case 9: return elastic(step, vars);
			case 10: return bounce(step, vars);
			}
		}

		private function easeIn(func:int, step:Number, vars:CRunEasingVars):Number
		{
			return doFunction(func, step, vars);
		}

		private function easeOut(func:int, step:Number, vars:CRunEasingVars):Number
		{
			return 1.0-doFunction(func, 1.0-step, vars);
		}

		private function easeInOut(functionA:int, functionB:int, step:Number, vars:CRunEasingVars):Number
		{
			if(step < 0.5){
				return easeIn(functionA, step * 2.0, vars) / 2.0;
			}
			else{
				return easeOut(functionB, (step - 0.5) * 2.0, vars) / 2.0 + 0.5;
			}
		}

		private function easeOutIn(functionA:int, functionB:int, step:Number, vars:CRunEasingVars):Number
		{
			if (step < 0.5){
				return easeOut(functionA, step * 2.0, vars) / 2.0;
			}
			else{
				return easeIn(functionB, (step - 0.5) * 2.0, vars) / 2.0 + 0.5;
			}
		}

		private function calculateEasingValue(mode:int, functionA:int, functionB:int, step:Number, vars:CRunEasingVars):Number
		{
			switch(mode)
			{
			default:
			case EASEIN:	return easeIn(functionA,step,vars);
			case EASEOUT:	return easeOut(functionA,step,vars);
			case EASEINOUT:	return easeInOut(functionA,functionB,step,vars);
			case EASEOUTIN:	return easeOutIn(functionA,functionB,step,vars);
			}
		}
		
		private function GetFixedFromObject(obj:CObject):int
	    {
	        return (obj.hoCreationId << 16) + (obj.hoNumber & 0xFFFF);
	    }
		private function GetControlledFromFixed(fixedvalue:int):CObject
	    {
	        for(var obj:Object in controlled)
        	{
				var cobj:CObject = CObject(obj);
	            if(GetFixedFromObject(cobj) == fixedvalue){	            	
	            	return cobj;
				}
	        }
			return null;
	    }
		private function GetObjectFromFixed(fixed:int):CObject
	    {
			return CObject(ho.hoAdRunHeader.rhObjectList[fixed & 0x0000FFFF]);
	    }
    }
}
