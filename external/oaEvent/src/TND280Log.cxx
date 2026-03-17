#include <vector>
#include <sstream>
#include <fstream>
#include <time.h>

#include "TND280Log.hxx"

ND::TND280Log::ErrorPriority ND::TND280Log::fErrorPriority = ND::TND280Log::ErrorLevel;
ND::TND280Log::LogPriority ND::TND280Log::fLogPriority = ND::TND280Log::LogLevel;
std::ostream* ND::TND280Log::fDebugStream = NULL;
std::ostream* ND::TND280Log::fLogStream = NULL;
std::map<std::string,ND::TND280Log::ErrorPriority> ND::TND280Log::fErrorTraces;
std::map<std::string,ND::TND280Log::LogPriority> ND::TND280Log::fLogTraces;
int ND::TND280Log::fIndentation = 0;

ND::TND280Log::TND280Log() { }
ND::TND280Log::~TND280Log() { }

void ND::TND280Log::SetDebugLevel(const char* trace, 
                              ND::TND280Log::ErrorPriority level) {
    fErrorTraces[trace] = level;
}

ND::TND280Log::ErrorPriority ND::TND280Log::GetDebugLevel(const char* trace) {
    std::map<std::string,ErrorPriority>::iterator elem = fErrorTraces.find(trace);
    if (elem == fErrorTraces.end()) return fErrorPriority;
    return elem->second;
}

namespace {
    std::string MakeTimeStamp() {
        std::string stamp = "Unknown Time";
        time_t t = time(NULL);
        struct tm *local = localtime(&t);
        if (!local) return stamp;
        char localTime[80];
        if (!strftime(localTime,sizeof(localTime),"%c",local)) return stamp;
        struct tm *utc = gmtime(&t);
        if (!utc) return stamp;
        char utcTime[80];
        if (!strftime(utcTime,sizeof(utcTime),"%Y-%m-%d %H:%M (UTC)",utc)) 
            return stamp;
        stamp = localTime;
        stamp += " [";
        stamp += utcTime;
        stamp += "]";
        return stamp;
    }
}

void ND::TND280Log::SetDebugStream(std::ostream* err) {
    ND::TND280Log::fDebugStream = err;
    if (!fDebugStream) return;
    std::ofstream* ofile = dynamic_cast<std::ofstream*>(err);
    if (ofile && !(ofile->is_open())) {
        fDebugStream = NULL;
        ND280Severe("Debug stream is not open.");
    }
    *fDebugStream << std::endl
                  << "##################################################" 
                  << std::endl
                  << "# ERROR LOG STARTS AT: " << MakeTimeStamp()
                  << std::endl
                  << "##################################################" 
                  << std::endl
                  << std::endl;
}

std::ostream& ND::TND280Log::GetDebugStream() {
    if (!ND::TND280Log::fDebugStream ) return GetLogStream();
    return *ND::TND280Log::fDebugStream;
}
    
void ND::TND280Log::SetLogLevel(const char* trace, 
                            ND::TND280Log::LogPriority level) {
    fLogTraces[trace] = level;
}

ND::TND280Log::LogPriority ND::TND280Log::GetLogLevel(const char* trace) {
    std::map<std::string,LogPriority>::iterator elem = fLogTraces.find(trace);
    if (elem == fLogTraces.end()) return fLogPriority;
    return elem->second;
}

void ND::TND280Log::SetLogStream(std::ostream* log) {
    ND::TND280Log::fLogStream = log;
    if (!fLogStream) return;
    std::ofstream* ofile = dynamic_cast<std::ofstream*>(log);
    if (ofile && !(ofile->is_open())) {
        fLogStream = NULL;
        ND280Severe("Log stream is not open.");
    }
    *fLogStream << std::endl
                << "##################################################" 
                << std::endl
                << "# LOG STARTS AT: " << MakeTimeStamp()
                << std::endl
                << "##################################################" 
                << std::endl
                << std::endl;
}

std::ostream& ND::TND280Log::GetLogStream() {
    if (!ND::TND280Log::fLogStream) return std::cout;
    return *ND::TND280Log::fLogStream;
}

void ND::TND280Log::SetIndentation(int i) {
    ND::TND280Log::fIndentation = std::max(i,0);
}

void ND::TND280Log::IncreaseIndentation() {
    ++ND::TND280Log::fIndentation;
}

void ND::TND280Log::DecreaseIndentation() {
    if (ND::TND280Log::fIndentation>0) --ND::TND280Log::fIndentation;
}
    
void ND::TND280Log::ResetIndentation() {
    ND::TND280Log::fIndentation = 0;
}


std::string ND::TND280Log::MakeIndent() {
    if (fIndentation<1) return "";
    std::string indent = "";
    for (int i=0; i<fIndentation; ++i) {
        indent += "..";
    }
    indent += " ";
    return indent;
}

namespace {
    bool TranslateLogLevel(const std::string& name, 
                           ND::TND280Log::LogPriority& level) {
        if (name == "QuietLevel") {
            level = ND::TND280Log::QuietLevel;
            return true;
        }
        if (name == "LogLevel") {
            level = ND::TND280Log::LogLevel;
            return true;
        }
        if (name == "InfoLevel") {
            level = ND::TND280Log::InfoLevel;
            return true;
        }
        if (name == "VerboseLevel") {
            level = ND::TND280Log::VerboseLevel;
            return true;
        }
        return false;
    }

    bool TranslateErrorLevel(const std::string& name, 
                             ND::TND280Log::ErrorPriority& level) {
        if (name == "SilentLevel") {
            level = ND::TND280Log::SilentLevel;
            return true;
        }
        if (name == "ErrorLevel") {
            level = ND::TND280Log::ErrorLevel;
            return true;
        }
        if (name == "SevereLevel") {
            level = ND::TND280Log::SevereLevel;
            return true;
        }
        if (name == "WarnLevel") {
            level = ND::TND280Log::WarnLevel;
            return true;
        }
        if (name == "DebugLevel") {
            level = ND::TND280Log::DebugLevel;
            return true;
        }
        if (name == "TraceLevel") {
            level = ND::TND280Log::TraceLevel;
            return true;
        }
        return false;
    }

    std::ostream* StreamPointer(const std::string& name) {
        if (name == "STDCOUT") return &std::cout;
        if (name == "STDCERR") return &std::cerr;
        if (name[0] != '"') return NULL;
        if (name[name.size()-1] != '"') return NULL;
        std::string file = name.substr(1,name.size()-2);
        std::ofstream* output = new std::ofstream(file.c_str(),
                                                  std::ios::out|std::ios::app);
        if (output->is_open()) return output;
        return NULL;
    }

    bool ReadConfigurationFile(const char* config) {
        std::ifstream input(config);
        if (!input.is_open()) return false;

        int inputLine = 0;
        for (;;) {
            std::string line;
            std::getline(input,line);
            if (input.eof()) break;

            // Save the current line number and cache the value so error
            // messages can be printed later.
            std::string cache(line);
            ++inputLine;
            
            // Strip the comments out of the file.
            std::string::size_type position = line.find("#");
            if (position != std::string::npos) line.erase(position);

            // Strip the white space at the beginning of the line.
            line.erase(0,line.find_first_not_of("\t "));

            // Skip lines that are too short.
            if (line.size()==0) continue;

            // Split the line into fields and a value.
            position = line.find("=");
            if (position == std::string::npos) {
                // Houston, we have a problem... There isn't a value.
                std::cerr << "WARNING: " << config << ":" << inputLine << ": "
                          << "Configuration line missing an '='"
                          << std::endl;
                std::cerr << "  Line: <" << cache << ">"
                          << std::endl;
                std::cerr << "  Configuration line has been skip" << std::endl;
                continue;
            }

            // Split the value off the end of the line.
            std::string value = line.substr(position+1);
            line.erase(position);

            // Strip the white space at the beginning of the value.
            value.erase(0,value.find_first_not_of("\t "));

            // Strip the white space at the end of the value.
            position = value.find_last_not_of("\t ");
            if (position != std::string::npos) value.erase(position+1);
            
            // Strip the white space at the end of the fields.
            position = line.find_last_not_of("\t ");
            if (position != std::string::npos) line.erase(position+1);
            
            // Split the remaining line in to fields.
            std::vector<std::string> fields;
            for (;;) {
                position = line.find(".");
                if (position == std::string::npos) {
                    fields.push_back(line);
                    break;
                }
                fields.push_back(line.substr(0,position));
                line.erase(0,position+1);
            } 
            
            // Process the fields and value.
            if (fields.size() == 2
                && fields[0] == "log"
                && fields[1] == "file") {
                // Set the log file name.
                std::ostream* str = StreamPointer(value);
                if (!str) {
                    std::cerr << "WARNING: " << config << ":" 
                              << inputLine << ": "
                              << "Cannot open log stream."
                              << std::endl;
                    std::cerr << "  Line: <" << cache << ">"
                              << std::endl;
                    std::cerr << "  Configuration line has been skip" 
                              << std::endl;
                    continue;
                }
                ND::TND280Log::SetLogStream(str);
            }
            else if (fields.size() == 2
                     && fields[0] == "error"
                     && fields[1] == "file") {
                // Set the error file name.
                std::ostream* str = StreamPointer(value);
                if (!str) {
                    std::cerr << "WARNING: " << config << ":" 
                              << inputLine << ": "
                              << "Cannot open error stream."
                              << std::endl;
                    std::cerr << "  Line: <" << cache << ">"
                              << std::endl;
                    std::cerr << "  Configuration line has been skip" 
                              << std::endl;
                    continue;
                }
                ND::TND280Log::SetDebugStream(str);
            }
            else if (fields.size() == 3
                     && fields[0] == "log"
                     && fields[1] == "default"
                     && fields[2] == "level") {
                // Set the default log level.
                ND::TND280Log::LogPriority level;
                if (!TranslateLogLevel(value,level)) {
                    std::cerr << "WARNING: " << config << ":" 
                              << inputLine << ": "
                              << "Unknown log level name."
                              << std::endl;
                    std::cerr << "  Line: <" << cache << ">"
                              << std::endl;
                    std::cerr << "  Configuration line has been skip" 
                              << std::endl;
                    continue;
                }
                ND::TND280Log::SetLogLevel(level);
            }
            else if (fields.size() == 3
                     && fields[0] == "error"
                     && fields[1] == "default"
                     && fields[2] == "level") {
                // Set the default error level.
                ND::TND280Log::ErrorPriority level;
                if (!TranslateErrorLevel(value,level)) {
                    std::cerr << "WARNING: " << config << ":" 
                              << inputLine << ": "
                              << "Unknown error level name."
                              << std::endl;
                    std::cerr << "  Line: <" << cache << ">"
                              << std::endl;
                    std::cerr << "  Configuration line has been skip" 
                              << std::endl;
                    continue;
                }
                ND::TND280Log::SetDebugLevel(level);
            }
            else if (fields.size() == 3
                     && fields[0] == "log"
                     && fields[2] == "level") {
                // Set the log level.
                ND::TND280Log::LogPriority level;
                if (!TranslateLogLevel(value,level)) {
                    std::cerr << "WARNING: " << config << ":" 
                              << inputLine << ": "
                              << "Unknown log level name."
                              << std::endl;
                    std::cerr << "  Line: <" << cache << ">"
                              << std::endl;
                    std::cerr << "  Configuration line has been skip" 
                              << std::endl;
                    continue;
                }
                ND::TND280Log::SetLogLevel(fields[1].c_str(),level);
            }
            else if (fields.size() == 3
                     && fields[0] == "error"
                     && fields[2] == "level") {
                // Set the error level.
                ND::TND280Log::ErrorPriority level;
                if (!TranslateErrorLevel(value,level)) {
                    std::cerr << "WARNING: " << config << ":" 
                              << inputLine << ": "
                              << "Unknown error level name."
                              << std::endl;
                    std::cerr << "  Line: <" << cache << ">"
                              << std::endl;
                    std::cerr << "  Configuration line has been skip" 
                              << std::endl;
                    continue;
                }
                ND::TND280Log::SetDebugLevel(fields[1].c_str(),level);
            }
            else {
                std::cerr << "WARNING: " << config << ":" << inputLine << ": "
                          << "Unknown command."
                          << std::endl;
                std::cerr << "  Line: <" << cache << ">"
                          << std::endl;
                std::cerr << "  Configuration line has been skip" << std::endl;
            }                
        }

        return true;
    }
}

void ND::TND280Log::Configure(const char* conf) {
    // Try to read a local configuration file.  
    ReadConfigurationFile("./nd280log.config");
    if (conf) {
        bool success = ReadConfigurationFile(conf);
        if (!success) ND280Log("ND280Log configuration file was not read.");
    }
}
