#include "ir.h"

static naelic::SWO swo;

namespace ir
{
  DigitalOut ir_emit(IR_EMIT, 0.0);
  AnalogIn ir_receive(IR_RECEIVE);

  void launch()
  {
    while(true) {
      ir_emit = 1;
      value = ir_receive.read();
      ir_emit = 0;
      ThisThread::sleep_for(500ms);
    }
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