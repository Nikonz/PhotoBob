# PhotoBob
Basic image editor.

# Examples of usage

### Just align

`./build/bin/align samples/align/00908.bmp result_0.bmp --align`

``./build/bin/align samples/align/00936.bmp result_1.bmp --align``

Before:

![](https://github.com/Nikonz/PhotoBob/blob/master/samples/align/00908u.bmp)
![](https://github.com/Nikonz/PhotoBob/blob/master/samples/align/00936u.bmp)

After:

![](https://github.com/Nikonz/PhotoBob/blob/master/samples/align/1.bmp)
![](https://github.com/Nikonz/PhotoBob/blob/master/samples/align/4.bmp)

### Borders selection

`./build/bin/align samples/canny/bLena.bmp result.bmp --canny 30 80`

Before:

![](https://github.com/Nikonz/PhotoBob/blob/master/samples/canny/bLena.bmp) 

After:

![](https://github.com/Nikonz/PhotoBob/blob/master/samples/canny/0.bmp)

### Contrast filter

`./build/bin/align samples/autocontrast/align.bmp  result_0.bmp --autocontrast 0.2`

`./build/bin/align samples/autocontrast/align5.bmp result_1.bmp --autocontrast 0.11`

Before:

![](https://github.com/Nikonz/PhotoBob/blob/master/samples/autocontrast/align.bmp) !![](https://github.com/Nikonz/PhotoBob/blob/master/samples/autocontrast/align5.bmp)

After:

![](https://github.com/Nikonz/PhotoBob/blob/master/samples/autocontrast/contrast.bmp) ![](https://github.com/Nikonz/PhotoBob/blob/master/samples/autocontrast/5.bmp)

### Gaussian filter

Before:

`./build/bin/align samples/gaussian/align.bmp result.bmp --gaussian 3 5`

![](https://github.com/Nikonz/PhotoBob/blob/master/samples/gaussian/tsar.bmp)

After:

![](https://github.com/Nikonz/PhotoBob/blob/master/samples/gaussian/0.bmp)

### Gray-world filter

`./build/bin/align samples/gray-world/align.bmp res.bmp result_0.bmp`

`./build/bin/align samples/gray-world/align.bmp align2.bmp result_1.bmp`

Before:

![](https://github.com/Nikonz/PhotoBob/blob/master/samples/gray-world/res.bmp)
![](https://github.com/Nikonz/PhotoBob/blob/master/samples/gray-world/align2.bmp)

After:

![](https://github.com/Nikonz/PhotoBob/blob/master/samples/gray-world/0.bmp)
![](https://github.com/Nikonz/PhotoBob/blob/master/samples/gray-world/2.bmp)

### Median filter

![]()
![]()

### Unsharp filter
