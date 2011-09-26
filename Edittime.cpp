// Common
#include	"common.h"
#if !defined(RUN_ONLY)

// PROPERTIES /////////////////////////////////////////////////////////////////

// Property identifiers
enum {
	PROPID_SETTINGS = PROPID_EXTITEM_CUSTOM_FIRST,

	PROPID_OVERSHOOT,
	PROPID_AMPLITUDE,
	PROPID_PERIOD,
};

// Property definitions
// Type, ID, Text, Text of Info box [, Options, Init Param]
PropData Properties[] = {

	PropData_EditFloat	(PROPID_OVERSHOOT,	IDS_PROP_OVERSHOOT,	IDS_PROPINFO_OVERSHOOT),
	PropData_EditFloat	(PROPID_AMPLITUDE,	IDS_PROP_AMPLITUDE,	IDS_PROPINFO_AMPLITUDE),
	PropData_EditFloat	(PROPID_PERIOD,		IDS_PROP_PERIOD,	IDS_PROPINFO_PERIOD),

	PropData_End()
};


char* GetMethodName(int index)
{
	char* names [] = {
		"Ease-In",
		"Ease-Out",
		"Ease-InOut",
		"Ease OutIn"
	};
	if(index >= 0 && index < 4)
		return names[index];
	else
		return names[0];
} 

char* GetFunctionName(int index)
{
	char* names [] = {
		"Linear",
		"Quadratic",
		"Cubic",
		"Quart",
		"Quint",
		"Sine",
		"Exponential",
		"Circular",
		"Back",
		"Elastic",
		"Bounce"
	};
	if(index >= 0 && index < 11)
		return names[index];
	else
		return names[0];
} 

BOOL CALLBACK DLLExport easingProc(HWND hDlg, uint msgType, WPARAM wParam, LPARAM lParam)
{
	EasingParam *	easingInfo;
	paramExt * paramInfo;

	HWND c_mode = GetDlgItem(hDlg, IDC_EASINGMODE);
	HWND firstFunction = GetDlgItem(hDlg, IDC_FIRSTFUNCTION);
	HWND secondFunction = GetDlgItem(hDlg, IDC_SECONDFUNCTION);

	switch (msgType)
	{
	case WM_INITDIALOG: // Init dialog
	{
		SetWindowLong(hDlg, DWL_USER, lParam);
		paramInfo = (paramExt*)lParam;
		easingInfo = (EasingParam*)paramInfo->pextData;
		int i;

		for(i=0; i<4; ++i)
			ComboBox_AddString(c_mode, GetMethodName(i));
		
		for(i=0; i<11; ++i)
		{
			ComboBox_AddString(firstFunction, GetFunctionName(i));
			ComboBox_AddString(secondFunction, GetFunctionName(i));
		}

		ComboBox_SetCurSel(c_mode, easingInfo->method);
		ComboBox_SetCurSel(firstFunction, easingInfo->firstFunction);
		ComboBox_SetCurSel(secondFunction, easingInfo->secondFunction);

		if(easingInfo->method <= 1)
			EnableWindow(secondFunction, FALSE);
		
		return TRUE;
	}

	case WM_COMMAND: // Command
	{

		paramInfo = (paramExt*)GetWindowLong(hDlg, DWL_USER);
		easingInfo = (EasingParam*)paramInfo->pextData;

		switch (wmCommandID)
		{
		case IDOK:
			{
				easingInfo->method = (unsigned char)ComboBox_GetCurSel(c_mode);
				easingInfo->firstFunction = (unsigned char)ComboBox_GetCurSel(firstFunction);
				easingInfo->secondFunction = (unsigned char)ComboBox_GetCurSel(secondFunction);
				paramInfo->pextSize = sizeof(paramExt) + sizeof(EasingParam);

				EndDialog(hDlg, IDOK);
			}
			return TRUE;

		case IDC_EASINGMODE:
		case IDC_FIRSTFUNCTION:
		case IDC_SECONDFUNCTION:
		{
			EnableWindow(secondFunction, !(ComboBox_GetCurSel(c_mode) <= 1));
			
			HWND easingDisplay = GetDlgItem(hDlg, IDC_EASINGDISPLAY);
			RECT rc;
			GetWindowRect(easingDisplay, &rc);
			ScreenToClient(hDlg, (LPPOINT)&rc.left);
			ScreenToClient(hDlg, (LPPOINT)&rc.right);
			
			RedrawWindow(hDlg, &rc, NULL, RDW_INVALIDATE);
			//RDW_INTERNALPAINT |

		}
			break;

		default:
			break;
		}
		break;
	}

	case WM_PAINT:
	{
		paramInfo = (paramExt*)GetWindowLong(hDlg, DWL_USER);
		easingInfo = (EasingParam*)paramInfo->pextData;

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);
		if (hdc != NULL)
		{
			HWND hPreview = GetDlgItem(hDlg, IDC_EASINGDISPLAY);
			if ( hPreview != NULL )
			{
				RECT rc;
				GetWindowRect(hPreview, &rc);
				ScreenToClient(hDlg, (LPPOINT)&rc.left);
				ScreenToClient(hDlg, (LPPOINT)&rc.right);

				// Create temporary surface (memory + DC)
				int w = rc.right-rc.left;
				int h = rc.bottom-rc.top;
				int depth = GetDeviceCaps (hdc, BITSPIXEL) * GetDeviceCaps (hdc, PLANES);
				LPSURFACE proto = NULL;
				GetSurfacePrototype(&proto, depth, ST_MEMORYWITHDC, SD_DIB);
				if ( proto != NULL )
				{
					cSurface sf;
					sf.Create(w, h, proto);

					// Get palette of application & set surface palette
					LPBYTE bufpal = NULL;
					if ( depth == 8 )
					{
						bufpal = new BYTE[sizeof(LOGPALETTE) + 256*sizeof(PALETTEENTRY)];
						if ( bufpal != NULL )
						{
							((LPLOGPALETTE)bufpal)->palVersion = 0x300;
							((LPLOGPALETTE)bufpal)->palNumEntries = 256;
							GetPaletteAppli((LPLOGPALETTE)bufpal, SM_8, easingInfo->mV->mvIdAppli);
							sf.SetPalette((LPLOGPALETTE)bufpal);
							sf.GetPaletteEntries(&((LPLOGPALETTE)bufpal)->palPalEntry[0], 0, 256);
						}
					}

					COLORREF lightblue = RGB(180,180,255);

					// Dialog background
					sf.Fill(255,255,255);
					sf.Rectangle(20,20,166,168,0,1,lightblue,false);

					unsigned char method = (unsigned char)ComboBox_GetCurSel(c_mode);
					unsigned char functionA = (unsigned char)ComboBox_GetCurSel(firstFunction);
					unsigned char functionB = (unsigned char)ComboBox_GetCurSel(secondFunction);

					EaseVars vars;
					vars.amplitude = 1.0f;
					vars.overshoot = 1.5f;
					vars.period = 0.4f;

					int prev = 0;
					for(int i=0; i<146; ++i)
					{
						int var = (int)(calculateEasingValue(method, functionA, functionB, i/146.0, vars)*146 + 0.5);
						sf.Line(i+20, 146-prev+20, i+20, 146-var+20, 1, BLACK);
						prev = var;
					}

					// Create DC surface
					cSurface    sdc;
					sdc.Create(hdc);

					// Select palette
					HPALETTE hPal = NULL;
					HPALETTE hOldPal = NULL;
					if ( bufpal != NULL )
					{
						hPal = CreatePalette((LPLOGPALETTE)bufpal);
						if ( hPal != NULL )
						{
							hOldPal = SelectPalette(hdc, hPal, TRUE);    // foreground palette
							RealizePalette(hdc);
						}
						delete [] bufpal;
						bufpal = NULL;
					}

					// Blit temp surface to screen
					sf.Blit(sdc, rc.left, rc.top);

					// Restore DC's palette
					if ( hOldPal != NULL )
						SelectPalette(hdc, hOldPal, TRUE);
				}
			}
		}
		EndPaint(hDlg, &ps);
		
	}
	break;

	default:
		break;
	}
	return FALSE;
}

BOOL CALLBACK DLLExport timeModeProc(HWND hDlg, uint msgType, WPARAM wParam, LPARAM lParam)
{
	TimeModeParam *	timeInfo;
	paramExt * paramInfo;

	HWND millseconds = GetDlgItem(hDlg, IDC_RADIO_MILLISECONDS);
	HWND eventloops = GetDlgItem(hDlg, IDC_RADIO_EVENTLOOPS);

	switch (msgType)
	{
	case WM_INITDIALOG: // Init dialog
		SetWindowLong(hDlg, DWL_USER, lParam);
		paramInfo = (paramExt*)lParam;
		timeInfo = (TimeModeParam*)paramInfo->pextData;

		if(timeInfo->type == 0)
			CheckRadioButton(hDlg, IDC_RADIO_MILLISECONDS, IDC_RADIO_EVENTLOOPS, IDC_RADIO_MILLISECONDS);
		else
			CheckRadioButton(hDlg, IDC_RADIO_MILLISECONDS, IDC_RADIO_EVENTLOOPS, IDC_RADIO_EVENTLOOPS);

		return TRUE;

	case WM_COMMAND: // Command

		paramInfo = (paramExt*)GetWindowLong(hDlg, DWL_USER);
		timeInfo = (TimeModeParam*)paramInfo->pextData;

		switch (wmCommandID)
		{
		case IDOK:
			{
				timeInfo->type = (IsDlgButtonChecked(hDlg, IDC_RADIO_EVENTLOOPS)==BST_CHECKED);
				paramInfo->pextSize = sizeof(paramExt) + sizeof(TimeModeParam);
				EndDialog(hDlg, IDOK);
			}
			return TRUE;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return FALSE;
}


// Structure defined to pass edptr and mv into setup box
typedef struct tagSetP
{
	EDITDATA _far *	edpt;
	mv _far	*		kv;
} setupParams;

#endif // !defined(RUN_ONLY)


// -----------------
// GetObjInfos
// -----------------
// Return object info
//
// Info displayed in the object's About properties
// Note: ObjComment is also displayed in the Quick Description box in the Insert Object dialog box
//
void WINAPI	DLLExport GetObjInfos (mv _far *mV, LPEDATA edPtr, LPSTR ObjName, LPSTR ObjAuthor, LPSTR ObjCopyright, LPSTR ObjComment, LPSTR ObjHttp)
{
#ifndef RUN_ONLY
	// Name
	LoadString(hInstLib, IDST_OBJNAME,ObjName, 255);

	// Author
	LoadString(hInstLib, IDST_AUTHOR,ObjAuthor,255);

	// Copyright
	LoadString(hInstLib, IDST_COPYRIGHT,ObjCopyright,255);

	// Comment
	LoadString(hInstLib, IDST_COMMENT,ObjComment,1024);

	// Internet address
	LoadString(hInstLib, IDST_HTTP,ObjHttp,255);
#endif // !defined(RUN_ONLY)
}

// -----------------
// GetHelpFileName
// -----------------
// Returns the help filename of the object.
//
LPCSTR WINAPI GetHelpFileName()
{
#ifndef RUN_ONLY
	// Return a file without path if your help file can be loaded by the MMF help file.
//	return "MyExt.chm";

	// Or return the path of your file, relatively to the MMF directory
	// if your file is not loaded by the MMF help file.
	return "Help\\Easing.chm";
#else
	return NULL;
#endif // !defined(RUN_ONLY)
}

// -----------------
// BmpToImg
// -----------------
// Converts an image from the resource to an image displayable under CC&C
// Not used in this template, but it is a good example on how to create
// an image.
//
/*
WORD BmpToImg(int bmID, npAppli idApp, short HotX = 0, short HotY = 0, short ActionX = 0, short ActionY = 0)
{
	Img					ifo;
	WORD				img;
	HRSRC				hs;
	HGLOBAL				hgBuf;
	LPBYTE				adBuf;
	LPBITMAPINFOHEADER	adBmi;

	img = 0;
	if ((hs = FindResource(hInstLib, MAKEINTRESOURCE(bmID), RT_BITMAP)) != NULL)
	{
		if ((hgBuf = LoadResource(hInstLib, hs)) != NULL)
		{
			if ((adBuf = (LPBYTE)LockResource(hgBuf)) != NULL)
			{
				adBmi = (LPBITMAPINFOHEADER)adBuf;
				ifo.imgXSpot = HotX;
				ifo.imgYSpot = HotY;
				ifo.imgXAction = ActionX;
				ifo.imgYAction = ActionY;
				if (adBmi->biBitCount > 4)
					RemapDib((LPBITMAPINFO)adBmi, idApp, NULL);
				img = (WORD)DibToImage(idApp, &ifo, adBmi);
				UnlockResource(hgBuf);
			}
			FreeResource(hgBuf);
		}
	}
	return img;
}
*/

// ============================================================================
//
// ROUTINES USED UNDER FRAME EDITOR
// 
// ============================================================================


// --------------------
// CreateObject
// --------------------
// Called when you choose "Create new object". It should display the setup box 
// and initialize everything in the datazone.

int WINAPI DLLExport CreateObject(mv _far *mV, fpLevObj loPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
	{
		edPtr->defaultVars.amplitude = 1.0f;
		edPtr->defaultVars.overshoot = 1.5f;
		edPtr->defaultVars.period = 0.4f;

		// Set default object settings
//		edPtr->swidth = 32;
//		edPtr->sheight = 32;
	}
#endif // !defined(RUN_ONLY)
	return 0;
}

// --------------------
// EditObject
// --------------------
// Called when the user selects the Edit command in the object's popup menu
//
BOOL WINAPI EditObject (mv _far *mV, fpObjInfo oiPtr, fpLevObj loPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
	{

	}
#endif // !defined(RUN_ONLY)
	return FALSE;
}

// --------------------
// SetEditSize
// --------------------
// Called when the object has been resized
//
// Note: remove the comments if your object can be resized (and remove the comments in the .def file)
/*
BOOL WINAPI SetEditSize(LPMV mv, LPEDATA edPtr, int cx, int cy)
{
#ifndef RUN_ONLY
	edPtr->swidth = cx;
	edPtr->sheight = cy;
#endif // !defined(RUN_ONLY)
	return TRUE;	// OK
}
*/

// --------------------
// PutObject
// --------------------
// Called when each individual object is dropped in the frame.
//
void WINAPI	DLLExport PutObject(mv _far *mV, fpLevObj loPtr, LPEDATA edPtr, ushort cpt)
{
#ifndef RUN_ONLY
#endif // !defined(RUN_ONLY)
}

// --------------------
// RemoveObject
// --------------------
// Called when each individual object is removed from the frame.
//
void WINAPI	DLLExport RemoveObject(mv _far *mV, fpLevObj loPtr, LPEDATA edPtr, ushort cpt)
{
#ifndef RUN_ONLY
	// Is the last object removed?
    if (0 == cpt)
	{
		// Do whatever necessary to remove our data
	}
#endif // !defined(RUN_ONLY)
}

// --------------------
// DuplicateObject
// --------------------
// Called when an object is created from another one (note: should be called CloneObject instead...)
//
void WINAPI DLLExport DuplicateObject(mv __far *mV, fpObjInfo oiPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
#endif // !defined(RUN_ONLY)
}

// --------------------
// GetObjectRect
// --------------------
// Returns the size of the rectangle of the object in the frame editor.
//
void WINAPI DLLExport GetObjectRect(mv _far *mV, RECT FAR *rc, fpLevObj loPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	rc->right = rc->left + 32;	// edPtr->swidth;
	rc->bottom = rc->top + 32;	// edPtr->sheight;
#endif // !defined(RUN_ONLY)
	return;
}


// --------------------
// EditorDisplay
// --------------------
// Displays the object under the frame editor
//
// Note: this function is optional. If it's not defined in your extension,
// MMF2 will load and display the EXO_IMAGE bitmap if it's defined in your resource file.
//
// If you need to draw the icon manually, remove the comments around this function and in the .def file.
//
/*
void WINAPI DLLExport EditorDisplay(mv _far *mV, fpObjInfo oiPtr, fpLevObj loPtr, LPEDATA edPtr, RECT FAR *rc)
{
#ifndef RUN_ONLY

	// This is a simple case of drawing an image onto MMF's frame editor window
	// First, we must get a pointer to the surface used by the frame editor

	LPSURFACE ps = WinGetSurface((int)mV->mvIdEditWin);
	if ( ps != NULL )		// Do the following if this surface exists
	{
		int x = rc->left;	// get our boundaries
		int y = rc->top;
		int w = rc->right-rc->left;
		int h = rc->bottom-rc->top;

		cSurface is;			// New surface variable for us to use
		is.Create(4, 4, ps);	// Create a surface implementation from a prototype (frame editor win)
		is.LoadImage(hInstLib, EXO_IMAGE, LI_REMAP);	// Load our bitmap from the resource,
														// and remap palette if necessary
		is.Blit(*ps, x, y, BMODE_TRANSP, BOP_COPY, 0);	// Blit the image to the frame editor surface!
		// This actually blits (or copies) the whole of our surface onto the frame editor's surface
		// at a specified position.
		// We could use different image effects when we copy, e.g. invert, AND, OR, XOR,
		// blend (semi-transparent, the 6th param is amount of transparency)
		// You can 'anti-alias' with the 7th param (default=0 or off)
	}

#endif // !defined(RUN_ONLY)
}
*/

// --------------------
// IsTransparent
// --------------------
// This routine tells CC&C if the mouse pointer is over a transparent zone of the object.
// 

extern "C" BOOL WINAPI DLLExport IsTransparent(mv _far *mV, fpLevObj loPtr, LPEDATA edPtr, int dx, int dy)
{
#ifndef RUN_ONLY
	// Write your code here
#endif // !defined(RUN_ONLY)
	return FALSE;
}

// --------------------
// PrepareToWriteObject
// --------------------
// Just before writing the datazone when saving the application, CC&C calls this routine.
// 
void WINAPI	DLLExport PrepareToWriteObject(mv _far *mV, LPEDATA edPtr, fpObjInfo adoi)
{
#ifndef RUN_ONLY
	// Write your code here
#endif // !defined(RUN_ONLY)
}

// --------------------
// GetFilters
// --------------------

BOOL WINAPI GetFilters(LPMV mV, LPEDATA edPtr, DWORD dwFlags, LPVOID pReserved)
{
#ifndef RUN_ONLY
	// If your extension uses image filters
//	if ( (dwFlags & GETFILTERS_IMAGES) != 0 )
//		return TRUE;

	// If your extension uses sound filters
//	if ( (dwFlags & GETFILTERS_SOUNDS) != 0 )
//		return TRUE;
#endif // RUN_ONLY
	return FALSE;
}

// --------------------
// UsesFile
// --------------------
// Triggers when a file is dropped onto the frame
// Return TRUE if you can create an object from the given file
//
BOOL WINAPI	DLLExport UsesFile (LPMV mV, LPSTR fileName)
{
	BOOL r = FALSE;
#ifndef RUN_ONLY

	// Example: return TRUE if file extension is ".txt"
/*	
	LPSTR	ext, npath;

	if ( fileName != NULL )
	{
		if ( (ext=(LPSTR)malloc(_MAX_EXT)) != NULL )
		{
			if ( (npath=(LPSTR)malloc(_MAX_PATH)) != NULL )
			{
				strcpy(npath, fileName);
				_splitpath(npath, NULL, NULL, NULL, ext);
				if ( _stricmp(ext, ".txt") == 0 )
					r = TRUE;
				free(npath);
			}
			free(ext);
		}
	} */
#endif // !defined(RUN_ONLY)
	return r;
}


// --------------------
// CreateFromFile
// --------------------
// Creates a new object from file
//
void WINAPI	DLLExport CreateFromFile (LPMV mV, LPSTR fileName, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Initialize your extension data from the given file
//	edPtr->swidth = 32;
//	edPtr->sheight = 32;

	// Example: store the filename
	// strcpy(edPtr->myFileName, fileName);
#endif // !defined(RUN_ONLY)
}

// ============================================================================
//
// PROPERTIES
// 
// ============================================================================

// --------------------
// GetProperties
// --------------------
// Inserts properties into the properties of the object.
//
BOOL WINAPI DLLExport GetProperties(LPMV mV, LPEDATA edPtr, BOOL bMasterItem)
{
#ifndef RUN_ONLY
	mvInsertProps(mV, edPtr, Properties, PROPID_TAB_GENERAL, TRUE);
#endif // !defined(RUN_ONLY)

	// OK
	return TRUE;
}

// --------------------
// ReleaseProperties
// --------------------
// Called when the properties are removed from the property window.
//
void WINAPI DLLExport ReleaseProperties(LPMV mV, LPEDATA edPtr, BOOL bMasterItem)
{
}

// --------------------
// GetPropCreateParam
// --------------------
// Called when a property is initialized and its creation parameter is NULL (in the PropData).
// Allows you, for example, to change the content of a combobox property according to specific settings in the EDITDATA structure.
//
LPARAM WINAPI DLLExport GetPropCreateParam(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
	return NULL;
}

// ----------------------
// ReleasePropCreateParam
// ----------------------
// Called after a property has been initialized.
// Allows you, for example, to free memory allocated in GetPropCreateParam.
//
void WINAPI DLLExport ReleasePropCreateParam(LPMV mV, LPEDATA edPtr, UINT nPropID, LPARAM lParam)
{
}

// --------------------
// GetPropValue
// --------------------
// Returns the value of properties that have a value.
// Note: see GetPropCheck for checkbox properties
//
LPVOID WINAPI DLLExport GetPropValue(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY

	switch(nPropID)
	{
	case PROPID_OVERSHOOT:
		return new CPropFloatValue(edPtr->defaultVars.overshoot);
	case PROPID_AMPLITUDE:
		return new CPropFloatValue(edPtr->defaultVars.amplitude);
	case PROPID_PERIOD:
		return new CPropFloatValue(edPtr->defaultVars.period);
	}

#endif // !defined(RUN_ONLY)
	return NULL;
}

// --------------------
// GetPropCheck
// --------------------
// Returns the checked state of properties that have a check box.
//
BOOL WINAPI DLLExport GetPropCheck(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
	return 0;
}

// --------------------
// SetPropValue
// --------------------
// This routine is called by MMF after a property has been modified.
//
void WINAPI DLLExport SetPropValue(LPMV mV, LPEDATA edPtr, UINT nPropID, LPVOID lParam)
{
#ifndef RUN_ONLY

	CPropValue* pValue = (CPropValue*)lParam;

	switch(nPropID)
	{
	case PROPID_OVERSHOOT:
		edPtr->defaultVars.overshoot = ((CPropFloatValue*)pValue)->m_fValue;
		break;
	case PROPID_AMPLITUDE:
		edPtr->defaultVars.amplitude = ((CPropFloatValue*)pValue)->m_fValue;
		break;
	case PROPID_PERIOD:
		edPtr->defaultVars.period = ((CPropFloatValue*)pValue)->m_fValue;
		break;
	}

#endif // !defined(RUN_ONLY)
}

// --------------------
// SetPropCheck
// --------------------
// This routine is called by MMF when the user modifies a checkbox in the properties.
//
void WINAPI DLLExport SetPropCheck(LPMV mV, LPEDATA edPtr, UINT nPropID, BOOL nCheck)
{
#ifndef RUN_ONLY
	// Example
	// -------
//	switch (nPropID)
//	{
//	case PROPID_CHECK:
//		edPtr->nCheck = nCheck;
//		mvInvalidateObject(mV, edPtr);
//		mvRefreshProp(mV, edPtr, PROPID_COMBO, TRUE);
//		break;
//	}
#endif // !defined(RUN_ONLY)
}

// --------------------
// EditProp
// --------------------
// This routine is called when the user clicks the button of a Button or EditButton property.
//
BOOL WINAPI DLLExport EditProp(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY

	// Example
	// -------
/*
	if (nPropID==PROPID_EDITCONTENT)
	{
		if ( EditObject(mV, NULL, NULL, edPtr) )
			return TRUE;
	}
*/

#endif // !defined(RUN_ONLY)
	return FALSE;
}

// --------------------
// IsPropEnabled
// --------------------
// This routine returns the enabled state of a property.
//
BOOL WINAPI IsPropEnabled(LPMV mV, LPEDATA edPtr, UINT nPropID)
{
#ifndef RUN_ONLY
	// Example
	// -------
/*
	switch (nPropID) {

	case PROPID_CHECK:
		return (edPtr->nComboIndex != 0);
	}
*/
#endif // !defined(RUN_ONLY)
	return TRUE;
}


// ============================================================================
//
// TEXT PROPERTIES
// 
// ============================================================================

// --------------------
// GetTextCaps
// --------------------
// Return the text capabilities of the object under the frame editor.
//
DWORD WINAPI DLLExport GetTextCaps(mv _far *mV, LPEDATA edPtr)
{
	return 0;	// (TEXT_ALIGN_LEFT|TEXT_ALIGN_HCENTER|TEXT_ALIGN_RIGHT|TEXT_ALIGN_TOP|TEXT_ALIGN_VCENTER|TEXT_ALIGN_BOTTOM|TEXT_FONT|TEXT_COLOR);
}

// --------------------
// GetTextFont
// --------------------
// Return the font used the object.
// Note: the pStyle and cbSize parameters are obsolete and passed for compatibility reasons only.
//
BOOL WINAPI DLLExport GetTextFont(mv _far *mV, LPEDATA edPtr, LPLOGFONT plf, LPSTR pStyle, UINT cbSize)
{
#if !defined(RUN_ONLY)
	// Example: copy LOGFONT structure from EDITDATA
	// memcpy(plf, &edPtr->m_lf, sizeof(LOGFONT));
#endif // !defined(RUN_ONLY)

	return TRUE;
}

// --------------------
// SetTextFont
// --------------------
// Change the font used the object.
// Note: the pStyle parameter is obsolete and passed for compatibility reasons only.
//
BOOL WINAPI DLLExport SetTextFont(mv _far *mV, LPEDATA edPtr, LPLOGFONT plf, LPCSTR pStyle)
{
#if !defined(RUN_ONLY)
	// Example: copy LOGFONT structure to EDITDATA
	// memcpy(&edPtr->m_lf, plf, sizeof(LOGFONT));
#endif // !defined(RUN_ONLY)

	return TRUE;
}

// --------------------
// GetTextClr
// --------------------
// Get the text color of the object.
//
COLORREF WINAPI DLLExport GetTextClr(mv _far *mV, LPEDATA edPtr)
{
	// Example
	return 0;	// edPtr->fontColor;
}

// --------------------
// SetTextClr
// --------------------
// Set the text color of the object.
//
void WINAPI DLLExport SetTextClr(mv _far *mV, LPEDATA edPtr, COLORREF color)
{
	// Example
	//edPtr->fontColor = color;
}

// --------------------
// GetTextAlignment
// --------------------
// Get the text alignment of the object.
//
DWORD WINAPI DLLExport GetTextAlignment(mv _far *mV, LPEDATA edPtr)
{
	DWORD dw = 0;
#if !defined(RUN_ONLY)
	// Example
	// -------
/*	if ( (edPtr->eData.dwFlags & ALIGN_LEFT) != 0 )
		dw |= TEXT_ALIGN_LEFT;
	if ( (edPtr->eData.dwFlags & ALIGN_HCENTER) != 0 )
		dw |= TEXT_ALIGN_HCENTER;
	if ( (edPtr->eData.dwFlags & ALIGN_RIGHT) != 0 )
		dw |= TEXT_ALIGN_RIGHT;
	if ( (edPtr->eData.dwFlags & ALIGN_TOP) != 0 )
		dw |= TEXT_ALIGN_TOP;
	if ( (edPtr->eData.dwFlags & ALIGN_VCENTER) != 0 )
		dw |= TEXT_ALIGN_VCENTER;
	if ( (edPtr->eData.dwFlags & ALIGN_BOTTOM) != 0 )
		dw |= TEXT_ALIGN_BOTTOM;
*/
#endif // !defined(RUN_ONLY)
	return dw;
}

// --------------------
// SetTextAlignment
// --------------------
// Set the text alignment of the object.
//
void WINAPI DLLExport SetTextAlignment(mv _far *mV, LPEDATA edPtr, DWORD dwAlignFlags)
{
#if !defined(RUN_ONLY)
	// Example
	// -------
/*	DWORD dw = edPtr->eData.dwFlags;

	if ( (dwAlignFlags & TEXT_ALIGN_LEFT) != 0 )
		dw = (dw & ~(ALIGN_LEFT|ALIGN_HCENTER|ALIGN_RIGHT)) | ALIGN_LEFT;
	if ( (dwAlignFlags & TEXT_ALIGN_HCENTER) != 0 )
		dw = (dw & ~(ALIGN_LEFT|ALIGN_HCENTER|ALIGN_RIGHT)) | ALIGN_HCENTER;
	if ( (dwAlignFlags & TEXT_ALIGN_RIGHT) != 0 )
		dw = (dw & ~(ALIGN_LEFT|ALIGN_HCENTER|ALIGN_RIGHT)) | ALIGN_RIGHT;

	if ( (dwAlignFlags & TEXT_ALIGN_TOP) != 0 )
		dw = (dw & ~(ALIGN_TOP|ALIGN_VCENTER|ALIGN_BOTTOM)) | ALIGN_TOP;
	if ( (dwAlignFlags & TEXT_ALIGN_VCENTER) != 0 )
		dw = (dw & ~(ALIGN_TOP|ALIGN_VCENTER|ALIGN_BOTTOM)) | ALIGN_VCENTER;
	if ( (dwAlignFlags & TEXT_ALIGN_BOTTOM) != 0 )
		dw = (dw & ~(ALIGN_TOP|ALIGN_VCENTER|ALIGN_BOTTOM)) | ALIGN_BOTTOM;

	edPtr->eData.dwFlags = dw;
*/
#endif // !defined(RUN_ONLY)
}


// ============================================================================
//
// ROUTINES USED UNDER EVENT / TIME / STEP-THROUGH EDITOR
// You should not need to change these routines
// 
// ============================================================================

// -----------------
// menucpy
// -----------------
// Internal routine used later, copy one menu onto another
// 
#ifndef RUN_ONLY
void menucpy(HMENU hTargetMenu, HMENU hSourceMenu)
{
	int			n, id, nMn;
	NPSTR		strBuf;
	HMENU		hSubMenu;

	nMn = GetMenuItemCount(hSourceMenu);
	strBuf = (NPSTR)LocalAlloc(LPTR, 80);
	for (n=0; n<nMn; n++)
	{
		if (0 == (id = GetMenuItemID(hSourceMenu, n)))
			AppendMenu(hTargetMenu, MF_SEPARATOR, 0, 0L);
		else
		{
			GetMenuString(hSourceMenu, n, strBuf, 80, MF_BYPOSITION);
			if (id != -1)
				AppendMenu(hTargetMenu, GetMenuState(hSourceMenu, n, MF_BYPOSITION), id, strBuf);
			else
			{
				hSubMenu = CreatePopupMenu();
				AppendMenu(hTargetMenu, MF_POPUP | MF_STRING, (uint)hSubMenu, strBuf);
				menucpy(hSubMenu, GetSubMenu(hSourceMenu, n));
			}
		}
	}
	LocalFree((HLOCAL)strBuf);
}

// -----------------
// GetPopupMenu
// -----------------
// Internal routine used later. Returns the first popup from a menu
// 
HMENU GetPopupMenu(short mn)
{
	HMENU	hMn, hSubMenu, hPopup = NULL;

	if ((hMn = LoadMenu(hInstLib, MAKEINTRESOURCE(mn))) != NULL)
	{
		if ((hSubMenu = GetSubMenu(hMn, 0)) != NULL)
		{
			if ((hPopup = CreatePopupMenu()) != NULL)
				menucpy(hPopup, hSubMenu);
		}
		DestroyMenu(hMn);
	}
	return hPopup;
}

// --------------------
// GetEventInformations
// --------------------
// Internal routine used later. Look for one event in one of the eventInfos array...
// No protection to go faster: you must properly enter the conditions/actions!
// 
static LPEVENTINFOS2 GetEventInformations(LPEVENTINFOS2 eiPtr, short code)

{
	while(eiPtr->infos.code != code)
		eiPtr = EVINFO2_NEXT(eiPtr);
	
	return eiPtr;
}
#endif // !defined(RUN_ONLY)


// ----------------------------------------------------
// GetConditionMenu / GetActionMenu / GetExpressionMenu
// ----------------------------------------------------
// Load the condition/action/expression menu from the resource, eventually
// enable or disable some options, and returns it to CC&C.
//
HMENU WINAPI DLLExport GetConditionMenu(mv _far *mV, fpObjInfo oiPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
		return GetPopupMenu(MN_CONDITIONS);
#endif // !defined(RUN_ONLY)
	return NULL;
}

HMENU WINAPI DLLExport GetActionMenu(mv _far *mV, fpObjInfo oiPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
		return GetPopupMenu(MN_ACTIONS);
#endif // !defined(RUN_ONLY)
	return NULL;
}

HMENU WINAPI DLLExport GetExpressionMenu(mv _far *mV, fpObjInfo oiPtr, LPEDATA edPtr)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
		return GetPopupMenu(MN_EXPRESSIONS);
#endif // !defined(RUN_ONLY)
	return NULL;
}


// -------------------------------------------------------
// GetConditionTitle / GetActionTitle / GetExpressionTitle
// -------------------------------------------------------
// Returns the title of the dialog box displayed when entering
// parameters for the condition, action or expressions, if any.
// Here, we simply return the title of the menu option
//

#ifndef RUN_ONLY
void GetCodeTitle(LPEVENTINFOS2 eiPtr, short code, short param, short mn, LPSTR strBuf, WORD maxLen)
{
	HMENU		hMn;

	// Finds event in array
	eiPtr=GetEventInformations(eiPtr, code);

	// If a special string is to be returned
	short strID = EVINFO2_PARAMTITLE(eiPtr, param);

	if ( strID != 0 )
		LoadString(hInstLib, strID, strBuf, maxLen);
	else
	{
		// Otherwise, returns the menu option 
		if ((hMn = LoadMenu(hInstLib, MAKEINTRESOURCE(mn))) != NULL )
		{
			GetMenuString(hMn, eiPtr->menu, strBuf, maxLen, MF_BYCOMMAND);
			DestroyMenu(hMn);
		}
	}
}
#else
#define GetCodeTitle(a,b,c,d,e,f)
#endif // !defined(RUN_ONLY)

void WINAPI DLLExport GetConditionTitle(mv _far *mV, short code, short param, LPSTR strBuf, short maxLen)
{
	GetCodeTitle((LPEVENTINFOS2)conditionsInfos, code, param, MN_CONDITIONS, strBuf, maxLen);
}
void WINAPI DLLExport GetActionTitle(mv _far *mV, short code, short param, LPSTR strBuf, short maxLen)
{
	GetCodeTitle((LPEVENTINFOS2)actionsInfos, code, param, MN_ACTIONS, strBuf, maxLen);
}
void WINAPI DLLExport GetExpressionTitle(mv _far *mV, short code, LPSTR strBuf, short maxLen)
{
	GetCodeTitle((LPEVENTINFOS2)expressionsInfos, code, 0, MN_EXPRESSIONS, strBuf, maxLen);
}

// -------------------------------------------------------
// GetConditionTitle / GetActionTitle / GetExpressionTitle
// -------------------------------------------------------
// From a menu ID, these routines returns the code of the condition,
// action or expression, as defined in the .H file
//

short WINAPI DLLExport GetConditionCodeFromMenu(mv _far *mV, short menuId)
{
#ifndef RUN_ONLY
	LPEVENTINFOS2	eiPtr;
	int				n;

	for (n=CND_LAST, eiPtr=(LPEVENTINFOS2)conditionsInfos; n>0 && eiPtr->menu!=menuId; n--)
		eiPtr = EVINFO2_NEXT(eiPtr);
	if (n>0) 
		return eiPtr->infos.code;
#endif // !defined(RUN_ONLY)
	return -1;
}

short WINAPI DLLExport GetActionCodeFromMenu(mv _far *mV, short menuId)
{
#ifndef RUN_ONLY
	LPEVENTINFOS2	eiPtr;
	int				n;

	for (n=ACT_LAST, eiPtr=(LPEVENTINFOS2)actionsInfos; n>0 && eiPtr->menu!=menuId; n--)
		eiPtr = EVINFO2_NEXT(eiPtr);
	if (n>0) 
		return eiPtr->infos.code;
#endif // !defined(RUN_ONLY)
	return -1;
}

short WINAPI DLLExport GetExpressionCodeFromMenu(mv _far *mV, short menuId)
{
#ifndef RUN_ONLY
	LPEVENTINFOS2	eiPtr;
	int				n;

	for (n=EXP_LAST, eiPtr=(LPEVENTINFOS2)expressionsInfos; n>0 && eiPtr->menu!=menuId; n--)
		eiPtr = EVINFO2_NEXT(eiPtr);
	if (n>0) 
		return eiPtr->infos.code;
#endif // !defined(RUN_ONLY)
	return -1;
}


// -------------------------------------------------------
// GetConditionInfos / GetActionInfos / GetExpressionInfos
// -------------------------------------------------------
// From a action / condition / expression code, returns 
// an infosEvents structure. 
//

LPINFOEVENTSV2 WINAPI DLLExport GetConditionInfos(mv _far *mV, short code)
{
#ifndef RUN_ONLY
	return &GetEventInformations((LPEVENTINFOS2)conditionsInfos, code)->infos;
#else
	return NULL;
#endif // !defined(RUN_ONLY)
}

LPINFOEVENTSV2 WINAPI DLLExport GetActionInfos(mv _far *mV, short code)
{
#ifndef RUN_ONLY
	return &GetEventInformations((LPEVENTINFOS2)actionsInfos, code)->infos;
#else
	return NULL;
#endif // !defined(RUN_ONLY)
}

LPINFOEVENTSV2 WINAPI DLLExport GetExpressionInfos(mv _far *mV, short code)
{
#ifndef RUN_ONLY
	return &GetEventInformations((LPEVENTINFOS2)expressionsInfos, code)->infos;
#else
	return NULL;
#endif // !defined(RUN_ONLY)
}


// ----------------------------------------------------------
// GetConditionString / GetActionString / GetExpressionString
// ----------------------------------------------------------
// From a action / condition / expression code, returns 
// the string to use for displaying it under the event editor
//

void WINAPI DLLExport GetConditionString(mv _far *mV, short code, LPSTR strPtr, short maxLen)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
		LoadString(hInstLib, GetEventInformations((LPEVENTINFOS2)conditionsInfos, code)->string, strPtr, maxLen);
#endif // !defined(RUN_ONLY)
}

void WINAPI DLLExport GetActionString(mv _far *mV, short code, LPSTR strPtr, short maxLen)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
		LoadString(hInstLib, GetEventInformations((LPEVENTINFOS2)actionsInfos, code)->string, strPtr, maxLen);
#endif // !defined(RUN_ONLY)
}

void WINAPI DLLExport GetExpressionString(mv _far *mV, short code, LPSTR strPtr, short maxLen)
{
#ifndef RUN_ONLY
	// Check compatibility
	if ( IS_COMPATIBLE(mV) )
		LoadString(hInstLib, GetEventInformations((LPEVENTINFOS2)expressionsInfos, code)->string, strPtr, maxLen);
#endif // !defined(RUN_ONLY)
}

// ----------------------------------------------------------
// GetExpressionParam
// ----------------------------------------------------------
// Returns the parameter name to display in the expression editor
//
void WINAPI DLLExport GetExpressionParam(mv _far *mV, short code, short param, LPSTR strBuf, short maxLen)
{
#if !defined(RUN_ONLY)
	short		strID;

	// Finds event in array
	LPEVENTINFOS2 eiPtr=GetEventInformations((LPEVENTINFOS2)expressionsInfos, code);

	// If a special string is to be returned
	strID = EVINFO2_PARAMTITLE(eiPtr, param);
	if ( strID != 0 )
		LoadString(hInstLib, strID, strBuf, maxLen);
	else
		*strBuf=0;
#endif // !defined(RUN_ONLY)
}









// ----------------------------------------------------------
// Custom Parameters
// ----------------------------------------------------------

// --------------------
// InitParameter
// --------------------
void WINAPI InitParameter(mv _far *mV, short code, paramExt* pExt)
{
#if !defined(RUN_ONLY)

	switch(code)
	{
	case PARAM_EASING:
		{
			EasingParam es;
			es.version = 0;
			es.method = 0;
			es.firstFunction = 0;
			es.secondFunction = 0;

			pExt->pextSize = sizeof(paramExt) + sizeof(EasingParam);
			memcpy(pExt->pextData, &es, sizeof(EasingParam));
		}
		break;
	case PARAM_TIMEMODE:
		{
			TimeModeParam tp;
			tp.type = 0;

			pExt->pextSize = sizeof(paramExt) + sizeof(TimeModeParam);
			memcpy(pExt->pextData, &tp, sizeof(TimeModeParam));
		}
	}
#endif // !defined(RUN_ONLY)
}


// --------------------
// EditParameter
// --------------------
void WINAPI EditParameter(mv _far *mV, short code, paramExt* pExt)
{
#if !defined(RUN_ONLY)

	switch(code)
	{
	case PARAM_EASING:
	{
		EasingParam* easingParam = (EasingParam*)pExt->pextData;
		easingParam->mV = mV;
		DialogBoxParam(hInstLib, MAKEINTRESOURCE(DB_EASING), mV->mvHEditWin, easingProc, (LPARAM)pExt);
	}
		break;
	case PARAM_TIMEMODE:
		DialogBoxParam(hInstLib, MAKEINTRESOURCE(DB_TIMEMODE), mV->mvHEditWin, timeModeProc, (LPARAM)pExt);
		break;
	}

#endif // !defined(RUN_ONLY)
}



// --------------------
// GetParameterString
// --------------------
void WINAPI GetParameterString(mv _far *mV, short code, paramExt* pExt, LPSTR pDest, short size)
{
#if !defined(RUN_ONLY)

	switch(code)
	{
	case PARAM_EASING:
		{
			EasingParam* param = (EasingParam*)pExt->pextData;
			if(param->method >= 0 && param->method <= 1)
				wsprintf(pDest, "%s %s", GetFunctionName(param->firstFunction), GetMethodName(param->method));
			else if(param->method > 1 && param->method <=3)
				wsprintf(pDest, "%s/%s %s", GetFunctionName(param->firstFunction), GetFunctionName(param->secondFunction), GetMethodName(param->method));
			else
				wsprintf(pDest, "Error, unknown method number: %i", param->method);
		}
		break;
	case PARAM_TIMEMODE:
		{
			TimeModeParam* param = (TimeModeParam*)pExt->pextData;
			if(param->type == 0)
				wsprintf(pDest, "milliseconds");
			else
				wsprintf(pDest, "event loops");
		}
		break;
	}




#endif // !defined(RUN_ONLY)
}

