/* image magic cli : mogrify -format bmp original.bmp */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS	8

struct thread_data
{
   unsigned char *pixels;
   int	from;
   int	to;
};

void *ImgThread(void *threadarg)
{

   struct thread_data *my_data;
   my_data = (struct thread_data *) threadarg;

   unsigned char *pixels = my_data->pixels;
   int my_from = my_data->from;
   int my_to = my_data->to;
   
   for(int i=my_from; i<my_to; i++) {
   unsigned char *pixel = pixels + i * 3;
   unsigned char gray = pixel[0] * 0.299 + pixel[1] * 0.587 + pixel[2] * 0.114;
   memset(pixel, gray, 3);
   }
   pthread_exit(NULL);
}
int main()
{
    FILE *fIn = fopen("7lake.bmp", "rb");
    FILE *fOut = fopen("newlake.bmp", "wb");
    if (!fIn || !fOut)
    {
        printf("File error.\n");
        return 0;
    }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, fIn);
    fwrite(header, sizeof(unsigned char), 54, fOut);

    int width = *(int*)&header[18];
    int height = abs(*(int*)&header[22]);
    int stride = (width * 3 + 3) & ~3;
    int padding = stride - width * 3;

    printf("width: %d (%d)\n", width, width * 3);
    printf("height: %d\n", height);
    printf("stride: %d\n", stride);
    printf("padding: %d\n", padding);

    int total_pixels = width * height;
    unsigned char *pixels = malloc(total_pixels * 3);

    unsigned char pixel[3];
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            fread(pixel, 3, 1, fIn);
            // unsigned char gray = pixel[0] * 0.299 + pixel[1] * 0.587 + pixel[2] * 0.114;
            // memset(pixel, gray, sizeof(pixel));
            // fwrite(&pixel, 3, 1, fOut);
            memcpy(pixels + (y * width + x) * 3, pixel, 3);
        }
        fread(pixel, padding, 1, fIn);
        fwrite(pixel, padding, 1, fOut);
    }
    /* begin time */
   struct timespec begin, end; 
   clock_gettime(CLOCK_REALTIME, &begin);


    pthread_t threads[NUM_THREADS];
    struct thread_data thread_args[NUM_THREADS];
     int block = total_pixels / NUM_THREADS;
    for(int t=0; t<NUM_THREADS; t++){
     thread_args[t].from = t*block;
     thread_args[t].to = thread_args[t].from+block;
     if (t == NUM_THREADS-1) thread_args[t].to = total_pixels;
     thread_args[t].pixels = pixels;
     pthread_create(&threads[t], NULL, ImgThread, (void *) &thread_args[t]);
   }
   
   for(int t=0; t<NUM_THREADS; t++){
     pthread_join(threads[t], NULL);
   }

   /* end time */
   clock_gettime(CLOCK_REALTIME, &end);
   long seconds = end.tv_sec - begin.tv_sec;
   long nanoseconds = end.tv_nsec - begin.tv_nsec;
   double elapsed = seconds + nanoseconds*1e-9;
   printf("Time measured: %.3f seconds.\n", elapsed);


   for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            memcpy(pixel, pixels + (y * width + x) * 3, 3);
            fwrite(&pixel, 3, 1, fOut);
        }
        fwrite(pixel, padding, 1, fOut);
    }

   free(pixels);
   fclose(fOut);
   fclose(fIn);
    return 0;
}

/* Times measured:
Picture:                Size:       1       2       4       8       Threads
1house.bmp              10,4MB     0,022   0,013   0,012   0,011
2aerial.bmp             25,7MB     0,053   0,027   0,027   0,027 
3tiger.bmp              68,6MB     0,143   0,079   0,075   0,069
4food.bmp               65,3MB     0,135   0,074   0,068   0,072
5landscape.bmp          51,2MB     0,107   0,060   0,056   0,055
6berries.bmp            60,7MB     0,128   0,076   0,080   0,066
7lake.bmp               78,5MB     0,166   0,089   0,098   0,084
*/