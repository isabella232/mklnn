#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "src/Random.c"
#else


void MKLNN_(Dropout_updateOutput)(
  THDoubleTensor *input,
  THTensor *output,
  double p)
{
    THArgCheck(p >= 0 && p <= 1, 1, "must be >= 0 and <= 1");

    struct timeval start;
    gettimeofday(&start,NULL);
    long seed = start.tv_sec * 1000 + (double)start.tv_usec/1000;
    //generate mt19937 random number
    //VSLStreamStatePtr stream_mt19937;
    //vslNewStream( &stream_mt19937, VSL_BRNG_MT19937, seed);
    //int *seed_mt19937 = (int*)malloc(1*sizeof(int));
    //vsRngUniform( VSL_RNG_METHOD_UNIFORM_STD, stream_mt19937, 1, seed_mt19937, 1, 4294967295);
    //vslDeleteStream(&stream_mt19937);
           
    //mt19937 as seed to generate bernoulli  
    

    RNG  rng = RNGInit(seed);
    unsigned long seedNew = RandInt(&rng);
    int n = THTensor_(nElement)(input);
    int *r = THTensor_(data)(output);
    printf("----------------run here!\n");

    int nthr = omp_get_max_threads();
    printf("----------------run here!\n");
 
    int *tmp = (int*)malloc(n*sizeof(int));
    
    # pragma omp parallel num_threads(nthr)
    {
    const int ithr = omp_get_thread_num();
    const int avg_amount = (n + nthr - 1) / nthr;
    const int my_offset = ithr * avg_amount;
    const int my_amount = min(my_offset + avg_amount, n) - my_offset;
         
    if (my_amount > 0) {
      VSLStreamStatePtr stream;
      vslNewStream(&stream, VSL_BRNG_MCG31, seedNew);
      vslSkipAheadStream(stream, my_offset);
      viRngBernoulli(VSL_RNG_METHOD_BERNOULLI_ICDF, stream, my_amount,
        tmp + my_offset, p);
      vslDeleteStream(&stream);
    }
    }

  int k;
  for(k=0;k<n;k++)
  {
    r[k]=tmp[k];
  }
  free(tmp);
  //free(seed_mt19937);

}

#endif
