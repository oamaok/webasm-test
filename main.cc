#include <stdint.h>
#include <math.h>

#include "hsv.h"

extern "C" {

struct complex {
  double real, imag;

  struct complex add(struct complex b) {
    return {
      real + b.real,
      imag + b.imag,
    };
  }

  struct complex sub(struct complex b) {
    return {
      real - b.real,
      imag - b.imag
    };
  }

  struct complex mul(struct complex b) {
    return {
      real * b.real - imag * b.imag,
      real * b.imag + imag * b.real,      
    };
  }

  struct complex div(struct complex b) {
    double d = 1.0 / (b.imag * b.imag + b.real * b.real);

    return {
      d * (real * b.real + imag * b.imag),
      d * (imag * b.real - real * b.imag),
    };
  }

  struct complex pow(struct complex b) {
    double arg = atan2(imag, real);
    double loh = log(real / cos(arg));

    double c = b.imag * loh + b.real * arg;
    double d = exp(b.real * loh - b.imag * arg);

    return {
      d * cos(c),
      d * sin(c)
    };
  }
 
  struct complex powN(int n) {
    struct complex z = { real, imag };
    struct complex ret = z;
    for (int i = 1; i < n; ++i) {
      ret = ret.mul(z);
    }

    return ret;
  }

  double abssq() {
    return real * real + imag * imag;
  }
};

struct complex re(double r) {
  return { r, 0.0 };
}

struct result {
  int iteration, root;
};


float hue(float p, float q, float t) {
  if(t < 0.0f) t += 1.0f;
  if(t > 1.0f) t -= 1.0f;
  if(t < 0.166666f) return p + (q - p) * 6.0f * t;
  if(t < 0.5f) return q;
  if(t < 0.666666f) return p + (q - p) * (0.666666f - t) * 6.0f;
  return p;
}

void hslToRgb(float h, float s, float l, uint8_t* rgb) {
    h = fmod(h, 1.0);

    float r, g, b;

    if (s < 0.001f) {
      r = g = b = l;
    } else {
      float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
      float p = 2.0f * l - q;
      r = hue(p, q, h + 0.333333f);
      g = hue(p, q, h);
      b = hue(p, q, h - 0.333333f);
    }


    rgb[0] = r * 255.0f;
    rgb[1] = g * 255.0f;
    rgb[2] = b * 255.0f;
}

struct result calculate(struct complex z, double a) {
  struct complex roots[] = {
    { 0.9229299, 0 },
    { -0.9229299, 0 },
  };

  for (int i = 0; i < 2000; ++i) {
    struct complex p = z
      .powN(6)
      .mul(re(1.61803))
      .sub(re(1.0));

    struct complex pp = z
      .powN(5).mul(re(9.708203));

    struct complex dz = z.sub(p.div(pp).mul(re(a)));

    for (int r = 0; r < sizeof(roots) / sizeof(roots[0]); ++r) {
      if (dz.sub(roots[r]).abssq() < 0.05) {
        return { i, r };
      }
    }

    z = dz;
  }

  return { 0, 0 };
}

void render(int width, int height, double a, uint8_t* buf) {
  struct result result;

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      double zi = ((double)y - (double)width * 0.5) * 0.01;
      double zr = ((double)x - (double)height * 0.5) * 0.01;

      result = calculate((struct complex){ zi, zr }, a);

      int idx = (y * width + x) * 4;

      RgbColor* color = (RgbColor*)(buf + idx);

      *color = HsvToRgb((HsvColor){ (result.root * 100) % 255, 255 - result.iteration * 6, result.iteration * 12 });

      // Manually set alpha
      buf[idx + 3] = 255;
    }
  }
}

}