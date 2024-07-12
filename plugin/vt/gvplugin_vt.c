/// \file
/// \brief Device that renders using ANSI terminal colors

#include <assert.h>
#include <gvc/gvplugin.h>
#include <gvc/gvplugin_device.h>
#include <limits.h>
#include <stddef.h>

#include <gvc/gvio.h>

/// an ANSI color
typedef struct {
  unsigned value;
  unsigned red;
  unsigned green;
  unsigned blue;

} color_t;

/// ANSI 3-bit colors
static const color_t COLORS[] = {
    {0, 0x00, 0x00, 0x00}, ///< black
    {1, 0xff, 0x00, 0x00}, ///< red
    {2, 0x00, 0xff, 0x00}, ///< green
    {3, 0xff, 0xff, 0x00}, ///< yellow
    {4, 0x00, 0x00, 0xff}, ///< blue
    {5, 0xff, 0x00, 0xff}, ///< magenta
    {6, 0x00, 0xff, 0xff}, ///< cyan
    {7, 0xff, 0xff, 0xff}, ///< white
};

/// a metric of “closeness” to a given color
static unsigned distance(const color_t base, unsigned red, unsigned green,
                         unsigned blue) {
  unsigned diff = 0;
  diff += red > base.red ? red - base.red : base.red - red;
  diff += green > base.green ? green - base.green : base.green - green;
  diff += blue > base.blue ? blue - base.blue : base.blue - blue;
  return diff;
}

/// find closest ANSI color
static unsigned get_color(unsigned red, unsigned green, unsigned blue) {
  unsigned winner = 0;
  unsigned diff = UINT_MAX;
  for (size_t i = 0; i < sizeof(COLORS) / sizeof(COLORS[0]); ++i) {
    unsigned d = distance(COLORS[i], red, green, blue);
    if (d < diff) {
      diff = d;
      winner = COLORS[i].value;
    }
  }
  return winner;
}

// number of bytes per pixel
static const unsigned BPP = 4;

static void process(GVJ_t *job, int color_depth) {

  unsigned char *data = (unsigned char *)job->imagedata;

  assert(color_depth == 3 || color_depth == 24);

  for (unsigned y = 0; y < job->height; y += 2) {
    for (unsigned x = 0; x < job->width; ++x) {

      {
        // extract the upper pixel
        unsigned offset = y * job->width * BPP + x * BPP;
        unsigned red = data[offset + 2];
        unsigned green = data[offset + 1];
        unsigned blue = data[offset];

        // use this to select a foreground color
        if (color_depth == 3) {
          unsigned fg = get_color(red, green, blue);
          gvprintf(job, "\033[3%um", fg);
        } else {
          assert(color_depth == 24);
          gvprintf(job, "\033[38;2;%u;%u;%um", red, green, blue);
        }
      }

      {
        // extract the lower pixel
        unsigned red = 0;
        unsigned green = 0;
        unsigned blue = 0;
        if (y + 1 < job->height) {
          unsigned offset = (y + 1) * job->width * BPP + x * BPP;
          red = data[offset + 2];
          green = data[offset + 1];
          blue = data[offset];
        }

        // use this to select a background color
        if (color_depth == 3) {
          unsigned bg = get_color(red, green, blue);
          gvprintf(job, "\033[4%um", bg);
        } else {
          assert(color_depth == 24);
          gvprintf(job, "\033[48;2;%u;%u;%um", red, green, blue);
        }
      }

      // print unicode “upper half block” to effectively do two rows of
      // pixels per one terminal row
      gvprintf(job, "▀\033[0m");
    }
    gvprintf(job, "\n");
  }
}

static void process3(GVJ_t *job) { process(job, 3); }

static void process24(GVJ_t *job) { process(job, 24); }

/// convert an RGB color to grayscale
static unsigned rgb_to_grayscale(unsigned red, unsigned green, unsigned blue) {

  /// use “perceptual” scaling,
  /// https://en.wikipedia.org/wiki/Grayscale#Colorimetric_(perceptual_luminance-preserving)_conversion_to_grayscale

  const double r_linear = red / 255.0;
  const double g_linear = green / 255.0;
  const double b_linear = blue / 255.0;

  const double y_linear =
      0.2126 * r_linear + 0.7152 * g_linear + 0.0722 * b_linear;
  return (unsigned)(y_linear * 255.999);
}

/// draw a 4-pixels-per-character monochrome image
static void process4up(GVJ_t *job) {

  unsigned char *data = (unsigned char *)job->imagedata;

  for (unsigned y = 0; y < job->height; y += 2) {
    for (unsigned x = 0; x < job->width; x += 2) {

      // block characters from the “Amstrad CPC character set”
      const char *tiles[] = {" ", "▘", "▝", "▀", "▖", "▍", "▞", "▛",
                             "▗", "▚", "▐", "▜", "▃", "▙", "▟", "█"};

      unsigned index = 0;

      for (unsigned y_offset = 0; y + y_offset < job->height && y_offset < 2;
           ++y_offset) {
        for (unsigned x_offset = 0; x + x_offset < job->width && x_offset < 2;
             ++x_offset) {

          const unsigned offset =
              (y + y_offset) * job->width * BPP + (x + x_offset) * BPP;
          const unsigned red = data[offset + 2];
          const unsigned green = data[offset + 1];
          const unsigned blue = data[offset];

          const unsigned gray = rgb_to_grayscale(red, green, blue);
          // The [0, 256) grayscale measurement can be quantized into 16
          // 16-stride buckets. I.e. [0, 16) as bucket 1, [16, 32) as bucket 2,
          // … Drawing a threshold at 240, and considering only the last bucket
          // to be white when converting to monochrome empirically seems to
          // generate reasonable results.
          const unsigned pixel = gray >= 240;

          index |= pixel << (y_offset * 2 + x_offset);
        }
      }

      gvputs(job, tiles[index]);
    }
    gvputc(job, '\n');
  }
}

static gvdevice_engine_t engine3 = {
    .format = process3,
};

static gvdevice_engine_t engine24 = {
    .format = process24,
};

static gvdevice_engine_t engine4up = {
    .format = process4up,
};

static gvdevice_features_t device_features = {
    .default_dpi = {96, 96},
};

static gvplugin_installed_t device_types[] = {
    {8, "vt:cairo", 0, &engine3, &device_features},
    {1 << 24, "vt-24bit:cairo", 0, &engine24, &device_features},
    {4, "vt-4up:cairo", 0, &engine4up, &device_features},
    {0},
};

static gvplugin_api_t apis[] = {
    {API_device, device_types},
    {(api_t)0, 0},
};

#ifdef GVDLL
#define GVPLUGIN_VT_API __declspec(dllexport)
#else
#define GVPLUGIN_VT_API
#endif

GVPLUGIN_VT_API gvplugin_library_t gvplugin_vt_LTX_library = {"vt", apis};
