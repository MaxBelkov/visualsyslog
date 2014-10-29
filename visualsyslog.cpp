//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("sourcecommon\messageform.cpp", MessageForm);
USEFORM("source\main.cpp", MainForm);
USEFORM("source\setup.cpp", SetupForm);
USEFORM("source\aboutbox.cpp", AboutBoxForm);
USEFORM("source\messmatchframe.cpp", MessMatchFr); /* TFrame: File Type */
USEFORM("source\messstyleframe.cpp", MessStyleFr); /* TFrame: File Type */
USEFORM("source\formhl.cpp", HighlightForm);
//---------------------------------------------------------------------------
extern bool bHideToTray;
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR ParamStr, int)
{
    bHideToTray = SameText(ParamStr, "tray");
    try
    {
         Application->Initialize();
         SetApplicationMainFormOnTaskBar(Application, true);
         Application->Title = "Visual Syslog Server";
         Application->CreateForm(__classid(TMainForm), &MainForm);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    catch (...)
    {
         try
         {
             throw Exception("");
         }
         catch (Exception &exception)
         {
             Application->ShowException(&exception);
         }
    }
    return 0;
}
//---------------------------------------------------------------------------
