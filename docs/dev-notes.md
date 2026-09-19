

Decided that (0,0) is top-left, with +Y going down, to follow OIIO/Halide/OpenCV etc, despite Nuke being bottom-left

To get info from an image:
oiiotool --info -v -a ./tests/fixtures/images/openexr/TestImages/stripes.exr 

Need to decide how to treat subimages from tiffs, since apparently they dont show up in oiio:subimages, needing a repeated seek