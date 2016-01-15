object FileFr: TFileFr
  Left = 0
  Top = 0
  Width = 486
  Height = 261
  TabOrder = 0
  object GroupBox: TGroupBox
    Left = 0
    Top = 0
    Width = 486
    Height = 261
    Align = alClient
    Caption = 'File settings'
    TabOrder = 0
    DesignSize = (
      486
      261)
    object Label11: TLabel
      Left = 11
      Top = 24
      Width = 27
      Height = 13
      Caption = 'Name'
    end
    object SpeedButton2: TSpeedButton
      Left = 455
      Top = 39
      Width = 25
      Height = 25
      Hint = 'Select log file'
      Anchors = [akTop, akRight]
      Flat = True
      Glyph.Data = {
        36040000424D3604000000000000360000002800000010000000100000000100
        2000000000000004000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000300000
        0033000000330000003300000033000000330000003300000033000000330000
        00330000003300000033000000330000002F00000000000000004598D1F24398
        D2FF4094D0FF3E92CFFF3E92CEFF3F92CEFF3F92CEFF3F92CEFF3F92CEFF3F92
        CEFF3F92CEFF3F92CEFF3F93CFFF4194CEF00000000E000000004499D2FF3F94
        D0FFABFBFFFF9BF3FFFF92F1FFFF93F1FFFF93F1FFFF93F1FFFF93F1FFFF93F1
        FFFF93F1FFFF93F1FFFFA6F8FFFF65B8E3FF31709D5F000000004398D2FF4FA6
        D9FF8EDAF5FFA2EEFFFF82E5FEFF84E5FEFF84E5FEFF85E6FEFF85E6FEFF85E6
        FEFF85E6FEFF84E6FEFF96EBFFFF8CD8F5FF3985BCB8000000004296D1FF6BBE
        E8FF6DBDE6FFBBF2FFFF75DEFDFF77DEFCFF78DEFCFF7BDFFCFF7DDFFCFF7DDF
        FCFF7DDFFCFF7CDFFCFF80E0FDFFADF0FFFF4D9DD3FF0000000E4095D0FF8AD7
        F5FF44A1D8FFDDFDFFFFDAFAFFFFDBFAFFFFDEFAFFFF74DCFCFF76DBFAFF75DA
        FAFF74DAFAFF74DAFAFF72D9FAFFA1E8FFFF7CBFE6FF306F9C5E3E94D0FFABF0
        FFFF449DD6FF368CCBFF368CCBFF368CCBFF378BCBFF5CBEEAFF6FD9FBFF6AD6
        FAFF68D5F9FF67D4F9FF66D4F9FF82DEFCFFAAE0F6FF3885BCB93D92CFFFB9F4
        FFFF73DBFBFF6BCCF2FF6CCDF3FF6CCEF3FF6DCEF3FF479CD4FF56BAE9FFDAF8
        FFFFD7F6FFFFD6F6FFFFD5F6FFFFD5F7FFFFDBFCFFFF3E94D0FF3C92CFFFC0F3
        FFFF71DAFBFF74DBFBFF75DBFCFF75DBFCFF76DCFCFF73DAFAFF449CD4FF378C
        CBFF368CCBFF358CCCFF348DCCFF3890CEFF3D94D0FF4398D2EB3B92CFFFCAF6
        FFFF69D5F9FF6CD5F9FF6BD5F9FF69D5F9FF69D5FAFF6AD7FBFF68D4FAFF5EC7
        F1FF5EC7F2FF5DC8F2FFB4E3F8FF3D94D0FF3F8FC669000000003B92CFFFD5F7
        FFFF60D1F9FF61D0F8FFB4EBFDFFD9F6FFFFDAF8FFFFDAF8FFFFDBF9FFFFDCFA
        FFFFDCFAFFFFDCFBFFFFE0FFFFFF3E95D0FF4599D333000000003D94D0FFDCFC
        FFFFD8F7FFFFD8F7FFFFDBFAFFFF358ECDFF3991CEFF3A92CFFF3A92CFFF3A92
        CFFF3A92CFFF3B92CFFF3D94D0FF4398D2D700000000000000004398D2B03D94
        D0FF3A92CFFF3A92CFFF3D94D0FF4197D1D20000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000}
      ParentShowHint = False
      ShowHint = True
      OnClick = SpeedButton2Click
    end
    object Label1: TLabel
      Left = 295
      Top = 135
      Width = 22
      Height = 13
      Caption = 'hour'
    end
    object FileEdit: TEdit
      Left = 11
      Top = 43
      Width = 440
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      OnChange = Change
    end
    object RotationRB1: TRadioButton
      Left = 11
      Top = 76
      Width = 113
      Height = 17
      Caption = 'Rotation off'
      TabOrder = 1
      OnClick = Change
    end
    object RotationRB2: TRadioButton
      Left = 11
      Top = 103
      Width = 113
      Height = 17
      Caption = 'Rotation by size:'
      TabOrder = 2
      OnClick = Change
    end
    object RotationRB3: TRadioButton
      Left = 11
      Top = 130
      Width = 113
      Height = 17
      Caption = 'Rotation by date:'
      TabOrder = 3
      OnClick = Change
    end
    object SizeEdit: TEdit
      Left = 136
      Top = 101
      Width = 49
      Height = 21
      TabOrder = 4
      Text = '1'
      OnChange = Change
    end
    object MultCB: TComboBox
      Left = 217
      Top = 101
      Width = 49
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 6
      Text = 'KBs'
      OnSelect = Change
      Items.Strings = (
        'KBs'
        'MBs'
        'GBs')
    end
    object MomentCB: TComboBox
      Left = 136
      Top = 130
      Width = 66
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 2
      TabOrder = 7
      Text = 'Month'
      OnSelect = Change
      Items.Strings = (
        'Day'
        'Week'
        'Month'
        'Year')
    end
    object HourEdit: TEdit
      Left = 216
      Top = 130
      Width = 49
      Height = 21
      TabOrder = 8
      Text = '0'
      OnChange = Change
    end
    object Panel1: TPanel
      Left = 11
      Top = 160
      Width = 466
      Height = 97
      Anchors = [akLeft, akTop, akRight]
      BevelOuter = bvNone
      TabOrder = 10
      DesignSize = (
        466
        97)
      object Label4: TLabel
        Left = 0
        Top = 0
        Width = 139
        Height = 13
        Caption = 'After rotation rename file to:'
      end
      object HelpLabel: TLabel
        Left = 432
        Top = 46
        Width = 27
        Height = 16
        Cursor = crHandPoint
        Anchors = [akTop, akRight]
        Caption = 'help'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clNavy
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold, fsUnderline]
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        OnClick = HelpLabelClick
        ExplicitLeft = 435
      end
      object Label2: TLabel
        Left = 0
        Top = 76
        Width = 76
        Height = 13
        Caption = 'Number of files:'
      end
      object RenamingRB0: TRadioButton
        Left = 8
        Top = 22
        Width = 174
        Height = 17
        Caption = 'Name + number'
        Checked = True
        TabOrder = 0
        TabStop = True
        OnClick = Change
      end
      object RenamingRB1: TRadioButton
        Left = 8
        Top = 46
        Width = 97
        Height = 17
        Hint = 'File name without path'
        Caption = 'New name:'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        OnClick = Change
      end
      object CountEdit: TEdit
        Left = 109
        Top = 72
        Width = 70
        Height = 21
        TabOrder = 3
        Text = '1'
        OnChange = Change
      end
      object NewNameEdit: TEdit
        Left = 109
        Top = 45
        Width = 312
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 2
        OnChange = Change
      end
      object CountEditUD: TUpDown
        Left = 179
        Top = 72
        Width = 17
        Height = 21
        Associate = CountEdit
        Min = 1
        Max = 32767
        Position = 1
        TabOrder = 4
        Thousands = False
      end
    end
    object HourEditUD: TUpDown
      Left = 265
      Top = 130
      Width = 17
      Height = 21
      Associate = HourEdit
      Max = 23
      TabOrder = 9
      Thousands = False
    end
    object SizeEditUD: TUpDown
      Left = 185
      Top = 101
      Width = 17
      Height = 21
      Associate = SizeEdit
      Min = 1
      Max = 32767
      Position = 1
      TabOrder = 5
      Thousands = False
    end
  end
  object OpenDialog: TOpenDialog
    DefaultExt = 'wav'
    Filter = 'All files (*.*)|*.*'
    Options = [ofNoChangeDir, ofEnableSizing]
    Left = 156
    Top = 3
  end
end
