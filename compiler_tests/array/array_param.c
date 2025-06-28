int
func (int arr[5])
{
  return sizeof (arr);
}

int
f ()
{
  int arr[5] = { 10, 20, 30, 40, 50 };
  return func (arr);
}
