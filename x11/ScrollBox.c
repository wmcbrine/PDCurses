/*
 * Copyright 1989 O'Reilly and Associates, Inc.

     The X Consortium, and any party obtaining a copy of these files from
     the X Consortium, directly or indirectly, is granted, free of charge, a
     full and unrestricted irrevocable, world-wide, paid up, royalty-free,
     nonexclusive right and license to deal in this software and
     documentation files (the "Software"), including without limitation the
     rights to use, copy, modify, merge, publish, distribute, sublicense,
     and/or sell copies of the Software, and to permit persons who receive
     copies from any such party to do so.  This license includes without
     limitation a license to do the foregoing actions under any patents of
     the party supplying this software to the X Consortium.
 */

/* 
 * ScrollBox.c - scrollBox composite widget
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include "x11/ScrollBoxP.h"

#include <stdio.h>

#define INITIAL_WIDTH 300
#define INITIAL_HEIGHT 300

/****************************************************************
 *
 * scrollBox Resources
 *
 ****************************************************************/

static XtResource resources[] = 
{
   { XtNhSpace, XtCHSpace, XtRDimension, sizeof(Dimension),
      XtOffset(ScrollBoxWidget, scrollBox.h_space), XtRImmediate, (XtPointer)4},
   { XtNvSpace, XtCVSpace, XtRDimension, sizeof(Dimension),
      XtOffset(ScrollBoxWidget, scrollBox.v_space), XtRImmediate, (XtPointer)4},
   { XtNheightInc, XtCHeightInc, XtRDimension, sizeof(Dimension),
      XtOffset(ScrollBoxWidget, scrollBox.increment_height), XtRImmediate, (XtPointer)13},
   { XtNwidthInc, XtCWidthInc, XtRDimension, sizeof(Dimension),
      XtOffset(ScrollBoxWidget, scrollBox.increment_width), XtRImmediate, (XtPointer)7},
};

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

static void Initialize();
static void Resize();
static Boolean SetValues();
static void ChangeManaged();
static XtGeometryResult QueryGeometry(), GeometryManager();
static void RefigureLocations();

ScrollBoxClassRec scrollBoxClassRec = {
  {
/* core_class fields      */
    /* superclass         */    (WidgetClass) &compositeClassRec,
    /* class_name         */    "scrollBox",
    /* widget_size        */    sizeof(ScrollBoxRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */  NULL,
    /* class_inited       */  FALSE,
    /* initialize         */    Initialize,
    /* initialize_hook    */  NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions     */  0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */  TRUE,
    /* compress_exposure  */  TRUE,
    /* compress_enterleave*/  TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    Resize,
    /* expose             */    NULL,
    /* set_values         */    SetValues,
    /* set_values_hook    */  NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */  NULL,
    /* accept_focus       */    NULL,
    /* version            */  XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */  QueryGeometry,
    /* display_accelerator*/  XtInheritDisplayAccelerator,
    /* extension          */  NULL
  },{
/* composite_class fields */
    /* geometry_manager   */    GeometryManager,
    /* change_managed     */    ChangeManaged,
    /* insert_child    */  XtInheritInsertChild,
    /* delete_child    */  XtInheritDeleteChild,
    /* extension          */  NULL
  },{
/* scrollBox class fields */
    /* empty        */  0,
  }
};

WidgetClass scrollBoxWidgetClass = (WidgetClass)&scrollBoxClassRec;


/****************************************************************
 *
 * Private Routines
 *
 ****************************************************************/

/*
 * Do a layout, either actually assigning positions, or just 
 * calculating size.
 */

/* ARGSUSED */
static void DoLayout(w, doit)
Widget w;
Boolean doit;
{
   ScrollBoxWidget sbw = (ScrollBoxWidget) w;
   Widget wmain, vscroll, hscroll;
   Widget child;
   Dimension mw, mh;   /* main window */
   Dimension vh;   /* vertical scrollbar length (height) */
   Dimension hw;   /* horizontal scrollbar length (width) */
   Position vx;
   Position hy;
#if 1
   Dimension th;
   Dimension tw;
#endif
   int i;

   if (sbw->composite.num_children != 3)
      XtAppError(XtWidgetToApplicationContext((Widget)sbw),
            "ScrollBox: must manage exactly three widgets.");

   for (i = 0; i < sbw->composite.num_children; i++) 
   {
      child = sbw->composite.children[i];
      if (!XtIsManaged(child)) 
      {
         XtAppError(XtWidgetToApplicationContext((Widget)sbw),
            "ScrollBox: all three widgets must be managed.");
      }
   }

   /*
    * Child one is the main window, two is the vertical scrollbar,
    * and three is the horizontal scrollbar.
    */
   wmain = sbw->composite.children[0];
   vscroll = sbw->composite.children[1];
   hscroll = sbw->composite.children[2];

   /* 
    * Size all three widgets so that space is fully utilized.
    */
#if 1
   mw = sbw->core.width - (2 * sbw->scrollBox.h_space) -
        vscroll->core.width - (2 * vscroll->core.border_width) -
        (2 * wmain->core.border_width);


   mh = sbw->core.height - (2 * sbw->scrollBox.v_space) -
        hscroll->core.height - (2 * hscroll->core.border_width) -
        (2 * wmain->core.border_width);

   /*
    * Force the main window to be sized to the appropriate increments
    */
   mw = (mw/sbw->scrollBox.increment_width)*sbw->scrollBox.increment_width;
   mh = ((mh/sbw->scrollBox.increment_height)*sbw->scrollBox.increment_height)+sbw->scrollBox.increment_height;

   vx = wmain->core.x + mw + sbw->scrollBox.h_space + wmain->core.border_width + vscroll->core.border_width; 

   hy = wmain->core.y + mh + sbw->scrollBox.v_space + wmain->core.border_width + hscroll->core.border_width; 

   vh = mh;   /* scrollbars are always same length as main window */
   hw = mw;
/* fprintf(stderr,"%s %d: mw %d mh %d vx %d hy %d hinc %d winc %d hspace %d\n",__FILE__,__LINE__,mw,mh,vx,hy,sbw->scrollBox.increment_height,sbw->scrollBox.increment_width,sbw->scrollBox.h_space); */

   if (doit) {
      XtResizeWidget(wmain, mw, mh, 1);
#else
   tw = wmain->core.width + (2 * sbw->scrollBox.h_space) +
        vscroll->core.width + (2 * vscroll->core.border_width) +
        (2 * wmain->core.border_width);

   th = wmain->core.height + (2 * sbw->scrollBox.v_space) +
        hscroll->core.height - (2 * hscroll->core.border_width) +
        (2 * wmain->core.border_width);

   hw = mw = wmain->core.width;
   vh = mh = wmain->core.height;

   vx = wmain->core.x + mw + sbw->scrollBox.h_space + wmain->core.border_width + vscroll->core.border_width; 
   hy = wmain->core.y + mh + sbw->scrollBox.v_space + wmain->core.border_width + hscroll->core.border_width; 

   if (doit) {
      XtResizeWidget(w, tw, th, 1);
#endif

      XtResizeWidget(vscroll, vscroll->core.width, vh, 1);
      XtMoveWidget(vscroll, vx, vscroll->core.y);

      XtResizeWidget(hscroll, hw, hscroll->core.height, 1);
      XtMoveWidget(hscroll, hscroll->core.x, hy);
   }
}

/* ARGSUSED */
static XtGeometryResult GeometryManager(w, request, reply)
    Widget w;
    XtWidgetGeometry *request;
    XtWidgetGeometry *reply;    /* RETURN */
{
   XtWidgetGeometry allowed;

   if (request->request_mode & ~(XtCWQueryOnly | CWWidth | CWHeight))
      return XtGeometryNo;

   if (request->request_mode & CWWidth)
      allowed.width = request->width;
   else
      allowed.width = w->core.width;

   if (request->request_mode & CWHeight)
      allowed.height = request->height;
   else
      allowed.height = w->core.height;

   if (allowed.width == w->core.width && allowed.height == w->core.height)
      return XtGeometryNo;

    if (!(request->request_mode & XtCWQueryOnly)) {
      RefigureLocations(w);
    }
    return XtGeometryYes;
}

static void RefigureLocations(sbw)
    ScrollBoxWidget sbw;
{
   DoLayout(sbw, False);
}


/*
 * Calculate preferred size.  We can't just use the current sizes
 * of the children, because that calculation would always end up with 
 * our current size.  Could query each child, and use that size to 
 * recalculate a size for us, then if it ends up being larger
 * than width and height passed in, accept bounding box.  
 * However, we know our children and they don't have any particular
 * preferred geometry, except the bigger the better.
 * Therefore, if the parent suggested a size, we'll take it.
 */

/* ARGSUSED */
static XtGeometryResult QueryGeometry(w, request, reply_return)
Widget w;
XtWidgetGeometry *request, *reply_return;
{
   XtGeometryResult result=XtGeometryNo;

   request->request_mode &= CWWidth | CWHeight;

   if (request->request_mode == 0)
   /* parent isn't going to change w or h, so nothing to re-compute */
      return XtGeometryYes;

    /* if proposed size is large enough, accept it.  Otherwise,
     * suggest our arbitrary initial size. */

   if (request->request_mode & CWHeight) 
   {
      if (request->height < INITIAL_HEIGHT) 
      {
         result = XtGeometryAlmost;
         reply_return->height = INITIAL_HEIGHT;
         reply_return->request_mode &= CWHeight;
      }
      else
         result = XtGeometryYes;
   }

   if (request->request_mode & CWWidth) 
   {
      if (request->width < INITIAL_WIDTH) 
      {
         result = XtGeometryAlmost;
         reply_return->width = INITIAL_WIDTH;
         reply_return->request_mode &= CWWidth;
      }
      else
         result = XtGeometryYes;
   }

   return(result);
}

/*
 * Actually layout the scrollBox
 */

static void Resize(w)
Widget w;
{
   ScrollBoxWidget sbw = (ScrollBoxWidget) w;
   DoLayout(sbw, True);
}

static void ChangeManaged(w)
ScrollBoxWidget w;
{
   ScrollBoxWidget sbw = (ScrollBoxWidget) w;
   DoLayout(sbw, True);
}

/* ARGSUSED */
static void Initialize(request, new)
    Widget request, new;
{
   ScrollBoxWidget newsbw = (ScrollBoxWidget)new;

   if (newsbw->core.width == 0)
      newsbw->core.width = INITIAL_WIDTH;

   if (newsbw->core.height == 0)
      newsbw->core.height = INITIAL_HEIGHT;

} /* Initialize */

/* ARGSUSED */
static Boolean SetValues(current, request, new, args, num_args)
Widget current, request, new;
ArgList args;
Cardinal *num_args;
{
   ScrollBoxWidget sbwcurrent = (ScrollBoxWidget) current;
   ScrollBoxWidget sbwnew = (ScrollBoxWidget) new;
   /* need to relayout if h_space or v_space change */
   if ((sbwnew->scrollBox.h_space != sbwcurrent->scrollBox.h_space) ||
                   (sbwnew->scrollBox.v_space != sbwcurrent->scrollBox.v_space))
      DoLayout(sbwnew, True);

   return False;
}
