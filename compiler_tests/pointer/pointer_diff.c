int
f ()
{
  int arr[6] = { 5, 10, 15, 20, 25, 30 };
  int *ptr1 = &arr[1];
  int *ptr2 = &arr[5];

  return ptr2 - ptr1;
}
