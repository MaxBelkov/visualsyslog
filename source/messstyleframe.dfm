object MessStyleFr: TMessStyleFr
  Left = 0
  Top = 0
  Width = 333
  Height = 132
  TabOrder = 0
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 333
    Height = 132
    Align = alClient
    Caption = 'Style'
    TabOrder = 0
    ExplicitHeight = 190
    object Label1: TLabel
      Left = 16
      Top = 19
      Width = 48
      Height = 13
      Caption = 'Text color'
    end
    object Label2: TLabel
      Left = 16
      Top = 75
      Width = 82
      Height = 13
      Caption = 'Background color'
    end
    object Label3: TLabel
      Left = 208
      Top = 21
      Width = 45
      Height = 13
      Caption = 'Text font'
    end
    object TextColorBox: TColorBox
      Left = 16
      Top = 38
      Width = 155
      Height = 22
      Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
      ItemHeight = 16
      TabOrder = 0
      OnSelect = StyleChange
    end
    object BackgroundColorBox: TColorBox
      Left = 16
      Top = 94
      Width = 155
      Height = 22
      Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
      ItemHeight = 16
      TabOrder = 1
      OnSelect = StyleChange
    end
    object BoldCB: TCheckBox
      Left = 208
      Top = 40
      Width = 97
      Height = 17
      Caption = 'Bold'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 2
      OnClick = StyleChange
    end
    object ItalicCB: TCheckBox
      Left = 208
      Top = 63
      Width = 97
      Height = 17
      Caption = 'Italic'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsItalic]
      ParentFont = False
      TabOrder = 3
      OnClick = StyleChange
    end
    object UnderlineCB: TCheckBox
      Left = 208
      Top = 86
      Width = 97
      Height = 17
      Caption = 'Underline'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsUnderline]
      ParentFont = False
      TabOrder = 4
      OnClick = StyleChange
    end
  end
end
