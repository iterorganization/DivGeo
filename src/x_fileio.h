/* File I/O dialogs */

#ifndef _x_fileio_h
#define _x_fileio_h

Widget OpenEquilDlg(View w);
Widget OpenTemplateDlg(View w);
Widget OpenSonnetDlg(View w);
Widget OpenFileOpenDlg(View w);
Widget OpenFileSaveDlg(View w,int bAtExit);
Widget OpenFilePrintDlg(View w);
Widget OpenOutputDlg(View w);
Widget OpenExportMeshDlg(View w,int bAtExit);
Widget OpenExportElemsAsTemplateDlg(View w);

void CloseXmView(View w,int bAsk);

void DwNotifyRecentFiles(Widget wg,View w,int evt,void* obj,void* d);

#endif
