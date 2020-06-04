#ifndef COBJECTCONTAINER_H
#define COBJECTCONTAINER_H

#include "CUSBPrinter.h"
#include "clogger.h"

class CObjectContainer
{
public:
  static void initialize();

  static CLogger* getTraceInstance();

  static CUSBPrinter* getPrinterInstance();

private:
  static CLogger* m_trace;
  static CUSBPrinter* m_comm;
};

#endif /* COBJECTCONTAINER_H */
