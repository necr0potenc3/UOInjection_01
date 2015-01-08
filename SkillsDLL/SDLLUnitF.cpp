//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SDLLUnitF.h"
#include "..\include\api.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

static int Skills[50];
static int Order[50];
bool SkillsFilled=false;
char *SkillsSet[]={
	"Alchemy",
	"Anatomy",
	"Animal Lore",
	"Item ID",
	"Arms Lore",
	"Parrying",
	"Begging ",
	"Blacksmithing",
	"Bowcraft",
	"Peacemaking",
	"Camping",
	"Carpentry",
	"Cartography",
	"Cooking",
	"Detect Hidden",
	"Enticement",
	"Evaluate Intelligence",
	"Healing",
	"Fishing ",
	"Forensic Evaluation",
	"Herding",
	"Hiding",
	"Provocation",
	"Inscription",
	"Lockpicking",
	"Magery",
	"Magic Resistance",
	"Tactics",
	"Snooping",
	"Musicianship",
	"Poisoning",
	"Archery",
	"Spirit Speak",
	"Stealing",
	"Tailoring",
	"Animal Taming",
	"Taste Identification",
	"Tinkering",
	"Tracking",
	"Veterinary",
	"Swordsmanship",
	"Mace Fighting",
	"Fencing",
	"Wrestling",
	"Lumberjacking",
	"Mining",
	"Meditation",
	"Stealth",
	"Remove Trap",
	"Necromancy"
};

TSkillsForm *SkillsForm=0;
//---------------------------------------------------------------------------
__fastcall TSkillsForm::TSkillsForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSkillsForm::FormCreate(TObject *Sender)
{
    Grid->Cells[0][0]="Skill name";
    Grid->Cells[1][0]="Value";
    Grid->Cells[2][0]="Base";
    Grid->Cells[3][0]="Change";
    Grid->Cells[4][0]="Lock";
    for(int i=0; i<50; i++)
    {
        Grid->Cells[0][i+1]=Skills[i];
    }
    for(int i=0; i<50; i++)
        Order[i]=i;
}
//---------------------------------------------------------------------------
void __fastcall TSkillsForm::Timer1Timer(TObject *Sender)
{
  try {
    RadioGroup1Click(Sender);
    char *Lk[]={"up","down","lock"};
    int S=0;
    for(int i=0; i<50; i++)
    {
        Grid->Cells[0][i+1]=SkillsSet[Order[i]];
        Grid->Cells[1][i+1]=GetSkill(Order[i])/10.0;
        Grid->Cells[2][i+1]=GetSkillBase(Order[i])/10.0;
        int t=GetSkillLock(Order[i]);
        Grid->Cells[4][i+1]=Lk[t%3];
        if(!SkillsFilled)
            Skills[Order[i]]=GetSkillBase(Order[i]);
        if(Skills[Order[i]]!=GetSkillBase(Order[i]))
        {
            Grid->Cells[3][i+1]=(GetSkillBase(Order[i])-Skills[Order[i]])/10.0;
            if(Grid->Cells[3][i+1][1]!='-')
                Grid->Cells[3][i+1]="+"+Grid->Cells[3][i+1];
        } else
            Grid->Cells[3][i+1]="";
        S+=Skills[Order[i]];
    }
    SkillsFilled = S>0;
  } catch (...) {}
}
//---------------------------------------------------------------------------
void __fastcall TSkillsForm::Button1Click(TObject *Sender)
{
    SkillsFilled = false;
}
//---------------------------------------------------------------------------
void __fastcall TSkillsForm::Button2Click(TObject *Sender)
{
    for(int i=0; i<50; i++)
        SetSkillLock(i,0);
}
//---------------------------------------------------------------------------
void __fastcall TSkillsForm::Button3Click(TObject *Sender)
{
    for(int i=0; i<50; i++)
        SetSkillLock(i,1);
}
//---------------------------------------------------------------------------
void __fastcall TSkillsForm::Button4Click(TObject *Sender)
{
    for(int i=0; i<50; i++)
        SetSkillLock(i,2);
}
//---------------------------------------------------------------------------
void __fastcall TSkillsForm::RadioGroup1Click(TObject *Sender)
{
    for(int i=0; i<50; i++)
        Order[i]=i;

    switch(RadioGroup1->ItemIndex)
    {
    case 1:
    {
        for(int i=0; i<50; i++)
        {
            for(int j=i+1; j<50; j++)
            {
                if(GetSkill(Order[j])>GetSkill(Order[i]))
                {
                    int t=Order[i]; Order[i]=Order[j]; Order[j]=t;
                }

            }
        }
        break;
    }
    case 2:
    {
        for(int i=0; i<50; i++)
        {
            for(int j=i+1; j<50; j++)
            {
                if(GetSkillBase(Order[j])>GetSkillBase(Order[i]))
                {
                    int t=Order[i]; Order[i]=Order[j]; Order[j]=t;
                }

            }
        }
        break;
    }
    case 3:
    {
        for(int i=0; i<50; i++)
        {
            for(int j=i+1; j<50; j++)
            {
                int t1=GetSkillBase(Order[j])-Skills[Order[j]];
                int t2=GetSkillBase(Order[i])-Skills[Order[i]];
                if(t1==0)
                    t1=-999;
                if(t2==0)
                    t2=-999;
                if(t1>t2)
                {
                    int t=Order[i]; Order[i]=Order[j]; Order[j]=t;
                }

            }
        }
        break;
    }
    };
}
//---------------------------------------------------------------------------
void __fastcall TSkillsForm::GridDblClick(TObject *Sender)
{
    if(Grid->Col==4)
    {
        int Sk=Order[Grid->Row-1];
//        for(int i=0; i<50; i++)
//            if(Order[i]==Grid->Row-1)
//                Sk=i;
        SetSkillLock(Sk,(GetSkillLock(Sk)+1)%3);
    } else
    {
        RadioGroup1->ItemIndex=Grid->Col;
        RadioGroup1Click(Sender);
    }
}
//---------------------------------------------------------------------------

