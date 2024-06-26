## YUV420P内存模型
在内存中，图像数据的排列取决于其颜色空间格式和子采样类型。对于638x480分辨率的YUV420P格式图像，内存排列通常遵循以下方式：

### Y分量（亮度）
Y分量占主导地位，因为在YUV420P格式中，每个像素的亮度信息都有单独的Y值。在内存中，这些Y分量会顺序排列，首先是第一行的638个Y值，然后是第二行，依此类推，直到最后一行的480行。
```
Y0  Y1  Y2 ... Y637  // 第1行
Y638...          // 第2行
...
...             // 直到第480行
```

### U和V分量（色度）
对于U和V分量，由于是YUV420P格式（其中色度分量的分辨率是亮度分量的一半），每两行共用一组色度值。因此，在内存中，首先会排列所有的U分量，接着是所有的V分量。每2x2像素块共享一对U和V值。在实际内存排列中，每一行将会有319个U值，然后是下一行319个U值，总共240行，V分量也是如此。

```
U0  U1  ... U318 // 第1行U分量（对应图像的第1和第2行）
U319...          // 第2行U分量（对应图像的第3和第4行）
...
...             // 直到第240行U分量
V0  V1  ... V318 // 第1行V分量（对应图像的第1和第2行）
V319...          // 第2行V分量（对应图像的第3和第4行）
...
...             // 直到第240行V分量
```

### 16字节对齐
对于16字节对齐，如果图像的宽度（即Y分量的行宽）或色度分量的行宽（即U、V的一半行宽）不是16的倍数，为了内存对齐，每行的末尾将需要填充额外的字节。例如，对于638像素宽的图像，Y分量的每行末尾会填充2个字节以到达640字节，这样每行的宽度就是16的倍数了。同理，对于319像素宽的图像，U和V分量的每行末尾会填充1个字节以到达320字节。这样整个YUV420P图像在内存中的排列就满足了16字节对齐的要求。

在上面的排列中，为说明清楚并未展示对齐填充，实际内存排列需要根据对齐要求进行相应调整。

## Stride计算
对于分辨率为638x480的YUV420P图像，我们需要处理亮度分量Y以及色度分量U和V，每个分量单独考虑对齐要求。

### Y分量（亮度）：
- 分辨率：638x480，这意味着每一行有638个Y分量。
- 对齐要求：由于需要16字节对齐，每一行Y分量的结束位置需补足以确保整个行的字节数为16的倍数。
638本身不是16的倍数，所以我们找到大于638且最接近638的16的倍数是640。也就是说，每行Y分量需要额外添加2个字节来满足16字节对齐的要求。

638的Y分量 + 2个填充字节 = 640字节，640是16的整数倍。

由于图像高度为480行，Y分量总共需要填充的字节为：
```
2个填充字节/行 × 480行 = 960个填充字节
```

### U分量和V分量（色度）：
- 分辨率：由于是YUV420P，U和V分量的分辨率是Y的一半，即宽度为319，高度为240。
- 对齐要求：U和V分量的每行也需要补足到16的倍数。
319同样不是16的倍数，我们同样要找到大于319且最接近的16的倍数是320。每行U和V分量需要额外添加1个字节来满足对齐要求。

319的色度分量 + 1个填充字节 = 320字节，320是16的整数倍。

由于U和V分量的高度各为240行，每个分量总共需要填充的字节为：
```
1个填充字节/行 × 240行 = 240个填充字节（对于U分量）
1个填充字节/行 × 240行 = 240个填充字节（对于V分量）
```

### 结论：
为了16字节对齐，Y分量需要960个填充字节，U分量和V分量各需要240个填充字节。因此，总共需要的填充字节数为：
```
Y填充 + U填充 + V填充 = 960 + 240 + 240 = 1440个填充字节。
```
所以，需要总共填充1440个字节以确保整个YUV420P图像在内存中是16字节对齐的。

### 举例说明：
```cpp
// 计算填充字节数的示例代码

int calculatePadding(int resolution, int alignment) {
    // 如果已经对齐，则不需要填充
    if (resolution % alignment == 0) return 0;
    // 弥补到最近的alignment倍数所需的字节数
    return alignment - (resolution % alignment);
}

int width = 638; // 图像宽度
int heightY = 480; // 亮度分量高度（Y分量）
int heightUV = 240; // 色度分量高度（U和V分量）

// 亮度分量Y需要填充的字节
int paddingY = calculatePadding(width, 16) * heightY; // 2*480

// 色度分量U和V需要填充的字节
int widthUV = width / 2; // 色度分量宽度
int paddingUV = calculatePadding(widthUV, 16) * heightUV; // 1*240 for U and 1*240 for V

// 总填充字节
int totalPadding = paddingY + 2 * paddingUV; // 960 + 240 + 240

std::cout << "Total padding bytes needed: " << totalPadding << std::endl;
```
