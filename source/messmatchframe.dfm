object MessMatchFr: TMessMatchFr
  Left = 0
  Top = 0
  Width = 333
  Height = 156
  TabOrder = 0
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 333
    Height = 156
    Align = alClient
    Caption = 'Match'
    TabOrder = 0
    object Label2: TLabel
      Left = 30
      Top = 28
      Width = 34
      Height = 13
      Alignment = taRightJustify
      Caption = 'Priority'
    end
    object Label1: TLabel
      Left = 21
      Top = 60
      Width = 43
      Height = 13
      Alignment = taRightJustify
      Caption = 'and Text'
    end
    object Label3: TLabel
      Left = 21
      Top = 95
      Width = 43
      Height = 13
      Alignment = taRightJustify
      Caption = 'and Text'
    end
    object FilterByPriorityCB: TComboBox
      Left = 70
      Top = 25
      Width = 145
      Height = 21
      Style = csDropDownList
      DropDownCount = 12
      ItemHeight = 13
      TabOrder = 0
      OnSelect = MatchChange
      Items.Strings = (
        '<all>'
        'emerg'
        'alert'
        'crit'
        'err'
        'warning'
        'notice'
        'info'
        'debug')
    end
    object TextEdit1: TEdit
      Left = 183
      Top = 57
      Width = 139
      Height = 21
      TabOrder = 2
      OnChange = MatchChange
    end
    object TextContainsCB1: TComboBox
      Left = 70
      Top = 57
      Width = 99
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 1
      Text = 'Contains'
      OnSelect = MatchChange
      Items.Strings = (
        'Contains'
        'NOT contains')
    end
    object MatchCaseCB: TCheckBox
      Left = 70
      Top = 128
      Width = 89
      Height = 17
      Caption = 'Match Case'
      Checked = True
      State = cbChecked
      TabOrder = 5
      OnClick = MatchChange
    end
    object TextContainsCB2: TComboBox
      Left = 70
      Top = 92
      Width = 99
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 3
      Text = 'Contains'
      OnSelect = MatchChange
      Items.Strings = (
        'Contains'
        'NOT contains')
    end
    object TextEdit2: TEdit
      Left = 183
      Top = 92
      Width = 139
      Height = 21
      TabOrder = 4
      OnChange = MatchChange
    end
  end
end
