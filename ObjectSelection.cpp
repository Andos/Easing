#include "ObjectSelection.h"


ObjectSelection::ObjectSelection(LPRH rhPtr, bool isHwa, bool isUnicode)
{
	this->rhPtr = rhPtr;
	this->ObjectList = rhPtr->rhObjectList;		//get a pointer to the mmf object list
	this->OiList = rhPtr->rhOiList;				//get a pointer to the mmf object info list
	this->QualToOiList = rhPtr->rhQualToOiList;	//get a pointer to the mmf qualifier to Oi list

	oiListItemSize = offsetof(objInfoList, oilName) + 24 + sizeof(int); // sizeof(objInfoList); => using offsetof to make it compatible with both 2.0 and 2.5 SDKs
	if(isUnicode)
		oiListItemSize += 24;
	if(isHwa)
		oiListItemSize += sizeof(LPVOID);
}


//Selects *all* objects of the given object-type
void ObjectSelection::SelectAll(short Oil)
{
	LPOIL pObjectInfo = (LPOIL)(((char*)OiList) + oiListItemSize*Oil);

	pObjectInfo->oilNumOfSelected = pObjectInfo->oilNObjects;
	pObjectInfo->oilListSelected = pObjectInfo->oilObject;
	pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount;

	int i = pObjectInfo->oilObject;
	while(i >= 0)
	{
		LPHO pObject = ObjectList[i].oblOffset;
		pObject->hoNextSelected = pObject->hoNumNext;
		i = pObject->hoNumNext;
	}
}

//Resets all objects of the given object-type
void ObjectSelection::SelectNone(short Oil)
{
	LPOIL pObjectInfo = (LPOIL)(((char*)OiList) + oiListItemSize*Oil);

	pObjectInfo->oilNumOfSelected = 0;
	pObjectInfo->oilListSelected = -1;
	pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount;
}

//Resets the SOL and inserts only one given object
void ObjectSelection::SelectOneObject(LPRO object)
{
	LPOIL pObjectInfo = object->roHo.hoOiList;

	pObjectInfo->oilNumOfSelected = 1;
	pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount;
	pObjectInfo->oilListSelected = object->roHo.hoNumber;
	ObjectList[object->roHo.hoNumber].oblOffset->hoNextSelected = -1;
}

//Resets the SOL and inserts the given list of objects
void ObjectSelection::SelectObjects(short Oil, LPRO* objects, int count)
{
	if(count <= 0)
		return;

	LPOIL pObjectInfo = (LPOIL)(((char*)OiList) + oiListItemSize*Oil);

	pObjectInfo->oilNumOfSelected = count;
	pObjectInfo->oilEventCount = rhPtr->rh2.rh2EventCount;
	
	short prevNumber = objects[0]->roHo.hoNumber;
	pObjectInfo->oilListSelected = prevNumber;
	
	for(int i=1; i<count; i++)
	{
		short currentNumber = objects[i]->roHo.hoNumber;
		ObjectList[prevNumber].oblOffset->hoNextSelected = currentNumber;
		prevNumber = currentNumber;
	}
	ObjectList[prevNumber].oblOffset->hoNextSelected = -1;
}


//Run a custom filter on the SOL (via function callback)
bool ObjectSelection::FilterObjects(LPRDATA rdPtr, short Oil, bool negate, bool (*filterFunction)(LPRDATA, LPRO))
{
	if(Oil & 0x8000)
		return FilterQualifierObjects(rdPtr, Oil & 0x7FFF, negate, filterFunction) ^ negate;
	else
		return FilterNonQualifierObjects(rdPtr, Oil, negate, filterFunction) ^ negate;
}


//Filter qualifier objects
bool ObjectSelection::FilterQualifierObjects(LPRDATA rdPtr, short Oil, bool negate, bool (*filterFunction)(LPRDATA, LPRO))
{
	bool hasSelected = false;

	LPQOI CurrentQualToOi = (LPQOI)((char*)QualToOiList + (Oil & 0x7FFF));
	while(CurrentQualToOi->qoiOiList >= 0)
	{
		hasSelected |= FilterNonQualifierObjects(rdPtr, CurrentQualToOi->qoiOiList, negate, filterFunction);
		CurrentQualToOi = (LPQOI)((char*)CurrentQualToOi + 4);
	}
	return hasSelected;
}

//Filter normal objects
bool ObjectSelection::FilterNonQualifierObjects(LPRDATA rdPtr, short Oil, bool negate, bool (*filterFunction)(LPRDATA, LPRO))
{
	LPOIL pObjectInfo = (LPOIL)(((char*)OiList) + oiListItemSize*Oil);
	bool hasSelected = false;

	if ( pObjectInfo == NULL )
		return false;

	if(pObjectInfo->oilEventCount != rhPtr->rh2.rh2EventCount)
		SelectAll(Oil);	//The SOL is invalid, must reset.

	//If SOL is empty
	if(pObjectInfo->oilNumOfSelected <= 0)
		return false;

	int firstSelected = -1;
	int count = 0;
	int current = pObjectInfo->oilListSelected;
	LPHO previous = NULL;

	while(current >= 0)
	{
		LPHO pObject = ObjectList[current].oblOffset;
		bool useObject = filterFunction(rdPtr, (LPRO)pObject) ^ negate;
		hasSelected |= useObject;

		if(useObject)
		{
			if(firstSelected == -1)
				firstSelected = current;

			if(previous != NULL)
				previous->hoNextSelected = current;
			
			previous = pObject;
			count++;
		}
		current = pObject->hoNextSelected;
	}
	if(previous != NULL)
		previous->hoNextSelected = -1;

	pObjectInfo->oilListSelected = firstSelected;
	pObjectInfo->oilNumOfSelected = count;

	return hasSelected;
}

//Return the number of selected objects for the given object-type
int ObjectSelection::GetNumberOfSelected(short Oil)
{
	if(Oil & 0x8000)
	{
		Oil &= 0x7FFF;	//Mask out the qualifier part
		int numberSelected = 0;

		LPQOI CurrentQualToOiStart = (LPQOI)((char*)QualToOiList + Oil);
		LPQOI CurrentQualToOi = CurrentQualToOiStart;

		while(CurrentQualToOi->qoiOiList >= 0)
		{
			LPOIL CurrentOi = (LPOIL)(((char*)OiList) + oiListItemSize*CurrentQualToOi->qoiOiList);
			numberSelected += CurrentOi->oilNumOfSelected;
			CurrentQualToOi = (LPQOI)((char*)CurrentQualToOi + 4);
		}
		return numberSelected;
	}
	else
	{
		LPOIL pObjectInfo = (LPOIL)(((char*)OiList) + oiListItemSize*Oil);
		return pObjectInfo->oilNumOfSelected;
	}
}

bool ObjectSelection::ObjectIsOfType(LPRO object, short Oil)
{
	if(Oil & 0x8000)
	{
		Oil &= 0x7FFF;	//Mask out the qualifier part
		LPQOI CurrentQualToOiStart = (LPQOI)((char*)QualToOiList + Oil);
		LPQOI CurrentQualToOi = CurrentQualToOiStart;

		while(CurrentQualToOi->qoiOiList >= 0)
		{
			LPOIL CurrentOi = (LPOIL)(((char*)OiList) + oiListItemSize*CurrentQualToOi->qoiOiList);
			if(CurrentOi->oilOi == object->roHo.hoOi)
				return true;
			CurrentQualToOi = (LPQOI)((char*)CurrentQualToOi + 4);
		}
		return false;
	}
	else
	{
		LPOIL pObjectInfo = (LPOIL)(((char*)OiList) + oiListItemSize*Oil);
		return (object->roHo.hoOi == pObjectInfo->oilOi);
	}
}
