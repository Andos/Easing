package Extensions 
{
	import Objects.*;
	import RunLoop.*;
	import Services.*;
	import Application.*;
	import Events.*;
	import RunLoop.*;
	import Objects.CObject;
		
	/**
	 * ...
	 * @author Anders Riggelsen
	 */
	public class CRunObjectSelection 
	{
		private var rhPtr:CRunApp;
		private var run:CRun;
		private var eventProgram:CEventProgram;
		
		private var ObjectList:Array;		//CObject
		private var OiList:Array;			//CObjInfo
		private var QualToOiList:Array;		//CQualToOiList
		
		public function CRunObjectSelection(runApp:CRunApp)
		{
			rhPtr = runApp;
			run = rhPtr.run;
			eventProgram = rhPtr.frame.evtProg;
			ObjectList = run.rhObjectList;				//get a pointer to the mmf object list
			OiList = run.rhOiList;						//get a pointer to the mmf object info list
			QualToOiList = eventProgram.qualToOiList;	//get a pointer to the mmf qualifier to Oi list
		}

		//Selects *all* objects of the given object-type
		public function selectAll(Oi:int):void
		{
			var pObjectInfo:CObjInfo = OiList[Oi];
			pObjectInfo.oilNumOfSelected = pObjectInfo.oilNObjects;
			pObjectInfo.oilListSelected = pObjectInfo.oilObject;
			pObjectInfo.oilEventCount = eventProgram.rh2EventCount;

			var i:int = pObjectInfo.oilObject;
			while(i >= 0)
			{
				try{
					var pObject:CObject = ObjectList[i];
					pObject.hoNextSelected = pObject.hoNumNext;
					i = pObject.hoNumNext;
				}
				catch (e:Error)
				{
					//Ignore mysterious but non-important stack overflow
				}
			}
		}

		//Resets all objects of the given object-type
		public function selectNone(Oi:int):void
		{
			var pObjectInfo:CObjInfo = OiList[Oi];
			pObjectInfo.oilNumOfSelected = 0;
			pObjectInfo.oilListSelected = -1;
			pObjectInfo.oilEventCount = eventProgram.rh2EventCount;
		}

		//Resets the SOL and inserts only one given object
		public function selectOneObject(object:CObject):void
		{
			var pObjectInfo:CObjInfo = OiList[object.hoOi];
			pObjectInfo.oilNumOfSelected = 1;
			pObjectInfo.oilEventCount = eventProgram.rh2EventCount;
			pObjectInfo.oilListSelected = object.hoNumber;
			CObject(ObjectList[object.hoNumber]).hoNextSelected = -1;
		}

		//Resets the SOL and inserts the given list of objects
		public function selectObjects(Oi:int, objects:Array):void
		{
			var pObjectInfo:CObjInfo = OiList[Oi];

			pObjectInfo.oilNumOfSelected = objects.length;
			pObjectInfo.oilEventCount = eventProgram.rh2EventCount;
			
			var prevNumber:int = CObject(objects[0]).hoNumber;
			pObjectInfo.oilListSelected = prevNumber;
			
			for(var i:int=1; i<objects.length; i++)
			{
				var currentNumber:int = CObject(objects[i]).hoNumber;
				CObject(ObjectList[prevNumber]).hoNextSelected = currentNumber;
				prevNumber = currentNumber;
			}
			CObject(ObjectList[prevNumber]).hoNextSelected = -1;
		}

		//Run a custom filter on the SOL (via function callback)
		public function filterObjects(rdPtr:Object, Oi:int, negate:Boolean, filter:Function):Boolean
		{
			if (Oi & 0x8000){
				return Boolean(int(filterQualifierObjects(rdPtr, Oi & 0x7FFF, negate, filter)) ^ int(negate));
			}
			return Boolean(int(filterNonQualifierObjects(rdPtr, Oi & 0x7FFF, negate, filter)) ^ int(negate));
		}

		//Filter qualifier objects
		public function filterQualifierObjects(rdPtr:Object, Oi:int, negate:Boolean, filter:Function):Boolean
		{
			var CurrentQualToOiStart:CQualToOiList = QualToOiList[Oi];
			var CurrentQualToOi:CQualToOiList = CurrentQualToOiStart;

			var hasSelected:Boolean;
			var i:int = 0;

			while(CurrentQualToOi.qoiList[1] >= 0)
			{
				var CurrentOi:CObjInfo = OiList[CurrentQualToOi.qoiList[1]];
				hasSelected = Boolean(int(hasSelected) | int(filterNonQualifierObjects(rdPtr, CurrentOi.oilOi, negate, filter)));
				CurrentQualToOi = QualToOiList[Oi+i];
				++i;
			}
			return hasSelected;
		}

		//Filter normal objects
		public function filterNonQualifierObjects(rdPtr:Object, Oi:int, negate:Boolean, filter:Function):Boolean
		{
			var pObjectInfo:CObjInfo = OiList[Oi];
			var hasSelected:Boolean;
			if (pObjectInfo.oilEventCount != eventProgram.rh2EventCount){
				selectAll(Oi);	//The SOL is invalid, must reset.
			}

			//If SOL is empty
			if(pObjectInfo.oilNumOfSelected <= 0){
				return false;
			}

			var firstSelected:int = -1;
			var count:int = 0;
			var current:int = pObjectInfo.oilListSelected;
			var previous:CObject = null;

			while(current >= 0)
			{
				var pObject:CObject = ObjectList[current];
				var filterResult:Boolean = filter(rdPtr, pObject);
				var useObject:Boolean = Boolean(int(filterResult) ^ int(negate));
				hasSelected = Boolean(int(hasSelected) | int(useObject));

				if(useObject)
				{
					if(firstSelected == -1){
						firstSelected = current;
					}

					if(previous != null){
						previous.hoNextSelected = current;
					}
					
					previous = pObject;
					count++;
				}
				current = pObject.hoNextSelected;
			}
			if(previous != null){
				previous.hoNextSelected = -1;
			}

			pObjectInfo.oilListSelected = firstSelected;
			pObjectInfo.oilNumOfSelected = count;

			return hasSelected;
		}


		//Return the number of selected objects for the given object-type
		public function getNumberOfSelected(Oi:int):int
		{
			if(Oi & 0x8000)
			{
				Oi &= 0x7FFF;	//Mask out the qualifier part
				var numberSelected:int = 0;

				var CurrentQualToOiStart:CQualToOiList = QualToOiList[Oi];
				var CurrentQualToOi:CQualToOiList = CurrentQualToOiStart;

				var i:int = 0;
				while(CurrentQualToOi.qoiList[1] >= 0)
				{
					var CurrentOi:CObjInfo = OiList[CurrentQualToOi.qoiList[1]];
					numberSelected += CurrentOi.oilNumOfSelected;
					CurrentQualToOi = QualToOiList[Oi+i];
					++i;
				}
				return numberSelected;
			}
			else
			{
				var pObjectInfo:CObjInfo = OiList[Oi];
				return pObjectInfo.oilNumOfSelected;
			}
		}

		public function objectIsOfType(obj:CObject, Oi:int):Boolean
		{
			if(Oi & 0x8000)
			{
				Oi &= 0x7FFF;	//Mask out the qualifier part
				var CurrentQualToOiStart:CQualToOiList = QualToOiList[Oi];
				var CurrentQualToOi:CQualToOiList = CurrentQualToOiStart;

				var i:int = 0;
				while(CurrentQualToOi.qoiList[1] >= 0)
				{
					var CurrentOi:CObjInfo = OiList[CurrentQualToOi.qoiList[1]];
					if(CurrentOi.oilOi == obj.hoOi)
						return true;
					CurrentQualToOi = QualToOiList[Oi+i];
					++i;
				}
				return false;
			}
			return (obj.hoOi == Oi);
		}
	}
}