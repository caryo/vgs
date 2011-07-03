
#include "wx/wx.h"
#include "wx/grid.h"

class MyGameApp : public wxApp {

    virtual bool OnInit();
};

class MyGameFrame : public wxFrame {

public:
    MyGameFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

enum {
    ID_Quit = 1,
    ID_About,
};

BEGIN_EVENT_TABLE(MyGameFrame, wxFrame)
    EVT_MENU(ID_Quit, MyGameFrame::OnQuit)
    EVT_MENU(ID_About, MyGameFrame::OnAbout)
END_EVENT_TABLE()

IMPLEMENT_APP(MyGameApp)

bool MyGameApp::OnInit() {

    wxLog *logger = new wxLogStream(&std::cout);
    wxLog::SetActiveTarget(logger);

    wxLogMessage(_("wxThread::GetCPUCount = %d\n"), wxThread::GetCPUCount());

    wxLogMessage(_("MyGameApp::OnInit - begin\n"));
    MyGameFrame *frame = new MyGameFrame( _("Baseball Simulation"),
        wxPoint(50,50), wxSize(450,340) );

    frame->Show(true);
    SetTopWindow(frame);
    wxLogMessage(_("MyGameApp::OnInit - end\n"));
    return true;
}

MyGameFrame::MyGameFrame( const wxString& title, const wxPoint& pos,
    const wxSize& size) : wxFrame( NULL, -1, title, pos, size ) {

    wxLogMessage(_("MyGameFrame::constructor - begin\n"));

    wxMenu *menuFile = new wxMenu;

    menuFile->Append( ID_About, _("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _("E&xit") );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _("&File") );

    SetMenuBar( menuBar );

    CreateStatusBar();
    SetStatusText( _("Welcome to Baseball Simulation!") );

    wxGrid *grid = new wxGrid( this, -1, wxPoint(100,100),
                               wxSize(200,150));
    grid->EnableEditing(false);
    grid->EnableCellEditControl(false);
    grid->EnableDragGridSize(false);
    grid->EnableGridLines(false);
    grid->SetColLabelSize(0);
    grid->SetRowLabelSize(0);
    //grid->HideColLabels();
    //grid->HideRowLabels();
    grid->CreateGrid( 7, 13 );
    //grid->SetRowSize( 0, 40 );
    //grid->SetRowSize( 1, 40 );
    //grid->SetRowSize( 2, 40 );
    grid->SetColSize( 0, 120 );
    grid->SetColSize( 1, 15 );
    grid->SetColSize( 2, 15 );
    grid->SetColSize( 3, 25 );
    grid->SetColSize( 4, 15 );
    grid->SetColSize( 5, 15 );
    grid->SetColSize( 6, 25 );
    grid->SetColSize( 7, 15 );
    grid->SetColSize( 8, 15 );
    grid->SetColSize( 9, 25 );
    grid->SetColSize( 10, 30 );
    grid->SetColSize( 11, 30 );
    grid->SetColSize( 12, 30 );
    grid->SetCellValue( 0,0, _("TEAM") );
    grid->SetCellValue( 1,0, _("Visitor") );
    grid->SetCellValue( 2,0, _("Home") );
    grid->SetCellValue( 0,1, _("1") );
    grid->SetCellValue( 0,2, _("2") );
    grid->SetCellValue( 0,3, _("3") );
    grid->SetCellValue( 0,4, _("4") );
    grid->SetCellValue( 0,5, _("5") );
    grid->SetCellValue( 0,6, _("6") );
    grid->SetCellValue( 0,7, _("7") );
    grid->SetCellValue( 0,8, _("8") );
    grid->SetCellValue( 0,9, _("9") );
    grid->SetCellValue( 0,10, _("R") );
    grid->SetCellValue( 0,11, _("H") );
    grid->SetCellValue( 0,12, _("E") );
    grid->SetCellValue( 1,1, _("0") );
    grid->SetCellValue( 1,2, _("0") );
    grid->SetCellValue( 1,3, _("0") );
    grid->SetCellValue( 1,4, _("0") );
    grid->SetCellValue( 1,5, _("0") );
    grid->SetCellValue( 1,6, _("0") );
    grid->SetCellValue( 1,7, _("0") );
    grid->SetCellValue( 1,8, _("0") );
    grid->SetCellValue( 1,9, _("0") );
    grid->SetCellValue( 1,10, _("0") );
    grid->SetCellValue( 1,11, _("0") );
    grid->SetCellValue( 1,12, _("0") );
    grid->SetCellValue( 2,1, _("0") );
    grid->SetCellValue( 2,2, _("0") );
    grid->SetCellValue( 2,3, _("0") );
    grid->SetCellValue( 2,4, _("0") );
    grid->SetCellValue( 2,5, _("0") );
    grid->SetCellValue( 2,6, _("0") );
    grid->SetCellValue( 2,7, _("0") );
    grid->SetCellValue( 2,8, _("0") );
    grid->SetCellValue( 2,9, _("0") );
    grid->SetCellValue( 2,10, _("0") );
    grid->SetCellValue( 2,11, _("0") );
    grid->SetCellValue( 2,12, _("0") );

    grid->SetCellValue( 4,0, _("Balls") );
    grid->SetCellValue( 5,0, _("Strikes") );
    grid->SetCellValue( 6,0, _("Outs") );

    grid->SetCellValue( 4, 1, _("0") );
    grid->SetCellValue( 5, 1, _("0") );
    grid->SetCellValue( 6, 1, _("0") );

    grid->SetCellValue( 4, 11, _("*o") );
    grid->SetCellValue( 5, 10, _("*o") );
    grid->SetCellValue( 5, 12, _("*o") );
    grid->SetCellValue( 6, 11, _(" v") );

    //grid->SetCellValue( 3,3, _("green on grey"));
    //grid->SetCellTextColour(3,3, *wxGREEN);
    //grid->SetCellBackgroundColour(3,3, *wxLIGHT_GREY);
    //grid->SetColFormatFloat(5,6,2);
    //grid->SetCellValue(0,6,_("3.1415"));

    wxLogMessage(_("MyGameFrame::constructor - end\n"));
}

void MyGameFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {

    wxLogMessage(_("MyGameFrame::OnQuit - begin\n"));
    Close(TRUE);
    wxLogMessage(_("MyGameFrame::OnQuit - end\n"));
}

void MyGameFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {

    wxLogMessage(_("MyGameFrame::OnAbout - begin\n"));
    wxMessageBox( _("This is a Baseball Simulation application"),
        _("About Baseball Simulation"),
        wxOK | wxICON_INFORMATION, this);
    wxLogMessage(_("MyGameFrame::OnAbout - end\n"));
}

