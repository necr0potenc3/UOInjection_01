//---------------------------------------------------------------------------

#ifndef SDLLUnitFH
#define SDLLUnitFH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TSkillsForm : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TPanel *Panel2;
    TButton *Button1;
    TButton *Button2;
    TButton *Button3;
    TButton *Button4;
    TStringGrid *Grid;
    TTimer *Timer1;
    TRadioGroup *RadioGroup1;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall Button3Click(TObject *Sender);
    void __fastcall Button4Click(TObject *Sender);
    void __fastcall RadioGroup1Click(TObject *Sender);
    void __fastcall GridDblClick(TObject *Sender);
private:	// User declarations
    TListBox *List;
public:		// User declarations
    __fastcall TSkillsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSkillsForm *SkillsForm;
//---------------------------------------------------------------------------
#endif
