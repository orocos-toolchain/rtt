#ifdef WIN32
int setenv(const char *name, const char *value, int overwrite)
{
  if (overwrite == 0)
  {
    char c;
    if (GetEnvironmentVariable(name, &c, sizeof (c)) > 0) return 0;
  }

  if (SetEnvironmentVariable(name, value) != 0) return 0;
  return -1;
}
#endif