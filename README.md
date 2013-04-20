onyx-i62hd-dfbsimple
====================

directfb simple test on onyx i62hd

20130420 directfb结构设计过于复杂，不适合简单设备。
20130414 可以画基本图形，显示文字，显示图片。无法旋转，刷新率略低，大约2fps，是否图片大小问题？

1. ${DFB\_ROOT}/include/directfb和${DFB\_ROOT}/include/directfb-internal
------------------------------------------------------------------------
如果只是开发directfb的应用程序的话，用include/directfb即可。如果要开发插件等需要访问directfb内部对象的时候，就要include/directfb-internal了。

2. 访问directfb显示设备的文件描述符
-----------------------------------

        #include <fbdev/fbdev.h>
        
        // 在恰当的地方用该语句
        FBDev* fbdev = dfb_system_data();

dfb\_system\_data()会返回无类型指针system\_data。将其当成FBDev指针来用的话，就可以通过fbdev->fd访问文件描述符。


