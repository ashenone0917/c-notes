```cpp
DWORD dwStyle = m_textStatusOrderDesc->GetTextStyle();
m_textStatusOrderDesc->SetTextStyle(dwStyle | DT_EDITCONTROL);
```
 有时候text控件因为全英文无法换行或设置了"endellipsis"属性结尾无法自动...时是需要增加属性DT_EDITCONTROL
