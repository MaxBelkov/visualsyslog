object MessMatchFr: TMessMatchFr
  Left = 0
  Top = 0
  Width = 366
  Height = 184
  TabOrder = 0
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 366
    Height = 184
    Align = alClient
    Caption = 'Match'
    TabOrder = 0
    object Label1: TLabel
      Left = 12
      Top = 60
      Width = 18
      Height = 13
      Alignment = taRightJustify
      Caption = 'and'
    end
    object Label3: TLabel
      Left = 14
      Top = 120
      Width = 18
      Height = 13
      Alignment = taRightJustify
      Caption = 'and'
    end
    object PriorityCB: TComboBox
      Left = 191
      Top = 25
      Width = 162
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
    object MatchCaseCB: TCheckBox
      Left = 38
      Top = 153
      Width = 89
      Height = 17
      Caption = 'Match Case'
      Checked = True
      State = cbChecked
      TabOrder = 6
      OnClick = MatchChange
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
      Left = 36
      Top = 52
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
      Top = 117
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
    object Memo1: TMemo
      Left = 191
      Top = 57
      Width = 162
      Height = 51
      ScrollBars = ssVertical
      TabOrder = 3
      WordWrap = False
      OnChange = MatchChange
    end
    object Memo2: TMemo
      Left = 191
      Top = 117
      Width = 162
      Height = 51
      ScrollBars = ssVertical
      TabOrder = 5
      WordWrap = False
      OnChange = MatchChange
    end
  end
end
