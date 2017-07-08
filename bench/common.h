unsigned long g_result;

static unsigned long long
cputime (void)
{
  struct rusage rus;
  getrusage (0, &rus);
  return rus.ru_utime.tv_sec * 1000000ULL + rus.ru_utime.tv_usec;
}

static unsigned int randValue = 0;
static void rand_init(void)
{
  randValue = 0;
}
static unsigned int rand_get(void)
{
  randValue = randValue * 31421U + 6927U;
  return randValue;
}

static void test_function(const char str[], size_t n, void (*func)(size_t))
{
  unsigned long long start, end;
  //  (*func)(n);
  rand_init();
  start = cputime();
  (*func)(n);
  end = cputime();
  end = (end - start) / 1000U;
  printf ("%s %Lu ms for n = %lu\n", str, end, n);
}
