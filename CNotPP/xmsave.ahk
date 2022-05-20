Run, XM2PSX.EXE
WinWait, XM2PSX - No file
WinActivate, XM2PSX - No file
WinMenuSelectItem, XM2PSX - No file,, File, Open
WinWaitActive, Open
ControlSend, Edit1, %1%, Open
ControlClick, Button2, Open,,,, NA
WinWaitClose, Open
ControlClick, TCheckBox1, XM2PSX - %1%,,,, NA
ControlClick, TCheckBox2, XM2PSX - %1%,,,, NA
ControlClick, TRadioButton2, XM2PSX - %1%,,,, NA
ControlClick, TBitBtn2, XM2PSX - %1%,,,, NA
WinWaitActive, Save XM file without sample data
ControlSend, Edit1, %2%
ControlClick, Button2, Save XM file without sample data,,,, NA
WinWaitClose, Save XM file without sample data
ControlClick, TBitBtn1, XM2PSX - %1%,,,, NA
WinWaitActive, VAB File to Create..
ControlSend, Edit1, %3%
ControlClick, Button2, VAB File to Create..,,,, NA
WinWaitClose, VAB File to Create..
WinClose, XM2PSX - %1%