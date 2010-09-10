/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:54:57 CEST 2010  test-runner.hpp

                        test-runner.hpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


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