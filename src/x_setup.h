/* Setup dialogs */

#ifndef _x_setup_h
#define _x_setup_h

void CbEditVarSetDefs(Widget wg,XtPointer pView,void* pcbs);
void CbSaveConfig(Widget wg,XtPointer pView,XtPointer pcbs);

Widget OpenOutputModeDlg(View w);
Widget OpenMeshOptionsDlg(View w);

#endif
