#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "webp/decode.h"

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options);
void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview);

/* -----------------------------------------------------------------------------
   Generate a preview for file

   This function's job is to create preview for designated file
   ----------------------------------------------------------------------------- */

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options)
{
    // To complete your generator please implement the function GeneratePreviewForURL in GeneratePreviewForURL.c
  
    // get posix path and convert it to C string then we can get data from file with simple C operation.
    // Sometimes CoreFoundation is pain in the ass. I don't want to use it to get byte data

    CFStringRef path = CFURLCopyPath(url);
    path = CFURLCreateStringByReplacingPercentEscapes(kCFAllocatorDefault, path, CFSTR(""));
    CFIndex length  = CFStringGetLength(path);
    CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
    
    char* c_path = (char*)malloc(maxSize);
    CFStringGetCString(path, c_path, maxSize, kCFStringEncodingUTF8);
    
    // let 's do the reading and decoding with libwebp
    
    FILE *file = fopen(c_path, "r");
    
    if (file != NULL) {
        
        fseek(file, 0, SEEK_END);
        long int size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        const uint8_t data[size];
        
        fread((void*)&data[0], sizeof(uint8_t), size, file);
        
        fclose(file);
        
        WebPBitstreamFeatures features;
        
        WebPGetFeatures(&data[0], size, &features);
        
        
        int width = 0;
        int height = 0;
        
        uint8_t *rgbData = NULL;
        
        int samples = 3;
        CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
        

            
        if( features.has_alpha ){
            rgbData = WebPDecodeRGBA(&data[0], size, &width, &height);
            samples = 4;
            bitmapInfo = kCGImageAlphaLast;
        }else{
            rgbData = WebPDecodeRGB(&data[0], size, &width, &height);
        }
        
        
        
        CGSize imageSize = CGSizeMake(width, height);
        
        CGContextRef ctx = QLPreviewRequestCreateContext(preview, imageSize, true, options);
        
        CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, rgbData, width*height*samples, NULL);
        
        CGImageRef image =  CGImageCreate(width, height, 8, 8 * samples, width * samples, CGColorSpaceCreateDeviceRGB(), bitmapInfo, provider, NULL, false, kCGRenderingIntentDefault);

        
        CGContextDrawImage(ctx, CGRectMake(0, 0, width, height), image);
        
        CGContextFlush(ctx);
        
        QLPreviewRequestFlushContext(preview, ctx);
        CFRelease(ctx);
        
        free(rgbData);
    }
    

    return noErr;
}

void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview)
{
    // Implement only if supported
}
