int
f ()
{
  int i;
  int arr[5];
  int sum = 0;

  for (i = 0; i < 5; i++)
    {
      arr[i] = i + 1;
    }

  for (i = 0; i < 5; i++)
    {
      sum += arr[i];
    }

  return sum;
}
