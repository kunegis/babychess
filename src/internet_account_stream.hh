#ifndef INTERNET_ACCOUNT_STREAM_HH
#define INTERNET_ACCOUNT_STREAM_HH

#include "internet_account.hh"

#include <stdio.h>

bool Read_Stream(FILE *, Internet_Account *);
bool Write_Stream(FILE *, const Internet_Account *);

#endif /* ! INTERNET_ACCOUNT_STREAM_HH */
