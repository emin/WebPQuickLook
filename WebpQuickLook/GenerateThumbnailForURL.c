#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "webp/decode.h"

OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options, CGSize maxSize);
void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail);

/* -----------------------------------------------------------------------------
    Generate a thumbnail for file

   This function's job is to create thumbnail for designated file as fast as possible
   ----------------------------------------------------------------------------- */

OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options, CGSize maxSize)
{
   
    // get posix path and convert it to C string then we can get data from file with simple C operation.
    // Sometimes CoreFoundation is pain in the ass. I don't want to use it to get byte data
    
    CFStringRef path = CFURLCopyPath(url);
    path = CFURLCreateStringByReplacingPercentEscapes(kCFAllocatorDefault, path, CFSTR(""));
    CFIndex length  = CFStringGetLength(path);
    CFIndex max = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
    
    char* c_path = (char*)malloc(max);
    CFStringGetCString(path, c_path, max, kCFStringEncodingUTF8);
    
    // let 's do the reading and decoding with libwebp
    
    FILE *file = fopen(c_path, "r");
    
    if (file != NULL) {
        
        fseek(file, 0, SEEK_END);
        long int size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        const uint8_t data[size];
        
        fread((void*)(&data[0]), sizeof(uint8_t), size, file);
        
        fclose(file);
        

        int width = (int)maxSize.width;
        int height = (int)maxSize.height;
        int image_w, image_h;
        
        WebPGetInfo(&data[0], size, &image_w, &image_h);
        
        
        if( image_w > image_h )
        {
            width = (int)maxSize.width;
            height = (int)( width * image_h/image_w );
        }
        else
        {
            height = (int)maxSize.height;
            width = (int)( height * image_w/image_h );
        }
            
        
        // for scaled size decoding
        
        WebPDecoderConfig config;
        WebPInitDecoderConfig(&config);
        
        config.options.use_scaling = 1;
        config.options.scaled_width = width;
        config.options.scaled_height = height;
        
        config.output.colorspace = MODE_RGBA;
        WebPDecode(&data[0], size, &config);
        
        
        
        uint8_t *rgbData = config.output.u.RGBA.rgba; //WebPDecodeRGB( &data[0], size, &width, &height);
        
        CGSize imageSize = CGSizeMake(width, height);
        
        CGContextRef ctx = QLThumbnailRequestCreateContext(thumbnail, imageSize, true, options);
        
        CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, rgbData, width*height*4, NULL);
        
        CGImageRef image =  CGImageCreate(width, height, 8, 32, width * 4, CGColorSpaceCreateDeviceRGB(), kCGImageAlphaLast, provider, NULL, false, kCGRenderingIntentDefault);
        
        
        CGContextDrawImage(ctx, CGRectMake(0, 0, width, height), image);
        
        CGContextFlush(ctx);
        
        QLThumbnailRequestFlushContext(thumbnail, ctx);
        CFRelease(ctx);
        
        WebPFreeDecBuffer(&config.output);
    }
    

    
    
    
    
    return noErr;
}

void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail)
{
    // Implement only if supported
}
