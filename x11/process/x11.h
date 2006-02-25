void dummy_function(void);
void get_GC(Display *, Window, GC *, XFontStruct *, int, int, bool);
void makeXY(int, int, int, int, int *, int *);
int get_colors(void);
void start_event_handler(void);

int XCursesTransformLine(int, int, chtype, int, int, int, char *);
int XCursesDisplayText(const chtype *, int, int, int, bool);
void XCursesDisplayScreen(bool);
void XCursesDisplayCursor(int, int, int, int);
void XCursesStructureNotify(Widget, XtPointer, XEvent *, Boolean *);
void XCursesEnterLeaveWindow(Widget, XtPointer, XEvent *, Boolean *);
void XCursesPasteSelection(Widget, XButtonEvent *);
void XCursesHandleString(Widget, XEvent *, String *, Cardinal *);
void XCursesKeyPress(Widget, XEvent *, String *, Cardinal *);
void XCursesModifierPress(Widget, XEvent *, String *, Cardinal *);
void XCursesButton(Widget, XEvent *, String *, Cardinal *);
Boolean XCursesConvertProc(Widget, Atom *, Atom *, Atom *, XtPointer *,
			   unsigned long *, int *);
void XCursesLoseOwnership(Widget, Atom *);
void XCursesRequestorCallbackForPaste(Widget, XtPointer, Atom *, Atom *,
				      XtPointer, unsigned long *, int *);
void XCursesRequestorCallbackForGetSelection(Widget, XtPointer, Atom *, Atom *,
					     XtPointer, unsigned long *, int *);
RETSIGTYPE XCursesSignalHandler(int);
void XCursesExitXCursesProcess(int, int, char *);

void SelectionOff(void);
void SelectionOn(int, int);
void SelectionExtend(int, int);
void SelectionSet(void);

int write_socket(int, char *, int);
int read_socket(int, char *, int);

int XCursesSetupX(char *display_name, int argc, char *argv[]);
RETSIGTYPE XCursesSigwinchHandler(int signo);

#ifdef _HPUX_SOURCE
# define FD_SET_CAST int *
#else
# define FD_SET_CAST fd_set *
#endif

extern fd_set readfds;
extern fd_set writefds;
extern struct timeval socket_timeout;

extern unsigned char *Xcurscr;
extern int XCursesProcess;
extern int shmidSP;
extern int shmid_Xcurscr;
extern int shmkeySP;
extern int shmkey_Xcurscr;
extern int otherpid;
extern int XCursesLINES;
extern int XCursesCOLS;
extern unsigned char *atrtab;
extern int display_sock;
extern int key_sock;
extern int display_sockets[2];
extern int key_sockets[2];
extern int exit_sock;
