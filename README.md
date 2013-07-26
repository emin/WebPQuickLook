Quick Look Plugin for WebP Files
================================

###What is this?
  
  Quick Look is a part of Mac OS X. It provides you a quick way
  to look at your content without open it in an application.
  In Finder, choose an file or folder and push the space button, then QuickLook
  mechanism quickly show the preview of the content. And also it is responsible
  for the thumbnail mechanism.

  Most people know this mechanism. By default, OS X doesn't provide preview and
  thumbnail for all file types. WebP is Google's new image format and OS X
  doesn't recognize the .webp files. This plugin will give you to ability to
  see preview and thumbnail of WebP images.


###How can I install it?
  
  This is simple for those of you who are familiar with terminal
  (console).

  Open your terminal app (or whatever you use) and copy paste the below
  commands:

  		curl  https://raw.github.com/emin/WebPQuickLook/master/WebpQuickLook.tar.gz | tar -xvz
		mkdir -p ~/Library/QuickLook/
		mv WebpQuickLook.qlgenerator ~/Library/QuickLook/
		qlmanage -r



