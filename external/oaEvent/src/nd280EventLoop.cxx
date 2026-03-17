#include <iostream>
#include <sstream>
#include <limits>
#include <memory>
#include <unistd.h>
#include <csignal>
#include <cmath>
#include <string>
#include <vector>
#include <map>

#include <TROOT.h>

#include "EoaCore.hxx"

#include "TND280Event.hxx"
#include "TMidasFile.hxx"
#include "TND280Input.hxx"
#include "TND280Output.hxx"
#include "TOADatabase.hxx"
#include "THandleHack.hxx"
#include "TND280Log.hxx"
#include "TMemoryUsage.hxx"
#include "nd280EventLoop.hxx"
#include "TND280Tags.hxx"

namespace {
    void nd280EventLoopUsage(std::string programName,
                             ND::TND280EventLoopFunction& userCode,
                             int readCount) {
        std::cout << "usage: " << programName
                  << " [options] [input-file] ..."
                  << std::endl
                  << std::endl;

        std::cout << "Options:" << std::endl;

        std::cout << "    -o <file>         Set the name of an output file."
                  << std::endl
                  << "                        Multiple output files can"
                  << " be provided."
                  << std::endl;

        std::cout << "    -a                Read all events";
        if (readCount<1) std::cout << " [Default]";
        std::cout << std::endl;

        std::cout << "    -c <file>         Set the logging config file name"
                  << std::endl;

        std::cout << "    -d                Increase the debug level"
                  << std::endl;

        std::cout << "    -D <name>=[error,severe,warn,debug,trace]"
                  << std::endl
                  << "                      Change the named debug level"
                  << std::endl;

        std::cout << "    -f <event>        Look for an event number"
                  << std::endl;

        std::cout << "    -f <run>,<event>  Look for a run and event number"
                  << std::endl;

        std::cout << "    -G <file>         Force a geometry file"
                  << std::endl;

        std::cout << "    -g                Don't save geometry in output"
                  << std::endl;

        std::cout << "    -H                Debug THandle (slow)"
                  << std::endl;

        std::cout << "    -m                MIDAS input file"
                  << std::endl;

        std::cout << "    -n <cnt>          Only read <cnt> events";
        if (readCount>0) std::cout << "  [Default: " << readCount << "]";
        std::cout << std::endl;

        std::cout << "    -q                Decrease the verbosity"
                  << std::endl;

        std::cout << "    -r                ND280 ROOT file"
                  << std::endl;
        std::cout << "    -R                Replace output file if it exists"
                  << std::endl;

        std::cout << "    -s <cnt>          Skip <cnt> events"
                  << std::endl;

        std::cout << "    -t [bpTPEFecC]    Only accept these triggers"
                  << std::endl
                  << "                        b  Beam triggers"
                  << std::endl
                  << "                        p  Pedestal/noise triggers"
                  << std::endl
                  << "                        T  TPC laser triggers"
                  << std::endl
                  << "                        P P0D light injection triggers"
                  << std::endl
                  << "                        E  ECal LED triggers"
                  << std::endl
                  << "                        F  FGD LED triggers"
                  << std::endl
                  << "                        e  Electronics calib triggers"
                  << std::endl
                  << "                        c  TFB cosmic ray triggers"
                  << std::endl
                  << "                        C  FGD cosmic ray triggers"
                  << std::endl;
        std::cout << "    -T <file>         Set the tag file name"
                  << std::endl;
        std::cout << "    -u                Log the memory and CPU usage"
                  << std::endl;

        std::cout << "    -v                Increase the verbosity"
                  << std::endl;

        std::cout << "    -V <name>=[quiet,log,info,verbose]"
                  << std::endl
                  << "                      Change the named log level"
                  << std::endl;

        std::cout << "    -O <opt>[=<val>]  Set an option for the user code"
                  << std::endl;

        std::cout << std::endl;

        userCode.Usage();
        std::exit(1);
    }
};

int ND::nd280EventLoop(int argc, char** argv,
                       ND::TND280EventLoopFunction& userCode,
                       int defaultReadCount) {
    if (defaultReadCount<0) defaultReadCount = 0;
    int readCount = defaultReadCount;
    int skipCount = 0;
    int debugLevel = 0;
    std::map<std::string, ND::TND280Log::ErrorPriority> namedDebugLevel;
    int logLevel = -1; // Will choose default logging level...
    std::map<std::string, ND::TND280Log::LogPriority> namedLogLevel;
    std::string programName = argv[0];
    char *configName = NULL;
    char *tagFileName = NULL;
    std::vector<std::string> outputNames;
    std::vector<TND280Output*> outputFiles;
    bool preventSavedGeometry = false;
    std::string geometryFile = "";
    int targetRun = -1;
    int targetEvent = -1;
    int exitStatus = 0;
    bool   overwriteOutput{kFALSE};
    TMemoryUsage memoryUsage;
    ND::TND280TagControl tagControl;
    tagControl.StoreCommandLine(argc,argv);

    // If this is not zero, then only accept triggers matched in this mask.
    unsigned int validTriggersMask = 0;
    signal(SIGSEGV, SIG_DFL);

    enum {
        kRootFile,
        kMIDASFile
    } fileType = kRootFile;

    // Process the options.
    for (;;) {
        int c = getopt(argc, argv, "ac:dD:f:G:gHmn:o:O:qrRs:t:T:uvV:");
        if (c<0) break;
        switch (c) {
        case 'a':
        {
            // readcount of zero means read the entire file.
            readCount = 0;
            break;
        }
        case 'c':
        {
            configName = strdup(optarg);
            break;
        }
        case 'd':
        {
            // increase the debugging level.
            ++debugLevel;
            break;
        }
        case 'D':
        {
            // Set the debug level for a named trace.
            std::string arg(optarg);
            std::size_t sep = arg.find("=");
            if (sep != std::string::npos) {
                std::string name = arg.substr(0,sep);
                std::string levelName = arg.substr(sep+1);
                switch (levelName[0]) {
                case 'e': case 'E':
                    namedDebugLevel[name.c_str()] = ND::TND280Log::ErrorLevel;
                    break;
                case 's': case 'S':
                    namedDebugLevel[name.c_str()] = ND::TND280Log::SevereLevel;
                    break;
                case 'w': case 'W':
                    namedDebugLevel[name.c_str()] = ND::TND280Log::WarnLevel;
                    break;
                case 'd': case 'D':
                    namedDebugLevel[name.c_str()] = ND::TND280Log::DebugLevel;
                    break;
                case 't': case 'T':
                    namedDebugLevel[name.c_str()] = ND::TND280Log::TraceLevel;
                    break;
                default:
                    nd280EventLoopUsage(programName,userCode,defaultReadCount);
                }
            }
            break;
        }
        case 'f':
        {
            // Look for a particular run and event number in the file.
            std::string arg(optarg);
            if (arg.find(',')!=std::string::npos) {
                std::istringstream input(arg);
                char c;
                input >> targetRun;
                input >> c;
                input >> targetEvent;
            }
            else {
                std::istringstream input(arg);
                input >> targetEvent;
            }
            break;
        }
        case 'G':
        {
            // Force a particular geometry file.
            geometryFile = optarg;
            break;
        }
        case 'g':
        {
            // prevent the geometry from being saved.
            preventSavedGeometry = true;
            break;
        }
        case 'H':
        {
            // Debug the handles.
            EnableHandleRegistry(true);
            break;
        }
        case 'm':
        {
            fileType = kMIDASFile;
            break;
        }
        case 'n':
        {
            std::istringstream tmp(optarg);
            tmp >> readCount;
            break;
        }
        case 'o':
        {
            outputNames.push_back(optarg);
            break;
        }
        case 'O':
        {
            std::string option = optarg;
            std::string value = "";
            // If the option is "<option>=<value>", split this into two
            // strings "<option>" and "<value>"
            std::string::size_type equals = option.find_first_of("=");
            if (equals != std::string::npos) {
                value = option.substr(equals+1,std::string::npos);
                option = option.substr(0,equals);
            }
            if (!userCode.SetOption(option,value)) {
                std::cerr << "ERROR: Illegal user option: " << option;
                if (value != "") std::cerr << "   Value: " << value;
                std::cerr << std::endl;
                nd280EventLoopUsage(programName,userCode,defaultReadCount);
            }
            break;
        }
        case 'q':
        {
            // decrease the verbosity level.
            if (logLevel>0) --logLevel;
            else logLevel = 0;
            break;
        }
        case 'r':
        {
            fileType = kRootFile;
            break;
        }
        case 'R':
        {
            overwriteOutput = kTRUE;
            break;
        }
        case 's':
        {
            std::istringstream tmp(optarg);
            tmp >> skipCount;
            break;
        }
        case 't':
        {
            std::cout << "Only following triggers will be accepted:";
            for (char *trig = optarg; *trig !=0 ; ++trig) {
                switch (*trig) {
                case 'b':
                    std::cout << " beam-spill";
                    validTriggersMask |= ND::Trigger::kBeamSpill;
                    break;
                case 'p':
                    std::cout << " pedestal";
                    validTriggersMask |= ND::Trigger::kPedestal;
                    break;
                case 'T':
                    std::cout << " TPC-laser";
                    validTriggersMask |= ND::Trigger::kTPCLaser;
                    break;
                case 'P':
                    std::cout << " P0D-LED";
                    validTriggersMask |= ND::Trigger::kP0DLED;
                    break;
                case 'E':
                    std::cout << " ECal-LED";
                    validTriggersMask |= ND::Trigger::kECalLED;
                    break;
                case 'F':
                    std::cout << " FGD-LED";
                    validTriggersMask |= ND::Trigger::kFGDLED;
                    break;
                case 'e':
                    std::cout << " FEE-calibration";
                    validTriggersMask |= ND::Trigger::kFEECalib;
                    break;
                case 'c':
                    std::cout << " TFB-cosmic";
                    validTriggersMask |= ND::Trigger::kTFBCosmic;
                    break;
                case 'C':
                    std::cout << " FGD-cosmic";
                    validTriggersMask |= ND::Trigger::kFGDCosmic;
                    break;
                default:
                    std::cout << std::endl
                              << "Invalid trigger type (" << *trig << ")"
                              << std::endl;
                    nd280EventLoopUsage(programName,userCode,defaultReadCount);
                }
            }
            std::cout << std::endl;
            break;
        }
        case 'T':
        {
            tagFileName = strdup(optarg);
            break;
        }
        case 'u':
        {
          // Enable logging of memory usage
          memoryUsage.Enable();
          break;
        }
        case 'v':
        {
            // increase the verbosity level.
            if (logLevel>0) ++logLevel;
            else logLevel = 2;
            break;
        }
        case 'V':
        {
            // Set the debug level for a named trace.
            std::string arg(optarg);
            std::size_t sep = arg.find("=");
            if (sep != std::string::npos) {
                std::string name = arg.substr(0,sep);
                std::string levelName = arg.substr(sep+1);
                switch (levelName[0]) {
                case 'q': case 'Q':
                    namedLogLevel[name.c_str()] = ND::TND280Log::QuietLevel;
                    break;
                case 'l': case 'L':
                    namedLogLevel[name.c_str()] = ND::TND280Log::LogLevel;
                    break;
                case 'i': case 'I':
                    namedLogLevel[name.c_str()] = ND::TND280Log::InfoLevel;
                    break;
                case 'v': case 'V':
                    namedLogLevel[name.c_str()] = ND::TND280Log::VerboseLevel;
                    break;
                default:
                    nd280EventLoopUsage(programName,userCode,defaultReadCount);
                }
            }
            break;
        }
        default:
            nd280EventLoopUsage(programName,userCode,defaultReadCount);
        }
    }

    // Set up the logging code.
    TND280Log::Configure(configName);

    if (logLevel == 0) {
        TND280Log::SetLogLevel(TND280Log::QuietLevel);
    }
    else if (logLevel == 1) {
        TND280Log::SetLogLevel(TND280Log::LogLevel);
        ND280Log("Set log level to LogLevel");
    }
    else if (logLevel == 2) {
        TND280Log::SetLogLevel(TND280Log::InfoLevel);
        ND280Info("Set log level to InfoLevel");
    }
    else if (logLevel >= 3) {
        TND280Log::SetLogLevel(TND280Log::VerboseLevel);
        ND280Verbose("Set log level to VerboseLevel");
    }

    for (std::map<std::string,ND::TND280Log::LogPriority>::iterator i
             = namedLogLevel.begin();
         i != namedLogLevel.end();
         ++i) {
        ND::TND280Log::SetLogLevel(i->first.c_str(), i->second);
    }

    if (debugLevel == 1) {
        TND280Log::SetDebugLevel(TND280Log::WarnLevel);
        ND280Warn("Set debug level to WarnLevel");
    }
    else if (debugLevel == 2) {
        TND280Log::SetDebugLevel(TND280Log::DebugLevel);
        ND280Debug("Set debug level to DebugLevel");
    }
    else if (debugLevel >= 2) {
        TND280Log::SetDebugLevel(TND280Log::TraceLevel);
        ND280Trace("Set debug level to TraceLevel");
    }

    for (std::map<std::string,ND::TND280Log::ErrorPriority>::iterator i
             = namedDebugLevel.begin();
         i != namedDebugLevel.end();
         ++i) {
        ND::TND280Log::SetDebugLevel(i->first.c_str(), i->second);
    }

    if (!outputNames.empty()) {
        for (std::vector<std::string>::iterator n = outputNames.begin();
             n != outputNames.end();
             ++n) {
            TString option("NEW");
            if(overwriteOutput)
                option="RECREATE";
            TND280Output* output = new TND280Output((*n).c_str(),option);
            if (!output->IsOpen()) {
                std::cerr << "ERROR: Output file not open" << std::endl;
                std::cerr << "(Use option -R to replace an existing file)" << std::endl;
                exit(2);
            }
            outputFiles.push_back(output);
        }
    }

    // Check that there is an input file to open.
    if (argc<=optind) {
        std::cerr << "ERROR: No input file" << std::endl << std::endl;
        nd280EventLoopUsage(programName,userCode,defaultReadCount);
    }

    if (!outputFiles.empty()) outputFiles.front()->cd();
    userCode.Initialize();

    // If readcount is zero, that means read the entire file.
    if (readCount<1) readCount = std::numeric_limits<int>::max();

    if (geometryFile != "") {
        TOADatabase::Get().SetGeometryOverride(geometryFile);
    }

    // read in tag information from file
    if(tagFileName!=NULL)
    {
        tagControl.ReadTagsFromFile(tagFileName);
    }

    int totalRead = 0;
    int totalWritten = 0;
    double nextOutput = 10;
    while (optind<argc) {
        // Check to see if we need to read more events.
        if (readCount<=totalRead) break;

        // Open input the file
        std::string fileName = argv[optind++];
        int lastEventId = -1;
        int lastRunId = -1;
        try {
            std::unique_ptr<ND::TVInputFile> input;
            switch (fileType) {
            case kRootFile:{
                TFile* file = TFile::Open(fileName.c_str(),"OLD");
                input.reset(new ND::TND280Input(file));
                break;
            }
            case kMIDASFile:
            {
                ND::TMidasFile* midas = new ND::TMidasFile();
                if (not midas->Open(fileName.c_str())) {
                    std::cout << "ERROR: MIDAS file not open" << std::endl;
                }
                input.reset(midas);
                break;
            }
            default:
                std::cout << "ERROR: Unknown file type" << std::endl;
            }

            if (!input->IsOpen()) {
                std::cout << "ERROR: File not found " << fileName << std::endl;
                continue;
            }

            if (!outputFiles.empty()) outputFiles.front()->cd();
            userCode.BeginFile(input.get());

            std::unique_ptr<TND280Event> event(input->FirstEvent());

            // Position to the first event in the file to read.  This might
            // leave the file positioned at the end of file.
            if (skipCount>0) {
                event.reset(input->NextEvent(skipCount-1));
                int position = input->GetPosition();
                if (position>0) skipCount = skipCount - position;
            }

            // Process the events in the file.
            for (;!input->EndOfFile();event.reset(input->NextEvent())) {

                if (targetRun>=0 && event->GetRunId()<targetRun) continue;
                else targetRun = -1;

                if (targetEvent>=0 && event->GetEventId()<targetEvent) continue;
                else targetEvent = -1;

                if (validTriggersMask != 0
                    && (event->GetHeader().GetTriggerWord()
                        == ND::TND280Event::Header::Invalid)
                    && event->GetHeader().IsData()) {
                    static int messageShown = 3;
                    if (messageShown>0) {
                        ND280Error("Attempting to apply trigger mask"
                                  << " to data with invalid trigger word");
                        -- messageShown;
                    }
                }

                // Only filter if a trigger bit mask was provided, the event
                // is data (not a message, run begin or end), and has a valid
                // trigger word (not MC unless the trigger is being
                // simulated).
                if (validTriggersMask != 0
                    && event->GetHeader().IsData()
                    && (event->GetHeader().GetTriggerWord()
                        != ND::TND280Event::Header::Invalid)
                    && 0 == (event->GetHeader().GetTriggerBits()
                             & validTriggersMask)) {
                    ND280Verbose("Skip event with trigger mask of "
                                 << std::hex
                                 << event->GetHeader().GetTriggerBits());
                    continue;
                }

                ++totalRead;
                lastEventId = event->GetEventId();
                lastRunId = event->GetRunId();
                memoryUsage.LogMemory();
                // store configuration information read in from file in the event /tags area
                tagControl.AddConfiguration(*event);

                int saveEvent = -1;
                try {
                    if (!outputFiles.empty()) outputFiles.front()->cd();
                    saveEvent = userCode.Process(*event,outputFiles.size());
                }
                catch (ENextEventLoopFile& ex) {
                    break;
                }

                if (!preventSavedGeometry) {
                    int geomFile = std::max(0,saveEvent);
                    // Check if the geometry should be saved.
                    if (0 <= saveEvent
                        && saveEvent < (int) outputFiles.size()) {
                        try {
                            outputFiles[geomFile]
                                ->WriteGeometry(TOADatabase::Get().Geometry());
                        }
                        catch (ENoGeometry const&) {
                            ND280Severe("Geometry not saved in output");
                        }
                        catch (EOADatabase const&) {
                            ND280Severe("Geometry not saved in output");
                        }
                    }
                }

                if (0 <= saveEvent && saveEvent < (int) outputFiles.size()) {
                    // Add program information to the event
                    // Do it here to avoid confusion.
                    tagControl.AddProgramVersionNumberTags(*event);
                    outputFiles[saveEvent]->WriteEvent(*event);
                    ++totalWritten;
                }

                event.reset(NULL);
                if (!CleanHandleRegistry()) {
                    DumpHandleRegistry();
                    std::cout << "WARNING: (ND280) Memory Leak in "
                              << std::endl;
                    std::cout << "    File(event,run): " << fileName
                              << ":("
                              << lastEventId
                              << "," << lastRunId << ")" << std::endl;
                }

                if (totalRead>(nextOutput-0.5)) {
                    nextOutput *= std::sqrt(10);
                    if (nextOutput - totalRead > 1000) {
                        nextOutput = totalRead + 1000;
                    }
                    ND280Log("Events Processed: " << totalRead);
                }

                // Check to see if we have read enough events.
                if (readCount<=totalRead) break;
            }

            if (!CleanHandleRegistry()) {
                DumpHandleRegistry();
                std::cout << "WARNING: (ND280) Memory Leak after finishing "
                          << std::endl;
                std::cout << "       " << fileName << std::endl;
            }

            if (!outputFiles.empty()) outputFiles.front()->cd();
            userCode.EndFile(input.get());
            input->CloseFile();
        }
        catch (std::exception& except) {
            std::cout << "ERROR: (ND280) Uncaught exception in "
                      << fileName << std::endl;
            std::cout << "      " << except.what() << std::endl;
            if (!CleanHandleRegistry()) {
                DumpHandleRegistry();
                std::cout << "WARNING: (ND280) Memory Leak after finishing "
                          << std::endl;
                std::cout << "       " << fileName << std::endl;
            }
            exitStatus = 3;
            break;
        }
        catch (...) {
            // Don't crash on an error, but try to go to the next file.
            std::cout << "ERROR: (ND280) Uncaught exception in "
                      << fileName << std::endl;
            if (!CleanHandleRegistry()) {
                DumpHandleRegistry();
                std::cout << "WARNING: (ND280) Memory Leak after finishing "
                          << std::endl;
                std::cout << "       " << fileName << std::endl;
            }
            exitStatus = 4;
            break;
        }
    }

    if (!outputFiles.empty()) {
        for (std::vector<ND::TND280Output*>::iterator file
                 = outputFiles.begin();
             file != outputFiles.end();
             ++ file) {
            userCode.Finalize(*file);
            memoryUsage.Write(*file);
        }
    }
    else {
        userCode.Finalize((ND::TND280Output*) NULL);
        memoryUsage.Write((ND::TND280Output*) NULL);
    }

    std::cout << "Total Events Read: " << totalRead << std::endl;
    if (!outputFiles.empty()) {
        std::cout << "Total Events Written: " << totalWritten << std::endl;
        for (std::vector<TND280Output*>::iterator file = outputFiles.begin();
             file != outputFiles.end();
             ++file) {
            (*file)->Close();
        }
    }

    if (exitStatus > 0) {
        ND280Error("Terminating with non-zero exit status.");
        std::exit(exitStatus);
    }

    return 0;
}
