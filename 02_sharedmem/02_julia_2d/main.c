#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
/* #include <mpi.h> */
#include "/usr/lib/openmpi/include/mpi.h"

#define MSG_YOUR_TURN 0

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


void print_diagram(char c[], int n, int total) {
  int i;
  for (i = 0; i < total; i++) {
    if (i == n) {
      printf("%s", c);
    } else {
      printf(".");
    }
  }
  printf(" %d/%d: ", n, total);
}



/**
 * Main
 *
 * We denote x-direction as the direction spanning the width of the image (from
 * left to right,) and the y-direction height (from top to bottom.)
 *
 */
int main(int argc, char **argv) {
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

  // Note that we do proceed when n_threads is not a perfect square.
  // In this case the extra cores will not be used.
  int n_segs = floor(sqrt(n_threads));

  int n_workers = n_segs * n_segs;
  int worker_id = my_rank;

  if (worker_id >= n_workers) {
#ifdef DEBUG
    print_diagram("!", worker_id, n_workers);
    printf("not used.\n");
#endif

    MPI_Finalize(); // Call once

    return 0;
  }

  int w_id = worker_id % n_segs;
  int h_id = worker_id / n_segs;

  int w_begin =  w_id      * width  / n_segs;
  int w_end   = (w_id + 1) * width  / n_segs;
  int h_begin =  h_id      * height / n_segs;
  int h_end   = (h_id + 1) * height / n_segs;

  int worker_height = h_end - h_begin;
  int worker_width  = w_end - w_begin;

#ifdef DEBUG
  print_diagram("+", worker_id, n_workers);
  printf("id %d %d - range w %d %d / h %d %d - size %d %d\n",
         w_id, h_id, w_begin, w_end, h_begin, h_end, worker_height, worker_width);
#endif

  // Allocate array (for the current worker only)
  unsigned char *pixels = (unsigned char *)calloc(worker_height * worker_width * 3, sizeof(char));

  srand((unsigned int)time(NULL));
  int i;
  for (i = 0; i < worker_id; i++) {
    srand(rand());
  }
  float tint = pow(2, (float)rand() / (float)(RAND_MAX) * 2 - 1);

  // Compute the pixels (this part is *fully* parallelized)
  int h, w;
  for (h = 0; h < worker_height; h++) {
    for (w = 0; w < worker_width; w++) {
      compute_julia_pixel(w + w_begin, h + h_begin, width, height, tint, &(pixels[h * 3 * worker_width + w * 3]));
    }
  }

  int dummy;

  // Open file for writing
  FILE *f;
  if (worker_id == 0) {
    f = fopen("./julia.bmp", "w");

    // Write header
    if (write_bmp_header(f, width, height)) {
      perror("Error writing header: ");
      exit(0);
    }

    fclose(f);
  }


  // Write the pixels (this part is *fully* serial)
  for (h = 0; h < worker_height; h++) {
    if (w_id == 0) {
      if (h == 0) {
        if (h_id == 0) {
          // I'm the *very* first one! Go without signal
        } else {
          // I'm on the first row of the first block in my row-group. I'll wait the last block of the *last* row-group to kick me.
#ifdef DEBUG
          print_diagram("<", worker_id, n_workers);
          printf("(first row of first block) waiting for worker %d.\n", h_id * n_segs - 1);
#endif
          MPI_Recv(&dummy, 1, MPI_INT, h_id * n_segs - 1, MSG_YOUR_TURN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#ifdef DEBUG
          print_diagram("-", worker_id, n_workers);
          printf("(first row of first block) received signal %d from worker %d.\n", dummy, h_id * n_segs - 1);
#endif
        }
      } else {
        // I'm first block in my row-group, but it's not our first row. I'll wait the last block of the *my* row-group to kick me.
#ifdef DEBUG
        print_diagram("<", worker_id, n_workers);
        printf("(first block) waiting for worker %d.\n", (h_id + 1) * n_segs - 1);
#endif
        MPI_Recv(&dummy, 1, MPI_INT, (h_id + 1) * n_segs - 1, MSG_YOUR_TURN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#ifdef DEBUG
        print_diagram("-", worker_id, n_workers);
        printf("(first block) received signal %d from worker %d.\n", dummy, (h_id + 1) * n_segs - 1);
#endif
      }
    } else {
      // I'm not even first block in my row-group. I'll wait the previous block of the *my* row-group to kick me.
#ifdef DEBUG
      print_diagram("<", worker_id, n_workers);
      printf("waiting for worker %d.\n", worker_id - 1);
#endif
      MPI_Recv(&dummy, 1, MPI_INT, worker_id - 1, MSG_YOUR_TURN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#ifdef DEBUG
      print_diagram("-", worker_id, n_workers);
      printf("received signal %d from worker %d.\n", dummy, worker_id - 1);
#endif
    }

    f = fopen("./julia.bmp", "a");

    for (w = 0; w < worker_width; w++) {
      fwrite(&(pixels[h * 3 * worker_width + w * 3]), sizeof(char), 3, f);

#ifdef DEBUG
      print_diagram("^", worker_id, n_workers);
      printf("writing (%d, %d).\n", h + h_begin, w + w_begin);
#endif
    }

    if (w_id == n_segs - 1) {
      // padding in case of an even number of pixels per row
      unsigned char padding[3] = {0, 0, 0};
      fwrite(padding, sizeof(char), ((width * 3) % 4), f);

#ifdef DEBUG
      print_diagram("^", worker_id, n_workers);
      printf("writing padding.\n");
#endif
    }

    // Close the file
    fclose(f);

    if (w_id == n_segs - 1) {
      if (h == worker_height - 1) {
        if (h_id == n_segs - 1) {
          // I'm the *very* last! Nothing to do now.
        } else {
          // I'm on the last row of the last block in my row-group. I'll kick the first block in the *next* row-group.
#ifdef DEBUG
          print_diagram(">", worker_id, n_workers);
          printf("(last row of last block) kicks worker %d.\n", (h_id + 1) * n_segs);
#endif
          dummy = h;
          MPI_Send(&dummy, 1, MPI_INT, (h_id + 1) * n_segs, MSG_YOUR_TURN, MPI_COMM_WORLD);
        }
      } else {
        // I'm the last block in my row-group, but it's not our last row. I'll kick the first block of *my* row-group.
#ifdef DEBUG
        print_diagram(">", worker_id, n_workers);
        printf("(last block) kicks worker %d.\n", h_id * n_segs);
#endif
        dummy = h;
        MPI_Send(&dummy, 1, MPI_INT, h_id * n_segs, MSG_YOUR_TURN, MPI_COMM_WORLD);
      }
    } else {
      // I'm not even the last block in my row-group. I'll kick the next block of *my* row-group.
#ifdef DEBUG
      print_diagram(">", worker_id, n_workers);
      printf("kicks worker %d.\n", worker_id + 1);
#endif
      dummy = h;
      MPI_Send(&dummy, 1, MPI_INT, worker_id + 1, MSG_YOUR_TURN, MPI_COMM_WORLD);
    }
  }

#ifdef DEBUG
  print_diagram("x", worker_id, n_workers);
  printf("done.\n");
#endif

  MPI_Finalize(); // Call once

  return 0;
}
