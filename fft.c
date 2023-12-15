#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

float pi;

void fft(float in[], size_t stride, float complex out[], size_t n) {
  assert(n > 0);

  if (n == 1) {
    out[0] = in[0];
    return;
  }

  fft(in, stride * 2, out, n / 2);
  fft(in + stride, stride * 2, out + n / 2, n / 2);

  for (size_t k = 0; k < n / 2; ++k) {
    float t = (float)k / n;
    float complex calc = cexp(-2 * I * pi * t) * out[k + n / 2];
    float complex prev = out[k];
    out[k] = prev + calc;
    out[k + n / 2] = prev - calc;
  }
}

int main(void) {
  pi = atan2f(1, 1) * 4;

  size_t n = 3200;
  float in[n];
  float complex out[n];

  for (size_t i = 0; i < n; ++i) {
    float t = (float)i / n;
    in[i] = cosf(2 * pi * t * 1) + sinf(2 * pi * t * 2);
  }

#if 0
  //
  // to print out even and odd example
  //
  printf("print out even and odd sets\n\n");
  
  size_t f = 1;
  printf("%02zu: ", f);
  for (size_t i = 0; i < n; i += 2) {
    float t = (float)i / n;
    printf("%.2f ", sinf(2*pi*t*f));
  }
  printf("\n");

  f += n/2;
  printf("%02zu: ", f);
  for (size_t i = 0; i < n; i += 2) {
    float t = (float)i / n;
    printf("%.2f ", sinf(2*pi*t*f));
  }
  printf("\n");

  f = 1;
  printf("%02zu: ", f);
  for (size_t i = 1; i < n; i += 2) {
    float t = (float)i / n;
    printf("%.2f ", sinf(2*pi*t*f));
  }
  printf("\n");

  f += n/2;
  printf("%02zu: ", f);
  for (size_t i = 1; i < n; i += 2) {
    float t = (float)i / n;
    printf("%.2f ", sinf(2*pi*t*f));
  }
  printf("\n");

#elif 0
  //
  // to print out the destructured version pf eular
  //
  printf("print out the destructure version\n\n");

  for (size_t f = 0; f < n; ++f) {
    // out[f] = 0;
    // for (size_t i = 0; i < n; ++i) {
    //   float t = (float)i/n;
    //   out[f] += in[i]*cexp(2*I*pi*f*t);
    // }
    printf("%02zu: ", f);
    for (size_t i = 0; i < n; ++i) {
      float t = (float)i / n;
      printf(" %5.2f", sinf(2 * pi * t * f));
    }
    printf("\n");
  }

  for (size_t f = 0; f < n; ++f) {
    printf("%02zu: %.2f, %.2f\n", f, creal(out[f]), cimag(out[f]));
  }

#elif 0
  //
  // to print the fft normally
  //
  printf("print fft normally\n\n");

  for (size_t f = 0; f < n; ++f) {
    for (size_t i = 0; i < n; ++i) {
      float t = (float)i / n;
      out[f] += in[i] * cexpf(2 * I * pi * t * f);
    }
  }

  for (size_t f = 0; f < n; ++f) {
    printf("%02zu: %.2f, %.2f\n", f, creal(out[f]), cimag(out[f]));
  }

#elif 0
  //
  // to print odd and even sample frequencies
  //
  printf("print fft exploting symmetry\n\n");

  for (size_t f = 0; f < n / 2; ++f) {
    out[f] = 0;
    out[f + n / 2] = 0;

    // even
    for (size_t i = 0; i < n; i += 2) {
      float t = (float)i / n;
      float complex calc = in[i] * cexpf(2 * I * pi * t * f);
      out[f] += calc;
      out[f + n / 2] += calc;
    }

    // odd
    for (size_t i = 1; i < n; i += 2) {
      float t = (float)i / n;
      float complex calc = in[i] * cexpf(2 * I * pi * t * f);
      out[f] += calc;
      out[f + n / 2] -= calc;
    }
  }

  for (size_t f = 0; f < n; ++f) {
    printf("%02zu: %.2f, %.2f\n", f, creal(out[f]), cimag(out[f]));
  }

#else
  //
  //
  //
  printf("final fft copy from wiki\n\n");
  fft(in, 1, out, n);

  // for (size_t f = 0; f < n; ++f) {
    // printf("%02zu: %.2f, %.2f\n", f, creal(out[f]), cimag(out[f]));
  // }

#endif

  return 0;
}
