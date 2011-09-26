// Object identifier "Esng" as in 'Easing'

#define IDENTIFIER	MAKEID(E,s,n,G)

#include	<vector>
using namespace std;


// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define	CND_ANYOBJECTSTOPPED			0
#define CND_SPECIFICOBJECTSTOPPED		1
#define CND_ISOBJECTMOVING              2

#define	CND_LAST						3

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define ACT_MOVEOBJ						0
#define ACT_STOPOBJECT					1
#define ACT_STOPALLOBJECTS				2
#define ACT_REVERSEOBJECT				3
#define ACT_SETAMPLITUDE                4
#define ACT_SETOVERSHOOT                5
#define ACT_SETPERIOD                   6

#define ACT_SETOBJECTAMPLITUDE          7
#define ACT_SETOBJECTOVERSHOOT          8
#define ACT_SETOBJECTPERIOD             9

#define	ACT_LAST						10

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------
#define	EXP_GETNUMCONTROLLED			0
#define EXP_GETSTOPPEDFIXED				1
#define EXP_CALCULATE_EASEIN			2
#define EXP_CALCULATE_EASEOUT			3
#define EXP_CALCULATE_EASEINOUT			4
#define EXP_CALCULATE_EASEOUTIN			5
#define EXP_CALCULATEBETWEEN_EASEIN		6
#define EXP_CALCULATEBETWEEN_EASEOUT	7
#define EXP_CALCULATEBETWEEN_EASEINOUT	8
#define EXP_CALCULATEBETWEEN_EASEOUTIN	9
#define EXP_GETAMPLITUDE                10
#define EXP_GETOVERSHOOT                11
#define EXP_GETPERIOD                   12
#define EXP_GETDEFAULTAMPLITUDE         13
#define EXP_GETDEFAULTOVERSHOOT         14
#define EXP_GETDEFAULTPERIOD            15

#define	EXP_LAST						16

// -------------------------------
// DEFINITION OF CUSTOM PARAMETERS
// -------------------------------
#define PARAM_EASING	(PARAM_EXTBASE)
#define PARAM_TIMEMODE	(PARAM_EXTBASE+1)

// -------------------------------
// DEFINITION OF OTHER CONSTANTS
// -------------------------------
#define	EASEIN		0
#define EASEOUT		1
#define EASEINOUT	2
#define	EASEOUTIN	3

//Custom action parameter structs
struct EasingParam
{
	unsigned char version;
	unsigned char method;
	unsigned char firstFunction;
	unsigned char secondFunction;
	mv * mV;
};

struct TimeModeParam
{
	unsigned char type;
};

//Easing parameter value struct
struct EaseVars
{
	float overshoot;
	float amplitude;
	float period;
};

//Runtime structures:
struct MoveStruct
{
	int		fixedValue;
	int		startX;
	int		startY;
	int		destX;
	int		destY;

	EaseVars vars;

	unsigned char	easingMode;
	unsigned char	functionA;
	unsigned char	functionB;

	unsigned char	timeMode;
	__int64			starttime;
	int				timespan;
	int				eventloop_step;
};



double linear(double step, EaseVars vars);
double quad(double step, EaseVars vars);
double cubic(double step, EaseVars vars);
double quart(double step, EaseVars vars);
double quint(double step, EaseVars vars);
double sine(double step, EaseVars vars);
double expo(double step, EaseVars vars);
double circ(double step, EaseVars vars);
double back(double step, EaseVars vars);
double elastic(double step, EaseVars vars);
double bounce(double step, EaseVars vars);
double doFunction(int number, double step, EaseVars vars);
double easeIn(int function, double step, EaseVars vars);
double easeOut(int function, double step, EaseVars vars);
double easeInOut(int functionA, int functionB, double step, EaseVars vars);
double easeOutIn(int functionA, int functionB, double step, EaseVars vars);
double calculateEasingValue(int mode, int functionA, int functionB, double step, EaseVars vars);



__int64 CurrentTime();


// ---------------------
// OBJECT DATA STRUCTURE 
// ---------------------
// Used at edit time and saved in the MFA/CCN/EXE files

typedef struct tagEDATA_V1
{
	// Header - required
	extHeader		eHeader;

	EaseVars		defaultVars;

	// Object's data
//	short			swidth;
//	short			sheight;

} EDITDATA;
typedef EDITDATA *			LPEDATA;

// Object versions
#define	KCX_CURRENT_VERSION			1

// --------------------------------
// RUNNING OBJECT DATA STRUCTURE
// --------------------------------
// Used at runtime. Initialize it in the CreateRunObject function.
// Free any allocated memory or object in the DestroyRunObject function.
//
// Note: if you store C++ objects in this structure and do not store 
// them as pointers, you must call yourself their constructor in the
// CreateRunObject function and their destructor in the DestroyRunObject
// function. As the RUNDATA structure is a simple C structure and not a C++ object.

typedef struct tagRDATA
{
	// Main header - required
	headerObject	rHo;					// Header

	vector <MoveStruct>		controlled;
	MoveStruct				currentMoved;
	EaseVars				easingVars;

	vector <MoveStruct>		deleted;

	bool isHwa;
	bool isUnicode;

	// Optional headers - depend on the OEFLAGS value, see documentation and examples for more info
//	rCom			rc;				// Common structure for movements & animations
//	rMvt			rm;				// Movements
//	rSpr			rs;				// Sprite (displayable objects)
//	rVal			rv;				// Alterable values

	// Object's runtime data

} RUNDATA;
typedef	RUNDATA	*			LPRDATA;

// Size when editing the object under level editor
// -----------------------------------------------
#define	MAX_EDITSIZE			sizeof(EDITDATA)

// Default flags - see documentation for more info
// -------------
#define	OEFLAGS      			0
#define	OEPREFS      			0


// If to handle message, specify the priority of the handling procedure
// 0= low, 255= very high. You should use 100 as normal.                                                
// --------------------------------------------------------------------
#define	WINDOWPROC_PRIORITY		100

#define EF_ISHWA				112
#define EF_ISUNICODE			113
