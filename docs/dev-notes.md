

Decided that (0,0) is top-left, with +Y going down, to follow OIIO/Halide/OpenCV etc, despite Nuke being bottom-left

To get info from an image:
oiiotool --info -v -a ./tests/fixtures/images/openexr/TestImages/stripes.exr 

Need to decide how to treat subimages from tiffs, since apparently they dont show up in oiio:subimages, needing a repeated seek


---
# next stuff to fix, as per codex's review


The deep dive found three real issues:

1. **Unsafe allocation math** — pixel count is multiplied as signed `int`. Large or malformed dimensions can overflow before `vector::resize()`. Volumetric images are worse: `spec.depth` is ignored, so the buffer may be undersized.

2. **Deep EXR is silently misrepresented** — `v2/Stereo/Balls.exr` opens and returns a flat `PixelBlock`, but deep pixels require variable sample counts and `DeepData`; ordinary `read_image()` is not the correct API. [OpenImageIO documentation](https://openimageio.readthedocs.io/en/v3.1.15.0/imageinput.html#reading-deep-data)

3. **Read errors can disappear** — with `png/broken/invalid_gray_alpha_sbit.png`, one layer fails with an error, then another returns an empty block while clearing `reader.error()`.

I’d fix these before adding more broad format coverage:

- Reject deep and `depth != 1` inputs explicitly.
- Validate positive dimensions and use checked `size_t` multiplication.
- Guarantee every failed `read()` leaves a nonempty error.
- Add regression tests using the deep EXR and broken PNG fixtures.
- Add `ScanLines/Blobbies.exr` for mixed channel types plus overscan.

The remaining oddities—subsampled chroma rejection, multiframe handling, and `Y+A` splitting into separate layers—look like scope/API decisions rather than immediate defects.
---