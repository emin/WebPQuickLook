Quick Look Plugin for WebP Files
================================

###What is this?
  
  Quick Look is a part of Mac OS X. It provides you a quick way
  to look at your content without open it in an application.
  In Finder, choose a file or folder and push the space button, then QuickLook
  mechanism quickly show the preview of the content. And also it is responsible
  for the thumbnail creation.

  Most people know this mechanism. By default, OS X doesn't provide preview and
  thumbnail for all file types. WebP is Google's new image format and OS X
  doesn't recognize the .webp files. This plugin will give you an ability to
  see previews and thumbnails of WebP images.

###How does it look like?

  Just like an ordinary image file:
  
  ![quicklook webp](https://raw.github.com/emin/WebPQuickLook/master/screenshot.png 'WebP')


###How can I install it?
  
  This is simple for those of you who are familiar with terminal
  (console).

  Open your terminal app (or whatever you use) and copy paste the below
  commands:

  		curl  https://raw.github.com/emin/WebPQuickLook/master/WebpQuickLook.tar.gz | tar -xvz
		mkdir -p ~/Library/QuickLook/
		mv WebpQuickLook.qlgenerator ~/Library/QuickLook/
		qlmanage -r
  
  That's it. You just installed it. Enjoy your WebP files just like ordinary
  image files.

###The other users can't use it. What can I do?

  You can do it same operations by logining their account. But if you want
  to install for all users, that's simple and similar the above commands. I
  have to delete one character (~) from above commands. As most of you quickly
  figured out, 

  		curl  https://raw.github.com/emin/WebPQuickLook/master/WebpQuickLook.tar.gz | tar -xvz
		mkdir -p /Library/QuickLook/
		mv WebpQuickLook.qlgenerator /Library/QuickLook/
		qlmanage -r

  This probably require your administration, if you are not administrator you
  can't install it.


