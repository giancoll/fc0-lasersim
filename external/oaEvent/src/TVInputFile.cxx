#include "EoaCore.hxx"
#include "TND280Event.hxx"
#include "TVInputFile.hxx"

ND::TND280Event* ND::TVInputFile::FirstEvent() {throw ND::EoaCore();}
ND::TND280Event* ND::TVInputFile::NextEvent(int skip) {throw ND::EoaCore();}
int ND::TVInputFile::GetPosition() const {throw ND::EoaCore();}
bool ND::TVInputFile::IsOpen() {throw ND::EoaCore();}
bool ND::TVInputFile::EndOfFile() {throw ND::EoaCore();}
void ND::TVInputFile::CloseFile() {throw ND::EoaCore();}
const char* ND::TVInputFile::GetFilename() const {return "";}
