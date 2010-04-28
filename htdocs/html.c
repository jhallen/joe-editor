/* Convert text file into html */

void fix(char *o, char *i)
  {
  for (; *i; ++i)
    if (*i=='&')
      {
      *o++ = '&';
      *o++ = 'a';
      *o++ = 'm';
      *o++ = 'p';
      *o++ = ';';
      }
    else if (*i=='<')
      {
      *o++ = '&';
      *o++ = 'l';
      *o++ = 't';
      *o++ = ';';
      }
    else if (*i=='>')
      {
      *o++ = '&';
      *o++ = 'g';
      *o++ = 't';
      *o++ = ';';
      }
    else
      *o++ = *i;
  *o = 0;
  }

main()
  {
  char buf[1024];
  char fixed[1024];
  int inpara = 0;
  while(gets(buf))
    {
    int x;
    int isblank = 1;
    fix(fixed, buf);
    for (x = 0; buf[x]; ++x)
      if(buf[x] != ' ' && buf[x] != '\t')
        isblank = 0;
    if (!inpara)
      {
      if (isblank)
        {
        printf("\n");
        }
      else
        {
        inpara = 1;
        printf("<p>%s\n",fixed);
        }
      }
    else
      {
      if (isblank)
        {
        printf("</p>\n");
        inpara = 0;
        }
      else
        {
        printf("%s\n",fixed);
        }
      }
    }
  if (inpara)
    printf("</p>\n");
  }
