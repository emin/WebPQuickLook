#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "webp/decode.h"
#include "webp/demux.h"

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
        
        uint8_t *data = malloc(size);
        
        fread(data, sizeof(uint8_t), size, file);
        
        fclose(file);
        
        WebPBitstreamFeatures features;
        
        WebPGetFeatures(data, size, &features);
        
        
        int width = 0;
        int height = 0;
        
        uint8_t *rgbData = NULL;
        
        int samples = 3;
        CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
        
        if ( features.has_animation ) {
            
            // it has animation we need to use demux
            WebPData webpData;
            webpData.bytes = data;
            webpData.size = size;
            
            WebPDemuxer *demux = WebPDemux(&webpData);
            
            //            uint32_t width = WebPDemuxGetI(demux, WEBP_FF_CANVAS_WIDTH);
            //            uint32_t height = WebPDemuxGetI(demux, WEBP_FF_CANVAS_HEIGHT);
            //            // ... (Get information about the features present in the WebP file).
            //            uint32_t flags = WebPDemuxGetI(demux, WEBP_FF_FORMAT_FLAGS);
            
            
            // ... (Iterate over all frames).
            WebPIterator iter;
            if (WebPDemuxGetFrame(demux, 1, &iter)) {
                
                do {
                    
                    if( features.has_alpha ){
                        
                        rgbData = WebPDecodeRGBA(iter.fragment.bytes, size, &width, &height);
                        samples = 4;
                        bitmapInfo = kCGImageAlphaLast;
                        
                    }else{
                        rgbData = WebPDecodeRGB(iter.fragment.bytes, size, &width, &height);
                        samples = 3;
                        bitmapInfo = kCGBitmapByteOrderDefault;
                    }
                    
                    // for now break it, it only show first frame
                    break;
                    
                    // ... (Consume 'iter'; e.g. Decode 'iter.fragment' with WebPDecode(),
                    // ... and get other frame properties like width, height, offsets etc.
                    // ... see 'struct WebPIterator' below for more info).
                } while (WebPDemuxNextFrame(&iter));
                WebPDemuxReleaseIterator(&iter);
            }

    }
    else
    {
        
    
        if( features.has_alpha ){
            rgbData = WebPDecodeRGBA(data, size, &width, &height);
            samples = 4;
            bitmapInfo = kCGImageAlphaLast;
        }else{
            rgbData = WebPDecodeRGB(data, size, &width, &height);
        }
        
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
