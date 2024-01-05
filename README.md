# YUV Player

This tool is designed to play uncompressed YUV video.  
The player GUI relies on Qt library to be compatible with OS like Windows, Unix.  
The code has been designed to keep separate the GUI and the backend, so that migrating the code with another C++ GUI framework should be simple.
The backend relies on C++ standard only to make it easily portable.

## what is supported today
* YUV player for I420 raw video
* zoom x0.5, x2
* add 16x16 white/black grid on video
* display only Y, U or V component
* display as progressive or interlaced fields
* magnifier button to zoom in an image area (x2, x4, x8)

## what will be supported next
* support UYVY format;
* support YUV 422 planar format;
* compare with another YUV file;

## nice to have
What could be improved in future:
* do not use RGB image to do all image processing
* add small toolbox to do extraction, conversion... 

