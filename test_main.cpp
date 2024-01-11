#include <iostream>
#include <aws/lambda-runtime/runtime.h>

#include "handler.h"
using namespace std;
using namespace aws::lambda_runtime;

int main()
{
  cout << "Running Handler text execution " << __DATE__ << "," << __TIME__ << endl;
  aws::lambda_runtime::invocation_request req;
  whisper_handler(req);
  return 0;
}