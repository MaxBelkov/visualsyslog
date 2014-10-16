object MessMatchFr: TMessMatchFr
  Left = 0
  Top = 0
  Width = 434
  Height = 132
  TabOrder = 0
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 434
    Height = 132
    Align = alClient
    Caption = 'Match'
    TabOrder = 0
    ExplicitWidth = 767
    ExplicitHeight = 337
    object Label2: TLabel
      Left = 22
      Top = 28
      Width = 34
      Height = 13
      Alignment = taRightJustify
      Caption = 'Priority'
    end
    object Label1: TLabel
      Left = 34
      Top = 60
      Width = 22
      Height = 13
      Alignment = taRightJustify
      Caption = 'Text'
    end
    object Label3: TLabel
      Left = 34
      Top = 95
      Width = 22
      Height = 13
      Alignment = taRightJustify
      Caption = 'Text'
    end
    object FilterByPriorityCB: TComboBox
      Left = 70
      Top = 25
      Width = 145
      Height = 19
      Style = csOwnerDrawFixed
      DropDownCount = 12
      ItemHeight = 13
      TabOrder = 0
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
      TabOrder = 1
    end
    object TextContainsCB1: TComboBox
      Left = 70
      Top = 57
      Width = 99
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 2
      Text = 'Contains'
      Items.Strings = (
        'Contains'
        'NOT contains')
    end
    object MatchCaseCB: TCheckBox
      Left = 336
      Top = 80
      Width = 89
      Height = 17
      Caption = 'Match Case'
      Checked = True
      State = cbChecked
      TabOrder = 3
    end
    object TextContainsCB2: TComboBox
      Left = 70
      Top = 92
      Width = 99
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 4
      Text = 'Contains'
      Items.Strings = (
        'Contains'
        'NOT contains')
    end
    object TextEdit2: TEdit
      Left = 183
      Top = 92
      Width = 139
      Height = 21
      TabOrder = 5
    end
  end
end
