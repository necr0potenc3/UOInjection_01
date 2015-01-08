object SkillsForm: TSkillsForm
  Left = 192
  Top = 107
  BorderIcons = []
  BorderStyle = bsNone
  ClientHeight = 174
  ClientWidth = 400
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 268
    Height = 174
    Align = alClient
    BevelOuter = bvNone
    BorderStyle = bsSingle
    TabOrder = 0
    object Grid: TStringGrid
      Left = 0
      Top = 0
      Width = 264
      Height = 170
      Align = alClient
      BorderStyle = bsNone
      DefaultColWidth = 40
      DefaultRowHeight = 16
      FixedCols = 0
      RowCount = 51
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goColSizing, goThumbTracking]
      TabOrder = 0
      OnDblClick = GridDblClick
      ColWidths = (
        83
        40
        40
        40
        40)
    end
  end
  object Panel2: TPanel
    Left = 268
    Top = 0
    Width = 132
    Height = 174
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 1
    object Button1: TButton
      Left = 22
      Top = 112
      Width = 88
      Height = 25
      Caption = 'Reset Changes'
      TabOrder = 0
      OnClick = Button1Click
    end
    object Button2: TButton
      Left = 2
      Top = 23
      Width = 55
      Height = 25
      Caption = 'All up'
      TabOrder = 1
      OnClick = Button2Click
    end
    object Button3: TButton
      Left = 2
      Top = 51
      Width = 55
      Height = 25
      Caption = 'All down'
      TabOrder = 2
      OnClick = Button3Click
    end
    object Button4: TButton
      Left = 2
      Top = 79
      Width = 55
      Height = 25
      Caption = 'All locked'
      TabOrder = 3
      OnClick = Button4Click
    end
    object RadioGroup1: TRadioGroup
      Left = 59
      Top = 21
      Width = 72
      Height = 81
      Caption = 'Sort by:'
      ItemIndex = 0
      Items.Strings = (
        'Skill'
        'Value'
        'Base'
        'Change')
      TabOrder = 4
      OnClick = RadioGroup1Click
    end
  end
  object Timer1: TTimer
    Interval = 500
    OnTimer = Timer1Timer
    Left = 92
    Top = 56
  end
end
