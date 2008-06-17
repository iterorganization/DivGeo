/* Help dialogs */

#ifndef _x_help_h
#define _x_help_h

typedef Widget HelpDlg;

Widget OpenHelpDlg(Widget wParent,String string);

void DisplayHelpText(View w,String text,size_t offset);
void DisplayHelpTitle(View w,String title);
void DisplayHelpTopic(View w,String topic);
void DisplayHelpIndex(View w);


#endif
