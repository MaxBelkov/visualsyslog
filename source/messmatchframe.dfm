object MessMatchFr: TMessMatchFr
  Left = 0
  Top = 0
  Width = 346
  Height = 156
  TabOrder = 0
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 346
    Height = 156
    Align = alClient
    Caption = 'Match'
    TabOrder = 0
    ExplicitLeft = 3
    ExplicitWidth = 333
    object Label1: TLabel
      Left = 12
      Top = 60
      Width = 18
      Height = 13
      Alignment = taRightJustify
      Caption = 'and'
    end
    object Label3: TLabel
      Left = 12
      Top = 95
      Width = 18
      Height = 13
      Alignment = taRightJustify
      Caption = 'and'
    end
    object PriorityCB: TComboBox
      Left = 191
      Top = 25
      Width = 139
      Height = 21
      Style = csDropDownList
      DropDownCount = 12
      ItemHeight = 13
      TabOrder = 1
      OnSelect = MatchChange
      Items.Strings = (
        '<any>'
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
      Left = 191
      Top = 57
      Width = 139
      Height = 21
      TabOrder = 3
      OnChange = MatchChange
    end
    object MatchCaseCB: TCheckBox
      Left = 38
      Top = 128
      Width = 89
      Height = 17
      Caption = 'Match Case'
      Checked = True
      State = cbChecked
      TabOrder = 6
      OnClick = MatchChange
    end
    object TextEdit2: TEdit
      Left = 191
      Top = 92
      Width = 139
      Height = 21
      TabOrder = 5
      OnChange = MatchChange
    end
    object PriorityOperationCB: TComboBox
      Left = 38
      Top = 25
      Width = 139
      Height = 21
      Style = csDropDownList
      DropDownCount = 12
      ItemHeight = 13
      TabOrder = 0
      OnSelect = MatchChange
      Items.Strings = (
        'Priority ='
        'Priority <='
        'Priority >=')
    end
    object FieldCB1: TComboBox
      Left = 38
      Top = 57
      Width = 139
      Height = 21
      Style = csDropDownList
      DropDownCount = 16
      ItemHeight = 13
      TabOrder = 2
      OnSelect = MatchChange
      Items.Strings = (
        'Text contains'
        'Text NOT contains'
        'Message contains'
        'Message NOT contains'
        'IP ='
        'IP <>'
        'Host ='
        'Host <>'
        'Facility ='
        'Facility <>'
        'Tag ='
        'Tag <>')
    end
    object FieldCB2: TComboBox
      Left = 38
      Top = 92
      Width = 139
      Height = 21
      Style = csDropDownList
      DropDownCount = 16
      ItemHeight = 13
      TabOrder = 4
      OnSelect = MatchChange
      Items.Strings = (
        'Text contains'
        'Text NOT contains'
        'Message contains'
        'Message NOT contains'
        'IP ='
        'IP <>'
        'Host ='
        'Host <>'
        'Facility ='
        'Facility <>'
        'Tag ='
        'Tag <>')
    end
  end
end
