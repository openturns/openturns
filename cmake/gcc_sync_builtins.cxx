

int main(int /*argc*/, const char* /*argv*/[])
{
  volatile int atomic = 2;
  __sync_bool_compare_and_swap (&atomic, 2, 3);
  return 0;
}
