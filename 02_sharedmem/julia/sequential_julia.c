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

int compute_julia_pixel(int x, int y, int width, int height, float tint_bias,
                        unsigned char *rgb);
int write_bmp_header(FILE *f, int width, int height);

/*
 * compute_julia_pixel(): compute RBG values of a pixel in a
 *                        particular Julia set image.
 *
 *  In:
 *	(x,y):            pixel coordinates
 *	(width, height):  image dimensions
 *	tint_bias:	  a float to "tweak" the tint (1.0 is "no additional
 *tint") Out: rgb: an already-allocated 3-byte array into which R, G, and B
 *	     bytes are written.
 *
 *  Return:
 *  	0 in success, -1 on failure
 *
 */

int compute_julia_pixel(int x, int y, int width, int height, float tint_bias,
                        unsigned char *rgb) {

  // Check coordinates
  if ((x < 0) || (x >= width) || (y < 0) || (y >= height)) {
    fprintf(stderr, "Invalid (%d,%d) pixel coordinates in a %d x %d image\n", x,
            y, width, height);
    return -1;
  }

  // "Zoom in" to a particular section of the Mandelbrot set
  float X_MIN = -1.6, X_MAX = 1.6, Y_MIN = -0.9, Y_MAX = +0.9;
  float float_y = (Y_MAX - Y_MIN) * (float)y / height + Y_MIN;
  float float_x = (X_MAX - X_MIN) * (float)x / width + X_MIN;

  // Point that defines the Julia set
  float julia_real = -.79;
  float julia_img = .15;

  // Maximum of iteration before we determine "did not converge"
  int max_iter = 300;

  // Computing the complex series convergence
  float real = float_y, img = float_x;
  int num_iter = max_iter;
  while ((img * img + real * real < 2 * 2) && (num_iter > 0)) {
    float xtemp = img * img - real * real + julia_real;
    real = 2 * img * real + julia_img;
    img = xtemp;
    num_iter--;
  }
  //
  // Paint pixels based on how many iterations were used, using some funky
  // colors
  float color_bias = (float)num_iter / max_iter;

  rgb[0] =
      (num_iter == 0 ? 200
                     : -500.0 * pow(tint_bias, 1.2) * pow(color_bias, 1.6));
  rgb[1] = (num_iter == 0 ? 100 : -255.0 * pow(color_bias, 0.3));
  rgb[2] = (num_iter == 0
                ? 100
                : 255 - 255.0 * pow(tint_bias, 1.2) * pow(color_bias, 3.0));

  return 0;
}

/* write_bmp_header():
 *
 *   In:
 *      f: A file open for writing ('w')
 *      (width, height): image dimensions
 *
 *   Return:
 *      0 on success, -1 on failure
 *
 */

int write_bmp_header(FILE *f, int width, int height) {

  unsigned int adjusted_width = width + (width % 4 == 0 ? 0 : (4 - width % 4));

  // Define all fields in the bmp header
  char id[2] = "BM";
  unsigned int filesize =
      54 + (int)(adjusted_width * height * 3 * sizeof(char));
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
  ret += fwrite(id, sizeof(char), 2, f);
  ret += fwrite(&filesize, sizeof(int), 1, f);
  ret += fwrite(reserved, sizeof(short), 2, f);
  ret += fwrite(&offset, sizeof(int), 1, f);
  ret += fwrite(&size, sizeof(int), 1, f);
  ret += fwrite(&width, sizeof(int), 1, f);
  ret += fwrite(&height, sizeof(int), 1, f);
  ret += fwrite(&planes, sizeof(short), 1, f);
  ret += fwrite(&bits, sizeof(short), 1, f);
  ret += fwrite(&compression, sizeof(int), 1, f);
  ret += fwrite(&image_size, sizeof(int), 1, f);
  ret += fwrite(&x_res, sizeof(int), 1, f);
  ret += fwrite(&y_res, sizeof(int), 1, f);
  ret += fwrite(&ncolors, sizeof(int), 1, f);
  ret += fwrite(&importantcolors, sizeof(int), 1, f);

  // Success means that we wrote 17 "objects" successfully
  return (ret != 17);
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

  int dummy = 0;

  int n_workers = n_threads - 1;
  if (my_rank != 0) {
    // Worker
    int worker_id = my_rank - 1;
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

    /* printf("[%d/%d] Done!\n", worker_id, n_workers); */

    MPI_Send(&dummy, 1, MPI_INT, 0, MSG_INIT,  MPI_COMM_WORLD);
    MPI_Send(&begin, 1, MPI_INT, 0, MSG_BEGIN,  MPI_COMM_WORLD);
    MPI_Send(&end,   1, MPI_INT, 0, MSG_END,    MPI_COMM_WORLD);
    MPI_Send(pixels, (end - begin) * width * 3, MPI_UNSIGNED_CHAR, 0, MSG_PIXELS, MPI_COMM_WORLD);
  } else {
    // Master
    int begin;
    int end;

    int worker_status[n_workers];

    // Allocate array (for the entire picture)
    unsigned char *pixels = (unsigned char *)calloc(height * width * 3, sizeof(char));

    // TODO: Change this to be order-insensitive
    int worker_idx;

    printf("Worker status: ");
    for (worker_idx = 0; worker_idx < n_workers; worker_idx++) {
      worker_status[worker_idx] = 0;
      printf(". ");
    }
    fflush(stdout);

    int i;
    for (i = 0; i < n_workers; i++) {
      MPI_Status status;

      MPI_Recv(&dummy,  1, MPI_INT, MPI_ANY_SOURCE, MSG_INIT,  MPI_COMM_WORLD, &status);

      int worker_idx = status.MPI_SOURCE - 1;

      worker_status[worker_idx] = 1;
      printf("\rWorker status: ");
      int worker_idx2;
      for (worker_idx2 = 0; worker_idx2 < n_workers; worker_idx2++) {
        if (worker_status[worker_idx2] == 0) {
          printf(". ");
        } else {
          printf("* ");
        }
      }
      fflush(stdout);

      MPI_Recv(&begin,  1, MPI_INT, worker_idx + 1, MSG_BEGIN,  MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(&end,    1, MPI_INT, worker_idx + 1, MSG_END,    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(&(pixels[begin * width * 3]), (end - begin) * width * 3, MPI_UNSIGNED_CHAR, worker_idx + 1, MSG_PIXELS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    printf("\n");
    printf("All workers done! Writing the array to file ...\n");


    // Open file for writing
    FILE *f = fopen("./julia.bmp", "w");

    // Write header
    if (write_bmp_header(f, width, height)) {
      perror("Error writing header: ");
      exit(0);
    }

    // Write the pixels
    for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
        fwrite(&(pixels[y * 3 * width + x * 3]), sizeof(char), 3, f);
      }
      // padding in case of an even number of pixels per row
      unsigned char padding[3] = {0, 0, 0};
      fwrite(padding, sizeof(char), ((width * 3) % 4), f);
    }

    // Close the file
    fclose(f);

    printf("Done!\n");
  }

  MPI_Finalize(); // Call once

  return 0;
}
