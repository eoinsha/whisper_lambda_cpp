#include <iostream>
#include <aws/lambda-runtime/runtime.h>
#include "handler.h"

using namespace std;
using namespace aws::lambda_runtime;

int main()
{
  cout << "Running Lambda Handler " << __DATE__ << "," << __TIME__ << endl;
  run_handler(whisper_handler);
  return 0;
}