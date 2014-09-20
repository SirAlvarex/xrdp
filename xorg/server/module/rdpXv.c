/*
Copyright 2014 Jay Sorg

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

XVideo

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* this should be before all X11 .h files */
#include <xorg-server.h>
#include <xorgVersion.h>

/* all driver need this */
#include <xf86.h>
#include <xf86_OSproc.h>

#include <xf86xv.h>
#include <X11/extensions/Xv.h>
#include <fourcc.h>

#include "rdp.h"
#include "rdpMisc.h"
#include "rdpReg.h"
#include "rdpClientCon.h"

#define LOG_LEVEL 1
#define LLOGLN(_level, _args) \
    do { if (_level < LOG_LEVEL) { ErrorF _args ; ErrorF("\n"); } } while (0)

#define T_NUM_ENCODINGS 1
static XF86VideoEncodingRec g_xrdpVidEncodings[T_NUM_ENCODINGS] =
{ { 0, "XV_IMAGE", 2046, 2046, { 1, 1 } } };

#define T_NUM_FORMATS 1
static XF86VideoFormatRec g_xrdpVidFormats[T_NUM_FORMATS] =
{ { 0, TrueColor } };

#define FOURCC_RV15 0x35315652
#define XVIMAGE_RV15 \
{ FOURCC_RV15,XvRGB,LSBFirst, \
  {'R','V','1','5',0,0,0,0,0,0,0,0,0,0,0,0}, \
  16,XvPacked,1,15,0x001f,0x03e0,0x7c00,0,0,0,0,0,0,0,0,0, \
  {'R','V','B',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
  XvTopToBottom \
}

#define FOURCC_RV16 0x36315652
#define XVIMAGE_RV16 \
{ FOURCC_RV16,XvRGB,LSBFirst, \
  {'R','V','1','6',0,0,0,0,0,0,0,0,0,0,0,0}, \
  16,XvPacked,1,16,0x001f,0x07e0,0xf800,0,0,0,0,0,0,0,0,0, \
  {'R','V','B',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
  XvTopToBottom \
}

#define FOURCC_RV24 0x34325652
#define XVIMAGE_RV24 \
{ FOURCC_RV24,XvRGB,LSBFirst, \
  {'R','V','2','4',0,0,0,0,0,0,0,0,0,0,0,0}, \
  32,XvPacked,1,24,0x000000ff,0x0000ff00,0x00ff0000,0,0,0,0,0,0,0,0,0, \
  {'R','V','B',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
  XvTopToBottom \
}

#define FOURCC_RV32 0x32335652
#define XVIMAGE_RV32 \
{ FOURCC_RV32,XvRGB,LSBFirst, \
  {'R','V','3','2',0,0,0,0,0,0,0,0,0,0,0,0}, \
  32,XvPacked,1,32,0x000000ff,0x0000ff00,0x00ff0000,0,0,0,0,0,0,0,0,0, \
  {'R','V','B',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
  XvTopToBottom \
}

/* XVIMAGE_YV12 FOURCC_YV12 0x32315659 */
/* XVIMAGE_YUY2 FOURCC_YUY2 0x32595559 */
/* XVIMAGE_UYVY FOURCC_UYVY 0x59565955 */
/* XVIMAGE_I420 FOURCC_I420 0x30323449 */

static XF86ImageRec g_xrdpVidImages[] =
{ XVIMAGE_RV15, XVIMAGE_RV16, XVIMAGE_RV24, XVIMAGE_RV32, XVIMAGE_YV12,
  XVIMAGE_YUY2, XVIMAGE_UYVY, XVIMAGE_I420 };

#define T_MAX_PORTS 1

/*****************************************************************************/
static int
xrdpVidPutVideo(ScrnInfoPtr pScrn, short vid_x, short vid_y,
                short drw_x, short drw_y, short vid_w, short vid_h,
                short drw_w, short drw_h, RegionPtr clipBoxes,
                pointer data, DrawablePtr pDraw)
{
    LLOGLN(0, ("xrdpVidPutVideo:"));
    return Success;
}

/*****************************************************************************/
static int
xrdpVidPutStill(ScrnInfoPtr pScrn, short vid_x, short vid_y,
                short drw_x, short drw_y, short vid_w, short vid_h,
                short drw_w, short drw_h, RegionPtr clipBoxes,
                pointer data, DrawablePtr pDraw)
{
    LLOGLN(0, ("xrdpVidPutStill:"));
    return Success;
}

/*****************************************************************************/
static int
xrdpVidGetVideo(ScrnInfoPtr pScrn, short vid_x, short vid_y,
                short drw_x, short drw_y, short vid_w, short vid_h,
                short drw_w, short drw_h, RegionPtr clipBoxes,
                pointer data, DrawablePtr pDraw)
{
    LLOGLN(0, ("xrdpVidGetVideo:"));
    return Success;
}

/*****************************************************************************/
static int
xrdpVidGetStill(ScrnInfoPtr pScrn, short vid_x, short vid_y,
                short drw_x, short drw_y, short vid_w, short vid_h,
                short drw_w, short drw_h, RegionPtr clipBoxes,
                pointer data, DrawablePtr pDraw)
{
    LLOGLN(0, ("FBDevTIVidGetStill:"));
    return Success;
}

/*****************************************************************************/
static void
xrdpVidStopVideo(ScrnInfoPtr pScrn, pointer data, Bool Cleanup)
{
    LLOGLN(0, ("xrdpVidStopVideo:"));
}

/*****************************************************************************/
static int
xrdpVidSetPortAttribute(ScrnInfoPtr pScrn, Atom attribute,
                        INT32 value, pointer data)
{
    LLOGLN(0, ("xrdpVidSetPortAttribute:"));
    return Success;
}

/*****************************************************************************/
static int
xrdpVidGetPortAttribute(ScrnInfoPtr pScrn, Atom attribute,
                        INT32 *value, pointer data)
{
    LLOGLN(0, ("xrdpVidGetPortAttribute:"));
    return Success;
}

/*****************************************************************************/
static void
xrdpVidQueryBestSize(ScrnInfoPtr pScrn, Bool motion,
                     short vid_w, short vid_h, short drw_w, short drw_h,
                     unsigned int *p_w, unsigned int *p_h, pointer data)
{
    LLOGLN(0, ("xrdpVidQueryBestSize:"));
}

/*****************************************************************************/
static int
YV12_to_RGB32(unsigned char *yuvs, int width, int height, int *rgbs)
{
    int size_total;
    int y;
    int u;
    int v;
    int c;
    int d;
    int e;
    int r;
    int g;
    int b;
    int t;
    int i;
    int j;

    size_total = width * height;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            y = yuvs[j * width + i];
            u = yuvs[(j / 2) * (width / 2) + (i / 2) + size_total];
            v = yuvs[(j / 2) * (width / 2) + (i / 2) + size_total + (size_total / 4)];
            c = y - 16;
            d = u - 128;
            e = v - 128;
            t = (298 * c + 409 * e + 128) >> 8;
            b = RDPCLAMP(t, 0, 255);
            t = (298 * c - 100 * d - 208 * e + 128) >> 8;
            g = RDPCLAMP(t, 0, 255);
            t = (298 * c + 516 * d + 128) >> 8;
            r = RDPCLAMP(t, 0, 255);
            rgbs[j * width + i] = (r << 16) | (g << 8) | b;
        }
    }
    return 0;
}

/*****************************************************************************/
static int
stretch_RGB32_RGB32(int *src, int src_width, int src_height,
                    int src_x, int src_y, int src_w, int src_h,
                    int *dst, int dst_w, int dst_h)
{
    int mwidth;
    int mheight;
    int index;

    mwidth = RDPMIN(src_width, dst_w);
    mheight = RDPMIN(src_height, dst_h);
    for (index = 0; index < mheight; index++)
    {
        g_memcpy(dst, src, mwidth * 4);
        src += src_width;
        dst += dst_w;
    }
    return 0; 
}

/*****************************************************************************/
static int
xrdpVidPutImage_FOURCC_YV12(ScrnInfoPtr pScrn,
                            short src_x, short src_y, short drw_x, short drw_y,
                            short src_w, short src_h, short drw_w, short drw_h,
                            int format, unsigned char* buf,
                            short width, short height,
                            Bool sync, RegionPtr clipBoxes,
                            pointer data, DrawablePtr dst)
{
    rdpPtr dev;
    char *dst8;
    int *dst32;
    int *src32;
    int *src32a;
    int *rgborg32;
    int *rgbend32;
    int index;
    int jndex;
    int num_clips;
    RegionRec dreg;
    BoxRec box;

    LLOGLN(0, ("xrdpVidPutImage_FOURCC_YV12:"));
    dev = XRDPPTR(pScrn);

    rgborg32 = (int *) g_malloc(width * height * 4, 0);
    rgbend32 = (int *) g_malloc(drw_w * drw_h * 4, 0);

    YV12_to_RGB32(buf, width, height, rgborg32);
    stretch_RGB32_RGB32(rgborg32, width, height, src_x, src_y, src_w, src_h,
                        rgbend32, drw_w, drw_h);

    box.x1 = drw_x;
    box.y1 = drw_y;
    box.x2 = box.x1 + drw_w;
    box.y2 = box.y1 + drw_h;
    rdpRegionInit(&dreg, &box, 0);

    num_clips = REGION_NUM_RECTS(clipBoxes);
    if (num_clips > 0)
    {
        rdpRegionIntersect(&dreg, &dreg, clipBoxes);
    }

    num_clips = REGION_NUM_RECTS(clipBoxes);
    for (jndex = 0; jndex < num_clips; jndex++)
    {
        box = REGION_RECTS(&dreg)[jndex];
        dst8 = dev->pfbMemory + box.y1 * dev->paddedWidthInBytes;
        src32a = rgbend32 + (box.y1 - drw_y) * drw_w;
        for (index = 0; index < box.y2 - box.y1; index++)
        {
            dst32 = (int *) dst8;
            dst32 += box.x1;
            src32 = src32a + (box.x1 - drw_x);
            g_memcpy(dst32, src32, (box.x2 - box.x1) * 4);
            dst8 += dev->paddedWidthInBytes;
            src32a += drw_w;
        }
    }

    rdpClientConAddAllReg(dev, &dreg, dst);
    rdpRegionUninit(&dreg);

    g_free(rgborg32);
    g_free(rgbend32);

    return Success;
}

/*****************************************************************************/
static int
xrdpVidPutImage(ScrnInfoPtr pScrn,
                short src_x, short src_y, short drw_x, short drw_y,
                short src_w, short src_h, short drw_w, short drw_h,
                int format, unsigned char* buf, short width, short height,
                Bool sync, RegionPtr clipBoxes, pointer data, DrawablePtr dst)
{
    LLOGLN(0, ("xrdpVidPutImage:"));
    switch (format)
    {
        case FOURCC_YV12:
            LLOGLN(10, ("xrdpVidPutImage: FOURCC_YV12"));
            return xrdpVidPutImage_FOURCC_YV12(pScrn,
                                               src_x, src_y, drw_x, drw_y,
                                               src_w, src_h, drw_w, drw_h,
                                               format, buf, width, height,
                                               sync, clipBoxes, data, dst);
        default:
            LLOGLN(0, ("xrdpVidPutImage: unknown format 0x%8.8x", format));
            break;
    }
    return Success;
}

/*****************************************************************************/
static int
xrdpVidQueryImageAttributes(ScrnInfoPtr pScrn, int id,
                            unsigned short *w, unsigned short *h,
                            int *pitches, int *offsets)
{
    int size, tmp;

    LLOGLN(0, ("xrdpVidQueryImageAttributes:"));
    /* this is same code as all drivers currently have */
    if (*w > 2046)
    {
        *w = 2046;
    }
    if (*h > 2046)
    {
        *h = 2046;
    }
    /* make w multiple of 4 so that resizing works properly */
    *w = (*w + 3) & ~3;
    if (offsets != NULL)
    {
        offsets[0] = 0;
    }
    switch (id)
    {
        case FOURCC_YV12:
        case FOURCC_I420:
            /* make h be even */
            *h = (*h + 1) & ~1;
            /* make w be multiple of 4 (ie. pad it) */
            size = (*w + 3) & ~3;
            /* width of a Y row => width of image */
            if (pitches != NULL)
            {
                pitches[0] = size;
            }
            /* offset of U plane => w * h */
            size *= *h;
            if (offsets != NULL)
            {
                offsets[1] = size;
            }
            /* width of U, V row => width / 2 */
            tmp = ((*w >> 1) + 3) & ~3;
            if (pitches != NULL)
            {
                pitches[1] = pitches[2] = tmp;
            }
            /* offset of V => Y plane + U plane (w * h + w / 2 * h / 2) */
            tmp *= (*h >> 1);
            size += tmp;
            if (offsets != NULL)
            {
                offsets[2] = size;
            }
            size += tmp;
            break;
        case FOURCC_RV15:
        case FOURCC_RV16:
        case FOURCC_YUY2:
        case FOURCC_UYVY:
            size = (*w) * 2;
            if (pitches != NULL)
            {
                pitches[0] = size;
            }
            size *= *h;
            break;
        case FOURCC_RV24:
            size = (*w) * 3;
            if (pitches != NULL)
            {
                pitches[0] = size;
            }
            size *= *h;
            break;
        case FOURCC_RV32:
            size = (*w) * 4;
            if (pitches != NULL)
            {
                pitches[0] = size;
            }
            size *= *h;
            break;
        default:
            LLOGLN(0, ("xrdpVidQueryImageAttributes: Unsupported image"));
            return 0;
    }
    LLOGLN(10, ("xrdpVidQueryImageAttributes: finished size %d id 0x%x", size, id));
    return size;
}

/*****************************************************************************/
Bool
rdpXvInit(ScreenPtr pScreen, ScrnInfoPtr pScrn)
{
    XF86VideoAdaptorPtr adaptor;
    DevUnion* pDevUnion;
    int bytes;

    adaptor = xf86XVAllocateVideoAdaptorRec(pScrn);
    if (adaptor == 0)
    {
        LLOGLN(0, ("rdpXvInit: xf86XVAllocateVideoAdaptorRec failed"));
        return 0;
    }
    adaptor->type = XvInputMask | XvImageMask | XvVideoMask | XvStillMask | XvWindowMask | XvPixmapMask;
    adaptor->flags = VIDEO_CLIP_TO_VIEWPORT;
    adaptor->name = "XORGXRDP XVideo Adaptor";
    adaptor->nEncodings = T_NUM_ENCODINGS;
    adaptor->pEncodings = &(g_xrdpVidEncodings[0]);
    adaptor->nFormats = T_NUM_FORMATS;
    adaptor->pFormats = &(g_xrdpVidFormats[0]);
    adaptor->pFormats[0].depth = pScrn->depth;
    LLOGLN(0, ("rdpXvInit: depth %d", pScrn->depth));
    adaptor->nImages = sizeof(g_xrdpVidImages) / sizeof(XF86ImageRec);
    adaptor->pImages = g_xrdpVidImages;
    adaptor->nAttributes = 0;
    adaptor->pAttributes = 0;
    adaptor->nPorts = T_MAX_PORTS;
    bytes = sizeof(DevUnion) * T_MAX_PORTS;
    pDevUnion = (DevUnion*) g_malloc(bytes, 1);
    adaptor->pPortPrivates = pDevUnion;
    adaptor->PutVideo = xrdpVidPutVideo;
    adaptor->PutStill = xrdpVidPutStill;
    adaptor->GetVideo = xrdpVidGetVideo;
    adaptor->GetStill = xrdpVidGetStill;
    adaptor->StopVideo = xrdpVidStopVideo;
    adaptor->SetPortAttribute = xrdpVidSetPortAttribute;
    adaptor->GetPortAttribute = xrdpVidGetPortAttribute;
    adaptor->QueryBestSize = xrdpVidQueryBestSize;
    adaptor->PutImage = xrdpVidPutImage;
    adaptor->QueryImageAttributes = xrdpVidQueryImageAttributes;
    if (!xf86XVScreenInit(pScreen, &adaptor, 1))
    {
        LLOGLN(0, ("rdpXvInit: xf86XVScreenInit failed"));
        return 0;
    }
    xf86XVFreeVideoAdaptorRec(adaptor);
    return 1;
}
