#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "webp/decode.h"
#include "webp/demux.h"

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
        
        uint8_t *data = malloc(size);
        
        fread(data, sizeof(uint8_t), size, file);
        
        fclose(file);
        
        
        WebPBitstreamFeatures features;
        WebPGetFeatures(data, size, &features);
        int width = (int)maxSize.width;
        int height = (int)maxSize.height;
        int image_w, image_h;
        
        WebPGetInfo(data, size, &image_w, &image_h);
        
        
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
                    
                    WebPDecode(iter.fragment.bytes, size, &config);
                    
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
            WebPDecode(data, size, &config);
        }
        
        
        
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
        
        free(data);
    }
    
    
    
    
    
    return noErr;
}

void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail)
{
    // Implement only if supported
}
