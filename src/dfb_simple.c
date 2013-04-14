#include <stdio.h>
#include <unistd.h>
#include <directfb.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/mxcfb.h>

#include <fbdev/fbdev.h>


static IDirectFB                *dfb = NULL;
static IDirectFBDisplayLayer    *layer=NULL;
static IDirectFBSurface         *primary = NULL;
static int screen_width  = 0;
static int screen_height = 0;

static IDirectFBFont           *font;
static int fontheight;

static IDirectFBSurface *logo = NULL;

static IDirectFBVideoProvider   *videoprovider;
static IDirectFBWindow          *videowindow = NULL;
static IDirectFBSurface         *videosurface = NULL;

#define DFBCHECK(x...)                                          \
{                                                               \
    DFBResult err = x;                                          \
                                                                \
    if (err != DFB_OK)                                          \
    {                                                           \
        fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );  \
        DirectFBErrorFatal( #x, err );                          \
    }                                                           \
}

#define EPDC_STR_ID		"mxc_epdc_fb"

#define WAVEFORM_MODE_INIT	0x0	/* Screen goes to white (clears) */
#define WAVEFORM_MODE_DU	0x1	/* Grey->white/grey->black */
#define WAVEFORM_MODE_GC16	0x2	/* High fidelity (flashing) */
#define WAVEFORM_MODE_GC4	0x3	/* Lower fidelity */
#define WAVEFORM_MODE_A2	0x4	/* Fast black/white animation */

void epdc_update(int left, int top, int width, int height, int waveform, int wait_for_complete, unsigned int flags);
void videoupdate(void* ctx);

int main (int argc, char **argv)
{
    int i;

    DFBCHECK (DirectFBInit (&argc, &argv));

    DFBCHECK( DirectFBSetOption( "no-vt", NULL ) );                // 由于onyx没有tty0，所以姑且禁用vt
    DFBCHECK( DirectFBSetOption( "disable-module", "tslib" ) );    // 暂不调试触屏，禁用tslib

    DFBCHECK (DirectFBCreate (&dfb));
    DFBCHECK (dfb->SetCooperativeLevel (dfb, DFSCL_FULLSCREEN));


    //// 创建主表面
    //{
    //    DFBSurfaceDescription dsc;
    //    dsc.flags = DSDESC_CAPS;
    //    dsc.caps  = DSCAPS_PRIMARY | DSCAPS_FLIPPING;

    //    DFBCHECK (dfb->CreateSurface( dfb, &dsc, &primary ));
    //    DFBCHECK (primary->GetSize (primary, &screen_width, &screen_height));
    //    DFBCHECK(dfb->GetDisplayLayer( dfb, DLID_PRIMARY, &layer ));
    //}

    //// 载入字体
    //{
    //    DFBFontDescription desc;

    //    desc.flags = DFDESC_HEIGHT;
    //    desc.height = 24;

    //    // MWO: dfb->CreateFont()建立字体。primary->SetFont()将字体设定给主表面。
    //    DFBCHECK(dfb->CreateFont( dfb, FONT, &desc, &font ));
    //    DFBCHECK(font->GetHeight( font, &fontheight ));
    //    DFBCHECK(primary->SetFont( primary, font ));
    //}

    //// 载入图片
    //{
    //    IDirectFBImageProvider *provider;
    //    DFBSurfaceDescription dsc;

    //    DFBCHECK (dfb->CreateImageProvider (dfb, DATADIR"/gameboy.png", &provider));
    //    DFBCHECK (provider->GetSurfaceDescription (provider, &dsc));
    //    DFBCHECK (dfb->CreateSurface( dfb, &dsc, &logo ));
    //    DFBCHECK (provider->RenderTo (provider, logo, NULL));
    //    provider->Release (provider);
    //}

    // 创建主图层
    {
        DFBCHECK(dfb->GetDisplayLayer( dfb, DLID_PRIMARY, &layer ));
    }
    // 载入gif
    {
        DFBSurfaceDescription sdsc;
        DFBWindowDescription wdsc;

        DFBCHECK (dfb->CreateVideoProvider (dfb, DATADIR"/2bittest.gif", &videoprovider));
        DFBCHECK (videoprovider->GetSurfaceDescription (videoprovider, &sdsc));
        wdsc.flags = DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT;
        wdsc.posx = 0;
        wdsc.posy = 0;
        wdsc.width = sdsc.width;
        wdsc.height = sdsc.height;
        DFBCHECK(layer->CreateWindow( layer, &wdsc, &videowindow ) );
        DFBCHECK(videowindow->GetSurface( videowindow, &videosurface ) );

        videowindow->SetOpacity( videowindow, 0xFF );

        DFBCHECK(videoprovider->PlayTo( videoprovider, videosurface, NULL, videoupdate, NULL ));
    }

    //// 画线
    //DFBCHECK (primary->SetColor (primary, 0x0, 0x0, 0x0, 0xff));
    //for (i=0;i<50;i++)
    //{
    //    DFBCHECK (primary->DrawLine (primary, 0, i*10+1, screen_width - 1, i*10+1));
    //}
    //// 输出信息
    //{
    //    char buf[32];
    //    snprintf( buf, sizeof(buf), "DirectFB on ONYX I62HD");
    //    primary->SetColor( primary, 0x0, 0x0, 0x0, 0xFF );
    //    primary->SetBlittingFlags(primary, DSBLIT_FLIP_HORIZONTAL | DSBLIT_FLIP_VERTICAL);
    //    primary->DrawString( primary, buf, -1, 100, 100, DSTF_LEFT | DSTF_TOP );
    //}


    //// 屏幕刷白
    //{
    //    DFBCHECK (primary->SetBlittingFlags(primary, DSBLIT_FLIP_HORIZONTAL | DSBLIT_FLIP_VERTICAL));
    //    DFBCHECK (primary->SetColor (primary, 0xFF, 0xFF, 0xFF, 0xFF));
    //    DFBCHECK (primary->FillRectangle (primary, 0, 0, screen_width, screen_height));
    //    DFBCHECK (primary->Flip (primary, NULL, 0));
    //    epdc_update(0,0, screen_width, screen_height, WAVEFORM_MODE_GC16, TRUE, 0);
    //}

    //{
    //    int i;
    //    for (i=0;i<100;i=i+2)
    //    {
    //        DFBCHECK (primary->FillRectangle (primary, 0, 0, screen_width, screen_height));
    //        DFBCHECK (primary->Blit (primary, logo, NULL, i, i));
    //        DFBCHECK (primary->Flip (primary, NULL, 0));
    //        epdc_update(0,0, screen_width, screen_height, WAVEFORM_MODE_A2, TRUE, EPDC_FLAG_FORCE_MONOCHROME);
    //    }
    //}

    //logo->Release (logo);
    //primary->Release( primary );
    //font->Release( font );

    sleep(20);

    videoprovider->Release (videoprovider);
    layer->Release(layer);
    dfb->Release( dfb );
    return 23;
}

void epdc_update(int left, int top, int width, int height, int waveform, int wait_for_complete, uint flags)
{
    // 见2013-02-22笔记1号
	struct mxcfb_update_data upd_data;
    __u32   upd_marker_data;
	int     retval;
	int     wait = wait_for_complete | flags;
	int     max_retry = 10;

    // FIXME: 计算出合适的边界值
    //int tleft= -(left+width)+screen_info_.xres;
    //int ttop = -(top+height-1)+screen_info_.yres;
    int tleft= left;
    int ttop = top;

    FBDev *dfb_fbdev = dfb_system_data();

	upd_data.update_mode = UPDATE_MODE_PARTIAL;
	upd_data.waveform_mode = waveform;
	upd_data.update_region.left = tleft;
	upd_data.update_region.width = width;
	upd_data.update_region.top = ttop;
	upd_data.update_region.height = height;
	upd_data.temp = TEMP_USE_AMBIENT;
	upd_data.flags = flags;

	if (wait)
    {
		/* Get unique marker value */
		//upd_data.update_marker = marker_val_++;
    }
	else
    {
		upd_data.update_marker = 0;
    }

	retval = ioctl(dfb_fbdev->fd, MXCFB_SEND_UPDATE, &upd_data);
	while (retval < 0) 
    {
		/* We have limited memory available for updates, so wait and
		 * then try again after some updates have completed */
		sleep(1);
		retval = ioctl(dfb_fbdev->fd, MXCFB_SEND_UPDATE, &upd_data);
		if (--max_retry <= 0) 
        {
			wait = 0;
			flags = 0;
			break;
		}
	}

	if (wait) 
    {
		upd_marker_data = upd_data.update_marker;

		/* Wait for update to complete */
		retval = ioctl(dfb_fbdev->fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &upd_marker_data);
		if (retval < 0) 
        {
			flags = 0;
		}
	}
}

void videoupdate(void* ctx)
{
    epdc_update(0,0, screen_width, screen_height, WAVEFORM_MODE_A2, TRUE, EPDC_FLAG_FORCE_MONOCHROME);
}
