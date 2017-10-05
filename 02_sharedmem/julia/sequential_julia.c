#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
/* #include <mpi.h> */
#include "/usr/lib/openmpi/include/mpi.h"

#define MSG_INIT 0
#define MSG_BEGIN 1
#define MSG_END 2
#define MSG_PIXELS 3

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

int compute_julia_pixel(int x, int y, int width, int height, float tint_bias, unsigned char *rgb);
int write_bmp_header(FILE *f, int width, int height);


/**
 * compute RBG values of a pixel in a particular Julia set image.
 *
 * @param[in]  x,y          Pixel coordinates
 * @param[in]  width,height Image dimensions
 * @param[in]  tint_bias    A float to "tweak" the tint (1.0 is "no additional tint")
 *
 * @param[out] rgb          An already-allocated 3-byte array into which R, G, and B bytes are written.
 *
 * @return 0 on success, non-zero on failure
 *
 */
int compute_julia_pixel(int x, int y, int width, int height, float tint_bias, unsigned char *rgb) {
  // Check coordinates
  if ((x < 0) || (x >= width) || (y < 0) || (y >= height)) {
    fprintf(stderr,"Invalid (%d,%d) pixel coordinates in a %d x %d image\n", x, y, width, height);
    return -1;
  }

  // "Zoom in" to a particular section of the Mandelbrot set
  float X_MIN = -1.6, X_MAX = 1.6, Y_MIN = -0.9, Y_MAX = +0.9;
  float float_y = (Y_MAX - Y_MIN) * (float)y / height + Y_MIN ;
  float float_x = (X_MAX - X_MIN) * (float)x / width  + X_MIN ;

  // Point that defines the Julia set
  float julia_real = -.79;
  float julia_img = .15;

  // Maximum of iteration before we determine "did not converge"
  int max_iter = 300;

  // Computing the complex series convergence
  float real=float_y, img=float_x;
  int num_iter = max_iter;
  while (( img * img + real * real < 2 * 2 ) && ( num_iter > 0 )) {
    float xtemp = img * img - real * real + julia_real;
    real = 2 * img * real + julia_img;
    img = xtemp;
    num_iter--;
  }
  //
  // Paint pixels based on how many iterations were used, using some funky colors
  float color_bias = (float) num_iter / max_iter;

  rgb[0] = (num_iter == 0 ? 200 : - 500.0 * pow(tint_bias, 1.2) *  pow(color_bias, 1.6));
  rgb[1] = (num_iter == 0 ? 100 : -255.0 *  pow(color_bias, 0.3));
  rgb[2] = (num_iter == 0 ? 100 : 255 - 255.0 * pow(tint_bias, 1.2) * pow(color_bias, 3.0));

  return 0;
}


/**
 * Write the BMP header to the specified file handle. 54 bytes in total.
 *
 * @param[in] f            A file open for writing ('w')
 * @param[in] width,height Image dimensions
 *
 * @return 0 on success, non-zero on failure
 *
 */
int write_bmp_header(FILE *f, int width, int height) {
  unsigned int adjusted_width = width + (width % 4 == 0 ? 0 : (4 - width % 4));

  // Define all fields in the bmp header
  char id[2] = "BM";
  unsigned int filesize = 54 + (int)(adjusted_width * height * 3 * sizeof(char));
  short reserved[2] = {0, 0};
  unsigned int offset = 54;

  unsigned int size = 40;
  unsigned short planes = 1;
  unsigned short bits = 24;
  unsigned int compression = 0;
  unsigned int image_size = width * height * 3 * sizeof(char);
  int x_res = 0;
  int y_res = 0;
  unsigned int ncolors = 0;
  unsigned int importantcolors = 0;

  // Write the bytes to the file, keeping track of the
  // number of written "objects"
  size_t ret = 0;
  ret += fwrite(id,               sizeof(char),  2, f); // size = 1
  ret += fwrite(&filesize,        sizeof(int),   1, f); // size = 4
  ret += fwrite(reserved,         sizeof(short), 2, f); // size = 2
  ret += fwrite(&offset,          sizeof(int),   1, f); // size = 4
  ret += fwrite(&size,            sizeof(int),   1, f); // size = 4
  ret += fwrite(&width,           sizeof(int),   1, f); // size = 4
  ret += fwrite(&height,          sizeof(int),   1, f); // size = 4
  ret += fwrite(&planes,          sizeof(short), 1, f); // size = 2
  ret += fwrite(&bits,            sizeof(short), 1, f); // size = 2
  ret += fwrite(&compression,     sizeof(int),   1, f); // size = 4
  ret += fwrite(&image_size,      sizeof(int),   1, f); // size = 4
  ret += fwrite(&x_res,           sizeof(int),   1, f); // size = 4
  ret += fwrite(&y_res,           sizeof(int),   1, f); // size = 4
  ret += fwrite(&ncolors,         sizeof(int),   1, f); // size = 4
  ret += fwrite(&importantcolors, sizeof(int),   1, f); // size = 4

  // Success means that we wrote 17 "objects" successfully
  return ret == 17 ? 0 : -1;
}


/**
 * Write the BMP header to the specified **MPI** file header. 54 bytes in total.
 *
 * @param[in] f            A file open for writing ('w')
 * @param[in] width,height Image dimensions
 *
 * @return 0 on success, non-zero on failure
 *
 */
void mpi_write_bmp_header(MPI_File *f, int width, int height) {
  unsigned int adjusted_width = width + (width % 4 == 0 ? 0 : (4 - width % 4));

  // Define all fields in the bmp header
  char id[2] = "BM";
  unsigned int filesize = 54 + (int)(adjusted_width * height * 3 * sizeof(char));
  short reserved[2] = {0, 0};
  unsigned int offset = 54;

  unsigned int size = 40;
  unsigned short planes = 1;
  unsigned short bits = 24;
  unsigned int compression = 0;
  unsigned int image_size = width * height * 3 * sizeof(char);
  int x_res = 0;
  int y_res = 0;
  unsigned int ncolors = 0;
  unsigned int importantcolors = 0;

  MPI_File_write(*f, id,               2, MPI_CHAR,  MPI_STATUS_IGNORE); // size = 1
  MPI_File_write(*f, &filesize,        1, MPI_INT,   MPI_STATUS_IGNORE); // size = 4
  MPI_File_write(*f, reserved,         2, MPI_SHORT, MPI_STATUS_IGNORE); // size = 2
  MPI_File_write(*f, &offset,          1, MPI_INT,   MPI_STATUS_IGNORE); // size = 4
  MPI_File_write(*f, &size,            1, MPI_INT,   MPI_STATUS_IGNORE); // size = 4
  MPI_File_write(*f, &width,           1, MPI_INT,   MPI_STATUS_IGNORE); // size = 4
  MPI_File_write(*f, &height,          1, MPI_INT,   MPI_STATUS_IGNORE); // size = 4
  MPI_File_write(*f, &planes,          1, MPI_SHORT, MPI_STATUS_IGNORE); // size = 2
  MPI_File_write(*f, &bits,            1, MPI_SHORT, MPI_STATUS_IGNORE); // size = 2
  MPI_File_write(*f, &compression,     1, MPI_INT,   MPI_STATUS_IGNORE); // size = 4
  MPI_File_write(*f, &image_size,      1, MPI_INT,   MPI_STATUS_IGNORE); // size = 4
  MPI_File_write(*f, &x_res,           1, MPI_INT,   MPI_STATUS_IGNORE); // size = 4
  MPI_File_write(*f, &y_res,           1, MPI_INT,   MPI_STATUS_IGNORE); // size = 4
  MPI_File_write(*f, &ncolors,         1, MPI_INT,   MPI_STATUS_IGNORE); // size = 4
  MPI_File_write(*f, &importantcolors, 1, MPI_INT,   MPI_STATUS_IGNORE); // size = 4
}


/*
 * main() function
 */

int main(int argc, char **argv) {
  // Looping indices that are going to be used repeatedly
  int y, x;

  // Parse command-line arguments
  int height;
  if ((argc != 2) || (sscanf(argv[1], "%d", &height) != 1)) {
    fprintf(stderr, "Usage: %s <height (int)>\n", argv[0]);
    exit(1);
  }

  int width = height * 2;


  int my_rank;
  int n_threads;
  char hostname[128];
  int namelen;

  MPI_Init(&argc, &argv); // Called once
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &n_threads);
  MPI_Get_processor_name(hostname, &namelen);

  MPI_File fh;
  MPI_File_open(MPI_COMM_WORLD, "julia.bmp", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);

  if (my_rank == 0) {
    printf("... Master\n");
    mpi_write_bmp_header(&fh, width, height);
    printf("xxx Master\n");
  } else {
    int n_workers = n_threads - 1;
    int worker_id = my_rank - 1;

    printf("... Worker %d / %d\n", worker_id, n_workers);

    int begin = worker_id * height / n_workers;
    int end = (worker_id + 1) * height / n_workers;

    /* printf("[%d/%d] %d - %d\n", worker_id, n_workers, begin, end - 1); */

    int worker_height = end - begin;

    // Allocate array (for the current worker only)
    unsigned char *pixels = (unsigned char *)calloc(worker_height * width * 3, sizeof(char));

    for (y = begin; y < end; y++) {
      for (x = 0; x < width; x++) {
        compute_julia_pixel(x, y, width, height, 1.0, &(pixels[(y - begin) * 3 * width + x * 3]));
      }
    }

    MPI_File_seek(fh, 54 + begin * width * 3, MPI_SEEK_SET);

    MPI_Offset offset;
    MPI_File_get_position(fh, &offset);
    printf("___ Worker %d / %d: offset = %lld\n", worker_id, n_workers, offset);

    // Write the pixels
    for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
        MPI_File_write(fh, &(pixels[y * 3 * width + x * 3]), 3, MPI_CHAR, MPI_STATUS_IGNORE);

        MPI_Offset offset;
        MPI_File_get_position(fh, &offset);
        printf("___ Worker %d / %d: offset = %lld\n", worker_id, n_workers, offset);
      }
      // padding in case of an even number of pixels per row
      unsigned char padding[3] = {0, 0, 0};
      MPI_File_write(fh, padding, ((width * 3) % 4), MPI_CHAR, MPI_STATUS_IGNORE);

      MPI_Offset offset;
      MPI_File_get_position(fh, &offset);
      printf("___ Worker %d / %d: ! offset = %lld\n", worker_id, n_workers, offset);
    }

    printf("xxx Worker %d / %d\n", worker_id, n_workers);
  }

  MPI_File_close(&fh);

  MPI_Finalize(); // Call once

  return 0;
}
