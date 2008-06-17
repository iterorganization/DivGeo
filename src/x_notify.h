/* Notification routines */

#ifndef _x_notify_h
#define _x_notify_h

void DwNotifyUndoButton(Widget wg,View w,int evt,void* obj,void* d);
void DwNotifyIfExists(Widget wg,View w,int evt,void* obj,void* d);
void DwNotifyIfNotExists(Widget wg,View w,int evt,void* obj,void* d);
void DwNotifyShowFlags(Widget wg,View w,int evt,void* obj,void* d);
void DwUnmap(Widget wg,View w,int evt,void* obj,void* d);
void DwDestroy(Widget wg,View w,int evt,void* obj,void* d);

#endif
