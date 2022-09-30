This is a modified version of ImGui (https://github.com/ocornut/imgui)

This file contains the log for the modifications done.

## imgui.h
### 1650: Add custom color to enum ImGuiCol_

# imgui_widgets.cpp

### 6382: Make Selectable show border on hover
From:
```cpp
RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
```
To:
```cpp
if (hovered)
{
    PushStyleColor(ImGuiCol_Border, GetColorU32(ImGuiCol_BorderHover));
    RenderFrame(bb.Min, bb.Max, col, true, 0.0f);
    PopStyleColor();
}
else
{
    RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
}
```


