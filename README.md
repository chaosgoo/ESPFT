版本 | 表现
:-: | :-:
2.11.1 | 渲染得到的bitmap.buffer里面全是0
2.4.12 | 正常
2.4.2 | 正常
2.4.3 | 正常
2.4.7 | 正常
2.5.0 | 正常
2.5.2 | 正常
2.5.5 | 正常
2.6.1 | 渲染时候奔溃
2.7.1 | 渲染时候奔溃
2.11.1 | 渲染后无内容

啊啊啊啊我要死了，怎么又回到了这个地方来
现在定位到ftsmooth.c里面render->raster_render( render->raster, &params );得到的结果是空的
render->raster_render定位到了gray_raster_render函数