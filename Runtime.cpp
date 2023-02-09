// ============================================================================
//
// This file contains routines that are handled during the Runtime.
//
// Including creating, display, and handling your object.
// 
// ============================================================================

// Common Include
#include	"common.h"

// DEBUGGER /////////////////////////////////////////////////////////////////

#if !defined(RUN_ONLY)
// Identifiers of items displayed in the debugger
enum
{
// Example
// -------
//	DB_CURRENTSTRING,
//	DB_CURRENTVALUE,
//	DB_CURRENTCHECK,
//	DB_CURRENTCOMBO
};

// Items displayed in the debugger
WORD DebugTree[]=
{
// Example
// -------
//	DB_CURRENTSTRING|DB_EDITABLE,
//	DB_CURRENTVALUE|DB_EDITABLE,
//	DB_CURRENTCHECK,
//	DB_CURRENTCOMBO,

	// End of table (required)
	DB_END
};

#endif // !defined(RUN_ONLY)


// --------------------
// GetRunObjectDataSize
// --------------------
// Returns the size of the runtime datazone of the object
// 
ushort WINAPI DLLExport GetRunObjectDataSize(fprh rhPtr, LPEDATA edPtr)
{
	return(sizeof(RUNDATA));
}


// ---------------
// CreateRunObject
// ---------------
// The routine where the object is actually created
// 
short WINAPI DLLExport CreateRunObject(LPRDATA rdPtr, LPEDATA edPtr, fpcob cobPtr)
{
	rdPtr->controlled.vector<MoveStruct>::vector();
	rdPtr->deleted.vector<MoveStruct>::vector();

	memset(&rdPtr->currentMoved, 0, sizeof(MoveStruct));
	rdPtr->easingVars = edPtr->defaultVars;

	mv* mvI = rdPtr->rHo.hoAdRunHeader->rh4.rh4Mv;
	rdPtr->isHwa = mvI->mvCallFunction(NULL, EF_ISHWA, 0, 0, 0) == 1;
	rdPtr->isUnicode = mvI->mvCallFunction(NULL, EF_ISUNICODE, 0, 0, 0) == 1;	

	return 0;
}


// ----------------
// DestroyRunObject
// ----------------
// Destroys the run-time object
// 
short WINAPI DLLExport DestroyRunObject(LPRDATA rdPtr, long fast)
{
	rdPtr->controlled.clear();
	rdPtr->deleted.clear();
	rdPtr->controlled.vector<MoveStruct>::~vector();
	rdPtr->deleted.vector<MoveStruct>::~vector();

	return 0;
}


// ----------------
// HandleRunObject
// ----------------
// Called (if you want) each loop, this routine makes the object live
// 
short WINAPI DLLExport HandleRunObject(LPRDATA rdPtr)
{
	int fixedvalue = 0;
	bool finnishedMoving = false;
	float step;

	for(unsigned int i = 0; i<rdPtr->controlled.size(); i++)
	{
		fixedvalue = 0;
		LPRO object = NULL;
		MoveStruct & moved = rdPtr->controlled.at(i);

		if(i >= 0 && i < rdPtr->controlled.size())
		{
			fixedvalue = moved.fixedValue;
			object = LproFromFixed(rdPtr,fixedvalue);
		}

		if( object != NULL )
		{
			if (moved.pausecount != 0)
				continue;
			if(moved.timeMode == 0)
			{
				__int64 currentTime = CurrentTime();
				__int64 diff = currentTime - moved.starttime;

				step = diff / (float)moved.timespan;

				if(diff >= moved.timespan)
					finnishedMoving = true;
			}
			else
			{
				moved.eventloop_step++;
				step = moved.eventloop_step / (float)moved.timespan;

				if(moved.eventloop_step >= moved.timespan)
					finnishedMoving = true;
			}

			float easeStep = (float)calculateEasingValue(moved.easingMode,moved.functionA,moved.functionB,step, moved.vars);
			
			object->roHo.hoX = (int)(moved.startX + (moved.destX-moved.startX)*easeStep + 0.5f);
			object->roHo.hoY = (int)(moved.startY + (moved.destY-moved.startY)*easeStep + 0.5f);
			object->roc.rcChanged = true;

			if(finnishedMoving)
			{
				finnishedMoving = false;

				object->roHo.hoX = moved.destX;
				object->roHo.hoY = moved.destY;
				
				rdPtr->deleted.push_back(moved);

				rdPtr->controlled.erase( rdPtr->controlled.begin() + i );
				i--;
			}
		}
		else
		{
			rdPtr->controlled.erase( rdPtr->controlled.begin() + i );
			i--;
		}
	}

	//Trigger the 'Object stopped moving' events
	for(unsigned int d=0; d<rdPtr->deleted.size(); ++d)
	{
		rdPtr->currentMoved = rdPtr->deleted.at(d);
		callRunTimeFunction(rdPtr,RFUNCTION_GENERATEEVENT,CND_SPECIFICOBJECTSTOPPED,0);
		callRunTimeFunction(rdPtr,RFUNCTION_GENERATEEVENT,CND_ANYOBJECTSTOPPED,0);
	}
	
	rdPtr->deleted.clear();

	return 0;
}

// ----------------
// DisplayRunObject
// ----------------
// Draw the object in the application screen.
// 
short WINAPI DLLExport DisplayRunObject(LPRDATA rdPtr)
{
/*
   If you return REFLAG_DISPLAY in HandleRunObject this routine will run.
*/
	// Ok
	return 0;
}

// ----------------
// PauseRunObject
// ----------------
// Enters the pause mode
// 
short WINAPI DLLExport PauseRunObject(LPRDATA rdPtr)
{
	extern short WINAPI DLLExport act_PauseAllObjects(LPRDATA rdPtr, long param1, long param2);
	act_PauseAllObjects(rdPtr, 0, 0);

	// Ok
	return 0;
}


// -----------------
// ContinueRunObject
// -----------------
// Quits the pause mode
//
short WINAPI DLLExport ContinueRunObject(LPRDATA rdPtr)
{
	extern short WINAPI DLLExport act_ResumeAllObjects(LPRDATA rdPtr, long param1, long param2);
	act_ResumeAllObjects(rdPtr, 0, 0);

	// Ok
	return 0;
}

// -----------------
// SaveRunObject
// -----------------
// Saves the object to disk
// 

BOOL WINAPI SaveRunObject(LPRDATA rdPtr, HANDLE hf)
{            
	BOOL bOK = FALSE;

#ifndef VITALIZE

	// Save the object's data here

	// OK
	bOK = TRUE;

#endif // VITALIZE

	return bOK;
}

// -----------------
// LoadRunObject
// -----------------
// Loads the object from disk
// 
BOOL WINAPI LoadRunObject(LPRDATA rdPtr, HANDLE hf)
{            
	BOOL bOK=FALSE;

	// Load the object's data here

	// OK
	bOK = TRUE;

	return bOK; 
}




// ============================================================================
//
// START APP / END APP / START FRAME / END FRAME routines
// 
// ============================================================================

// -------------------
// StartApp
// -------------------
// Called when the application starts or restarts.
// Useful for storing global data
// 
void WINAPI DLLExport StartApp(mv _far *mV, CRunApp* pApp)
{
	// Example
	// -------
	// Delete global data (if restarts application)
//	CMyData* pData = (CMyData*)mV->mvGetExtUserData(pApp, hInstLib);
//	if ( pData != NULL )
//	{
//		delete pData;
//		mV->mvSetExtUserData(pApp, hInstLib, NULL);
//	}
}

// -------------------
// EndApp
// -------------------
// Called when the application ends.
// 
void WINAPI DLLExport EndApp(mv _far *mV, CRunApp* pApp)
{
	// Example
	// -------
	// Delete global data
//	CMyData* pData = (CMyData*)mV->mvGetExtUserData(pApp, hInstLib);
//	if ( pData != NULL )
//	{
//		delete pData;
//		mV->mvSetExtUserData(pApp, hInstLib, NULL);
//	}
}

// -------------------
// StartFrame
// -------------------
// Called when the frame starts or restarts.
// 
void WINAPI DLLExport StartFrame(mv _far *mV, DWORD dwReserved, int nFrameIndex)
{
}

// -------------------
// EndFrame
// -------------------
// Called when the frame ends.
// 
void WINAPI DLLExport EndFrame(mv _far *mV, DWORD dwReserved, int nFrameIndex)
{
}

// ============================================================================
//
// DEBUGGER ROUTINES
// 
// ============================================================================

// -----------------
// GetDebugTree
// -----------------
// This routine returns the address of the debugger tree
//
LPWORD WINAPI DLLExport GetDebugTree(LPRDATA rdPtr)
{
#if !defined(RUN_ONLY)
	return DebugTree;
#else
	return NULL;
#endif // !defined(RUN_ONLY)
}

// -----------------
// GetDebugItem
// -----------------
// This routine returns the text of a given item.
//
void WINAPI DLLExport GetDebugItem(LPSTR pBuffer, LPRDATA rdPtr, int id)
{
#if !defined(RUN_ONLY)

	// Example
	// -------
/*
	char temp[DB_BUFFERSIZE];

	switch (id)
	{
	case DB_CURRENTSTRING:
		LoadString(hInstLib, IDS_CURRENTSTRING, temp, DB_BUFFERSIZE);
		wsprintf(pBuffer, temp, rdPtr->text);
		break;
	case DB_CURRENTVALUE:
		LoadString(hInstLib, IDS_CURRENTVALUE, temp, DB_BUFFERSIZE);
		wsprintf(pBuffer, temp, rdPtr->value);
		break;
	case DB_CURRENTCHECK:
		LoadString(hInstLib, IDS_CURRENTCHECK, temp, DB_BUFFERSIZE);
		if (rdPtr->check)
			wsprintf(pBuffer, temp, "TRUE");
		else
			wsprintf(pBuffer, temp, "FALSE");
		break;
	case DB_CURRENTCOMBO:
		LoadString(hInstLib, IDS_CURRENTCOMBO, temp, DB_BUFFERSIZE);
		wsprintf(pBuffer, temp, rdPtr->combo);
		break;
	}
*/

#endif // !defined(RUN_ONLY)
}

// -----------------
// EditDebugItem
// -----------------
// This routine allows to edit editable items.
//
void WINAPI DLLExport EditDebugItem(LPRDATA rdPtr, int id)
{
#if !defined(RUN_ONLY)

	// Example
	// -------
/*
	switch (id)
	{
	case DB_CURRENTSTRING:
		{
			EditDebugInfo dbi;
			char buffer[256];

			dbi.pText=buffer;
			dbi.lText=TEXT_MAX;
			dbi.pTitle=NULL;

			strcpy(buffer, rdPtr->text);
			long ret=callRunTimeFunction(rdPtr, RFUNCTION_EDITTEXT, 0, (LPARAM)&dbi);
			if (ret)
				strcpy(rdPtr->text, dbi.pText);
		}
		break;
	case DB_CURRENTVALUE:
		{
			EditDebugInfo dbi;

			dbi.value=rdPtr->value;
			dbi.pTitle=NULL;

			long ret=callRunTimeFunction(rdPtr, RFUNCTION_EDITINT, 0, (LPARAM)&dbi);
			if (ret)
				rdPtr->value=dbi.value;
		}
		break;
	}
*/
#endif // !defined(RUN_ONLY)
}


