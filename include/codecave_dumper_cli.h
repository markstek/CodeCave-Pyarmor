#pragma once
#include "codecave_dumper.h"

class CodecaveDumperCLI {
private:
    CodecaveDumper dumper;
    void listProcesses();

public:
    void run();
};