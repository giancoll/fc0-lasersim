#ifndef TND280Log_hxx_seen
#define TND280Log_hxx_seen

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

namespace ND {
    class TND280Log;
}

////////////////////////////////////////////////////////////
// $Id$

/// Provide control over log and error output.  The TND280Log class provides a
/// set of static methods which control the amount of output from the ND280
/// logging macros.  These macros come in two varieties (logging macros, and
/// error reporting macros).  The level of output from the logging and error
/// macros is controlled separately.
///
/// The logging macros print the output to the log file with a simple header
/// which allows them to be located using a search tool (like grep).  All of
/// these macros accept a \ref streamish and can produce multiple lines of
/// output.  The logging macros are:
///
/// - ND280Log() -- Print output at the LogLevel output level.  This is for
///   messages that should be written to the output file during
///   production.  
/// - ND280Info() -- Print output at the InfoLevel output level.
/// - ND280Verbose() -- Print output at the VerboseLevel output level.
///
/// The indentation levels of log messages can be controlled using the
/// ND::TND280Log::IncreaseIndentation() and
/// ND::TND280Log::DecreaseIndentation() static methods.  These should be used
/// in pairs.
///
/// The error macros print output to the error file with a header indicating
/// the error level (ERROR, SEVERE, WARN, DEBUG, or TRACE), the file and the
/// line of code where the message originated.  Error macros should be used to
/// report any problem found during execution.  They are particularly useful
/// for debugging.  The syntax for the error macros is the same as for the
/// logging macros.  The error macros are:
///
/// - ND280Error() -- Print output at the ErrorLevel output level.  This
///   should be reserved for messages printed right before you expect the
///   program to crash. 
/// - ND280Severe() -- Print output at the SevereLevel output level.  This
///   should be reserved for error conditions where event data is going to be
///   lost, or the event will contain incorrect information.
/// - ND280Warn() -- Print output at the WarnLevel output level.  This should
///   be used when a correctable, but unexpected, problem is found with an
///   event. 
/// - ND280Debug() -- Print output at the DebugLevel output level.  This
///   should be used for general debugging output.
/// - ND280Trace() -- Print output at the TraceLevel output level.  This
///   should be used for very verbose debugging output.
///
/// In addition to the default logging and error macros, there are macros
/// that take a "trace" name.  The level of output from each type of trace can
/// be controlled separately.  The named variety of macro takes two arguments
/// (a \ref trace and a \ref streamish).  The named macros are
/// ND280NamedLog(), ND280NamedInfo(), ND280NamedVerbose(), ND280NamedError(),
/// ND280NamedSevere(), ND280NamedWarn(), ND280NamedDebug(), and
/// ND280NamedTrace().
///
/// \section defaultLogging Using the Default Logging Macros
///
/// The logging (and error) output macros are used like functions, except
/// they take arguments that can be output to a stream variable (a so called
/// \ref streamish). 
/// 
/// \code
/// int i = 42;
/// double pi = 3.1415;
/// 
/// ND280Log("The answer is " << i);
/// ND280Info("Multi-line output is" << std::endl << "     very useful.");
/// ND280Verbose("Have a piece of " << pi);
/// \endcode
///
/// If the logging level is set to TND280Log::LogLevel produces: 
///
/// \code
/// % The answer is 42.
/// \endcode
///
/// But if the logging level is set to TND280::VerboseLevel, this writes:
///
/// \code
/// % The answer is 42.
/// %% Multi-line output is
///      very useful.
/// %%% Have a piece of 3.1415
/// \endcode
///
/// The logging level can be set using the "nd280log.config" file (see \ref
/// nd280logConfig) to define a log level
///
/// \code
/// log.default.level = VerboseLevel # also LogLevel or InfoLevel
/// \endcode
///
/// \section namedLogging Using the Named Logging Macros
///
/// The named logging macros are used in the same way as the unnamed macros,
/// except that you must provide a string as the first argument.  This string
/// is then used to define a "trace" which can be turned on explicitly without
/// causing output from other logging statements.  
///
/// \code
/// int i = 42;
/// double pi = 3.1415;
/// 
/// ND280NamedLog("traceName",
///               "The answer is " << i 
///                    << ".  So we might as well have " << pi);
/// ND280NamedInfo("traceName",
///               "Multi-line output is" << std::endl
///                    << "     very useful.");
/// ND280NamedInfo("anotherTrace","Won't be printed");
/// \endcode
///
/// If the "traceName" logging level is set to TND280Log::InfoLevel or above,
/// this produces:
///
/// \code
/// # The answer is 42. So we might as well have 3.1415
/// # Multi-line output is
///      very useful.
/// \endcode
///
/// The logging level for traceName can be set using the configuration file
/// statement. 
///
/// \code
/// log.traceName.level = InfoLevel
/// \endcode
///
/// \section nd280logConfig The TND280Log Configuration File
///
/// The configuration of the TND280Log object can be handled using static
/// methods, or using a configuration file that is read by a call to the
/// TND280Log::Configure() method.  When the TND280Log::Configure() method is
/// called, it will first try to read "nd280log.config" in the local
/// directory.  If TND280Log::Configure() is called with a file name, the
/// named configuration file will be read after the nd280log.config file and
/// can override any settings.
///
/// The TND280Log configuration file uses a simple line oriented syntax.  
///
/// - Comments are begun with "#"
/// - Configuration statements are case-sensitive
/// - Each line has one configuration statement
/// - The configuration statement syntax is
///    - a field name (defined below)
///    - an equal sign
///    - a field value
///
/// The legal field names are 
/// 
/// - log.file -- The name of a log file.  This can be either a string
///  enclosed in double quotes, STDCOUT, or STDCERR.  If the file exists, then
///  output will be appended to the end.  If the file doesn't exist, it will
///  be created.
///
/// - error.file -- The name of the error file.  This has the same syntax as
///  log.file.  If an error file isn't provided, then the error output
///  will go to the same destination as the logging output.
///
/// - log.default.level -- The default logging level.  This accepts the name
///  of a logging level (QuietLevel, LogLevel, InfoLevel, or VerboseLevel).
///
/// - error.default.level -- The default error level.  This accepts the
///  name of a error level (SilentLevel, ErrorLevel, SevereLevel,
///  WarnLevel, DebugLevel, TraceLevel).  
///
/// - log.[trace].level -- Set the logging level for the named trace.  The
///  level names are the same as log.default.level.
///
/// - error.[trace].level -- Set the error level for the named trace.  The
///  level names are the same as error.default.level.
///
/// An example of the nd280log.config file shows how it might be used.  This
/// file causes the log messages to be printed in "output.log", and the error
/// messages to be printed in "output.err".  The default log level is set to
/// TND280Log::InfoLevel so that messages are written from all ND280Log(),
/// ND280NamedLog(), ND280Info(), and ND280NamedInfo() macros.  The default
/// error level is set to TND280Log::SevereLevel so messages are written
/// from all ND280Error(), ND280NamedError(), ND280Severe(), and
/// ND280NamedSevere() macros.  The error level for the "myTrace" trace is
/// set to TND280Log::DebugLevel, so messages are written for any
/// ND280NamedWarn() and ND280NamedDebug() macro which has a trace argument of
/// "myTrace" (e.g. ND280NamedWarn("myTrace","some output") will write "some
/// output" into the log file).
///
/// \code
/// # An atypical example nd280log.config
/// log.file = "output.log"       # Set the name of the output file.
/// error.file = "output.err"     # Set the name of the error file.
///
/// log.default.level = InfoLevel
/// error.default.level = SevereLevel
///
/// error.myTrace.level = DebugLevel # set myTrace to use TraceLevel.
/// 
/// # End of nd280log.config
/// \endcode
///
/// Here is an example of a typical nd280log.config file that would be used
/// during debuging.  This assumes that your routine is using the \ref trace
/// MyRoutine (a bad name!), and prints all of the error messages to the
/// terminal. 
///
/// \code 
/// # A Typical nd280log.config file.  Copy this into your directory as you
/// #   are debugging a program. 
///
/// # Uncomment the next line to save the log and debugging output to
/// # output.log. 
/// # log.file = "output.log"
///
/// # Print all of the possible log messages.
/// log.default.level = VerboseLevel
///
/// # Only print error messages of WarnLevel or above.
/// error.default.level = WarnLevel
///
/// # Print all of the debugging messages from my code.
/// error.MyRoutine.level = TraceLevel
///
/// # End of nd280log.config
/// \endcode
///
/// \section logLevel Log Levels
///
/// The available log output levels are:
///
/// - TND280Log::QuietLevel -- No log output.
///
/// - TND280Log::LogLevel -- This is for messages that should be printed in
///    the log file.  Messages from ND280Log(), and ND280NamedLog() will be
///    printed.
/// 
/// - TND280Log::InfoLevel -- This is for messages that will be useful in log
///   files, but may be skipped during large production jobs.  Messages from
///   ND280Info(), and ND280NamedInfo() will be printed.
///
/// - TND280Log::VerboseLevel -- This is for messages that curious users can
///   use to track the progress of local jobs, but which do not belong in a
///   production log file.  Messages from ND280Verbose(), and
///   ND280NamedVerbose() will be printed.
///
/// \section debugLevel Error Levels
///
/// The available error levels are:
///
/// - TND280Log::SilentLevel -- No error output. Exception: Message from
///   ND280Error() are not suppressed even when the error output level is
///   set to SilentLevel.  These messages can be suppressed by defining
///   ND280_ERROR_OUTPUT to false in the source code
///
/// - TND280Log::ErrorLevel -- This should be reserved for messages that are
///   printed just before you expect a program to crash, and which should
///   never be supressed.  Messages from ND280Error() and ND280NamedError()
///   are printed at this level.
///
/// - TND280Log::SevereLevel -- This is for messages that are printed when an
///   event has triggered a problem where the code is going to produce an
///   incorrect result, but probably won't crash.  Messages from ND280Severe()
///   and ND280NamedSevere() are printed.
///
/// - TND280Log::WarnLevel -- This is for messages printed when a problem has
///   been found, but it is unlikely to cause significant problems in the
///   analysis.  Messages from ND280Warn() and ND280NamedWarn() are printed.
///
/// - TND280Log::DebugLevel -- This is for general debugging messages.
///   Messages from ND280Debug() and ND280NamedDebug() are printed.
///
/// - TND280Log::TraceLevel -- This is for really verbose debugging messages.
///   Messages from ND280Trace() and ND280NamedTrace() are printed.
///
/// TND280Log is singleton class that controls the level of output from the
/// ND280 error and logging statements.  All of the user visible
/// functions are static.
///
/// \section trace Trace Name
///
/// A "trace name" is a string that can be used to define a group of log (or
/// error) messages that will be printed at the same logging (or error)
/// level.  These are used with the "Named" variant of the log macros:
///
/// \code
/// ND280NamedLog("traceArgument","some output");
/// \endcode
///
/// \section streamish Streamish Argument
///
/// A "streamish argument" is an argument to a logging macro that can be
/// compiled into a stream output statement.  For example, a call
///
/// \code
/// ND280Log("The answer is " << 42);
/// \endcode
///
/// has one streamish argument, and will print "A streamish argument" into the
/// log file.  The TND280Log macros will accept any set of << operator
/// arguments that could be printed to std::cout.  NOTE: the streamish
/// argument doesn't start with a << operator.
///
/// \section logRationale Rationale
///
/// Writing messages to track the progress of a job, or to help debug software
/// has a few specific requirements.  In particular, it needs to meet (at
/// least) the following desiderata.
///
/// -# The amount of output should be controllable at run-time.
/// -# The generated code should be very efficient and impose as small a CPU
///    requirement as possible.
///    -# When a message is "switched off" at run-time, the text for the
///    message should not be formatted and none of the routines required to
///    generate the text should be called.
/// -# It must be possible for the log writing code to be removed at
///    compilation time so that it will have no affect on critical sections of
///    code.   
/// -# It must be possible to direct log and error output to separate
///    streams. 
/// -# The type safety and formating of the normal C++ stream operators should
///    be leveraged.
///
/// These desiderata basically the usual log/error output scheme where the
/// basic log code looks like this:
///
/// \code
/// #ifdef INCLUDE_DEBUGGING_CODE
/// if (0<outputLevel) {
///   myStream << "debugging output " << slowFunction() << std::endl;
/// }
/// #endif
/// \endcode
///
/// where outputLevel and myStream are global variables.  This meets the first
/// requirement since outputLevel is a global variable that can be changed at
/// run-time.  The second requirement is met since slowFunction is only called
/// when outputLevel is greater than zero.  The second requirement is met
/// since code is only generated when INCLUDE_DEBUGGING_CODE is set.
///
/// The final desiderata implies that the ideal source code interface would
/// result in code something like:
///
/// \code
/// logStream << "A log message" << std::endl;
/// debugStream << "An informational message" << std::endl;
/// infoStream << "Result of a function " << SlowFunction() << std::endl;
/// \endcode
/// 
/// Unfortunately, the way C++ processes streams means that this type of
/// interface can't mean the disiderata "2.1", and "3".  For instance, even if
/// the run-time verbosity is set so that infoStream will not generate output,
/// the function "SlowFunction()" will be called.  It's also not possible for
/// this sort of code to be removed during compilation.  
///
/// While C Macros are generally discouraged in C++, they provide the only
/// mechanism that meets the design requirements for logging.  This results in
/// an interface similar to the one defined by TND280Log.  
///
/// \subsection logRationaleOther Why not use package X?
///
/// There are several log writing packages available for C++, so why not use
/// one of them?  It turns out that several (for an example, google "log4cpp")
/// define an unsatisfactory streams based interface.  Other packages I'm
/// aware of tend to have very rich features and a very large number of lines
/// (i.e. log4cplus has 11 KLOC).  Many of these packages also bring
/// significant build requirements.  By comparison, TND280Log is less than 700
/// lines of code.  
/// 
class ND::TND280Log {
public:
    ~TND280Log();

    typedef enum {SilentLevel,
                  ErrorLevel, 
                  SevereLevel, 
                  WarnLevel, 
                  DebugLevel, 
                  TraceLevel} ErrorPriority;
    
    typedef enum {QuietLevel, 
                  LogLevel, 
                  InfoLevel, 
                  VerboseLevel} LogPriority;

    /// Cause the logging and error output streams to be initialized.
    /// Basic configuration for logging occurs automatically, so this need not
    /// be called.  If this is called, then it first trys to read the
    /// nd280log.config file in the current directory.  If Configure is called
    /// with an argument, then the named file will be read after reading the
    /// local configuration file.  If Configure is called with a named file
    /// then that file must exist.  The \ref nd280logConfig syntax is
    /// described elsewhere.
    static void Configure(const char* conf=NULL);

    /// Set the output stream to be used for error messages.
    static void SetDebugStream(std::ostream* err);

    /// Set the default debugging level.  The level parameter takes a value
    /// with type TND280Log::ErrorPriority.
    static void SetDebugLevel(ErrorPriority level) {fErrorPriority = level;}

    /// Set the debugging level for a particular trace.
    static void SetDebugLevel(const char* trace, ErrorPriority level);

    /// Get the current debugging level;
    static ErrorPriority GetDebugLevel() {return fErrorPriority;}

    /// Get the current debugging level for a named trace.
    static ErrorPriority GetDebugLevel(const char* trace);

    /// Set the output stream to be used for log messages.
    static void SetLogStream(std::ostream* log);

    /// Set the default logging level.
    static void SetLogLevel(LogPriority level) {fLogPriority = level;}

    /// Set the logging level for a named trace.
    static void SetLogLevel(const char* trace, LogPriority level);

    /// Get the current logging level;
    static LogPriority GetLogLevel() {return fLogPriority;}

    /// Get the current logging level;
    static LogPriority GetLogLevel(const char* trace);

    /// Return the stream associated with the error file.
    static std::ostream& GetDebugStream();
    
    /// Return the stream associated with the log file.
    static std::ostream& GetLogStream();

    /// Set the indentation level for a log message.
    static void SetIndentation(int i);

    /// Increase the indentation level.
    static void IncreaseIndentation();

    /// Decrease the indentation level.
    static void DecreaseIndentation();
    
    /// Reset the indentation to zero.
    static void ResetIndentation();

    /// [Internal method] Make an indentation for a log message.
    static std::string MakeIndent();

private: 
    static ErrorPriority fErrorPriority;
    static LogPriority fLogPriority;
    static std::ostream* fDebugStream;
    static std::ostream* fLogStream;
    static std::map<std::string,ErrorPriority> fErrorTraces;
    static std::map<std::string,LogPriority> fLogTraces;
    static int fIndentation;

    TND280Log();
};

/// INTERNAL: A macro to handle the output of an error message.  This is used
/// by the user visible macros.
#ifndef _ND280_OUTPUT_ERROR
# define _ND280_OUTPUT_ERROR(trace,outStream)                           \
    do {                                                                \
        std::ios::fmtflags save = ND::TND280Log::GetDebugStream().flags(); \
        ND::TND280Log::GetDebugStream() << trace << __FILE__            \
                                        << ":" << __LINE__ << ": "      \
                                        << outStream                    \
                                        << std::setprecision(6)         \
                                        << std::setw(0)                 \
                                        << std::setfill(' ')            \
                                        << std::endl;                   \
        ND::TND280Log::GetDebugStream().flags(save);                    \
    } while (0)
#endif

/// Set this to false if the error output code should not be included in
/// the executable.   This can be redefined in user code and depends on the
/// optimizers to not emit code a constant contitionals (that's the usual
/// behavior for a compiler). 
#ifndef ND280_ERROR_OUTPUT 
# define ND280_ERROR_OUTPUT true
#endif

#ifndef ND280Error
/// Print an error message that cannot be suppressed by changing the error
/// output level.  The use of ND280Error() can generated a lot of chatter, so
/// it should be reserved to cases where you expect the program to crash (for
/// example, the next line is a call to abort()), and prefer the ND280Severe
/// macro.  The ND280Error macro takes one \ref streamish providing the error
/// message.
# define ND280Error(outStream)                                        \
    do {                                                              \
        if (ND280_ERROR_OUTPUT) {                                     \
            _ND280_OUTPUT_ERROR("ERROR: ",outStream);                 \
        }                                                             \
    } while (0)
#else
#warning ND280Error has been redefined and unexpected behaviour may result.
#endif

#ifndef ND280NamedError
/// Print an named error message that appears at the default error output
/// level.  The use of ND280NamedError() should generally be reserved to cases
/// where you expect the program to crash, or for which data is going to be
/// lost.  This macro takes two arguments: The first argument must be a \ref
/// trace (a string).  The second argument is a \ref streamish providing the
/// error message.
# define ND280NamedError(trace,outStream)                              \
    do {                                                                \
        if (ND280_ERROR_OUTPUT) {                                       \
            if (ND::TND280Log::ErrorLevel <= ND::TND280Log::GetDebugLevel(trace)) \
                _ND280_OUTPUT_ERROR("ERROR[" trace "]: ", outStream);  \
        }                                                               \
    } while (0)
#else
#warning ND280NamedError has been redefined and unexpected behaviour may result.
#endif

#ifndef ND280Severe
/// Print an error message that appears at the ND::TND280Log::SevereLevel of
/// error output.  This macro should be reserved for error conditions
/// where event data is going to be lost, or where the event might contain
/// incorrect information.  This macro takes one \ref streamish providing the
/// error message.
# define ND280Severe(outStream)                                       \
    do {                                                              \
        if (ND280_ERROR_OUTPUT) {                                     \
            if (ND::TND280Log::SevereLevel <= ND::TND280Log::GetDebugLevel()) \
                _ND280_OUTPUT_ERROR("SEVERE: ",outStream);            \
        }                                                             \
    } while (0)
#else
#warning ND280Severe has been redefined and unexpected behaviour may result.
#endif

#ifndef ND280NamedSevere
/// Print an error message that appears at the ND::TND280Log::SevereLevel of
/// error output.  This macro should be reserved for error conditions
/// where event data is going to be lost, or where the event might contain
/// incorrect information.  This macro takes two arguments: The first argument
/// must be a \ref trace (a string).  The second argument is a \ref streamish
/// providing the error message.
# define ND280NamedSevere(trace,outStream)                              \
    do {                                                                \
        if (ND280_ERROR_OUTPUT) {                                       \
            if (ND::TND280Log::SevereLevel <= ND::TND280Log::GetDebugLevel(trace)) \
                _ND280_OUTPUT_ERROR("SEVERE[" trace "]: ", outStream);  \
        }                                                               \
    } while (0)
#else
#warning ND280NamedSevere has been redefined and unexpected behaviour may result.
#endif

#ifndef ND280Warn
/// Print an error message that appears at the ND::TND280Log::WarnLevel of
/// error output.  This macro should be used when a correctable, but
/// unexpected, problem is found with an event.  This macro takes one \ref
/// streamish providing the error message.
# define ND280Warn(outStream)                                          \
    do {                                                               \
        if (ND280_ERROR_OUTPUT) {                                      \
            if (ND::TND280Log::WarnLevel <= ND::TND280Log::GetDebugLevel())    \
                _ND280_OUTPUT_ERROR("WARNING: ",outStream);            \
        }                                                              \
    } while (0)
#else
#warning ND280Warn has been redefined and unexpected behaviour may result.
#endif

#ifndef ND280NamedWarn
/// Print an error message that appears at the ND::TND280Log::WarnLevel of
/// error output.  This macro should be used when a correctable, but
/// unexpected, problem is found with an event.  This macro takes two
/// arguments: The first argument must be a \ref trace (a string).  The second
/// argument is a \ref streamish providing the error message.
# define ND280NamedWarn(trace,outStream)                                \
    do {                                                                \
        if (ND280_ERROR_OUTPUT) {                                       \
            if (ND::TND280Log::WarnLevel <= ND::TND280Log::GetDebugLevel(trace)) \
                _ND280_OUTPUT_ERROR("WARNING[" trace "]: ", outStream); \
        }                                                               \
    } while (0)
#else
#warning ND280NamedWarn has been redefined and unexpected behaviour may result.
#endif

#ifndef ND280Debug
/// Print an debugging message that appears at the ND::TND280Log::DebugLevel of
/// error output.  This macro should be used to print output needed during
/// the debugging.  The ND280Trace() macro should be used during debugging to
/// provide traces of the code execution.  This macro takes one \ref streamish
/// providing the error message.
#define ND280Debug(outStream)                                          \
    do {                                                               \
        if (ND280_ERROR_OUTPUT) {                                      \
            if (ND::TND280Log::DebugLevel <= ND::TND280Log::GetDebugLevel())   \
                _ND280_OUTPUT_ERROR("DEBUG: ",outStream);              \
        }                                                              \
    } while (0)
#else
#warning ND280Debug has been redefined and unexpected behaviour may result.
#endif

#ifndef ND280NamedDebug
/// Print an debugging message that appears at the ND::TND280Log::DebugLevel of
/// error output.  This macro should be used to print output needed during
/// the debugging.  The ND280Trace() macro should be used during debugging to
/// provide traces of the code execution.  This macro takes two arguments: The
/// first argument must be a \ref trace (a string).  The second argument is a
/// \ref streamish providing the error message.
#define ND280NamedDebug(trace,outStream)                                \
    do {                                                                \
        if (ND280_ERROR_OUTPUT) {                                       \
            if (ND::TND280Log::DebugLevel <= ND::TND280Log::GetDebugLevel(trace)) \
                _ND280_OUTPUT_ERROR("DEBUG[" trace "]: ", outStream);   \
        }                                                               \
    } while (0)
#else
#warning ND280NamedDebug has been redefined and unexpected behaviour may result.
#endif

#ifndef ND280Trace
/// Print an debugging message that appears at the ND::TND280Log::TraceLevel of
/// error output.  This macro should be used to print short messages that
/// trace the execution of code being debugged.  This macro takes one \ref
/// streamish providing the error message.
#define ND280Trace(outStream)                                          \
    do {                                                               \
        if (ND280_ERROR_OUTPUT) {                                      \
            if (ND::TND280Log::TraceLevel <= ND::TND280Log::GetDebugLevel())   \
                _ND280_OUTPUT_ERROR("TRACE:",outStream);               \
        }                                                              \
    } while (0)
#else
#warning ND280Trace has been redefined and unexpected behaviour may result.
#endif

#ifndef ND280NamedTrace
/// Print an debugging message that appears at the ND::TND280Log::TraceLevel of
/// error output.  This macro should be used to print short messages that
/// trace the execution of code being debugged. This macro takes two
/// arguments: The first argument must be a \ref trace (a string).  The second
/// argument is a \ref streamish providing the error message.
#define ND280NamedTrace(trace,outStream)                                \
    do {                                                                \
        if (ND280_ERROR_OUTPUT) {                                       \
            if (ND::TND280Log::TraceLevel <= ND::TND280Log::GetDebugLevel(trace)) \
                _ND280_OUTPUT_ERROR("TRACE[" trace "]: ", outStream);   \
        }                                                               \
    } while (0)
#else
#warning ND280NamedTrace has been redefined and unexpected behaviour may result.
#endif

/// INTERNAL: A macro to handle the output of a log message.  This is used
/// by the user visible macros.
#ifndef _ND280_OUTPUT_LOG
#define _ND280_OUTPUT_LOG(trace,outStream)                              \
    do {                                                                \
        std::ios::fmtflags save = ND::TND280Log::GetLogStream().flags(); \
        ND::TND280Log::GetLogStream() << trace                          \
                                      << ND::TND280Log::MakeIndent()    \
                                      << outStream                      \
                                      << std::setprecision(6)           \
                                      << std::setw(0)                   \
                                      << std::setfill(' ')              \
                                      << std::endl;                     \
        ND::TND280Log::GetLogStream().flags(save);                      \
    } while (0)
#endif

/// Set this to false if the logging output code should not be included in
/// the executable.  This can be redefined in user code and depends on the
/// optimizers to not emit code a constant contitionals (that's the usual
/// behavior for a compiler).
#ifndef ND280_LOG_OUTPUT 
#  define ND280_LOG_OUTPUT true
#endif

#ifndef ND280Log
/// Print a message to the log that appears at the ND::TND280Log::LogLevel of
/// output.  This should be reserved for log messages that should appear
/// during production.  This macro takes one \ref
/// streamish providing the log message.
#define ND280Log(outStream)                                             \
    do {                                                                \
        if (ND280_LOG_OUTPUT) {                                         \
            if (ND::TND280Log::LogLevel <= ND::TND280Log::GetLogLevel()) \
                _ND280_OUTPUT_LOG("% ",outStream);                      \
        }                                                               \
    } while (0)
#else
#warning ND280Log has been redefined and unexpected logging \
behaviour may result.
#endif

#ifndef ND280NamedLog
/// Print a message to the log that appears at the ND::TND280Log::LogLevel of
/// output.  This macro takes two arguments: The first argument must be a \ref
/// trace (a string).  The second argument is a \ref streamish providing the
/// log message.
#define ND280NamedLog(trace,outStream)                                  \
    do {                                                                \
        if (ND280_LOG_OUTPUT) {                                         \
            if (ND::TND280Log::LogLevel <= ND::TND280Log::GetLogLevel(trace)) \
                _ND280_OUTPUT_LOG("% [" trace "] ",outStream);          \
        }                                                               \
    } while (0)
#else
#warning ND280NamedLog has been redefined and unexpected logging \
behaviour may result.
#endif

#ifndef ND280Info
/// Print a message to the log that appears at the ND::TND280Log::InfoLevel of
/// output.  This macro takes one \ref streamish providing the log message.
#define ND280Info(outStream)                                            \
    do {                                                                \
        if (ND280_LOG_OUTPUT) {                                         \
            if (ND::TND280Log::InfoLevel <= ND::TND280Log::GetLogLevel()) \
                _ND280_OUTPUT_LOG("%% ",outStream);                     \
        }                                                               \
    } while (0)
#else
#warning ND280Info has been redefined and unexpected logging \
behaviour may result.
#endif

#ifndef ND280NamedInfo
/// Print a message to the log that appears at the ND::TND280Log::InfoLevel of
/// output.  This macro takes two arguments: The first argument must a \ref
/// trace (a string).  The second argument is a \ref streamish providing the
/// log message.
#define ND280NamedInfo(trace,outStream)                                 \
    do {                                                                \
        if (ND280_LOG_OUTPUT) {                                         \
            if (ND::TND280Log::InfoLevel <= ND::TND280Log::GetLogLevel(trace)) \
                _ND280_OUTPUT_LOG("%% [" trace "] ",outStream);         \
        }                                                               \
    } while (0)
#else
#warning ND280NamedInfo has been redefined and unexpected logging \
behaviour may result.
#endif

#ifndef ND280Verbose
/// Print a message to the log that appears at the ND::TND280Log::VerboseLevel of
/// output.  This macro takes one \ref streamish providing the log message.
#define ND280Verbose(outStream) \
    do {                                                                \
        if (ND280_LOG_OUTPUT) {                                         \
            if (ND::TND280Log::VerboseLevel <= ND::TND280Log::GetLogLevel()) \
                _ND280_OUTPUT_LOG("%%% ",outStream);                      \
        }                                                               \
    } while (0)
#else
#warning ND280Verbose has been redefined and unexpected logging \
behaviour may result.
#endif

#ifndef ND280NamedVerbose
/// Print a message to the log that appears at the ND::TND280Log::VerboseLevel of
/// output.  This macro takes two arguments: The first argument must be a \ref
/// trace (a string).  The second argument is a \ref streamish providing the
/// log message.
#define ND280NamedVerbose(trace,outStream)                              \
    do {                                                                \
        if (ND280_LOG_OUTPUT) {                                         \
            if (ND::TND280Log::VerboseLevel <= ND::TND280Log::GetLogLevel(trace)) \
                _ND280_OUTPUT_LOG("%%% [" trace "] ",outStream);          \
        }                                                               \
    } while (0)
#else
#warning ND280NamedVerbose has been redefined and unexpected logging \
behaviour may result.
#endif

#endif
