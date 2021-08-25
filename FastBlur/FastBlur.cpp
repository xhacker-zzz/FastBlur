// FastBlur.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include "FastBlur.h"
#include "EasyBMP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

#define COLOR_RED 1
#define COLOR_GREEN 2
#define COLOR_BLUE 3
#define COLOR_PURPLE 4
#define COLOR_YELLOW 5
#define COLOR_CYAN 6
#define COLOR_TRANSPARENT 0

// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

void Blur(BMP& bitmap, short radius, bool cmp)
{
    if (radius < 1)
    {
        return;
    }
    int w = bitmap.TellWidth();
    int h = bitmap.TellHeight();
    int* pix = new int[w * h];
    int* oripix = new int[w * h];
    for (size_t i = 0; i < w * h - 1; i++)
    {
        RGBApixel p = bitmap.GetPixel(i % w, i / w);
        pix[i] = (((p.Red << 8) | p.Green) << 8) | p.Blue;
        oripix[i] = pix[i];
    }
    int wm = w - 1;
    int hm = h - 1;
    int wh = w * h;
    int div = radius + radius + 1;
    int* r = new int[wh];
    int* g = new int[wh];
    int* b = new int[wh];
    int rsum, gsum, bsum, x, y, i, p, yp, yi, yw;
    int* vmin = new int[max(w, h)];
    int divsum = (div + 1) >> 1;
    divsum *= divsum;
    int temp = 256 * divsum;
    int* dv = new int[temp];
    for (i = 0; i < temp; i++)
    {
        dv[i] = (i / divsum);
    }
    yw = yi = 0;
    int** stack = new int* [div];
    for (size_t i = 0; i < div; i++)
    {
        stack[i] = new int[3];
    }
    int stackpointer;
    int stackstart;
    int* sir;
    int rbs;
    int r1 = radius + 1;
    int routsum, goutsum, boutsum;
    int rinsum, ginsum, binsum;
    for (y = 0; y < h; y++)
    {
        rinsum = ginsum = binsum = routsum = goutsum = boutsum = rsum = gsum = bsum = 0;
        for (i = -radius; i <= radius; i++)
        {
            p = pix[yi + min(wm, max(i, 0))];
            sir = stack[i + radius];
            sir[0] = (p & 0xff0000) >> 16;
            sir[1] = (p & 0x00ff00) >> 8;
            sir[2] = (p & 0x0000ff);
            rbs = r1 - abs(i);
            rsum += sir[0] * rbs;
            gsum += sir[1] * rbs;
            bsum += sir[2] * rbs;
            if (i > 0)
            {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
            }
            else
            {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
            }
        }
        stackpointer = radius;
        for (x = 0; x < w; x++)
        {
            r[yi] = dv[rsum];
            g[yi] = dv[gsum];
            b[yi] = dv[bsum];
            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;
            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];
            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];
            if (y == 0)
            {
                vmin[x] = min(x + radius + 1, wm);
            }
            p = pix[yw + vmin[x]];
            sir[0] = (p & 0xff0000) >> 16;
            sir[1] = (p & 0x00ff00) >> 8;
            sir[2] = (p & 0x0000ff);
            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];
            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;
            stackpointer = (stackpointer + 1) % div;
            sir = stack[(stackpointer) % div];
            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];
            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];
            yi++;
        }
        yw += w;
    }
    for (x = 0; x < w; x++)
    {
        rinsum = ginsum = binsum = routsum = goutsum = boutsum = rsum = gsum = bsum = 0;
        yp = -radius * w;
        for (i = -radius; i <= radius; i++)
        {
            yi = max(0, yp) + x;
            sir = stack[i + radius];
            sir[0] = r[yi];
            sir[1] = g[yi];
            sir[2] = b[yi];
            rbs = r1 - abs(i);
            rsum += r[yi] * rbs;
            gsum += g[yi] * rbs;
            bsum += b[yi] * rbs;
            if (i > 0)
            {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
            }
            else
            {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
            }
            if (i < hm)
            {
                yp += w;
            }
        }
        yi = x;
        stackpointer = radius;
        for (y = 0; y < h; y++)
        {
            pix[yi] = (0xff000000 & pix[yi]) | (dv[rsum] << 16) | (dv[gsum] << 8) | dv[bsum];
            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;
            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];
            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];
            if (x == 0)
            {
                vmin[y] = min(y + r1, hm) * w;
            }
            p = x + vmin[y];
            sir[0] = r[p];
            sir[1] = g[p];
            sir[2] = b[p];
            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];
            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;
            stackpointer = (stackpointer + 1) % div;
            sir = stack[stackpointer];
            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];
            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];
            yi += w;
        }
    }
    int cmpmode = -1;
    if (cmp == true)
    {
        srand(time(0));
        cmpmode = rand() % 8;
    }
    for (size_t i = 0; i < w * h - 1; i++)
    {
        RGBApixel p;
        if (cmp == true)
        {
            switch (cmpmode)
            {
            case 0:
                if ((i % w) <= (w / 2))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 1:
                if ((i % w) > (w / 2))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 2:
                if ((i / w) <= (h / 2))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 3:
                if ((i / w) > (h / 2))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 4:
                if (((i % w) <= (w / 2) && (i / w) <= (h / 2)) || ((i % w) > (w / 2) && (i / w) > (h / 2)))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 5:
                if (!(((i % w) <= (w / 2) && (i / w) <= (h / 2)) || ((i % w) > (w / 2) && (i / w) > (h / 2))))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 6:
                if (((i % w) <= (w / 3) && ((i / w) <= (h / 3) || (i / w) > (h * 2 / 3))) ||
                    ((i % w) > (w * 2 / 3) && ((i / w) <= (h / 3) || (i / w) > (h * 2 / 3))) ||
                    ((w / 3) < (i % w) && (i % w) <= (w * 2 / 3) && (h / 3) < (i / w) && (i / w) <= (h * 2 / 3)))
                {
                    pix[i] = oripix[i];
                }
                break;
            case 7:
                if (!(((i % w) <= (w / 3) && ((i / w) <= (h / 3) || (i / w) > (h * 2 / 3))) ||
                    ((i % w) > (w * 2 / 3) && ((i / w) <= (h / 3) || (i / w) > (h * 2 / 3))) ||
                    ((w / 3) < (i % w) && (i % w) <= (w * 2 / 3) && (h / 3) < (i / w) && (i / w) <= (h * 2 / 3))))
                {
                    pix[i] = oripix[i];
                }
                break;
            default:
                break;
            }
        }
        p.Red = pix[i] >> 16;
        p.Green = (pix[i] - (p.Red << 16)) >> 8;
        p.Blue = pix[i] - (p.Red << 16) - (p.Green << 8);
        p.Alpha = 0x0;
        bitmap.SetPixel(i % w, i / w, p);
    }
    return;
}

void FilterColor(BMP& bitmap, short colorFilter)
{
    if (0 >= colorFilter || colorFilter > 6)
    {
        return;
    }
    int w = bitmap.TellWidth();
    int h = bitmap.TellHeight();
    for (size_t i = 0; i < w * h - 1; i++)
    {
        RGBApixel p = bitmap.GetPixel(i % w, i / w);
        if (colorFilter == COLOR_RED || colorFilter == COLOR_PURPLE || colorFilter == COLOR_YELLOW)
        {
            p.Red = 0;
        }
        if (colorFilter == COLOR_GREEN || colorFilter == COLOR_YELLOW || colorFilter == COLOR_CYAN)
        {
            p.Green = 0;
        }
        if (colorFilter == COLOR_BLUE || colorFilter == COLOR_PURPLE || colorFilter == COLOR_CYAN)
        {
            p.Blue = 0;
        }
        p.Alpha = 0x0;
        bitmap.SetPixel(i % w, i / w, p);
    }
    return;
}

void Brighten(BMP& bitmap, short brighten)
{
    if (-255 > brighten || brighten > 255)
    {
        return;
    }
    int w = bitmap.TellWidth();
    int h = bitmap.TellHeight();
    for (size_t i = 0; i < w * h - 1; i++)
    {
        RGBApixel p = bitmap.GetPixel(i % w, i / w);
        int tRGB = 0;
        tRGB = p.Red + brighten;
        if (tRGB > 255)
        {
            p.Red = 255;
        }
        else
        {
            if (tRGB < 0)
            {
                p.Red = 0;
            }
            else
            {
                p.Red = tRGB;
            }
        }
        tRGB = p.Green + brighten;
        if (tRGB > 255)
        {
            p.Green = 255;
        }
        else
        {
            if (tRGB < 0)
            {
                p.Green = 0;
            }
            else
            {
                p.Green = tRGB;
            }
        }
        tRGB = p.Blue + brighten;
        if (tRGB > 255)
        {
            p.Blue = 255;
        }
        else
        {
            if (tRGB < 0)
            {
                p.Blue = 0;
            }
            else
            {
                p.Blue = tRGB;
            }
        }
        p.Alpha = 0x0;
        bitmap.SetPixel(i % w, i / w, p);
    }
    return;
}

void Binarization(BMP& bitmap, int threshold, short probability)
{
    probability = 10001 - probability;
    if (0 > threshold || threshold > 0xffffff || probability <= 0 || probability > 10000)
    {
        return;
    }
    int w = bitmap.TellWidth();
    int h = bitmap.TellHeight();
    srand(time(0));
    for (size_t i = 0; i < w * h - 1; i++)
    {
        RGBApixel p = bitmap.GetPixel(i % w, i / w);
        int pix = (((p.Red << 8) | p.Green) << 8) | p.Blue;
        if (pix >= threshold)
        {
            if (rand() % probability == 0)
            {
                p.Red = 0;
                p.Green = 0;
                p.Blue = 0;
            }
            else
            {
                p.Red = 255;
                p.Green = 255;
                p.Blue = 255;
            }
        }
        else
        {
            if (rand() % probability == 0)
            {
                p.Red = 255;
                p.Green = 255;
                p.Blue = 255;
            }
            else
            {
                p.Red = 0;
                p.Green = 0;
                p.Blue = 0;
            }
        }
        bitmap.SetPixel(i % w, i / w, p);
    }
    return;
}

void Grayed(BMP& bitmap, double gamma)
{
    int w = bitmap.TellWidth();
    int h = bitmap.TellHeight();
    for (size_t i = 0; i < w * h - 1; i++)
    {
        RGBApixel p = bitmap.GetPixel(i % w, i / w);
        short gray = pow((pow(p.Red, gamma) * 0.2973 + pow(p.Green, gamma) * 0.6274 + pow(p.Blue, gamma) * 0.0753), (1 / gamma));
        p.Red = gray;
        p.Green = gray;
        p.Blue = gray;
        bitmap.SetPixel(i % w, i / w, p);
    }
    return;
}

void process(char* input, char* output)
{
    BMP bitmap;
    if (!bitmap.ReadFromFile(input))
        return;
    short blurRadius = 0;
    short colorFilter = 0;
    short brighten = 0;
    int binaryThreshold = 0;
    short probability = 0;
    double gamma = 0;
    //cout << "输入虚化半径：" << endl;
    //cin >> blurRadius;
    //cout << "输入颜色筛选器（0~6）：" << endl;
    //cin >> colorFilter;
    //cout << "输入颜色增量（-255~255）：" << endl;
    //cin >> brighten;
    //cout << "输入二值化阈值（0~0xFFFFFF）：" << endl;
    //cin >> binaryThreshold;
    //cout << "输入反色概率（1~10000）：" << endl;
    //cin >> probability;
    cout << "输入Gamma（建议值 2.2）：" << endl;
    cin >> gamma;
    //Blur(bitmap, blurRadius, true);
    //FilterColor(bitmap, colorFilter);
    //Brighten(bitmap, brighten);
    //Binarization(bitmap, binaryThreshold, probability);
    Grayed(bitmap, gamma);
    bitmap.WriteToFile(output);
    return;
}

int main(int argc, char** argv)
{
    char* input = new char[MAX_PATH];
    if (argc == 1)
    {
        cout << "输入文件路径或拖拽文件至此。" << endl;
        cin >> input;
    }
    else
        strcpy(input, argv[1]);

    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // 初始化 MFC 并在失败时显示错误
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 在此处为应用程序的行为编写代码。
            wprintf(L"错误: MFC 初始化失败\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: 在此处为应用程序的行为编写代码。
        }
    }
    else
    {
        // TODO: 更改错误代码以符合需要
        wprintf(L"错误: GetModuleHandle 失败\n");
        nRetCode = 1;
    }

    char* output = new char[MAX_PATH];
    strcpy(output, input);
    strcat(output, ".Blurred.bmp");
    process(input, output);
    ShellExecuteA(0, "open", output, "", "", SW_SHOW);
    return nRetCode;
}
