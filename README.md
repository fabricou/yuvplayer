# YUV Player

This tool is a player for uncompressed YUV video.  
The player relies on Qt library to be compatible with OS like Windows, Unix.  
However the code has been designed to keep separate the GUI and the backend, so that migrating the code with another C++ GUI framework should be simple.
The backend relies on C++ standard only, for the same purpose.

## what is supported today
* YUV player
* zoom x0.5, x2
* add 16x16 white/black grid on video
* display only Y, U or V component
* display as progressive or interlaced fields
* magnifier button to zoom in an image area (x2, x4, x8)

## what will be supported next
* improve YUV I420 support for interlaced cases;
* support UYVY format;
* support YUV 422 planar format;
* compare with another YUV file;

## nice to have
What could be improved in future:
* do not use RGB image to do all image processing
* add small toolbox to do extraction, conversion... 

## Second Level Heading

Paragraph.

- bullet
+ other bullet
* another bullet
    * child bullet

1. ordered
2. next ordered

### Third Level Heading

Some *italic* and **bold** text and `inline code`.

An empty line starts a new paragraph.

Use two spaces at the end  
to force a line break.

A horizontal ruler follows:

---

Add links inline like [this link to the Qt homepage](https://www.qt.io),
or with a reference like [this other link to the Qt homepage][1].

    Add code blocks with
    four spaces at the front.

> A blockquote
> starts with >
>
> and has the same paragraph rules as normal text.

First Level Heading in Alternate Style
======================================

Paragraph.

Second Level Heading in Alternate Style
---------------------------------------

Paragraph.

[1]: https://www.qt.io
