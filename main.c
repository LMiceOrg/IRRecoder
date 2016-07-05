#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#include <Shlwapi.h>
#elif  defined(__linux__)
#include <sys/types.h>
#include <unistd.h>
#endif

#include "XCamera.h" // Xeneth SDK main header.
//#include "XFilters.h" // Xeneth SDK main header.
FilterID IMPEXPC XC_FLT_Queue         (XCHANDLE hnd, const char *fltname, const char *fltparms);  ///< @sa QueueFilter

XCHANDLE connect_cam();
void startsnap_cam(double fps);
void cleanup_cam(XCHANDLE handle);

void save_tif(int count, char* buffer, int width, int height, int bpp) {
    const unsigned char tiff_head[] = { /* 8 bytes-long */
            0x49, 0x49, 0x2a, 0x00,/* Tiff head mask */
            0x08, 0x80, 0x02, 0x00 /* tiff data length + head 163840+8*/
        };
    const unsigned char tiff_tail[] = {/* 138 bytes-long */
        0x0b , 0x00 ,
        0x00 , 0x01 , 0x03 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x40 , 0x01 , 0x00 , 0x00 ,
        0x01 , 0x01 , 0x03 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 , 0x00 , 0x00 ,
        0x02 , 0x01 , 0x03 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x10 , 0x00 , 0x00 , 0x00 ,
        0x03 , 0x01 , 0x03 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 ,
        0x06 , 0x01 , 0x03 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 ,
        0x0a , 0x01 , 0x03 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 ,
        0x11 , 0x01 , 0x04 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x08 , 0x00 , 0x00 , 0x00 ,
        0x15 , 0x01 , 0x03 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 ,
        0x16 , 0x01 , 0x03 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x00 , 0x01 , 0x00 , 0x00 ,
        0x17 , 0x01 , 0x04 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x00 , 0x80 , 0x02 , 0x00 ,
        0x1c , 0x01 , 0x03 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 , 0x01 , 0x00 , 0x00 , 0x00 ,
        0x00 , 0x00 , 0x00 , 0x00
    };
    FILE* fp;
    char name[64]={0};
    sprintf_s(name, 64, "img_ir_%06d.tiff", count);
    fp = fopen(name, "wb");
    fwrite(tiff_head, 1, 8, fp);
    fwrite(buffer, 1, width * height * bpp/8, fp);
    fwrite(tiff_tail, 1, 138, fp);
    fclose(fp);
}

//#include <tiffio.h>
//void save_img(int count, char* buffer, int width, int height, int bpp);
//void save_img(int count, char* buffer, int width, int height, int bpp) {
//    TIFF *image;
//    char name[64]={0};
//    sprintf_s(name, 64, "img_ir_%06d.tiff", count);


//    // Open the TIFF file
//    if((image = TIFFOpen(name, "w")) == NULL){
//        printf("Could not open output.tif for writing\n");
//        return;
//    }
//    // We need to set some values for basic tags before we can add any data
//    TIFFSetField(image, TIFFTAG_IMAGEWIDTH, width );
//    TIFFSetField(image, TIFFTAG_IMAGELENGTH, height);
//    TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, bpp);
//    TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
//    TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, height);
//    TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
//    //TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
//    //TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);
//    TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
//    TIFFSetField(image, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
//    TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
//    //  TIFFSetField(image, TIFFTAG_XRESOLUTION, 30.0);
//    //  TIFFSetField(image, TIFFTAG_YRESOLUTION, 30.0);
//    //  TIFFSetField(image, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

//    // Write the information to the file
//    TIFFWriteEncodedStrip(image, 0, (char*)buffer, width * height *bpp/8);
//    //    for (j = 0; j < 144; j++)
//    //        TIFFWriteScanline(image, &buffer[j * 25], j, 0);
//    // Close the file
//    TIFFClose(image);
//}

XCHANDLE connect_cam() {
    XCHANDLE handle = 0; // Handle to the camera
    ErrCode ec = 0;
    do {
        handle = XC_OpenCamera("cam://0", 0, 0);
        if(XC_IsInitialised(handle))
        {

            // ... start capturing
            printf("Start capturing.\n");
            if (ec = XC_StartCapture(handle) != I_OK)
            {
                printf("Could not start capturing, errorCode: %u\n", ec);
            }
            else if (XC_IsCapturing(handle)) // When the camera is capturing ...
            {
                break;
            }
        }
        else
        {
            printf("Initialization failed\n");
        }

        // Cleanup.
        cleanup_cam(handle);

        Sleep(500);

    } while(handle);

    return handle;
}

void cleanup_cam(XCHANDLE handle) {
    ErrCode ec = 0;
    // Cleanup.

    // When the camera is still capturing, ...
    if(XC_IsCapturing(handle))
    {
        // ... stop capturing.
        printf("Stop capturing.\n");
        if (ec = XC_StopCapture(handle) != I_OK)
        {
            printf("Could not stop capturing, errorCode: %u\n", ec);
        }
    }

    // When the handle to the camera is still initialised ...
    if (XC_IsInitialised(handle))
    {
        printf("Closing connection to camera.\n");
        XC_CloseCamera(handle);
    }

}

int main(void)
{
    // Variables
    XCHANDLE handle = 0; // Handle to the camera
    ErrCode errorCode = 0; // Used to store returned errorCodes from the SDK functions.
    word *frameBuffer = 0; // 16-bit buffer to store the capture frame.
    dword frameSize = 0; // The size in bytes of the raw image.
    FilterID fltSoftwareCorrection = 0;

    // Open a connection to the first detected camera by using connection string cam://0
    printf("Opening connection to cam://0\n");
    handle = XC_OpenCamera("cam://0?fg=XFrameGrabberNative", 0, 0);

    // When the connection is initialised, ...
    if(XC_IsInitialised(handle))
    {
        char buff[512]={0};
        GetModuleFileNameA(NULL, buff, 512);
        PathRemoveFileSpecA(buff);
        PathAppendA(buff, "bobcat3746-2ms-HG_3746.xca");

        if (I_OK == XC_LoadCalibration(handle, buff, 0))
        {
            fltSoftwareCorrection = XC_FLT_Queue(handle, "SoftwareCorrection", 0);

            if (fltSoftwareCorrection > 0)
            {
                printf("Software Correction initialised.\n");
            }

            /* The softwarecorrection is now initialised and started, reference the SoftwareCorrection
                    documentation for information about the differentparameter settings.
                    Also reference the image filter samples for working with image filters. */
        }
        else
        {
            printf("Failed loading the calibration pack.\n");
        }

        // ... start capturing
        printf("Start capturing.\n");
        if (errorCode = XC_StartCapture(handle) != I_OK)
        {
            printf("Could not start capturing, errorCode: %u\n", errorCode);
        }
        else if (XC_IsCapturing(handle)) // When the camera is capturing ...
        {
            int count =0;
            // Determine native framesize.
            frameSize = XC_GetFrameSize(handle);
            int width= XC_GetWidth(handle);
            int height = XC_GetHeight(handle);
            FrameType type = XC_GetFrameType(handle);
            int bpp = 16;
            switch(type) {
            case FT_8_BPP_GRAY:
                bpp = 8;
                break;
            case FT_16_BPP_GRAY:
                bpp = 16;
                break;
            case FT_32_BPP_GRAY:
                bpp = 32;
                break;
            case FT_32_BPP_RGBA:
                bpp = 32;
                break;
            case FT_32_BPP_RGB:
            case FT_32_BPP_BGRA:
            case FT_32_BPP_BGR:
                bpp = 32;
                break;
            default:
                bpp = 8;
                break;

            }

            // Initialize the 16-bit buffer.
            frameBuffer = (word*)malloc(sizeof(word) * (frameSize / 2));

            do {
                // ... grab a frame from the camera.
                printf("Grabbing a frame %lf  %u.  %d  %d\n", XC_GetFrameRate(handle), frameSize, width, height);
                if (errorCode = XC_GetFrame(handle, FT_NATIVE, XGF_Blocking, frameBuffer, frameSize) != I_OK)
                {
                    //   printf("Problem while fetching frame, errorCode %u", errorCode);
                }
                printf("bpp = %d\n", bpp);
                save_tif(count, (char*)frameBuffer, width, height, bpp );
                //XC_SaveData(handle, name, 0);
                Sleep(50);

                count ++;
            } while(1);
        }
    }
    else
    {
        printf("Initialization failed\n");
    }

    // Cleanup.

    // When the camera is still capturing, ...
    if(XC_IsCapturing(handle))
    {
        // ... stop capturing.
        printf("Stop capturing.\n");
        if (errorCode = XC_StopCapture(handle) != I_OK)
        {
            printf("Could not stop capturing, errorCode: %u\n", errorCode);
        }
    }

    // When the handle to the camera is still initialised ...
    if (XC_IsInitialised(handle))
    {
        printf("Closing connection to camera.\n");
        XC_CloseCamera(handle);
    }

    printf("Clearing buffers.\n");
    if (frameBuffer != 0)
    {
        free(frameBuffer);
        frameBuffer = 0;
    }

    return 0;
}

