#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <iostream>
#include <iterator>
#include "tools.h"


Error::Error()
{
  stop=false;
}


void Error::warning(string s)
{
  s.insert(0, "Warning: ");
  messages.push_back(s);
}


void Error::error(string s)
{
  s.insert(0, "Error: ");
  messages.push_back(s);
  stop=true;
}


void Error::print()
{
  copy(messages.begin(), messages.end(), ostream_iterator<string>(cerr));
}


/*----------------------------------------------------------------------*/


Regexp::Regexp()
{
}


bool Regexp::matches(string pattern, string src, list<string> & matches)
{
  regex_t pattern_buffer;
  regmatch_t regs[10];
  /* try to compile pattern */
  int err=0;
  if (src.empty() || 
      (err=regcomp(&pattern_buffer, pattern.c_str(), REG_EXTENDED)) )
    {
      char msg[100];
      if (!(src.empty()))
        {
          regerror(err, &pattern_buffer, msg, 100);
          cerr << "Error in regular expression: " 
               << pattern << '\n' << msg << '\n';
        }
      /* if error occured, no matching was found */
      return false;
    }
  /* match the pattern against the given string */
  if (regexec(&pattern_buffer, src.c_str(), 10, regs, 0)!=0)
    /* no match */
    return false;
  /* copy the part left to the point where the matching starts */

  int i;
  for (i=1; i<9; i++)
  {
    if (regs[i].rm_so>=0)
      matches.push_back(src.substr(regs[i].rm_so, regs[i].rm_eo - regs[i].rm_so));
  }

  /* return the end of the last match */
  return true;
}


bool Regexp::substitute_local(string pattern, string src, string replacement,
                              string & dest, int & dest_end)
{
  //cout << pattern << ":" << src << ":" << replacement << endl;
  regex_t pattern_buffer;
  regmatch_t regs[10];
  /* try to compile pattern */
  int err=0;
  if (src.empty() || (err=regcomp(&pattern_buffer, pattern.c_str(), 
                                  REG_EXTENDED)) )
  {
    char msg[100];
    if (!src.empty())
      {
        regerror(err, &pattern_buffer, msg, 100);
        cerr << "Error in regular expression: " 
             << pattern << '\n' << msg << '\n';
      }
    /* if error occured, do not make substitution */
    dest=src;
    return false;
  }
  /* match the pattern against the given string */
  if (regexec(&pattern_buffer, src.c_str(), 10, regs, 0)!=0)
    {
      /* no match */
      dest=src;
      return false;
    }
  /* copy the part left to the point where the matching starts */
  dest=src.substr(0,regs[0].rm_so);
  unsigned int i=0;
  while (i<replacement.length()) {
    if (replacement[i]=='\\') {
      i++;
      if (replacement[i]>='0' && replacement[i]<='9') {
        int num=replacement[i]-'0';
        dest+=src.substr(regs[num].rm_so, regs[num].rm_eo - regs[num].rm_so);
      } else
        dest+=replacement[i];
    }
    else
      dest+=replacement[i];
    i++;
  }
  dest_end=dest.length();
  dest+=src.substr(regs[0].rm_eo, src.length() - regs[0].rm_eo);

  //cout << ":" << dest << endl;
  return true;
}


void Regexp::substitute(string pattern, string src, 
                        string replacement, string & dest, bool global)
{
  string src_left, src_right, dest_right, result;
  int differ, index=0, de=0;

  //cout << "!" << src << "!" << global << endl;
  if (global)
    {
      result=src;
      do {
        src_left=result.substr(0,index);
        src_right=result.substr(index);
        differ=substitute_local(pattern, src_right, 
                                replacement, dest_right, de);
        result=src_left+dest_right;
        index+=de;
      } while (differ);
      dest=result;
    }
  else
    substitute_local(pattern, src, replacement, dest, de);
}


/* transliterate a string like sed's y */
void Regexp::transliterate(string from, string to, string src, string & dest)
{
  int i, pos;
  dest=string();
  for (i=0; i<(int)src.length(); i++)
    {
      pos=from.find(src[i]);
      if (pos>=0 && pos<(int)to.length()) dest+=to[pos];
      else dest+=src[i];
    }
}


void Regexp::split(char c, string s, list<string> & r)
{
  size_t b=0, e;
  e=s.find(c,b);
  if (e<s.npos)
    {
      do 
        {
          r.push_back(s.substr(b,e-b)); // between separators
          b=e+1;
          e=s.find(c,b);
        }
      while (e<s.npos);
      if (b<s.length()) 
        r.push_back(s.substr(b,e-b)); // after the last separator
    }
  else
    r.push_back(s); // no separator present
}




void Regexp::join(char c, list<string> & r, string & s)
{
  list<string>::iterator it;
  for (it=r.begin();
       it!=r.end();
       it++)
    {
      if (it!=r.begin())
        s+=c;
      s=(*it);
    }
}
