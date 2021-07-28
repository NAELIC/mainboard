#include "ir.h"

static naelic::SWO swo;

namespace ir
{
  float value = 0.0;
  DigitalOut ir_emit(IR_EMIT, 0.0);
  AnalogIn ir_receive(IR_RECEIVE);

  void compute()
  {
      ir_emit = 1;
      // TODO : verify if wait is needed
      value = ir_receive.read();
      ir_emit = 0;
  }

  bool present() {
    return value < IR_THRESHOLD ? true : false;
  }
}

SHELL_COMMAND(ir, "IR Debug") {
  while(!shell_available()) {
    shell_print("detection : ");
    shell_print(ir::present());
    shell_print(", value : ");
    shell_println(ir::value);
  }
}