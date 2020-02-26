#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(void)
{
    double p16 = 1, pi = 0, precision = 100000;
    int k;
    unsigned long micros = 0;
    float millis = 0.0;
    clock_t start, end;
    start = clock();
    // This section calculates pi
    for(k = 0; k <= precision; k++)
    {
        pi += 1.0 / p16 * (4.0 / (8 * k + 1) - 2.0 / (8 * k + 4) - 1.0 / (8 * k + 5) - 1.0 / (8 * k + 6));
        p16 *= 16;
    }
    end = clock();
    micros = end - start;
    millis = micros / 1000;
    printf("%f %ld\n", millis,micros); //my time keeps being returned as 0

    printf("this value of pi is  : %f\n", pi);
    return 0;
}
