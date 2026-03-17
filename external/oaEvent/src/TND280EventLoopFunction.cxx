#include "TND280Log.hxx"
#include "TND280EventLoopFunction.hxx"

ND::TND280EventLoopFunction::TND280EventLoopFunction() {}

ND::TND280EventLoopFunction::~TND280EventLoopFunction() {}

bool ND::TND280EventLoopFunction::operator () (TND280Event& event) {
    ND280Warn("Event loop without processing operator " 
              << event.GetName());
    return true;
}

int ND::TND280EventLoopFunction::Process(TND280Event& event, int outputFiles) {
    if (operator()(event)) return 0;
    return -1;
}

void ND::TND280EventLoopFunction::Initialize(void) {}

void ND::TND280EventLoopFunction::BeginFile(TVInputFile *const) {}

void ND::TND280EventLoopFunction::EndFile(TVInputFile *const) {}

//void ND::TND280EventLoopFunction::Finalize(TND280Output& value) {}

void ND::TND280EventLoopFunction::Finalize(TND280Output * const value) {
    // The user program didn't define a Finalize method.  Check to see if
    // there is a valid output file, and if there is, try the old (bogus) form
    // of Finalize in case the program hasn't been updated. 
//    if (value) Finalize(*value);
}

void ND::TND280EventLoopFunction::Usage(void) {}

bool ND::TND280EventLoopFunction::SetOption(std::string option,
                                            std::string value) {
    ND280Warn("Empty set option called with " << option << "=" << value);
    return false;
}
